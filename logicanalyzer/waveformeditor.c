#include <gtk/gtk.h>
#include <cairo.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include "liblogicanalyzer.h"
#include "enums.h"
#include "error.h"
#include "utils.h"
#include "types.h"
#include "gtk_funcs.h"
#include "filter/windows.h"
#include "filter/filter.h"
#include "filterwindow.h"
#include "serial.h"

#define LA_WAVE_WIDTH 4
#define LA_WAVE_SCALEX 1
#define PADDING_X 64
#define WAVE_WIDTH ((256 * LA_WAVE_WIDTH * LA_WAVE_SCALEX) + PADDING_X)

#define LA_WAVE_HEIGHT 4
#define LA_WAVE_SCALEY 0.25
#define PADDING_Y 64
#define WAVE_HEIGTH ((256 * LA_WAVE_HEIGHT * LA_WAVE_SCALEY) + PADDING_Y)


LAErrorCode LACreateWaveform(LAWaveform **waveform){
	LA_HANDLE_NULLPTR(waveform, LA_PROPAGATE_ERROR);

	(*waveform) = (LAWaveform *)malloc(sizeof(LAWaveform));
	if((*waveform) == NULL){
		LA_RAISE_ERROR(LA_ERROR_MALLOC);
		return LA_ERROR_MALLOC;
	}

	(*waveform)->size = 256;
	memset((*waveform)->data, LA_DEFAULT_WAVE_FLAT_VALUE, LA_WAVEFORM_SIZE);
	(*waveform)->prev = NULL;
	(*waveform)->next = NULL;

	return LA_NO_ERROR;
}

LAErrorCode LADestroyWaveform(LAWaveform **waveform){
	LA_HANDLE_NULLPTR(waveform, LA_PROPAGATE_ERROR);

	LAWaveform *endWave = NULL;

	if((*waveform) != NULL){
		if((*waveform)->prev != NULL){
			(*waveform)->prev->next = (*waveform)->next;
			endWave = (*waveform)->prev;
		}

		if((*waveform)->next != NULL){
			(*waveform)->next->prev = (*waveform)->prev;
			endWave = (*waveform)->next;
		}
			
		free((*waveform));
		(*waveform) = NULL;
	}

	if(endWave != NULL){
		(*waveform) = endWave;
	}

	return LA_NO_ERROR;
}

LAErrorCode LADestroyWaveformAll(LAWaveform **waveform){
	LA_HANDLE_NULLPTR(waveform, LA_PROPAGATE_ERROR);

	LAWaveform *wave = (*waveform);
	LAWaveform *next = NULL;

	while(wave != NULL){
		next = wave->next;
		free(wave);
		wave = next;
	}

	(*waveform) = NULL;
	return LA_NO_ERROR;
}


void LADrawCurrentWave(cairo_t *cr, LAWaveform *wave){
	if(cr == NULL || wave == NULL) return;

	double x0 = 0;
	double y0 = 0;
	double h  = LA_WAVE_HEIGHT;
	double w  = (WAVE_WIDTH - PADDING_X) / (double) wave->size;

	for(uint16_t i = 0; i < wave->size; i++){
		x0 = (PADDING_X / 2) + i * ((WAVE_WIDTH - PADDING_X) / (double) (wave->size));
		y0 = (PADDING_Y / 2) + (0xFF - wave->data[i]) * (WAVE_HEIGTH - PADDING_Y) / (double) 256;

		cairo_set_source_rgb(cr, 0, 0, 0);
		cairo_rectangle(cr, x0, y0, w, h);
		cairo_stroke(cr);

		cairo_set_source_rgb(cr, 0.5, 0.5, 1);
		cairo_rectangle(cr, x0, y0, w, h);
		cairo_fill(cr);
		cairo_stroke(cr);
	}
}

void LAOnWaveformEditorDraw(GtkWidget *widget, cairo_t *cr, LAWaveformEditor *lae){
	cairo_set_source_rgb(cr, 1, 1, 1);
	cairo_paint(cr);

	LADrawCurrentWave(cr, lae->currentwave);

	(void) widget;
}

