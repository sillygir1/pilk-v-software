CC = ~/coding/duo-buildroot-sdk/host-tools/gcc/riscv64-linux-musl-x86_64/bin/riscv64-unknown-linux-musl-gcc
CFLAGS ?= -O3 -g0 -mcpu=c906fdv -march=rv64imafdcv0p7xthead -mcmodel=medany -mabi=lp64d -I/usr/include

main:
	${CC} $(CFLAGS) encoder.c -o encoder

test:
	${CC} $(CFLAGS) -DPLUGIN test.c encoder.c -o test

main-run: clean main
	scp -O encoder root@192.168.42.1:/root/encoder
	ssh root@192.168.42.1

test-run: clean main test
	scp -O encoder root@192.168.42.1:/root/encoder
	scp -O test root@192.168.42.1:/root/test
	ssh root@192.168.42.1

clean:
	rm -f encoder
	rm -f test

