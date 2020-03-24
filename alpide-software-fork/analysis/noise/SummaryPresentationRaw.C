#include "SummaryPresentationRaw.h"



// create summary presentation file with comparison plots
//---------------------------------------------------------------
Bool_t create_summary_RateBusy(TString path_results, Int_t sec, Int_t n_excl_pix) {

    // read necessary information
    //------------------------------------
    // chip name
    TString f_chip_name = path_results + "/chip_name.txt";
    ifstream f_chip(f_chip_name.Data());
    TString chip_name;
    string line;
    if(!f_chip.good()) {  
        cout << "ithr file not found, please check!" << endl;
        return kFALSE; 
    }
    else {
        getline(f_chip, line);
        cout << line << endl;
        chip_name = line;
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

    // summary presentation file
    //------------------------------------
    TString f_sum_name = TString::Format("%s/comparisons/%s_Summary_ExclPix%i_Sec%i.tex", path_results.Data(), chip_name.Data(), n_excl_pix, sec);
    ofstream f_sum(f_sum_name.Data());
    if(!f_sum.good()) {  
        cout << "sum file could not be opened, please check!" << endl;
        return kFALSE; 
    }

    // header
    add_header(f_sum);

    // title
    TString title = "pALPIDE-1 Noise Characterization\\\\ --- \\\\ Studies on effect of Trigger Rate and Busy Times";
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

    for (UInt_t i_vbb=0; i_vbb<vbb_list.size(); i_vbb++) {
        cout << "vbb: " << vbb_list[i_vbb] << endl;

        section_title = TString::Format("pALPIDE-1 %s, Sector %i, VBB %2.1f\\,V", chip_name.Data(), sec, vbb_list[i_vbb]);
        add_section(f_sum, section_title, kTRUE, kTRUE);

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
            cout << " vcasn: " << vcasn_list[i_vcasn] << endl;

            for (UInt_t i_ithr=0; i_ithr<ithr_list.size(); i_ithr++) {
                cout << "  ithr: " << ithr_list[i_ithr] << endl;

                subsection_title = TString::Format("VCASN%i, ITHR%i", vcasn_list[i_vcasn], ithr_list[i_ithr]);
                add_subsection(f_sum, subsection_title, kTRUE, kTRUE);

                plot_name = TString::Format("%s/plots/%s_vsBusy_VBB%2.1f_VCASN%i_ITHR%i_sec%i_pixExcl%i.pdf", 
                  path_results.Data(), chip_name.Data(), vbb_list[i_vbb], vcasn_list[i_vcasn], ithr_list[i_ithr], sec, n_excl_pix);
                slide_title = "Noise Occupancy vs Min Busy";
                slide_subtitle = TString::Format("VBB%2.1f, VCASN%i, ITHR%i, sec%i, pixExcl%i", 
                  vbb_list[i_vbb], vcasn_list[i_vcasn], ithr_list[i_ithr], sec, n_excl_pix);

                //add_slide_plot(f_sum, slide_title, slide_subtitle, plot_name);
                plot_name = plot_name(0, plot_name.Length()-4);
                add_slide_plot(f_sum, slide_title, slide_subtitle, Form("{%s}.pdf", plot_name.Data()));


                plot_name = TString::Format("%s/plots/%s_vsRate_VBB%2.1f_VCASN%i_ITHR%i_sec%i_pixExcl%i.pdf", 
                  path_results.Data(), chip_name.Data(), vbb_list[i_vbb], vcasn_list[i_vcasn], ithr_list[i_ithr], sec, n_excl_pix);
                slide_title = "Noise Occupancy vs Avg Trigger Rate";
                slide_subtitle = TString::Format("VBB%2.1f, VCASN%i, ITHR%i, sec%i, pixExcl%i", 
                  vbb_list[i_vbb], vcasn_list[i_vcasn], ithr_list[i_ithr], sec, n_excl_pix);

                plot_name = plot_name(0, plot_name.Length()-4);
                add_slide_plot(f_sum, slide_title, slide_subtitle, Form("{%s}.pdf", plot_name.Data()));

            }
        } 
    }
    //add_trailer(f_sum);
    add_enddocument(f_sum);
    f_sum.close();


    return kTRUE;
}



