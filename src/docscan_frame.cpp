#include "docscan_frame.hpp"

#include "image_editor.hpp"

Docscan_frame::Docscan_frame ()
    : wxFrame {nullptr, wxID_ANY, "Docscan"}
{
    auto menu_bar = new wxMenuBar {};
    SetMenuBar (menu_bar);

    auto control = new wxFlexGridSizer {2, wxSize {10, 10}};
    auto name = new wxTextCtrl {this, wxID_ANY};
    auto name_lbl = new wxStaticText {this, wxID_ANY, "Name:"};
    auto date = new wxTextCtrl {this, wxID_ANY};
    auto date_lbl = new wxStaticText {this, wxID_ANY, "Date:"};
    auto save = new wxButton {this, wxID_SAVE};
    control->Add (name_lbl, 0, wxALIGN_RIGHT);
    control->Add (name);
    control->Add (date_lbl, 0, wxALIGN_RIGHT);
    control->Add (date);
    control->AddStretchSpacer ();
    control->Add (save);

    auto editor = new Image_editor {this, wxID_ANY};

    auto top = new wxBoxSizer {wxHORIZONTAL};
    auto top_flags = wxSizerFlags {}.Border (wxALL, 10).Expand ();
    top->Add (control, top_flags);
    top->Add (editor, top_flags.Proportion (1));

    SetSizerAndFit (top);
}
