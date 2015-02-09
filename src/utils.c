#include <sys/stat.h>
#include <unistd.h>
#include "utils.h"


static char utils_FormatedTimeResult[20];
char* utils_S2T(time_t second){
    struct tm* tm_time=localtime(&second);
    sprintf(utils_FormatedTimeResult,"%4d-%02d-%02d %02d:%02d:%02d",
        tm_time->tm_year+1900
        ,tm_time->tm_mon+1
        ,tm_time->tm_mday
        ,tm_time->tm_hour
        ,tm_time->tm_min
        ,tm_time->tm_sec
    );
    return utils_FormatedTimeResult;
};

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


int getIntProperty(const char* data,const char* key,int* ret){
    char value[20];
    int rc=getStrProperty(data,key,value);
    if(rc!=0){
        *ret=0;
    }else{
        *ret=atoi(value);
    }
    return rc;
};

int getStrProperty(const char* data,const char* key,char* ret){
    char* fullkey=malloc(strlen(key)+3);
    sprintf(fullkey,";%s=",key);
    int fullkey_len=strlen(fullkey);
    char* p0=strstr(data,(const char*)fullkey);
    if(p0==NULL){
        if(memcmp(data,fullkey+1,fullkey_len-1)==0){
            p0=((char*)data)+fullkey_len-1;
        }
    }else{
         p0+=fullkey_len;
    }
    free(fullkey);
    if(p0!=NULL){
        char* p1=strstr((const char*)p0,";");
        if(p1!=NULL){
            memcpy(ret,p0,p1-p0);
        }else{
            strcpy(ret,p0);
        }
        return 0;
    }else{
        return 1;
    }
};

struct config my_config;
pthread_t thread_refresh_config;
int refresh_config_running=0;
struct stat* config_stat=NULL;
char * config_filename="config.json";
void* refresh_config(void* arg){
    if(refresh_config_running)return NULL;
    refresh_config_running=1;
    uint32_t times=0;
    while(refresh_config_running){
        if(times<50){
            times++;
            usleep(100000);
            continue;
        };
        times=0;
        struct stat statbuf;
        if(stat(config_filename,&statbuf)==0){
            if(config_stat==NULL){
                config_stat=malloc(sizeof(struct stat));
                memcpy(config_stat,&statbuf,sizeof(struct stat));
            }else{
                if(config_stat->st_mtime!=statbuf.st_mtime 
                    || config_stat->st_size!=statbuf.st_size){
                    printf("config file changed,it will be reloaded\n");
                    initConfig();
                    memcpy(config_stat,&statbuf,sizeof(struct stat));
                }
            }
        }
    }
    return NULL;
};
void shutdown_refresh_config(){
    refresh_config_running=0;
    pthread_join(thread_refresh_config,NULL);
}
void config_free(){
    free(my_config.filter_subtype);
    memset(&my_config,0,sizeof(struct config));
};
struct config* initConfig(){
    if(!refresh_config_running){
        pthread_create(&thread_refresh_config,NULL,refresh_config,(void *)NULL);
    }
    struct stat statbuf;
    if(stat(config_filename,&statbuf)==0){
        int size=statbuf.st_size;
        FILE *config_file;
        config_file=fopen("config.json","r");
        if(config_file!=NULL){
            char* file_content_buff=malloc(size);
            fread(file_content_buff,size,1,config_file);
            fclose(config_file);
            cJSON* json=cJSON_Parse(file_content_buff);
            free(file_content_buff);
            if(json!=NULL){
                config_free(&my_config);
                cJSON* c_j=cJSON_GetObjectItem(cJSON_GetObjectItem(json,"filter"),"subtype");
                int array_size=cJSON_GetArraySize(c_j);
                int total_size=0;
                for(int i=0;i<array_size;i++){
                    cJSON* c_item=cJSON_GetArrayItem(c_j,i);
                    total_size+=strlen(c_item->valuestring)+1;
                }
                total_size+=2;
                my_config.filter_subtype=malloc(total_size);
                memset(my_config.filter_subtype,0,total_size);
                char* c_c_filter_subtype=my_config.filter_subtype;
                for(int i=0;i<array_size;i++){
                    cJSON* c_item=cJSON_GetArrayItem(c_j,i);
                    strcpy(c_c_filter_subtype,c_item->valuestring);
                    c_c_filter_subtype+=strlen(c_item->valuestring)+1;;
                }
            }else{
                printf("invalid json!\n");
            }
            cJSON_Delete(json);
        }
    }

    return  &my_config;
};

int subtype_in_config_filter(char* subtype){
    char* c_c_filter_subtype=my_config.filter_subtype;
    if(c_c_filter_subtype!=NULL){
        while(1){
            int len=strlen(c_c_filter_subtype);
            if(len==0)break;
            char buff[30];
            strcpy(buff,c_c_filter_subtype);
            if(strcmp(buff,subtype)==0)return 1;
            c_c_filter_subtype+=len+1;
        }
    }
    return 0;
};

struct config* getConfig(){
    return &my_config;
}
