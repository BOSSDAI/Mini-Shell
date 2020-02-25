CFLAGS = -Wall -Werror -ggdb3
TARGET = shell
OBJECTS = main.o execute.o rep_loop.o basic.o alias.o
$(TARGET):$(OBJECTS)
	g++ $(CFLAGS) $^ -o $@
%.o: %.cpp
	g++ $(CFLAGS) -c $< -o $@
clean:
	rm *.o *~
