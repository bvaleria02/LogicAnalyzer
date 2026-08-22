#include <gtk/gtk.h>
#include <cairo.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "liblogicanalyzer.h"

double defaultChannelColors[MAX_CHANNEL_COUNT][RGB_COUNT] = {
	{0.5, 	0.7, 	1},	
	{0.3, 	0.5,  0.4},	
	{0.7, 	0.4,  0.7},	
	{0.2, 	0.3,  0.8},	
	{0.8, 	0.1,  0.6},	
	{0.4, 	0.6,  0.9},	
	{0.9, 	0.4,  0.7},	
	{1, 	0.6,  0.4}	
};

double defaultMidLineColor[RGB_COUNT] = {
	0.7, 0.7, 0.7
};
