#include "docscan_frame.hpp"

#include "image_editor.hpp"
#include "controller.hpp"

#include <wx/dnd.h>

wxBEGIN_EVENT_TABLE (Docscan_frame, wxFrame)
EVT_NOTIFY (CROP_UPDATE_EVENT, wxID_ANY, Docscan_frame::on_crop_update)
wxEND_EVENT_TABLE ();

class Drop_target : public wxFileDropTarget
{
public:
    using Callback = std::function<bool(const wxArrayString&)>;
    Drop_target (Callback callback)
        : callback_ {std::move (callback)} {}

    bool OnDropFiles (wxCoord x, wxCoord y, const wxArrayString& files) override
    {
        return callback_ (files);
    }

private:
    Callback callback_;
};

Docscan_frame::Docscan_frame (Controller& controller)
    : wxFrame {nullptr, wxID_ANY, "Docscan"},
      controller_ {controller}
{
    auto menu_bar = new wxMenuBar {};
    SetMenuBar (menu_bar);

    auto control_panel = new wxPanel {this};
    auto control = new wxFlexGridSizer {2, wxSize {10, 10}};
    name_ = new wxTextCtrl {control_panel, wxID_ANY};
    auto name_lbl = new wxStaticText {control_panel, wxID_ANY, "Name:"};
    date_ = new wxTextCtrl {control_panel, wxID_ANY};
    auto date_lbl = new wxStaticText {control_panel, wxID_ANY, "Date:"};
    auto save = new wxButton {control_panel, wxID_SAVE};
    control->Add (name_lbl, 0, wxALIGN_RIGHT);
    control->Add (name_);
    control->Add (date_lbl, 0, wxALIGN_RIGHT);
    control->Add (date_);
    control->AddStretchSpacer ();
    control->Add (save);
    control_panel->SetSizer (control);

    // initialized here and not in the initalization list because
    // wxWidgets has some bug related to focus that would steal focus
    // from the panel if the panel is not the first thing initialized
    editor_ = new Image_editor {this, wxID_ANY};

    auto top = new wxBoxSizer {wxHORIZONTAL};
    auto top_flags = wxSizerFlags {}.Border (wxALL, 10).Expand ();
    top->Add (control_panel, top_flags);
    top->Add (editor_, top_flags.Proportion (1));

    SetSizerAndFit (top);
    Maximize ();

    using namespace std::placeholders;
    auto drop_target = new Drop_target {std::bind (&Docscan_frame::on_drop_files, this, _1)};
    SetDropTarget (drop_target);
    DragAcceptFiles (true);
}

void Docscan_frame::load_image (const string& path)
{
    editor_->load_image (path);
}

bool Docscan_frame::on_drop_files (const wxArrayString& files)
{
    if (files.empty ())
    {
        return false;
    }
    auto files_vector = std::vector<string> (std::begin(files), std::end(files));
    controller_.on_drop_files (std::move (files_vector));

    return true;
}

void Docscan_frame::on_crop_update (wxNotifyEvent& e)
{
    name_->SetFocus ();
}
