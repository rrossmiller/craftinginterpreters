package jlox;

import java.util.List;

public interface LoxCallable {
    public Object call(Interpreter interpreter, List<Object> args);

    public int arity();
}