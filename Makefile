CC_FLAGS = -g
CC = gcc $(CC_FLAGS)
OBJ = sqlite3.o
LIBS = -lm -lpthread -ldl

all : main
.PHONY : all

test_1 : main.o $(OBJ)
	$(CC) -o main main.o $(OBJ) $(LIBS)

$(OBJ) : sqlite3.h

.PHONY : clean
clean : 
	rm -rf main main.o $(OBJ)
