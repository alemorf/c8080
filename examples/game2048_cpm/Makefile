COMPILER_DIR=../../
COMPILER=${COMPILER_DIR}c8080
#COMPILER=wine ${COMPILER_DIR}c8080.exe

all: game2048

game2048: game2048.c ${COMPILER_DIR}c8080 Makefile
	${COMPILER} -I${COMPILER_DIR}include -OCPM -DARCH_CPM -agame2048.asm -ogame2048.com game2048.c

clean:
	rm game2048.asm game2048.com game2048.lst

