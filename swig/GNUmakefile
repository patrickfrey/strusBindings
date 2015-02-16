TOPDIR=..
include $(TOPDIR)/variables.mk

SWIG_CALL=swig2.0 -Wall -Wallkw -Werror -I. -c++ -small -outdir gen

all: gen php java ruby csharp perl python tcl ocaml xml

-include php.d
-include java.d
-include ruby.d
-include csharp.d
-include perl.d
-include python.d
-include tcl.d
-include ocaml.d
-include xml.d

gen:
	mkdir -p gen

php: php.d
	$(SWIG_CALL) -php strus.i

php.d:
	$(SWIG_CALL) -MM -php strus.i > php.d

java: java.d
	$(SWIG_CALL) -java strus.i

java.d:
	$(SWIG_CALL) -MM -java strus.i > java.d

ruby: ruby.d
	$(SWIG_CALL) -ruby strus.i

ruby.d:
	$(SWIG_CALL) -MM -ruby strus.i > ruby.d

csharp: csharp.d
	$(SWIG_CALL) -csharp strus.i

csharp.d:
	$(SWIG_CALL) -MM -csharp strus.i > csharp.d

perl: perl.d
	$(SWIG_CALL) -perl strus.i

perl.d:
	$(SWIG_CALL) -MM -perl strus.i > perl.d

python: python.d
	$(SWIG_CALL) -python strus.i

python.d:
	$(SWIG_CALL) -MM -python strus.i > python.d

tcl: tcl.d
	$(SWIG_CALL) -tcl strus.i

tcl.d:
	$(SWIG_CALL) -MM -tcl strus.i > tcl.d

ocaml:
	$(SWIG_CALL) -ocaml strus.i

ocaml.d:
	$(SWIG_CALL) -MM -ocaml strus.i > ocaml.d

xml:
	$(SWIG_CALL) -xml strus.i

xml.d:
	$(SWIG_CALL) -MM -ocaml strus.i > xml.d

test: all

clean:
	@-rm -f gen/*
	@-rm -f *.d
	@-rmdir gen
	@-rm -f strus_wrap.cpp
	@-rm -f strus_wrap.cxx
	@-rm -f strus_wrap.xml

install:

uninstall:


