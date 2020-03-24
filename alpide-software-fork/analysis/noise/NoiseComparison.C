#include "NoiseComparison.h"


// read settings
//---------------------------------------------------------------
vector<Int_t> read_int_vals(ifstream &f) {
    vector<Int_t> v_vals;
    Int_t val;
    while(f >> val && f.good()) {      
            v_vals.push_back(val); 
    }
    
    return v_vals;
}

// read vbb values
//---------------------------------------------------------------
vector<Float_t> read_float_vals(ifstream &f) {
    vector<Float_t> v_vals;
    Float_t val;
    while(f >> val && f.good()) {      
            v_vals.push_back(val); 
    }
    
    return v_vals;
}


// look if setting available in measurement folder
//---------------------------------------------------------------
Bool_t is_setting_available(
                TString path_results, 
                TString chip_name_comp, 
                Int_t temperature,  
                Float_t irrad_level_comp,  
                Float_t vbb, 
                Int_t vcasn, 
                Int_t ithr, 
                Int_t rate, 
                Int_t busy
                ) {

    // chip_name
    TString f_chip_name = path_results + "/chip_name.txt";
    ifstream f_chip(f_chip_name.Data());
    TString chip_name;
    string line;
    if(!f_chip.good()) {  
        cout << "chip name file not found, please check!" << endl;
        return kFALSE; 
    }
    else {
        getline(f_chip, line);
        //cout << line << endl;
        chip_name = line;
    }
    if (chip_name_comp.CompareTo(chip_name)!=0) {
        return kFALSE;
    }
    // irrad file
    TString f_irrad_name = path_results + "/irrad_level.txt";
    ifstream f_irrad(f_irrad_name.Data());
    vector<Float_t> irrad_list;
    if(!f_irrad.good()) {  
        cout << "irrad file not found, please check!" << endl;
        return kFALSE; 
    }
    else {
        irrad_list = read_float_vals(f_irrad);
        if (irrad_list.size()!=1) {
            cout << "irrad file has too many lines, please check!" << endl;
            return kFALSE; 
        }
        else {
            // compare irrad level to the one desired to find
            if (irrad_level_comp!=irrad_list[0]) {
                return kFALSE;
            }
        }
    }
    
    // tempearature file
    TString f_temp_name = path_results + "/temperature.txt";
    ifstream f_temp(f_temp_name.Data());
    vector<Int_t> temp_list;
    if(!f_temp.good()) {  
        cout << "temp file not found, please check!" << endl;
        return kFALSE; 
    }
    else {
        temp_list = read_int_vals(f_temp);
        //for (UInt_t i=0; i<temp_list.size(); i++) {
        //    cout << temp_list[i] << endl;
        //}
    }
    // vbb file
    TString f_vbb_name = path_results + "/vbb_list.txt";
    ifstream f_vbb(f_vbb_name.Data());
    vector<Float_t> vbb_list;
    if(!f_vbb.good()) {  
        cout << "vbb file not found, please check!" << endl;
        return kFALSE; 
    }
    else {
        vbb_list = read_float_vals(f_vbb);
    }
    // ithr file
    TString f_ithr_name = path_results + "/ithr_list.txt";
    ifstream f_ithr(f_ithr_name.Data());
    vector<Int_t> ithr_list;
    if(!f_ithr.good()) {  
        cout << "ithr file not found, please check!" << endl;
        return kFALSE; 
    }
    else {
        ithr_list = read_int_vals(f_ithr);
    }
    // rate file
    TString f_rate_name = path_results + "/rate_list.txt";
    ifstream f_rate(f_rate_name.Data());
    vector<Int_t> rate_list;
    if(!f_rate.good()) {  
        cout << "ithr file not found, please check!" << endl;
        return kFALSE; 
    }
    else {
        rate_list = read_int_vals(f_rate);
    }
    //cout << rate_list.size() << endl;
    // busy file
    TString f_busy_name = path_results + "/busy_list.txt";
    ifstream f_busy(f_busy_name.Data());
    vector<Int_t> busy_list;
    if(!f_busy.good()) {  
        cout << "busy file not found, please check!" << endl;
        return kFALSE; 
    }
    else {
        busy_list = read_int_vals(f_busy);
    }
    //cout << busy_list.size() << endl;

    // loop through all available setting combinations
    for (UInt_t i_temp=0; i_temp<temp_list.size(); i_temp++) {
        //cout << "temp: " << temp_list[i_temp] << endl;
        for (UInt_t i_vbb=0; i_vbb<vbb_list.size(); i_vbb++) {
            //cout << " vbb: " << vbb_list[i_vbb] << endl;
            // vcasn file
            TString f_vcasn_name = TString::Format("%s/TEMP-%i.0/VBB-%2.1f/vcasn_list.txt", path_results.Data(), temp_list[i_temp], vbb_list[i_vbb]);
            ifstream f_vcasn(f_vcasn_name.Data());
            vector<Int_t> vcasn_list;
            if(!f_vcasn.good()) {  
                cout << f_vcasn_name.Data() << endl;
                cout << "vcasn file not found, please check!" << endl;
                return kFALSE; 
            }
            else {
                vcasn_list = read_int_vals(f_vcasn);
            }
            for (UInt_t i_vcasn=0; i_vcasn<vcasn_list.size(); i_vcasn++) {
                //cout << "  vcasn: " << vcasn_list[i_vcasn] << endl;
                
                for (UInt_t i_ithr=0; i_ithr<ithr_list.size(); i_ithr++) {
                    //cout << " ithr: " << ithr_list[i_ithr] << endl;

                    for (UInt_t i_rate=0; i_rate<rate_list.size(); i_rate++) {
                        //cout << "   rate: " << rate_list[i_rate] << endl;

                        for (UInt_t i_busy=0; i_busy<busy_list.size(); i_busy++) {
                            //cout << "    busy: " << busy_list[i_busy] << endl;

                            // check if find match with input 
                            if (temp_list[i_temp]==temperature
                              && vbb_list[i_vbb]==vbb 
                              && vcasn_list[i_vcasn]==vcasn
                              && ithr_list[i_ithr]==ithr
                              && rate_list[i_rate]==rate
                              && busy_list[i_busy]==busy) {

                                return kTRUE; // match found

                            } // if
                        } // busy
                    } // rate
                } // ithr
            } // vcasn
        } // vbb
    } // temp
    
    return kFALSE; // no match found
}


