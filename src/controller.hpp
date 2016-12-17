#pragma once

class Docscan_frame;
class Frame_data;

#include "common.hpp"

class Controller
{
public:
    Controller () = default;
    Controller (Controller&) = delete;
    void init (Docscan_frame * frame);

    void on_drop_files (std::vector<string> files);
    void on_submit (Frame_data data);

private:
    Docscan_frame * frame_ = nullptr;
};
