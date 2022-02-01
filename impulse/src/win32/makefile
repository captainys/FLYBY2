# Makefile for "Blue Impulse Library for Win95 WinG Version"

LIB = lib.exe
LIBFLAGS = /NOLOGO

CC = cl.exe


#DEBUG MODE
#CFLAGS=/nologo /W3 /GX /YX /Z7 /O2 /D "WIN32" /D "_WINDOWS" /c

#RELEASE MODE
CFLAGS=/nologo /W3 /GX /YX /O2 /D "WIN32" /D "_WINDOWS" /c


LISTFILE = nul

MV = mv
CP = cp
USERINCDIR = C:\MSVC20\INCLUDE
USERLIBDIR = C:\MSVC20\LIB


WINGOBJ=iwin95g.obj iwildcat.obj iwcpolyg.obj iwccolor.obj
APIOBJ=iwin95a.obj
OBJ = icalc.obj iscrn.obj i2dg.obj i3dg.obj i3dgobj.obj i3dgbbox.obj imodel.obj itwist.obj iterrain.obj ilncross.obj itmpheap.obj i2dpict.obj irender.obj irendplg.obj irendtri.obj irendlnk.obj itiffile.obj iepslink.obj iepsfile.obj idevice.obj iutil.obj ifield.obj iclock.obj iprimrc.obj icalctab.obj ibmpfile.obj istrrc.obj i3dtext.obj ippmfile.obj

TARGET1 = impls32g.lib
TARGET2 = impls32a.lib
TARGETH = ..\impulse.h
REALTARGET = $(USERLIBDIR)\$(TARGET1)

$(REALTARGET) : $(TARGET1) $(TARGET2)
		$(MV) $(TARGET1) $(USERLIBDIR)
		$(MV) $(TARGET2) $(USERLIBDIR)
		$(CP) $(TARGETH) $(USERINCDIR)

$(TARGET1) : $(OBJ) $(WINGOBJ)
		$(LIB) @<<
	$(LIBFLAGS) /OUT:"$(TARGET1)" $(OBJ) $(WINGOBJ)
<<

$(TARGET2) : $(OBJ) $(APIOBJ)
		$(LIB) @<<
	$(LIBFLAGS) /OUT:"$(TARGET2)" $(OBJ) $(APIOBJ)
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
		$(CC) $(CFLAGS) ../irender.c

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





iwin95g.obj : iwin95g.c
		$(CC) $(CFLAGS) iwin95g.c

iwildcat.obj : iwildcat.c
		$(CC) $(CFLAGS) iwildcat.c

iwcpolyg.obj : iwcpolyg.c
		$(CC) $(CFLAGS) iwcpolyg.c

iwccolor.obj : iwccolor.c
		$(CC) $(CFLAGS) iwccolor.c




iwin95a.obj : iwin95a.c
		$(CC) $(CFLAGS) iwin95a.c

