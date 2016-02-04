CXX=clang++
CXX_FLAGS=-g3 -std=c++1y -Wno-c++11-extensions -Wno-c++11-long-long
LIBS=-lm

SOURCES = $(wildcard *.cpp)
INCLUDES := $(wildcard *.h)
OBJECTS = $(SOURCES:.cpp=.o)

DEPFILE	= .depends
DEPTOKEN	= '\# MAKEDEPENDS'
DEPFLAGS	= -Y -f $(DEPFILE) -s $(DEPTOKEN)

%.o: %.cpp
	$(CXX) $(CXX_FLAGS) -c -o $@ $< 

qcsv: $(OBJECTS)
	echo $(OBJECTS)
	$(CXX) $(CXX_FLAGS) -o $@ $^ $(LIBS)

.PHONY: clean

clean:
	rm -f qcsv *.o *~ core 

depend:
	rm -f $(DEPFILE)
	@echo $(DEPTOKEN) > $(DEPFILE)
	clang++ -MD -MF $(DEPFILE) 

 # put this file in the last line of your Makefile
 sinclude $(DEPFILE)
