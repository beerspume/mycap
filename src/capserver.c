#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <signal.h>

#include "radiotap_parse.h"
#include "80211_parse.h"
#include "utils.h"

sqlite3 * db = 0;
void CatchShutdown(int sig) {
    if(db!=0){
        sqlite3_close(db);
    }
    exit(1);
}

int initDB(){
    const char * create_table_sql = 
"create table frame(\
id INTEGER PRIMARY KEY,\
`type` varchar(20),\
subtype varchar(50),\
tofrom varchar(20),\
addr1 varchar(20),\
addr2 varchar(20),\
addr3 varchar(20),\
`time` varchar(50)\
);";
    char * pErrMsg = 0;
    int ret = sqlite3_open("./mycap.db", &db);
    if( ret != SQLITE_OK ) {
        printf("can't open db: %s", sqlite3_errmsg(db));
        sqlite3_close(db);
        db=0;
        return 0;
    }
    printf("db open success!\n");
    sqlite3_exec( db, create_table_sql, 0, 0, &pErrMsg );
    if( ret != SQLITE_OK ){
        printf("SQL error: %s\n", pErrMsg);
        sqlite3_free(pErrMsg);
        sqlite3_close(db);
        db=0;
        return 0;
    }
    printf("init table success!\n");
    return 1;
}

int initSocket(int port,int* serverSocket){
    *serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    int val=1;
    setsockopt(*serverSocket, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));

    struct sockaddr_in serverAddr;

    serverAddr.sin_family      = AF_INET;
    serverAddr.sin_port        = htons(port);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    memset(&(serverAddr.sin_zero), 0, 8);


    int rc = bind(*serverSocket, (struct sockaddr*) &serverAddr,
       sizeof(struct sockaddr));
    return rc;
}

char sql[1024];
void do_parse(u_char* buff){
    int caplen;
    getIntProperty((const char*)buff,"len",&caplen);
    char time_str[30]="";
    getStrProperty((const char*)buff,"time",time_str);
    char _p_data[BUFSIZ];
    getStrProperty((const char*)buff,"data",_p_data);

    
    u_char data[BUFSIZ];
    int data_len;
    data_len=base64_decode((const char*)_p_data,data);
    struct std_rt_header rt_header;
    parseRadioHeader(data,&rt_header);

    int frame_len=data_len-rt_header.len;
    const u_char* p=data+rt_header.len;

    struct std_80211 s_80211;
    parse80211(p,frame_len,&s_80211);

    sprintf(sql,"insert into frame (`type`,subtype,tofrom,addr1,addr2,addr3,`time`) values ('%s','%s','%s','%s','%s','%s','%s');"
        ,s_80211.type
        ,s_80211.subtype
        ,s_80211.tofrom
        ,s_80211.mac_addr1
        ,s_80211.mac_addr2
        ,s_80211.mac_addr3
        ,time_str
    );
    char * pErrMsg = 0;
    int result = sqlite3_exec( db, sql, 0, 0, &pErrMsg);
    if(result != SQLITE_OK){
        printf("insert faile:%s\n",pErrMsg);
        sqlite3_free(pErrMsg);
    }
}

u_char pre_read_buff[BUFSIZ]="";
void do_recv(u_char* buff,int len){
    u_char temp_buff[BUFSIZ];
    strcpy((char*)temp_buff,(const char *)pre_read_buff);
    memset(pre_read_buff,0,BUFSIZ);

    u_char* current_temp_buff=temp_buff+strlen((const char *)temp_buff);
    u_char* current_buff=buff;

    int remain_len=len-(current_buff-buff);
    while(remain_len>0){
        int zero_index=0;
        for(zero_index=0;zero_index<remain_len;zero_index++){
            if(current_buff[zero_index]==0) break;
        }
        if(zero_index>=remain_len && *(current_buff+zero_index)!='\0'){
            memcpy(current_temp_buff,current_buff,zero_index);
            *(current_temp_buff+zero_index)='\0';
            strcpy((char*)pre_read_buff,(const char *)temp_buff);
            break;
        }else{
            memcpy(current_temp_buff,current_buff,zero_index+1);
            current_temp_buff=temp_buff;
            current_buff+=zero_index+1;
            remain_len=len-(current_buff-buff);
            do_parse(current_temp_buff);
        }
    }

}

int main(int argc, char const *argv[]){
    signal(SIGINT, CatchShutdown);
    signal(SIGTERM, CatchShutdown);
    signal(SIGHUP, CatchShutdown);
    signal(SIGQUIT, CatchShutdown);
    if(initDB()){
        while(1){
            int serverSocket;
            int rc=initSocket(8000,&serverSocket);
            if (rc == -1) {
             printf("Bad bind\n");
             exit(1);
            }
            rc = listen(serverSocket, 5);
            if (rc == -1) {
             printf("Bad listen\n");
             exit(1);
            }

            int sock;
            struct sockaddr_in clientAddr;
            int clientAddrSize = sizeof(struct sockaddr_in);
            sock = accept(serverSocket,
               (struct sockaddr*) &clientAddr,
               (socklen_t*) &clientAddrSize);
            close(serverSocket);
            int len;
            u_char buf[BUFSIZ];
            while((len=recv(sock,buf,BUFSIZ,0))>0){
                do_recv(buf,len);
            }
        }
    }
    CatchShutdown(-1);
    return 0;
}