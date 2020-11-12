CC=g++
INCPATH=./inc/
OUTPATH=./out/
SRCPATH=./src/
FUNCTESTSPATH=./func_tests/
UNITTESTSPATH=./unit_tests/
CFLAGS=--coverage -ggdb -I$(INCPATH) -Wall -Wpedantic -pedantic -Wextra -c
LFLAGS=--coverage
.PHONY: clean
.NOTPARALLEL: release debug

release: CFLAGS=-I$(INCPATH) -o2 -c
release: LFLAGS=-o2
release: $(OUTPATH) $(OUTPATH)release.lastbuildstate app.exe

debug: $(OUTPATH) $(OUTPATH)debug.lastbuildstate app.exe

all: app.exe

$(OUTPATH)debug.lastbuildstate:
	rm -fv *.exe
	rm -fv $(OUTPATH)*.o
	rm -fv $(OUTPATH)*.gcno
	rm -fv $(OUTPATH)*.gcda
	rm -fv $(OUTPATH)release.lastbuildstate
	touch $(OUTPATH)debug.lastbuildstate

$(OUTPATH)release.lastbuildstate:
	rm -fv *.exe
	rm -fv $(OUTPATH)*.o
	rm -fv $(OUTPATH)*.gcno
	rm -fv $(OUTPATH)*.gcda
	rm -fv $(OUTPATH)debug.lastbuildstate
	touch $(OUTPATH)release.lastbuildstate

app.exe: $(OUTPATH) $(OUTPATH)main.o $(OUTPATH)Map_API.o $(OUTPATH)Driver_Dispatcher.o $(OUTPATH)Order_Dispatcher.o
	$(CC) $(LFLAGS) -o app.exe $(OUTPATH)main.o $(OUTPATH)Map_API.o $(OUTPATH)Driver_Dispatcher.o $(OUTPATH)Order_Dispatcher.o

$(OUTPATH)Map_API.o: $(SRCPATH)Map_API.cpp
	$(CC) $(CFLAGS) -o$(OUTPATH)Map_API.o $(SRCPATH)Map_API.cpp

$(OUTPATH)Order_Dispatcher.o: $(OUTPATH)Map_API.o $(SRCPATH)Order_Dispatcher.cpp
	$(CC) $(CFLAGS) -o$(OUTPATH)Order_Dispatcher.o $(SRCPATH)Order_Dispatcher.cpp
	
$(OUTPATH)Driver_Dispatcher.o: $(OUTPATH)Map_API.o $(SRCPATH)Driver_Dispatcher.cpp
	$(CC) $(CFLAGS) -o$(OUTPATH)Driver_Dispatcher.o $(SRCPATH)Driver_Dispatcher.cpp

$(OUTPATH)main.o: $(SRCPATH)main.cpp
	$(CC) $(CFLAGS) -o$(OUTPATH)main.o $(SRCPATH)main.cpp

dirs: $(INCPATH) $(OUTPATH) $(SRCPATH)

$(OUTPATH):
	mkdir -p $(OUTPATH)

$(INCPATH):
	mkdir $(INCPATH)

$(SRCPATH):
	mkdir $(SRCPATH)

clean:
	rm -fv *.exe
	rm -fv $(OUTPATH)*.o
	rm -fv $(OUTPATH)*.gcno
	rm -fv $(OUTPATH)*.gcda
	rm -fv $(OUTPATH)*.lastbuildstate