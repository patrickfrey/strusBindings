TOPDIR=../../..
include $(TOPDIR)/variables.mk

CPP_OBJS=\
	testQuery.o

CPP_INCLUDE=-I$(TOPDIR)/include -I$(INSTALL_INCDIR)
CXXFLAGS=-g -Werror -Wall -Wfatal-errors -pedantic -std=c++98
LDFLAGS=-L$(TOPDIR)/src -L$(INSTALL_LIBDIR)
LIBS=-lstrus_bindings -lstrus_database_leveldb -lstrus_storage -lstrus_queryeval -lstrus_queryproc -lstrus_analyzer -lstrus_textproc -lstrus_normalizer_snowball -lstemmer -lstrus_tokenizer_punctuation -lstrus_tokenizer_word -lstrus_segmenter_textwolf -lstrus_utils -lboost_system

all: testQuery

-include $(CPP_OBJS:.o=.d)

%.o : %.cpp GNUmakefile
	$(CXX) -c -o $@ $(CXXFLAGS) $(CPP_INCLUDE) $*.cpp
	$(CXX) -MM $(CXXFLAGS) $(CPP_INCLUDE) $*.cpp > $*.d

testQuery: $(CPP_OBJS)
	$(CXX) -o $@ $(CPP_OBJS) $(LDFLAGS) $(LIBS)

.PHONY: clean
clean:
	rm -f $(CPP_OBJS) $(CPP_OBJS:.o=.d) testQuery


