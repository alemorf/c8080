all: micro80.bin

.PHONY: clean
clean:
	rm micro80.bin micro80.lst micro80.asm

micro80.bin: micro80.c Makefile
	../../c8080e -m micro80.c