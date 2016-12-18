#include "controller.hpp"

#include "docscan_frame.hpp"

void Controller::init (Docscan_frame * frame)
{
    frame_ = frame;
}

void Controller::on_drop_files (std::vector<string> files)
{
    current_file_ = files[0];
    frame_->load_image (current_file_);
}

void Controller::on_submit (Frame_data data)
{
    auto path = frame_->show_jpeg_save_dialog (data.date + " " + data.name);
    if (path.empty ())
    {
        return;
    }

    auto tl = data.crop.GetTopLeft ();
    auto br = data.crop.GetBottomRight ();
    std::cout << "Crop " << current_file_
              << " from " << tl.x << "x" << tl.y
              << " to " << br.x << "x" << br.y
              << " and save at " << path
              << "\n";
}
