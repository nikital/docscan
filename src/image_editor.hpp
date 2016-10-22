#pragma once

#include "common.hpp"

class Image_editor : public wxWindow
{
public:
    Image_editor (wxWindow * parent, wxWindowID id);

    void load_image (const string& path);

private:
    void on_paint (wxPaintEvent& e);

    wxStaticText * const drop_here_;
    std::unique_ptr<wxBitmap> bitmap_;

    wxDECLARE_EVENT_TABLE ();
};
