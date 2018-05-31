.DEFAULT_GOAL := proto
CC=gcc
CXX=g++
RM=rm -f
CPPFLAGS=-O2 -std=c++11 -I. -I/usr/include/SDL2 -I../CoreUI -Wall -Wno-unknown-pragmas -Wno-reorder
LDFLAGS=-Wl,-rpath,'$$ORIGIN' -L../CoreUI
LDLIBS=-lcoreui -lSDL2 -lSDL2_ttf -lSDL2_image

SRCS:=main.cpp
OBJS=$(subst .cpp,.o,$(SRCS))

-include $(OBJS:.o=.d)

#link
proto: $(OBJS)
	$(CXX) $(OBJS) $(LDFLAGS) $(LDLIBS) -o proto
	cp ../CoreUI/libcoreui.so.1 .

%.o: %.cpp
	$(CXX) $(CPPFLAGS) -g -o $*.o -c $*.cpp
	$(CXX) $(CPPFLAGS) -MM $*.cpp > $*.d
	@mv -f $*.d $*.d.tmp
	@sed -e 's|.*:|$*.o:|' < $*.d.tmp > $*.d
	@sed -e 's/.*://' -e 's/\\$$//' < $*.d.tmp | fmt -1 | \
	  sed -e 's/^ *//' -e 's/$$/:/' >> $*.d
	@rm -f $*.d.tmp
	
clean: 
	$(RM) *.o *.d *~ libcoreui.*
