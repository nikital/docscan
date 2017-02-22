cmake_minimum_required (VERSION 3.6)

find_package (wxWidgets REQUIRED)
include (${wxWidgets_USE_FILE})

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project (docscan)
set (CMAKE_CXX_STANDARD 14)

add_executable (Docscan MACOSX_BUNDLE
  src/main.cpp
  src/controller.cpp
  src/docscan_frame.cpp
  src/image_editor.cpp
  src/multipage_editor.cpp
  src/document_exporter.cpp
  )

target_link_libraries (Docscan ${wxWidgets_LIBRARIES})
