SRC += utils.c
SRC += 80211_parse.c
SRC += radiorap_parse.c

SERVER_SRC = $(SRC)

CLIENT_SRC += $(SRC)
CLIENT_SRC += spy.c

all: main.c
	$(CC) $(CFLAGS) $(LDFLAGS) -Wall -g -o mycap main.c $(CLIENT_SRC) -lpcap -lpthread -std=gnu99 
	$(CC) $(CFLAGS) $(LDFLAGS) -Wall -g -o capserver capserver.c $(SERVER_SRC) -lpcap -lpthread -std=gnu99 

clean:
	rm -rf *.o mycap capserver