#include "controller.hpp"

#include "docscan_frame.hpp"
#include "document_exporter.hpp"

#include <sstream>
#include <iomanip>

void Controller::init (Docscan_frame * frame)
{
    frame_ = frame;
}

void Controller::on_drop_new_documents (std::vector<string> files)
{
    load_new_document ({files[0]});
    next_documents_.clear ();
    next_documents_.insert (next_documents_.begin (), files.begin () + 1, files.end ());
}

void Controller::on_drop_new_pages (std::vector<string> files)
{
    if (!doc_)
    {
        load_new_document (files);
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

void Controller::load_new_document (std::vector<string> pages)
{
    doc_ = std::make_unique<Document> ();

    std::stringstream current_month;
    current_month << wxDateTime::GetCurrentYear ();
    current_month << '-';
    current_month << std::setfill ('0') << std::setw (2);
    current_month << wxDateTime::GetCurrentMonth () + 1;
    current_month << '-';
    doc_->date = current_month.str ();

    for (auto& page : pages)
    {
        doc_->pages.push_back (Page {page});
    }
    current_page_ = 0;

    frame_->push_document_data (*doc_, current_page_);
    frame_->load_page (doc_->pages[current_page_]);
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
        if (!next_documents_.empty ())
        {
            load_new_document ({next_documents_.front ()});
            next_documents_.erase (next_documents_.begin ());
        }
    }
    else
    {
        frame_->show_error_message ("Failed to export image");
    }
}
