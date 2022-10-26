#! /bin/zsh
clear
rm jlox/*.class
javac jlox/*.java && java jlox.Lox $@
