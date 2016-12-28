#include "document_exporter.hpp"

bool Document_exporter::export_jpeg (const string& output, const string& input,
                                     const wxRect& crop)
{
    wxImage source {input};
    auto cropped = source.GetSubImage (crop);
    return cropped.SaveFile (output, wxBITMAP_TYPE_JPEG);
}
