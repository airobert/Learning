
# CPP = g++

# CFLAGS = -c

# OFLAGS = -o

# # INCLUDE = 
# SRC = main.cpp

# OBJ = main.o

# mainobj:
# 	$(CPP)$  $(CFLAGS)$  $(SRC)$

# main: mainobj
# 	$(CPP)$  $(OBJ)$  $(OFLAGS)$  main	

# clean: 
# 	rm *.o 
# 	rm main


CXX = g++
CXXFLAGS = -O3 -W -Wall -I..
LFLAGS =
SRCS = main.cc  
OBJS = $(SRCS:.cc=.o)
SMODELSSRCS = smodels.cc stack.cc dcl.cc atomrule.cc read.cc \
queue.cc timer.cc list.cc improve.cc program.cc api.cc stable.cc \
tree.cc denant.cc restart.cc
SMODELSOBJS = $(SMODELSSRCS:%.cc=../%.o)

main: $(OBJS)
	$(CXX) $(LFLAGS) -o main main.o $(SMODELSOBJS)

%.o: %.cc
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm main main.o