#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <net/if.h>

#include "cJSON.h"

#ifndef my_utils
#define my_utils

struct config{
    char* filter_subtype;
    char * mac;
    char device_name[50];
    char device_mac[20];
};

struct string_buffer_chain{
    char* data;
    struct string_buffer_chain* next;
    // log_buffer_chain* prev;
};

char* utils_S2T(time_t second);

int isBigEndian();

uint16_t convertToLittleEndian16(uint8_t*);
uint32_t convertToLittleEndian32(uint8_t*);
uint64_t convertToLittleEndian64(uint8_t*);

char* base64_encode(const u_char*,char*,int);
int base64_decode(const char*,u_char*);

void queue_push(struct string_buffer_chain*);
struct string_buffer_chain* queue_popup();
void queue_init_lock();

int getIntProperty(const char*,const char*,int*);
int getStrProperty(const char*,const char*,char*);

void shutdown_refresh_config();
struct config* initConfig();
struct config* getConfig();
int subtype_in_config_filter(char* subtype);
int mac_in_config(char* mac);

char* readDeviceMac(char* device_name);
void set_config_device_name(char* device_name);
const char* get_config_device_name();
const char* get_config_device_mac();

#endif