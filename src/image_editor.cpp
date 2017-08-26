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

void Image_editor::load_image (const string& path, int rotation)
{
    std::cout << "Loading " << path << "\n";
    source_image_ = wxImage {path, wxBITMAP_TYPE_ANY};
    switch (rotation)
    {
    case 0:
        break;
    case 90:
        source_image_ = source_image_.Rotate90 (true);
        break;
    case 270:
        source_image_ = source_image_.Rotate90 (false);
        break;
    case 180:
        source_image_ = source_image_.Rotate180 ();
        break;
    default:
        assert (0);
    }
    update_image (source_image_);

    drop_here_->Hide ();

    state_ = State::NONE;
    update_zoom ();
    emit_crop_update ();

    Refresh ();
}

void Image_editor::unload_image ()
{
    bitmap_ = nullptr;
    downsampled_bitmap_ = nullptr;
    drop_here_->Show ();
    state_ = State::NONE;

    Refresh ();
}

wxRect Image_editor::get_crop () const
{
    if (state_ == State::CROPPED)
    {
        return crop_;
    }
    return {};
}

void Image_editor::set_crop (const wxRect& crop)
{
    if (!crop.IsEmpty ())
    {
        crop_ = crop;
        state_ = State::CROPPED;
        update_zoom ();
    }
}

void Image_editor::rotate_90 (bool clockwise)
{
    source_image_ = source_image_.Rotate90 (clockwise);
    update_image (source_image_);

    state_ = State::NONE;
    update_zoom ();
    emit_crop_update ();

    Refresh ();
}

void Image_editor::update_image (const wxImage& image)
{
    image_size_ = image.GetSize ();
    bitmap_ = std::make_unique<wxBitmap> (image, wxBITMAP_TYPE_ANY);

    auto downsample_resolution = this->GetSize ().Scale (0.8, 0.8);
    if (image_size_.GetWidth () > downsample_resolution.GetWidth ()
        || image_size_.GetHeight () > downsample_resolution.GetHeight ())
    {
        auto scale_w = (float) downsample_resolution.GetWidth () / image_size_.GetWidth ();
        auto scale_h = (float) downsample_resolution.GetHeight () / image_size_.GetHeight ();
        auto scale = std::max (scale_w, scale_h);
        auto downsampled = image;
        downsampled.Rescale (
            image_size_.GetWidth () * scale, image_size_.GetHeight () * scale,
            wxIMAGE_QUALITY_HIGH);
        downsampled_bitmap_ = std::make_unique<wxBitmap> (downsampled, wxBITMAP_TYPE_ANY);
    }
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

    auto bitmap = (state_ == State::CROPPING_RECT && downsampled_bitmap_)
        ? downsampled_bitmap_.get () : bitmap_.get ();
    wxMemoryDC bitmap_dc {*bitmap};
    dc.Clear ();

    const auto image_rect = compute_image_rect ();
    dc.StretchBlit (
        image_rect.GetX (), image_rect.GetY (),
		image_rect.GetWidth (), image_rect.GetHeight (),
		&bitmap_dc,
		0, 0,
		bitmap->GetWidth (), bitmap->GetHeight ()
        );

    if (state_ == State::CROPPING_RECT)
    {
        dc.SetPen (wxPen {*wxRED, 4});
        dc.SetBrush (*wxTRANSPARENT_BRUSH);
        dc.DrawRectangle (crop_);
    }
    else if (state_ == State::CROPPED)
    {
        dc.SetPen (*wxBLACK_PEN);
        dc.SetBrush (*wxTRANSPARENT_BRUSH);
        auto crop = window_space_from_image_space (crop_, image_size_, image_rect);
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
    switch (state_)
    {
    case State::NONE:
    case State::CROPPED:
        if (e.LeftDown ())
        {
            crop_ = wxRect {e.GetPosition (), e.GetPosition ()};
            state_ = State::CROPPING_RECT;
        }
        break;
    case State::CROPPING_RECT:
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
                auto top_left = crop_.GetTopLeft ();
                auto bottom_right = crop_.GetBottomRight ();
                if (top_left.x > bottom_right.x) std::swap (top_left.x, bottom_right.x);
                if (top_left.y > bottom_right.y) std::swap (top_left.y, bottom_right.y);

                auto image_rect = compute_image_rect ();
                crop_ = image_space_from_window_space (wxRect {top_left, bottom_right},
                                                       image_size_, image_rect);
                crop_ *= wxRect {image_size_};

                state_ = State::CROPPED;
            }
            update_zoom ();
            emit_crop_update ();
            Refresh ();
        }
        break;
    }
}

wxRect Image_editor::compute_image_rect () const
{
    auto client_rect = GetClientRect ();
    auto zoom_origin = zoom_view_.GetTopLeft ();
    auto zoom_size = zoom_view_.GetSize ();

    // Calculate how much to scale the image to fit the zoom rect
    // inside the client rect
    auto scale_w = (float) client_rect.GetWidth () / zoom_size.GetWidth ();
    auto scale_h = (float) client_rect.GetHeight () / zoom_size.GetHeight ();
    auto scale = std::min (scale_w, scale_h);
    zoom_size.Scale (scale, scale);

    // Position the zoom rect in the center if the client rect
    auto zoom_rect = wxRect {zoom_size}.CenterIn (client_rect);

    // Scale other properties according to the final scale
    auto image_size = image_size_;
    image_size.Scale (scale, scale);
    zoom_origin.x *= scale;
    zoom_origin.y *= scale;

    // Calculate the image rect from the newly positioned zoom rect
    return wxRect {zoom_rect.GetTopLeft () - zoom_origin, image_size};
}

void Image_editor::update_zoom ()
{
    if (state_ == State::CROPPED)
    {
        zoom_view_ = crop_;
        const auto margin = 20;
        zoom_view_.Inflate (margin, margin);
    }
    else
    {
        zoom_view_ = wxRect {image_size_};
    }
}

void Image_editor::emit_crop_update ()
{
    wxNotifyEvent crop_event {CROP_UPDATE_EVENT};
    ProcessWindowEvent (crop_event);
}
