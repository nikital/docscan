#pragma once

#include "common.hpp"

wxDECLARE_EVENT (CROP_UPDATE_EVENT, wxNotifyEvent);

class Image_editor : public wxWindow
{
public:
    Image_editor (wxWindow * parent, wxWindowID id);

    void load_image (const string& path);

private:
    void on_paint (wxPaintEvent& e);
    void on_mouse (wxMouseEvent& e);
    wxRect compute_image_rect ();
    void emit_crop_update ();

    wxStaticText * const drop_here_;
    std::unique_ptr<wxBitmap> bitmap_;

    enum class State
    {
        NONE,
        CROPPING,
        CROPPED,
    } state_ = State::NONE;
    // When cropping, points are in windows space
    // When cropped, points are in image space
    wxRect crop_;

    wxDECLARE_EVENT_TABLE ();
};
