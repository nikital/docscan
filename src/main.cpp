#include <iostream>

#include <wx/wx.h>

class MyApp: public wxApp
{
public:
    bool OnInit() override;
};

class MyFrame: public wxFrame
{
public:
    MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
};

wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit()
{
    auto frame = new MyFrame {"Hello World", wxDefaultPosition, wxSize{450, 340}};
    frame->Show (true);
    return true;
}

MyFrame::MyFrame (const wxString& title, const wxPoint& pos, const wxSize& size)
    : wxFrame {NULL, wxID_ANY, title, pos, size}
{
    auto menu_bar = new wxMenuBar;
    SetMenuBar (menu_bar);
}
