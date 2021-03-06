#pragma once

#include "common.hpp"

class Multipage_editor : public wxWindow
{
public:
    Multipage_editor (wxWindow * parent, wxWindowID id);

    void set_pages (const std::vector<Page>& pages, int selected_index);

private:
    wxStaticText * text_;
    std::vector<wxButton*> buttons_;

    //wxDECLARE_EVENT_TABLE ();
};
