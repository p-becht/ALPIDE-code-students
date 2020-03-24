#include "ThresholdSummary.h"


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


// compare hists
//---------------------------------------------------------------
Bool_t plot_graphs(vector<TGraph*> v_graphs, Float_t y_min, Float_t y_max,  TString y_title, TString x_title, TString legend_title, TString legend_subtitle, vector<TString> v_legend_str, TString chip_name, TString plot_name, Bool_t set_color) {
    int marker1[7] = {20, 21, 22, 34, 29, 33, 23};
    bool draw_legend = true;
    if (v_graphs.size() != v_legend_str.size()) {
    	cerr << "Number of legends doesn't correspond to the number of hists!" << endl;
    	draw_legend = false;
    }
    
    TCanvas *c = new TCanvas("c","",1200,800);
    c->cd(); 
    //c->cd()->SetLogy(); 
    c->cd()->SetGridx(); 
    c->cd()->SetGridy(); 

    TString title = TString::Format("pALPIDE-1 %s", chip_name.Data());
    gStyle->SetTitleX(0.22);
    gStyle->SetTitleY(0.95);
    gStyle->SetTitleW(0.195);
    gStyle->SetTitleH(0.05);
    gStyle->SetTitleSize(0.035, "t");

    if (set_color) {
        for (UInt_t i=0; i<v_graphs.size(); i++) {
    	    //v_graphs[i]->SetFillColor(0);
    	    //v_graphs[i]->SetLineColor(2);
        	v_graphs[i]->SetLineColor(i<4?i+1:i+2); // avoid yellow
        	v_graphs[i]->SetMarkerColor(i<4?i+1:i+2); // avoid yellow
    	    //v_graphs[i]->SetLineStyle(i+2);
    	    //v_graphs[i]->SetMarkerColor(2);
        }
    }

    for (UInt_t i=0; i<v_graphs.size(); i++) {
    	v_graphs[i]->SetMarkerStyle(marker1[i]);
    	v_graphs[i]->SetMarkerSize(1.5);
        
    	//v_graphs[i]->SetTitle("");
        v_graphs[i]->SetMinimum(y_min);
        v_graphs[i]->SetMaximum(y_max);
        v_graphs[i]->SetTitle(title);
        v_graphs[i]->GetXaxis()->SetTitle(x_title);
        v_graphs[i]->GetYaxis()->SetTitle(y_title);
    	//v_graphs[i]->DrawClone(i==0?"":"SAME");
    	v_graphs[i]->DrawClone(i==0?"alp":"lp");
    }

    Float_t leg_height = 0.08+v_graphs.size()*0.036;
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
    		leg->AddEntry(v_graphs[i], v_legend_str[i].Data(), "lpe");
    	}
    	leg->SetHeader(legend_title);
    	leg->Draw();
    }

    if (plot_name.CompareTo("")!=0) {
        c->Print(plot_name);
    }
    return kTRUE;
}


