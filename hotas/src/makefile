TARGET=hotas.a
OBJ=htdlgs.o hotas.o

CC=gcc
CFLAGS=-I. -I../impulse



$(TARGET) : $(OBJ)
	ar cru $(TARGET) $(OBJ)
	ranlib $(TARGET)
	mv $(TARGET) $(HOME)/pub/lib/.

.c.obj :
	$(CC) -c $*.c $(CFLAGS)

