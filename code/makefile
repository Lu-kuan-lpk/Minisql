SRC = ${wildcard  *.cpp}

minisql: clean y.tab.h
	g++ -g lex.yy.c y.tab.c ${SRC} -o minisql -std=c++11

clean:
	del /f *.o minisql.output
