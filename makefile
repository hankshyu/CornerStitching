CXX = g++
FLAGS = -std=c++17
CFLAGS = -c
DEBUGFLAGS = -g

SRCPATH = ./src
BINPATH = ./bin
OBJPATH = ./obj
BOOSTPATH = ./boost_1_84_0/ # ! Remember to specify the path

all: csrun
debug: csrun_debug

# LINKFLAGS = -pedantic -Wall -fomit-frame-pointer -funroll-all-loops -O3
LINKFLAGS = -O3

_OBJS =	main.o \
		cord.o line.o rectangle.o tile.o globalPhaseAdapter.o tessera.o cornerStitching.o cSException.o

OBJS = $(patsubst %,$(OBJPATH)/%,$(_OBJS))
DBG_OBJS = $(patsubst %.o, $(OBJPATH)/%_dbg.o, $(_OBJS))

csrun: $(OBJS)
	$(CXX) $(FLAGS) -I $(BOOSTPATH) $(LINKFLAGS) $^ -o $(BINPATH)/$@

$(OBJPATH)/main.o: $(SRCPATH)/main.cpp 
	$(CXX) $(FLAGS) -I $(BOOSTPATH) $(CFLAGS) $(LINKFLAGS) -DCOMPILETIME="\"`date`\"" $^ -o $@

$(OBJPATH)/%.o: $(SRCPATH)/%.cpp $(SRCPATH)/%.h
	$(CXX) $(FLAGS) -I $(BOOSTPATH) $(CFLAGS) $(LINKFLAGS) $< -o $@



csrun_debug: $(SRCPATH)/units.h $(SRCPATH)/colours.h $(DBG_OBJS)
	$(CXX) $(DEBUGFLAGS) -I $(BOOSTPATH) $(LINKFLAGS) $^ -o $(BINPATH)/$@

main_db.o: $(SRCPATH)/main.cpp 
	$(CXX) $(DEBUGFLAGS) -I $(BOOSTPATH) $(CFLAGS) -DCOMPILETIME="\"`date`\"" $^ -o $@

%_db.o: $(SRCPATH)/%.cpp $(SRCPATH)/%.h
	$(CXX) $(DEBUGFLAGS) -I $(BOOSTPATH) $(CFLAGS) $< -o $@

.PHONY: clean
clean:
	rm -rf *.gch *.out $(OBJPATH)/* $(BINPATH)/* 

