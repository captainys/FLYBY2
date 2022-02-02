LIB = lib.exe
LIBFLAGS = /NOLOGO

CC = cl.exe
CFLAGS=/nologo /W3 /GX /YX /Z7 /Od /D "WIN32" /D "_WINDOWS" /c
LISTFILE = nul

CP = copy
USERINCDIR = C:\include
USERLIBDIR = C:\lib


OBJ=adyna.obj adynaf.obj aurora.obj asmoke.obj apath.obj apathf.obj ashare.obj afldfile.obj

TARGET=aurora32.lib
TARGETH=aurora.h


$(TARGET) : $(OBJ)
		$(LIB) @<<
	$(LIBFLAGS) /OUT:"$(TARGET)" $(OBJ)
<<
		$(CP) $(TARGET) $(USERLIBDIR)
		$(CP) $(TARGETH) $(USERINCDIR)


.c.obj:
	$(CC) $(CFLAGS) $*.c

