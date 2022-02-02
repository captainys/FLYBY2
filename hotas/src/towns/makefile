CC = 386 -c HCD386P
LIB = 386lib
LIBFILE = libfile.lib
CFLAGS =

TARGET = HOTAS.LIB
HEADER = hotas.h htfmt.h
USERINCDIR = E:\hc386\inc
USERLIBDIR = E:\hc386\small

OBJ = hotas.obj htclgus.obj htclprg.obj htfdlgpr.obj htfmtdlg.obj htlinpgs.obj htlinppr.obj htmbgus.obj htmbprg.obj htnbgus.obj htnbprg.obj htrepain.obj htwnd.obj htlboxpr.obj htlboxgs.obj


$(TARGET) : $(OBJ)
	del $(TARGET)
	$(LIB) @<<$(LIBFILE)
		$(TARGET) -create $(OBJ)
<<
	cp $(TARGET) $(USERLIBDIR)
	cp $(HEADER) $(USERINCDIR)

.c.obj :
	$(CC) $*.c $(CFLAGS)

