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
    doc_->pages.push_back (Page {files[0]});
    current_page_ = 0;

    frame_->load_page (doc_->pages[current_page_]);
    frame_->push_document_data (*doc_, current_page_);
}

void Controller::on_drop_new_pages (std::vector<string> files)
{
    if (!doc_)
    {
        on_drop_new_documents (files);
        return;
    }

    frame_->pull_document_data (doc_.get ());
    for (auto& file : files)
    {
        doc_->pages.push_back (Page {file});
    }
    frame_->push_document_data (*doc_, current_page_);
}

void Controller::on_submit ()
{
    frame_->pull_document_data (doc_.get ());
    frame_->pull_page_data (&doc_->pages[0]);

    auto path = frame_->show_jpeg_save_dialog (doc_->date + " " + doc_->name);
    if (path.empty ())
    {
        return;
    }

    auto& page = doc_->pages[0];
    auto tl = page.crop.GetTopLeft ();
    auto br = page.crop.GetBottomRight ();
    std::cout << "Crop" << page.path
              << " from " << tl.x << "x" << tl.y
              << " to " << br.x << "x" << br.y
              << " and save at " << path
              << "\n";

    auto success = Document_exporter::export_jpeg (
        path, page.path, page.crop);
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
