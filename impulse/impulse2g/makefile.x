TARGET=libimpulse2g.a
INCLUDE=impulse2g.h

CC = gcc
CFLAGS = -I/afs/andrew.cmu.edu/usr10/soji/pub/include -I.




.IGNORE:




OBJ = impulse2g.o falcon2g.o

$(TARGET) : $(OBJ)
	ar cru $(TARGET) $(OBJ)
	ranlib $(TARGET)

.c.o :
	$(CC) -c $*.c $(CFLAGS)

