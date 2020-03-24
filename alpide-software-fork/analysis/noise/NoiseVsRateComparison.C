#include "NoiseVsRateComparison.h"


// read settings
//---------------------------------------------------------------
vector<Int_t> read_settings(ifstream &f) {
    vector<Int_t> v_vals;
    Int_t val;
    while(f >> val && f.good()) {      
            v_vals.push_back(val); 
    }
    
    return v_vals;
}

// read vbb values
//---------------------------------------------------------------
vector<Float_t> read_vbb_vals(ifstream &f) {
    vector<Float_t> v_vals;
    Float_t val;
    while(f >> val && f.good()) {      
            v_vals.push_back(val); 
    }
    
    return v_vals;
}

// graphs
//---------------------------------------------------------------
Bool_t create_graphs(TString path_results, Int_t n_excl_pix) {
    //TString path_results = "/Users/jvanhoor/work/software/pALPIDEfs-software/analysis/results/threshold/overview-3V/";

    // read in configs
    //--------------------------------------------
    // noise vs rate result file
    TString f_in_name = path_results + "/NoiseOccupancy.root";
    TFile *f_in = new TFile(f_in_name, "READ");

    // vbb file
    TString f_vbb_name = path_results + "/vbb_list.txt";
    ifstream f_vbb(f_vbb_name.Data());
    vector<Float_t> vbb_list;
    if(!f_vbb.good()) {  
        cout << "vbb file not found, please check!" << endl;
        return kFALSE; 
    }
    else {
        vbb_list = read_vbb_vals(f_vbb);
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
        ithr_list = read_settings(f_ithr);
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
        rate_list = read_settings(f_rate);
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
        busy_list = read_settings(f_busy);
    }
    //cout << busy_list.size() << endl;

    // prepare output
    //--------------------------------------------
    // output file with 2D plots
    TString f_out_name = TString::Format("%s/NoiseOccupancyOverview_%i.root", path_results.Data(), n_excl_pix) ;
    TFile *f_out = new TFile(f_out_name, "RECREATE");

    // rate limits
    Int_t rate_min = *std::min_element(rate_list.begin(), rate_list.end());
    Int_t rate_max = *std::max_element(rate_list.begin(), rate_list.end());
    cout << rate_min << "\t" << rate_max << endl;

    // busy limits
    Int_t busy_min = *std::min_element(busy_list.begin(), busy_list.end());
    Int_t busy_max = *std::max_element(busy_list.begin(), busy_list.end());
    cout << busy_min << "\t" << busy_max << endl;

    
    // 1D graphs
    const Int_t n_busy = busy_list.size();
    const Int_t n_rate = rate_list.size();
    TGraph *g_vsRate[n_busy][4];
    TGraph *g_vsBusy[n_rate][4];

    TH1F *h_noiseocc;
    TString name_noiseocc;

    f_out->cd();
    for (UInt_t i_vbb=0; i_vbb<vbb_list.size(); i_vbb++) {
        cout << "vbb: " << vbb_list[i_vbb] << endl;
        for (UInt_t i_ithr=0; i_ithr<ithr_list.size(); i_ithr++) {
            cout << " ithr: " << ithr_list[i_ithr] << endl;
            // vcasn file
            TString f_vcasn_name = TString::Format("%s/VBB-%2.1f/vcasn_list.txt", path_results.Data(), vbb_list[i_vbb]);
            ifstream f_vcasn(f_vcasn_name.Data());
            vector<Int_t> vcasn_list;
            if(!f_vcasn.good()) {  
                cout << "vcasn file not found, please check!" << endl;
                return kFALSE; 
            }
            else {
                vcasn_list = read_settings(f_vcasn);
            }

            for (UInt_t i_vcasn=0; i_vcasn<vcasn_list.size(); i_vcasn++) {
                cout << "  vcasn: " << vcasn_list[i_vcasn] << endl;

                // graphs vs busy values
                for (Int_t i_rate=0; i_rate<n_rate; i_rate++) {
                    //cout << "   rate: " << rate_list[i_rate] << endl;
                    for (Int_t i_sec=0; i_sec<4; i_sec++) {
                        g_vsBusy[i_rate][i_sec] = new TGraph();
                        g_vsBusy[i_rate][i_sec]->SetName(Form("g_vsBusy_VBB%2.1f_VCASN%i_ITHR%i_RATE%i_sec%i",
                          vbb_list[i_vbb], vcasn_list[i_vcasn], ithr_list[i_ithr], rate_list[i_rate], i_sec));
                        g_vsBusy[i_rate][i_sec]->SetLineColor(i_sec+1);
                        g_vsBusy[i_rate][i_sec]->SetMarkerColor(i_sec+1);

                        for (Int_t i_busy=0; i_busy<n_busy; i_busy++) {
                            //cout << "    busy: " << busy_list[i_busy] << endl;
                     
                            name_noiseocc = TString::Format("h_noiseocc_VBB%2.1f_VCASN%i_ITHR%i_RATE%i_BUSY%i_sec%i", 
                              vbb_list[i_vbb], vcasn_list[i_vcasn], ithr_list[i_ithr], rate_list[i_rate], busy_list[i_busy], i_sec);
                            cout << name_noiseocc << endl;
                            h_noiseocc = (TH1F*)f_in->Get(name_noiseocc);
                            if(h_noiseocc) {
                                g_vsBusy[i_rate][i_sec]->SetPoint(g_vsBusy[i_rate][i_sec]->GetN(), busy_list[i_busy], 
                                  h_noiseocc->GetBinContent(n_excl_pix));     
                            }
                            else {
                                cout << "hist not found!" << endl;
                                return kFALSE;
                            } 

                        }
                        g_vsBusy[i_rate][i_sec]->Draw("alp");
                        g_vsBusy[i_rate][i_sec]->Write();
                    }
                }
                // graphs vs busy values
                for (Int_t i_busy=0; i_busy<n_busy; i_busy++) {
                    //cout << "   busy: " << busy_list[i_busy] << endl;
                    for (Int_t i_sec=0; i_sec<4; i_sec++) {
                        g_vsRate[i_busy][i_sec] = new TGraph();
                        g_vsRate[i_busy][i_sec]->SetName(Form("g_vsRate_VBB%2.1f_VCASN%i_ITHR%i_BUSY%i_sec%i",
                          vbb_list[i_vbb], vcasn_list[i_vcasn], ithr_list[i_ithr], busy_list[i_busy], i_sec));
                        g_vsRate[i_busy][i_sec]->SetLineColor(i_sec+1);
                        g_vsRate[i_busy][i_sec]->SetMarkerColor(i_sec+1);

                        for (Int_t i_rate=0; i_rate<n_rate; i_rate++) {
                            //cout << "    rate: " << rate_list[i_rate] << endl;
                     
                            name_noiseocc = TString::Format("h_noiseocc_VBB%2.1f_VCASN%i_ITHR%i_RATE%i_BUSY%i_sec%i", 
                              vbb_list[i_vbb], vcasn_list[i_vcasn], ithr_list[i_ithr], rate_list[i_rate], busy_list[i_busy], i_sec);
                            cout << name_noiseocc << endl;
                            h_noiseocc = (TH1F*)f_in->Get(name_noiseocc);
                            if(h_noiseocc) {
                                g_vsRate[i_busy][i_sec]->SetPoint(g_vsRate[i_busy][i_sec]->GetN(), rate_list[i_rate], 
                                  h_noiseocc->GetBinContent(n_excl_pix));     
                            }
                            else {
                                cout << "hist not found!" << endl;
                                return kFALSE;
                            } 

                        }
                        g_vsRate[i_busy][i_sec]->Draw("alp");
                        g_vsRate[i_busy][i_sec]->Write();
                    }
                }
            }
        }
    }

    f_out->Close();

    return kTRUE;
}

