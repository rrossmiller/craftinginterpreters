#! /bin/zsh
clear
rm bin/jlox/*.class
#echo 'compiling'
#javac -d bin src/jlox/*.java \
#	&& echo 'running\n'\
# &&	java -cp bin jlox.Lox $@

javac -d bin src/jlox/*.java && java -cp bin jlox.Lox $@
