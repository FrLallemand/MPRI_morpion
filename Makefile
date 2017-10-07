CC=gcc
FLAGS= -lm
OBJECTS = jeu.o
EXEC = jeu

default: $(EXEC)

%.o: %.c $(HEADERS)
	$(CC) -o $@ -c $< $(FLAGS)

jeu: $(OBJECTS)
	$(CC) $(OBJECTS) -o $@


run: $(EXEC)
	./$(EXEC)

clean:
	rm -f $(OBJECTS)
	rm -f $(EXEC)
