#pragma once

#include <wx/wx.h>
#include <wx/dnd.h>

class Docscan_frame : public wxFrame, public wxFileDropTarget
{
public:
    Docscan_frame ();

private:
    bool OnDropFiles (wxCoord x, wxCoord y, const wxArrayString& files) override;
};
