all: tinychain

tinychain: tinychain.c
	gcc tinychain.c -o tinychain

clean:
	rm -rf tinychain
