TOPDIR=../..
include $(TOPDIR)/variables.mk
LD_LIBRARY_PATH=$(TOPDIR)/src:$(INSTALL_LIBDIR)

all:
	cd src; make all; cd ..

test: all dataclean
	mkdir -p data
	LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) src/testQuery

clean: dataclean
	cd src; make clean; cd ..

dataclean:
	@rm -Rf data/test/

