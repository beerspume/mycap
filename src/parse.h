#include "radiotap_parse.h"
#include "80211_parse.h"

#ifndef my_parse
#define my_parse

void parsePacket(const struct pcap_pkthdr *header, const u_char *packet,
    struct std_rt_header* rt_header,struct std_80211* s_80211);

#endif