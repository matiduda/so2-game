CFLAGS = -lpthread -lrt -lncurses -g
SOURCE_DIR = src
INCLUDE_DIR = lib

BUILD_DIR = .
BOT_APP_NAME = bot
CLIENT_APP_NAME = client
ENEMY_APP_NAME = beast
SERVER_APP_NAME = server

CLIENT_INCLUDES = common.c client_func.c
SERVER_INCLUDES = common.c connect.c server_func.c

CLIENT_C = $(addprefix $(INCLUDE_DIR)/, $(CLIENT_INCLUDES))
SERVER_C = $(addprefix $(INCLUDE_DIR)/, $(SERVER_INCLUDES))

all: bot client enemy server

bot: $(SOURCE_DIR)/bot.c
	gcc $(SOURCE_DIR)/bot.c $(CLIENT_C) -o $(BUILD_DIR)/$(BOT_APP_NAME) $(CFLAGS)

client: $(SOURCE_DIR)/client.c
	gcc $(SOURCE_DIR)/client.c $(CLIENT_C) -o $(BUILD_DIR)/$(CLIENT_APP_NAME) $(CFLAGS)

enemy: $(SOURCE_DIR)/enemy.c
	gcc $(SOURCE_DIR)/enemy.c $(CLIENT_C) -o $(BUILD_DIR)/$(ENEMY_APP_NAME) $(CFLAGS)

server: $(SOURCE_DIR)/server.c
	gcc $(SOURCE_DIR)/server.c $(SERVER_C) -o $(BUILD_DIR)/$(SERVER_APP_NAME) $(CFLAGS)
	rm pipes/*

clean:
	rm $(BUILD_DIR)/$(CLIENT_APP_NAME) $(BUILD_DIR)/$(SERVER_APP_NAME)