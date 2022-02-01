LIB = 386lib.exe
LIBFILE = lib.lbc
CC = 386 -c HCD386P
#CFLAGS = -off Floating_Point
CFLAGS =

USERINCDIR = E:/HC386/INC
USERLIBDIR = E:/HC386/SMALL
RM = del
CP = cp


OBJ = itowns.obj \
      itwgus.obj \
      i2dg.obj \
      i2dpict.obj \
      i3dg.obj \
      i3dgbbox.obj \
      i3dgobj.obj \
      ibmpfile.obj \
      icalc.obj \
      icalctab.obj \
      iclock.obj \
      idevice.obj \
      iepsfile.obj \
      iepslink.obj \
      ifield.obj \
      ilncross.obj \
      imodel.obj \
      iprimrc.obj \
      irender.obj \
      irendlnk.obj \
      irendplg.obj \
      irendtri.obj \
      iscrn.obj \
      iterrain.obj \
      itiffile.obj \
      itmpheap.obj \
      itwist.obj \
      iutil.obj \
      ippmfile.obj \
      i3dtext.obj \
      istrrc.obj \
      ivrmfile.obj \
      iclick.obj \
      ipolyg.obj



TARGET = impulse.lib

$(TARGET) : $(OBJ)
	$(RM) $(TARGET)
	$(LIB) @<<$(LIBFILE)
		$(TARGET) -create $(OBJ) accel.bin
<<
	$(CP) $(TARGET) $(USERLIBDIR)
	$(CP) ../impulse.h $(USERINCDIR)


i2dg.obj : ../i2dg.c
	$(CC) ../i2dg.c $(CFLAGS)

i2dpict.obj : ../i2dpict.c
	$(CC) ../i2dpict.c $(CFLAGS)

i3dg.obj : ../i3dg.c
	$(CC) ../i3dg.c $(CFLAGS)

i3dgbbox.obj : ../i3dgbbox.c
	$(CC) ../i3dgbbox.c $(CFLAGS)

i3dgobj.obj : ../i3dgobj.c
	$(CC) ../i3dgobj.c $(CFLAGS)

ibmpfile.obj : ../ibmpfile.c
	$(CC) ../ibmpfile.c $(CFLAGS)

icalc.obj : ../icalc.c
	$(CC) ../icalc.c $(CFLAGS)

icalctab.obj : ../icalctab.c
	$(CC) ../icalctab.c $(CFLAGS)

iclock.obj : ../iclock.c
	$(CC) ../iclock.c $(CFLAGS)

idevice.obj : ../idevice.c
	$(CC) ../idevice.c $(CFLAGS)

iepsfile.obj : ../iepsfile.c
	$(CC) ../iepsfile.c $(CFLAGS)

iepslink.obj : ../iepslink.c
	$(CC) ../iepslink.c $(CFLAGS)

ifield.obj : ../ifield.c
	$(CC) ../ifield.c $(CFLAGS)

ilncross.obj : ../ilncross.c
	$(CC) ../ilncross.c $(CFLAGS)

imodel.obj : ../imodel.c
	$(CC) ../imodel.c $(CFLAGS)

iprimrc.obj : ../iprimrc.c
	$(CC) ../iprimrc.c $(CFLAGS)

irender.obj : ../irender.c
	$(CC) ../irender.c $(CFLAGS)

irendlnk.obj : ../irendlnk.c
	$(CC) ../irendlnk.c $(CFLAGS)

irendplg.obj : ../irendplg.c
	$(CC) ../irendplg.c $(CFLAGS)

irendtri.obj : ../irendtri.c
	$(CC) ../irendtri.c $(CFLAGS)

iscrn.obj : ../iscrn.c
	$(CC) ../iscrn.c $(CFLAGS)

iterrain.obj : ../iterrain.c
	$(CC) ../iterrain.c $(CFLAGS)

itiffile.obj : ../itiffile.c
	$(CC) ../itiffile.c $(CFLAGS)

itmpheap.obj : ../itmpheap.c
	$(CC) ../itmpheap.c $(CFLAGS)

itwist.obj : ../itwist.c
	$(CC) ../itwist.c $(CFLAGS)

iutil.obj : ../iutil.c
	$(CC) ../iutil.c $(CFLAGS)

ippmfile.obj : ../ippmfile.c
	$(CC) ../ippmfile.c $(CFLAGS)

i3dtext.obj : ../i3dtext.c
	$(CC) ../i3dtext.c $(CFLAGS)

istrrc.obj : ../istrrc.c
	$(CC) ../istrrc.c $(CFLAGS)

ivrmfile.obj : ../ivrmfile.c
	$(CC) ../ivrmfile.c $(CFLAGS)

iclick.obj : ../iclick.c
	$(CC) ../iclick.c $(CFLAGS)

ipolyg.obj : ../ipolyg.c
	$(CC) ../ipolyg.c $(CFLAGS)




itowns.obj : itowns.c
		$(CC) itowns.c $(CFLAGS)

itwgus.obj : itwgus.c
		$(CC) itwgus.c $(CFLAGS)
