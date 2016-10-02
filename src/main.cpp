#include "docscan_frame.hpp"

#include <wx/wx.h>
#include <iostream>

class Docscan_app: public wxApp
{
public:
    bool OnInit() override;
};

wxIMPLEMENT_APP(Docscan_app);

bool Docscan_app::OnInit()
{
    auto frame = new Docscan_frame {};
    frame->Show (true);
    return true;
}
