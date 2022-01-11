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

LIBDIR?=/c/dev/lib$(BIT_SUFFIX)
LIBSDIR?=-L$(LIBDIR)
INCLUDE?=-I/c/dev/include/freetype2 -I/c/dev/include -I/usr/include -I/usr/include/freetype2 -I./src

INCLUDEDIR=$(INCLUDE)

LIBSDIR+=-L./$(BUILDDIR)

_LIB_SRC_FILES=r_font
LIBSRC=$(patsubst %,src/%,$(patsubst %,%.c,$(_LIB_SRC_FILES)))
LIBHEADER=$(patsubst %,src/%,$(patsubst %,%.h,$(_LIB_SRC_FILES)))
OBJS=r_font.o
LIB_NAME=r_font
LIBS= $(LIB_NAME) freetype dl_list utilsmath vec mat

TESTLIB=$(patsubst %,-l%,$(LIBS))
_TEST_SRC_FILES=test_r_font default_provider
TESTSRC=$(patsubst %,src/%,$(patsubst %,%.c,$(_TEST_SRC_FILES)))
TESTBIN=test_r_font.exe

LIB=lib$(LIB_NAME).a

all: mkbuilddir $(BUILDPATH)$(LIB) $(BUILDPATH)$(TESTBIN)

$(BUILDPATH)$(OBJS):
	$(CC) $(CFLAGS) -c $(LIBSRC) $(INCLUDEDIR) -o $(BUILDPATH)$(OBJS)

$(BUILDPATH)$(LIB): $(BUILDPATH)$(OBJS)
	$(AR) $(ARFLAGS) $(BUILDPATH)$(LIB) $(BUILDPATH)$(OBJS)

.PHONY: clean mkbuilddir test

test: $(BUILDPATH)$(TESTBIN)
	./$(BUILDPATH)$(TESTBIN)

install_folder:
	mkdir -p $(INSTALL_ROOT)include
	mkdir -p $(INSTALL_ROOT)lib$(BIT_SUFFIX)

install_header: install_folder
	cp ./src/r_font.h $(INSTALL_ROOT)include/r_font.h

install: install_header
	cp $(BUILDPATH)$(LIB) $(INSTALL_ROOT)lib$(BIT_SUFFIX)/$(LIB)

mkbuilddir:
	mkdir -p $(BUILDDIR)

clean:
	-rm -dr $(BUILDROOT)

$(BUILDPATH)$(TESTBIN): $(TESTSRC) $(LIBSRC) $(LIBHEADER)
	$(CC) $(CFLAGS) $(TESTSRC) $(LIBSDIR) $(TESTLIB) $(INCLUDE) $(debug) -o $(BUILDPATH)$(TESTBIN)
