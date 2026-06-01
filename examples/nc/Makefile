all: nc.vt100.com nc.micro80.com

CLEAN += nc.com nc.lst nc.asm
nc.com: *.c *.h
	../../c8080e -OCPM -DARCH_CPM -DARCH_MICRO80_COLOR -onc.com *.c

CLEAN += nc.vt100.com nc.vt100.lst nc.vt100.asm
nc.vt100.com: *.c *.h
	../../c8080e -OCPM -DARCH_CPM -DARCH_VT100 -DTEXT_WIDTH=64 -DTEXT_HEIGHT=25 -onc.vt100.com *.c

CLEAN += nc.micro80.com nc.micro80.lst nc.micro80.asm
nc.micro80.com: *.c *.h Makefile
	../../c8080e -OCPM -DARCH_CPM -DARCH_MICRO80_COLOR -DFULL_COLOR_MODE -onc.micro80.com *.c

.PHONY: clean
clean:
	rm $(CLEAN)