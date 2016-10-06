#pragma once

#include "common.hpp"

class Controller;

class Docscan_frame : public wxFrame
{
public:
    Docscan_frame (Controller& controller);

private:
    bool on_drop_files (const wxArrayString& files);

    Controller& controller_;
};
