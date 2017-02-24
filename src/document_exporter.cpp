#include "document_exporter.hpp"

bool Document_exporter::export_jpeg (const string& output, Document doc)
{
    std::vector<wxImage> images;
    images.reserve (doc.pages.size ());

    auto max_width = 0;
    auto total_height = 0;
    for (auto& page : doc.pages)
    {
        wxImage image {page.path};
        if (page.crop.IsEmpty ())
        {
            page.crop = wxRect {image.GetSize ()};
            images.push_back (std::move (image));
        }
        else
        {
            images.push_back (image.GetSubImage (page.crop));
        }

        max_width = std::max (max_width, page.crop.GetWidth ());
        total_height += page.crop.GetHeight ();
    }
    
    auto output_image = wxImage {max_width, total_height};
    output_image.Clear (0xFF);
    auto current_y = 0;
    for (auto& image : images)
    {
        output_image.Paste (image, 0, current_y);
        current_y += image.GetHeight ();
    }

    return output_image.SaveFile (output, wxBITMAP_TYPE_JPEG);
}
