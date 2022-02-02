add_executable(flyby2_s.scr FLYBY2_S.c)
target_link_libraries(flyby2_s.scr impulse_win32scsv hotas_win32 skyhawk)

add_executable(flyby2_s_gl.scr FLYBY2_S.c)
target_link_libraries(flyby2_s_gl.scr impulse_win32scsvogl hotas_win32 skyhawk)
