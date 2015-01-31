#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <pcap.h>
#include <time.h>


#include "radiotap_parse.h"
#include "80211_parse.h"
#include "spy.h"

pcap_t* pd;
int enable_mac_filter=0;

void CatchShutdown(int sig) {
    if(pd!=NULL){
        pcap_close(pd);
        printf("\npd is closed\n");
    }
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


struct timeval tv;
struct timezone tz;
void got_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet){
    if(packet!=NULL){
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
            || ((addressIn(&s_80211,my_mac,1))
                || (frame_control_match(s_80211.frame_control[0],TYPE_80211_Data,TYPE_80211_Mask_Code) &&
                    frame_control_match(s_80211.frame_control[0],TYPE_80211_D_QoSData,SUBTYPE_80211_Mask_Code))
            )
        ){

            gettimeofday (&tv , &tz);
            // printf("%02x%02x:",s_80211.frame_control[0],s_80211.frame_control[1]);
            // printf("%s",convertToBinartString(s_80211.frame_control[0],8));
            // printf("%s\n",convertToBinartString(s_80211.frame_control[1],8));
            if(rt_header.has_TSFT){
                spy_printf("%ld ",rt_header.v_TSFT/1000000);
            }
            spy_printf("%s\t%s\t%s\t%s\t%s\t%s\t%s\n",
                s_80211.type,s_80211.subtype,s_80211.tofrom
                ,s_80211.mac_addr1,s_80211.mac_addr2,s_80211.mac_addr3
                ,spy_S2T(tv.tv_sec));
            
            // spy_printf("Package: caplen=%d\n",header->caplen);
            // for(int i=0;i<header->caplen;i++){
            //     spy_printf("%02x ",packet[i]);
            // }
            // spy_printf("\n\n");
        }



    }
}
int main(int argc, char  *argv[])
 {

#ifdef __MAC_OS
    printf("Fdsafdsfdsfdsfdsf\n")l
    return 1;
#endif
    int ch;
    opterr=0;
    char device[50];
    strcpy(device,"mon0");
    while((ch=getopt(argc,argv,"i:f"))!=-1){
        switch(ch){
            case 'i':
                strcpy(device,optarg);
                break;
            case 'f':
                enable_mac_filter=1;
                break;
            default:
                break;
        }
    };

    signal(SIGINT, CatchShutdown);
    signal(SIGTERM, CatchShutdown);
    signal(SIGHUP, CatchShutdown);
    signal(SIGQUIT, CatchShutdown);
    spy_printf("");
    char errBuf[1024]="";

    
    pd=pcap_open_live(device,2048,1,1000,errBuf);
    if(pd!=NULL){
        printf("success: device %s\n",device);
        // pcap_loop(pd,0,got_packet,NULL);
        struct pcap_pkthdr pkthdr;
        const u_char* data;
        while(1){
            data=pcap_next(pd,&pkthdr);
            if(data!=NULL){
                got_packet(NULL, (const struct pcap_pkthdr*)(&pkthdr), data);
            }
        }
    }else{
        printf("error: %s\n",errBuf);
    }
    CatchShutdown(-1);
    return 0;
 } 
