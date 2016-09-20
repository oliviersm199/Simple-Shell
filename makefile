SRC := oscode.c list.c
OUT := shell
CFLAGS := -std=c99 -g

test:
	gcc $(SRC) -o $(OUT) $(CFLAGS)

clean:
	rm $(OUT)
