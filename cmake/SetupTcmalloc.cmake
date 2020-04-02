# Distributed under the MIT License.
# See LICENSE.txt for details.

find_package(TCMALLOC REQUIRED)

# Allocators should be linked as early as possible.
set(SPECTRE_LIBRARIES "${TCMALLOC_LIBRARIES};${SPECTRE_LIBRARIES}")

message(STATUS "tcmalloc libs: " ${TCMALLOC_LIBRARIES})
message(STATUS "tcmalloc incl: " ${TCMALLOC_INCLUDE_DIRS})

file(APPEND
  "${CMAKE_BINARY_DIR}/LibraryVersions.txt"
  "tcmalloc Version:  ${TCMALLOC_VERSION}\n"
  )

add_library(Tcmalloc INTERFACE IMPORTED)
set_property(TARGET Tcmalloc
  APPEND PROPERTY INTERFACE_LINK_LIBRARIES ${TCMALLOC_LIBRARIES})
set_property(TARGET Tcmalloc PROPERTY
  INTERFACE_INCLUDE_DIRECTORIES ${TCMALLOC_INCLUDE_DIRS})

set_property(
  GLOBAL APPEND PROPERTY SPECTRE_THIRD_PARTY_LIBS
  Tcmalloc
  )
