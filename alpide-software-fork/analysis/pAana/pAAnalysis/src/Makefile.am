SHELL = /bin/sh

SUBDIRS  := src
THECLEAN := $(SUBDIRS:%=clean-%)
THEDISTC := $(SUBDIRS:%=distclean-%)

CXX      := `root-config --cxx`
CXXFLAGS := `root-config --cflags`

LD      := `root-config --ld`
LDFLAGS := `root-config --ldflags`
LDLIBS  := `root-config --libs`

.NOEXPORT:
