CC = run386 HCD386P
LIB = 386lib
LIBFILE = lib.lbc
CFLAGS =

TARGET = HOTAS.LIB
HEADER = hotas.h htfmt.h
USERINCDIR = ..\..\..\include
USERLIBDIR = ..\..\..\lib



OBJ = hotas.obj htclgus.obj htclprg.obj htfdlgpr.obj htfmtdlg.obj htlinpgs.obj htlinppr.obj htmbgus.obj htmbprg.obj htnbgus.obj htnbprg.obj htrepain.obj htwnd.obj htlboxpr.obj htlboxgs.obj


$(TARGET) : $(OBJ)
	del $(TARGET)
	$(LIB) @$(LIBFILE)
	copy $(TARGET) $(USERLIBDIR)
	copy $(HEADER) $(USERINCDIR)

.c.obj :
	$(CC) $*.c $(CFLAGS)

