#pragma once

class Docscan_frame;

#include "common.hpp"

class Controller
{
public:
    Controller () = default;
    Controller (Controller&) = delete;
    void init (Docscan_frame * frame);

    void on_drop_new_documents (std::vector<string> files);
    void on_drop_new_pages (std::vector<string> files);

    void on_submit ();
    void on_next_page ();
    void on_prev_page ();
    void on_delete_page ();

private:
    Docscan_frame * frame_ = nullptr;

    std::unique_ptr<Document> doc_;
    int current_page_ = 0;
};
