# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\PodcastUI_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\PodcastUI_autogen.dir\\ParseCache.txt"
  "PodcastUI_autogen"
  )
endif()
