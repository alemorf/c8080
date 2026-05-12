all: test.bin

.PHONY: clean
clean:
	rm test.bin test.lst test.asm

test.bin: test.c Makefile
	../../c8080${ext} ${asm} -m test.c