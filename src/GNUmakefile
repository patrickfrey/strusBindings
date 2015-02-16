TOPDIR=..
include $(TOPDIR)/variables.mk

CPP_OBJS=\
	bindingObjects.o

CPP_INCLUDE=-I$(TOPDIR)/include -I. -I$(INSTALL_INCDIR)
CXXFLAGS=-g -Wfatal-errors -Werror -Wall -pedantic $(OPTIMIZE) -std=c++98
LDFLAGS=-L$(INSTALL_LIBDIR)
LIBS=-lstrus_database_leveldb -lstrus_storage -lstrus_queryeval -lstrus_queryproc -lstrus_analyzer -lstrus_textproc -lstrus_tokenizer_word -lstrus_tokenizer_punctuation -lstrus_normalizer_snowball -lstemmer -lstrus_utils -lboost_system

all: libstrus_bindings.so

-include $(CPP_OBJS:.o=.d)

%.o : %.cpp GNUmakefile
	$(CXX) -fPIC $(VISIBILITY) -c -o $@ $(CXXFLAGS) $(CPP_INCLUDE) $*.cpp
	$(CXX) -MM $(CXXFLAGS) $(CPP_INCLUDE) $*.cpp > $*.d

libstrus_bindings.so: $(CPP_OBJS)
	$(CXX)  -shared -o $@ $(CPP_OBJS) $(LDFLAGS) $(LIBS)

.PHONY: clean
clean:
	rm -f $(CPP_OBJS) $(CPP_OBJS:.o=.d) libstrus_bindings.so


.PHONY: install
install:
	@mkdir -p $(INSTALL_LIBDIR)
	@cp libstrus_bindings.so $(INSTALL_LIBDIR)

.PHONY: uninstall
uninstall:
	@rm $(INSTALL_LIBDIR)/libstrus_bindings.so