// compare hists
//---------------------------------------------------------------
Bool_t plot_hists(vector<TH1F*> v_hists, Float_t y_min, Float_t y_max,  TString y_title, TString x_title, TString legend_title, TString legend_subtitle, vector<TString> v_legend_str, TString chip_name, TString plot_name, Bool_t set_color) {
    //int marker1[7] = {20, 21, 22, 34, 29, 33, 23};
    bool draw_legend = true;
    if (v_hists.size() != v_legend_str.size()) {
    	cerr << "Number of legends doesn't correspond to the number of hists!" << endl;
    	draw_legend = false;
    }
    
    TCanvas *c = new TCanvas("c","",1200,800);
    c->cd(); 
    c->cd()->SetLogy(); 
    c->cd()->SetGridx(); 
    c->cd()->SetGridy(); 

    TString title = TString::Format("pALPIDE-1 %s", chip_name.Data());
    gStyle->SetTitleX(0.22);
    gStyle->SetTitleY(0.95);
    gStyle->SetTitleW(0.195);
    gStyle->SetTitleH(0.05);
    gStyle->SetTitleSize(0.035, "t");

    if (set_color) {
        for (UInt_t i=0; i<v_hists.size(); i++) {
    	    //v_hists[i]->SetFillColor(0);
    	    //v_hists[i]->SetLineColor(2);
        	v_hists[i]->SetLineColor(i<4?i+1:i+2); // avoid yellow
        	v_hists[i]->SetMarkerColor(i<4?i+1:i+2); // avoid yellow
    	    //v_hists[i]->SetLineStyle(i+2);
    	    //v_hists[i]->SetMarkerStyle(marker1[i]);
    	    //v_hists[i]->SetMarkerSize(1.5);
    	    //v_hists[i]->SetMarkerColor(2);
        }
    }

    for (UInt_t i=0; i<v_hists.size(); i++) {
    	//v_hists[i]->SetTitle("");
        v_hists[i]->SetMinimum(y_min);
        v_hists[i]->SetMaximum(y_max);
        v_hists[i]->SetTitle(title);
        v_hists[i]->GetXaxis()->SetTitle(x_title);
        v_hists[i]->GetYaxis()->SetTitle(y_title);
    	v_hists[i]->DrawCopy(i==0?"":"SAME");
    }

    Float_t leg_height = 0.08+v_hists.size()*0.036;
    Float_t leg_length1 = legend_title.Length()*0.0126;
    Float_t leg_length2 = v_legend_str[0].Length()*0.0126+0.1;
    Float_t leg_length = leg_length1>leg_length2 ? leg_length1 : leg_length2;

    //TLegend *leg = new TLegend(0.51,0.946-leg_height,0.86,0.946);
    TLegend *leg = new TLegend(0.95-leg_length,0.946-leg_height,0.95,0.946);
    if (draw_legend)
    {
        leg->SetTextSize(0.035);
    	leg->SetFillColor(0);
        //leg->AddEntry((TObject*)0, "", "");
        leg->AddEntry((TObject*)0, legend_subtitle, "");
        //leg->AddEntry((TObject*)0, Form("%i hottest pixels excluded", n_excl_pix), "");
    	for (UInt_t i=0; i<v_legend_str.size(); i++) {
    		leg->AddEntry(v_hists[i], v_legend_str[i].Data());
    	}
    	leg->SetHeader(legend_title);
    	leg->Draw();
    }

    if (plot_name.CompareTo("")!=0) {
        c->Print(plot_name);
    }

    return kTRUE;
}



