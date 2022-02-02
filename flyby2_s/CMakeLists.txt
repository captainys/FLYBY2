add_executable(flyby2_s.scr flyby2_s.c data.c data.h)
target_link_libraries(flyby2_s.scr impulse_win32scsv hotas_win32 skyhawk aurora winmm)

add_executable(flyby2_s_gl.scr flyby2_s.c data.c data.h)
target_link_libraries(flyby2_s_gl.scr impulse_win32scsvogl hotas_win32 skyhawk aurora winmm)