// create summary presentation file with comparison plots
//---------------------------------------------------------------
Bool_t create_summary_Temp(TString path_results, Int_t sec, Int_t comparison_select) {

    // lists of 'settings'
    const Int_t n_vbb = 2;
    Float_t vbb_list[n_vbb] = {0, 3};

    const Int_t n_vcasn = 2;
    Int_t vcasn_list[n_vcasn] = {57, 135};

    const Int_t n_ithr = 5;
    Int_t ithr_list[n_ithr] = {10, 20, 30, 51, 70};

    const Int_t n_chips = 4;
    TString chip_list[n_chips] = {"W1-25", "W2-31", "W9-16", "W9-38"};

    // summary presentation file
    //------------------------------------
    TString f_sum_name = TString::Format("%s/vsTemp/vsTemp_Summary_Sec%i.tex", path_results.Data(), sec);
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
        title = "pALPIDE-1 Noise Characterization\\\\ --- \\\\ Studies on influence of Temperature";
    }
    else if (comparison_select==1) { // threshold
        title = "pALPIDE-1 Threshold Characterization\\\\ --- \\\\ Studies on influence of Temperature";
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

    for (Int_t i_chip=0; i_chip<n_chips; i_chip++) {
        cout << "chip: " << chip_list[i_chip] << endl;

        section_title = TString::Format("pALPIDE-1 %s, Sector %i", chip_list[i_chip].Data(), sec);
        add_section(f_sum, section_title, kTRUE, kTRUE);

        for (Int_t i_vbb=0; i_vbb<n_vbb; i_vbb++) {
            cout << "  vbb: " << vbb_list[i_vbb] << "\t vcasn: " << vcasn_list[i_vbb] << endl;
        
            subsection_title = TString::Format("VBB %2.1f V", vbb_list[i_vbb]);
            add_subsection(f_sum, subsection_title, kTRUE, kTRUE);

            for (Int_t i_ithr=0; i_ithr<n_ithr; i_ithr++) {
                cout << "  ithr: " << ithr_list[i_ithr] << endl;

                //subsection_title = TString::Format("ITHR %2.1f V", ithr_list[i_vbb]);
                //add_subsubsection(f_sum, subsection_title, kTRUE, kTRUE);

                plot_name = TString::Format("%s/vsTemp/%s/VBB-%2.1f/sec_%i/%s_vsTemp_VBB%2.1f_VCASN%i_ITHR%i_sec%i.pdf", 
                  path_results.Data(), chip_list[i_chip].Data(), vbb_list[i_vbb], sec, chip_list[i_chip].Data(), vbb_list[i_vbb], vcasn_list[i_vbb], ithr_list[i_ithr], sec);
                if (file_exists(plot_name)) {
                    if (comparison_select==0) { // noise 
                        slide_title = "Noise Occupancy for different Temperatures";
                    }
                    else if (comparison_select==1) { // threshold
                        slide_title = "Threshold distribution for different Temperatures";
                    } 
                    else {
                        cout << "selected comparison key not found, please check!" << endl;
                        return kFALSE;
                    }
                    slide_subtitle = TString::Format("ITHR%i, sec%i", 
                      ithr_list[i_ithr], sec);

                    //add_slide_plot(f_sum, slide_title, slide_subtitle, plot_name);
                    plot_name = plot_name(0, plot_name.Length()-4);
                    add_slide_plot(f_sum, slide_title, slide_subtitle, Form("{%s}.pdf", plot_name.Data()));
                    
                }
                else {
                    cout << "file does not exist" << endl;
                }
            }
        }
    }
    //add_trailer(f_sum);
    add_enddocument(f_sum);
    f_sum.close();


    return kTRUE;
}


