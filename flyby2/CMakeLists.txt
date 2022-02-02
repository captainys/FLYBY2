add_executable(FLYBY2 ASMOKE.C ASMOKE.H FLYBY.C)
target_link_libraries(FLYBY2 impulse_win32)

add_executable(FLYBY2GL ASMOKE.C ASMOKE.H FLYBY.C)
target_link_libraries(FLYBY2GL impulse_win32ogl)
