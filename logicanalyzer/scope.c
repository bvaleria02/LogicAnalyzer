#include <gtk/gtk.h>
#include <cairo.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "liblogicanalyzer.h"
#include <string.h>
#include <time.h>
#include <pthread.h>

uint16_t LAGetDataBufferIndexScope(LAWindow *law, uint16_t i){
	int32_t index = (i + law->rd.dataOffset + law->rd.scopeOffset);
	index -= (LA_BUFFER_SIZE / LAGetZoomMultiplier(law));
	if(index < 0){
		index = index * -1;
		index = index % LA_LARGE_BUFFER_SIZE;
		index = (LA_LARGE_BUFFER_SIZE - 1) - index;
	} else {
		index = index % LA_LARGE_BUFFER_SIZE;
	}

	return (uint16_t) index;
}

void LAGetWindowLowerSample(LAWindow *law, uint16_t *lower, uint32_t *upper, uint16_t *size){
	if(law == NULL || lower == NULL || upper == NULL || size == NULL){
		return;
	}

	(*size)	 = LA_BUFFER_SIZE / LAGetZoomMultiplier(law);
	(*lower) = LAGetDataBufferIndexScope(law, 0);
	(*upper) = ((uint32_t)((*lower) + (*size))) % LA_LARGE_BUFFER_SIZE;
}


void LAChannelDrawScopeData(cairo_t *cr, uint8_t index, uint8_t channelBit){
	double x0 = 0;
	double x1 = 0;
	double y0 = (SCOPE_HEIGTH / 5); 
	double y1 = (SCOPE_HEIGTH / 5); 
	uint8_t bit;

	LAChannel *channel = &(lawp->channel[index]);

	cairo_set_line_width(cr, 1.5);
	cairo_set_source_rgb(cr, channel->r, channel->g, channel->b);
	cairo_move_to(cr, x0, y0);

	double zoomMultiplier = LAGetZoomMultiplier(lawp);
/*
	for(uint16_t i = 0; i < LA_LARGE_BUFFER_SIZE; i++){
		bit = !((lawp->dataBuffer[LAGetDataBufferIndexScope(lawp, i)] >> channelBit) & 0x1);
//		if(i == 0){
//			g_print("Offset: %i\n", LAGetDataBufferIndexScope(lawp, i));
//		}

		if(x1 > SCOPE_WIDTH){
			break;
		}

		y0 = y1;
		x0 = x1;

		x1 = (SCOPE_WIDTH / (double) LA_BUFFER_SIZE) * i * zoomMultiplier;
		y1 = (SCOPE_HEIGTH /5) * (1 + bit * 3);
		
		cairo_move_to(cr, x0, y0);
		cairo_line_to(cr, x1, y0);
		cairo_line_to(cr, x1, y1);
		cairo_stroke(cr);
	}
*/
	double nf = 0;

	pthread_mutex_lock(&(lawp->mutexes.dataBufferAccess));

	for(uint16_t i = 0; i < SCOPE_WIDTH; i++){
		nf = (LA_BUFFER_SIZE / (double) LAGetZoomMultiplier(lawp)) * (i / (double) SCOPE_WIDTH);
		bit = !((lawp->dataBuffer[LAGetDataBufferIndexScope(lawp, (uint16_t) nf)] >> channelBit) & 0x1);

		y0 = y1;
		x0 = x1;

		x1 = i;
		y1 = (SCOPE_HEIGTH /5) * (1 + bit * 3);
		
		cairo_move_to(cr, x0, y0);
		cairo_line_to(cr, x1, y0);
		cairo_line_to(cr, x1, y1);
	}
	
	cairo_stroke(cr);
	pthread_mutex_unlock(&(lawp->mutexes.dataBufferAccess));

}

void LAChannelDrawScopeLines(cairo_t *cr){
	cairo_set_line_width(cr, 1);
	CAIRO_COLOR_FROM_ARRAY(cr, defaultMidLineColor);
	cairo_move_to(cr, 0,           SCOPE_HEIGTH / 2);
	cairo_line_to(cr, SCOPE_WIDTH, SCOPE_HEIGTH / 2);
	cairo_stroke(cr);
}

void LAScopePutIntValue(cairo_t *cr, uint16_t x, uint16_t y, int64_t value){
	char buffer[LA_SMALL_BUFFER_SIZE];
	snprintf(buffer, LA_SMALL_BUFFER_SIZE, "%li", value);
	cairo_move_to(cr, x, y);
	cairo_show_text(cr, buffer);
	cairo_stroke(cr);
}

