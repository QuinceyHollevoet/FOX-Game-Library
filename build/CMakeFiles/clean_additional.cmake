# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\FoxGameLibrary_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\FoxGameLibrary_autogen.dir\\ParseCache.txt"
  "CMakeFiles\\Uninstaller_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\Uninstaller_autogen.dir\\ParseCache.txt"
  "CMakeFiles\\Updater_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\Updater_autogen.dir\\ParseCache.txt"
  "FoxGameLibrary_autogen"
  "Uninstaller_autogen"
  "Updater_autogen"
  )
endif()