void LAOnWaveSizeChange(GtkWidget *widget, LAWaveformEditor *lae){
	if(lae->currentwave == NULL) return;

	uint32_t size = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(lae->waveSize));
	lae->currentwave->size = size;
	gtk_widget_queue_draw(lae->wave);
	(void) widget;
}

void LAWaveFuncRectangle(uint8_t *buffer, uint16_t size, double duty){
	if(buffer == NULL) return;

	for(uint16_t i = 0; i < size; i++){
		if(i < (duty * size)){
			buffer[i] = 0xFF;
		} else {
			buffer[i] = 0x0;
		}
	}
}

void LAWaveFuncSquare(uint8_t *buffer, uint16_t size){
	if(buffer == NULL) return;
	LAWaveFuncRectangle(buffer, size, 0.5);
}

void LAWaveFuncSine(uint8_t *buffer, uint16_t size){
	if(buffer == NULL) return;

	for(uint16_t i = 0; i < size; i++){
		buffer[i] = 0x80 + 0x7F *  sin(2 * M_PI * (i / (double) size));
	}
}

void LAWaveFuncTriangle(uint8_t *buffer, uint16_t size){
	if(buffer == NULL) return;

	double a = 0;
	for(uint16_t i = 0; i < size; i++){
		if(i < (size / (double) 4)){
			a = i / (size / (double) 4);
		} else if (i >= (size / (double) 4) && i < (3 * size / (double) 4)){
			a = 1 - ((i - (size / (double) 4))) / (size / (double) 4);
		} else {
			a = -1 + (i - (3 * size / (double) 4)) / (size / (double) 4);
		}

		buffer[i] = 0x80 + a * 0x7F;
	}
}

void LAWaveFuncSaw(uint8_t *buffer, uint16_t size){
	if(buffer == NULL) return;

	double a = 0;
	for(uint16_t i = 0; i < size; i++){
		a = -1 + 2*(i / (double) size);
		buffer[i] = 0x80 + a * 0x7F;
	}
}

void LAWaveFuncExp(uint8_t *buffer, uint16_t size){
	if(buffer == NULL) return;

	double a = 0;
	for(uint16_t i = 0; i < size; i++){
		a = (i / (double) size);
		a = a * a;
		a = -1 + 2*a;
		buffer[i] = 0x80 + a * 0x7F;
	}
}

void LAWaveFuncNoise(uint8_t *buffer, uint16_t size){
	if(buffer == NULL) return;

	for(uint16_t i = 0; i < size; i++){
		buffer[i] = rand() & 0xFF;
	}
}

void LAWaveFuncPulse25_0(uint8_t *buffer, uint16_t size){
	if(buffer == NULL) return;
	LAWaveFuncRectangle(buffer, size, 0.25);
}

void LAWaveFuncPulse12_5(uint8_t *buffer, uint16_t size){
	if(buffer == NULL) return;
	LAWaveFuncRectangle(buffer, size, 0.125);
}

void LAOnPresetClicked(GtkWidget *widget, LAWaveformEditor *lae){
	if(lae->currentwave == NULL) return;
	
	uint32_t preset = gtk_combo_box_get_active(GTK_COMBO_BOX(lae->presets));
	//uint16_t size   = lae->currentwave->size;

	switch(preset){
		case LA_WAVE_PRESET_CUSTOM		:	break;
		case LA_WAVE_PRESET_SQUARE		:	
											LAWaveFuncSquare(lae->currentwave->data, lae->currentwave->size);
											break;
		case LA_WAVE_PRESET_SINE		:	
											LAWaveFuncSine(lae->currentwave->data, lae->currentwave->size);
											break;
		case LA_WAVE_PRESET_TRIANGLE	:	
											LAWaveFuncTriangle(lae->currentwave->data, lae->currentwave->size);
											break;
		case LA_WAVE_PRESET_SAW			:	
											LAWaveFuncSaw(lae->currentwave->data, lae->currentwave->size);
											break;
		case LA_WAVE_PRESET_EXP			:	
											LAWaveFuncExp(lae->currentwave->data, lae->currentwave->size);
											break;
		case LA_WAVE_PRESET_NOISE		:	
											LAWaveFuncNoise(lae->currentwave->data, lae->currentwave->size);
											break;
		case LA_WAVE_PRESET_PULSE25_0	:	
											LAWaveFuncPulse25_0(lae->currentwave->data, lae->currentwave->size);
											break;
		case LA_WAVE_PRESET_PULSE12_5	:	
											LAWaveFuncPulse12_5(lae->currentwave->data, lae->currentwave->size);
											break;
		default:
											break;
	}

	gtk_widget_queue_draw(lae->wave);
	(void) widget;
}

