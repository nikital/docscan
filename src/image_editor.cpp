#include "image_editor.hpp"

wxBEGIN_EVENT_TABLE (Image_editor, wxWindow)
EVT_PAINT (Image_editor::on_paint)
wxEND_EVENT_TABLE ();

Image_editor::Image_editor (wxWindow * parent, wxWindowID id)
    : wxWindow {parent, id, wxDefaultPosition, wxSize {300, 400}}
{
    auto drop_here = new wxStaticText {
        this, wxID_ANY, "Drop Documents Here...",
        wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE_HORIZONTAL};

    auto top = new wxBoxSizer {wxHORIZONTAL};
    auto top_flags = wxSizerFlags {1}.Center ();
    top->Add (drop_here, top_flags);
    SetSizer (top);
}

void Image_editor::on_paint (wxPaintEvent& e)
{
    wxPaintDC dc {this};
    dc.SetBackground (*wxWHITE_BRUSH);
    dc.Clear ();
}