//---------------------------------------------------------------
Bool_t compare_graphs(TString path_results, TString type, Float_t vbb, Int_t vcasn, Int_t ithr, Int_t sec, Int_t n_excl_pix, Bool_t save_plot) {

    // check input
    if (type.CompareTo("vsRate")==0 || type.CompareTo("vsBusy")==0) {
        cout << "comparison type: " << type << endl;
    }
    else {
        cout << "comparison type not valid!" << endl;
        return kFALSE;
    }

    TFile *f_in = new TFile(Form("%s/NoiseOccupancyOverview_%i.root", path_results.Data(), n_excl_pix), "READ");
    if (!f_in->IsOpen()) {
        cout << "input file not found!" << endl;
        return kFALSE;
    }
    
    // read in necessarry information
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
        cout << line << endl;
        chip_name = line;
    }
    TString f_rate_name = path_results + "/rate_list.txt";
    ifstream f_rate(f_rate_name.Data());
    vector<Int_t> rate_list;
    if(!f_rate.good()) {  
        cout << "ithr file not found, please check!" << endl;
        return kFALSE; 
    }
    else {
        rate_list = read_settings(f_rate);
    }
    TString f_busy_name = path_results + "/busy_list.txt";
    ifstream f_busy(f_busy_name.Data());
    vector<Int_t> busy_list;
    if(!f_busy.good()) {  
        cout << "busy file not found, please check!" << endl;
        return kFALSE; 
    }
    else {
        busy_list = read_settings(f_busy);
    }
    const Int_t n_busy = busy_list.size();
    const Int_t n_rate = rate_list.size();
    
    // read in right graphs
    TGraph *g = new TGraph();
    TString name_graph;
    vector<TGraph*> v_graphs;
    vector<TString> v_legend_str;
    TString legend_title = TString::Format("VBB-%2.1f_VCASN%i_ITHR%i_sec%i", vbb, vcasn, ithr, sec);

    if (type.CompareTo("vsRate")==0) {
        for (Int_t i_busy=0; i_busy<n_busy; i_busy++) {
            name_graph = TString::Format("g_vsRate_VBB%2.1f_VCASN%i_ITHR%i_BUSY%i_sec%i", vbb, vcasn, ithr, busy_list[i_busy], sec);
            g = (TGraph*)f_in->Get(name_graph); 
            if (g) {
                v_graphs.push_back(g);
                v_legend_str.push_back(Form("BUSY = %i #mus", busy_list[i_busy]));
            }
            else {
                cout << name_graph << endl;
                cout << "graph not found!" << endl;
                return kFALSE;
            }
        }
        if (save_plot) {
            TString plot_name = TString::Format("%s/plots/%s_vsRate_VBB%2.1f_VCASN%i_ITHR%i_sec%i_pixExcl%i.pdf", 
              path_results.Data(), chip_name.Data(), vbb, vcasn, ithr, sec, n_excl_pix);
            plot_graphs(v_graphs, 1e-9, 1e-3, "Noise Occupancy [/event/pixel]", "Avg Trg Rate [Hz]", legend_title, v_legend_str, n_excl_pix, chip_name, plot_name);
        }
        else {
            plot_graphs(v_graphs, 1e-9, 1e-3, "Noise Occupancy [/event/pixel]", "Avg Trg Rate [Hz]", legend_title, v_legend_str, n_excl_pix, chip_name);
        }
    }
    if (type.CompareTo("vsBusy")==0) {
        for (Int_t i_rate=0; i_rate<n_rate; i_rate++) {
            name_graph = TString::Format("g_vsBusy_VBB%2.1f_VCASN%i_ITHR%i_RATE%i_sec%i", vbb, vcasn, ithr, rate_list[i_rate], sec);
            g = (TGraph*)f_in->Get(name_graph); 
            if (g) {
                v_graphs.push_back(g);
                v_legend_str.push_back(Form("RATE = %i Hz", rate_list[i_rate]));
            }
            else {
                cout << name_graph << endl;
                cout << "graph not found!" << endl;
                return kFALSE;
            }
        }
        if (save_plot) {
            TString plot_name = TString::Format("%s/plots/%s_vsBusy_VBB%2.1f_VCASN%i_ITHR%i_sec%i_pixExcl%i.pdf", 
              path_results.Data(), chip_name.Data(), vbb, vcasn, ithr, sec, n_excl_pix);
            plot_graphs(v_graphs, 1e-9, 1e-3, "Noise Occupancy [/event/pixel]", "Min Busy Duration [#mus]", legend_title, v_legend_str, n_excl_pix, chip_name, plot_name);
        }
        else {
            plot_graphs(v_graphs, 1e-9, 1e-3, "Noise Occupancy [/event/pixel]", "Min Busy Duration [#mus]", legend_title, v_legend_str, n_excl_pix, chip_name);
        }
    }

    return kTRUE;
} 


