#include "docscan_frame.hpp"
#include "controller.hpp"

class Docscan_app: public wxApp
{
public:
    bool OnInit() override;

private:
    Controller controller_;
};

wxIMPLEMENT_APP(Docscan_app);

bool Docscan_app::OnInit()
{
    wxImage::AddHandler (new wxJPEGHandler);
    wxImage::AddHandler (new wxPNGHandler);
    auto frame = new Docscan_frame {controller_};
    controller_.init (frame);
    frame->Show (true);
    return true;
}