// compare different vbb
//---------------------------------------------------------------
Bool_t compare_vbb(TString path_data, TString run_list_name, TString chip_name, Int_t temperature, Float_t irrad_level, Int_t ithr, Int_t rate, Int_t busy, Int_t sec) {
    Int_t vcasn_list[7] = {57, 83, 109, 135, 143, 152, 160}; // nominal vcasn list..

    // list of measurements
    vector<TString> run_list;

    fstream f_list(Form("%s/%s", path_data.Data(), run_list_name.Data())); 
    if (!f_list.is_open()) {
        cout << "summary list file not found!!! please check" << endl;
        return kFALSE; 
    }
    // create measurement folder list
    string line;
    Int_t n_cnt = 0;
    while (getline(f_list, line)) {
        ++n_cnt;
    }
    f_list.clear();
    f_list.seekg(0, ios::beg);
    const Int_t n_meas = n_cnt;
    cout << "number of measurements found: " << n_meas << endl;

    TString run_folder; 
    for (Int_t i_meas=0; i_meas<n_meas; i_meas++) { 
        f_list >> run_folder;
        cout << run_folder << endl;
        run_list.push_back(run_folder);
    }

    vector<TH1F*> v_hists;
    vector<TString> v_legend_str;
    TString legend_title = TString::Format("ITHR%i_sec%i", ithr, sec);
    TString legend_subtitle = TString::Format("T_{cool} = %i C", temperature);
    //TString legend_subtitle = TString::Format("RATE %i Hz, BUSY %i #mus", rate, busy);
    //TString legend_subtitle = TString::Format("");
    TH1F *h;
    TString hist_name;
    TString path_results;
    TFile *f_in = new TFile();

    Float_t vbb=0;
    Int_t   vcasn=0;

    // search for measurements containing values to compare
    for (Int_t i_meas=0; i_meas<n_meas; i_meas++) { 
        path_results = path_data + run_list[i_meas]; 
        f_in = TFile::Open(path_results + "/NoiseOccupancy.root", "READ");
        if (f_in==NULL) {
            cout << path_results << endl;
            cerr << "input file could not be opened! please check! " << endl;
            continue;
        }

        for (Int_t i_vbb=0; i_vbb<=6; i_vbb++) {
            vbb = i_vbb;
            vcasn = vcasn_list[i_vbb];

            if (is_setting_available(path_results, chip_name, temperature, irrad_level, vbb, vcasn, ithr, rate, busy)) {
                cout << "found combination of settings" << endl;
                hist_name = TString::Format("h_noiseocc_TEMP%i.0_VBB%2.1f_VCASN%i_ITHR%i_RATE%i_BUSY%i_sec%i", 
                  temperature, vbb, vcasn, ithr, rate, busy, sec);
                h = (TH1F*)f_in->Get(hist_name);
                if (h) {
                    v_hists.push_back(h);
                    v_legend_str.push_back(Form("VBB%2.1f, VCASN%i", vbb, vcasn));
                }
                else {
                    cout << hist_name << endl;
                    cerr << "hist not found even though setting should be available! please check!" << endl;
                } // if
            } // if
        } // ithr
        //if (f_in!=NULL) {
        //    f_in->Close(); 
        //}
    } // meas
    cout << "loop finished" << endl;
    cout << v_hists.size() << " settings found to compare" << endl;
    if (v_hists.size()<2) {
        return kFALSE;
    }

    TString plot_name = TString::Format("%s/comparisons_noise/%s_vsVBB_TEMP%i.0_ITHR%i_sec%i.pdf", 
      path_data.Data(), chip_name.Data(), temperature, ithr, sec);

    plot_hists(v_hists, 1e-9, 1e-3, "noise occupancy [/event/pixel]", "# excluded pixels", legend_title, legend_subtitle, v_legend_str, chip_name.Data(), plot_name);

    return kTRUE;
} 