//---------------------------------------------------------------
Bool_t plot_graphs(vector<TGraph*> v_graphs, Float_t y_min, Float_t y_max,  TString y_title, TString x_title, TString legend_title, vector<TString> v_legend_str, Int_t n_excl_pix, TString chip_name, TString plot_name) {
    int marker1[7] = {20, 21, 22, 34, 29, 33, 23};
    bool draw_legend = true;
    if (v_graphs.size() != v_legend_str.size()) {
    	cerr << "Number of legends doesn't correspond to the number of graphs!" << endl;
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
    
    for (UInt_t i=0; i<v_graphs.size(); i++) {
    	v_graphs[i]->SetFillColor(0);
    	//v_graphs[i]->SetLineColor(2);
    	v_graphs[i]->SetLineColor(i<4?i+1:i+2); // avoid yellow
    	v_graphs[i]->SetLineStyle(i+2);
    	v_graphs[i]->SetMarkerStyle(marker1[i]);
    	v_graphs[i]->SetMarkerSize(1.5);
    	//v_graphs[i]->SetMarkerColor(2);
    	v_graphs[i]->SetMarkerColor(i<4?i+1:i+2); // avoid yellow
    	//v_graphs[i]->SetTitle("");
        v_graphs[i]->SetMinimum(y_min);
        v_graphs[i]->SetMaximum(y_max);
        v_graphs[i]->SetTitle(title);
    	v_graphs[i]->Draw(i==0?"APL":"SAMEPL");
        v_graphs[i]->GetXaxis()->SetTitle(x_title);
        v_graphs[i]->GetYaxis()->SetTitle(y_title);
    	v_graphs[i]->DrawClone(i==0?"APL":"SAMEPL");
    }

    Float_t leg_height = 0.08+v_graphs.size()*0.036;
    //TLegend *leg = new TLegend(0.51,0.946-leg_height,0.86,0.946);
    TLegend *leg = new TLegend(0.6,0.946-leg_height,0.95,0.946);
    if (draw_legend)
    {
        leg->SetTextSize(0.035);
    	leg->SetFillColor(0);
        //leg->AddEntry((TObject*)0, Form("pALPIDE-1 %s", chip_name.Data()), "");
        leg->AddEntry((TObject*)0, Form("%i hottest pixels excluded", n_excl_pix), "");
    	for (UInt_t i=0; i<v_legend_str.size(); i++) {
    		leg->AddEntry(v_graphs[i]->Clone(), v_legend_str[i].Data());
    	}
    	leg->SetHeader(legend_title);
    	leg->Draw();
    }

    if (plot_name.CompareTo("")!=0) {
        c->Print(plot_name);
    }

    return kTRUE;
}










