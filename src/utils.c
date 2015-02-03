#include "utils.h"

int isBigEndian(){
    union{
        uint16_t v1;
        uint8_t v2;
    }v;
    v.v1=1;
    return v.v2==0;
};

uint16_t convertToLittleEndian16(uint8_t* s){
    if(isBigEndian()){
        uint8_t v[2];
        v[0]=*(s+1);
        v[1]=*s;
        return *(uint16_t*)v;
    }else{
        return *(uint16_t*)s;
    }
};

uint32_t convertToLittleEndian32(uint8_t* s){
    if(isBigEndian()){
        uint8_t v[4];
        v[0]=*(s+3);
        v[1]=*(s+2);
        v[2]=*(s+1);
        v[3]=*s;
        return *(uint32_t*)v;
    }else{
        return *(uint32_t*)s;
    }
};
uint64_t convertToLittleEndian64(uint8_t* s){
    if(isBigEndian()){
        uint8_t v[8];
        v[0]=*(s+7);
        v[1]=*(s+6);
        v[2]=*(s+5);
        v[3]=*(s+4);
        v[4]=*(s+3);
        v[5]=*(s+2);
        v[6]=*(s+1);
        v[7]=*s;
        return *(uint64_t*)v;
    }else{
        return *(uint64_t*)s;
    }
};


const char * base64char = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

char * base64_encode( const unsigned char * bindata, char * base64, int binlength )
{
    int i, j;
    unsigned char current;

    for ( i = 0, j = 0 ; i < binlength ; i += 3 )
    {
        current = (bindata[i] >> 2) ;
        current &= (unsigned char)0x3F;
        base64[j++] = base64char[(int)current];

        current = ( (unsigned char)(bindata[i] << 4 ) ) & ( (unsigned char)0x30 ) ;
        if ( i + 1 >= binlength )
        {
            base64[j++] = base64char[(int)current];
            base64[j++] = '=';
            base64[j++] = '=';
            break;
        }
        current |= ( (unsigned char)(bindata[i+1] >> 4) ) & ( (unsigned char) 0x0F );
        base64[j++] = base64char[(int)current];

        current = ( (unsigned char)(bindata[i+1] << 2) ) & ( (unsigned char)0x3C ) ;
        if ( i + 2 >= binlength )
        {
            base64[j++] = base64char[(int)current];
            base64[j++] = '=';
            break;
        }
        current |= ( (unsigned char)(bindata[i+2] >> 6) ) & ( (unsigned char) 0x03 );
        base64[j++] = base64char[(int)current];

        current = ( (unsigned char)bindata[i+2] ) & ( (unsigned char)0x3F ) ;
        base64[j++] = base64char[(int)current];
    }
    base64[j] = '\0';
    return base64;
};

int base64_decode( const char * base64, unsigned char * bindata )
{
    int i, j;
    unsigned char k;
    unsigned char temp[4];
    for ( i = 0, j = 0; base64[i] != '\0' ; i += 4 )
    {
        memset( temp, 0xFF, sizeof(temp) );
        for ( k = 0 ; k < 64 ; k ++ )
        {
            if ( base64char[k] == base64[i] )
                temp[0]= k;
        }
        for ( k = 0 ; k < 64 ; k ++ )
        {
            if ( base64char[k] == base64[i+1] )
                temp[1]= k;
        }
        for ( k = 0 ; k < 64 ; k ++ )
        {
            if ( base64char[k] == base64[i+2] )
                temp[2]= k;
        }
        for ( k = 0 ; k < 64 ; k ++ )
        {
            if ( base64char[k] == base64[i+3] )
                temp[3]= k;
        }

        bindata[j++] = ((unsigned char)(((unsigned char)(temp[0] << 2))&0xFC)) |
                ((unsigned char)((unsigned char)(temp[1]>>4)&0x03));
        if ( base64[i+2] == '=' )
            break;

        bindata[j++] = ((unsigned char)(((unsigned char)(temp[1] << 4))&0xF0)) |
                ((unsigned char)((unsigned char)(temp[2]>>2)&0x0F));
        if ( base64[i+3] == '=' )
            break;

        bindata[j++] = ((unsigned char)(((unsigned char)(temp[2] << 6))&0xF0)) |
                ((unsigned char)(temp[3]&0x3F));
    }
    return j;
};

struct string_buffer_chain* top_of_queue=NULL;
struct string_buffer_chain* end_of_queue=NULL;
pthread_mutex_t queue_mutex;
void queue_init_lock(){
    pthread_mutex_init(&queue_mutex,NULL);
};


void queue_push(struct string_buffer_chain* data){
    pthread_mutex_lock(&queue_mutex);
    if(end_of_queue==NULL){
        top_of_queue=data;
    }else{
        end_of_queue->next=data;
    }
    end_of_queue=data;
    pthread_mutex_unlock(&queue_mutex);
}
struct string_buffer_chain* queue_popup(){
    pthread_mutex_lock(&queue_mutex);
    struct string_buffer_chain* ret=top_of_queue;
    if(ret!=NULL){
        top_of_queue=top_of_queue->next;
        if(top_of_queue==NULL){
            end_of_queue=NULL;
        }
    }
    pthread_mutex_unlock(&queue_mutex);
    return ret;
};