void LAOnWaveFlat(GtkWidget *widget, LAWaveformEditor *lae){
	if(lae->currentwave == NULL) return;
	
	uint16_t size   = lae->currentwave->size;
	for(uint16_t i = 0; i < size; i++){
		lae->currentwave->data[i] = LA_DEFAULT_WAVE_FLAT_VALUE;
	}

	gtk_widget_queue_draw(lae->wave);
	(void) widget;
}

void LAWaveUpdateWaveSelectUpper(LAWaveformEditor *lae){
	uint16_t waveCount = 0;
	
	if(lae->wavetable == NULL) return;
	LAWaveform *wave = lae->wavetable;

	while(wave != NULL){
		waveCount++;
		wave = wave->next;
	}

	gtk_adjustment_set_upper(GTK_ADJUSTMENT(lae->waveSelectorAdj), waveCount);
}

void LAOnWaveSelectorChange(GtkWidget *widget, LAWaveformEditor *lae){
	if(lae->wavetable == NULL) return;

	uint16_t waveCount = 0;
	uint16_t targetIndex = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(lae->waveSelector));

	LAWaveform *wave = lae->wavetable;
	while(wave != NULL){
		waveCount++;

		if(waveCount == targetIndex){
			lae->currentwave = wave;
			break;
		}

		wave = wave->next;
	}

	gtk_widget_queue_draw(lae->wave);
	(void) widget;
}

void LAOnWaveNew(GtkWidget *widget, LAWaveformEditor *lae){
	LAWaveform *newWave;
	LAErrorCode code = LACreateWaveform(&(newWave));
	if(code) return;

	if(lae->currentwave != NULL){
		newWave->prev = lae->currentwave;
		newWave->next = lae->currentwave->next;
		lae->currentwave->next = newWave;
		if(newWave->next != NULL){
			newWave->next->prev = newWave;
		}
	} else if (lae->wavetable == NULL){
		lae->wavetable = newWave;
		lae->currentwave = newWave;
	} else {
		g_print("Invalid wave\n");
		LADestroyWaveform(&newWave);
	}

	LAWaveUpdateWaveSelectUpper(lae);
	gtk_widget_queue_draw(lae->wave);
	(void) widget;
}

void LAOnWaveDelete(GtkWidget *widget, LAWaveformEditor *lae){
	LADestroyWaveform(&(lae->currentwave));
	if(lae->currentwave == NULL){
		lae->wavetable = NULL;
	}

	LAWaveUpdateWaveSelectUpper(lae);
	gtk_widget_queue_draw(lae->wave);
	(void) widget;
}

void LAOnWaveDeleteAll(GtkWidget *widget, LAWaveformEditor *lae){
	LADestroyWaveformAll(&(lae->wavetable));
	lae->wavetable = NULL;
	lae->currentwave = NULL;
	gtk_widget_queue_draw(lae->wave);
	(void) widget;
}

void LAWaveForceSelectorChange(LAWaveformEditor *lae, int32_t delta){
	int32_t value = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(lae->waveSelector));
	value += delta;
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(lae->waveSelector), value);
}

