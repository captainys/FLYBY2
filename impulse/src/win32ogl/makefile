# Makefile for "Blue Impulse Library for Win95/NT OpenGL Version

LIB = lib.exe
LIBFLAGS = /NOLOGO

CC = cl.exe


#DEBUG MODE
CFLAGS=/nologo /W3 /MT /Z7 /O2 /D "WIN32" /D "_WINDOWS" /D "BIACCEL3D" /c

#RELEASE MODE
#CFLAGS=/nologo /W3 /GX /YX /O2 /D "WIN32" /D "_WINDOWS" /D "BIACCEL3D" /c


LISTFILE = nul

MV = mv
CP = copy
USERINCDIR = C:\INCLUDE
USERLIBDIR = C:\LIB


WINGOBJ=iwin32og.obj iwinapi.obj iopengl.obj
OBJ = icalc.obj iscrn.obj i2dg.obj i3dg.obj i3dgobj.obj i3dgbbox.obj imodel.obj itwist.obj iterrain.obj ilncross.obj itmpheap.obj i2dpict.obj irender.obj irendplg.obj irendtri.obj irendlnk.obj itiffile.obj iepslink.obj iepsfile.obj idevice.obj iutil.obj ifield.obj iclock.obj iprimrc.obj icalctab.obj ibmpfile.obj istrrc.obj i3dtext.obj ippmfile.obj ivrmfile.obj iclick.obj ipolyg.obj

TARGET = impls32o.lib
TARGETH = ..\impulse.h
REALTARGET = $(USERLIBDIR)\$(TARGET)

$(REALTARGET) : $(TARGET)
		$(CP) $(TARGET) $(USERLIBDIR)
		$(CP) $(TARGETH) $(USERINCDIR)

$(TARGET) : $(OBJ) $(WINGOBJ)
		$(LIB) @<<
	$(LIBFLAGS) /OUT:"$(TARGET)" $(OBJ) $(WINGOBJ)
<<


icalc.obj : ../icalc.c
		$(CC) $(CFLAGS) ../icalc.c

iscrn.obj : ../iscrn.c
		$(CC) $(CFLAGS) ../iscrn.c

i2dg.obj : ../i2dg.c
		$(CC) $(CFLAGS) ../i2dg.c

i3dg.obj : ../i3dg.c
		$(CC) $(CFLAGS) ../i3dg.c

i3dgobj.obj : ../i3dgobj.c
		$(CC) $(CFLAGS) ../i3dgobj.c

i3dgbbox.obj : ../i3dgbbox.c
		$(CC) $(CFLAGS) ../i3dgbbox.c

imodel.obj : ../imodel.c
		$(CC) $(CFLAGS) ../imodel.c

itwist.obj : ../itwist.c
		$(CC) $(CFLAGS) ../itwist.c

iterrain.obj : ../iterrain.c
		$(CC) $(CFLAGS) ../iterrain.c

ilncross.obj : ../ilncross.c
		$(CC) $(CFLAGS) ../ilncross.c

itmpheap.obj : ../itmpheap.c
		$(CC) $(CFLAGS) ../itmpheap.c

i2dpict.obj : ../i2dpict.c
		$(CC) $(CFLAGS) ../i2dpict.c

iepsfile.obj : ../iepsfile.c
		$(CC) $(CFLAGS) ../iepsfile.c

iepslink.obj : ../iepslink.c
		$(CC) $(CFLAGS) ../iepslink.c

idevice.obj : ../idevice.c
		$(CC) $(CFLAGS) ../idevice.c

irender.obj : ../irender.c
		$(CC) /c /Z7 ../irender.c
#		$(CC) $(CFLAGS) ../irender.c

irendplg.obj : ../irendplg.c
		$(CC) $(CFLAGS) ../irendplg.c

irendtri.obj : ../irendtri.c
		$(CC) $(CFLAGS) ../irendtri.c

itiffile.obj : ../itiffile.c
		$(CC) $(CFLAGS) ../itiffile.c

irendlnk.obj : ../irendlnk.c
		$(CC) $(CFLAGS) ../irendlnk.c

iutil.obj : ../iutil.c
		$(CC) $(CFLAGS) ../iutil.c

ifield.obj : ../ifield.c
		$(CC) $(CFLAGS) ../ifield.c

iclock.obj : ../iclock.c
		$(CC) $(CFLAGS) ../iclock.c

iprimrc.obj : ../iprimrc.c
		$(CC) $(CFLAGS) ../iprimrc.c

icalctab.obj : ../icalctab.c
		$(CC) $(CFLAGS) ../icalctab.c

ibmpfile.obj : ../ibmpfile.c
		$(CC) $(CFLAGS) ../ibmpfile.c

istrrc.obj : ../istrrc.c
		$(CC) $(CFLAGS) ../istrrc.c

i3dtext.obj : ../i3dtext.c
		$(CC) $(CFLAGS) ../i3dtext.c

ippmfile.obj : ../ippmfile.c
		$(CC) $(CFLAGS) ../ippmfile.c

ivrmfile.obj : ../ivrmfile.c
		$(CC) $(CFLAGS) ../ivrmfile.c

iclick.obj : ../iclick.c
		$(CC) $(CFLAGS) ../iclick.c

ipolyg.obj : ../ipolyg.c
		$(CC) $(CFLAGS) ../ipolyg.c





iwin32og.obj : iwin32og.c
		$(CC) $(CFLAGS) iwin32og.c

iopengl.obj : iopengl.c
		$(CC) $(CFLAGS) iopengl.c

iwinapi.obj : iwinapi.c
		$(CC) $(CFLAGS) iwinapi.c

