package jlox;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import jlox.Stmt.Function;

// tokens to statements
class Parser {
    private static class ParseError extends RuntimeException {
    }

    private final List<Token> tokens;
    private int current = 0; // stores idx of the next token

    Parser(List<Token> tokens) {
        this.tokens = tokens;
    }

    public List<Stmt> parse() {
        List<Stmt> statements = new ArrayList<>();
        while (!isAtEnd()) {
            statements.add(declaration());
        }

        return statements;
    }

    /*
     * Each method for parsing a grammar rule produces a syntax tree
     * for that rule and returns it to the caller. When the body of the
     * rule contains a nonterminal—a reference to another rule—we call that other
     * rule’s method.
     */
    private Expr expression() {
        return assignment();
    }

    private Stmt declaration() {
        try {
            if (findMatch(TokenType.FUN))
                return function("function");
            if (findMatch(TokenType.VAR))
                return varDeclaration();

            return statement();
        } catch (ParseError error) {
            synchronize();
            return null;
        }
    }

    // each statement gets its own method
    private Stmt statement() {
        if (findMatch(TokenType.CLASS))
            return classDeclaration();
        if (findMatch(TokenType.FOR))
            return forStatement();
        if (findMatch(TokenType.IF))
            return ifStatement();
        if (findMatch(TokenType.PRINT))
            return printStatement();
        if (findMatch(TokenType.RETURN))
            return returnStatement();
        if (findMatch(TokenType.WHILE))
            return whileStatement();
        if (findMatch(TokenType.LEFT_BRACE))
            return new Stmt.Block(block());
        return expressionStatement();
    }

    private Stmt classDeclaration() {
        Token name = consume(TokenType.IDENTIFIER, "Expect class name.");
        consume(TokenType.LEFT_BRACE, "Expect '{' before class body.");

        List<Stmt.Function> methods = new ArrayList<>();
        while (!check(TokenType.RIGHT_BRACE) && !isAtEnd()) {
            methods.add(function("method"));
        }

        consume(TokenType.RIGHT_BRACE, "Expect '}' after class body.");

        return new Stmt.Class(name, methods);
    }

    private Stmt forStatement() {
        consume(TokenType.LEFT_PAREN, "Expect '(' after 'for'.");
        // **
        Stmt initializer;
        if (findMatch(TokenType.SEMICOLON)) { // initializer omited
            initializer = null;
        } else if (findMatch(TokenType.VAR)) { // initialize new var
            initializer = varDeclaration();
        } else {
            initializer = expressionStatement();
        }

        // **
        Expr condition = null;
        if (!check(TokenType.SEMICOLON)) {
            condition = expression();
        }
        consume(TokenType.SEMICOLON, "Expect ';' after loop condition");

        // **
        Expr increment = null;
        if (!check(TokenType.RIGHT_PAREN)) {
            increment = expression();
        }
        consume(TokenType.RIGHT_PAREN, "Expect ')' after for clauses.");

        // **
        Stmt body = statement();
        if (increment != null)
            // run the body, then run the increment
            body = new Stmt.Block(
                    Arrays.asList(body, new Stmt.Expression(increment)));

        if (condition == null)
            // if there's no for condition, run infinite while
            condition = new Expr.Literal(true);
        body = new Stmt.While(condition, body);

        if (initializer != null)
            // run initializer first, then body (which could be definied differenly above)
            body = new Stmt.Block(Arrays.asList(initializer, body));

        return body;
    }

    private Stmt ifStatement() {
        consume(TokenType.LEFT_PAREN, "Expect '(' after 'if'.");
        Expr condition = expression();
        consume(TokenType.RIGHT_PAREN, "expect ')' after if condition.");

        Stmt thenBranch = statement();
        Stmt elseBranch = null;

        if (findMatch(TokenType.ELSE)) // Else is bound to nearest if. GSQL is context-based
            elseBranch = statement();
        return new Stmt.If(condition, thenBranch, elseBranch);
    }

    private Stmt printStatement() {
        Expr value = expression();
        consume(TokenType.SEMICOLON, "Expect ';' after value.");
        return new Stmt.Print(value);
    }

    private Stmt returnStatement() {
        Token keyword = previous();
        Expr value = null;
        if (!check(TokenType.SEMICOLON)) {
            value = expression();
        }

        consume(TokenType.SEMICOLON, "Expect ';' after return value.");
        return new Stmt.Return(keyword, value);
    }