void LAOnWaveMoveLeft(GtkWidget *widget, LAWaveformEditor *lae){
	if(lae->currentwave == NULL) return;
	if(lae->wavetable   == NULL) return;
	if(lae->currentwave->prev == NULL) return;

	uint8_t swapWavetableStart = 0;
	if(lae->currentwave->prev->prev == NULL) swapWavetableStart = 1;

	LAWaveform *a = lae->currentwave->prev;
	LAWaveform *b = lae->currentwave;
	LAWaveform *c = lae->currentwave->next;
	LAWaveform *z = (a != NULL) ? a->prev : NULL;
/*
	g_print("Before:");
	g_print("\tz: %p \tprev: %p\tnext: %p\n", z, (z != NULL) ? z->prev : NULL, (z != NULL) ? z->next : NULL);
	g_print("\ta: %p \tprev: %p\tnext: %p\n", a, (a != NULL) ? a->prev : NULL, (a != NULL) ? a->next : NULL);
	g_print("\tb: %p \tprev: %p\tnext: %p\n", b, (b != NULL) ? b->prev : NULL, (b != NULL) ? b->next : NULL);
	g_print("\tv: %p \tprev: %p\tnext: %p\n", c, (c != NULL) ? c->prev : NULL, (c != NULL) ? c->next : NULL);
*/
	if(z != NULL){
		z->next = b;
	}

	if(b != NULL){
		b->prev = z;
		b->next = a;
	}

	if(a != NULL){
		a->prev = b;
		a->next = c;
	}

	if(c != NULL){
		c->prev = a;
	}
/*
	g_print("After:");
	g_print("\tz: %p \tprev: %p\tnext: %p\n", z, (z != NULL) ? z->prev : NULL, (z != NULL) ? z->next : NULL);
	g_print("\ta: %p \tprev: %p\tnext: %p\n", a, (a != NULL) ? a->prev : NULL, (a != NULL) ? a->next : NULL);
	g_print("\tb: %p \tprev: %p\tnext: %p\n", b, (b != NULL) ? b->prev : NULL, (b != NULL) ? b->next : NULL);
	g_print("\tv: %p \tprev: %p\tnext: %p\n", c, (c != NULL) ? c->prev : NULL, (c != NULL) ? c->next : NULL);
*/
	if(swapWavetableStart){
		lae->wavetable = b;
	}

	LAWaveForceSelectorChange(lae, -1);
	gtk_widget_queue_draw(lae->wave);
	(void) widget;
}

void LAOnWaveMoveRight(GtkWidget *widget, LAWaveformEditor *lae){
	if(lae->currentwave == NULL) return;
	if(lae->wavetable   == NULL) return;
	if(lae->currentwave->next == NULL) return;

	uint8_t swapWavetableStart = 0;
	if(lae->currentwave->prev == NULL) swapWavetableStart = 1;

	LAWaveform *z = lae->currentwave->prev;
	LAWaveform *a = lae->currentwave;
	LAWaveform *b = lae->currentwave->next;
	LAWaveform *c = (b != NULL) ? b->next : NULL;
/*
	g_print("Before:");
	g_print("\tz: %p \tprev: %p\tnext: %p\n", z, (z != NULL) ? z->prev : NULL, (z != NULL) ? z->next : NULL);
	g_print("\ta: %p \tprev: %p\tnext: %p\n", a, (a != NULL) ? a->prev : NULL, (a != NULL) ? a->next : NULL);
	g_print("\tb: %p \tprev: %p\tnext: %p\n", b, (b != NULL) ? b->prev : NULL, (b != NULL) ? b->next : NULL);
	g_print("\tv: %p \tprev: %p\tnext: %p\n", c, (c != NULL) ? c->prev : NULL, (c != NULL) ? c->next : NULL);
*/
	if(z != NULL){
		z->next = b;
	}

	if(b != NULL){
		b->prev = z;
		b->next = a;
	}

	if(a != NULL){
		a->prev = b;
		a->next = c;
	}

	if(c != NULL){
		c->prev = a;
	}
/*
	g_print("After:");
	g_print("\tz: %p \tprev: %p\tnext: %p\n", z, (z != NULL) ? z->prev : NULL, (z != NULL) ? z->next : NULL);
	g_print("\ta: %p \tprev: %p\tnext: %p\n", a, (a != NULL) ? a->prev : NULL, (a != NULL) ? a->next : NULL);
	g_print("\tb: %p \tprev: %p\tnext: %p\n", b, (b != NULL) ? b->prev : NULL, (b != NULL) ? b->next : NULL);
	g_print("\tv: %p \tprev: %p\tnext: %p\n", c, (c != NULL) ? c->prev : NULL, (c != NULL) ? c->next : NULL);
*/
	if(swapWavetableStart){
		lae->wavetable = b;
	}

	LAWaveForceSelectorChange(lae, 1);
	gtk_widget_queue_draw(lae->wave);
	(void) widget;
}

