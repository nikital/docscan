#pragma once

class Docscan_frame;

#include "common.hpp"

class Controller
{
public:
    Controller () = default;
    Controller (Controller&) = delete;
    void init (Docscan_frame * frame);

    void on_drop_files (std::vector<string> files);

private:
    Docscan_frame * frame_ = nullptr;
};
