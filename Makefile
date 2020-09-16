CC	= gcc

CFLAGS = -ansi -pedantic -Werror -Wall -Wextra -D_POSIX_C_SOURCE 
CFLAGS += -g3 

CPPFLAGS = $(addprefix -I,$(INC_DIR) $(DS_DIR))

LDFLAGS = $(LIB_DIR)/libChatDS.a -ldl

CLIENT_SOURCES = $(TEST_DIR)/client_app.c $(SRC_DIR)/client_manager.c $(SRC_DIR)/client_group_manager.c\
$(SRC_DIR)/client_network.c $(SRC_DIR)/client_ui.c $(SRC_DIR)/chat_protocol.c $(SRC_DIR)/signal_sender.c

SERVER_SOURCES = $(TEST_DIR)/server_app.c $(SRC_DIR)/server_manager.c $(SRC_DIR)/users_manager.c\
$(SRC_DIR)/group_manager.c $(SRC_DIR)/server_network.c $(SRC_DIR)/chat_protocol.c

READ_SOURCES = $(TEST_DIR)/chat_read.c $(SRC_DIR)/client_network.c $(SRC_DIR)/client_ui.c $(SRC_DIR)/chat_protocol.c\
$(SRC_DIR)/signal_sender.c

WRITE_SOURCES = $(TEST_DIR)/chat_write.c $(SRC_DIR)/client_network.c $(SRC_DIR)/client_ui.c $(SRC_DIR)/chat_protocol.c\
$(SRC_DIR)/signal_sender.c

SERVER_OBJS = $(SERVER_SOURCES:.c=.o)
CLIENT_OBJS = $(CLIENT_SOURCES:.c=.o)
READ_OBJS = $(READ_SOURCES:.c=.o)
WRITE_OBJS = $(WRITE_SOURCES:.c=.o)

INC_DIR = inc
SRC_DIR = src
DS_DIR = ds
LIB_DIR = lib
TEST_DIR = test

CLIENT_TARGET = $(TEST_DIR)/client_app
SERVER_TARGET = $(TEST_DIR)/server_app
READ_TARGET = $(TEST_DIR)/chat_read
WRITE_TARGET = $(TEST_DIR)/chat_write

OBJECTS = $(wildcard $(SRC_DIR)/*.o)
OBJECTS += $(wildcard $(TEST_DIR)/*.o)

all: $(CLIENT_TARGET) $(SERVER_TARGET) $(READ_TARGET) $(WRITE_TARGET) $(OBJECTS)


$(CLIENT_TARGET) : $(CLIENT_OBJS)
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $(CLIENT_TARGET) $(CLIENT_OBJS) $(LDFLAGS)

$(SERVER_TARGET) : $(SERVER_OBJS)
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $(SERVER_TARGET) $(SERVER_OBJS) $(LDFLAGS)

$(READ_TARGET) : $(READ_OBJS) 
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $(READ_TARGET) $(READ_OBJS)

$(WRITE_TARGET) : $(WRITE_OBJS)
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $(WRITE_TARGET) $(WRITE_OBJS)

server: $(SERVER_TARGET) $(SERVER_OBJS)
	./$(SERVER_TARGET) 

client: $(CLIENT_TARGET) $(CLIENT_OBJS)
	./$(CLIENT_TARGET) 

#valgrind:
#	valgrind --leak-check=full ./$(TARGET)

clean: 
	rm -f $(SERVER_OBJS) $(CLIENT_OBJS) $(OBJECTS) *.txt 

#debug:
#	gdb ./

.PHONY: clean server client #valgrind debug