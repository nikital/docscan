#include "image_editor.hpp"

wxBEGIN_EVENT_TABLE (Image_editor, wxWindow)
EVT_PAINT (Image_editor::on_paint)
wxEND_EVENT_TABLE ();

Image_editor::Image_editor (wxWindow * parent, wxWindowID id)
    : wxWindow {parent, id, wxDefaultPosition, wxSize {300, 400}},
      drop_here_ {new wxStaticText {
              this, wxID_ANY, "Drop Documents Here...",
                  wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE_HORIZONTAL
                  }}
{
    auto top = new wxBoxSizer {wxHORIZONTAL};
    auto top_flags = wxSizerFlags {1}.Center ();
    top->Add (drop_here_, top_flags);
    SetSizer (top);
}

void Image_editor::load_image (const string& path)
{
    std::cout << "Loading " << path << "\n";
    bitmap_ = std::make_unique<wxBitmap> (path, wxBITMAP_TYPE_ANY);
    drop_here_->Hide ();
    Refresh ();
}


void Image_editor::on_paint (wxPaintEvent& e)
{
    wxPaintDC dc {this};
    if (!bitmap_)
    {
        dc.SetBackground (*wxWHITE_BRUSH);
        dc.Clear ();
        return;
    }

    auto canvas_size = GetSize ();
    auto bitmap_size = bitmap_->GetSize ();
    auto scale = (float) canvas_size.GetWidth () / bitmap_size.GetWidth ();
    dc.SetUserScale (scale, scale);
    dc.DrawBitmap (*bitmap_,
                   (canvas_size.GetWidth () - bitmap_size.GetWidth () * scale) / 2,
                   (canvas_size.GetHeight () - bitmap_size.GetHeight () * scale) / 2);
}
