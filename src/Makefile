all: desc target

SRC += utils.c
SRC += 80211_parse.c
SRC += radiotap_parse.c
SRC += parse.c
SRC += cJSON.c

SERVER_SRC = $(SRC)

CLIENT_SRC += $(SRC)
CLIENT_SRC += spy.c

desc:

target:main.c
ifndef OPENWRT_BUILD
	$(CC) $(CFLAGS) $(LDFLAGS) -Wall -g -o capserver capserver.c $(SERVER_SRC) -lpcap -lpthread -lsqlite3 -lmysqlclient -lm -std=gnu99 
endif
	$(CC) $(CFLAGS) $(LDFLAGS) -Wall -g -o mycap main.c $(CLIENT_SRC) -lpcap -lpthread -lm -std=gnu99 
	$(CC) $(CFLAGS) $(LDFLAGS) -Wall -g -o test test.c $(CLIENT_SRC) -lpcap -lpthread -lm -std=gnu99
clean:
	rm -rf *.o mycap capserver mycap.db