uint64_t LAConvertTimespecNano(struct timespec *ts){
	uint64_t nsec = 0;
	nsec += ts->tv_nsec; 
	nsec += (ts->tv_sec * 1000000000);
	return nsec;
}

int64_t LAGetNSecBySamples(LAWindow *law, int64_t samples){
	return (samples * law->rd.pollingTime) * 1000;
}

void LAScopePutNSecTime(cairo_t *cr, uint16_t x, uint16_t y, int64_t nsecTotal){
	char buffer[LA_SMALL_BUFFER_SIZE];
	char sign = ' ';

	if(nsecTotal < 0){
		sign = '-';
		nsecTotal *= -1;
	}

	nsecTotal = nsecTotal / 1000;

	uint64_t usec = (nsecTotal) % 1000;
	nsecTotal -= usec;
	nsecTotal /= 1000;

	uint64_t msec = (nsecTotal) % 1000;
	nsecTotal -= usec;

	uint64_t sec = (nsecTotal / 1000) % 60;
	nsecTotal -= sec * 1000;

	uint64_t min = (nsecTotal / (60 * 1000)) % 60;
	nsecTotal -= min * (60 * 1000);

	uint64_t hour = (nsecTotal / (60 * 60 * 1000));
	nsecTotal -= hour * (60 * 60 * 1000);

	snprintf(buffer, LA_SMALL_BUFFER_SIZE, "%c%02li:%02li:%02li.%03li.%03li", sign, hour, min, sec, msec, usec);
	cairo_move_to(cr, x, y);
	cairo_show_text(cr, buffer);
	cairo_stroke(cr);
}

void LAScopePutAbsTime(cairo_t *cr, uint16_t xl, uint16_t xr, uint16_t y, int32_t upper, int32_t size){
	struct timespec endTime;
	timespec_get(&endTime, TIME_UTC);
	uint64_t endNanosec   	= LAConvertTimespecNano(&endTime);
	uint64_t startNanosec 	= LAConvertTimespecNano(&(lawp->rd.startTime));
	int64_t diffNanosec  	= endNanosec - startNanosec;
//	g_print("Diff: %li\tUpper: %i\t size: %i\n", diffNanosec, upper, size);

	int64_t leftTime		= diffNanosec + LAGetNSecBySamples(lawp, (int64_t) upper - (int64_t )size);
	int64_t rightTime		= diffNanosec + LAGetNSecBySamples(lawp, (int64_t) upper);
//	g_print("left: %li\tright: %li\n", leftTime, rightTime);

	LAScopePutNSecTime(cr, xl, y, leftTime);
	LAScopePutNSecTime(cr, xr - SCOPE_NSEC_TIME_RIGHT, y, rightTime);
}

void LAScopePutRelime(cairo_t *cr, uint16_t xl, uint16_t xr, uint16_t y, int32_t upper, int32_t size){
	int64_t leftTime 	= LAGetNSecBySamples(lawp, upper - size);
	int64_t rightTime 	= LAGetNSecBySamples(lawp, upper);
	LAScopePutNSecTime(cr, xl, y, leftTime);
	LAScopePutNSecTime(cr, xr - SCOPE_NSEC_TIME_RIGHT, y, rightTime);
}

void LAChannelDisplayTime(cairo_t *cr, uint8_t index){
	uint16_t lower;
	uint32_t upper;
	uint16_t size;
	LAGetWindowLowerSample(lawp, &lower, &upper, &size);

	LAChannel *channel = &(lawp->channel[index]);
	cairo_set_line_width(cr, 1);
	cairo_set_source_rgb(cr, channel->r, channel->g, channel->b);
	
	uint16_t x0 = SCOPE_LABEL_TIME_LEFT_X;
	uint16_t x1 = SCOPE_LABEL_TIME_RIGHT_X;
	uint16_t y0 = SCOPE_LABEL_TIME_Y;

	if(lawp->rd.showRelativeTime){
		LAScopePutRelime(cr, x0, x1, y0, lawp->rd.scopeOffset, size);
	}

	uint16_t y1 = y0 - SCOPE_LABEL_TIME_DY;

	if(lawp->rd.showAbsoluteTime){
		LAScopePutAbsTime(cr, x0, x1, y1, lawp->rd.scopeOffset, size);
	}

	uint16_t y2 = y1 - SCOPE_LABEL_TIME_DY;

	if(lawp->rd.showRelativeSample){
		LAScopePutIntValue(cr, x0, y2, (int64_t)lawp->rd.scopeOffset - size);
		LAScopePutIntValue(cr, x1, y2, (int64_t)lawp->rd.scopeOffset);
	}

	uint16_t y3 = y2 - SCOPE_LABEL_TIME_DY;

	if(lawp->rd.showAbsoluteSample){
		LAScopePutIntValue(cr, x0, y3, (int64_t)lawp->rd.sampleCounter + lawp->rd.scopeOffset - size);
		LAScopePutIntValue(cr, x1, y3, (int64_t)lawp->rd.sampleCounter + lawp->rd.scopeOffset);
	}
}

