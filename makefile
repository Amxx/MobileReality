CC =			g++
CFLAGS =	-g -Wall -O3 -std=c++11

LFLAGS =	-lglut -lGL -lGLU -lm
LFLAGS +=	-lopencv_core -lopencv_calib3d -lopencv_highgui -lopencv_imgproc -lopencv_features2d
LFLAGS +=	-lzbar

HEA =			$(shell find src/ -name *.hh -o -name *.hpp)
SRC =			$(shell find src/ -name *.cc)
OBJ =			$(patsubst src/%.cc, objs/%.o, $(SRC))
EXE =			Reality

.PHONY: all mkdir build clean clear

all : mkdir build

mkdir :
	mkdir -p objs

build : $(OBJ)
	$(CC) $(LFLAGS) -o objs/$(EXE) $^
	ln -s -f objs/$(EXE) .

$(OBJ) : objs/%.o : src/%.cc $(HEA) makefile
	$(CC) $(CFLAGS) -c $< -o $@

clean :
	rm -rf $(OBJ)
clear : clean
	rm -rf objs/$(EXE) $(EXE)
	
