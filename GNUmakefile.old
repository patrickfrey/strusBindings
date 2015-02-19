include variables.mk

all:
	cd src; make all; cd ..
	cd swig; make all; cd ..
	cd tests; make all; cd ..

test: all
	cd tests; make test; cd ..

clean:
	cd src; make clean; cd ..
	cd swig; make clean; cd ..

install:
	cd src; make install; cd ..
	cd swig; make install; cd ..
	@cp include/strus/*.hpp $(INSTALL_INCDIR)

uninstall:
	cd src; make uninstall; cd ..
	cd swig; make uninstall; cd ..
	@cp include/strus/*.hpp $(INSTALL_INCDIR)
	@-rm -f $(INSTALL_INCDIR)/bindingObjects.hpp
	@-rmdir $(INSTALL_INCDIR)
	@-rmdir $(INSTALL_LIBDIR)

