CC = ~/coding/duo-buildroot-sdk/host-tools/gcc/riscv64-linux-musl-x86_64/bin/riscv64-unknown-linux-musl-gcc
CFLAGS ?= -O3 -g0 -mcpu=c906fdv -march=rv64imafdcv0p7xthead -mcmodel=medany -mabi=lp64d -I/usr/include

all:
	${CC} $(CFLAGS) battery.c -o battery

run: clean all
	scp -O battery root@192.168.42.1:/root/battery
	ssh root@192.168.42.1

clean:
	rm -f battery

