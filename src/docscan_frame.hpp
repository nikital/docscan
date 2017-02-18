#pragma once

#include "common.hpp"

class Controller;
class Image_editor;
class Multipage_editor;

class Docscan_frame : public wxFrame
{
public:
    Docscan_frame (Controller& controller);

    void load_page (const Page& page);
    void unload_page ();

    void pull_document_data (Document * doc);
    void push_document_data (const Document& doc, int selected_index);
    void pull_page_data (Page * page);

    string show_jpeg_save_dialog (const string& name);
    void show_error_message (const string& message);

private:
    bool on_drop_new_doc (const wxArrayString& files);
    bool on_drop_new_page (const wxArrayString& files);
    void on_crop_update (wxNotifyEvent& e);
    void on_submit (wxCommandEvent& e);
    void reset_form ();

    Controller& controller_;
    Image_editor * editor_;
    Multipage_editor * multipage_;
    wxTextCtrl * name_;
    wxTextCtrl * date_;

    wxDECLARE_EVENT_TABLE ();
};
