#include "liblogicanalyzer.h"
#include <stdlib.h>
#include <stdint.h>

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
