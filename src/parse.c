#include "parse.h"

void parsePacket(const struct pcap_pkthdr *header, const u_char *packet
    ,struct std_rt_header* rt_header,struct std_80211* s_80211){

    parseRadioHeader(packet,rt_header);

    int data_len=header->caplen-rt_header->len;
    const u_char* p=packet+rt_header->len;

    parse80211(p,data_len,s_80211);

};