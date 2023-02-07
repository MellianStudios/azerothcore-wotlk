#
# This file is part of the AzerothCore Project. See AUTHORS file for Copyright information
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
#

set(SMALLFOLK_HEADERS
  smallfolk.h)

set(SMALLFOLK_SOURCES
  smallfolk.cpp)

add_library(smallfolk_cpp STATIC ${SMALLFOLK_SOURCES} ${SMALLFOLK_HEADERS})


target_include_directories(smallfolk_cpp
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(smallfolk_cpp
  PRIVATE
    acore-dependency-interface)

set_target_properties(smallfolk_cpp
  PROPERTIES
    FOLDER
      "deps")

if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
    add_definitions(-D_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
else ()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wextra -Wno-long-long -pedantic -std=c++11")
endif ()
