#! /bin/zsh
clear
rm gbin/tool/*.class
javac -d gbin src/tool/*.java && java -cp gbin tool.GenerateAst $@
