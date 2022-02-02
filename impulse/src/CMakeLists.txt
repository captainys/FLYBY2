
set(WINDIBSRCS
win32dib/idummywin.c
win32dib/iwccolor.c
win32dib/iwcpolyg.c
win32dib/iwildcat.c
win32dib/iwildcat.h
win32dib/iwin95di.c
win32dib/iwinapi.c
win32dib/makefile
win32dib/wimpulse.h
)

set(WINOGLSRCS
win32ogl/iopengl.c
win32ogl/iwin32og.c
win32ogl/iwinapi.c
win32ogl/wimpulse.h
)

set(WINSCSVSRCS
winscsv/iwccolor.c
winscsv/iwcpolyg.c
winscsv/iwildcat.c
winscsv/iwildcat.h
winscsv/iwinscsv.c
)

set(WINOGLSCSVSRCS
winscsvogl/iopengl.c
winscsvogl/iwinscsvogl.c
)

set(SRCS
i2dg.c
i2dpict.c
i3dg.c
i3dg.h
i3dgbbox.c
i3dgobj.c
i3dtext.c
ibmpfile.c
icalc.c
icalctab.c
iclick.c
iclock.c
idevice.c
iepsfile.c
iepsfile.h
iepslink.c
ifield.c
ilncross.c
imalloc.ccc
imodel.c
impulse.h
ipolyg.c
ippmfile.c
iprimrc.c
irender.c
irender.h
irendlnk.c
irendplg.c
irendtri.c
iscrn.c
istrrc.c
iterrain.c
iterrain.ccc
itiffile.c
itmpheap.c
itwist.c
iutil.c
iutil.h
ivrmfile.c
ivrmfile.h
)

add_library(impulse_header impulse.h empty.c)
target_include_directories(impulse_header PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_library(impulse_win32 ${SRCS} ${WINDIBSRCS})
target_link_libraries(impulse_win32 gdi32.lib user32.lib kernel32.lib)
target_include_directories(impulse_win32 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_library(impulse_win32ogl ${SRCS} ${WINOGLSRCS})
target_link_libraries(impulse_win32ogl gdi32.lib user32.lib kernel32.lib opengl32.lib glu32.lib)
target_include_directories(impulse_win32ogl PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_library(impulse_win32scsv ${SRCS} ${WINSCSVSRCS})
target_link_libraries(impulse_win32scsv gdi32.lib user32.lib kernel32.lib SCRNSAVE.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib comctl32.lib
)
target_include_directories(impulse_win32scsv PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_library(impulse_win32scsvogl ${SRCS} ${WINOGLSCSVSRCS})
target_link_libraries(impulse_win32scsvogl gdi32.lib user32.lib kernel32.lib opengl32.lib glu32.lib SCRNSAVE.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib comctl32.lib
)
target_include_directories(impulse_win32scsvogl PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