    private Stmt expressionStatement() {
        Expr expr = expression();
        consume(TokenType.SEMICOLON, "Expect ';' after expression.");
        return new Stmt.Expression(expr);
    }

    private Function function(String kind) {
        Token name = consume(TokenType.IDENTIFIER, "Expect " + kind + " name.");
        consume(TokenType.LEFT_PAREN, "Expect '(' after " + kind + " name.");
        List<Token> parameters = new ArrayList<>();
        if (!check(TokenType.RIGHT_PAREN)) {
            do {
                if (parameters.size() >= 255) {
                    error(peek(), "Can't have more than 255 parameters.");
                }

                parameters.add(
                        consume(TokenType.IDENTIFIER, "Expect parameter name."));
            } while (findMatch(TokenType.COMMA));
        }
        consume(TokenType.RIGHT_PAREN, "Expect ')' after parameters.");
        consume(TokenType.LEFT_BRACE, "Expect '{' before " + kind + " body.");
        // block() assumes the left brace has alredy been consumed
        List<Stmt> body = block();
        return new Stmt.Function(name, parameters, body);
    }

    /**
     * Create an empty list and then parse statements and add them to the list until
     * we reach the end of the block, marked by the closing }. Note that the loop
     * also has an explicit check for isAtEnd(). We have to be careful to avoid
     * infinite loops, even when parsing invalid code. If the user forgets a closing
     * }, the parser needs to not get stuck.
     * 
     * @return
     */
    private List<Stmt> block() {
        List<Stmt> statements = new ArrayList<>();

        while (!check(TokenType.RIGHT_BRACE) && !isAtEnd()) {
            statements.add(declaration());
        }

        consume(TokenType.RIGHT_BRACE, "Expect '}' after block");
        return statements;
    }

    private Expr assignment() {
        Expr expr = logicalOr();
        if (findMatch(TokenType.EQUAL)) {
            Token equals = previous();
            Expr value = assignment();
            if (expr instanceof Expr.Variable) {
                Token name = ((Expr.Variable) expr).name;
                return new Expr.Assign(name, value);
            } else if (expr instanceof Expr.Get) {
                Expr.Get get = (Expr.Get) expr;
                return new Expr.Set(get.object, get.name, value);
            }
            error(equals, "Invalid assignment target.");
        } else if (findMatch(TokenType.PLUSEQUALS, TokenType.MINUSEQUALS)) {

            Token operator = previous();
            Expr right = factor();
            Token name = ((Expr.Variable) expr).name;
            Expr value = new Expr.Binary(expr, operator, right);

            if (expr instanceof Expr.Get) {
                Expr.Get get = (Expr.Get) expr;
                return new Expr.Set(get.object, get.name, value);
            } // TODO TEST THIS
            return new Expr.Assign(name, value);

            // TODO put this in error(equals, "Invalid assignment target.");
        }

        return expr;
    }

    private Expr logicalOr() {
        Expr expr = logicalAnd();
        while (findMatch(TokenType.OR)) {
            Token operator = previous();
            Expr right = logicalAnd();
            expr = new Expr.Logical(expr, operator, right);
        }
        return expr;
    }

    private Expr logicalAnd() {
        Expr expr = equality();

        while (findMatch(TokenType.AND)) {
            Token operator = previous();
            Expr right = equality();
            expr = new Expr.Logical(expr, operator, right);
        }
        return expr;
    }

    private Stmt varDeclaration() {
        Token name = consume(TokenType.IDENTIFIER, "Expect variable name.");
        Expr initializer = null;

        if (findMatch(TokenType.EQUAL)) {
            initializer = expression();
        }
        consume(TokenType.SEMICOLON, "Expect ';' after variable declaration.");
        return new Stmt.Var(name, initializer);
    }

    private Stmt whileStatement() {
        consume(TokenType.LEFT_PAREN, "Expect '(' after while'.");
        Expr condition = expression();
        consume(TokenType.RIGHT_PAREN, "Expect ')' after condition.");
        Stmt body = statement();

        return new Stmt.While(condition, body);
    }

    private Expr equality() {
        Expr expr = comparison();

        while (findMatch(TokenType.BANG_EQUAL, TokenType.EQUAL_EQUAL)) {
            Token operator = previous();
            Expr right = comparison();
            expr = new Expr.Binary(expr, operator, right);
        }
        return expr;
    }

