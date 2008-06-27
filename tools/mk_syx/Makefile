# $Id$

all: cleanall
	find *.ini -type f -exec perl mk_syx.pl {} -device_id 0 \;

cleanall:
	rm -f *.syx
