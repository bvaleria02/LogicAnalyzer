#include <gtk/gtk.h>
#include <cairo.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "logicanalyzer/liblogicanalyzer.h"
#include <math.h>

LAWindow *lawp;

int main(int argc, char **argv){
	gtk_init(&argc, &argv);

	LAWindow law;
	lawp = &law;

	LAWindowCreate(&law);
	LAWindowCreateMenu(&law);
//	LAWindowCreateConnect(&law);
	LAWindowCreateChannels(&law);
	LAPlaceStatusBar(&law);

	for(uint16_t i = 0; i < LA_LARGE_BUFFER_SIZE; i++){
		law.dataBuffer[i] = ((int) i & 0xFF);
	}

	LAWindowRun(&law);
	return 0;
}
