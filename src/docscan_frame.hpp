#pragma once

#include "common.hpp"

class Controller;
class Image_editor;

class Docscan_frame : public wxFrame
{
public:
    Docscan_frame (Controller& controller);

    void load_image (const string& path);

private:
    bool on_drop_files (const wxArrayString& files);

    Controller& controller_;
    Image_editor * const editor_;
};
