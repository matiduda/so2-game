CFLAGS = -lpthread -lrt -lncurses
SOURCE_DIR = src
INCLUDE_DIR = lib

BUILD_DIR = .
CLIENT_APP_NAME = client
SERVER_APP_NAME = server

CLIENT_INCLUDES = common.c client_func.c
SERVER_INCLUDES = common.c server_func.c connect.c

CLIENT_C = $(addprefix $(INCLUDE_DIR)/, $(CLIENT_INCLUDES))
SERVER_C = $(addprefix $(INCLUDE_DIR)/, $(SERVER_INCLUDES))

all: client server

client: $(SOURCE_DIR)/client.c
	gcc $(SOURCE_DIR)/client.c $(CLIENT_C) -o $(BUILD_DIR)/$(CLIENT_APP_NAME) $(CFLAGS)

server: $(SOURCE_DIR)/server.c
	gcc $(SOURCE_DIR)/server.c $(SERVER_C) -o $(BUILD_DIR)/$(SERVER_APP_NAME) $(CFLAGS)
	rm pipes/*

clean:
	rm $(BUILD_DIR)/$(CLIENT_APP_NAME) $(BUILD_DIR)/$(SERVER_APP_NAME)