TOPDIR=..
include $(TOPDIR)/variables.mk

SWIG_CALL=swig2.0 -Wall -Wallkw -Werror -I. -c++ -small -outdir gen

all:
	mkdir -p gen
	$(SWIG_CALL) -php strus.i
	$(SWIG_CALL) -java strus.i
	$(SWIG_CALL) -ruby strus.i
	$(SWIG_CALL) -csharp strus.i
	$(SWIG_CALL) -perl strus.i
	$(SWIG_CALL) -python strus.i
	$(SWIG_CALL) -tcl strus.i
	$(SWIG_CALL) -ocaml strus.i

test: all

clean:
	@-rm -f gen/*
	@-rmdir gen
	@-rm -f strus_wrap.cpp
	@-rm -f strus_wrap.cxx
	@-rm -f strus_wrap.xml

install:

uninstall:


