CFLAGS = -lpthread -lrt -lncurses
SOURCE_DIR = src
BUILD_DIR = .
CLIENT_APP_NAME = client
SERVER_APP_NAME = server

all: client server

client: $(SOURCE_DIR)/client.c
	gcc $(SOURCE_DIR)/client.c -o $(BUILD_DIR)/$(CLIENT_APP_NAME) $(CFLAGS)

server: $(SOURCE_DIR)/server.c
	gcc $(SOURCE_DIR)/server.c -o $(BUILD_DIR)/$(SERVER_APP_NAME) $(CFLAGS)

clean:
	rm $(BUILD_DIR)/$(CLIENT_APP_NAME) $(BUILD_DIR)/$(SERVER_APP_NAME)