#! /bin/zsh
clear
rm bin/jlox/*.class
echo 'compiling'; javac -d bin src/jlox/*.java && echo 'running\n'; java -cp bin jlox.Lox $@
