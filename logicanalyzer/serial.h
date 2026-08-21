#ifndef LA_SERIAL
#define LA_SERIAL

#include <stdint.h>
#include <stddef.h>

#define LA_SERIAL_FRAME_LENGTH     16
#define LA_SERIAL_V2_DATA_LENGTH   257
#define LA_SERIAL_V2_FRAME_LENGTH  269
#define LA_SERIAL_V2R_DATA_LENGTH  1025
#define LA_SERIAL_V2R_FRAME_LENGTH 1030
#define LA_SERAIL_V2_VERSION 1

#ifndef LASerialV2Protocol
	typedef struct _la_serial_v2_protocol LASerialV2Protocol;
#endif

typedef enum {
	LA_COMMAND_NOP			            = 0,
	LA_COMMAND_POLLING_RATE         = 1,
	LA_COMMAND_READ_ENABLE          = 2,
	LA_COMMAND_BUFFER_SIZE          = 3,
	LA_COMMAND_TEST_CLOCK1_RATE 	  = 4,
	LA_COMMAND_TEST_CLOCK2_RATE 	  = 5,
	LA_COMMAND_TEST_NOISE_RATE 		  = 6,
	LA_COMMAND_TEST_NOISE_MODE 		  = 7,
  LA_COMMAND_TEST_DAC_VALUE     	= 8,
  LA_COMMAND_TEST_DAC_MODE      	= 9,
  LA_COMMAND_TEST_DAC_SPEED     	= 10,
  LA_COMMAND_TEST_DAC_WRAP      	= 11,
  LA_COMMAND_TEST_DAC_WAVE      	= 12,
  LA_COMMAND_OUT_VALUE          	= 13,
	LA_COMMAND_TEST_CLOCK1_MUTE 	  = 14,
	LA_COMMAND_TEST_CLOCK2_MUTE 	  = 15,
	LA_COMMAND_TEST_NOISE_MUTE 		  = 16,
	LA_COMMAND_TEST_DAC_MUTE 		    = 17,
  LA_COMMAND_TEST_DAC_WAVE_FULL  	= 18,
	LA_COMMAND_SEND_DAC_STREAM		  = 19
} LACommand;

typedef enum {
	LA_RX_COMMAND_NOP				= 0,
	LA_RX_COMMAND_ACK				= 1,
	LA_RX_COMMAND_CAPTURE		= 2
} LARecvCommand;

typedef enum {
	LA_PROTOCOL_V2_FLAG_ACK  = 1 << 0,
	LA_PROTOCOL_V2_FLAG_NACK = 1 << 1
} LAProtocolV2Flags;

typedef struct {
	uint8_t command;
	uint32_t value;
	uint8_t frames[LA_SERIAL_FRAME_LENGTH];
} LASerialProtocol;

struct _la_serial_v2_protocol {
	uint8_t version;
	uint8_t command;
	uint16_t length;
	uint16_t flags;
	uint16_t transactionId;
	uint16_t headerChecksum;
	uint16_t fcs;
	uint8_t data[LA_SERIAL_V2_DATA_LENGTH];
	uint8_t frames[LA_SERIAL_V2_FRAME_LENGTH];
	uint16_t frameLength;
	uint8_t isReady;
};

typedef struct {
	uint8_t command;
	uint16_t length;
	uint16_t headerChecksum;
	uint16_t fcs;
	uint8_t data[LA_SERIAL_V2R_DATA_LENGTH];
	uint8_t frames[LA_SERIAL_V2R_FRAME_LENGTH];
	uint16_t frameLength;
	uint8_t isReady;
	uint16_t offset;
} LASerialV2RecvProtocol;

// protocol
uint16_t LAModbusCRC16(uint16_t crc, uint8_t byte, bool init);
void LAPrepareProtocol(LASerialProtocol *p, uint8_t command, uint32_t value);
void LAPrepareProtocolV2Basic(LASerialV2Protocol *p, uint8_t command, uint32_t value);
uint8_t LAFuncPopcount(uint8_t value);
uint16_t LACalculateFCSProtocolV2(LASerialV2Protocol *p);
void LACompileProtocolV2Frames(LASerialV2Protocol *p);
void LAPrepareProtocolV2Wave(LASerialV2Protocol *p, uint8_t bank, uint8_t *wave, uint16_t size);
void LAPrepareProtocolV2Stream(LASerialV2Protocol *p, uint8_t *data, uint16_t size);
LAErrorCode LAProtocolV2RInit(LASerialV2RecvProtocol *p);
LAErrorCode LAProtocolV2RFill(LASerialV2RecvProtocol *p, uint8_t *data, uint16_t size);
uint8_t LAProtocolV2RIsReady(LASerialV2RecvProtocol *p);
LAErrorCode LAProtocolV2RUnpack(LASerialV2RecvProtocol *p);

#endif  //LA_SERIAL
