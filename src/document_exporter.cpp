#include "document_exporter.hpp"

bool Document_exporter::export_jpeg (const string& output, const string& input,
                                     const wxRect& crop)
{
    wxImage source {input};
    auto sub_rect = crop.IsEmpty () ? wxRect {source.GetSize ()} : crop;
    auto cropped = source.GetSubImage (sub_rect);
    return cropped.SaveFile (output, wxBITMAP_TYPE_JPEG);
}