// create noise vs ithr graphs for different settings or conditions
//---------------------------------------------------------------
Bool_t create_threshold_graphs_vsITHR(TString path_data, TString run_list_name) {
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

    TH1F *h;
    TString hist_name;
    TString graph_name;
    TString path_results;
    TFile *f_in = new TFile();
    TFile *f_out = new TFile(path_data + "ThresholdGraphsvsITHR.root", "RECREATE");

    vector<Float_t> x_vals_thres;
    vector<Float_t> y_vals_thres;
    vector<Float_t> y_err_thres;
    vector<Float_t> x_vals_noise;
    vector<Float_t> y_vals_noise;
    vector<Float_t> y_err_noise;

    // search for measurements containing values to compare
    for (Int_t i_meas=0; i_meas<n_meas; i_meas++) { 

        path_results = path_data + run_list[i_meas]; 
        f_in = TFile::Open(path_results + "/Thresholds.root", "READ");
        f_in->cd();
        if (f_in==NULL) {
            cout << path_results << endl;
            cerr << "input file could not be opened! please check! " << endl;
            continue;
        }

        // loop through list of settings to create graphs
        // -----------------------------------
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
        }
        
        // temperature file
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
                    for (Int_t i_sec=0; i_sec<4; i_sec++) {
                        x_vals_thres.clear();
                        y_vals_thres.clear();
                        y_err_thres.clear();
                        x_vals_noise.clear();
                        y_vals_noise.clear();
                        y_err_noise.clear();

                        for (UInt_t i_ithr=0; i_ithr<ithr_list.size(); i_ithr++) {
                            //cout << " ithr: " << ithr_list[i_ithr] << endl;
                            hist_name = TString::Format("h_thresholds_TEMP%i.0_VBB%2.1f_VCASN%i_ITHR%i_sec%i", 
                              temp_list[i_temp], vbb_list[i_vbb], vcasn_list[i_vcasn], ithr_list[i_ithr], i_sec);
                            h = (TH1F*)f_in->Get(hist_name);
                            if (h) {
                                x_vals_thres.push_back(ithr_list[i_ithr]);
                                y_vals_thres.push_back(h->GetMean());
                                y_err_thres.push_back(h->GetRMS());
                            }
                            hist_name = TString::Format("h_noise_TEMP%i.0_VBB%2.1f_VCASN%i_ITHR%i_sec%i", 
                              temp_list[i_temp], vbb_list[i_vbb], vcasn_list[i_vcasn], ithr_list[i_ithr], i_sec);
                            h = (TH1F*)f_in->Get(hist_name);
                            if (h) {
                                x_vals_noise.push_back(ithr_list[i_ithr]);
                                y_vals_noise.push_back(h->GetMean());
                                y_err_noise.push_back(h->GetRMS());
                            }
                        } // ithr

                        TGraphErrors *g_thres = new TGraphErrors(x_vals_thres.size(), &x_vals_thres[0], &y_vals_thres[0], 0, &y_err_thres[0]);
                        graph_name = TString::Format("g_thresVsITHR_%s_TEMP%i.0_VBB%2.1f_VCASN%i_sec%i", 
                              chip_name.Data(), temp_list[i_temp], vbb_list[i_vbb], vcasn_list[i_vcasn], i_sec);
                        g_thres->SetName(graph_name);

                        TGraphErrors *g_noise = new TGraphErrors(x_vals_noise.size(), &x_vals_noise[0], &y_vals_noise[0], 0, &y_err_noise[0]);
                        graph_name = TString::Format("g_thresnoiseVsITHR_%s_TEMP%i.0_VBB%2.1f_VCASN%i_sec%i", 
                              chip_name.Data(), temp_list[i_temp], vbb_list[i_vbb], vcasn_list[i_vcasn], i_sec);
                        g_noise->SetName(graph_name);

                        f_out->cd();
                        g_thres->Write();
                        g_noise->Write();
                    }
                } // vcasn
            } // vbb
        } // temp
    
    }

    f_out->Close();
    return kTRUE;
}



