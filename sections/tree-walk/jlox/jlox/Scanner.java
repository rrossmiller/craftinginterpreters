package jlox;

import java.util.ArrayList;
import java.util.List;
import jlox.TokenType;

public class Scanner {
    private final String src;
    private final List<Token> tokens = new ArrayList<>();
    private int start = 0;
    private int current = 0;
    private int line = 1;

    Scanner(String src) {
        this.src = src;
    }

    List<Token> scanTokens() {
        while (!isAtEnd()) {
            start = current;
            scanToken();
        }

        tokens.add(new Token(TokenType.EOF, "", null, line));
        return tokens;
    }

    void scanToken() {

    }

    boolean isAtEnd() {
        return false;
    }
}
