#include "image_editor.hpp"

wxDEFINE_EVENT (CROP_UPDATE_EVENT, wxNotifyEvent);

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

    state_ = State::NONE;
    emit_crop_update ();

    Refresh ();
}

void Image_editor::unload_image ()
{
    bitmap_ = nullptr;
    drop_here_->Show ();

    Refresh ();
}

/**
   Transform a window-space rect to an image-space rect, given an
   image in window space that the transform is relative to.
 */
static wxRect image_space_from_window_space (
    wxRect window_space,
    wxSize image_size, wxRect image_in_window)
{
    // This is going to be off a bit because wxWidgets are stupid and
    // they add +1 to the width and height given the bounds...
    auto scale_w = (float) image_size.GetWidth () / image_in_window.GetWidth ();
    auto scale_h = (float) image_size.GetHeight () / image_in_window.GetHeight ();

    auto image_in_window_position = image_in_window.GetPosition ();
    auto window_space_position = window_space.GetPosition ();
    auto image_space_position = wxPoint (
        std::round ((window_space_position.x - image_in_window_position.x) * scale_w),
        std::round ((window_space_position.y - image_in_window_position.y) * scale_h));
    auto window_space_size = window_space.GetSize ();
    auto image_space_size = wxSize (
        std::round (window_space_size.GetWidth () * scale_w),
        std::round (window_space_size.GetHeight () * scale_h));

    return wxRect {image_space_position, image_space_size};
}

static wxRect window_space_from_image_space (
    wxRect image_space,
    wxSize image_size, wxRect image_in_window)
{
    auto scale_w = (float) image_in_window.GetWidth () / image_size.GetWidth ();
    auto scale_h = (float) image_in_window.GetHeight () / image_size.GetHeight ();

    auto image_in_window_position = image_in_window.GetPosition ();
    auto image_space_position = image_space.GetPosition ();
    auto window_space_position = wxPoint (
        std::round (image_space_position.x * scale_w + image_in_window_position.x),
        std::round (image_space_position.y * scale_h + image_in_window_position.y));
    auto image_space_size = image_space.GetSize ();
    auto window_space_size = wxSize (
        std::round (image_space_size.GetWidth () * scale_w),
        std::round (image_space_size.GetHeight () * scale_h));

    return wxRect {window_space_position, window_space_size};
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

    const auto image_rect = compute_image_rect ();
    dc.StretchBlit (
        image_rect.GetX (), image_rect.GetY (),
		image_rect.GetWidth (), image_rect.GetHeight (),
		&bitmap,
		0, 0,
		bitmap_->GetWidth (), bitmap_->GetHeight ()
        );

    if (state_ == State::CROPPING)
    {
        dc.SetPen (wxPen {*wxRED, 4});
        dc.SetBrush (*wxTRANSPARENT_BRUSH);
        dc.DrawRectangle (crop_);
    }
    else if (state_ == State::CROPPED)
    {
        dc.SetPen (*wxBLACK_PEN);
        dc.SetBrush (*wxTRANSPARENT_BRUSH);
        auto crop = window_space_from_image_space (crop_, bitmap_->GetSize (), image_rect);
        dc.DrawRectangle (crop);

        dc.SetPen (*wxTRANSPARENT_PEN);
        dc.SetBrush (wxBrush {*wxBLACK, wxBRUSHSTYLE_CROSSDIAG_HATCH });

        dc.DrawRectangle (wxRect {
                image_rect.GetTopLeft (),
                wxPoint {image_rect.GetRight (), crop.GetTop ()}});
        dc.DrawRectangle (wxRect {
                wxPoint {image_rect.GetLeft (), crop.GetBottom ()},
                image_rect.GetBottomRight ()});
        dc.DrawRectangle (wxRect {
                wxPoint {image_rect.GetLeft (), crop.GetTop ()},
                crop.GetBottomLeft ()});
        dc.DrawRectangle (wxRect {
                crop.GetTopRight (),
                wxPoint {image_rect.GetRight (), crop.GetBottom ()}});
    }

}

void Image_editor::on_mouse (wxMouseEvent& e)
{
    if (e.LeftDown ())
    {
        assert (state_ == State::NONE || state_ == State::CROPPED);
        crop_ = wxRect {e.GetPosition (), e.GetPosition ()};
        state_ = State::CROPPING;
    }
    if (e.Dragging ())
    {
        Refresh ();
        crop_.SetBottomRight (e.GetPosition ());
    }
    if (e.LeftUp ())
    {
        if (crop_.GetLeft () == crop_.GetRight () || crop_.GetTop () == crop_.GetBottom ())
        {
            state_ = State::NONE;
        }
        else
        {
            state_ = State::CROPPED;

            auto top_left = crop_.GetTopLeft ();
            auto bottom_right = crop_.GetBottomRight ();
            if (top_left.x > bottom_right.x) std::swap (top_left.x, bottom_right.x);
            if (top_left.y > bottom_right.y) std::swap (top_left.y, bottom_right.y);

            auto bitmap_size = bitmap_->GetSize ();
            auto image_rect = compute_image_rect ();
            crop_ = image_space_from_window_space (wxRect {top_left, bottom_right},
                                                   bitmap_size, image_rect);
            crop_ *= wxRect {bitmap_size};

            emit_crop_update ();
        }
        Refresh ();
    }
}

wxRect Image_editor::compute_image_rect ()
{
    auto client_rect = GetClientRect ();
    auto bitmap_size = bitmap_->GetSize ();

    auto scale_w = (float) client_rect.GetWidth () / bitmap_size.GetWidth ();
    auto scale_h = (float) client_rect.GetHeight () / bitmap_size.GetHeight ();
    auto scale = std::min (scale_w, scale_h);
    bitmap_size.Scale (scale, scale);

    return wxRect {bitmap_size}.CenterIn (client_rect);
}

void Image_editor::emit_crop_update ()
{
    wxNotifyEvent crop_event {CROP_UPDATE_EVENT};
    ProcessWindowEvent (crop_event);
}
