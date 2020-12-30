TARGET = parser
OBJECT = parser.tab.c parser.tab.o lex.yy.c alloc.o functions.o semanticAnalysis.o symbolTable.o codeGen.o
OUTPUT = parser.output parser.tab.h
CC = gcc -g
LEX = flex
YACC = bison -v
YACCFLAG = -d
# LIBS = -lfl	 	# under unix
LIBS = -ll 			# under mac

parser: parser.tab.o alloc.o functions.o symbolTable.o semanticAnalysis.o codeGen.o register.o
	$(CC) -o $(TARGET) parser.tab.o alloc.o functions.o symbolTable.o semanticAnalysis.o codeGen.o register.o $(LIBS)

parser.tab.o: parser.tab.c lex.yy.c alloc.o functions.c symbolTable.o semanticAnalysis.o codeGen.o
	$(CC) -c parser.tab.c
    
semanticAnalysis.o: semanticAnalysis.c symbolTable.o
	$(CC) -c semanticAnalysis.c

codeGen.o: codeGen.c symbolTable.o register.o
	$(CC) -c codeGen.c

register.o: register.c
	$(CC) -c register.c

symbolTable.o: symbolTable.c
	$(CC) -c symbolTable.c

lex.yy.c: lexer3.l
	$(LEX) lexer3.l

parser.tab.c: parser.y 
	$(YACC) $(YACCFLAG) parser.y

alloc.o: alloc.c
	$(CC) -c alloc.c
	
functions.o: functions.c
	$(CC) -c functions.c

clean:
	rm -f $(TARGET) $(OBJECT) $(OUTPUT)

