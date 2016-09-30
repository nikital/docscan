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
    MyFrame(const wxString& title);
};

wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit()
{
    auto frame = new MyFrame {"Hello World"};
    frame->Show (true);
    return true;
}

MyFrame::MyFrame (const wxString& title)
    : wxFrame {NULL, wxID_ANY, title}
{
    auto menu_bar = new wxMenuBar;
    SetMenuBar (menu_bar);

    auto ok = new wxButton {this, wxID_OK};
    auto cancel = new wxButton {this, wxID_CANCEL};

    auto sizer = new wxBoxSizer {wxHORIZONTAL};
    auto sizerFlags = wxSizerFlags(0).Border(wxALL, 10);
    sizer->Add (ok, sizerFlags);
    sizer->Add (cancel, sizerFlags);
    SetSizerAndFit (sizer);
}
