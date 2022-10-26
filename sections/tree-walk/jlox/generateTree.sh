#! /bin/zsh
clear
rm gbin/tool/*.class
javac -d gbin tool/*.java && java -cp gbin tool.GenerateAst $@
