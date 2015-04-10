#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
// #include <sqlite3.h>
#include <signal.h>
#include <mysql/mysql.h>

#include "radiotap_parse.h"
#include "80211_parse.h"
#include "utils.h"

// sqlite3 * db = 0;

#define MY_BUFSIZ 1024

struct socket_thread
{
    pthread_t* t;
    int sock;
    int running;
    MYSQL* conn;
    u_char* pre_read_buff;
};


struct socket_thread st[10];

// MYSQL my_connection; 

void* socket_loop(void* arg);
void stopThread(struct socket_thread* st);
void clearThread();
struct socket_thread* getFreeST();

void CatchShutdown(int sig) {
    // if(db!=0){
    //     sqlite3_close(db);
    // }
    shutdown_refresh_config();
    clearThread();
    // mysql_close(&my_connection);
    exit(1);
}

MYSQL* getConnection(){
    MYSQL* conn=malloc(sizeof(MYSQL));
    mysql_init(conn);
    if(mysql_real_connect(conn,"127.0.0.1"  
                            ,"mycap","123456","mycap",3306,NULL,0)){  
        printf("db open success!\n");
        return conn;
    }else{
        printf("can't open db\n");
        return NULL;
    }

}

int initDB(){
    const char * create_table_sql = 
"create table frame(\
id INTEGER not null auto_increment PRIMARY KEY ,\
`type` varchar(20),\
subtype varchar(50),\
tofrom varchar(20),\
addr1 varchar(20),\
addr2 varchar(20),\
addr3 varchar(20),\
`time` varchar(50),\
antenna_signal int(11) default 0,\
source varchar(20)\
);";
/*
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
*/
    MYSQL* conn=getConnection();
    if(conn!=NULL){
        mysql_query(conn,create_table_sql);
        mysql_close(conn);
        free(conn);
    }else{
        return 0;
    }
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

void do_parse(char* buff,struct socket_thread* _st){
    printf("%s\n",buff);
    return;

    if(_st==NULL){
        printf("why I can not found socket_threadwhen do_parse?\n");
        return;
    }
    if(_st->conn==NULL){
        _st->conn=getConnection();
        if(_st->conn==NULL){
            printf("can not get connection when do_parse\n");
            return;
        }
    }

    char source[20]="";
    getStrProperty((const char*)buff,"source",source);
    int caplen=0;
    getIntProperty((const char*)buff,"len",&caplen);
    char time_str[30]="";
    getStrProperty((const char*)buff,"time",time_str);
    char _p_data[MY_BUFSIZ];
    getStrProperty((const char*)buff,"data",_p_data);

    if(strlen(source)!=17 || caplen==0 || strlen(_p_data)==0){
        printf("%s\n",buff);
        return;
    }

    u_char data[MY_BUFSIZ];
    int data_len;
    data_len=base64_decode((const char*)_p_data,data);
    if(data_len!=caplen){
        printf("%s\n",buff);
        printf("%d,%d\n",data_len,caplen);
        return;
    }
    struct std_rt_header rt_header;
    parseRadioHeader(data,&rt_header);

    int frame_len=data_len-rt_header.len;
    const u_char* p=data+rt_header.len;

    struct std_80211 s_80211;
    parse80211(p,frame_len,&s_80211);

    char sql[1024];
    sprintf(sql,"insert into frame (source,`type`,subtype,tofrom,addr1,addr2,addr3,`time`,antenna_signal) values ('%s','%s','%s','%s','%s','%s','%s','%s',%d);"
        ,source
        ,s_80211.type
        ,s_80211.subtype
        ,s_80211.tofrom
        ,s_80211.mac_addr1
        ,s_80211.mac_addr2
        ,s_80211.mac_addr3
        ,time_str
        ,rt_header.v_AntennaSignal
    );


        int result=mysql_query(_st->conn,sql);
        if(result!=0){
            printf("insert faile\n");
        // }
    }
}

// u_char pre_read_buff[BUFSIZ]="";
void do_recv(char* buff_0,struct socket_thread* _st){
    char buff[MY_BUFSIZ];
    strcpy(buff,(const char*)buff_0);
    char data[MY_BUFSIZ];
    strcpy(data,(const char*)_st->pre_read_buff);
    _st->pre_read_buff[0]='\0';

    char* p_split_start=strstr((const char*)buff,"|-");
    char* p_split_end=strstr((const char*)buff,"-|");

    if(p_split_start!=NULL && p_split_end!=NULL && p_split_end>p_split_start){
        p_split_end[0]='\0';
        strcpy(data,(const char*)(p_split_start+2));
        do_parse(data,_st);
    // }else if(p_split_start!=NULL && p_split_end!=NULL && p_split_end<p_split_start){
    //     p_split_end[0]='\0';
    //     char* last_data=(char*)(data+strlen((const char*)data));
    //     strcpy(last_data,(const char*)buff);
    //     do_parse(data,_st);

    //     do_recv(p_split_start,_st);

    }else if(p_split_start!=NULL && p_split_end==NULL){
        strcpy((char*)_st->pre_read_buff,(const char*)(p_split_start+2));
    }else if(p_split_start==NULL && p_split_end!=NULL){
        p_split_end[0]='\0';
        char* last_data=(char*)(data+strlen((const char*)data));
        strcpy(last_data,(const char*)buff);
        do_parse(data,_st);

    }else{
        char* last_data=(char*)(data+strlen((const char*)data));
        strcpy(last_data,(const char*)buff);
        strcpy((char*)_st->pre_read_buff,(const char*)data);
    }
}

void _stopThread(struct socket_thread* _st){
    if(_st->sock!=0){
        close(_st->sock);
        _st->sock=0;
    }
    if(_st->conn!=NULL){
        mysql_close(_st->conn);
        free(_st->conn);
        _st->conn=NULL;
    }
    if(_st->t!=NULL){
        _st->running=0;
        free(_st->t);
        _st->t=NULL;
    }
    if(_st->pre_read_buff!=NULL){
        free(_st->pre_read_buff);
        _st->pre_read_buff=NULL;
    }
}

void* socket_loop(void* arg){
    struct socket_thread* _st=(struct socket_thread*)arg;
    if(_st!=NULL){
        _st->running=1;
        int len;
        char buf[MY_BUFSIZ];
        while((len=recv(_st->sock,buf,MY_BUFSIZ-1,0))>0 && _st->running==1){
            buf[len]='\0';
            do_recv(buf,_st);
        }
        _stopThread(_st);
    }
    return NULL;
}



void stopThread(struct socket_thread* _st){
    if(_st->t!=NULL){
        _st->running=0;
        pthread_join(*_st->t,NULL);
    }
    _stopThread(_st);
}
void clearThread(){
    for(int i=0;i<10;i++){
        stopThread(&st[i]);
    }
}
struct socket_thread* getFreeST(){
    for(int i=0;i<10;i++){
        if(st[i].t==NULL){
            st[i].t=malloc(sizeof(pthread_t));
            st[i].pre_read_buff=malloc(MY_BUFSIZ);
            st[i].pre_read_buff[0]='\0';
            return &st[i];
        }
    }
    return NULL;
}

int main(int argc, char const *argv[]){
    // char* buff_1="fasdfadklfjdlaks-|fdsfdas|-source=3C:46:D8:16:CA:";
    // char* buff_2="B8;len=110;time=2015-04-10 04:08:22;utime=3018";
    // char* buff_3="24;data=AAAkAC9AAKAgCAAAAAAAANtYav4HAQAAEAJsCcAArQAAAK0AQAAAAP///////wAhaiMiNP///////7AcAAABCAIECxYMEhgkMgQwSGBsLRo8CBf///8AAAAAAAAAAAAAAAAAAAAAAAAAAHAkEno=-|";
    // struct socket_thread* _st=getFreeST();
    // do_recv(buff_1,_st);
    // do_recv(buff_2,_st);
    // do_recv(buff_3,_st);
    // _stopThread(_st);
    // return 0;
    signal(SIGINT, CatchShutdown);
    signal(SIGTERM, CatchShutdown);
    signal(SIGHUP, CatchShutdown);
    signal(SIGQUIT, CatchShutdown);
    clearThread();
    if(initDB()){
    initConfig();
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
            struct socket_thread* _st=getFreeST();
            if(_st!=NULL){
                _st->sock=sock;
                pthread_create(_st->t,NULL,socket_loop,(void *)_st);
            }else{
                close(sock);
            }
            // int len;
            // u_char buf[BUFSIZ];
            // while((len=recv(sock,buf,BUFSIZ,0))>0){
            //     do_recv(buf,len);
            // }
        }
    }
    CatchShutdown(-1);
    return 0;
}