.DEFAULT_GOAL := digilib
CC=gcc
CXX=g++
RM=rm -f
CPPFLAGS=-I. -Wall -fPIC -Wno-unknown-pragmas -Wno-reorder
LDFLAGS=-shared -Wl,-soname,libdigilib.so.1
LDLIBS=

SUBDIRS = . Core Tools Parser BasicGates
SUBDIRSCLEAN=$(addsuffix clean,$(SUBDIRS))

SRCS:=$(wildcard *.cpp) $(wildcard */*.cpp)
SRCS:=$(filter-out dllmain.cpp, $(SRCS))
OBJS=$(subst .cpp,.o,$(SRCS))

-include $(OBJS:.o=.d)

#link
digilib: $(OBJS)
	$(CXX) $(LDFLAGS) -o libdigilib.so.1.0 $(OBJS)
	ln -fs libdigilib.so.1.0 libdigilib.so.1
	ln -fs libdigilib.so.1 libdigilib.so

%.o: %.cpp
	$(CXX) $(CPPFLAGS) -g -o $*.o -c $*.cpp
	$(CXX) $(CPPFLAGS) -MM $*.cpp > $*.d
	@mv -f $*.d $*.d.tmp
	@sed -e 's|.*:|$*.o:|' < $*.d.tmp > $*.d
	@sed -e 's/.*://' -e 's/\\$$//' < $*.d.tmp | fmt -1 | \
	  sed -e 's/^ *//' -e 's/$$/:/' >> $*.d
	@rm -f $*.d.tmp
	
clean: $(SUBDIRSCLEAN)

clean_curdir:
	$(RM) *.o *.d *~ *.so

%clean: %
	$(MAKE) -C $< -f $(PWD)/makefile clean_curdir
	