CC=gcc
LIBS = -lm
FLAGS =
OBJECTS = jeu.o mcts.o main.o
HEADERS = jeu.h mcts.h
EXEC = main

default: $(EXEC)

%.o: %.c $(HEADERS)
	$(CC) $(FLAGS) -c $< -o $@

$(EXEC): $(OBJECTS)
	$(CC) $(OBJECTS) $(LIBS) -o $@


run: $(EXEC)
	./$(EXEC)

clean:
	rm -f $(OBJECTS)
	rm -f $(EXEC)
