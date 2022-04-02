CC = g++
CFLAGS = -Wall 
LFLAGS = -lsfml-graphics -lsfml-window -lsfml-system

SRCS=snake.cpp
EXE=snake

$(EXE): $(SRCS)
	$(CC) $(CFLAGS) $^ -o $@ $(LFLAGS)

.PHONY: run
run: $(EXE)
	./$<

.PHONY: clean
clean:
	rm -f $(EXE)