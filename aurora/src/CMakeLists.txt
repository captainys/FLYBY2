add_library(aurora
adyna.c
adynaf.c
afldfile.c
apath.c
apathf.c
ashare.c
asmoke.c
aurora.c
aurora.h
)
target_link_libraries(aurora impulse_header hotas_win32 skyhawk)
target_include_directories(aurora PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
