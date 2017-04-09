# This makefile is used as a stub for creating Makefile

all: all_build libtcl_sl.a libreadline.a libhistory.a

all_build:
	if  [ ! -f Makefile ]; then \
		../../shell/tclsh/unix/configure  --disable-load --disable-shared; \
	fi
	make -f Makefile

libtcl8.4.a:all_build

libtcl_sl.a: libtcl8.4.a
	cp libtcl8.4.a libtcl_sl.a
	cp -f libtcl_sl.a ../../lib

libreadline.a: all_build
	cp -f readline/libreadline.a ../../lib

libhistory.a: all_build
	cp -f readline/libhistory.a ../../lib

clean:
	make clean -f Makefile
	cd readline; make clean -f Makefile

install:
	make install -f Makefile
	make install -f readline/Makefile
