#! /bin/zsh
clear
rm bin/jlox/*.class
javac -d bin jlox/*.java && java -cp bin jlox.Lox $@
