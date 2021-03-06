#include "radiotap_parse.h"

int parseRadioHeader(const u_char* pkg_data,struct std_rt_header* header){
	if(header==NULL){
		return 0;
	}
	radioHeader_clean(header);
	header->len=*(uint16_t*)(pkg_data+2);
	u_char* p=malloc(header->len+1);
	u_char* read_p=p;
	p[header->len]=0;
	memcpy(p,pkg_data,header->len);

	header->version=*(uint8_t*)read_p;
	read_p+=2;
	header->len=convertToLittleEndian16(read_p);
	read_p+=sizeof(uint16_t);
	header->present=convertToLittleEndian32(read_p);

	read_p+=sizeof(uint32_t);
	uint32_t search_present=header->present;
	while((search_present&(1<<RADIOTAP_PRESENC_EXT))){
		header->has_ExtendedPresence=1;
		search_present=convertToLittleEndian32(read_p);
		read_p+=sizeof(uint32_t);
	}

    if(convertToLittleEndian32(read_p)==0){// TL-WR702N Router have 32bit 0 after PRESENT
        read_p+=sizeof(uint32_t);
    }

	if(header->present&(1<<RADIOTAP_PRESENC_TSFT)){
		header->has_TSFT=1;
		header->v_TSFT=convertToLittleEndian64(read_p);
		read_p+=sizeof(uint64_t);
	}
	if(header->present&(1<<RADIOTAP_PRESENC_FLAGS)){
		header->has_Flags=1;
		header->v_Flags=*(uint8_t*)read_p;
		read_p+=sizeof(uint8_t);
	}
	if(header->present&(1<<RADIOTAP_PRESENC_RATE)){
		header->has_Rate=1;
		header->v_Flags=*(uint8_t*)read_p;
		read_p+=sizeof(uint8_t);
	}
	if(header->present&(1<<RADIOTAP_PRESENC_CHANNEL)){
		header->has_Channel=1;
		header->v_Channel_Frequency=convertToLittleEndian16(read_p);
		read_p+=sizeof(uint16_t);
		header->v_Channel_Flags=convertToLittleEndian16(read_p);
		read_p+=sizeof(uint16_t);
	}
	if(header->present&(1<<RADIOTAP_PRESENC_FHSS)){
		header->has_FHSS=1;
		header->v_FHSS_HopSet=*(uint8_t*)read_p;
		read_p+=sizeof(uint8_t);
		header->v_FHSS_HopPattern=*(uint8_t*)read_p;
		read_p+=sizeof(uint8_t);
	}
	if(header->present&(1<<RADIOTAP_PRESENC_DBM_ANTSIGNAL)){
		header->has_AntennaSignal=1;
		header->v_AntennaSignal=*(int8_t*)read_p;
		read_p+=sizeof(int8_t);
	}
	if(header->present&(1<<RADIOTAP_PRESENC_DBM_ANTNOISE)){
		header->has_AntennaNoise=1;
		header->v_AntennaNoise=*(int8_t*)read_p;
		read_p+=sizeof(int8_t);
	}
	if(header->present&(1<<RADIOTAP_PRESENC_LOCK_QUALITY)){
		header->has_LockQuality=1;
		header->v_LockQualit=convertToLittleEndian16(read_p);
		read_p+=sizeof(uint16_t);
	}
	if(header->present&(1<<RADIOTAP_PRESENC_TX_ATTENUATION)){
		header->has_TXAttenuation=1;
		header->v_TXAttenuation=convertToLittleEndian16(read_p);
		read_p+=sizeof(uint16_t);
	}
	if(header->present&(1<<RADIOTAP_PRESENC_DB_TX_ATTENUATION)){
		header->has_dBTXAttenuation=1;
		header->v_dBTXAttenuation=convertToLittleEndian16(read_p);
		read_p+=sizeof(uint16_t);
	}
	if(header->present&(1<<RADIOTAP_PRESENC_DBM_TX_POWER)){
		header->has_dBmTXPower=1;
		header->v_dBmTXPower=*(int8_t*)read_p;
		read_p+=sizeof(int8_t);
	}
	if(header->present&(1<<RADIOTAP_PRESENC_ANTENNA)){
		header->has_Antenna=1;
		header->v_Antenna=*(uint8_t*)read_p;
		read_p+=sizeof(uint8_t);
	}
	if(header->present&(1<<RADIOTAP_PRESENC_DB_ANTSIGNAL)){
		header->has_dBAntennaSignal=1;
		header->v_dBAntennaSignal=*(uint8_t*)read_p;
		read_p+=sizeof(uint8_t);
	}
	if(header->present&(1<<RADIOTAP_PRESENC_DB_ANTNOISE)){
		header->has_dBAntennaNoise=1;
		header->v_dBAntennaNoise=*(uint8_t*)read_p;
		read_p+=sizeof(uint8_t);
	}
	if(header->present&(1<<RADIOTAP_PRESENC_RX_FLAGS)){
		header->has_RXFlags=1;
		header->v_RXFlags=convertToLittleEndian16(read_p);
		read_p+=sizeof(uint16_t);
	}
	if(header->present&(1<<RADIOTAP_PRESENC_MCS)){
		header->has_MCS=1;
		header->v_MCS_Known=*(uint8_t*)read_p;
		read_p+=sizeof(uint8_t);
		header->v_MCS_Flags=*(uint8_t*)read_p;
		read_p+=sizeof(uint8_t);
		header->v_MCS_Mcs=*(uint8_t*)read_p;
		read_p+=sizeof(uint8_t);
	}
	if(header->present&(1<<RADIOTAP_PRESENC_AMPDU_STATUS)){
		header->has_AMPDUStatus=1;
		header->v_AMPDUStatus_RefNum=convertToLittleEndian32(read_p);
		read_p+=sizeof(uint32_t);
		header->v_AMPDUStatus_Flags=convertToLittleEndian16(read_p);
		read_p+=sizeof(uint16_t);
		header->v_AMPDUStatus_CRC=*(uint8_t*)read_p;
		read_p+=sizeof(uint8_t);
		header->v_AMPDUStatus_Reserved=*(uint8_t*)read_p;
		read_p+=sizeof(uint8_t);
	}
	if(header->present&(1<<RADIOTAP_PRESENC_VHT)){
		header->has_VHT=1;
		header->v_VHT_Known=convertToLittleEndian16(read_p);
		read_p+=sizeof(uint16_t);
		header->v_VHT_Flags=*(uint8_t*)read_p;
		read_p+=sizeof(uint8_t);
		header->v_VHT_Bandwidth=*(uint8_t*)read_p;
		read_p+=sizeof(uint8_t);
		header->v_VHT_mcs_nss[0]=*(uint8_t*)read_p;
		read_p+=sizeof(uint8_t);
		header->v_VHT_mcs_nss[1]=*(uint8_t*)read_p;
		read_p+=sizeof(uint8_t);
		header->v_VHT_mcs_nss[2]=*(uint8_t*)read_p;
		read_p+=sizeof(uint8_t);
		header->v_VHT_mcs_nss[3]=*(uint8_t*)read_p;
		read_p+=sizeof(uint8_t);
		header->v_VHT_Coding=*(uint8_t*)read_p;
		read_p+=sizeof(uint8_t);
		header->v_VHT_Groupid=*(uint8_t*)read_p;
		read_p+=sizeof(uint8_t);
		header->v_VHT_Partialaid=convertToLittleEndian16(read_p);
		read_p+=sizeof(uint16_t);
	}

	free(p);
	return header->len;
};

