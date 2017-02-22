#include "controller.hpp"

#include "docscan_frame.hpp"
#include "document_exporter.hpp"

void Controller::init (Docscan_frame * frame)
{
    frame_ = frame;
}

void Controller::on_drop_new_documents (std::vector<string> files)
{
    doc_ = std::make_unique<Document> ();
    for (auto& file : files)
    {
        doc_->pages.push_back (Page {file});
    }
    current_page_ = 0;

    frame_->push_document_data (*doc_, current_page_);
    frame_->load_page (doc_->pages[current_page_]);
}

void Controller::on_drop_new_pages (std::vector<string> files)
{
    if (!doc_)
    {
        on_drop_new_documents (files);
        return;
    }

    frame_->pull_document_data (doc_.get (), current_page_);
    auto is_last_and_cropped =
        !doc_->pages[current_page_].crop.IsEmpty ()
        && current_page_ == doc_->pages.size () - 1;
    for (auto& file : files)
    {
        doc_->pages.push_back (Page {file});
    }
    frame_->push_document_data (*doc_, current_page_);
    if (is_last_and_cropped)
    {
        on_next_page ();
    }
}

void Controller::on_next_page ()
{
    frame_->pull_document_data (doc_.get (), current_page_);
    current_page_ = std::min (static_cast<int> (doc_->pages.size ()) - 1, current_page_ + 1);
    frame_->push_document_data (*doc_, current_page_);
    frame_->load_page (doc_->pages[current_page_]);
}
void Controller::on_prev_page ()
{
    frame_->pull_document_data (doc_.get (), current_page_);
    current_page_ = std::max (0, current_page_ - 1);
    frame_->push_document_data (*doc_, current_page_);
    frame_->load_page (doc_->pages[current_page_]);
}
void Controller::on_remove_page ()
{
    frame_->pull_document_data (doc_.get (), current_page_);

    doc_->pages.erase (doc_->pages.begin () + current_page_);
    current_page_ = std::min (static_cast<int> (doc_->pages.size ()) - 1, current_page_);

    frame_->push_document_data (*doc_, current_page_);
    frame_->load_page (doc_->pages[current_page_]);
}

void Controller::on_submit ()
{
    frame_->pull_document_data (doc_.get (), current_page_);

    auto path = frame_->show_jpeg_save_dialog (doc_->date + " " + doc_->name);
    if (path.empty ())
    {
        return;
    }

    auto success = Document_exporter::export_jpeg (path, *doc_);
    if (success)
    {
        frame_->unload_page ();
        doc_ = nullptr;
    }
    else
    {
        frame_->show_error_message ("Failed to export image");
    }
}
