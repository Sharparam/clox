#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "clox_compiler.h"
#include "clox_config.h"
#include "clox_scanner.h"
#include "clox_chunk.h"
#include "clox_value.h"

#ifdef CLOX_DEBUG_PRINT_CODE
#include "clox_debug.h"
#endif

#define BYTES(...) ((uint8_t[]){__VA_ARGS__})
#define MAX_NUM_CONSTANTS UINT16_MAX

typedef void (*CloxParseFunc)();

typedef enum CloxPrecedence {
    PRECEDENCE_NONE,
    PRECEDENCE_ASSIGNMENT,  // =
    PRECEDENCE_OR,          // or
    PRECEDENCE_AND,         // and
    PRECEDENCE_EQUALITY,    // == !=
    PRECEDENCE_COMPARISON,  // < > <= >=
    PRECEDENCE_TERM,        // + -
    PRECEDENCE_FACTOR,      // * /
    PRECEDENCE_UNARY,       // ! - +
    PRECEDENCE_CALL,        // . () []
    PRECEDENCE_PRIMARY
} CloxPrecedence;

typedef struct CloxParseRule CloxParseRule;
struct CloxParseRule {
    CloxParseFunc prefix;
    CloxParseFunc infix;
    CloxPrecedence precedence;
};

typedef struct CloxParser CloxParser;
struct CloxParser {
    CloxToken current;
    CloxToken previous;
    bool had_error;
    bool panic_mode;
} parser;

static CloxChunk * current_chunk();

static void error_at(const CloxToken * const token, const char * const message);
static void error(const char * const message);
static void error_at_current(const char * const message);

static void advance();
static void consume(CloxTokenType type, const char * const message);
static uint16_t make_constant(CloxValue value);

static void emit_byte(uint8_t byte);
static void emit_bytes(size_t len, const uint8_t * const bytes);
static void emit_return();
static void emit_constant(CloxValue value);

static void end_compiler();

static void parse_precedence(CloxPrecedence precedence);

static const CloxParseRule * get_rule(CloxTokenType type);

static void number();
static void binary();
static void expression();
static void grouping();
static void unary();

static const CloxParseRule rules[] = {
    { grouping, NULL,    PRECEDENCE_CALL },       // TOKEN_LEFT_PAREN
    { NULL,     NULL,    PRECEDENCE_NONE },       // TOKEN_RIGHT_PAREN
    { NULL,     NULL,    PRECEDENCE_NONE },       // TOKEN_LEFT_BRACE
    { NULL,     NULL,    PRECEDENCE_NONE },       // TOKEN_RIGHT_BRACE
    { NULL,     NULL,    PRECEDENCE_NONE },       // TOKEN_COMMA
    { NULL,     NULL,    PRECEDENCE_CALL },       // TOKEN_DOT
    { unary,    binary,  PRECEDENCE_TERM },       // TOKEN_MINUS
    { NULL,     binary,  PRECEDENCE_TERM },       // TOKEN_PLUS
    { NULL,     NULL,    PRECEDENCE_NONE },       // TOKEN_SEMICOLON
    { NULL,     binary,  PRECEDENCE_FACTOR },     // TOKEN_SLASH
    { NULL,     binary,  PRECEDENCE_FACTOR },     // TOKEN_STAR
    { NULL,     NULL,    PRECEDENCE_NONE },       // TOKEN_BANG
    { NULL,     NULL,    PRECEDENCE_EQUALITY },   // TOKEN_BANG_EQUAL
    { NULL,     NULL,    PRECEDENCE_NONE },       // TOKEN_EQUAL
    { NULL,     NULL,    PRECEDENCE_EQUALITY },   // TOKEN_EQUAL_EQUAL
    { NULL,     NULL,    PRECEDENCE_COMPARISON }, // TOKEN_GREATER
    { NULL,     NULL,    PRECEDENCE_COMPARISON }, // TOKEN_GREATER_EQUAL
    { NULL,     NULL,    PRECEDENCE_COMPARISON }, // TOKEN_LESS
    { NULL,     NULL,    PRECEDENCE_COMPARISON }, // TOKEN_LESS_EQUAL
    { NULL,     NULL,    PRECEDENCE_NONE },       // TOKEN_IDENTIFIER
    { NULL,     NULL,    PRECEDENCE_NONE },       // TOKEN_STRING
    { number,   NULL,    PRECEDENCE_NONE },       // TOKEN_NUMBER
    { NULL,     NULL,    PRECEDENCE_AND },        // TOKEN_AND
    { NULL,     NULL,    PRECEDENCE_NONE },       // TOKEN_CLASS
    { NULL,     NULL,    PRECEDENCE_NONE },       // TOKEN_ELSE
    { NULL,     NULL,    PRECEDENCE_NONE },       // TOKEN_FALSE
    { NULL,     NULL,    PRECEDENCE_NONE },       // TOKEN_FUN
    { NULL,     NULL,    PRECEDENCE_NONE },       // TOKEN_FOR
    { NULL,     NULL,    PRECEDENCE_NONE },       // TOKEN_IF
    { NULL,     NULL,    PRECEDENCE_NONE },       // TOKEN_NIL
    { NULL,     NULL,    PRECEDENCE_OR },         // TOKEN_OR
    { NULL,     NULL,    PRECEDENCE_NONE },       // TOKEN_PRINT
    { NULL,     NULL,    PRECEDENCE_NONE },       // TOKEN_RETURN
    { NULL,     NULL,    PRECEDENCE_NONE },       // TOKEN_SUPER
    { NULL,     NULL,    PRECEDENCE_NONE },       // TOKEN_THIS
    { NULL,     NULL,    PRECEDENCE_NONE },       // TOKEN_TRUE
    { NULL,     NULL,    PRECEDENCE_NONE },       // TOKEN_VAR
    { NULL,     NULL,    PRECEDENCE_NONE },       // TOKEN_WHILE
    { NULL,     NULL,    PRECEDENCE_NONE },       // TOKEN_ERROR
    { NULL,     NULL,    PRECEDENCE_NONE }        // TOKEN_EOF
};