    private Expr comparison() {
        Expr expr = term();
        while (findMatch(TokenType.GREATER, TokenType.GREATER_EQUAL, TokenType.LESS, TokenType.LESS_EQUAL)) {
            Token operator = previous();
            Expr right = term();
            expr = new Expr.Binary(expr, operator, right);
        }
        return expr;
    }

    private Expr term() {
        Expr expr = factor();

        while (findMatch(TokenType.MINUS, TokenType.PLUS)) {
            Token operator = previous();
            Expr right = factor();
            expr = new Expr.Binary(expr, operator, right);
        }

        return expr;
    }

    private Expr factor() {
        Expr expr = unary();

        while (findMatch(TokenType.SLASH, TokenType.STAR, TokenType.MODULO)) {
            Token operator = previous();
            Expr right = unary();
            expr = new Expr.Binary(expr, operator, right);
        }

        return expr;
    }

    private Expr unary() {
        if (findMatch(TokenType.BANG, TokenType.MINUS)) {
            Token operator = previous();
            Expr right = unary();
            return new Expr.Unary(operator, right);
        }

        return call();
    }

    private Expr call() {
        Expr expr = primary();

        while (true) {
            if (findMatch(TokenType.LEFT_PAREN)) {
                expr = finishCall(expr);
            } else if (findMatch(TokenType.DOT)) {
                Token name = consume(TokenType.IDENTIFIER, "Expect property name after '.'.");
                expr = new Expr.Get(expr, name);
            } else {
                break;
            }
        }
        return expr;
    }

    private Expr finishCall(Expr callee) {
        List<Expr> arguments = new ArrayList<>();
        if (!check(TokenType.RIGHT_PAREN)) {
            do {
                if (arguments.size() >= 255) {
                    error(peek(), "Can't have more than 255 args.");
                }
                arguments.add(expression());
            } while (findMatch(TokenType.COMMA));
        }

        Token paren = consume(TokenType.RIGHT_PAREN, "Expect ')' after arguments.");

        return new Expr.Call(callee, paren, arguments);
    }

    private Expr primary() {
        if (findMatch(TokenType.FALSE))
            return new Expr.Literal(false);

        if (findMatch(TokenType.TRUE))
            return new Expr.Literal(true);

        if (findMatch(TokenType.NIL))
            return new Expr.Literal(null);

        if (findMatch(TokenType.NUMBER, TokenType.STRING)) {
            return new Expr.Literal(previous().literal);
        }

        if (findMatch(TokenType.THIS))
            return new Expr.This(previous());

        if (findMatch(TokenType.IDENTIFIER))
            return new Expr.Variable(previous());

        if (findMatch(TokenType.LEFT_PAREN)) {
            Expr expr = expression();
            consume(TokenType.RIGHT_PAREN, "Expect ')' after expression.");
            return new Expr.Grouping(expr);
        }

        // if here, current is a token that can't start an expression
        throw error(peek(), "Expect expression.");
    }

    private boolean findMatch(TokenType... types) {
        for (TokenType type : types) {
            if (check(type)) {
                advance();
                return true;
            }
        }

        return false;
    }

    private Token consume(TokenType type, String message) {
        if (check(type))
            return advance();

        throw error(peek(), message);
    }

    // is the current token the same as the input type
    private boolean check(TokenType type) {
        if (isAtEnd())
            return false;
        return peek().type == type;
    }

    // consumes current token and returns it
    private Token advance() {
        if (!isAtEnd())
            current++;
        return previous();
    }

    private boolean isAtEnd() {
        return peek().type == TokenType.EOF;
    }

    private Token peek() {
        return tokens.get(current);
    }

    private Token previous() {
        return tokens.get(current - 1);
    }

    private ParseError error(Token token, String message) {
        Lox.error(token, message);
        return new ParseError();
    }

    private void synchronize() {
        advance();

        // discard tokens until a statement boundary is found
        while (!isAtEnd()) {
            if (previous().type == TokenType.SEMICOLON) // doesn't work in a ; separated for loop
                return;

            switch (peek().type) {
                case CLASS:
                case FUN:
                case VAR:
                case FOR:
                case IF:
                case WHILE:
                case PRINT:
                case RETURN:
                    return;
                default:
                    break;
            }

            advance();
        }
    }
}