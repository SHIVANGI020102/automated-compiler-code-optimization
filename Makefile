CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -O2
OBJ = main.o tac.o analyzer.o optimizer.o verifier.o cost.o orchestrator.o dataset.o logger.o csv.o

optimizer: $(OBJ)
	$(CC) $(CFLAGS) -o optimizer $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o optimizer

run: optimizer
	./optimizer

demo: optimizer
	./optimizer --demo
