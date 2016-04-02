LDFLAGS=-L../preprocessors/Profinet -lprofinet
OPTLIBS=../preprocessors/Profinet/libprofinet.a

BASH=/bin/bash

INCLUDES=-I../preprocessors/Profinet -I. -I.. -I../sfutil -I../detection-plugins

CFLAGS=-g -O2 -Wall -Wextra -Isrc -rdynamic -DNDEBUG $(OPTFLAGS) $(LDFLAGS)
LIBS=-ldl $(OPTLIBS)
PREFIX?=/usr/local

TEST_SRC=$(shell find . -name '*_test.c')
#TEST_SRC=./test_spp_profinet.c
TESTS=$(patsubst %.c,%,$(TEST_SRC))

all: rebuild comp testing

comp: $(TEST_SRC:.c=)

.c:
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $@ $(LIBS)

#all: rebuild testing
#all: tests $(TARGET)

rebuild:
	$(MAKE) -C ../preprocessors/Profinet/ all

testing:
	#$(CC) $(CFLAGS) -o $(TESTS) $(TEST_SRC) $(LIBS)
	$(BASH) ./runtests.sh



#$(TARGET): $(CC) $(CFLAGS) $(TESTS) $(TARGET)

#$(SO_TARGET): $(TARGET) $(OBJECTS)
#	$(CC) -shared -o $@ $(OBJECTS)

echo-tests:
	@echo $(TESTS)
	@echo $(TEST_SRC)

#The Unit TESTS
.PHONY: tests
tests: CFLAGS += $(TARGET)
tests: $(TESTS)
	sh ./runtests.sh

valgrind:
	VALGRIND="valgrind --log-file=/tmp/valgrind-$p.log" $(MAKE)

clean:
	rm -f *.log
	sh ./runclean.sh
