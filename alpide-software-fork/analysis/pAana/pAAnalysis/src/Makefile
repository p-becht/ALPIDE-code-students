include Makefile.am

THEALLS := libpAAnalysis.so

THETAGS := Analysis
THETAGS += ThresholdScan DigitalScan AnalogueScan Noiseocc SourceScan
THETAGS += Hitmap DACScan
THETAGS += Scan Utils

THEOBJS := $(foreach tgt,$(THETAGS),TpA$(tgt).o)
THEINCS := $(foreach tgt,$(THETAGS),TpA$(tgt).h)
THESRCS := $(foreach tgt,$(THETAGS),TpA$(tgt).cxx)

all : LIB BIN
.PHONY : all

BIN : FitThresholds.exe
.PHONY : BIN

LIB : $(THEALLS)
.PHONY : LIB

%.exe : %.C
	$(CXX) $(LDFLAGS) $(CXXFLAGS) $^ -o $@ $(LDLIBS)

lib%.so : T%Dict.o $(THEOBJS)
	$(CXX) -O3 -Wall -Werror -fPIC -shared $(CXXFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

T%Dict.o : $(THESRCS) $(THEINCS)
	rootcint -f $(@:.o=.cxx) -c $(CXXFLAGS) $(THEINCS) $(@:Dict.o=LinkDef.h)
	$(CXX) -O3 -Wall -Werror -fPIC -c $(CXXFLAGS) $(@:.o=.cxx) -o $@

%.o : %.cxx %.h
	$(CXX) -O3 -Wall -Werror -fPIC -c $(CXXFLAGS) $< -o $@

depend : .depend
.PHONY : depend

.depend : $(THESRCS) $(THEINCS)
	$(CXX) -MM $(CXXFLAGS) $^ > ./$@

clean :
	@rm -f *.exe *Dict.* *.so *.o
.PHONY : clean

dist-clean : clean
	@rm -rf *~ .depend
.PHONY : dist-clean

include .depend
