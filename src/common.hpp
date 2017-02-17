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
    wxRect crop;
};

struct Document
{
    string name;
    string date;
    std::vector<Page> pages;
};
