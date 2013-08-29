
include ./Rules.mak
dirs = 01_gps 02_gprs 

all: 
	for i in $(dirs) ; do  make -C $$i || exit $? ; done
clean:
	-for i in $(dirs) ; do \
		[ ! -d $$i ] || make -C $$i clean; \
	done

install:
	-for i in $(dirs) ; do \
		[ ! -d $$i ] || make -C $$i install; \
	done
