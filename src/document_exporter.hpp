#pragma once

#include "common.hpp"

namespace Document_exporter
{
    bool export_jpeg (const string& output, const string& input,
                      const wxRect& crop);
}
