#pragma once

#include "common.hpp"

class Image_editor : public wxWindow
{
public:
    Image_editor (wxWindow * parent, wxWindowID id);

private:
    void on_paint (wxPaintEvent& e);

    wxDECLARE_EVENT_TABLE ();
};
