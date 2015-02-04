#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>


#ifndef my_utils
#define my_utils


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

#endif