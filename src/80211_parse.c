#include "80211_parse.h"

//struct 80211 constans
static char* CONSTANS_80211_TYPE_Management                 ="Management";
static char* CONSTANS_80211_TYPE_Control                    ="Control";
static char* CONSTANS_80211_TYPE_Data                       ="Data";
static char* CONSTANS_80211_TYPE_Reserved                   ="Reserved";

static char* CONSTANS_80211_SUBTYPE_AssociationRequest      ="Association request";
static char* CONSTANS_80211_SUBTYPE_AssociationResponse     ="Association response";
static char* CONSTANS_80211_SUBTYPE_ReassociationRequest    ="Reassociation request";
static char* CONSTANS_80211_SUBTYPE_ReassociationResponse   ="Reassociation response";
static char* CONSTANS_80211_SUBTYPE_ProbeRequest            ="Probe request";
static char* CONSTANS_80211_SUBTYPE_ProbeResponse           ="Probe response";
static char* CONSTANS_80211_SUBTYPE_TimingAdvertisement     ="Timing Advertisement";
static char* CONSTANS_80211_SUBTYPE_Beacon                  ="Beacon";
static char* CONSTANS_80211_SUBTYPE_ATIM                    ="ATIM";
static char* CONSTANS_80211_SUBTYPE_Disassociation          ="Disassociation";
static char* CONSTANS_80211_SUBTYPE_Authentication          ="Authentication";
static char* CONSTANS_80211_SUBTYPE_Deauthentication        ="Deauthentication";
static char* CONSTANS_80211_SUBTYPE_Action                  ="Action";
static char* CONSTANS_80211_SUBTYPE_ActionNoAck             ="Action No Ack";

static char* CONSTANS_80211_SUBTYPE_ControlWrapper          ="Control Wrapper";
static char* CONSTANS_80211_SUBTYPE_BlockAckReq             ="Block Ack Request";
static char* CONSTANS_80211_SUBTYPE_BlockAck                ="Block Ack";
static char* CONSTANS_80211_SUBTYPE_PSPoll                  ="PS-Poll";
static char* CONSTANS_80211_SUBTYPE_RTS                     ="RTS";
static char* CONSTANS_80211_SUBTYPE_CTS                     ="CTS";
static char* CONSTANS_80211_SUBTYPE_ACK                     ="ACK";
static char* CONSTANS_80211_SUBTYPE_CFEnd                   ="CF-End";
static char* CONSTANS_80211_SUBTYPE_CFEnd_CFAck             ="CF-End+CF-Ack";

static char* CONSTANS_80211_SUBTYPE_Data                    ="Data";
static char* CONSTANS_80211_SUBTYPE_Data_CFAck              ="Data+CF-Ack";
static char* CONSTANS_80211_SUBTYPE_Data_CFPoll             ="Data+CF-Poll";
static char* CONSTANS_80211_SUBTYPE_Data_CFAck_CFPoll       ="Data+CF-Ack+CF-Poll";
static char* CONSTANS_80211_SUBTYPE_Null                    ="Null (no data)";
static char* CONSTANS_80211_SUBTYPE_CFAck                   ="CF-Ack(no data)";
static char* CONSTANS_80211_SUBTYPE_CFPoll                  ="CF-Poll(no data)";
static char* CONSTANS_80211_SUBTYPE_CFAck_CFPoll            ="CF-Ack+CF-Poll(no data)";
static char* CONSTANS_80211_SUBTYPE_QoSData                 ="QoS Data";
static char* CONSTANS_80211_SUBTYPE_QoSData_CFAck           ="QoS Data+CF-Ack";
static char* CONSTANS_80211_SUBTYPE_QoSData_CFPoll          ="QoS Data+CF-Poll";
static char* CONSTANS_80211_SUBTYPE_QoSData_CFAck_CFPoll    ="QoS Data+CF-Ack + CF-Poll";
static char* CONSTANS_80211_SUBTYPE_QoSNull                 ="QoS Null(no data)";
static char* CONSTANS_80211_SUBTYPE_QoSCFPoll               ="QoS CF-Poll(no data)";
static char* CONSTANS_80211_SUBTYPE_QoSCFAck_CFPoll         ="QoS CF-Ack+CF-Poll(no data)";
static char* CONSTANS_80211_SUBTYPE_Reserved                ="Reserved";


static char* CONSTANS_80211_TF_StaToSta                     ="Sta To Sta";
static char* CONSTANS_80211_TF_StaToAp                      ="Sta To Ap";
static char* CONSTANS_80211_TF_ApToSta                      ="Ap To Sta";
static char* CONSTANS_80211_TF_ApToAp                       ="Ap To Ap";


