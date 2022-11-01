package jlox;

class FunReturn extends RuntimeException {
    final Object value;

    FunReturn(Object value) {
        super(null, null, false, false);
        this.value = value;
    }
}