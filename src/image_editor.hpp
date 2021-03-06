#pragma once

#include "common.hpp"

wxDECLARE_EVENT (CROP_UPDATE_EVENT, wxNotifyEvent);

class Image_editor : public wxWindow
{
public:
    Image_editor (wxWindow * parent, wxWindowID id);

    void load_image (const string& path, int rotation);
    void unload_image ();
    wxRect get_crop () const;
    void set_crop (const wxRect& crop);
    void rotate_90 (bool clockwise);

private:
    void update_image (const wxImage& image);
    void on_paint (wxPaintEvent& e);
    void on_mouse (wxMouseEvent& e);
    void update_zoom ();
    void emit_crop_update ();

    wxRect compute_image_rect () const;

    wxStaticText * const drop_here_;
    wxImage source_image_;
    std::unique_ptr<wxBitmap> bitmap_;
    // Used when user crops, to speed up rendering
    std::unique_ptr<wxBitmap> downsampled_bitmap_;
    wxSize image_size_;

    enum class State
    {
        NONE,
        CROPPING_RECT,
        CROPPING_POINTS,
        CROPPED,
    } state_ = State::NONE;
    // When cropping, points are in windows space
    // When cropped, points are in image space
    wxRect crop_;
    wxPoint crop_points_[4];
    int crop_points_count_;
    wxRect zoom_view_; // In image space

    wxDECLARE_EVENT_TABLE ();
};
