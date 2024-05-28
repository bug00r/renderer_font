_CFLAGS:=$(CFLAGS)
CFLAGS:=$(_CFLAGS)
_LDFLAGS:=$(LDFLAGS)
LDFLAGS:=$(_LDFLAGS)

ARFLAGS?=rcs
PATHSEP?=/
BUILDROOT?=build

BUILDDIR?=$(BUILDROOT)$(PATHSEP)$(CC)
BUILDPATH?=$(BUILDDIR)$(PATHSEP)

ifndef PREFIX
	INSTALL_ROOT=$(BUILDPATH)
else
	INSTALL_ROOT=$(PREFIX)$(PATHSEP)
	ifeq ($(INSTALL_ROOT),/)
	INSTALL_ROOT=$(BUILDPATH)
	endif
endif

ifdef DEBUG
	CFLAGS+=-ggdb
	ifeq ($(DEBUG),)
	CFLAGS+=-Ddebug=1
	else 
	CFLAGS+=-Ddebug=$(DEBUG)
	endif
endif

ifeq ($(M32),1)
	CFLAGS+=-m32
	BIT_SUFFIX+=32
endif

override CFLAGS+=-std=c11 -Wpedantic -pedantic-errors -Wall -Wextra
#-ggdb
#-pg for profiling 

override LDFLAGS+=-L./$(BUILDDIR) -L/c/dev/lib$(BIT_SUFFIX) 
override CFLAGS+=-I./src -I/c/dev/include -I/usr/include

NAME=r_font
OBJS=$(BUILDPATH)$(NAME).o
LIBS=$(NAME) dl_list utilsmath vec mat

override LDFLAGS+=$(patsubst %,-l%,$(LIBS))

_TEST_SRC_FILES=test_r_font font_provider_default
TESTSRC=$(patsubst %,src/%,$(patsubst %,%.c,$(_TEST_SRC_FILES)))
TESTBIN=$(BUILDPATH)test_$(NAME).exe
LIBNAME=lib$(NAME).a
LIB=$(BUILDPATH)$(LIBNAME)

all: mkbuilddir $(LIB)

$(OBJS):
	$(CC) $(CFLAGS) -c src/$(@F:.o=.c) -o $@

$(LIB): $(OBJS)
	$(AR) $(ARFLAGS) $@ $^
 
$(TESTBIN): $(LIB) $(TESTSRC) src/font_provider_default.h
	$(CC) $(CFLAGS) $(TESTSRC) -o $@ $(LDFLAGS)

.PHONY: clean mkbuilddir test

test: mkbuilddir $(TESTBIN)
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
