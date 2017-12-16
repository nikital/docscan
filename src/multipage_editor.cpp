#include "multipage_editor.hpp"

#include <sstream>
#include <wx/filename.h>

static const string DROP_HERE = "Drop images here to add page\n\n";

Multipage_editor::Multipage_editor (wxWindow * parent, wxWindowID id)
    : wxWindow {parent, id}
{
    auto multipage = new wxStaticBoxSizer {wxVERTICAL, this};
    wxSizerFlags padding;
    padding.Border (wxALL, 10);

    auto buttons = new wxBoxSizer {wxHORIZONTAL};
    auto down = new wxButton {this, wxID_UP, "<", wxDefaultPosition,
                              wxDefaultSize, wxBU_EXACTFIT};
    buttons->Add (down);
    auto up = new wxButton {this, wxID_DOWN, ">", wxDefaultPosition,
                            wxDefaultSize, wxBU_EXACTFIT};
    buttons->Add (up);
    auto remove = new wxButton {this, wxID_REMOVE, "X", wxDefaultPosition,
                                wxDefaultSize, wxBU_EXACTFIT};
    buttons->Add (remove);
    multipage->Add (buttons, padding);
    buttons_ = {down, up, remove};

    multipage->AddSpacer (5);
    text_ = new wxStaticText {this, wxID_ANY, ""};
    multipage->Add (text_, padding);
    SetSizer (multipage);

    set_pages ({}, 0);
}

void Multipage_editor::set_pages (const std::vector<Page>& pages, int selected_index)
{
    std::stringstream ss;
    ss << DROP_HERE;
    if (pages.size () < 2)
    {
        for (auto btn : buttons_) btn->Disable ();
    }
    else
    {
        for (auto btn : buttons_) btn->Enable ();
    }

    if (!pages.empty ()) {
        assert (selected_index >= 0 && selected_index < pages.size ());
    }
    for (size_t i = 0; i < pages.size (); ++i)
    {
        ss << (i == selected_index ? "> " : "- ");
        wxFileName filename = pages[i].path;
        ss << filename.GetName () << "\n";
    }
    text_->SetLabel (ss.str ());
    SetInitialSize ();
}
