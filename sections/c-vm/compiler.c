#include "compiler.h"

#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "scanner.h"

typedef struct {
    Token current;
    Token previous;
    bool hadError;
    bool panicMode;
} Parser;

Parser parser;
Chunk *compilingChunk;
static Chunk *currentChunk() {
    return compilingChunk;
}

static void errorAt(Token *token, const char *message) {
    if (parser.panicMode) {
        return;
    }
    parser.panicMode = true;
    fprintf(stderr, "[line %d] Error", token->line);

    if (token->type == TOKEN_EOF) {
        fprintf(stderr, "[line %d] Error", token->line);
    } else if (token->type == TOKEN_ERROR) {
        //
    } else {
        fprintf(stderr, " at '%.*s'", token->length, token->start);
    }
    fprintf(stderr, ": %s\n", message);
    parser.hadError = true;
}

static void errorAtCurrent(const char *message) {
    errorAt(&parser.current, message);
}

static void error(const char *message) {
    errorAt(&parser.previous, message);
}

static void advance() {
    parser.previous = parser.current;

    for (;;) {
        parser.current = scanToken();
        if (parser.current.type != TOKEN_ERROR)
            break;

        errorAtCurrent(parser.current.start);
    }
}

static void consume(TokenType type, const char *message) {
    if (parser.current.type == type) {
        advance();
        return;
    }
    errorAtCurrent(message);
}

static void emitByte(uint8_t byte) {
    writeChunk(currentChunk(), byte, parser.previous.line);
}
static void emitBytes(uint8_t byte1, uint8_t byte2) {
    emitByte(byte1);
    emitByte(byte2);
}

static void emitReturn() {
    emitByte(OP_RETURN);
}
static void endCompiler() {
    emitReturn();
}
static void expression() {
    // What goes here?
}

const char *tt_toStr(TokenType t) {
    switch (t) {
        // Single-character tokens.
        case TOKEN_LEFT_PAREN:
            return "TOKEN_LEFT_PAREN";
        case TOKEN_RIGHT_PAREN:
            return "TOKEN_RIGHT_PAREN";
        case TOKEN_LEFT_BRACE:
            return "TOKEN_LEFT_BRACE";
        case TOKEN_RIGHT_BRACE:
            return "TOKEN_RIGHT_BRACE";
        case TOKEN_COMMA:
            return "TOKEN_COMMA";
        case TOKEN_DOT:
            return "TOKEN_DOT";
        case TOKEN_MINUS:
            return "TOKEN_MINUS";
        case TOKEN_PLUS:
            return "TOKEN_PLUS";
        case TOKEN_SEMICOLON:
            return "TOKEN_SEMICOLON";
        case TOKEN_SLASH:
            return "TOKEN_SLASH";
        case TOKEN_STAR:
            return "TOKEN_STAR";
        // One or two character tokens.
        case TOKEN_BANG:
            return "TOKEN_BANG";
        case TOKEN_BANG_EQUAL:
            return "TOKEN_BANG_EQUAL";
        case TOKEN_EQUAL:
            return "TOKEN_EQUAL";
        case TOKEN_EQUAL_EQUAL:
            return "TOKEN_EQUAL_EQUAL";
        case TOKEN_GREATER:
            return "TOKEN_GREATER";
        case TOKEN_GREATER_EQUAL:
            return "TOKEN_GREATER_EQUAL";
        case TOKEN_LESS:
            return "TOKEN_LESS";
        case TOKEN_LESS_EQUAL:
            return "TOKEN_LESS_EQUAL";
        // Literals.
        case TOKEN_IDENTIFIER:
            return "TOKEN_IDENTIFIER";
        case TOKEN_STRING:
            return "TOKEN_STRING";
        case TOKEN_NUMBER:
            return "TOKEN_NUMBER";
        // Keywords.
        case TOKEN_AND:
            return "TOKEN_AND";
        case TOKEN_CLASS:
            return "TOKEN_CLASS";
        case TOKEN_ELSE:
            return "TOKEN_ELSE";
        case TOKEN_FALSE:
            return "TOKEN_FALSE";
        case TOKEN_FOR:
            return "TOKEN_FOR";
        case TOKEN_FUN:
            return "TOKEN_FUN";
        case TOKEN_IF:
            return "TOKEN_IF";
        case TOKEN_NIL:
            return "TOKEN_NIL";
        case TOKEN_OR:
            return "TOKEN_OR";
        case TOKEN_PRINT:
            return "TOKEN_PRINT";
        case TOKEN_RETURN:
            return "TOKEN_RETURN";
        case TOKEN_SUPER:
            return "TOKEN_SUPER";
        case TOKEN_THIS:
            return "TOKEN_THIS";
        case TOKEN_TRUE:
            return "TOKEN_TRUE";
        case TOKEN_VAR:
            return "TOKEN_VAR";
        case TOKEN_WHILE:
            return "TOKEN_WHILE";
        case TOKEN_ERROR:
            return "TOKEN_ERROR";
        case TOKEN_EOF:
            return "TOKEN_EOF";
    }
}

bool compile(const char *source, Chunk *chunk) {
    initScanner(source);

    //
    // int line = -1;
    // for (;;) {
    //     Token token = scanToken();
    //     if (token.line != line) {
    //         printf("%4d ", token.line);
    //         line = token.line;
    //     } else {
    //         printf("   | ");
    //     }
    //     printf("%2d '%.*s'\n", token.type, token.length, token.start);
    //
    //     if (token.type == TOKEN_EOF)
    //         return false;
    // }
    //
    compilingChunk = chunk;

    parser.hadError = false;
    parser.panicMode = false;

    advance();
    expression();
    consume(TOKEN_EOF, "Expect end of expression.");
    endCompiler();

    return !parser.hadError;
}
