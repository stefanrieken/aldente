pasta: src/pasta.c src/core.c src/int.c
	$(CC) -g $^ -o $@

clean:
	rm pasta