// create summary presentation file with comparison plots
//---------------------------------------------------------------
Bool_t create_summary_VBB(TString path_results, Int_t sec, Int_t comparison_select) {

    // lists of 'settings'
    //const Int_t n_vbb = 2;
    //Float_t vbb_list[n_vbb] = {0, 3};

    //const Int_t n_vcasn = 2;
    //Int_t vcasn_list[n_vcasn] = {57, 135};

    const Int_t n_temp = 3;
    Int_t temp_list[n_temp] = {12, 20, 28};

    const Int_t n_ithr = 5;
    Int_t ithr_list[n_ithr] = {10, 20, 30, 51, 70};

    const Int_t n_chips = 7;
    TString chip_list[n_chips] = {"W1-25", "W2-31", "W9-16", "W9-38", "W6-6", "W5-25", "W5-21"};

    // summary presentation file
    //------------------------------------
    TString f_sum_name = TString::Format("%s/vsVBB/vsVBB_Summary_Sec%i.tex", path_results.Data(), sec);
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
        title = "pALPIDE-1 Noise Characterization\\\\ --- \\\\ Studies on influence of VBB";
    }
    else if (comparison_select==1) { // threshold
        title = "pALPIDE-1 Threshold Characterization\\\\ --- \\\\ Studies on influence of VBB";
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

    for (Int_t i_chip=0; i_chip<n_chips; i_chip++) {
        cout << "chip: " << chip_list[i_chip] << endl;

        section_title = TString::Format("pALPIDE-1 %s, Sector %i", chip_list[i_chip].Data(), sec);
        add_section(f_sum, section_title, kTRUE, kTRUE);

        for (Int_t i_temp=0; i_temp<n_temp; i_temp++) {
            cout << " temp: " << temp_list[i_temp] << endl;

            subsection_title = TString::Format("TEMP %i.0", temp_list[i_temp]);
            add_subsection(f_sum, subsection_title, kTRUE, kTRUE);

            for (Int_t i_ithr=0; i_ithr<n_ithr; i_ithr++) {
                cout << "  ithr: " << ithr_list[i_ithr] << endl;

                plot_name = TString::Format("%s/vsVBB/%s/TEMP-%i.0/sec_%i/%s_vsVBB_TEMP%i.0_ITHR%i_sec%i.pdf", 
                  path_results.Data(), chip_list[i_chip].Data(), temp_list[i_temp], sec, chip_list[i_chip].Data(), temp_list[i_temp], ithr_list[i_ithr], sec);
                if (file_exists(plot_name)) {
                    if (comparison_select==0) { // noise 
                        slide_title = "Noise Occupancy for different VBB";
                    }
                    else if (comparison_select==1) { // threshold
                        slide_title = "Threshold distribution for different VBB";
                    } 
                    else {
                        cout << "selected comparison key not found, please check!" << endl;
                        return kFALSE;
                    }
                    slide_subtitle = TString::Format("ITHR%i, sec%i", 
                      ithr_list[i_ithr], sec);

                    //add_slide_plot(f_sum, slide_title, slide_subtitle, plot_name);
                    plot_name = plot_name(0, plot_name.Length()-4);
                    add_slide_plot(f_sum, slide_title, slide_subtitle, Form("{%s}.pdf", plot_name.Data()));
                    
                }
                else {
                    cout << "file does not exist" << endl;
                }
            }
        }
    }
    //add_trailer(f_sum);
    add_enddocument(f_sum);
    f_sum.close();


    return kTRUE;
}



