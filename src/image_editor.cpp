#include "image_editor.hpp"

wxBEGIN_EVENT_TABLE (Image_editor, wxWindow)
EVT_PAINT (Image_editor::on_paint)
EVT_MOUSE_EVENTS (Image_editor::on_mouse)
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

    wxMemoryDC bitmap {*bitmap_.get ()};
    dc.Clear ();
    auto canvas_size = GetSize ();
    auto bitmap_size = bitmap_->GetSize ();
    auto scale_w = (float) canvas_size.GetWidth () / bitmap_size.GetWidth ();
    auto scale_h = (float) canvas_size.GetHeight () / bitmap_size.GetHeight ();
    auto scale = std::min (scale_w, scale_h);
    auto bitmap_scale_size = bitmap_size;
    bitmap_scale_size.Scale (scale, scale);
    auto dest_rect = wxRect {
        (canvas_size.GetWidth () - bitmap_scale_size.GetWidth ()) / 2,
        (canvas_size.GetHeight () - bitmap_scale_size.GetHeight ()) / 2,
		bitmap_scale_size.GetWidth (), bitmap_scale_size.GetHeight ()
    };

    // Blit scaled from center
    dc.StretchBlit (
        dest_rect.GetX (), dest_rect.GetY (),
		dest_rect.GetWidth (), dest_rect.GetHeight (),
		&bitmap,
		0, 0,
		bitmap_size.GetWidth (), bitmap_size.GetHeight ()
        );

    if (state_ == State::CROP)
    {
        dc.SetPen (*wxTRANSPARENT_PEN);
        dc.SetBrush (wxBrush {*wxBLACK, wxBRUSHSTYLE_FDIAGONAL_HATCH });

        dc.DrawRectangle (wxRect {
                wxPoint {dest_rect.GetLeft (), dest_rect.GetTop ()},
                wxPoint {dest_rect.GetRight (), crop_start_.y}});
        dc.DrawRectangle (wxRect {
                wxPoint {dest_rect.GetLeft (), crop_end_.y},
                wxPoint {dest_rect.GetRight (), dest_rect.GetBottom ()}});
        if (crop_start_.x > dest_rect.GetLeft ())
        {
            dc.DrawRectangle (wxRect {
                    wxPoint {dest_rect.GetLeft (), crop_start_.y},
                    wxPoint {crop_start_.x, crop_end_.y}});
        }
        if (crop_end_.x < dest_rect.GetRight ())
        {
            dc.DrawRectangle (wxRect {
                    wxPoint {crop_end_.x, crop_start_.y},
                    wxPoint {dest_rect.GetRight (), crop_end_.y}});
        }

        dc.SetPen (wxPen {*wxRED, 4});
        dc.SetBrush (*wxTRANSPARENT_BRUSH);
        auto size = wxSize {crop_end_.x - crop_start_.x, crop_end_.y - crop_start_.y};
        dc.DrawRectangle (crop_start_, size);
    }

}

void Image_editor::on_mouse (wxMouseEvent& e)
{
    if (e.LeftDown ())
    {
        std::cout << "Drag start\n";
        assert (state_ == State::NONE);
        crop_end_ = crop_start_ = e.GetPosition ();
        state_ = State::CROP;
    }
    if (e.Dragging ())
    {
        std::cout << "Dragging\n";
        Refresh ();
        crop_end_ = e.GetPosition ();
    }
    if (e.LeftUp ())
    {
        std::cout << "Drag complete\n";
        state_ = State::NONE;
        Refresh ();
    }
}
