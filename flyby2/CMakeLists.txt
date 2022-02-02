add_executable(FLYBY2 ASMOKE.C ASMOKE.H FLYBY.C)
target_link_libraries(FLYBY2 impulse_win32 hotas_win32 skyhawk)

add_executable(FLYBY2GL ASMOKE.C ASMOKE.H FLYBY.C)
target_link_libraries(FLYBY2GL impulse_win32ogl hotas_win32 skyhawk)
