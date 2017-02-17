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

    frame_->load_page (doc_->pages[0]);
}

void Controller::on_submit ()
{
    frame_->update_document (doc_.get ());
    frame_->update_page (&doc_->pages[0]);

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
    }
    else
    {
        frame_->show_error_message ("Failed to export image");
    }
}