// create summary presentation file with comparison plots
//---------------------------------------------------------------
Bool_t create_summary_ITHR(TString path_results, Int_t sec, Int_t comparison_select) {

    // lists of 'settings'
    const Int_t n_temp = 3;
    Int_t temp_list[n_temp] = {12, 20, 28};

    const Int_t n_vbb = 2;
    Float_t vbb_list[n_vbb] = {0, 3};

    const Int_t n_vcasn = 2;
    Int_t vcasn_list[n_vcasn] = {57, 135};

    //const Int_t n_ithr = 5;
    //Int_t ithr_list[n_ithr] = {10, 20, 30, 51, 70};

    const Int_t n_chips = 7;
    TString chip_list[n_chips] = {"W1-25", "W2-31", "W9-16", "W9-38", "W6-6", "W5-25", "W5-21"};

    // summary presentation file
    //------------------------------------
    TString f_sum_name = TString::Format("%s/vsITHR/vsITHR_Summary_Sec%i.tex", path_results.Data(), sec);
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
        title = "pALPIDE-1 Noise Characterization\\\\ --- \\\\ Studies on influence of ITHR";
    }
    else if (comparison_select==1) { // threshold
        title = "pALPIDE-1 Threshold Characterization\\\\ --- \\\\ Studies on influence of ITHR";
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


    for (Int_t i_temp=0; i_temp<n_temp; i_temp++) {
        cout << "temp: " << temp_list[i_temp] << endl;

        section_title = TString::Format("pALPIDE-1, Sector %i, TEMP %i.0", sec, temp_list[i_temp]);
        add_section(f_sum, section_title, kTRUE, kTRUE);

        for (Int_t i_vbb=0; i_vbb<n_vbb; i_vbb++) {
            cout << " vbb: " << vbb_list[i_vbb] << endl;

            subsection_title = TString::Format("VBB %2.1f\\,V", vbb_list[i_vbb]);
            add_subsection(f_sum, subsection_title, kTRUE, kTRUE);

            for (Int_t i_chip=0; i_chip<n_chips; i_chip++) {
                cout << "  chip: " << chip_list[i_chip] << endl;

                //subsection_title = TString::Format("VCASN%i, ITHR%i", vcasn_list[i_vbb], ithr_list[i_ithr]);
                //add_subsubsection(f_sum, subsection_title, kTRUE, kTRUE);

                plot_name = TString::Format("%s/vsITHR/%s/TEMP-%i.0/sec_%i/%s_vsITHR_TEMP%i.0_VBB%2.1f_VCASN%i_sec%i.pdf", 
                  path_results.Data(), chip_list[i_chip].Data(), temp_list[i_temp], sec, chip_list[i_chip].Data(), temp_list[i_temp], vbb_list[i_vbb], vcasn_list[i_vbb], sec);
                if (file_exists(plot_name)) {
                    if (comparison_select==0) { // noise 
                        slide_title = "Noise Occupancy for different ITHR";
                    }
                    else if (comparison_select==1) { // threshold
                        slide_title = "Threshold distribution for different ITHR";
                    } 
                    else {
                        cout << "selected comparison key not found, please check!" << endl;
                        return kFALSE;
                    }
                    slide_subtitle = TString::Format("VBB%2.1f, VCASN%i, sec%i", 
                      vbb_list[i_vbb], vcasn_list[i_vbb], sec);

                    //add_slide_plot(f_sum, slide_title, slide_subtitle, plot_name);
                    plot_name = plot_name(0, plot_name.Length()-4);
                    add_slide_plot(f_sum, slide_title, slide_subtitle, Form("{%s}.pdf", plot_name.Data()));
                    
                }
                else {
                    cout << "file does not exist" << endl;
                }
            }
        }
    }
    //add_trailer(f_sum);
    add_enddocument(f_sum);
    f_sum.close();


    return kTRUE;
}




