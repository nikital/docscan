#include "docscan_frame.hpp"

#include "image_editor.hpp"
#include "controller.hpp"

#include <wx/dnd.h>
#include <sstream>
#include <iomanip>

wxBEGIN_EVENT_TABLE (Docscan_frame, wxFrame)
EVT_NOTIFY (CROP_UPDATE_EVENT, wxID_ANY, Docscan_frame::on_crop_update)
EVT_TEXT_ENTER (wxID_ANY, Docscan_frame::on_submit)
EVT_BUTTON (wxID_SAVE, Docscan_frame::on_submit)
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
    const auto input_size = wxSize {150, -1};
    name_ = new wxTextCtrl {control_panel, wxID_ANY,
                            wxEmptyString, wxDefaultPosition, input_size,
                            wxTE_PROCESS_ENTER};
    auto name_lbl = new wxStaticText {control_panel, wxID_ANY, "Name:"};
    date_ = new wxTextCtrl {control_panel, wxID_ANY,
                            wxEmptyString, wxDefaultPosition, input_size,
                            wxTE_PROCESS_ENTER};
    auto date_lbl = new wxStaticText {control_panel, wxID_ANY, "Date:"};
    auto save = new wxButton {control_panel, wxID_SAVE};

    auto multipage_lbl = new wxStaticText {control_panel, wxID_ANY, "Multipage:"};
    auto multipage_frame = new wxWindow {control_panel, wxID_ANY};
    auto multipage = new wxStaticBoxSizer {wxVERTICAL, multipage_frame};
    multipage->Add (new wxStaticText {multipage_frame, wxID_ANY, "Drop here"});
    multipage_frame->SetSizer (multipage);

    control->Add (name_lbl, 0, wxALIGN_RIGHT);
    control->Add (name_);
    control->Add (date_lbl, 0, wxALIGN_RIGHT);
    control->Add (date_);
    control->AddStretchSpacer ();
    control->Add (save);
    control->Add (multipage_lbl);
    control->Add (multipage_frame);
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
    auto new_doc_drop_target = new Drop_target {
        std::bind (&Docscan_frame::on_drop_new_doc, this, _1)};
    SetDropTarget (new_doc_drop_target);
    DragAcceptFiles (true);
    auto new_page_drop_target = new Drop_target {
        std::bind (&Docscan_frame::on_drop_new_page, this, _1)};
    multipage_frame->SetDropTarget (new_page_drop_target);
    multipage_frame->DragAcceptFiles (true);
}

void Docscan_frame::load_image (const string& path)
{
    editor_->load_image (path);
    reset_form ();
}

void Docscan_frame::unload_image ()
{
    editor_->unload_image ();
    reset_form ();
}

string Docscan_frame::show_jpeg_save_dialog (const string& name)
{
    wxFileDialog d {this, "Save scanned document",
            wxEmptyString, name,
            "JPEG file (*.jpg)|*.jpg",
            wxFD_SAVE | wxFD_OVERWRITE_PROMPT};
    d.ShowModal ();
    return d.GetPath ();
}

void Docscan_frame::show_error_message (const string& message)
{
    wxMessageDialog d {this, message, "Error", wxOK | wxICON_ERROR};
    d.ShowModal ();
}

bool Docscan_frame::on_drop_new_doc (const wxArrayString& files)
{
    if (files.empty ())
    {
        return false;
    }
    auto files_vector = std::vector<string> (std::begin(files), std::end(files));
    controller_.on_drop_files (std::move (files_vector));

    return true;
}

bool Docscan_frame::on_drop_new_page (const wxArrayString& files)
{
    if (files.empty ())
    {
        return false;
    }
    std::cout << "Yay we added "<< files[0] << "\n";

    return true;
}

Frame_data Docscan_frame::get_data ()
{
    Frame_data data;
    data.crop = editor_->get_crop ();
    data.name = name_->GetValue ();
    data.date = date_->GetValue ();
    return data;
}

void Docscan_frame::on_crop_update (wxNotifyEvent& e)
{
    name_->SetFocus ();
}

void Docscan_frame::on_submit (wxCommandEvent& e)
{
    controller_.on_submit (get_data ());
}

void Docscan_frame::reset_form ()
{
    std::stringstream current_month;
    current_month << wxDateTime::GetCurrentYear ();
    current_month << '-';
    current_month << std::setfill ('0') << std::setw (2);
    current_month << wxDateTime::GetCurrentMonth () + 1;
    current_month << '-';

    name_->Clear ();
    date_->Clear ();
    date_->SetValue (current_month.str ());
    date_->SetInsertionPointEnd ();
}
