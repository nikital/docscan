#pragma once

#include "common.hpp"

class Controller;
class Image_editor;

struct Frame_data
{
    wxRect crop;
    string name;
    string date;
};

class Docscan_frame : public wxFrame
{
public:
    Docscan_frame (Controller& controller);

    void load_image (const string& path);
    void unload_image ();

    string show_jpeg_save_dialog (const string& name);
    void show_error_message (const string& message);

private:
    bool on_drop_files (const wxArrayString& files);
    Frame_data get_data ();
    void on_crop_update (wxNotifyEvent& e);
    void on_submit (wxCommandEvent& e);
    void reset_form ();

    Controller& controller_;
    Image_editor * editor_;
    wxTextCtrl * name_;
    wxTextCtrl * date_;

    wxDECLARE_EVENT_TABLE ();
};
