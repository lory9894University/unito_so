CFLAGS = -std=c89 -Wpedantic
TARGET = SO_project

OBJ    = error_handling.h pawn.o master.o player.o shared_res.o env_operation.h


$(TARGET): $(OBJ)
	$(CC) $(OBJ) $(LDFLAGS) -o $(TARGET)


all: $(TARGET)


clean:
	rm -f *.o $(TARGET) *~

run: $(TARGET)
	./$(TARGET)
