.SUFFIXES: .c .cpp .obj

TARGET = target.exg
OBJ =
DOSEXT = e:\run386
BINPATH = e:/hc386/bin
LIBPATH = e:/hc386/small



CC = $(DOSEXT) $(BINPATH)/HCD386P
LINK = $(DOSEXT) $(BINPATH)/TLINKP -STACK 98000
CFLAGS =
LNKFILE=fmt.lnk


TOWNSLIB = -lib $(LIBPATH)/impulse.lib \
           -lib $(LIBPATH)/hotas.lib \
           -lib $(LIBPATH)/skyhawk.lib \
           -lib $(LIBPATH)/aurora.lib \
           -lib $(LIBPATH)/file_dlg.lib \
           -lib $(LIBPATH)/guidbg.lib \
           -lib $(LIBPATH)/snd.lib \
           -lib $(LIBPATH)/tbios.lib \
           -lib $(LIBPATH)/gui.lib \
           -lib $(LIBPATH)/fmcfrb.sdl \
           -lib $(LIBPATH)/xld_exg.lib \
           -lib $(LIBPATH)/t_os.sdl \
           -lib $(LIBPATH)/vectcg_h.lib \
           -lib $(LIBPATH)/hcc.lib \
           -stack 98000




$(TARGET) : $(OBJ)
		$(LINK) @<<$(LNKFILE)
	$(OBJ)
	$(TOWNSLIB)
	-relexe $(TARGET)
<<




.c.obj:
	$(CC) $(CFLAGS) $*.c