// create summary presentation file with comparison plots
//---------------------------------------------------------------
Bool_t create_summary_Chips(TString path_results, Int_t sec, Int_t comparison_select) {

    // lists of 'settings'
    const Int_t n_irrad = 2;
    Float_t irrad_list[n_irrad] = {0, 1e13};

    const Int_t n_temp = 3;
    Int_t temp_list[n_temp] = {12, 20, 28};

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
    TString f_sum_name = TString::Format("%s/vsChips/vsChip_Summary_Sec%i.tex", path_results.Data(), sec);
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
        title = "pALPIDE-1 Noise Characterization\\\\ --- \\\\ Studies on Chip to Chip Variations";
    }
    else if (comparison_select==1) { // threshold
        title = "pALPIDE-1 Threshold Characterization\\\\ --- \\\\ Studies on Chip to Chip Variations";
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

    for (Int_t i_irrad=0; i_irrad<n_irrad; i_irrad++) {
        cout << "irrad: " << irrad_list[i_irrad] << endl;

        section_title = TString::Format("pALPIDE-1, Sector %i, IRRAD %2.1e", sec, irrad_list[i_irrad]);
        add_section(f_sum, section_title, kTRUE, kTRUE);

        for (Int_t i_temp=0; i_temp<n_temp; i_temp++) {
            cout << "temp: " << temp_list[i_temp] << endl;

            subsection_title = TString::Format("TEMP %i.0", temp_list[i_temp]);
            add_subsection(f_sum, subsection_title, kTRUE, kTRUE);

            for (Int_t i_vbb=0; i_vbb<n_vbb; i_vbb++) {
                cout << "vbb: " << vbb_list[i_vbb] << endl;

                subsection_title = TString::Format("VBB %2.1f\\,V", vbb_list[i_vbb]);
                add_subsubsection(f_sum, subsection_title, kTRUE, kTRUE);

                for (Int_t i_ithr=0; i_ithr<n_ithr; i_ithr++) {
                    cout << "  ithr: " << ithr_list[i_ithr] << endl;

                    //subsection_title = TString::Format("VCASN%i, ITHR%i", vcasn_list[i_vbb], ithr_list[i_ithr]);
                    //add_subsection(f_sum, subsection_title, kTRUE, kTRUE);

                    plot_name = TString::Format("%s/vsChips/IRRAD-%2.1e/TEMP-%i.0/sec_%i/vsChips_IRRAD%2.1e_TEMP%i.0_VBB%2.1f_VCASN%i_ITHR%i_sec%i.pdf", 
                      path_results.Data(), irrad_list[i_irrad], temp_list[i_temp], sec, irrad_list[i_irrad], temp_list[i_temp], vbb_list[i_vbb], vcasn_list[i_vbb], ithr_list[i_ithr], sec);
                    if (file_exists(plot_name)) {
                        if (comparison_select==0) { // noise 
                            slide_title = "Noise Occupancy for different Chips";
                        }
                        else if (comparison_select==1) { // threshold
                            slide_title = "Threshold distribution for different Chips";
                        } 
                        else {
                            cout << "selected comparison key not found, please check!" << endl;
                            return kFALSE;
                        }
                        slide_subtitle = TString::Format("ITHR%i, IRRAD%2.1e, TEMP%i.0,  VBB%2.1f, VCASN%i, sec%i", 
                          ithr_list[i_ithr], irrad_list[i_irrad], temp_list[i_temp], vbb_list[i_vbb], vcasn_list[i_vbb], sec);

                        //add_slide_plot(f_sum, slide_title, slide_subtitle, plot_name);
                        plot_name = plot_name(0, plot_name.Length()-4);
                        add_slide_plot(f_sum, slide_title, slide_subtitle, Form("{%s}.pdf", plot_name.Data()));
                        
                    }
                    else {
                        cout << plot_name << endl;
                        cout << "file does not exist" << endl;
                    }
                }
            }
        }
    }
    //add_trailer(f_sum);
    add_enddocument(f_sum);
    f_sum.close();


    return kTRUE;
}