int frame_control_match(uint8_t frame,uint8_t contant,uint8_t mack_code){
    return ((frame&mack_code)^(contant))==0;
};

void parse80211(const u_char* data,int data_len,struct std_80211* s_80211){
    if(s_80211!=NULL){std_80211_clean(s_80211);}
    u_char* p=(u_char*)data;

    if(p-data>=data_len) goto parse_end;
    s_80211->frame_control[0]=*(uint8_t*)p;
    p++;
    s_80211->frame_control[1]=*(uint8_t*)p;
    p+=3;
    if(p-data>=data_len) goto parse_end;
    for(int i=0;i<6;i++){
        s_80211->address1[i]=*(uint8_t*)p;
        p+=sizeof(uint8_t);
    }
    convertToMacChar(s_80211->address1,s_80211->mac_addr1);

    if(p-data>=data_len) goto parse_end;
    for(int i=0;i<6;i++){
        s_80211->address2[i]=*(uint8_t*)p;
        p+=sizeof(uint8_t);
    }
    convertToMacChar(s_80211->address2,s_80211->mac_addr2);

    if(p-data>=data_len) goto parse_end;
    for(int i=0;i<6;i++){
        s_80211->address3[i]=*(uint8_t*)p;
        p+=sizeof(uint8_t);
    }
    convertToMacChar(s_80211->address3,s_80211->mac_addr3);

parse_end:
    if(frame_control_match(s_80211->frame_control[0],TYPE_80211_Management,TYPE_80211_Mask_Code)){
        s_80211->type=CONSTANS_80211_TYPE_Management;

        if(frame_control_match(s_80211->frame_control[0],TYPE_80211_M_AssociationRequest,SUBTYPE_80211_Mask_Code)){
            s_80211->subtype=CONSTANS_80211_SUBTYPE_AssociationRequest;
        }else if(frame_control_match(s_80211->frame_control[0],TYPE_80211_M_AssociationResponse,SUBTYPE_80211_Mask_Code)){
            s_80211->subtype=CONSTANS_80211_SUBTYPE_AssociationResponse;
        }else if(frame_control_match(s_80211->frame_control[0],TYPE_80211_M_ReassociationRequest,SUBTYPE_80211_Mask_Code)){
            s_80211->subtype=CONSTANS_80211_SUBTYPE_ReassociationRequest;
        }else if(frame_control_match(s_80211->frame_control[0],TYPE_80211_M_ReassociationResponse,SUBTYPE_80211_Mask_Code)){
            s_80211->subtype=CONSTANS_80211_SUBTYPE_ReassociationResponse;
        }else if(frame_control_match(s_80211->frame_control[0],TYPE_80211_M_ProbeRequest,SUBTYPE_80211_Mask_Code)){
            s_80211->subtype=CONSTANS_80211_SUBTYPE_ProbeRequest;
        }else if(frame_control_match(s_80211->frame_control[0],TYPE_80211_M_ProbeResponse,SUBTYPE_80211_Mask_Code)){
            s_80211->subtype=CONSTANS_80211_SUBTYPE_ProbeResponse;
        }else if(frame_control_match(s_80211->frame_control[0],TYPE_80211_M_TimingAdvertisement,SUBTYPE_80211_Mask_Code)){
            s_80211->subtype=CONSTANS_80211_SUBTYPE_TimingAdvertisement;
        }else if(frame_control_match(s_80211->frame_control[0],TYPE_80211_M_Beacon,SUBTYPE_80211_Mask_Code)){
            s_80211->subtype=CONSTANS_80211_SUBTYPE_Beacon;
        }else if(frame_control_match(s_80211->frame_control[0],TYPE_80211_M_ATIM,SUBTYPE_80211_Mask_Code)){
            s_80211->subtype=CONSTANS_80211_SUBTYPE_ATIM;
        }else if(frame_control_match(s_80211->frame_control[0],TYPE_80211_M_Disassociation,SUBTYPE_80211_Mask_Code)){
            s_80211->subtype=CONSTANS_80211_SUBTYPE_Disassociation;
        }else if(frame_control_match(s_80211->frame_control[0],TYPE_80211_M_Authentication,SUBTYPE_80211_Mask_Code)){
            s_80211->subtype=CONSTANS_80211_SUBTYPE_Authentication;
        }else if(frame_control_match(s_80211->frame_control[0],TYPE_80211_M_Deauthentication,SUBTYPE_80211_Mask_Code)){
            s_80211->subtype=CONSTANS_80211_SUBTYPE_Deauthentication;
        }else if(frame_control_match(s_80211->frame_control[0],TYPE_80211_M_Action,SUBTYPE_80211_Mask_Code)){
            s_80211->subtype=CONSTANS_80211_SUBTYPE_Action;
        }else if(frame_control_match(s_80211->frame_control[0],TYPE_80211_M_ActionNoAck,SUBTYPE_80211_Mask_Code)){
            s_80211->subtype=CONSTANS_80211_SUBTYPE_ActionNoAck;
        }else{
            s_80211->subtype=CONSTANS_80211_SUBTYPE_Reserved;
            s_80211->isReserved=1;
        }

    }else if(frame_control_match(s_80211->frame_control[0],TYPE_80211_Control,TYPE_80211_Mask_Code)){
        s_80211->type=CONSTANS_80211_TYPE_Control;

        if(frame_control_match(s_80211->frame_control[0],TYPE_80211_C_ControlWrapper,SUBTYPE_80211_Mask_Code)){
            s_80211->subtype=CONSTANS_80211_SUBTYPE_ControlWrapper;
        }else if(frame_control_match(s_80211->frame_control[0],TYPE_80211_C_BlockAckReq,SUBTYPE_80211_Mask_Code)){
            s_80211->subtype=CONSTANS_80211_SUBTYPE_BlockAckReq;
        }else if(frame_control_match(s_80211->frame_control[0],TYPE_80211_C_BlockAck,SUBTYPE_80211_Mask_Code)){
            s_80211->subtype=CONSTANS_80211_SUBTYPE_BlockAck;
        }else if(frame_control_match(s_80211->frame_control[0],TYPE_80211_C_PSPoll,SUBTYPE_80211_Mask_Code)){
            s_80211->subtype=CONSTANS_80211_SUBTYPE_PSPoll;
        }else if(frame_control_match(s_80211->frame_control[0],TYPE_80211_C_RTS,SUBTYPE_80211_Mask_Code)){
            s_80211->subtype=CONSTANS_80211_SUBTYPE_RTS;
        }else if(frame_control_match(s_80211->frame_control[0],TYPE_80211_C_CTS,SUBTYPE_80211_Mask_Code)){
            s_80211->subtype=CONSTANS_80211_SUBTYPE_CTS;
        }else if(frame_control_match(s_80211->frame_control[0],TYPE_80211_C_ACK,SUBTYPE_80211_Mask_Code)){
            s_80211->subtype=CONSTANS_80211_SUBTYPE_ACK;
        }else if(frame_control_match(s_80211->frame_control[0],TYPE_80211_C_CFEnd,SUBTYPE_80211_Mask_Code)){
            s_80211->subtype=CONSTANS_80211_SUBTYPE_CFEnd;
        }else if(frame_control_match(s_80211->frame_control[0],TYPE_80211_C_CFEnd_CFAck,SUBTYPE_80211_Mask_Code)){
            s_80211->subtype=CONSTANS_80211_SUBTYPE_CFEnd_CFAck;
        }else{
            s_80211->subtype=CONSTANS_80211_SUBTYPE_Reserved;
            s_80211->isReserved=1;
        }
    }else if(frame_control_match(s_80211->frame_control[0],TYPE_80211_Data,TYPE_80211_Mask_Code)){
        s_80211->type=CONSTANS_80211_TYPE_Data;

        if(frame_control_match(s_80211->frame_control[0],TYPE_80211_D_Data,SUBTYPE_80211_Mask_Code)){
            s_80211->subtype=CONSTANS_80211_SUBTYPE_Data;
        }else if(frame_control_match(s_80211->frame_control[0],TYPE_80211_D_Data_CFAck,SUBTYPE_80211_Mask_Code)){
            s_80211->subtype=CONSTANS_80211_SUBTYPE_Data_CFAck;
        }else if(frame_control_match(s_80211->frame_control[0],TYPE_80211_D_Data_CFPoll,SUBTYPE_80211_Mask_Code)){
            s_80211->subtype=CONSTANS_80211_SUBTYPE_Data_CFPoll;
        }else if(frame_control_match(s_80211->frame_control[0],TYPE_80211_D_Data_CFAck_CFPoll,SUBTYPE_80211_Mask_Code)){
            s_80211->subtype=CONSTANS_80211_SUBTYPE_Data_CFAck_CFPoll;
        }else if(frame_control_match(s_80211->frame_control[0],TYPE_80211_D_Null,SUBTYPE_80211_Mask_Code)){
            s_80211->subtype=CONSTANS_80211_SUBTYPE_Null;
        }else if(frame_control_match(s_80211->frame_control[0],TYPE_80211_D_CFAck,SUBTYPE_80211_Mask_Code)){
            s_80211->subtype=CONSTANS_80211_SUBTYPE_CFAck;
        }else if(frame_control_match(s_80211->frame_control[0],TYPE_80211_D_CFPoll,SUBTYPE_80211_Mask_Code)){
            s_80211->subtype=CONSTANS_80211_SUBTYPE_CFPoll;
        }else if(frame_control_match(s_80211->frame_control[0],TYPE_80211_D_CFAck_CFPoll,SUBTYPE_80211_Mask_Code)){
            s_80211->subtype=CONSTANS_80211_SUBTYPE_CFAck_CFPoll;
        }else if(frame_control_match(s_80211->frame_control[0],TYPE_80211_D_QoSData,SUBTYPE_80211_Mask_Code)){
            s_80211->subtype=CONSTANS_80211_SUBTYPE_QoSData;
        }else if(frame_control_match(s_80211->frame_control[0],TYPE_80211_D_QoSData_CFAck,SUBTYPE_80211_Mask_Code)){
            s_80211->subtype=CONSTANS_80211_SUBTYPE_QoSData_CFAck;
        }else if(frame_control_match(s_80211->frame_control[0],TYPE_80211_D_QoSData_CFPoll,SUBTYPE_80211_Mask_Code)){
            s_80211->subtype=CONSTANS_80211_SUBTYPE_QoSData_CFPoll;
        }else if(frame_control_match(s_80211->frame_control[0],TYPE_80211_D_QoSData_CFAck_CFPoll,SUBTYPE_80211_Mask_Code)){
            s_80211->subtype=CONSTANS_80211_SUBTYPE_QoSData_CFAck_CFPoll;
        }else if(frame_control_match(s_80211->frame_control[0],TYPE_80211_D_QoSNull,SUBTYPE_80211_Mask_Code)){
            s_80211->subtype=CONSTANS_80211_SUBTYPE_QoSNull;
        }else if(frame_control_match(s_80211->frame_control[0],TYPE_80211_D_QoSCFPoll,SUBTYPE_80211_Mask_Code)){
            s_80211->subtype=CONSTANS_80211_SUBTYPE_QoSCFPoll;
        }else if(frame_control_match(s_80211->frame_control[0],TYPE_80211_D_QoSCFAck_CFPoll,SUBTYPE_80211_Mask_Code)){
            s_80211->subtype=CONSTANS_80211_SUBTYPE_QoSCFAck_CFPoll;
        }else{
            s_80211->subtype=CONSTANS_80211_SUBTYPE_Reserved;
            s_80211->isReserved=1;
        }
    }else{
        s_80211->type=CONSTANS_80211_TYPE_Reserved;
        s_80211->subtype=CONSTANS_80211_SUBTYPE_Reserved;
        s_80211->isReserved=1;
    }

    if(frame_control_match(s_80211->frame_control[1],TF_80211_StaToSta,TF_80211_Mask_Code)){
        s_80211->tofrom=CONSTANS_80211_TF_StaToSta;
    }else if(frame_control_match(s_80211->frame_control[1],TF_80211_StaToAp,TF_80211_Mask_Code)){
        s_80211->tofrom=CONSTANS_80211_TF_StaToAp;
    }else if(frame_control_match(s_80211->frame_control[1],TF_80211_ApToSta,TF_80211_Mask_Code)){
        s_80211->tofrom=CONSTANS_80211_TF_ApToSta;
    }else if(frame_control_match(s_80211->frame_control[1],TF_80211_ApToAp,TF_80211_Mask_Code)){
        s_80211->tofrom=CONSTANS_80211_TF_ApToAp;
    }

};

void std_80211_clean(struct std_80211* s){
    memset(s,0,sizeof(struct std_80211));
    strcpy(s->mac_addr1,"00:00:00:00:00:00");
    strcpy(s->mac_addr2,s->mac_addr1);
    strcpy(s->mac_addr3,s->mac_addr1);
};

void convertToMacChar(uint8_t* address,char* mac_char){
    sprintf(mac_char,"%02x:%02x:%02x:%02x:%02x:%02x"
        ,address[0],address[1],address[2],address[3],address[4],address[5]);
};

int addressIn(struct std_80211* s_80211,char* mac_addr[],int len){
    for(int i=0;i<len;i++){
        if(strcmp(s_80211->mac_addr1,mac_addr[i])==0) return 1;
        if(strcmp(s_80211->mac_addr2,mac_addr[i])==0) return 1;
        if(strcmp(s_80211->mac_addr3,mac_addr[i])==0) return 1;
        // if(strcmp(s_80211->mac_addr4,mac_addr[i])==0) return 1;
    }
    return 0;

};