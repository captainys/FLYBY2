add_library(skyhawk
args.c
exprintf.c
filename.c
reldir.c
skyhawk.h
string.c
)
target_include_directories(skyhawk PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