// compare different temperatures
//---------------------------------------------------------------
Bool_t compare_temperature(TString path_data, TString run_list_name, TString chip_name, Float_t irrad_level, Float_t vbb, Int_t vcasn, Int_t ithr, Int_t rate, Int_t busy, Int_t sec) {
    // list of measurements
    vector<TString> run_list;

    fstream f_list(Form("%s/%s", path_data.Data(), run_list_name.Data())); 
    if (!f_list.is_open()) {
        cout << "summary list file not found!!! please check" << endl;
        return kFALSE; 
    }
    // create measurement folder list
    string line;
    Int_t n_cnt = 0;
    while (getline(f_list, line)) {
        ++n_cnt;
    }
    f_list.clear();
    f_list.seekg(0, ios::beg);
    const Int_t n_meas = n_cnt;
    cout << "number of measurements found: " << n_meas << endl;

    TString run_folder; 
    for (Int_t i_meas=0; i_meas<n_meas; i_meas++) { 
        f_list >> run_folder;
        cout << run_folder << endl;
        run_list.push_back(run_folder);
    }

    vector<TH1F*> v_hists;
    vector<TString> v_legend_str;
    TString legend_title = TString::Format("VBB-%2.1f_VCASN%i_sec%i", vbb, vcasn, sec);
    TString legend_subtitle = TString::Format("ITHR%i", ithr);
    //TString legend_subtitle = TString::Format("RATE %i Hz, BUSY %i #mus", rate, busy);
    //TString legend_subtitle = TString::Format("");
    TH1F *h;
    TString hist_name;
    TString path_results;
    TFile *f_in = new TFile();

    Int_t temp=0;

    // search for measurements containing values to compare
    for (Int_t i_meas=0; i_meas<n_meas; i_meas++) { 
        path_results = path_data + run_list[i_meas]; 
        f_in = TFile::Open(path_results + "/NoiseOccupancy.root", "READ");
        if (f_in==NULL) {
            cout << path_results << endl;
            cerr << "input file could not be opened! please check! " << endl;
            continue;
        }

        for (Int_t i_temp=0; i_temp<=40; i_temp++) {
            temp = i_temp;

            if (is_setting_available(path_results, chip_name, temp, irrad_level, vbb, vcasn, ithr, rate, busy)) {
                cout << "found combination of settings" << endl;
                hist_name = TString::Format("h_noiseocc_TEMP%i.0_VBB%2.1f_VCASN%i_ITHR%i_RATE%i_BUSY%i_sec%i", 
                  temp, vbb, vcasn, ithr, rate, busy, sec);
                h = (TH1F*)f_in->Get(hist_name);
                if (h) {
                    v_hists.push_back(h);
                    v_legend_str.push_back(Form("TEMP %i.0", temp));
                }
                else {
                    cout << hist_name << endl;
                    cerr << "hist not found even though setting should be available! please check!" << endl;
                } // if
            } // if
        } // ithr
        //if (f_in!=NULL) {
        //    f_in->Close(); 
        //}
    } // meas
    cout << "loop finished" << endl;
    cout << v_hists.size() << " settings found to compare" << endl;
    if (v_hists.size()<2) {
        return kFALSE;
    }

    TString plot_name = TString::Format("%s/comparisons_noise/%s_vsTemp_VBB%2.1f_VCASN%i_ITHR%i_sec%i.pdf", 
      path_data.Data(), chip_name.Data(), vbb, vcasn, ithr, sec);

    plot_hists(v_hists, 1e-9, 1e-3, "noise occupancy [/event/pixel]", "# excluded pixels", legend_title, legend_subtitle, v_legend_str, chip_name.Data(), plot_name);

    return kTRUE;
} 



