cmake_minimum_required (VERSION 3.6)

find_package (wxWidgets REQUIRED)
include (${wxWidgets_USE_FILE})

project (docscan)
set (CMAKE_CXX_STANDARD 14)

add_executable (docscan
  src/main.cpp
  src/docscan_frame.cpp
  src/image_editor.cpp
  )

target_link_libraries (docscan ${wxWidgets_LIBRARIES})
