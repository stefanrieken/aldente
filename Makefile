pasta: src/pasta.c src/core.c src/int.c
	$(CC) $^ -o $@

clean:
	rm pasta
