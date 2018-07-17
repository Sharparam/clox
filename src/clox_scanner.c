#include <stdbool.h>
#include <ctype.h>
#include <string.h>

#include "clox_scanner.h"

typedef struct CloxScanner CloxScanner;
struct CloxScanner {
    const char *start;
    const char *current;
    int line;
} scanner;

static bool isalscore(char c) {
    return isalpha(c) || c == '_';
}

static bool isalnumscore(char c) {
    return isalnum(c) || c == '_';
}

static bool is_at_end() {
    return *scanner.current == '\0';
}

static char advance() {
    return *scanner.current++;
}

static bool match(char expected) {
    if (is_at_end())
        return false;

    if (*scanner.current != expected)
        return false;

    scanner.current++;
    return true;
}

static char peek() {
    return *scanner.current;
}

static char peek_next() {
    return is_at_end() ? '\0' : scanner.current[1];
}

static void skip_whitespace() {
    for (;;) {
        char next = peek();
        switch (next) {
            case ' ':
            case '\t':
            case '\r':
                advance();
                break;

            case '\n':
                scanner.line++;
                advance();
                break;

            case '/':
                if (peek_next() == '/') {
                    while (peek() != '\n' && !is_at_end()) {
                        advance();
                    }
                } else {
                    return;
                }

                break;

            default:
                return;
        }
    }
}

static CloxToken make_token(CloxTokenType type) {
    CloxToken token = {
        type,
        scanner.start,
        (int)(scanner.current - scanner.start),
        scanner.line
    };
    return token;
}

static CloxToken token_error(const char * const message) {
    CloxToken token = {
        TOKEN_ERROR,
        message,
        (int)strlen(message),
        scanner.line
    };
    return token;
}

static CloxTokenType check_keyword(int start, int length, char *rest, CloxTokenType type) {
    if (scanner.current - scanner.start == start + length && memcmp(scanner.start + start, rest, length) == 0) {
        return type;
    }

    return TOKEN_IDENTIFIER;
}

static CloxTokenType identifier_type() {
    switch (scanner.start[0]) {
        case 'a':
            return check_keyword(1, 2, "nd", TOKEN_AND);

        case 'c':
            return check_keyword(1, 4, "lass", TOKEN_CLASS);

        case 'e':
            return check_keyword(1, 3, "lse", TOKEN_ELSE);

        case 'f':
            if (scanner.current - scanner.start > 1) {
                switch (scanner.start[1]) {
                    case 'a':
                        return check_keyword(2, 3, "lse", TOKEN_FALSE);

                    case 'o':
                        return check_keyword(2, 1, "r", TOKEN_FOR);

                    case 'u':
                        return check_keyword(2, 1, "n", TOKEN_FUN);
                }
            }

            break;

        case 'i':
            return check_keyword(1, 1, "f", TOKEN_IF);

        case 'n':
            return check_keyword(1, 2, "il", TOKEN_NIL);

        case 'o':
            return check_keyword(1, 1, "r", TOKEN_OR);

        case 'p':
            return check_keyword(1, 4, "rint", TOKEN_PRINT);

        case 'r':
            return check_keyword(1, 5, "eturn", TOKEN_RETURN);

        case 's':
            return check_keyword(1, 4, "uper", TOKEN_SUPER);

        case 't':
            if (scanner.current - scanner.start > 1) {
                switch (scanner.start[1]) {
                    case 'h':
                        return check_keyword(2, 2, "is", TOKEN_THIS);

                    case 'r':
                        return check_keyword(2, 2, "ue", TOKEN_TRUE);
                }
            }

            break;

        case 'v':
            return check_keyword(1, 2, "ar", TOKEN_VAR);

        case 'w':
            return check_keyword(1, 4, "hile", TOKEN_WHILE);
    }

    return TOKEN_IDENTIFIER;
}

static CloxToken identifier() {
    while (isalnumscore(peek())) {
        advance();
    }

    return make_token(identifier_type());
}

static CloxToken number() {
    while (isdigit(peek())) {
        advance();
    }

    if (peek() == '.' && isdigit(peek_next())) {
        advance();

        while (isdigit(peek())) {
            advance();
        }
    }

    return make_token(TOKEN_NUMBER);
}

static CloxToken string() {
    while (peek() != '"' && !is_at_end()) {
        if (peek() == '\n') {
            scanner.line++;
        }

        advance();
    }

    if (is_at_end()) {
        return token_error("Unterminated string.");
    }

    advance();
    return make_token(TOKEN_STRING);
}

void clox_scanner_init(const char * const source) {
    scanner.start = source;
    scanner.current = source;
    scanner.line = 1;
}

CloxToken clox_scanner_scan_token() {
    skip_whitespace();

    scanner.start = scanner.current;

    if (is_at_end()) {
        return make_token(TOKEN_EOF);
    }

    char next = advance();

    if (isalscore(next)) {
        return identifier();
    }

    if (isdigit(next)) {
        return number();
    }

    switch (next) {
        case '(':
            return make_token(TOKEN_LEFT_PAREN);

        case ')':
            return make_token(TOKEN_RIGHT_PAREN);

        case '{':
            return make_token(TOKEN_LEFT_BRACE);

        case '}':
            return make_token(TOKEN_RIGHT_BRACE);

        case ';':
            return make_token(TOKEN_SEMICOLON);

        case ',':
            return make_token(TOKEN_COMMA);

        case '.':
            return make_token(TOKEN_DOT);

        case '-':
            return make_token(TOKEN_MINUS);

        case '+':
            return make_token(TOKEN_PLUS);

        case '/':
            return make_token(TOKEN_SLASH);

        case '*':
            return make_token(TOKEN_STAR);

        case '!':
            return make_token(match('=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);

        case '=':
            return make_token(match('=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL);

        case '<':
            return make_token(match('=') ? TOKEN_LESS_EQUAL : TOKEN_LESS);

        case '>':
            return make_token(match('=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);

        case '"':
            return string();
    }

    return token_error("Unexpected character.");
}
