#set(CMAKE_VERBOSE_MAKEFILE ON)

set(CHUCK_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/core)

include_directories( 
    "${CHUCK_INCLUDE}"
)

add_subdirectory(core)
add_subdirectory(host)
add_subdirectory(host_embed)

