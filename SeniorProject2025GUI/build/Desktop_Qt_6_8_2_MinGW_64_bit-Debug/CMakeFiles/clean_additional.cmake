# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\SeniorProject2025GUI_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\SeniorProject2025GUI_autogen.dir\\ParseCache.txt"
  "SeniorProject2025GUI_autogen"
  )
endif()
