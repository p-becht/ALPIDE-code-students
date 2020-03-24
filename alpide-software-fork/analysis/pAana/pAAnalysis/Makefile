include Makefile.in

SUBDIRS  := src
THECLEAN := $(SUBDIRS:%=clean-%)
THEDISTC := $(SUBDIRS:%=distclean-%)

all : $(SUBDIRS)
.PHONY : all

install : all
	@install -d $(THEINSTD)/bin
	@install -d $(THEINSTD)/lib
	@install -d $(THEINSTD)/include
	@install -c -m 755 src/*.exe $(THEINSTD)/bin
	@install -c -m 755 src/*.so  $(THEINSTD)/lib
	@install -c -m 644 src/*.h   $(THEINSTD)/include
	@rm -f $(THEINSTD)/include/*Dict.h
	@rm -f $(THEINSTD)/include/*LinkDef.h
.PHONY : install

uninstall :
	@rm -f $(THEINSTD)/bin/*
	@rm -f $(THEINSTD)/lib/*
	@rm -f $(THEINSTD)/include/*
.PHONY : uninstall

$(SUBDIRS) :
	@$(MAKE) -C $@ all
.PHONY : $(SUBDIRS)

clean : $(THECLEAN)
	@rm -rf include lib bin
.PHONY : clean

dist-clean : clean $(THEDISTC)
.PHONY : dist-clean

$(THECLEAN) :
	@$(MAKE) -C $(@:clean-%=%) clean
.PHONY : $(THECLEAN)

$(THEDISTC) :
	@rm -f Makefile.in
	@$(MAKE) -C $(@:distclean-%=%) dist-clean
.PHONY : $(THEDISTC)
