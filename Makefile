CC=gcc
CXX=g++
LD=g++
CFLAGS=-Wall -g -fexceptions -O3
LDFLAGS=-Wl,-Map,$@.map

ifeq ($(HIDE),)
HIDE:= @
endif

BINLIST=main 
default: project3

.cpp.o:
	@ mkdir -p .depend
	@ echo CXX $@
	$(HIDE) $(CXX) $(CFLAGS) $(DEBUGFLAGS) -c -o $@ -MMD -MF $(@:%.o=.depend/%.d) $(firstword $^)

project3: $(BINLIST)

main: main.o utils.o ni.o
	@ echo LD $@
	$(HIDE) $(LD) $(LDFLAGS) -lpthread -o $@ $^
	$(HIDE) rm -f sol{1,2} 
	$(HIDE) ln -s main sol1
	$(HIDE) ln -s main sol2

clean:
	rm -f *.map *.o $(BINLIST)

distclean: clean
	rm -rf .depend
	rm -f submission.zip
	rm -rf submission

dist: project2
	mkdir -p submission
	cp *.cpp *.h README Makefile AvgDegree* submission
	zip submission.zip submission/*

-include .depend/*.d
