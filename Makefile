CC = gcc
SRC = $(shell find -name '*.c')
OBJ = $(SRC:.c=.o)
TARGET = Jogo-AED
CFLAGS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

$(TARGET): $(OBJ)
	$(CC) -o $(TARGET) $(OBJ) $(CFLAGS)
	chmod +x $(TARGET)

%.o: %.c
	$(CC) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

all: $(TARGET)

run: all
	clear
	./$(TARGET)