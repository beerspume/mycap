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
#include "parse.h"
#include "spy.h"

pcap_t* pd;
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

struct tmp_packet
{
    const struct pcap_pkthdr *header;
    const u_char *packet;
};

int filter(const struct pcap_pkthdr *header, const u_char *packet){
    u_char* read_p=(u_char*)packet;
    uint16_t radio_len=convertToLittleEndian16(read_p+2);
    uint8_t type_int=*(uint8_t*)(read_p+radio_len);
    type_int=(type_int & 0b11111100);
    if(type_int==0x48 || type_int==0x40){
        return 1;
    }else{
        return 0;
    }

};
void got_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet){
    if(packet!=NULL){
        if(filter(header,packet)==0)return;
        bin_print(header,packet);

        // struct std_rt_header rt_header;
        // struct std_80211 s_80211;
        // parsePacket(header,packet,&rt_header,&s_80211);


        // if(!subtype_in_config_filter(s_80211.subtype)){
        // if(s_80211.type_int==0x08
        // &&(mac_in_config(s_80211.mac_addr1)
        // || mac_in_config(s_80211.mac_addr2)
        // || mac_in_config(s_80211.mac_addr3))
        // ){
            // printf("%s\t%s\t%s\t%s\t%s\t%s\t%d\n",
            //     s_80211.type,s_80211.subtype,
            //     s_80211.mac_addr1,
            //     s_80211.mac_addr2,
            //     s_80211.mac_addr3,
            //     utils_S2T(header->ts.tv_sec),
            //     rt_header.v_AntennaSignal
            // );
            // for(int i=0;i<header->caplen;i++){
            //     printf("%02x ",packet[i]);
            // }
            // printf("\n");
        // }

        // if(!subtype_in_config_filter(s_80211.subtype)){
        //     spy_printf("%s\t%s\t%s\t%s\t%s\t%s\t%s\n",
        //         s_80211.type,s_80211.subtype,s_80211.tofrom
        //         ,s_80211.mac_addr1,s_80211.mac_addr2,s_80211.mac_addr3
        //         ,utils_S2T(header->ts.tv_sec));
        // }
    }
}
int main(int argc, char  *argv[])
 {
    initConfig();

    int ch;
    opterr=0;
    char device[50];
    strcpy(device,"mon0");
    while((ch=getopt(argc,argv,"i:h:p:"))!=-1){
        switch(ch){
            case 'i':
                strcpy(device,optarg);
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
    set_config_device_name(device);

    signal(SIGINT, CatchShutdown);
    signal(SIGTERM, CatchShutdown);
    signal(SIGHUP, CatchShutdown);
    signal(SIGQUIT, CatchShutdown);

    char errBuf[1024]="";


    pd=pcap_open_live(device,BUFSIZ,1,1000,errBuf);
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