// compare settings
//---------------------------------------------------------------
Bool_t compare_settings(TString path_data, TString run_list_name, TString chip_name, Int_t temperature, Float_t irrad_level, TString param_to_compare, Int_t param1, Int_t param2, Int_t rate, Int_t busy, Int_t sec) {

    // list of measurements
    vector<TString> run_list;

    fstream f_list(Form("%s/%s", path_data.Data(), run_list_name.Data())); 
    if (!f_list.is_open()) {
        cout << "summary list file not found!!! please check" << endl;
        return kFALSE; 
    }
    // create measurement directories list
    string line;
    Int_t n_cnt = 0;
    while (getline(f_list, line)) {
        ++n_cnt;
    }
    f_list.clear();
    f_list.seekg(0, ios::beg);
    const Int_t n_meas = n_cnt;
    cout << "number of measurements (runs) found: " << n_meas << endl;

    TString run_folder; 
    for (Int_t i_meas=0; i_meas<n_meas; i_meas++) { 
        f_list >> run_folder;
        cout << run_folder << endl;
        run_list.push_back(run_folder);
    }

    // compare different ITHR values
    if (param_to_compare.CompareTo("ITHR")==0) {
        Float_t vbb = (Float_t)param1;  
        Int_t vcasn = param2;  

        vector<TH1F*> v_hists;
        vector<TString> v_legend_str;
        TString legend_title = TString::Format("VBB-%2.1f_VCASN%i_sec%i", vbb, vcasn, sec);
        TString legend_subtitle = TString::Format("T_{cool} = %i C", temperature);
        //TString legend_subtitle = TString::Format("RATE %i Hz, BUSY %i #mus", rate, busy);
        //TString legend_subtitle = TString::Format("");
        TH1F *h;
        TString hist_name;
        TString path_results;
        TFile *f_in = new TFile();
        // search for measurements containing values to compare
        for (Int_t i_meas=0; i_meas<n_meas; i_meas++) { 
            path_results = path_data + run_list[i_meas]; 
            f_in = TFile::Open(path_results + "/NoiseOccupancy.root", "READ");
            if (f_in==NULL) {
                cout << path_results << endl;
                cerr << "input file could not be opened! please check! " << endl;
                continue;
            }

            for (Int_t ithr=0; ithr<71; ithr++) {
                if (is_setting_available(path_results, chip_name, temperature, irrad_level, vbb, vcasn, ithr, rate, busy)) {
                    cout << "found combination of settings" << endl;
                    hist_name = TString::Format("h_noiseocc_TEMP%i.0_VBB%2.1f_VCASN%i_ITHR%i_RATE%i_BUSY%i_sec%i", 
                      temperature, vbb, vcasn, ithr, rate, busy, sec);
                    h = (TH1F*)f_in->Get(hist_name);
                    if (h) {
                        v_hists.push_back(h);
                        v_legend_str.push_back(Form("ITHR = %i", ithr));
                    }
                    else {
                        cout << hist_name << endl;
                        cerr << "hist not found even though setting should be available! please check!" << endl;
                    } // if
                } // if
            } // ithr
            //if (f_in!=NULL) {
            //    f_in->Close(); 
            //}
        } // meas
        cout << "loop finished" << endl;
        cout << v_hists.size() << " settings found to compare" << endl;
        if (v_hists.size()<2) {
            return kFALSE;
        }
        TString plot_name = TString::Format("%s/comparisons_noise/%s_vsITHR_TEMP%i.0_VBB%2.1f_VCASN%i_sec%i.pdf", 
          path_data.Data(), chip_name.Data(), temperature, vbb, vcasn, sec);

        plot_hists(v_hists, 1e-9, 1e-3, "noise occupancy [/event/pixel]", "# excluded pixels", legend_title, legend_subtitle, v_legend_str, chip_name, plot_name);

    } // if ITHR comparison 
    // TODO other comparisons    
    else {
        cerr << "parameter to compare not found, please check!" << endl;
        return kFALSE;
    }

    return kTRUE;
} 


