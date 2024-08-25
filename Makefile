CC = clang
CFLAGS = -Wall -Wextra -std=c99 -Iinclude \
    -I/System/Volumes/Data/opt/homebrew/include \
    -I/opt/homebrew/opt/libmicrohttpd/include \
    -I/opt/homebrew/opt/libsodium/include \
    -I/opt/homebrew/opt/libpq/include \
    -I/opt/homebrew/opt/mysql-client/include/mysql

LDFLAGS = -L/System/Volumes/Data/opt/homebrew/lib \
    -L/opt/homebrew/opt/libmicrohttpd/lib \
    -L/opt/homebrew/opt/libsodium/lib \
    -L/opt/homebrew/opt/libpq/lib \
    -L/opt/homebrew/opt/mysql-client/lib \
    -L/System/Volumes/Data/opt/homebrew/lib \
    -lmicrohttpd -lsodium -lpq -lmysqlclient -ljwt

SRC = src/main.c src/routes.c src/db.c src/utils.c src/jwt.c
OBJ = $(SRC:.c=.o)

all: my_c_server

my_c_server: $(OBJ)
	$(CC) $(CFLAGS) -o build/$@ $^ $(LDFLAGS)

clean:
		rm -f $(OBJ) build/my_c_server