static CloxChunk *compiling_chunk;

static CloxChunk * current_chunk() {
    return compiling_chunk;
}

static void error_at(const CloxToken * const token, const char * const message) {
    if (parser.panic_mode) {
        return;
    }

    parser.panic_mode = true;
    fprintf(stderr, "[line %d] Error", token->line);

    if (token->type == TOKEN_EOF) {
        fprintf(stderr, " at end");
    } else if (token->type != TOKEN_ERROR) {
        fprintf(stderr, " at '%.*s'", token->length, token->start);
    }

    fprintf(stderr, ": %s\n", message);
    parser.had_error = true;
}

static void error(const char * const message) {
    error_at(&parser.previous, message);
}

static void error_at_current(const char * const message) {
    error_at(&parser.current, message);
}

static void advance() {
    parser.previous = parser.current;

    for (;;) {
        parser.current = clox_scanner_scan_token();
        if (parser.current.type != TOKEN_ERROR)
            break;

        error_at_current(parser.current.start);
    }
}

static void consume(CloxTokenType type, const char * const message) {
    if (parser.current.type == type) {
        advance();
        return;
    }

    error_at_current(message);
}

static uint16_t make_constant(CloxValue value) {
    int constantIndex = clox_chunk_add_constant(current_chunk(), value);

    if (constantIndex > MAX_NUM_CONSTANTS) {
        error("Too many constants in one chunk.");
        return 0;
    }

    return (uint16_t)constantIndex;
}

static void emit_byte(uint8_t byte) {
    clox_chunk_write(current_chunk(), byte, parser.previous.line);
}

static void emit_bytes(size_t len, const uint8_t * const bytes) {
    for (size_t i = 0; i < len; i++) {
        emit_byte(bytes[i]);
    }
}

static void emit_return() {
    emit_byte(OP_RETURN);
}

static void emit_constant(CloxValue value) {
    uint16_t constantIndex = make_constant(value);
    uint8_t lower = (uint8_t)(constantIndex & 0xFF);
    if (constantIndex > UINT8_MAX) {
        uint8_t upper = (uint8_t)(constantIndex >> 8);
        emit_bytes(3, BYTES(OP_CONSTANT_LONG, upper, lower));
    } else {
        emit_bytes(2, BYTES(OP_CONSTANT, lower));
    }
}

static void end_compiler() {
    emit_return();

#ifdef CLOX_DEBUG_PRINT_CODE
    if (!parser.had_error) {
        clox_chunk_disassemble(current_chunk(), "code");
    }
#endif
}

static void parse_precedence(CloxPrecedence precedence) {
    advance();
    CloxParseFunc prefixFunc = get_rule(parser.previous.type)->prefix;

    if (prefixFunc == NULL) {
        error("Expected expression.");
        return;
    }

    prefixFunc();

    while (precedence <= get_rule(parser.current.type)->precedence) {
        advance();
        CloxParseFunc infixFunc = get_rule(parser.previous.type)->infix;
        infixFunc();
    }
}

static const CloxParseRule * get_rule(CloxTokenType type) {
    return &rules[type];
}

static void number() {
    double value = strtod(parser.previous.start, NULL);
    emit_constant(value);
}

static void binary() {
    CloxTokenType opType = parser.previous.type;

    const CloxParseRule * const rule = get_rule(opType);
    parse_precedence((CloxPrecedence)(rule->precedence + 1));

    switch (opType) {
        case TOKEN_PLUS:
            emit_byte(OP_ADD);
            break;

        case TOKEN_MINUS:
            emit_byte(OP_SUBTRACT);
            break;

        case TOKEN_STAR:
            emit_byte(OP_MULTIPLY);
            break;

        case TOKEN_SLASH:
            emit_byte(OP_DIVIDE);
            break;

        default:
            return;
    }
}

static void expression() {
    parse_precedence(PRECEDENCE_ASSIGNMENT);
}

static void grouping() {
    expression();
    consume(TOKEN_RIGHT_PAREN, "Expected ')' after expression.");
}

static void unary() {
    CloxTokenType opType = parser.previous.type;

    parse_precedence(PRECEDENCE_UNARY);

    switch (opType) {
        case TOKEN_MINUS:
            emit_byte(OP_NEGATE);
            break;

        default:
            return;
    }
}

bool clox_compiler_compile(const char * const source, CloxChunk *chunk) {
    clox_scanner_init(source);

    compiling_chunk = chunk;

    parser.had_error = false;
    parser.panic_mode = false;

    advance();
    expression();
    consume(TOKEN_EOF, "Expected end of expression.");
    end_compiler();
    return !parser.had_error;
}
