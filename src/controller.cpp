#include "controller.hpp"

#include "docscan_frame.hpp"

void Controller::init (Docscan_frame * frame)
{
    frame_ = frame;
}

void Controller::on_drop_files (std::vector<string> files)
{
    frame_->load_image (files[0]);
}

void Controller::on_submit (Frame_data data)
{
    auto tl = data.crop.GetTopLeft ();
    auto br = data.crop.GetBottomRight ();
    std::cout << tl.x << "x" << tl.y
              << " to "
              << br.x << "x" << br.y
              << "\n";
}