// compare different chips 
//---------------------------------------------------------------
Bool_t compare_chips(TString path_data, TString run_list_name, Int_t temperature, Float_t irrad_level, Float_t vbb, Int_t vcasn, Int_t ithr, Int_t rate, Int_t busy, Int_t sec) {
    // compare only chips of same irradiation level
    Int_t irrad_select=0; 
    if (irrad_level==0) {
        irrad_select = 0;
    }
    else if (irrad_level>0.9e+13) {
        irrad_select = 1;
    } 
    else {
        cout << "irradiation level not found, please check!" << endl;
        return kFALSE;
    }

    // list of measured chips
    const Int_t n_chips = 4;
    const Int_t n_irrad = 2;
    TString chip_list[n_irrad][n_chips];
    // non irrad chips
    chip_list[0][0] = "W1-25";
    chip_list[0][1] = "W2-31";
    chip_list[0][2] = "W9-16";
    chip_list[0][3] = "W9-38";
    // 1e13 chips
    chip_list[1][0] = "W6-6";
    chip_list[1][1] = "W5-25";
    chip_list[1][2] = "W5-21";
    chip_list[1][3] = "";
    
    // list of measurements
    vector<TString> run_list;

    fstream f_list(Form("%s/%s", path_data.Data(), run_list_name.Data())); 
    if (!f_list.is_open()) {
        cout << "summary list file not found!!! please check" << endl;
        return kFALSE; 
    }
    // create measurement folder list
    string line;
    Int_t n_cnt = 0;
    while (getline(f_list, line)) {
        ++n_cnt;
    }
    f_list.clear();
    f_list.seekg(0, ios::beg);
    const Int_t n_meas = n_cnt;
    cout << "number of measurements found: " << n_meas << endl;

    TString run_folder; 
    for (Int_t i_meas=0; i_meas<n_meas; i_meas++) { 
        f_list >> run_folder;
        cout << run_folder << endl;
        run_list.push_back(run_folder);
    }

    vector<TH1F*> v_hists;
    vector<TString> v_legend_str;
    TString legend_title = TString::Format("VBB-%2.1f_VCASN%i_ITHR%i_sec%i", vbb, vcasn, ithr, sec);
    TString legend_subtitle = TString::Format("T_{cool} = %i C", temperature);
    //TString legend_subtitle = TString::Format("RATE %i Hz, BUSY %i #mus", rate, busy);
    //TString legend_subtitle = TString::Format("");
    TH1F *h;
    TString hist_name;
    TString path_results;
    TFile *f_in = new TFile();
    // search for measurements containing values to compare
    for (Int_t i_meas=0; i_meas<n_meas; i_meas++) { 
        path_results = path_data + run_list[i_meas]; 
        f_in = TFile::Open(path_results + "/NoiseOccupancy.root", "READ");
        if (f_in==NULL) {
            cout << path_results << endl;
            cerr << "input file could not be opened! please check! " << endl;
            continue;
        }

        for (Int_t i_chip=0; i_chip<n_chips; i_chip++) {
            if (is_setting_available(path_results, chip_list[irrad_select][i_chip], temperature, irrad_level, vbb, vcasn, ithr, rate, busy)) {
                cout << "found combination of settings" << endl;
                hist_name = TString::Format("h_noiseocc_TEMP%i.0_VBB%2.1f_VCASN%i_ITHR%i_RATE%i_BUSY%i_sec%i", 
                  temperature, vbb, vcasn, ithr, rate, busy, sec);
                h = (TH1F*)f_in->Get(hist_name);
                if (h) {
                    v_hists.push_back(h);
                    v_legend_str.push_back(Form("%s, %2.1e 1MeV neq", chip_list[irrad_select][i_chip].Data(), irrad_level));
                }
                else {
                    cout << path_results << endl;
                    cout << hist_name << endl;
                    cerr << "hist not found even though setting should be available! please check!" << endl;
                } // if
            } // if
        } // ithr
        //if (f_in!=NULL) {
        //    f_in->Close(); 
        //}
    } // meas
    cout << "loop finished" << endl;
    cout << v_hists.size() << " settings found to compare" << endl;
    if (v_hists.size()<2) {
        return kFALSE;
    }
    
    TString plot_name = TString::Format("%s/comparisons_noise/vsChips_IRRAD%2.1e_TEMP%i.0_VBB%2.1f_VCASN%i_ITHR%i_sec%i.pdf", 
      path_data.Data(), irrad_level, temperature, vbb, vcasn, ithr, sec);

    plot_hists(v_hists, 1e-9, 1e-3, "noise occupancy [/event/pixel]", "# excluded pixels", legend_title, legend_subtitle, v_legend_str, "", plot_name);

    return kTRUE;
} 


