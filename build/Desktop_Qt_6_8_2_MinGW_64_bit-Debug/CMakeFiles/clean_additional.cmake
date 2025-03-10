# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\P2PApp_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\P2PApp_autogen.dir\\ParseCache.txt"
  "P2PApp_autogen"
  )
endif()
