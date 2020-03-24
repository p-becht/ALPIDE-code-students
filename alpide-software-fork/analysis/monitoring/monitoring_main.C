void monitoring_main(char *filename="") {

    gSystem->Load("AliPALPIDEFSRawStreamMS_cpp.so");
    gROOT->LoadMacro("monitoring_palpidefs.C+");

    Monitoring r;
    r.SetFile(filename);
    r.Run();

}