void radioHeader_clean(struct std_rt_header* header){
    header->version=0;
    header->len=0;
    header->present=0;
	header->has_ExtendedPresence=0;
    header->has_TSFT=0;
    header->v_TSFT=0;
    header->has_Flags=0;
    header->v_Flags=0;
    header->has_Rate=0;
    header->v_Rate=0;
    header->has_Channel=0;
    header->v_Channel_Frequency=0;
    header->v_Channel_Flags=0;
    header->has_FHSS=0;
    header->v_FHSS_HopSet=0;
    header->v_FHSS_HopPattern=0;
    header->has_AntennaSignal=0;
    header->v_AntennaSignal=0;
    header->has_AntennaNoise=0;
    header->v_AntennaNoise=0;
    header->has_LockQuality=0;
    header->v_LockQualit=0;
    header->has_TXAttenuation=0;
    header->v_TXAttenuation=0;
    header->has_dBTXAttenuation=0;
    header->v_dBTXAttenuation=0;
    header->has_dBmTXPower=0;
    header->v_dBmTXPower=0;
    header->has_Antenna=0;
    header->v_Antenna=0;
    header->has_dBAntennaSignal=0;
    header->v_dBAntennaSignal=0;
    header->has_dBAntennaNoise=0;
    header->v_dBAntennaNoise=0;
    header->has_RXFlags=0;
    header->v_RXFlags=0;
    header->has_MCS=0;
    header->v_MCS_Known=0;
    header->v_MCS_Flags=0;
    header->v_MCS_Mcs=0;
    header->has_AMPDUStatus=0;
    header->v_AMPDUStatus_RefNum=0;
    header->v_AMPDUStatus_Flags=0;
    header->v_AMPDUStatus_CRC=0;
    header->v_AMPDUStatus_Reserved=0;
    header->has_VHT=0;
    header->v_VHT_Known=0;
    header->v_VHT_Flags=0;
    header->v_VHT_Bandwidth=0;
    header->v_VHT_mcs_nss[0]=0;
    header->v_VHT_mcs_nss[1]=0;
    header->v_VHT_mcs_nss[2]=0;
    header->v_VHT_mcs_nss[3]=0;
    header->v_VHT_Coding=0;
    header->v_VHT_Groupid=0;
    header->v_VHT_Partialaid=0;
};
