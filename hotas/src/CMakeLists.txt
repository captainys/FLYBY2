add_library(hotas_win32
win32/hotas.h
win32/hotasrc.ico
win32/hotasrc.rc
win32/htcoldlg.c
win32/htfdlg.c
win32/htlinput.c
win32/htlstbox.c
win32/htmbox.c
win32/htnumber.c
win32/htwnd.c
win32/htyesno.c
win32/resource.h)
target_include_directories(hotas_win32 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/win32)
target_link_libraries(hotas_win32 impulse_header)
