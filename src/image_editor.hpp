#pragma once

#include <wx/wx.h>

class Image_editor : public wxWindow
{
public:
    Image_editor (wxWindow * parent, wxWindowID id);

private:
    void on_paint (wxPaintEvent& e);

    int brush_ = 0;

    wxDECLARE_EVENT_TABLE ();
};
