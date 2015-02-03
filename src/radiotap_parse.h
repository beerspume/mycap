#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pcap.h>

#include "utils.h"

#ifndef radiotap_parse
#define radiotap_parse

enum radiotap_presenc_type {
    RADIOTAP_PRESENC_TSFT = 0,
    RADIOTAP_PRESENC_FLAGS = 1,
    RADIOTAP_PRESENC_RATE = 2,
    RADIOTAP_PRESENC_CHANNEL = 3,
    RADIOTAP_PRESENC_FHSS = 4,
    RADIOTAP_PRESENC_DBM_ANTSIGNAL = 5,
    RADIOTAP_PRESENC_DBM_ANTNOISE = 6,
    RADIOTAP_PRESENC_LOCK_QUALITY = 7,
    RADIOTAP_PRESENC_TX_ATTENUATION = 8,
    RADIOTAP_PRESENC_DB_TX_ATTENUATION = 9,
    RADIOTAP_PRESENC_DBM_TX_POWER = 10,
    RADIOTAP_PRESENC_ANTENNA = 11,
    RADIOTAP_PRESENC_DB_ANTSIGNAL = 12,
    RADIOTAP_PRESENC_DB_ANTNOISE = 13,
    RADIOTAP_PRESENC_RX_FLAGS = 14,
    RADIOTAP_PRESENC_TX_FLAGS = 15,
    RADIOTAP_PRESENC_RTS_RETRIES = 16,
    RADIOTAP_PRESENC_DATA_RETRIES = 17,

    RADIOTAP_PRESENC_MCS = 19,
    RADIOTAP_PRESENC_AMPDU_STATUS = 20,
    RADIOTAP_PRESENC_VHT = 21,

    RADIOTAP_PRESENC_RADIOTAP_NAMESPACE = 29,
    RADIOTAP_PRESENC_VENDOR_NAMESPACE = 30,
    RADIOTAP_PRESENC_EXT = 31
};

struct std_rt_header{
    uint8_t version;
    uint16_t len;
    uint32_t present;

    uint8_t has_ExtendedPresence;

    uint8_t has_TSFT;uint64_t v_TSFT;
    uint8_t has_Flags;uint8_t v_Flags;
    uint8_t has_Rate;uint8_t v_Rate;
    uint8_t has_Channel;uint16_t v_Channel_Frequency;uint16_t v_Channel_Flags;
    uint8_t has_FHSS;uint8_t v_FHSS_HopSet;uint8_t v_FHSS_HopPattern;
    uint8_t has_AntennaSignal;int8_t v_AntennaSignal;
    uint8_t has_AntennaNoise;int8_t v_AntennaNoise;
    uint8_t has_LockQuality;uint16_t v_LockQualit;
    uint8_t has_TXAttenuation;uint16_t v_TXAttenuation;
    uint8_t has_dBTXAttenuation;uint16_t v_dBTXAttenuation;
    uint8_t has_dBmTXPower;int8_t v_dBmTXPower;
    uint8_t has_Antenna;uint8_t v_Antenna;
    uint8_t has_dBAntennaSignal;uint8_t v_dBAntennaSignal;
    uint8_t has_dBAntennaNoise;uint8_t v_dBAntennaNoise;
    uint8_t has_RXFlags;uint16_t v_RXFlags;
    uint8_t has_MCS;uint8_t v_MCS_Known;uint8_t v_MCS_Flags;uint8_t v_MCS_Mcs;
    uint8_t has_AMPDUStatus;uint32_t v_AMPDUStatus_RefNum;uint16_t v_AMPDUStatus_Flags;
                            uint8_t v_AMPDUStatus_CRC;uint8_t v_AMPDUStatus_Reserved;
    uint8_t has_VHT;uint16_t v_VHT_Known;uint8_t v_VHT_Flags;uint8_t v_VHT_Bandwidth;
                    uint8_t v_VHT_mcs_nss[4];uint8_t v_VHT_Coding;uint8_t v_VHT_Groupid;uint16_t v_VHT_Partialaid;
};

int parseRadioHeader(const u_char*,struct std_rt_header*);

void radioHeader_clean(struct std_rt_header*);

#endif
