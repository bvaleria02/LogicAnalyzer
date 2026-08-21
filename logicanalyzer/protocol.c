#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "liblogicanalyzer.h"
#include "error.h"
#include "utils.h"
#include "serial.h"

// PLEASE, REMOVE WHEN MERGING WITH THE OTHER BRANCH, THIS IS FOR TESTING
uint16_t transactionIdAccumulator = 0;


uint16_t LAModbusCRC16(uint16_t crc, uint8_t byte, bool init){
	if(init) crc = 0xFFFF;

	crc ^= (uint16_t)byte;
	for(size_t i = 0; i < 8; i++){
		if(crc & 0x0001){
			crc = (crc >> 1) ^ 0xA001;
		} else {
			crc = (crc >> 1);
		}
	}
		
	return crc;
}

void LABigEndiandCpy32(uint8_t *dest, uint32_t value){
	if(dest == NULL) return;

	dest[0] = (value >>  0) & 0xFF;
	dest[1] = (value >>  8) & 0xFF;
	dest[2] = (value >> 16) & 0xFF;
	dest[3] = (value >> 24) & 0xFF;
}

void LAPrepareProtocol(LASerialProtocol *p, uint8_t command, uint32_t value){
	if(p == NULL) return;
	p->command = command;
	p->value   = value;

	memset(p->frames, 0, LA_SERIAL_FRAME_LENGTH);
	p->frames[0] = p->command;
	p->frames[1] = (p->value >>  0) & 0xFF;
	p->frames[2] = (p->value >>  8) & 0xFF;
	p->frames[3] = (p->value >> 16) & 0xFF;
	p->frames[4] = (p->value >> 24) & 0xFF;
}

LAErrorCode LAProtocolV2GetHeaderChecksum(LASerialV2Protocol *p, uint16_t *checksum){
	LA_CHECK_NULLPTR(p);
	LA_CHECK_NULLPTR(checksum);
	
	(*checksum) = 0xFFFF;
	(*checksum) = LAModbusCRC16((*checksum), p->version, true);
	(*checksum) = LAModbusCRC16((*checksum), p->command, false);
	(*checksum) = LAModbusCRC16((*checksum), (p->length >> 0) & 0xFF, false);
	(*checksum) = LAModbusCRC16((*checksum), (p->length >> 8) & 0xFF, false);
	(*checksum) = LAModbusCRC16((*checksum), (p->transactionId >> 0) & 0xFF, false);
	(*checksum) = LAModbusCRC16((*checksum), (p->transactionId >> 8) & 0xFF, false);
	(*checksum) = LAModbusCRC16((*checksum), (p->flags >> 0) & 0xFF, false);
	(*checksum) = LAModbusCRC16((*checksum), (p->flags >> 8) & 0xFF, false);

	return LA_NO_ERROR;
}

void LAPrepareProtocolV2Basic(LASerialV2Protocol *p, uint8_t command, uint32_t value){
	if(p == NULL) return;

	p->isReady = 0;
	p->command = command;
	p->length  = sizeof(uint32_t);
	p->transactionId = transactionIdAccumulator;
	p->version = 1;
	p->flags = LA_PROTOCOL_V2_FLAG_ACK;
	p->headerChecksum = 0;

	(void) LAProtocolV2GetHeaderChecksum(p, &(p->headerChecksum));
	
	memset(p->data, 0, LA_SERIAL_V2_DATA_LENGTH);
	LABigEndiandCpy32(&(p->data[0]), value);

	p->fcs = LACalculateFCSProtocolV2(p);
	LACompileProtocolV2Frames(p);

	transactionIdAccumulator++;
}

uint8_t LAFuncPopcount(uint8_t value){
	return __builtin_popcount(value);
}

uint16_t LACalculateFCSProtocolV2(LASerialV2Protocol *p){
	uint16_t fcs = 0;
	
	fcs += LAFuncPopcount(p->command);
	fcs += LAFuncPopcount(p->length & 0xFF);
	fcs += LAFuncPopcount(p->length >> 8);

	for(uint16_t i = 0; i < p->length; i++){
		fcs += LAFuncPopcount(p->data[i]);
	}

	return fcs;
}

LAErrorCode LAHostToProtocolU16(uint16_t src, uint8_t *dest, size_t index, size_t length){
	LA_CHECK_NULLPTR(dest);

	LAErrorCode code = LA_NO_ERROR;

	size_t index1 = 0;
	code = LACheckedSizeAdd(index, 1, &index1);
	if(code) return code;

	if(index1 >= length) return LA_ERROR_OUTOFBOUND;

	dest[index    ] = (uint8_t)(src        & 0xFF);
	dest[index + 1] = (uint8_t)((src >> 8) & 0xFF);
	return LA_NO_ERROR;
}

