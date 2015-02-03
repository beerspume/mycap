#include "utils.h"
#include "spy.h"


FILE *SPY_LOG_FILE;
void spy_printf(const char *fmt, ...){
	va_list ap;

	va_start(ap, fmt);

	if(SPY_LOG_FILE==NULL){
		SPY_LOG_FILE=fopen("log_spy.txt","a");
	}
	if(SPY_LOG_FILE!=NULL){
		vfprintf(SPY_LOG_FILE,fmt,ap);
		fflush(SPY_LOG_FILE);
	}
	va_end(ap);
}

static char spy_FormatedTimeResult[20];
char* spy_S2T(time_t second){
	struct tm* tm_time=localtime(&second);
	sprintf(spy_FormatedTimeResult,"%4d-%02d-%02d %02d:%02d:%02d",
		tm_time->tm_year+1900
		,tm_time->tm_mon+1
		,tm_time->tm_mday
		,tm_time->tm_hour
		,tm_time->tm_min
		,tm_time->tm_sec
	);
	return spy_FormatedTimeResult;
};

char log_data[1024];
void buf_printf(const char *fmt, ...){
	va_list ap;
	va_start(ap, fmt);

	vsprintf(log_data,fmt,ap);

	int len=strlen(log_data);
	char* data=malloc(len+1);
	strcpy(data,log_data);
	struct string_buffer_chain* _log=malloc(sizeof(struct string_buffer_chain));
	_log->data=data;
	_log->next=NULL;
	queue_push(_log);
	va_end(ap);
};


int message_discard=0;
int message_connected=0;
char config_hostname[128]="127.0.0.1";
int config_port=8000;
int client_sockfd=0;

void spy_setHostname(char* hostname){
	strcpy(config_hostname,hostname);
}
void spy_setPort(int port){
	config_port=port;
}


void CatchServerBroken(int sig) {
	client_sockfd=0;
	printf("Server %s:%d is down\n",config_hostname,config_port);
	message_discard=0;
	message_connected=0;
}
int initSocket(){
	signal(SIGPIPE, CatchServerBroken);
	if(client_sockfd>0)
		close(client_sockfd);
	client_sockfd=0;
    struct sockaddr_in remote_addr;
    memset(&remote_addr,0,sizeof(remote_addr));
    remote_addr.sin_family=AF_INET;
    remote_addr.sin_addr.s_addr=inet_addr(config_hostname);
    remote_addr.sin_port=htons(config_port);
    
    client_sockfd=socket(PF_INET,SOCK_STREAM,0);
	// int val=1;
 //    setsockopt(client_sockfd, SOL_SOCKET, SO_SNDTIMEO, &val, sizeof(val));

	int rc=connect(client_sockfd,(struct sockaddr *)&remote_addr,sizeof(struct sockaddr));
    if(rc<0){
    	close(client_sockfd);
    	client_sockfd=0;
    	return rc;
    }
    if(!message_connected){
		printf("connected to %s:%d\n",config_hostname,config_port);
		message_discard=0;
		message_connected=1;
    }
    return 0;
}
int count=0;
void do_trans(char* data){
	if(client_sockfd==0){
	    int rc=initSocket(config_hostname,config_port);
	    if(rc<0){
	    	if(!message_discard){
		    	printf("can not connect to %s:%d,next data will be Discard\n",config_hostname,config_port);
		    	message_discard=1;
		    	message_connected=0;
	    	}
	    	return;
	    }
	}else{
		send(client_sockfd,data,strlen(data),0);
	}
};



static int trans_running=1;
void* trans_buff(void* arg){
	while(trans_running){
		struct string_buffer_chain* _log=queue_popup();
		if(_log!=NULL){
			do_trans(_log->data);
			free(_log->data);
			free(_log);
		}
		usleep(1);
	};
	return NULL;
};
void stop_trans(){
	trans_running=0;
}

char base64_buff[1024];
void bin_print(const struct pcap_pkthdr *header, const u_char* packet){
	memset(base64_buff,0,1024);
	base64_encode(packet,base64_buff,header->caplen);

	// buf_printf(base64_buff);
	do_trans(base64_buff);
}


