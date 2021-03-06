#!smake
#
#

MACHINE= $(shell uname -s)

ifeq ($(MACHINE),Darwin)
	OPENGL_INC= -FOpenGL -I/usr/X11R6/include
	OPENGL_LIB= -framework OpenGL -framework GLUT -framework Cocoa
else
	OPENGL_INC= -I/usr/include
	OPENGL_LIB= -I/usr/lib64 
	LLDLIBS = -L/usr/lib64 -lglut  -lGLU -lGL -lXmu -lXext -lX11 -lm
endif

CXX=g++
COMPILER_FLAGS= -g

INCLUDE= $(OPENGL_INC)
LIBS= $(OPENGL_LIB)

TARGETS = interface

default : $(TARGETS)

SRCS = interface.cpp

OBJS = $(SRCS:.cpp=.o)

%.o: %.cpp
	$(CXX) -c -o $@ $(COMPILER_FLAGS)  $< $(INCLUDE)

interface: interface.o
	$(CXX) $(COMPILER_FLAGS) $(LLDLIBS) interface.o  -o interface

clean:
	rm -f $(OBJS) $(TARGETS) *~