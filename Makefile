# 
# Makefile
# 

.PHONY: file-manager launcher

all: file-manager launcher

clean:
	make -C file-manager clean
	make -C lvgl-launcher clean

file-manager:
	make -C file-manager
	mkdir -p build
	cp file-manager/build/bin/file-manager build/file-manager

launcher:
	make -C lvgl-launcher
	mkdir -p build
	cp lvgl-launcher/build/bin/launcher build/launcher

