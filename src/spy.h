#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <time.h>
#include <stdarg.h>
#include <string.h>
#include <pcap.h>
#include <signal.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "utils.h"

#ifndef JOYFUL_SPY
#define JOYFUL_SPY


void spy_printf(const char *fmt, ...);

void buf_printf(const char *fmt, ...);
void* trans_buff(void*);

void stop_trans();
void bin_print(const struct pcap_pkthdr *header, const u_char*);

int initSocket();

void spy_setHostname(char*);
void spy_setPort(int);
#endif