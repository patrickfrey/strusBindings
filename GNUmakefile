include variables.mk

all:
	cd src; make all; cd ..
	cd tests; make all; cd ..

test: all
	cd tests; make test; cd ..

clean:
	cd src; make clean; cd ..
	@-rm -f php_strus.h
	@-rm -f strus.php
	@-rm -f strus_wrap.c
	@-rm -f strus_wrap.cpp

install:
	cd src; make install; cd ..
	@cp include/strus/*.hpp $(INSTALL_INCDIR)

uninstall:
	cd src; make uninstall; cd ..
	@cp include/strus/*.hpp $(INSTALL_INCDIR)
	@-rm -f $(INSTALL_INCDIR)/bindingObjects.hpp
	@-rmdir $(INSTALL_INCDIR)
	@-rmdir $(INSTALL_LIBDIR)

