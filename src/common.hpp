#pragma once

#include <vector>
#include <iostream>
#include <functional>
#include <memory>
#include <wx/wx.h>

using string = wxString;

struct Page
{
    string path;
    int rotation;
    wxRect crop; // Crop coordinates are after rotation
};

struct Document
{
    string name;
    string date;
    std::vector<Page> pages;
};