// create summary presentation file with comparison plots
//---------------------------------------------------------------
Bool_t create_summary_Irrad(TString path_results, Int_t sec, Int_t comparison_select) {

    // lists of 'settings'
    //const Int_t n_irrad = 2;
    //Float_t irrad_list[n_irrad] = {0, 1e13};

    const Int_t n_temp = 3;
    Int_t temp_list[n_temp] = {12, 20, 28};

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
    TString f_sum_name = TString::Format("%s/vsIrrad/vsIrrad_Summary_Sec%i.tex", path_results.Data(), sec);
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
        title = "pALPIDE-1 Noise Characterization\\\\ --- \\\\ Studies on Irradiation Level Effects";
    }
    else if (comparison_select==1) { // threshold
        title = "pALPIDE-1 Threshold Characterization\\\\ --- \\\\ Studies on Irradiation Level Effects";
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


    for (Int_t i_temp=0; i_temp<n_temp; i_temp++) {
        cout << "temp: " << temp_list[i_temp] << endl;

        section_title = TString::Format("pALPIDE-1, Sector %i, TEMP %i.0", sec, temp_list[i_temp]);
        add_section(f_sum, section_title, kTRUE, kTRUE);

        for (Int_t i_vbb=0; i_vbb<n_vbb; i_vbb++) {
            cout << "vbb: " << vbb_list[i_vbb] << endl;

            subsection_title = TString::Format("VBB %2.1f\\,V", vbb_list[i_vbb]);
            add_subsection(f_sum, subsection_title, kTRUE, kTRUE);

            for (Int_t i_ithr=0; i_ithr<n_ithr; i_ithr++) {
                cout << "  ithr: " << ithr_list[i_ithr] << endl;

                //subsection_title = TString::Format("VCASN%i, ITHR%i", vcasn_list[i_vbb], ithr_list[i_ithr]);
                //add_subsection(f_sum, subsection_title, kTRUE, kTRUE);

                plot_name = TString::Format("%s/vsIrrad/TEMP-%i.0/sec_%i/vsIrrad_TEMP%i.0_VBB%2.1f_VCASN%i_ITHR%i_sec%i.pdf", 
                  path_results.Data(), temp_list[i_temp], sec, temp_list[i_temp], vbb_list[i_vbb], vcasn_list[i_vbb], ithr_list[i_ithr], sec);
                if (file_exists(plot_name)) {
                    if (comparison_select==0) { // noise 
                        slide_title = "Noise Occupancy for different Irradiation Levels";
                    }
                    else if (comparison_select==1) { // threshold
                        slide_title = "Threshold distribution for different Irradiation Levels";
                    } 
                    else {
                        cout << "selected comparison key not found, please check!" << endl;
                        return kFALSE;
                    }
                    slide_subtitle = TString::Format("ITHR%i, TEMP%i.0,  VBB%2.1f, VCASN%i, sec%i", 
                      ithr_list[i_ithr], temp_list[i_temp], vbb_list[i_vbb], vcasn_list[i_vbb], sec);

                    //add_slide_plot(f_sum, slide_title, slide_subtitle, plot_name);
                    plot_name = plot_name(0, plot_name.Length()-4);
                    add_slide_plot(f_sum, slide_title, slide_subtitle, Form("{%s}.pdf", plot_name.Data()));
                    
                }
                else {
                    cout << plot_name << endl;
                    cout << "file does not exist" << endl;
                }
            }
        }
    }
    //add_trailer(f_sum);
    add_enddocument(f_sum);
    f_sum.close();


    return kTRUE;
}






