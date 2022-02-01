#MAKE?=mingw32-make
AR?=ar
ARFLAGS?=rcs
PATHSEP?=/
CC=gcc
BUILDROOT?=build

ifeq ($(CLANG),1)
	export CC=clang
endif

BUILDDIR?=$(BUILDROOT)$(PATHSEP)$(CC)
BUILDPATH?=$(BUILDDIR)$(PATHSEP)

INSTALL_ROOT?=$(BUILDPATH)

#DEBUGSYMBOLS=-ggdb
DEBUGSYMBOLS=-g

ifeq ($(DEBUG),1)
	export debug=$(DEBUGSYMBOLS) -Ddebug=1
	export isdebug=1
endif

ifeq ($(ANALYSIS),1)
	export analysis=-Danalysis=1
	export isanalysis=1
endif

ifeq ($(DEBUG),2)
	export debug=$(DEBUGSYMBOLS) -Ddebug=2
	export isdebug=1
endif

ifeq ($(DEBUG),3)
	export debug=$(DEBUGSYMBOLS) -Ddebug=3
	export isdebug=1
endif

ifeq ($(OUTPUT),1)
	export outimg= -Doutput=1
endif


BIT_SUFFIX=

ifeq ($(M32),1)
	CFLAGS+=-m32
	BIT_SUFFIX+=32
endif

CFLAGS+= -std=c11 -Wpedantic -pedantic-errors -Wall -Wextra $(debug)
#-ggdb
#-pg for profiling 

LIBSDIR?=-L/c/dev/lib$(BIT_SUFFIX) -L./$(BUILDDIR)
INCLUDE?=-I/c/dev/include -I/usr/include -I./src

NAME=r_font
OBJS=$(NAME).o
LIBS=$(NAME) dl_list utilsmath vec mat

TESTLIB=$(patsubst %,-l%,$(LIBS))
_TEST_SRC_FILES=test_r_font font_provider_default
TESTSRC=$(patsubst %,src/%,$(patsubst %,%.c,$(_TEST_SRC_FILES)))
TESTBIN=$(BUILDPATH)test_$(NAME).exe
LIBNAME=lib$(NAME).a
LIB=$(BUILDPATH)$(LIBNAME)

all: mkbuilddir $(LIB) $(TESTBIN)

$(OBJS):
	$(CC) $(CFLAGS) -c src/$(@F:.o=.c) -o $@ $(INCLUDE)

$(LIB): $(OBJS)
	$(AR) $(ARFLAGS) $@ $^

$(TESTBIN): $(LIB) src/font_provider_default.h
	$(CC) $(CFLAGS) $(TESTSRC) -o $@ $(LIBSDIR) $(TESTLIB) $(INCLUDE) $(debug)

.PHONY: clean mkbuilddir test

test: $(TESTBIN)
	./$(TESTBIN)

install_folder:
	mkdir -p $(INSTALL_ROOT)include
	mkdir -p $(INSTALL_ROOT)lib$(BIT_SUFFIX)

install_header: install_folder
	cp ./src$(PATHSEP)$(NAME).h $(INSTALL_ROOT)include$(PATHSEP)$(NAME).h

install: install_header
	cp $(LIB) $(INSTALL_ROOT)lib$(BIT_SUFFIX)$(PATHSEP)$(LIBNAME)

mkbuilddir:
	mkdir -p $(BUILDDIR)

clean:
	-rm -dr $(BUILDROOT)