// create noise vs ithr graphs for different settings or conditions
//---------------------------------------------------------------
Bool_t create_threshold_graphs_vsTemp(TString path_data, TString run_list_name) {
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

    TH1F *h;
    TString hist_name;
    TString graph_name;
    TString path_results;
    TFile *f_in = new TFile();
    TFile *f_out = new TFile(path_data + "ThresholdGraphsvsTemp.root", "RECREATE");

    vector<Float_t> x_vals_thres;
    vector<Float_t> y_vals_thres;
    vector<Float_t> y_err_thres;
    vector<Float_t> x_vals_noise;
    vector<Float_t> y_vals_noise;
    vector<Float_t> y_err_noise;

    // search for measurements containing values to compare
    for (Int_t i_meas=0; i_meas<n_meas; i_meas++) { 

        path_results = path_data + run_list[i_meas]; 
        f_in = TFile::Open(path_results + "/Thresholds.root", "READ");
        f_in->cd();
        if (f_in==NULL) {
            cout << path_results << endl;
            cerr << "input file could not be opened! please check! " << endl;
            continue;
        }

        // loop through list of settings to create graphs
        // -----------------------------------
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
        }
        
        // vbb file
        const Int_t n_vbb = 2;
        Float_t vbb_list[n_vbb] = {0, 3};
        // vcasn file
        const Int_t n_vcasn = 5;
        Int_t vcasn_list[n_vcasn] = {57, 135};
        // ithr file
        const Int_t n_ithr = 5;
        Int_t ithr_list[n_ithr] = {10, 20, 30, 51, 70};
        // temp file
        const Int_t n_temp = 4;
        Int_t temp_list[n_temp] = {12, 20, 28, 36};

        // loop through all available setting combinations
        for (Int_t i_vbb=0; i_vbb<n_vbb; i_vbb++) {
            for (Int_t i_ithr=0; i_ithr<n_ithr; i_ithr++) {
                for (Int_t i_sec=0; i_sec<4; i_sec++) {
                    x_vals_thres.clear();
                    y_vals_thres.clear();
                    y_err_thres.clear();
                    x_vals_noise.clear();
                    y_vals_noise.clear();
                    y_err_noise.clear();

                    for (Int_t i_temp=0; i_temp<n_temp; i_temp++) {
                        //cout << " ithr: " << ithr_list[i_ithr] << endl;
                        hist_name = TString::Format("h_thresholds_TEMP%i.0_VBB%2.1f_VCASN%i_ITHR%i_sec%i", 
                          temp_list[i_temp], vbb_list[i_vbb], vcasn_list[i_vbb], ithr_list[i_ithr], i_sec);
                        h = (TH1F*)f_in->Get(hist_name);
                        if (h) {
                            x_vals_thres.push_back(temp_list[i_temp]);
                            y_vals_thres.push_back(h->GetMean());
                            y_err_thres.push_back(h->GetRMS());
                        }
                        hist_name = TString::Format("h_noise_TEMP%i.0_VBB%2.1f_VCASN%i_ITHR%i_sec%i", 
                          temp_list[i_temp], vbb_list[i_vbb], vcasn_list[i_vbb], ithr_list[i_ithr], i_sec);
                        h = (TH1F*)f_in->Get(hist_name);
                        if (h) {
                            x_vals_noise.push_back(temp_list[i_temp]);
                            y_vals_noise.push_back(h->GetMean());
                            y_err_noise.push_back(h->GetRMS());
                        }
                    } // temp

                    TGraphErrors *g_thres = new TGraphErrors(x_vals_thres.size(), &x_vals_thres[0], &y_vals_thres[0], 0, &y_err_thres[0]);
                    graph_name = TString::Format("g_thresVsTemp_%s_VBB%2.1f_VCASN%i_ITHR%i_sec%i", 
                          chip_name.Data(), vbb_list[i_vbb], vcasn_list[i_vbb], ithr_list[i_ithr], i_sec);
                    g_thres->SetName(graph_name);

                    TGraphErrors *g_rms = new TGraphErrors(x_vals_thres.size(), &x_vals_thres[0], &y_err_thres[0], 0, 0);
                    graph_name = TString::Format("g_thresrmsVsTemp_%s_VBB%2.1f_VCASN%i_ITHR%i_sec%i", 
                          chip_name.Data(), vbb_list[i_vbb], vcasn_list[i_vbb], ithr_list[i_ithr], i_sec);
                    g_rms->SetName(graph_name);

                    TGraphErrors *g_noise = new TGraphErrors(x_vals_noise.size(), &x_vals_noise[0], &y_vals_noise[0], 0, &y_err_noise[0]);
                    graph_name = TString::Format("g_thresnoiseVsTemp_%s_VBB%2.1f_VCASN%i_ITHR%i_sec%i", 
                          chip_name.Data(), vbb_list[i_vbb], vcasn_list[i_vbb], ithr_list[i_ithr], i_sec);
                    g_noise->SetName(graph_name);

                    f_out->cd();
                    g_thres->Write();
                    g_rms->Write();
                    g_noise->Write();

                }
            } // ithr
        } // vbb
    }

    f_out->Close();
    return kTRUE;
}
/*



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// vsITHR graph comparisons
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// compare different temperatures
//---------------------------------------------------------------
Bool_t compare_vsITHR_temperature(TString path_data, TString chip_name, Float_t irrad_level, Float_t vbb, Int_t vcasn, Int_t rate, Int_t busy, Int_t sec) {
    vector<TGraph*> v_graphs;
    vector<TString> v_legend_str;
    TString legend_title = TString::Format("VBB-%2.1f_VCASN%i_sec%i", vbb, vcasn, sec);
    //TString legend_subtitle = TString::Format("ITHR%i", ithr);
    //TString legend_subtitle = TString::Format("RATE %i Hz, BUSY %i #mus", rate, busy);
    TString legend_subtitle = TString::Format("");
    TGraph *g;
    TString graph_name;
    TString path_results;
    TFile *f_in = new TFile();

    Int_t temp=0;

    // search for graphs to compare

    f_in = TFile::Open(path_data + "/NoiseGraphsvsITHR.root", "READ");
    if (f_in==NULL) {
        cout << path_data << endl;
        cerr << "input file could not be opened! please check! " << endl;
        return kFALSE;
    }

    for (Int_t i_temp=0; i_temp<=40; i_temp++) {
        temp = i_temp;
        
        graph_name = TString::Format("g_noiseVsITHR_%s_TEMP%i.0_VBB%2.1f_VCASN%i_RATE%i_BUSY%i_sec%i", 
          chip_name.Data(), temp, vbb, vcasn, rate, busy, sec);
        
        g = (TGraph*)f_in->Get(graph_name);
        if (g) {
            v_graphs.push_back(g);
            v_legend_str.push_back(Form("TEMP %i.0", temp));
        }

    } // temp
    //if (f_in!=NULL) {
    //    f_in->Close(); 
    //}
    cout << "loop finished" << endl;
    cout << v_graphs.size() << " settings found to compare" << endl;
    if (v_graphs.size()<2) {
        return kFALSE;
    }

    TString plot_name = TString::Format("%s/comparisons_noise/summary_plots/%s_vsTemp_VBB%2.1f_VCASN%i_sec%i.pdf", 
      path_data.Data(), chip_name.Data(), vbb, vcasn, sec);

    plot_graphs(v_graphs, 1e-9, 1e-3, "noise occupancy [/event/pixel]", "ITHR [DAC]", legend_title, legend_subtitle, v_legend_str, chip_name.Data(), plot_name);

    return kTRUE;
} 

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// vsTemp graph comparisons
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// vsTemp graphs: compare different sectors
//---------------------------------------------------------------
Bool_t compare_vsTemp_Sectors(TString path_data, TString chip_name, Float_t irrad_level, Float_t vbb, Int_t vcasn, Int_t ithr, Int_t rate, Int_t busy) {
    vector<TGraph*> v_graphs;
    vector<TString> v_legend_str;
    TString legend_title = TString::Format("VBB-%2.1f_VCASN%i_ITHR%i", vbb, vcasn, ithr);
    //TString legend_subtitle = TString::Format("ITHR%i", ithr);
    //TString legend_subtitle = TString::Format("RATE %i Hz, BUSY %i #mus", rate, busy);
    TString legend_subtitle = TString::Format("");
    TGraph *g;
    TString graph_name;
    TString path_results;
    TFile *f_in = new TFile();

    Int_t sec=0;

    // search for graphs to compare

    f_in = TFile::Open(path_data + "/NoiseGraphsvsTemp.root", "READ");
    if (f_in==NULL) {
        cout << path_data << endl;
        cerr << "input file could not be opened! please check! " << endl;
        return kFALSE;
    }

    for (Int_t i_sec=0; i_sec<4; i_sec++) {
        sec = i_sec;
        
        graph_name = TString::Format("g_noiseVsTemp_%s_VBB%2.1f_VCASN%i_ITHR%i_RATE%i_BUSY%i_sec%i", 
          chip_name.Data(), vbb, vcasn, ithr, rate, busy, sec);
        
        g = (TGraph*)f_in->Get(graph_name);
        if (g) {
            v_graphs.push_back(g);
            v_legend_str.push_back(Form("Sector %i", i_sec));
        }

    } // temp
    //if (f_in!=NULL) {
    //    f_in->Close(); 
    //}
    cout << "loop finished" << endl;
    cout << v_graphs.size() << " settings found to compare" << endl;
    if (v_graphs.size()<2) {
        return kFALSE;
    }

    TString plot_name = TString::Format("%s/comparisons_noise/summary_plots/%s_vsSec_VBB%2.1f_VCASN%i_ITHR%i.pdf", 
      path_data.Data(), chip_name.Data(), vbb, vcasn, ithr, sec);

    plot_graphs(v_graphs, 1e-9, 1e-3, "noise occupancy [/event/pixel]", "temperature [#circC]", legend_title, legend_subtitle, v_legend_str, chip_name.Data(), plot_name);

    return kTRUE;
} 

// vsTemp graphs: compare different ithr
//---------------------------------------------------------------
Bool_t compare_vsTemp_ITHR(TString path_data, TString chip_name, Float_t irrad_level, Float_t vbb, Int_t vcasn, Int_t rate, Int_t busy, Int_t sec) {
    vector<TGraph*> v_graphs;
    vector<TString> v_legend_str;
    TString legend_title = TString::Format("VBB-%2.1f_VCASN%i_sec%i", vbb, vcasn, sec);
    //TString legend_subtitle = TString::Format("ITHR%i", ithr);
    //TString legend_subtitle = TString::Format("RATE %i Hz, BUSY %i #mus", rate, busy);
    TString legend_subtitle = TString::Format("");
    TGraph *g;
    TString graph_name;
    TString path_results;
    TFile *f_in = new TFile();

    Int_t ithr=0;

    // search for graphs to compare

    f_in = TFile::Open(path_data + "/NoiseGraphsvsTemp.root", "READ");
    if (f_in==NULL) {
        cout << path_data << endl;
        cerr << "input file could not be opened! please check! " << endl;
        return kFALSE;
    }

    for (Int_t i_ithr=5; i_ithr<=80; i_ithr++) {
        ithr = i_ithr;
        
        graph_name = TString::Format("g_noiseVsTemp_%s_VBB%2.1f_VCASN%i_ITHR%i_RATE%i_BUSY%i_sec%i", 
          chip_name.Data(), vbb, vcasn, ithr, rate, busy, sec);
        
        g = (TGraph*)f_in->Get(graph_name);
        if (g) {
            v_graphs.push_back(g);
            v_legend_str.push_back(Form("ITHR %i", ithr));
        }

    } // temp
    //if (f_in!=NULL) {
    //    f_in->Close(); 
    //}
    cout << "loop finished" << endl;
    cout << v_graphs.size() << " settings found to compare" << endl;
    if (v_graphs.size()<2) {
        return kFALSE;
    }

    TString plot_name = TString::Format("%s/comparisons_noise/summary_plots/%s_vsITHR_VBB%2.1f_VCASN%i_sec%i.pdf", 
      path_data.Data(), chip_name.Data(), vbb, vcasn, sec);

    plot_graphs(v_graphs, 1e-9, 1e-3, "noise occupancy [/event/pixel]", "temperature [#circC]", legend_title, legend_subtitle, v_legend_str, chip_name.Data(), plot_name);

    return kTRUE;
} 

// vsTemp graphs: compare different VBB
//---------------------------------------------------------------
Bool_t compare_vsTemp_VBB(TString path_data, TString chip_name, Float_t irrad_level, Int_t ithr, Int_t rate, Int_t busy, Int_t sec) {
    Int_t vcasn_list[7] = {57, 83, 109, 135, 143, 152, 160}; // nominal vcasn list..

    vector<TGraph*> v_graphs;
    vector<TString> v_legend_str;
    TString legend_title = TString::Format("ITHR%i_sec%i", ithr, sec);
    //TString legend_subtitle = TString::Format("ITHR%i", ithr);
    //TString legend_subtitle = TString::Format("RATE %i Hz, BUSY %i #mus", rate, busy);
    TString legend_subtitle = TString::Format("");
    TGraph *g;
    TString graph_name;
    TString path_results;
    TFile *f_in = new TFile();

    Float_t vbb=0;
    Int_t vcasn=0;

    // search for graphs to compare

    f_in = TFile::Open(path_data + "/NoiseGraphsvsTemp.root", "READ");
    if (f_in==NULL) {
        cout << path_data << endl;
        cerr << "input file could not be opened! please check! " << endl;
        return kFALSE;
    }

    for (Int_t i_vbb=0; i_vbb<=6; i_vbb++) {
        vbb = i_vbb;
        vcasn = vcasn_list[i_vbb];

        graph_name = TString::Format("g_noiseVsTemp_%s_VBB%2.1f_VCASN%i_ITHR%i_RATE%i_BUSY%i_sec%i", 
          chip_name.Data(), vbb, vcasn, ithr, rate, busy, sec);
        
        g = (TGraph*)f_in->Get(graph_name);
        if (g) {
            v_graphs.push_back(g);
            v_legend_str.push_back(Form("VBB %2.1f V", vbb));
        }

    } // temp
    //if (f_in!=NULL) {
    //    f_in->Close(); 
    //}
    cout << "loop finished" << endl;
    cout << v_graphs.size() << " settings found to compare" << endl;
    if (v_graphs.size()<2) {
        return kFALSE;
    }

    TString plot_name = TString::Format("%s/comparisons_noise/summary_plots/%s_vsVBB_ITHR%i_sec%i.pdf", 
      path_data.Data(), chip_name.Data(), ithr, sec);

    plot_graphs(v_graphs, 1e-9, 1e-3, "noise occupancy [/event/pixel]", "temperature [#circC]", legend_title, legend_subtitle, v_legend_str, chip_name.Data(), plot_name);

    return kTRUE;
} 

// vsTemp graphs: compare different chips
//---------------------------------------------------------------
Bool_t compare_vsTemp_Chips(TString path_data, Float_t irrad_level, Float_t vbb, Int_t vcasn, Int_t ithr, Int_t rate, Int_t busy, Int_t sec) {
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

    vector<TGraph*> v_graphs;
    vector<TString> v_legend_str;
    TString legend_title = TString::Format("VBB-%2.1f_VCASN%i_sec%i", vbb, vcasn, sec);
    TString legend_subtitle = TString::Format("ITHR%i", ithr);
    //TString legend_subtitle = TString::Format("RATE %i Hz, BUSY %i #mus", rate, busy);
    //TString legend_subtitle = TString::Format("");
    TGraph *g;
    TString graph_name;
    TString path_results;
    TFile *f_in = new TFile();

    // search for graphs to compare

    f_in = TFile::Open(path_data + "/NoiseGraphsvsTemp.root", "READ");
    if (f_in==NULL) {
        cout << path_data << endl;
        cerr << "input file could not be opened! please check! " << endl;
        return kFALSE;
    }

    for (Int_t i_chip=0; i_chip<n_chips; i_chip++) {
        
        graph_name = TString::Format("g_noiseVsTemp_%s_VBB%2.1f_VCASN%i_ITHR%i_RATE%i_BUSY%i_sec%i", 
          chip_list[irrad_select][i_chip].Data(), vbb, vcasn, ithr, rate, busy, sec);
        
        g = (TGraph*)f_in->Get(graph_name);
        if (g) {
            v_graphs.push_back(g);
            v_legend_str.push_back(Form("%s, %2.1e 1MeV neq", chip_list[irrad_select][i_chip].Data(), irrad_level));
        }

    } // chip
    //if (f_in!=NULL) {
    //    f_in->Close(); 
    //}
    cout << "loop finished" << endl;
    cout << v_graphs.size() << " settings found to compare" << endl;
    if (v_graphs.size()<2) {
        return kFALSE;
    }

    TString plot_name = TString::Format("%s/comparisons_noise/summary_plots/vsChips_IRRAD%2.1e_VBB%2.1f_VCASN%i_ITHR%i_sec%i.pdf", 
      path_data.Data(), irrad_level, vbb, vcasn, ithr, sec);

    plot_graphs(v_graphs, 1e-9, 1e-3, "noise occupancy [/event/pixel]", "temperature [#circC]", legend_title, legend_subtitle, v_legend_str, "", plot_name);

    return kTRUE;
} 

*/

