#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <pcap.h>
#include <time.h>
#include <pthread.h>

#include "utils.h"
#include "radiotap_parse.h"
#include "80211_parse.h"
#include "spy.h"

pcap_t* pd;
int enable_mac_filter=0;
pthread_t t1;

void CatchShutdown(int sig) {
    if(pd!=NULL){
        pcap_close(pd);
        printf("\npd is closed\n");
    }
    stop_trans();
    pthread_join(t1,NULL);
    printf("\ntrans thread is stoped\n");
    exit(1);
}



static char BinaryNumber[33];
const char* convertToBinartString(uint32_t number,int maxlen){
    int len=maxlen>32?32:maxlen;
    memset(BinaryNumber,0,33);
    for(int i=0;i<len;i++){
        if(number&(1<<i)){
            BinaryNumber[len-i-1]='1';
        }else{
            BinaryNumber[len-i-1]='0';
        }
    }
    return BinaryNumber;
}


void got_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet){
    if(packet!=NULL){
        bin_print(header,packet);
/*        
        struct std_rt_header rt_header;
        parseRadioHeader(packet,&rt_header);

        int data_len=header->caplen-rt_header.len;
        const u_char* p=packet+rt_header.len;

        struct std_80211 s_80211;
        parse80211(p,data_len,&s_80211);

        char* my_mac[]={
            "b4:0b:44:0c:99:2e"
            ,"00:08:22:aa:ae:fc"
            ,"a4:ed:4e:40:e2:81"
            ,"68:96:7b:e6:d6:62"
        };

        if(!enable_mac_filter 
            || ((addressIn(&s_80211,my_mac,4))
                
            )
        ){

            if(rt_header.has_TSFT){
                http_printf("%ld ",rt_header.v_TSFT/1000000);
            }
            http_printf("%s\t%s\t%s\t%s\t%s\t%s\t%s\n",
                s_80211.type,s_80211.subtype,s_80211.tofrom
                ,s_80211.mac_addr1,s_80211.mac_addr2,s_80211.mac_addr3
                ,spy_S2T(header->ts.tv_sec));
        }
*/


    }
}
int main(int argc, char  *argv[])
 {
    // uint8_t s[]={
    //     0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08
    // };

    // printf("%d,%d,%ld\n",convertToLittleEndian16(s),convertToLittleEndian32(s),convertToLittleEndian64(s));
    // return 0;

    int ch;
    opterr=0;
    char device[50];
    strcpy(device,"mon0");
    while((ch=getopt(argc,argv,"i:fh:p:"))!=-1){
        switch(ch){
            case 'i':
                strcpy(device,optarg);
                break;
            case 'f':
                enable_mac_filter=1;
                break;
            case 'h':
                spy_setHostname(optarg);
                break;
            case 'p':
                spy_setPort(atoi(optarg));
            default:
                break;
        }
    };

    signal(SIGINT, CatchShutdown);
    signal(SIGTERM, CatchShutdown);
    signal(SIGHUP, CatchShutdown);
    signal(SIGQUIT, CatchShutdown);

    char errBuf[1024]="";

    
    pd=pcap_open_live(device,2048,1,1000,errBuf);
    if(pd!=NULL){
        printf("success: device %s\n",device);

        pthread_create(&t1,NULL,trans_buff,(void *)NULL);

        pcap_loop(pd,0,got_packet,NULL);
        // struct pcap_pkthdr pkthdr;
        // const u_char* data;
        // while(1){
        //     data=pcap_next(pd,&pkthdr);
        //     if(data!=NULL){
        //         got_packet(NULL, (const struct pcap_pkthdr*)(&pkthdr), data);
        //     }
        // }
    }else{
        printf("error: %s\n",errBuf);
    }
    CatchShutdown(-1);
    return 0;
 } 
