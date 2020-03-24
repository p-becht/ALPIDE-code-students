void compile_classes() {
    gROOT->LoadMacro("AliPALPIDEFSRawStreamMS.cpp+");
    gROOT->LoadMacro("BinaryPixel.cpp+");
    gROOT->LoadMacro("BinaryCluster.cpp+");
    gROOT->LoadMacro("BinaryPlane.cpp+");
    gROOT->LoadMacro("BinaryEvent.cpp+");
    gROOT->LoadMacro("helpers.cpp+");

    cout << "compile_classes() : Classes compiled." << endl;
}