void LAWaveChangeWaveformFromCoordinates(LAWaveformEditor *lae, double x, double y){
	x -= (PADDING_X / 2);
	y -= (PADDING_Y / 2);

	if(x > (WAVE_WIDTH - (PADDING_X / 2))) 	x = (WAVE_WIDTH - (PADDING_X / 2));
	else if (x < 0)	   					    x = 0;

	if(y > (WAVE_HEIGTH - (PADDING_Y / 2))) y = (WAVE_HEIGTH - (PADDING_Y / 2));
	else if (y < 0)	   					    y = 0;

	uint16_t index = (x / (double) (WAVE_WIDTH - PADDING_X)) * lae->currentwave->size;
	int16_t  value = 0xFF - (y / (double) (WAVE_HEIGTH - PADDING_Y)) * 255;

	if(index >= lae->currentwave->size) index = lae->currentwave->size - 1;
	if(value > 0xFF) value = 0xFF;
	if(value < 0)    value = 0;

	lae->currentwave->data[index] = value;
	gtk_widget_queue_draw(lae->wave);
}

gboolean LAOnWaveformEditorClick(GtkWidget *widget, GdkEventButton *event, LAWaveformEditor *lae){
	if(!(event->type == GDK_BUTTON_PRESS && event->button == 1)) return FALSE;
	if(lae->currentwave == NULL) return TRUE;

	double x = event->x;
	double y = event->y;
	lae->mouseClick = 1;

	LAWaveChangeWaveformFromCoordinates(lae, x, y);

	(void) widget;
	return TRUE;
}

gboolean LAOnWaveformEditorRelease(GtkWidget *widget, GdkEventButton *event, LAWaveformEditor *lae){
	if(lae->currentwave == NULL) return TRUE;

	lae->mouseClick = 0;
	(void) widget;
	(void) event;
	return TRUE;
}

gboolean LAOnWaveformEditorMotion(GtkWidget *widget, GdkEventMotion *event, LAWaveformEditor *lae){
	if(lae->mouseClick == 0) return FALSE;
	if(lae->currentwave == NULL) return TRUE;

	double x = event->x;
	double y = event->y;
	LAWaveChangeWaveformFromCoordinates(lae, x, y);

	(void) widget;
	return TRUE;
}

void LAOnWaveSendThis(GtkWidget *widget, LAWaveformEditor *lae){
	if(lae == NULL) return;
	if(lae->currentwave == NULL) return;

	LASerialV2Protocol p;
	LAPrepareProtocolV2Wave(&p, 0, lae->currentwave->data, lae->currentwave->size);
	LASendSerialV2(lawp, &p);

	LAPrepareProtocolV2Basic(&p, LA_COMMAND_TEST_DAC_WRAP, lae->currentwave->size);
	LASendSerialV2(lawp, &p);
	(void) widget;
}

