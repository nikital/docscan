#include "docscan_frame.hpp"

Docscan_frame::Docscan_frame ()
    : wxFrame {nullptr, wxID_ANY, "Docscan"}
{
    auto menu_bar = new wxMenuBar {};
    SetMenuBar (menu_bar);

    auto control = new wxFlexGridSizer {2, wxSize {10, 10}};
    auto name = new wxTextCtrl {this, -1};
    auto name_lbl = new wxStaticText {this, -1, "Name:"};
    auto date = new wxTextCtrl {this, -1};
    auto date_lbl = new wxStaticText {this, -1, "Date:"};
    auto save = new wxButton {this, wxID_SAVE};
    control->Add (name_lbl, 0, wxALIGN_RIGHT);
    control->Add (name);
    control->Add (date_lbl, 0, wxALIGN_RIGHT);
    control->Add (date);
    control->AddStretchSpacer ();
    control->Add (save);

    auto top = new wxBoxSizer {wxHORIZONTAL};
    auto top_flags = wxSizerFlags {}.Border (wxALL, 10);
    top->Add (control, top_flags);

    SetSizerAndFit (top);
}
