CPPFLAGS=-g  -Wall 
CC=g++

USB=-lusb-1.0 
LAC=lac.o firgelli.o


SCRIPTS=setall 
EXECS=lac
all: $(EXECS) 

firgelli.o: firgelli.C firgelli.h
lac.o: lac.C firgelli.h

lac:	$(LAC)  
	$(CC) $(LAC)  -o lac -lm $(USB) 

tar: 
	tar cf firgelli.tar *.C *.h Makefile README $(SCRIPTS) *.rules 

clean:
	rm *.o $(EXECS) foo* *.tmp