void LAChannelDisplayBufferEnd(cairo_t *cr, uint8_t index){
	if(!(lawp->rd.showBufferEnd)){
		return;
	}

	double x = SCOPE_WIDTH - ((lawp->rd.scopeOffset * SCOPE_WIDTH )/ (double) (LA_BUFFER_SIZE / LAGetZoomMultiplier(lawp)));
	double y0 = 0;
	double y1 = SCOPE_HEIGTH;

	cairo_set_line_width(cr, 4);
	cairo_set_source_rgb(cr, 1, 0, 0);
	cairo_move_to(cr, x, y0);
	cairo_line_to(cr, x, y1);
	cairo_stroke(cr);

	cairo_move_to(cr, x - SCOPE_BUFFER_END_TEXT_DX, y0 + SCOPE_LABEL_TIME_DY);
	cairo_show_text(cr, "Buffer end");
	cairo_stroke(cr);

	cairo_move_to(cr, x + SCOPE_BUFFER_START_TEXT_DX, y0 + SCOPE_LABEL_TIME_DY);
	cairo_show_text(cr, "Buffer start");
	cairo_stroke(cr);
}

void LAChannelDrawBufferGrid(cairo_t *cr, uint8_t index){
	if(!(lawp->rd.showBufferRuler)){
		return;
	}

	uint16_t lower;
	uint32_t upper;
	uint16_t size;
	LAGetWindowLowerSample(lawp, &lower, &upper, &size);

	int32_t lowerS = (int32_t) upper - (int32_t) (size * LAGetZoomMultiplier(lawp));
	int32_t upperN = upper - lawp->rd.dataOffset;
	int32_t lowerN = lower - lawp->rd.dataOffset;

	int32_t upperG = (upperN / lawp->rd.virtualBufferSize);
	upperG = (upperG * lawp->rd.virtualBufferSize) - lawp->rd.scopeOffset;
	int32_t lowerG = (lowerN / lawp->rd.virtualBufferSize);
	lowerG = (lowerG * lawp->rd.virtualBufferSize) - lawp->rd.scopeOffset;
	
	//g_print("l: %i\tu: %i\td: %i\n", lowerN, upperN, lawp->rd.dataOffset);
	//g_print("l: %i\tu: %i\tv: %i\n\n", lowerG, upperG, lawp->rd.virtualBufferSize);

	double x;
	double y0 = 0;
	double y1 = SCOPE_HEIGTH;

	cairo_set_source_rgb(cr, 0.2, 0.2, 0.2);
	cairo_set_line_width(cr, 2);

	for(int32_t i = lowerG; i <= upperG; i += lawp->rd.virtualBufferSize){
	//	g_print("\t%i\n", i - lawp->rd.scopeOffset);
		x = SCOPE_WIDTH * (1 + ((i /(double) LA_BUFFER_SIZE) * LAGetZoomMultiplier(lawp)));
	//	g_print("x: %lf\n", x);

		cairo_move_to(cr, x, y0);
		cairo_line_to(cr, x, y1);
		cairo_stroke(cr);
	}

}

void LAChannelOnDraw(GtkWidget *widget, cairo_t *cr, uintptr_t indexp){
	uint8_t index = CONVERT_GPOINTER_TO_INT(indexp);
	LAChannel *channel = &(lawp->channel[index]);

	cairo_set_source_rgb(cr, 0, 0, 0);
	cairo_paint(cr);

	LAChannelDrawScopeLines(cr);

	if(channel->muted) return;
	LAChannelDrawBufferGrid(cr, index);
	LAChannelDrawScopeData(cr, index, channel->bit);

	LAChannelDisplayTime(cr, index);
	LAChannelDisplayBufferEnd(cr, index);
}


