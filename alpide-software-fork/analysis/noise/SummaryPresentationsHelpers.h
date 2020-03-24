
// check if file exists
//---------------------------------------------------------------
Bool_t file_exists(TString fname) {
    if( access( fname.Data(), F_OK ) != -1 ) {
        //cout << "file exists" << endl;
        return kTRUE;
    }
    else {
        //cout << "file doesn't exist" << endl;
        return kFALSE;
    } 
}


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





//---------------------------------------------------------------
Bool_t add_header(ofstream &f) {
    f << "\\documentclass[aspectratio=43]{beamer}" << endl;
    f << "\\usepackage{cpcg}" << endl;
    f << endl;
    //f << "\\usepackage{siunitx}" << endl;
    //f << "\\sisetup{math-micro=\\mu,text-micro=$\\mu$}" << endl;
    f << endl;

    return kTRUE;
}

//---------------------------------------------------------------
Bool_t add_title(ofstream &f, TString title, TString title_footer) {
    f << "\\title[" << title_footer << "]{" << title << "}" << endl;

    return kTRUE;
}
//---------------------------------------------------------------
Bool_t add_author(ofstream &f, TString author, TString author_footer) {
    f << "\\author[" << author_footer << "]{" << author << "}" << endl;

    return kTRUE;
}
//---------------------------------------------------------------
Bool_t add_institute(ofstream &f, TString institute) {
    f << "\\institute[" << institute << "]{\\scalebox{2}{\\insertlogo}}" << endl;

    return kTRUE;
}
//---------------------------------------------------------------
Bool_t add_date(ofstream &f, TString date, TString date_footer) {
    f << "\\date[" << date_footer << "]{" << date << "}" << endl;

    return kTRUE;
}
//---------------------------------------------------------------
Bool_t add_begindocument(ofstream &f) {
    f << endl;
    f << "\\begin{document}" << endl;
    f << endl;

    return kTRUE;
}
//---------------------------------------------------------------
Bool_t add_enddocument(ofstream &f) {
    f << endl;
    f << "\\end{document}" << endl;
    f << endl;

    return kTRUE;
}
//---------------------------------------------------------------
Bool_t add_slide_tableofcontents(ofstream &f, TString title) {
    f << endl;
    f << "\\frame{ \\frametitle{" << title << "}" << endl;
    f << "\\tableofcontents[subsectionstyle=hide, subsubsectionstyle=hide]" << endl;
    f << "}" << endl;
    f << endl;

    return kTRUE;
}
//---------------------------------------------------------------
Bool_t add_section(ofstream &f, TString title, Bool_t add_to_tableofcontents, Bool_t create_tableofcontents_slide) {
    f << endl;
    if (add_to_tableofcontents) {
        f << "\\section{" << title << "}" << endl;
    }
    else {
        f << "\\section*{" << title << "}" << endl;
    }
    f << endl;
    if (create_tableofcontents_slide) {
        f << "\\frame{ \\frametitle{" << title << "}" << endl;
        f << "\\tableofcontents[sectionstyle=show/shaded,subsectionstyle=show/shaded/hide,subsubsectionstyle=show/shaded/hide]" << endl;
        f << "}" << endl;
        f << endl;
    }

    return kTRUE;
}
//---------------------------------------------------------------
Bool_t add_subsection(ofstream &f, TString title, Bool_t add_to_tableofcontents, Bool_t create_tableofcontents_slide) {
    f << endl;
    if (add_to_tableofcontents) {
        f << "\\subsection{" << title << "}" << endl;
    }
    else {
        f << "\\subsection*{" << title << "}" << endl;
    }
    f << endl;
    if (create_tableofcontents_slide) {
        f << "\\frame{ \\frametitle{" << title << "}" << endl;
        f << "\\tableofcontents[sectionstyle=show/shaded,subsectionstyle=show/shaded/hide,subsubsectionstyle=show/shaded/hide]" << endl;
        f << "}" << endl;
        f << endl;
    }

    return kTRUE;
}
//---------------------------------------------------------------
Bool_t add_subsubsection(ofstream &f, TString title, Bool_t add_to_tableofcontents, Bool_t create_tableofcontents_slide) {
    f << endl;
    if (add_to_tableofcontents) {
        f << "\\subsubsection{" << title << "}" << endl;
    }
    else {
        f << "\\subsubsection*{" << title << "}" << endl;
    }
    f << endl;
    if (create_tableofcontents_slide) {
        f << "\\frame{ \\frametitle{" << title << "}" << endl;
        f << "\\tableofcontents[sectionstyle=show/shaded,subsectionstyle=show/shaded/hide,subsubsectionstyle=show/shaded/hide]" << endl;
        f << "}" << endl;
        f << endl;
    }

    return kTRUE;
}
//---------------------------------------------------------------
Bool_t add_slide_plot(ofstream &f, TString title, TString subtitle, TString plot_name) {
    f << endl;
    f << "\\frame{ \\frametitle{" << title << "}" << endl;
    if (!subtitle.CompareTo("")==0) {
       f << "\\framesubtitle{" << subtitle << "}" << endl; 
    }
    f << "\\begin{center}" << endl;
    f << "\\includegraphics[width=0.95\\textwidth]{" << plot_name << "}" << endl;
    f << "\\end{center}" << endl;
    f << "}" << endl;
    f << endl;

    return kTRUE;
}
//---------------------------------------------------------------
Bool_t add_slide_4plots(ofstream &f, TString title, TString subtitle, vector<TString> v_plot_name) {
    f << endl;
    f << "\\frame{ \\frametitle{" << title << "}" << endl;
    if (!subtitle.CompareTo("")==0) {
       f << "\\framesubtitle{" << subtitle << "}" << endl; 
    }
    f << "\\begin{columns}" << endl;

    f << "\\column{0.5\\textwidth}" << endl;
    f << "\\begin{center}" << endl;
    f << "\\vspace{-3mm}" << endl;
    f << "\\includegraphics[width=0.99\\textwidth]{" << v_plot_name[0] << "}\\\\" << endl;
    f << "\\includegraphics[width=0.99\\textwidth]{" << v_plot_name[2] << "}" << endl;
    f << "\\end{center}" << endl;

    f << "\\column{0.5\\textwidth}" << endl;
    f << "\\begin{center}" << endl;
    f << "\\vspace{-3mm}" << endl;
    f << "\\includegraphics[width=0.99\\textwidth]{" << v_plot_name[1] << "}\\\\" << endl;
    f << "\\includegraphics[width=0.99\\textwidth]{" << v_plot_name[3] << "}" << endl;
    f << "\\end{center}" << endl;

    f << "\\end{columns}" << endl;

    f << "}" << endl;
    f << endl;

    return kTRUE;
}


//---------------------------------------------------------------
Bool_t add_title_slide(ofstream &f) {
    f << endl;
    f << "\\begin{frame}[plain]" << endl;
    f << "\\titlepage" << endl;
    f << "\\end{frame}" << endl;
    f << endl;

    return kTRUE;
}