// create summary presentation file with comparison plots
//---------------------------------------------------------------
Bool_t create_summary_Hitmaps(TString path_results) {

    // lists of 'settings'
    const Int_t n_temp = 2;
    Int_t temp_list[n_temp] = {0, 3};

    const Int_t n_vbb = 2;
    Float_t vbb_list[n_vbb] = {0, 3};

    const Int_t n_vcasn = 2;
    Int_t vcasn_list[n_vcasn] = {57, 135};

    const Int_t n_ithr = 2;
    //Int_t ithr_list[n_ithr] = {10, 20, 30, 51, 70};
    Int_t ithr_list[n_ithr] = {20, 51};

    const Int_t n_chips = 2;
    TString chip_list[n_chips] = {"W2-31", "W9-16"};

    TString folder_list[n_chips]; //{"W2-31", "W9-16"};
      folder_list[0] = "NoiseVsRate_150402_1009"; // W2-31
      folder_list[1] = "NoiseVsRate_150402_1726"; // W9-16

    const Int_t n_rate = 3;
    Int_t rate_list[n_rate] = {1000, 10000, 50000};

    const Int_t n_busy = 5;
    Int_t busy_list[n_busy] = {1, 10, 20, 50, 100};


    // summary presentation file
    //------------------------------------
    TString f_sum_name = TString::Format("%s/comparisons/hitmaps/Hitmap_Summary.tex", path_results.Data());
    ofstream f_sum(f_sum_name.Data());
    if(!f_sum.good()) {  
        cout << "sum file could not be opened, please check!" << endl;
        return kFALSE; 
    }

    // header
    add_header(f_sum);

    // title
    TString title = "pALPIDE-1 Noise Characterization\\\\ --- \\\\ Hit map Summary";
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

    for (Int_t i_temp=0; i_temp<n_temp; i_temp++) {
        cout << "temp: " << temp_list[i_temp] << endl;

        for (Int_t i_vbb=0; i_vbb<n_vbb; i_vbb++) {
            cout << " vbb: " << vbb_list[i_vbb] << endl;

            section_title = TString::Format("pALPIDE-1, TEMP %i.0,  VBB %2.1f\\,V", temp_list[i_temp], vbb_list[i_vbb]);
            add_section(f_sum, section_title, kTRUE, kTRUE);

            for (Int_t i_busy=0; i_busy<n_busy; i_busy++) {
                cout << "  busy: " << busy_list[i_busy] << endl;

                subsection_title = TString::Format("BUSY%i", busy_list[i_busy]);
                add_subsection(f_sum, subsection_title, kTRUE, kTRUE);

                for (Int_t i_rate=0; i_rate<n_rate; i_rate++) {
                    cout << "   rate: " << rate_list[i_rate] << endl;

                    subsection_title = TString::Format("RATE%i", rate_list[i_rate]);
                    add_subsubsection(f_sum, subsection_title, kTRUE, kTRUE);

                    for (Int_t i_ithr=0; i_ithr<n_ithr; i_ithr++) {
                        cout << "    ithr: " << ithr_list[i_ithr] << endl;

                        vector<TString> v_plot_name;

                        for (Int_t i_chip=0; i_chip<n_chips; i_chip++) {
                            cout << "   chip: " << chip_list[i_chip] << endl;

                            plot_name = TString::Format("%s/%s/TEMP-%i.0/VBB-%2.1f/VCASN%i/ITHR%i/RATE%i/BUSY%i/hitmap.png", 
                              path_results.Data(), folder_list[i_chip].Data(), temp_list[i_temp], vbb_list[i_vbb], vcasn_list[i_vbb], ithr_list[i_ithr], rate_list[i_rate], busy_list[i_busy]);

                            if (file_exists(plot_name)) {
                                plot_name = plot_name(0, plot_name.Length()-4);
                                v_plot_name.push_back(Form("{%s}.png", plot_name.Data()));
                            }
                            else {
                                cout << "file does not exist" << endl;
                            }

                        }
                        if (v_plot_name.size()==4) { 
                            slide_title = "Noise Occupancy for different Chips";
                            slide_subtitle = TString::Format("ITHR%i, RATE%i, BUSY%i, VBB%2.1f, VCASN%i, TEMP%i.0", 
                              ithr_list[i_ithr], rate_list[i_rate], busy_list[i_busy], vbb_list[i_vbb], vcasn_list[i_vbb], temp_list[i_temp]);

                            add_slide_4plots(f_sum, slide_title, slide_subtitle, v_plot_name);
                        }         
                    }
                }
            }
        }
    }
    //add_trailer(f_sum);
    add_enddocument(f_sum);
    f_sum.close();


    return kTRUE;
}
