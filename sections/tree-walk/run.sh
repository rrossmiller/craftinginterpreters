#! /bin/zsh
clear
rm bin/jlox/*.class
javac -d bin src/jlox/*.java && java -cp bin jlox.Lox $@
