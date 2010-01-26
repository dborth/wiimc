# Linux makefile for OpenJPEG

VER_MAJOR = 2
VER_MINOR = 1.3.0

SRCS = ./libopenjpeg/bio.c ./libopenjpeg/cio.c ./libopenjpeg/dwt.c ./libopenjpeg/event.c ./libopenjpeg/image.c ./libopenjpeg/j2k.c ./libopenjpeg/j2k_lib.c ./libopenjpeg/jp2.c ./libopenjpeg/jpt.c ./libopenjpeg/mct.c ./libopenjpeg/mqc.c ./libopenjpeg/openjpeg.c ./libopenjpeg/pi.c ./libopenjpeg/raw.c ./libopenjpeg/t1.c ./libopenjpeg/t2.c ./libopenjpeg/tcd.c ./libopenjpeg/tgt.c
INCLS = ./libopenjpeg/bio.h ./libopenjpeg/cio.h ./libopenjpeg/dwt.h ./libopenjpeg/event.h ./libopenjpeg/fix.h ./libopenjpeg/image.h ./libopenjpeg/int.h ./libopenjpeg/j2k.h ./libopenjpeg/j2k_lib.h ./libopenjpeg/jp2.h ./libopenjpeg/jpt.h ./libopenjpeg/mct.h ./libopenjpeg/mqc.h ./libopenjpeg/openjpeg.h ./libopenjpeg/pi.h ./libopenjpeg/raw.h ./libopenjpeg/t1.h ./libopenjpeg/t2.h ./libopenjpeg/tcd.h ./libopenjpeg/tgt.h ./libopenjpeg/opj_malloc.h ./libopenjpeg/opj_includes.h 
INCLUDE = -Ilibopenjpeg

# General configuration variables:
CC = ${DEVKITPPC}/bin/powerpc-eabi-gcc
AR = ${DEVKITPPC}/bin/powerpc-eabi-ar

INSTALL_LIBDIR = $(DEVKITPRO)/libogc/lib/wii
INSTALL_INCLUDE = $(DEVKITPRO)/libogc/include

# Converts cr/lf to just lf
DOS2UNIX = dos2unix

COMPILERFLAGS = -mcpu=750 -Wall -O3 -ffast-math -std=c99 -fPIC
LIBRARIES = -lstdc++

MODULES = $(SRCS:.c=.o)
CFLAGS = $(COMPILERFLAGS) $(INCLUDE)

TARGET  = openjpeg
STATICLIB = lib$(TARGET).a
SHAREDLIB = lib$(TARGET)-$(VER_MAJOR).$(VER_MINOR).so
LIBNAME = lib$(TARGET).so.$(VER_MAJOR)



default: all

all: OpenJPEG

dist: OpenJPEG
	install -d dist
	install -m 644 $(STATICLIB) dist
	install -m 755 $(SHAREDLIB) dist
	ln -sf $(SHAREDLIB) dist/$(LIBNAME)
	install libopenjpeg/openjpeg.h dist

dos2unix:
	@$(DOS2UNIX) $(SRCS) $(INCLS)

OpenJPEG: $(STATICLIB)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

$(STATICLIB): $(MODULES)
	$(AR) -r $@ $(MODULES)

install: OpenJPEG
	install -d '$(DESTDIR)$(INSTALL_LIBDIR)' '$(DESTDIR)$(INSTALL_INCLUDE)'
	install $(STATICLIB) '$(DESTDIR)$(INSTALL_LIBDIR)'
	${DEVKITPPC}/bin/powerpc-eabi-ranlib '$(DESTDIR)$(INSTALL_LIBDIR)/$(STATICLIB)'
	install libopenjpeg/openjpeg.h '$(DESTDIR)$(INSTALL_INCLUDE)'

clean:
	rm -rf core dist/ u2dtmp* $(MODULES) $(STATICLIB) $(SHAREDLIB) $(LIBNAME)
