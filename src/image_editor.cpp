#include "image_editor.hpp"

wxBEGIN_EVENT_TABLE (Image_editor, wxWindow)
EVT_PAINT (Image_editor::on_paint)
wxEND_EVENT_TABLE ();


Image_editor::Image_editor (wxWindow * parent, wxWindowID id)
    : wxWindow {parent, id, wxDefaultPosition, wxSize {300, 400}}
{
    
}

void Image_editor::on_paint (wxPaintEvent& e)
{
    wxPaintDC dc {this};
    auto& b = brush_++ % 2 ? *wxCYAN_BRUSH : *wxRED_BRUSH;
    dc.SetBackground (b);
    dc.Clear ();
}
