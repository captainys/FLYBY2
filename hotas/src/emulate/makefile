TARGET=libhotas.a
INCLUDE=hotas.h
OBJ=htdlgs.o hotas.o

CC=gcc
CFLAGS=-I/afs/andrew.cmu.edu/usr10/soji/pub/include




.IGNORE:




$(TARGET) : $(OBJ)
	ar cru $(TARGET) $(OBJ)
	ranlib $(TARGET)

.c.obj :
	$(CC) -c $*.c $(CFLAGS)

