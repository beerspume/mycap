#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef n_80211_parse
#define n_80211_parse


//MaskCode
#define TYPE_80211_Mask_Code                0b00001100
#define SUBTYPE_80211_Mask_Code             0b11110000
#define TF_80211_Mask_Code                  0b00000011
//80211 type
#define TYPE_80211_Management               0b00000000
#define TYPE_80211_Control                  0b00000100
#define TYPE_80211_Data                     0b00001000

//subtype for management
#define TYPE_80211_M_AssociationRequest     0b00000000
#define TYPE_80211_M_AssociationResponse    0b00010000
#define TYPE_80211_M_ReassociationRequest   0b00100000
#define TYPE_80211_M_ReassociationResponse  0b00110000
#define TYPE_80211_M_ProbeRequest           0b01000000
#define TYPE_80211_M_ProbeResponse          0b01010000
#define TYPE_80211_M_TimingAdvertisement    0b01100000
#define TYPE_80211_M_Beacon                 0b10000000
#define TYPE_80211_M_ATIM                   0b10010000
#define TYPE_80211_M_Disassociation         0b10100000
#define TYPE_80211_M_Authentication         0b10110000
#define TYPE_80211_M_Deauthentication       0b11000000
#define TYPE_80211_M_Action                 0b11010000
#define TYPE_80211_M_ActionNoAck            0b11100000

//subtype for control
#define TYPE_80211_C_ControlWrapper         0b01110000
#define TYPE_80211_C_BlockAckReq            0b10000000
#define TYPE_80211_C_BlockAck               0b10010000
#define TYPE_80211_C_PSPoll                 0b10100000
#define TYPE_80211_C_RTS                    0b10110000
#define TYPE_80211_C_CTS                    0b11000000
#define TYPE_80211_C_ACK                    0b11010000
#define TYPE_80211_C_CFEnd                  0b11100000
#define TYPE_80211_C_CFEnd_CFAck            0b11110000

//subtype fro data
#define TYPE_80211_D_Data                   0b00000000
#define TYPE_80211_D_Data_CFAck             0b00010000
#define TYPE_80211_D_Data_CFPoll            0b00100000
#define TYPE_80211_D_Data_CFAck_CFPoll      0b00110000
#define TYPE_80211_D_Null                   0b01000000
#define TYPE_80211_D_CFAck                  0b01010000
#define TYPE_80211_D_CFPoll                 0b01100000
#define TYPE_80211_D_CFAck_CFPoll           0b01110000
#define TYPE_80211_D_QoSData                0b10000000
#define TYPE_80211_D_QoSData_CFAck          0b10010000
#define TYPE_80211_D_QoSData_CFPoll         0b10100000
#define TYPE_80211_D_QoSData_CFAck_CFPoll   0b10110000
#define TYPE_80211_D_QoSNull                0b11000000
#define TYPE_80211_D_QoSCFPoll              0b11100000
#define TYPE_80211_D_QoSCFAck_CFPoll        0b11110000

//To and From
#define TF_80211_StaToSta                   0b00000000
#define TF_80211_StaToAp                    0b00000001
#define TF_80211_ApToSta                    0b00000010
#define TF_80211_ApToAp                     0b00000011



struct std_80211{
    uint8_t frame_control[2];
    uint8_t address1[6];
    uint8_t address2[6];
    uint8_t address3[6];
    // uint8_t address4[6];

    char mac_addr1[19];
    char mac_addr2[19];
    char mac_addr3[19];
    // char mac_addr4[19];

    char* type;
    char* subtype;
    char* tofrom;

    uint8_t type_int;
    uint8_t tofrom_int;

    uint8_t isReserved;

};

void std_80211_clean(struct std_80211*);

void parse80211(const u_char*,int,struct std_80211*);

void convertToMacChar(uint8_t*,char*);

int addressIn(struct std_80211*,char*[],int);

int frame_control_match(uint8_t,uint8_t,uint8_t);

#endif