// compare different irradiation levels
//---------------------------------------------------------------
Bool_t compare_irradiation(TString path_data, TString run_list_name, Int_t temperature, Float_t vbb, Int_t vcasn, Int_t ithr, Int_t rate, Int_t busy, Int_t sec) {
    // list of measured chips
    const Int_t n_chips = 3;
    const Int_t n_irrad = 2;
    TString chip_list[n_irrad][n_chips];
    // non irrad chips
    chip_list[0][0] = "W2-31";
    chip_list[0][1] = "W9-16";
    chip_list[0][2] = "W9-38";
    // 1e13 chips
    chip_list[1][0] = "W6-6";
    chip_list[1][1] = "W5-25";
    chip_list[1][2] = "W5-21";
   
    Float_t irrad_list[n_irrad] = {0, 1e13};

    // list of measurements
    vector<TString> run_list;

    fstream f_list(Form("%s/%s", path_data.Data(), run_list_name.Data())); 
    if (!f_list.is_open()) {
        cout << "summary list file not found!!! please check" << endl;
        return kFALSE; 
    }
    // create measurement folder list
    string line;
    Int_t n_cnt = 0;
    while (getline(f_list, line)) {
        ++n_cnt;
    }
    f_list.clear();
    f_list.seekg(0, ios::beg);
    const Int_t n_meas = n_cnt;
    cout << "number of measurements found: " << n_meas << endl;

    TString run_folder; 
    for (Int_t i_meas=0; i_meas<n_meas; i_meas++) { 
        f_list >> run_folder;
        cout << run_folder << endl;
        run_list.push_back(run_folder);
    }

    vector<TH1F*> v_hists;
    vector<TString> v_legend_str;
    TString legend_title = TString::Format("VBB-%2.1f_VCASN%i_ITHR%i_sec%i", vbb, vcasn, ithr, sec);
    TString legend_subtitle = TString::Format("T_{cool} = %i C", temperature);
    //TString legend_subtitle = TString::Format("RATE %i Hz, BUSY %i #mus", rate, busy);
    //TString legend_subtitle = TString::Format("");
    TH1F *h;
    TString hist_name;
    TString path_results;
    TFile *f_in = new TFile();
    // search for measurements containing values to compare
    for (Int_t i_meas=0; i_meas<n_meas; i_meas++) { 
        path_results = path_data + run_list[i_meas]; 
        f_in = TFile::Open(path_results + "/NoiseOccupancy.root", "READ");
        if (f_in==NULL) {
            cout << path_results << endl;
            cerr << "input file could not be opened! please check! " << endl;
            continue;
        }

        for (Int_t i_irrad=0; i_irrad<n_irrad; i_irrad++) {
            for (Int_t i_chip=0; i_chip<n_chips; i_chip++) {
                if (is_setting_available(path_results, chip_list[i_irrad][i_chip], temperature, irrad_list[i_irrad], vbb, vcasn, ithr, rate, busy)) {
                    cout << "found combination of settings" << endl;
                    hist_name = TString::Format("h_noiseocc_TEMP%i.0_VBB%2.1f_VCASN%i_ITHR%i_RATE%i_BUSY%i_sec%i", 
                      temperature, vbb, vcasn, ithr, rate, busy, sec);
                    h = (TH1F*)f_in->Get(hist_name);
                    if (h) {
                        h->SetLineColor(i_irrad+1);
                        h->SetMarkerColor(i_irrad+1);
                        v_hists.push_back(h);
                        v_legend_str.push_back(Form("%s, %2.1e 1MeV neq", chip_list[i_irrad][i_chip].Data(), irrad_list[i_irrad]));
                    }
                    else {
                        cout << path_results << endl;
                        cout << hist_name << endl;
                        cerr << "hist not found even though setting should be available! please check!" << endl;
                    } // if
                } // if
            } // chips
        } // irrad
    } // meas
    cout << "loop finished" << endl;
    cout << v_hists.size() << " settings found to compare" << endl;
    if (v_hists.size()<2) {
        return kFALSE;
    }
    
    TString plot_name = TString::Format("%s/comparisons_noise/vsIrrad_TEMP%i.0_VBB%2.1f_VCASN%i_ITHR%i_sec%i.pdf", 
      path_data.Data(), temperature, vbb, vcasn, ithr, sec);

    plot_hists(v_hists, 1e-9, 1e-3, "noise occupancy [/event/pixel]", "# excluded pixels", legend_title, legend_subtitle, v_legend_str, "", plot_name, kFALSE);

    return kTRUE;
} 

















