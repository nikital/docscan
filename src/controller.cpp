#include "controller.hpp"

#include "docscan_frame.hpp"

void Controller::init (Docscan_frame * frame)
{
    frame_ = frame;
}

void Controller::on_drop_files (std::vector<string> files)
{
    std::cout << "Dropped:\n";
    for (const auto& f : files)
    {
        std::cout << f << "\n";
    }
}
