all: nc.com

nc.com: *.c *.h
	../../c8080e -DARCH_CPM -OCPM -DARCH_CPM_CCP -DARCH_MICRO80_COLOR -I../../include/micro80 -onc.com *.c

.PHONY: clean
clean:
	rm nc.com nc.lst nc.asm