void LAOpenWaveformEditor(GtkWidget *widget, LAWindow *law){
	LAWaveformEditor laev;
	LAWaveformEditor *lae = &laev;

	LACreateWaveform(&(lae->wavetable));
	if(lae->wavetable != NULL){
		lae->currentwave = lae->wavetable;
	}
	lae->mouseClick = 0;
	
	lae->window = gtk_dialog_new_with_buttons(
					"Waveform editor",
					GTK_WINDOW(law->window),
					GTK_DIALOG_DESTROY_WITH_PARENT,
					"_Quit", GTK_RESPONSE_CANCEL,
					NULL
				);

	gtk_window_set_modal(GTK_WINDOW(lae->window), FALSE);
	gtk_container_set_border_width(GTK_CONTAINER(lae->window), 8);
	lae->content = gtk_dialog_get_content_area(GTK_DIALOG(lae->window));

	lae->vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
	gtk_container_add(GTK_CONTAINER(lae->content), GTK_WIDGET(lae->vbox));

	lae->hboxControl1 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
	gtk_container_add(GTK_CONTAINER(lae->vbox), GTK_WIDGET(lae->hboxControl1));

	lae->hboxControl2 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
	gtk_container_add(GTK_CONTAINER(lae->vbox), GTK_WIDGET(lae->hboxControl2));

	lae->hboxWave = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
	gtk_container_add(GTK_CONTAINER(lae->vbox), GTK_WIDGET(lae->hboxWave));

	lae->hboxHexview = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
	gtk_container_add(GTK_CONTAINER(lae->vbox), GTK_WIDGET(lae->hboxHexview));

	lae->hboxButtons = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
	gtk_container_add(GTK_CONTAINER(lae->vbox), GTK_WIDGET(lae->hboxButtons));

	lae->waveSelectorLabel 	= gtk_label_new("Wave:");
	lae->waveSelectorAdj  	= gtk_adjustment_new(1, 1, 1, 1, 1, 0);
	lae->waveSelector 		= gtk_spin_button_new(lae->waveSelectorAdj, 1, 0);
	lae->buttonNew 			= gtk_button_new_with_label("New");
	lae->buttonDelete 		= gtk_button_new_with_label("Delete");
	lae->buttonMoveLeft 	= gtk_button_new_with_label("<");
	lae->buttonMoveRight 	= gtk_button_new_with_label(">");
	lae->buttonDeleteAll	= gtk_button_new_with_label("Delete all");

	lae->waveSizeAdj		= gtk_adjustment_new(256, 1, 256, 1, 16, 0);
	lae->waveSizeLabel 		= gtk_label_new("Size:");
	lae->waveSize 			= gtk_spin_button_new(lae->waveSizeAdj, 1, 0);
	lae->flat 				= gtk_button_new_with_label("Flat wave");
	lae->presetsLabel 		= gtk_label_new("Preset:");
	lae->presets 			= gtk_combo_box_text_new();
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(lae->presets), "Custom");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(lae->presets), "Square");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(lae->presets), "Sine");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(lae->presets), "Triangle");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(lae->presets), "Saw");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(lae->presets), "Exp");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(lae->presets), "Noise");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(lae->presets), "Pulse 25%");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(lae->presets), "Pulse 12.5%");
	gtk_combo_box_set_active(GTK_COMBO_BOX(lae->presets), 0);
	lae->filterButton 	= gtk_button_new_with_label("Open Filter Editor");

	lae->wave 			= gtk_drawing_area_new();
	gtk_widget_set_size_request(lae->wave, WAVE_WIDTH, WAVE_HEIGTH);

	lae->sendThis 		= gtk_button_new_with_label("Send this");
	lae->sendAll 		= gtk_button_new_with_label("Send all");
	lae->open 			= gtk_button_new_with_label("Open");
	lae->exportThis 	= gtk_button_new_with_label("Export this");
	lae->exportAll 		= gtk_button_new_with_label("Export all");

	gtk_container_add(GTK_CONTAINER(lae->hboxControl1), GTK_WIDGET(lae->waveSelectorLabel));
	gtk_container_add(GTK_CONTAINER(lae->hboxControl1), GTK_WIDGET(lae->waveSelector));
	gtk_container_add(GTK_CONTAINER(lae->hboxControl1), GTK_WIDGET(lae->buttonNew));
	gtk_container_add(GTK_CONTAINER(lae->hboxControl1), GTK_WIDGET(lae->buttonDelete));
	gtk_container_add(GTK_CONTAINER(lae->hboxControl1), GTK_WIDGET(lae->buttonMoveLeft));
	gtk_container_add(GTK_CONTAINER(lae->hboxControl1), GTK_WIDGET(lae->buttonMoveRight));
	gtk_container_add(GTK_CONTAINER(lae->hboxControl1), GTK_WIDGET(lae->buttonDeleteAll));

	gtk_container_add(GTK_CONTAINER(lae->hboxControl2), GTK_WIDGET(lae->waveSizeLabel));
	gtk_container_add(GTK_CONTAINER(lae->hboxControl2), GTK_WIDGET(lae->waveSize));
	gtk_container_add(GTK_CONTAINER(lae->hboxControl2), GTK_WIDGET(lae->flat));
	gtk_container_add(GTK_CONTAINER(lae->hboxControl2), GTK_WIDGET(lae->presetsLabel));
	gtk_container_add(GTK_CONTAINER(lae->hboxControl2), GTK_WIDGET(lae->presets));
	gtk_container_add(GTK_CONTAINER(lae->hboxControl2), GTK_WIDGET(lae->filterButton));

	gtk_container_add(GTK_CONTAINER(lae->hboxWave), GTK_WIDGET(lae->wave));

	gtk_container_add(GTK_CONTAINER(lae->hboxButtons), GTK_WIDGET(lae->sendThis));
	gtk_container_add(GTK_CONTAINER(lae->hboxButtons), GTK_WIDGET(lae->sendAll));
	gtk_container_add(GTK_CONTAINER(lae->hboxButtons), GTK_WIDGET(lae->open));
	gtk_container_add(GTK_CONTAINER(lae->hboxButtons), GTK_WIDGET(lae->exportThis));
	gtk_container_add(GTK_CONTAINER(lae->hboxButtons), GTK_WIDGET(lae->exportAll));

	gtk_widget_set_events(lae->wave, GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK | GDK_POINTER_MOTION_MASK);

	g_signal_connect(lae->waveSelector,   	"value-changed",   		G_CALLBACK(LAOnWaveSelectorChange), 	lae);
	g_signal_connect(lae->waveSize,   		"value-changed",   		G_CALLBACK(LAOnWaveSizeChange), 		lae);
	g_signal_connect(lae->flat, 			"clicked", 				G_CALLBACK(LAOnWaveFlat), 				lae);
	g_signal_connect(lae->buttonMoveLeft, 	"clicked", 				G_CALLBACK(LAOnWaveMoveLeft), 			lae);
	g_signal_connect(lae->buttonMoveRight, 	"clicked", 				G_CALLBACK(LAOnWaveMoveRight), 			lae);
	g_signal_connect(lae->buttonDelete, 	"clicked", 				G_CALLBACK(LAOnWaveDelete), 			lae);
	g_signal_connect(lae->buttonDeleteAll, 	"clicked", 				G_CALLBACK(LAOnWaveDeleteAll), 			lae);
	g_signal_connect(lae->buttonNew, 		"clicked", 				G_CALLBACK(LAOnWaveNew), 				lae);
	g_signal_connect(lae->presets,   		"changed",   			G_CALLBACK(LAOnPresetClicked), 			lae);
	g_signal_connect(lae->filterButton, 	"clicked", 				G_CALLBACK(LAOnFilterButtonWave),		lae);
	g_signal_connect(lae->wave,   			"draw",    				G_CALLBACK(LAOnWaveformEditorDraw),		lae);
	g_signal_connect(lae->wave,   			"button-press-event",   G_CALLBACK(LAOnWaveformEditorClick), 	lae);
	g_signal_connect(lae->wave,   			"button-release-event", G_CALLBACK(LAOnWaveformEditorRelease), 	lae);
	g_signal_connect(lae->wave,   			"motion-notify-event",  G_CALLBACK(LAOnWaveformEditorMotion), 	lae);
	g_signal_connect(lae->sendThis, 		"clicked", 				G_CALLBACK(LAOnWaveSendThis), 			lae);

	gtk_widget_set_can_focus(lae->window, TRUE);
	gtk_widget_grab_focus(lae->window);
	gtk_widget_show_all(lae->window);
	gtk_dialog_run(GTK_DIALOG(lae->window));
	gtk_widget_destroy(lae->window);

	LADestroyWaveformAll(&(lae->wavetable));
	(void) widget;
}