// vsTemp graphs: compare different irradiation levels
//---------------------------------------------------------------
Bool_t compare_vsTemp_Irrad(TString path_data, Float_t vbb, Int_t vcasn, Int_t ithr, Int_t sec, TString compare_select) {
    if (!compare_select.EqualTo("thres") 
        && !compare_select.EqualTo("thresnoise")
        && !compare_select.EqualTo("thresrms")
        ) {
        cout << "comparison selection not found, please check!" << endl;
        return kFALSE;
    }

    gStyle->SetOptStat(0);
    
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

    vector<TGraph*> v_graphs;
    vector<TString> v_legend_str;
    TString legend_title = TString::Format("VBB-%2.1f_VCASN%i_sec%i", vbb, vcasn, sec);
    TString legend_subtitle = TString::Format("ITHR%i", ithr);
    //TString legend_subtitle = TString::Format("RATE %i Hz, BUSY %i #mus", rate, busy);
    //TString legend_subtitle = TString::Format("");
    TGraph *g;
    TString graph_name;
    TString path_results;
    TFile *f_in = new TFile();

    // search for graphs to compare

    f_in = TFile::Open(path_data + "/ThresholdGraphsvsTemp.root", "READ");
    if (f_in==NULL) {
        cout << path_data << endl;
        cerr << "input file could not be opened! please check! " << endl;
        return kFALSE;
    }

    for (Int_t i_irrad=0; i_irrad<n_irrad; i_irrad++) {
        for (Int_t i_chip=0; i_chip<n_chips; i_chip++) {
            
            graph_name = TString::Format("g_%sVsTemp_%s_VBB%2.1f_VCASN%i_ITHR%i_sec%i", 
              compare_select.Data(), chip_list[i_irrad][i_chip].Data(), vbb, vcasn, ithr, sec);
            
            g = (TGraph*)f_in->Get(graph_name);
            if (g) {
                g->SetLineColor(i_irrad+1);
                g->SetMarkerColor(i_irrad+1);
                v_graphs.push_back(g);
                v_legend_str.push_back(Form("%s, %2.1e 1MeV neq", chip_list[i_irrad][i_chip].Data(), irrad_list[i_irrad]));
            }
            else {
                cout << graph_name << endl;
            }
        } // chip
    } // irrad
    cout << "loop finished" << endl;
    cout << v_graphs.size() << " settings found to compare" << endl;
    if (v_graphs.size()<2) {
        return kFALSE;
    }

    if (compare_select.EqualTo("thres")) { // thres
        TString plot_name = TString::Format("%s/comparisons_threshold/summary_plots/vsIrrad_thres_VBB%2.1f_VCASN%i_ITHR%i_sec%i.pdf", 
          path_data.Data(), vbb, vcasn, ithr, sec);
        plot_graphs(v_graphs, 0, 300, "threshold [/e^{-}]", "temperature [#circC]", legend_title, legend_subtitle, v_legend_str, "", plot_name, kFALSE);
    }
    else if (compare_select.EqualTo("thresnoise")) { // thresnoise
        TString plot_name = TString::Format("%s/comparisons_threshold/summary_plots/vsIrrad_thresnoise_VBB%2.1f_VCASN%i_ITHR%i_sec%i.pdf", 
          path_data.Data(), vbb, vcasn, ithr, sec);
        plot_graphs(v_graphs, 0, 20, "threshold noise [/e^{-}]", "temperature [#circC]", legend_title, legend_subtitle, v_legend_str, "", plot_name, kFALSE);
    }
    else { // thresrms
        TString plot_name = TString::Format("%s/comparisons_threshold/summary_plots/vsIrrad_thresrms_VBB%2.1f_VCASN%i_ITHR%i_sec%i.pdf", 
          path_data.Data(), vbb, vcasn, ithr, sec);
        plot_graphs(v_graphs, 0, 50, "threshold RMS [/e^{-}]", "temperature [#circC]", legend_title, legend_subtitle, v_legend_str, "", plot_name, kFALSE);
    }



    return kTRUE;
} 




