void x_csa() {

    gROOT->LoadMacro("../classes/AliPALPIDEFSRawStreamMS.cpp+");
    gROOT->LoadMacro("../classes/BinaryPixel.cpp+");
    gROOT->LoadMacro("../classes/BinaryCluster.cpp+");
    gROOT->LoadMacro("../classes/BinaryPlane.cpp+");
    gROOT->LoadMacro("../classes/BinaryEvent.cpp+");
    gROOT->LoadMacro("../classes/helpers.cpp+");
    gROOT->LoadMacro("csa.C+");
    gROOT->LoadMacro("basic_analysis.C+");
    gROOT->LoadMacro("interesting_events.C+");

    csa("../pALPIDEfs-software/pALPIDEfs-software/Data/alfa_2015-02-20/RawHits_150220_1626.dat",
        "~/work/tmp/palpidefs.root",
        "../pALPIDEfs-software/analysis/data/mask_1.dat");
//    basic_analysis();
//    interesting_events();
//    TBrowser* lal = new TBrowser();
}
    