LAErrorCode LAHostToProtocolU32(uint32_t src, uint8_t *dest, size_t index, size_t length){
	LA_CHECK_NULLPTR(dest);

	LAErrorCode code = LA_NO_ERROR;

	size_t index3 = 0;
	code = LACheckedSizeAdd(index, 3, &index3);
	if(code) return code;

	if(index3 >= length) return LA_ERROR_OUTOFBOUND;

	dest[index    ] = (uint8_t)(src         & 0xFF);
	dest[index + 1] = (uint8_t)((src >> 8)  & 0xFF);
	dest[index + 2] = (uint8_t)((src >> 16) & 0xFF);
	dest[index + 3] = (uint8_t)((src >> 24) & 0xFF);

	return LA_NO_ERROR;
}

void LACompileProtocolV2Frames(LASerialV2Protocol *p){
	if(p == NULL) return;

	p->frames[0] = p->version;
	p->frames[1] = p->command;
	(void) LAHostToProtocolU16(p->length,         p->frames, 2, LA_SERIAL_V2_FRAME_LENGTH);
	(void) LAHostToProtocolU16(p->transactionId,  p->frames, 4, LA_SERIAL_V2_FRAME_LENGTH);
	(void) LAHostToProtocolU16(p->flags,          p->frames, 6, LA_SERIAL_V2_FRAME_LENGTH);
	(void) LAHostToProtocolU16(p->headerChecksum, p->frames, 8, LA_SERIAL_V2_FRAME_LENGTH);
	//p->frames[1] = (p->length >> 0) & 0xFF;
	//p->frames[2] = (p->length >> 8) & 0xFF; 

	for(uint16_t i = 0; i < p->length; i++){
		p->frames[7+i] = p->data[i];
	}

	(void) LAHostToProtocolU16(p->fcs, p->frames, 10 + p->length, LA_SERIAL_V2_FRAME_LENGTH);
	//p->frames[3+p->length] = (p->fcs >> 0) & 0xFF;
	//p->frames[4+p->length] = (p->fcs >> 8) & 0xFF;

	p->frameLength = 12 + p->length;
	p->isReady = 1;
}

void LAPrepareProtocolV2Wave(LASerialV2Protocol *p, uint8_t bank, uint8_t *wave, uint16_t size){
	if(p == NULL) return;
	if(wave == NULL) return;

	p->isReady = 0;
	p->command = LA_COMMAND_TEST_DAC_WAVE_FULL;
	p->length  = size + 1;
	(void) LAProtocolV2GetHeaderChecksum(p, &(p->headerChecksum));
	
	memset(p->data, 0, LA_SERIAL_V2_DATA_LENGTH);
	p->data[0] = bank;
	memcpy(&(p->data[1]), wave, size);

	p->fcs = LACalculateFCSProtocolV2(p);
	LACompileProtocolV2Frames(p);
}

void LAPrepareProtocolV2Stream(LASerialV2Protocol *p, uint8_t *data, uint16_t size){
	if(p == NULL) return;
	if(data == NULL) return;

	p->isReady = 0;
	p->command = LA_COMMAND_SEND_DAC_STREAM;
	p->length  = size;
	(void) LAProtocolV2GetHeaderChecksum(p, &(p->headerChecksum));
	
	memcpy(p->data, data, size);

	p->fcs = LACalculateFCSProtocolV2(p);
	LACompileProtocolV2Frames(p);
}

LAErrorCode LAProtocolV2RInit(LASerialV2RecvProtocol *p){
	LA_HANDLE_NULLPTR(p,	LA_PROPAGATE_ERROR);

	p->command = LA_COMMAND_NOP;
	p->length = 0;
	p->headerChecksum = 0;
	p->fcs = 0;
	p->isReady = 0;
	p->offset = 0;

	return LA_NO_ERROR;
}

LAErrorCode LAProtocolV2RFill(LASerialV2RecvProtocol *p, uint8_t *data, uint16_t size){
	LA_HANDLE_NULLPTR(p,		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(data,		LA_PROPAGATE_ERROR);

	for(uint16_t i = 0; i < size; i++){
		if(p->offset >= LA_SERIAL_V2R_FRAME_LENGTH){
			p->isReady = 1;
			break;
		}

		p->frames[p->offset] = data[i];
		p->offset += 1;
	}

	return LA_NO_ERROR;
}

uint8_t LAProtocolV2RIsReady(LASerialV2RecvProtocol *p){
	if(p == NULL) return 0;

	if(p->offset >= 3){
		p->length  = p->frames[1] | (p->frames[2] << 8);
		if(p->length > LA_SERIAL_V2R_DATA_LENGTH){
			p->length = LA_SERIAL_V2R_DATA_LENGTH;
		}
		printf("Length: %i\n", p->length);
	}

	return ((p->length + 5) <= p->offset);
}

LAErrorCode LAProtocolV2RUnpack(LASerialV2RecvProtocol *p){
	LA_HANDLE_NULLPTR(p,	LA_PROPAGATE_ERROR);

	p->command = p->frames[0];
	p->length  = p->frames[1] | (p->frames[2] << 8);
	if(p->length > LA_SERIAL_V2R_DATA_LENGTH){
		p->length = LA_SERIAL_V2R_DATA_LENGTH;
	}

	memcpy(p->data, &(p->frames[3]), p->length);
	p->fcs = p->frames[3+p->length] | (p->frames[4+p->length] << 8);
	p->isReady = 1;

	return LA_NO_ERROR;
}
