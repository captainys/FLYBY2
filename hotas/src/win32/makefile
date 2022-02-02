# Makefile for "Blue Impulse Library for Win95"

LIB = lib.exe
LIBFLAGS = /NOLOGO /OUT:"HOTAS32.lib"

CC = cl.exe
CFLAGS=/nologo /W3 /GX /YX /Z7 /O2 /D "WIN32" /D "_WINDOWS" /c
LISTFILE = nul

CP = copy
USERINCDIR = C:\include
USERLIBDIR = C:\lib


OBJ = htcoldlg.obj htfdlg.obj htlinput.obj htlstbox.obj htmbox.obj htnumber.obj htyesno.obj htwnd.obj


REALTARGET = $(USERLIBDIR)/hotas32.lib
TARGET = hotas32.lib
TARGETH = hotas.h

$(REALTARGET) : $(TARGET)
		$(CP) $(TARGET) $(USERLIBDIR)
		$(CP) $(TARGETH) $(USERINCDIR)


$(TARGET) : $(OBJ)
		$(LIB) @<<
	$(LIBFLAGS) $(OBJ)
<<



.c.obj:
	$(CC) $(CFLAGS) $*.c

