#include "SummaryPresentation.h"


// create summary presentation file with comparison plots
//---------------------------------------------------------------
Bool_t create_summary_Irrad(TString path_results, Int_t comparison_select) {

    TString suffix="";
    TString plotslide_title="";
    if (comparison_select==0) {
        suffix="";
        plotslide_title = "Noise Occupancy for different Irradiation Levels";
    }
    else if (comparison_select==1) {
        suffix="_thres";
        plotslide_title = "Threshold for different Irradiation Levels";
    }
    else if (comparison_select==2) {
        suffix="_thresnoise";
        plotslide_title = "Threshold Noise for different Irradiation Levels";
    }
    else {
        cout << "selected comparison key not found, please check!" << endl;
        return kFALSE;
    }

    // lists of 'settings'
    //const Int_t n_irrad = 2;
    //Float_t irrad_list[n_irrad] = {0, 1e13};

    //const Int_t n_temp = 3;
    //Int_t temp_list[n_temp] = {12, 20, 28};

    const Int_t n_vbb = 2;
    Float_t vbb_list[n_vbb] = {0, 3};

    const Int_t n_vcasn = 2;
    Int_t vcasn_list[n_vcasn] = {57, 135};

    const Int_t n_ithr = 5;
    Int_t ithr_list[n_ithr] = {10, 20, 30, 51, 70};

    //const Int_t n_chips = 4;
    //TString chip_list[n_chips] = {"W1-25", "W2-31", "W9-16", "W9-38"};

    // summary presentation file
    //------------------------------------
    TString f_sum_name = TString::Format("%s/vsIrrad/vsIrrad_Summary%s.tex", path_results.Data(), suffix.Data());
    ofstream f_sum(f_sum_name.Data());
    if(!f_sum.good()) {  
        cout << "sum file could not be opened, please check!" << endl;
        return kFALSE; 
    }

    // header
    add_header(f_sum);

    // title
    TString title;
    if (comparison_select==0) { // noise 
        title = "pALPIDE-1 Noise Occupancy Characterization\\\\ --- \\\\ Studies on Irradiation Level Effects";
    }
    else if (comparison_select==1) { // threshold
        title = "pALPIDE-1 Threshold Characterization\\\\ --- \\\\ Studies on Irradiation Level Effects";
    } 
    else if (comparison_select==2) { // threshold noise
        title = "pALPIDE-1 Threshold Noise Characterization\\\\ --- \\\\ Studies on Irradiation Level Effects";
    } 
    else {
        cout << "selected comparison key not found, please check!" << endl;
        return kFALSE;
    }

    TString title_footer = "Status Report";
    add_title(f_sum, title, title_footer);
    // author
    TString author = "Jacobus van Hoorne";
    TString author_footer = "JvH";
    add_author(f_sum, author, author_footer);
    // institute
    TString institute = "CERN";
    add_institute(f_sum, institute);
    // date
    //TString date = "ITS upgrade characterisation --- March, 13th 2015";
    //TString date_footer = "ITS Char, 20/03/15";
    time_t t = time(0);   // get time now
    struct tm *now = localtime(&t);
    char date_now[20];
    sprintf(date_now, "%02d/%02d/%02d", now->tm_mday, now->tm_mon + 1, now->tm_year - 100);
    TString date = TString::Format("ITS upgrade characterisation --- %s", date_now);
    TString date_footer = TString::Format("ITS Char, %s", date_now);
    add_date(f_sum, date, date_footer);
    // begin document
    add_begindocument(f_sum);
    // title slide
    add_title_slide(f_sum);
    // tableofcontents
    TString slide_title = "Outline";
    add_slide_tableofcontents(f_sum, slide_title);

    TString slide_subtitle;
    TString section_title;
    TString subsection_title;
    TString plot_name;

    for (Int_t i_vbb=0; i_vbb<n_vbb; i_vbb++) {
        cout << "vbb: " << vbb_list[i_vbb] << endl;

        section_title = TString::Format("pALPIDE-1, VBB %2.1f", vbb_list[i_vbb]);
        add_section(f_sum, section_title, kTRUE, kTRUE);
        //subsection_title = TString::Format("VBB %2.1f\\,V", vbb_list[i_vbb]);
        //add_subsection(f_sum, subsection_title, kTRUE, kTRUE);

        for (Int_t i_ithr=0; i_ithr<n_ithr; i_ithr++) {
            cout << " ithr: " << ithr_list[i_ithr] << endl;

            //subsection_title = TString::Format("VCASN%i, ITHR%i", vcasn_list[i_vbb], ithr_list[i_ithr]);
            //add_subsection(f_sum, subsection_title, kTRUE, kTRUE);
            vector<TString> v_plot_name;

            for (Int_t i_sec=0; i_sec<4; i_sec++) {
                cout << "  sec: " << i_sec << endl;
                plot_name = TString::Format("%s/vsIrrad/sec_%i/vsIrrad%s_VBB%2.1f_VCASN%i_ITHR%i_sec%i.pdf", 
                  path_results.Data(), i_sec, suffix.Data(), vbb_list[i_vbb], vcasn_list[i_vbb], ithr_list[i_ithr], i_sec);
                if (file_exists(plot_name)) {
                    plot_name = plot_name(0, plot_name.Length()-4);
                    v_plot_name.push_back(Form("{%s}.pdf", plot_name.Data()));
                }
                else {
                    cout << plot_name << endl;
                    cout << "file does not exist" << endl;
                }
            } // sec

            if (v_plot_name.size()==4) { 
                //slide_title = "Noise Occupancy for different Irradiation Levels";
                slide_subtitle = TString::Format("ITHR%i, VBB%2.1f, VCASN%i", 
                  ithr_list[i_ithr], vbb_list[i_vbb], vcasn_list[i_vbb]);

                add_slide_4plots(f_sum, plotslide_title, slide_subtitle, v_plot_name);
            }         
        }
    }
    //add_trailer(f_sum);
    add_enddocument(f_sum);
    f_sum.close();

    return kTRUE;
}



