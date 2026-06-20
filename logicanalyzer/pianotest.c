#include <gtk/gtk.h>
#include <cairo.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "liblogicanalyzer.h"
#include <math.h>

#define WHITE_KEY_SIZE 40
#define WHITE_KEY_HEIGHT 108
#define KEY_COUNT 19
#define KEY_COUNT_ALL 33
#define PIANO_WIDTH  (WHITE_KEY_SIZE * KEY_COUNT)
#define PIANO_HEIGTH 112

#define BLACK_OFFSET 24
#define BLACK_WIDTH 20
#define BLACK_HEIGHT 64

#define WHITE_COLOR 1, 1, 1
#define BLACK_COLOR 0, 0, 0
#define OUTLINE_COLOR 0, 0, 0

#define KEYCODE_MUTE 0xFF
#define CLOCK_FREQ 1000000
#define MIDI_A4 69
#define A4_FREQ 440

typedef struct {
	GtkWidget *window;
	GtkWidget *content;
	GtkWidget *vbox;
	
	GtkWidget *hboxControl;
	GtkWidget *labelChannel;
	GtkWidget *channel;
	GtkWidget *labelOctave;
	GtkWidget *octave;
	GtkWidget *labelTranspose;
	GtkWidget *transpose;
	GtkWidget *labelFine;
	GtkWidget *fine;
	GtkWidget *buttonPanic;

	GtkWidget *hboxPiano;
	GtkWidget *piano;

	uint8_t lastKey;
} LATestPianoMenu;

typedef enum {
	LAKey_White		= 0,
	LAKey_Black		= 1,
	LAKey_White_L	= 2,
	LAKey_White_R	= 3,
	LAKey_White_I	= 4
} LAPianoKeyType;

typedef struct {
	uint8_t value;
	uint8_t offset;
	const char *label;
	LAPianoKeyType type;
	guint key1;
	guint key2;
} LAPianoKey;

LAPianoKey LAKeys[KEY_COUNT_ALL] = {
	{0, 	0,	"C", 	LAKey_White_L,	GDK_KEY_Z, 0x0},
	{1, 	0,	"C#", 	LAKey_Black,	GDK_KEY_S, 0x0},
	{2, 	1,	"D", 	LAKey_White_I,	GDK_KEY_X, 0x0},
	{3, 	1,	"D#", 	LAKey_Black,	GDK_KEY_D, 0x0},
	{4, 	2,	"E", 	LAKey_White_R,	GDK_KEY_C, 0x0},
	{5, 	3,	"F", 	LAKey_White_L,	GDK_KEY_V, 0x0},
	{6, 	3,	"F#", 	LAKey_Black,	GDK_KEY_G, 0x0},
	{7, 	4,	"G", 	LAKey_White_I,	GDK_KEY_B, 0x0},
	{8, 	4,	"G#", 	LAKey_Black,	GDK_KEY_H, 0x0},
	{9, 	5,	"A", 	LAKey_White_I,	GDK_KEY_N, 0x0},
	{10, 	5,	"Bb", 	LAKey_Black,	GDK_KEY_J, 0x0},
	{11, 	6,	"B", 	LAKey_White_R,	GDK_KEY_M, 0x0},
	{12, 	7,	"C", 	LAKey_White_L,	GDK_KEY_Q, GDK_KEY_comma},
	{13, 	7,	"C#", 	LAKey_Black,	GDK_KEY_2, GDK_KEY_L},
	{14, 	8,	"D", 	LAKey_White_I,	GDK_KEY_W, 0x0},
	{15, 	8,	"D#", 	LAKey_Black,	GDK_KEY_3, 0x0},
	{16, 	9,	"E", 	LAKey_White_R,	GDK_KEY_E, 0x0},
	{17, 	10,	"F", 	LAKey_White_L,	GDK_KEY_R, 0x0},
	{18, 	10,	"F#", 	LAKey_Black,	GDK_KEY_5, 0x0},
	{19, 	11,	"G", 	LAKey_White_I,	GDK_KEY_T, 0x0},
	{20, 	11,	"G#", 	LAKey_Black,	GDK_KEY_6, 0x0},
	{21, 	12,	"A", 	LAKey_White_I,	GDK_KEY_Y, 0x0},
	{22, 	12,	"Bb", 	LAKey_Black,	GDK_KEY_7, 0x0},
	{23, 	13,	"B", 	LAKey_White_R,	GDK_KEY_U, 0x0},
	{24, 	14,	"C", 	LAKey_White_L,	GDK_KEY_I, 0x0},
	{25, 	14,	"C#", 	LAKey_Black,	GDK_KEY_9, 0x0},
	{26, 	15,	"D", 	LAKey_White_I,	GDK_KEY_O, 0x0},
	{27, 	15,	"D#", 	LAKey_Black,	GDK_KEY_0, 0x0},
	{28, 	16,	"E", 	LAKey_White_R,	GDK_KEY_P, 0x0},
	{29, 	17,	"F", 	LAKey_White_L,	GDK_KEY_A, 0x0},
	{30, 	17,	"F#", 	LAKey_Black,	GDK_KEY_question, 0x0},
	{31, 	18,	"G", 	LAKey_White_I,	GDK_KEY_plus, 0x0},
	{32, 	18,	"G#", 	LAKey_Black,	0x0, 		0x0},
};

LAErrorCode LAPianoDrawKey(cairo_t *cr, LAPianoKey key, GdkRGBA *color){
	double x0 = key.offset * WHITE_KEY_SIZE;
	double  w = WHITE_KEY_SIZE;
	double y0 = (PIANO_HEIGTH - WHITE_KEY_HEIGHT);
	double  h = WHITE_KEY_HEIGHT;
	
	if(color == NULL)	cairo_set_source_rgb(cr, WHITE_COLOR);
	else				cairo_set_source_rgb(cr, color->red, color->green, color->blue);

	switch(key.type){
		case LAKey_White:
							cairo_rectangle(cr, x0, y0, w, h);
							cairo_fill(cr);
							cairo_stroke(cr);
							cairo_set_source_rgb(cr, OUTLINE_COLOR);
							cairo_rectangle(cr, x0, y0, w, h);
							cairo_stroke(cr);
							break;
		case LAKey_Black:	
							x0 += BLACK_OFFSET;
							w   = BLACK_WIDTH;
							h   = BLACK_HEIGHT;
							if(color == NULL) cairo_set_source_rgb(cr, BLACK_COLOR);
							cairo_rectangle(cr, x0, y0, w, h);
							cairo_fill(cr);
							cairo_stroke(cr);
							cairo_set_source_rgb(cr, OUTLINE_COLOR);
							cairo_rectangle(cr, x0, y0, w, h);
							cairo_stroke(cr);
							break;
		case LAKey_White_L:
							cairo_move_to(cr, x0, y0);
							cairo_rel_line_to(cr, 0, WHITE_KEY_HEIGHT);
							cairo_rel_line_to(cr, WHITE_KEY_SIZE, 0);
							cairo_rel_line_to(cr, 0, -(WHITE_KEY_HEIGHT - BLACK_HEIGHT));
							cairo_rel_line_to(cr, -(WHITE_KEY_SIZE - BLACK_OFFSET), 0);
							cairo_rel_line_to(cr, 0, -BLACK_HEIGHT);
							cairo_rel_line_to(cr, -BLACK_OFFSET, 0);
							cairo_fill(cr);
							cairo_stroke(cr);
							cairo_move_to(cr, x0, y0);
							cairo_rel_line_to(cr, 0, WHITE_KEY_HEIGHT);
							cairo_rel_line_to(cr, WHITE_KEY_SIZE, 0);
							cairo_rel_line_to(cr, 0, -(WHITE_KEY_HEIGHT - BLACK_HEIGHT));
							cairo_rel_line_to(cr, -(WHITE_KEY_SIZE - BLACK_OFFSET), 0);
							cairo_rel_line_to(cr, 0, -BLACK_HEIGHT);
							cairo_rel_line_to(cr, -BLACK_OFFSET, 0);
							cairo_set_source_rgb(cr, OUTLINE_COLOR);
							cairo_stroke(cr);
							break;
		case LAKey_White_I:	
							cairo_move_to(cr, x0 + (BLACK_OFFSET + BLACK_WIDTH - WHITE_KEY_SIZE), y0);
							cairo_rel_line_to(cr, 0, BLACK_HEIGHT);
							cairo_rel_line_to(cr, -(BLACK_OFFSET + BLACK_WIDTH - WHITE_KEY_SIZE), 0);
							cairo_rel_line_to(cr, 0, (WHITE_KEY_HEIGHT - BLACK_HEIGHT));
							cairo_rel_line_to(cr, WHITE_KEY_SIZE, 0);
							cairo_rel_line_to(cr, 0, -(WHITE_KEY_HEIGHT - BLACK_HEIGHT));
							cairo_rel_line_to(cr, -(WHITE_KEY_SIZE - BLACK_OFFSET), 0);
							cairo_rel_line_to(cr, 0, -BLACK_HEIGHT);
							cairo_rel_line_to(cr, -(WHITE_KEY_SIZE - BLACK_WIDTH), 0);
							cairo_fill(cr);
							cairo_stroke(cr);
							cairo_move_to(cr, x0 + (BLACK_OFFSET + BLACK_WIDTH - WHITE_KEY_SIZE), y0);
							cairo_rel_line_to(cr, 0, BLACK_HEIGHT);
							cairo_rel_line_to(cr, -(BLACK_OFFSET + BLACK_WIDTH - WHITE_KEY_SIZE), 0);
							cairo_rel_line_to(cr, 0, (WHITE_KEY_HEIGHT - BLACK_HEIGHT));
							cairo_rel_line_to(cr, WHITE_KEY_SIZE, 0);
							cairo_rel_line_to(cr, 0, -(WHITE_KEY_HEIGHT - BLACK_HEIGHT));
							cairo_rel_line_to(cr, -(WHITE_KEY_SIZE - BLACK_OFFSET), 0);
							cairo_rel_line_to(cr, 0, -BLACK_HEIGHT);
							cairo_rel_line_to(cr, -(WHITE_KEY_SIZE - BLACK_WIDTH), 0);
							cairo_set_source_rgb(cr, OUTLINE_COLOR);
							cairo_stroke(cr);
							break;
		case LAKey_White_R:	
							cairo_move_to(cr, x0 + (BLACK_OFFSET + BLACK_WIDTH - WHITE_KEY_SIZE), y0);
							cairo_rel_line_to(cr, 0, BLACK_HEIGHT);
							cairo_rel_line_to(cr, -(BLACK_OFFSET + BLACK_WIDTH - WHITE_KEY_SIZE), 0);
							cairo_rel_line_to(cr, 0, (WHITE_KEY_HEIGHT - BLACK_HEIGHT));
							cairo_rel_line_to(cr, WHITE_KEY_SIZE, 0);
							cairo_rel_line_to(cr, 0, -WHITE_KEY_HEIGHT);
							cairo_rel_line_to(cr, -(WHITE_KEY_SIZE - (BLACK_OFFSET + BLACK_WIDTH - WHITE_KEY_SIZE)), 0);
							cairo_fill(cr);
							cairo_stroke(cr);
							cairo_move_to(cr, x0 + (BLACK_OFFSET + BLACK_WIDTH - WHITE_KEY_SIZE), y0);
							cairo_rel_line_to(cr, 0, BLACK_HEIGHT);
							cairo_rel_line_to(cr, -(BLACK_OFFSET + BLACK_WIDTH - WHITE_KEY_SIZE), 0);
							cairo_rel_line_to(cr, 0, (WHITE_KEY_HEIGHT - BLACK_HEIGHT));
							cairo_rel_line_to(cr, WHITE_KEY_SIZE, 0);
							cairo_rel_line_to(cr, 0, -WHITE_KEY_HEIGHT);
							cairo_rel_line_to(cr, -(WHITE_KEY_SIZE - (BLACK_OFFSET + BLACK_WIDTH - WHITE_KEY_SIZE)), 0);
							cairo_set_source_rgb(cr, OUTLINE_COLOR);
							cairo_stroke(cr);
		default:
	}

	return LA_NO_ERROR;
}

void LAOnPianoDraw(GtkWidget *widget, cairo_t *cr, LATestPianoMenu *lap){
	cairo_set_source_rgb(cr, 1, 1, 1);
	cairo_paint(cr);

	for(uint8_t i = 0; i < KEY_COUNT_ALL; i++){
		LAPianoDrawKey(cr, LAKeys[i], NULL);
	}

	uint8_t octave = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(lap->octave));
	GdkRGBA highlightColor;
	highlightColor.red = 0.2;
	highlightColor.green = 0.2;
	highlightColor.blue = 1;
	highlightColor.alpha = 1;

	uint8_t lowerLimit = octave * 12;
	uint8_t upperLimit = octave * 12 + KEY_COUNT_ALL;

	if(lap->lastKey != KEYCODE_MUTE && lap->lastKey >= lowerLimit && lap->lastKey <= upperLimit){
		LAPianoDrawKey(cr, LAKeys[lap->lastKey - lowerLimit], &highlightColor);
	}
	(void) widget;
}

void LAPianoAddOctave(LATestPianoMenu *lap){
	double value = gtk_spin_button_get_value(GTK_SPIN_BUTTON(lap->octave));
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(lap->octave), value + 1.0);
}

void LAPianoSubOctave(LATestPianoMenu *lap){
	double value = gtk_spin_button_get_value(GTK_SPIN_BUTTON(lap->octave));
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(lap->octave), value - 1.0);
}

void LAHandlePianoKey(LATestPianoMenu *lap, uint8_t key){
	lap->lastKey = key;
	gtk_widget_queue_draw(lap->piano);
	int32_t transpose = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(lap->transpose));
	int32_t fine 	  = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(lap->fine));

	double keyf = key + transpose + (fine/(double) 100);
	double freq = A4_FREQ * powf(2, (keyf - MIDI_A4) / (double) 12);
	uint32_t timev = CLOCK_FREQ / (double) (16*freq);

	int channel = gtk_combo_box_get_active(GTK_COMBO_BOX(lap->channel));
	int command = 0;
	int command2 = 0;
	switch(channel){
		case 0:	command = LA_COMMAND_TEST_CLOCK1_RATE;
				command2 = LA_COMMAND_TEST_CLOCK1_MUTE;
				break;
		case 1:	command = LA_COMMAND_TEST_CLOCK2_RATE;
				command2 = LA_COMMAND_TEST_CLOCK2_MUTE;
				break;
		case 2:	command = LA_COMMAND_TEST_NOISE_RATE;
				command2 = LA_COMMAND_TEST_NOISE_MUTE;
				break;
		case 3:	command = LA_COMMAND_TEST_DAC_SPEED;
				command2 = LA_COMMAND_TEST_DAC_MUTE;
				break;
		default:
				command = LA_COMMAND_NOP;
				command2 = LA_COMMAND_NOP;
				break;
	}

	LASendBasicSerial(lawp, command, timev);
	LASendBasicSerial(lawp, command2, 0);
}

void LAOnPianoPanic(GtkWidget *widget, LATestPianoMenu *lap){
	LASendBasicSerial(lawp, LA_COMMAND_TEST_CLOCK1_MUTE, 1);
	LASendBasicSerial(lawp, LA_COMMAND_TEST_CLOCK2_MUTE, 1);
	LASendBasicSerial(lawp, LA_COMMAND_TEST_NOISE_MUTE, 1);
	LASendBasicSerial(lawp, LA_COMMAND_TEST_DAC_MUTE, 1);
	lap->lastKey = KEYCODE_MUTE;
	gtk_widget_queue_draw(lap->piano);
	(void) widget;
}

gboolean LAOnKeyPressPiano(GtkWidget *widget, GdkEventKey *event, LATestPianoMenu *lap){
	int response = TRUE;

	switch(event->keyval){
		case GDK_KEY_KP_Add:		LAPianoAddOctave(lap);
									break;
		case GDK_KEY_KP_Subtract:	LAPianoSubOctave(lap);
									break;
	}

	uint8_t octave = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(lap->octave));
	guint keyval = gdk_keyval_to_upper(event->keyval);
	for(uint8_t i = 0; i < KEY_COUNT_ALL; i++){
		if(keyval == LAKeys[i].key1 || keyval == LAKeys[i].key2){
			LAHandlePianoKey(lap, octave * 12 + LAKeys[i].value);
			break;
		}
	}

	return response;
	(void) widget;
}

void LAOpenTestPiano(GtkWidget *widget, LAWindow *law){
	LATestPianoMenu lapv;
	LATestPianoMenu *lap = &lapv;

	lap->lastKey = KEYCODE_MUTE;

	lap->window = gtk_dialog_new_with_buttons(
					"Test Piano",
					GTK_WINDOW(law->window),
					GTK_DIALOG_DESTROY_WITH_PARENT,
					"_Quit", GTK_RESPONSE_CANCEL,
					NULL
				);

	gtk_window_set_modal(GTK_WINDOW(lap->window), FALSE);
	gtk_container_set_border_width(GTK_CONTAINER(lap->window), 8);
	lap->content = gtk_dialog_get_content_area(GTK_DIALOG(lap->window));

	lap->vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
	gtk_container_add(GTK_CONTAINER(lap->content), GTK_WIDGET(lap->vbox));

	lap->hboxControl = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
	gtk_container_add(GTK_CONTAINER(lap->vbox), GTK_WIDGET(lap->hboxControl));

	lap->hboxPiano = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
	gtk_container_add(GTK_CONTAINER(lap->vbox), GTK_WIDGET(lap->hboxPiano));

	lap->labelChannel 	= gtk_label_new("Channel:");
	lap->channel 		= gtk_combo_box_text_new();
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(lap->channel), "Clock1");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(lap->channel), "Clock2");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(lap->channel), "Noise");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(lap->channel), "Wave");
	gtk_combo_box_set_active(GTK_COMBO_BOX(lap->channel), 0);
	GtkAdjustment *adjOctave  = gtk_adjustment_new(4, 0,  8, 1, 2, 0);
	lap->octave			= gtk_spin_button_new(adjOctave, 1, 0);
	lap->labelOctave 	= gtk_label_new("Octave:");
	GtkAdjustment *adjTranspose = gtk_adjustment_new(0, -36,  36, 1, 12, 0);
	lap->labelTranspose 	= gtk_label_new("Transpose:");
	lap->transpose			= gtk_spin_button_new(adjTranspose, 1, 0);
	GtkAdjustment *adjFine  = gtk_adjustment_new(0, -100, 100, 1, 10, 0);
	lap->labelFine 			= gtk_label_new("Fine:");
	lap->fine				= gtk_spin_button_new(adjFine, 1, 0);
	lap->buttonPanic 	= gtk_button_new_with_label("Panic");

	lap->piano 			= gtk_drawing_area_new();

	gtk_container_add(GTK_CONTAINER(lap->hboxControl), GTK_WIDGET(lap->labelChannel));
	gtk_container_add(GTK_CONTAINER(lap->hboxControl), GTK_WIDGET(lap->channel));
	gtk_container_add(GTK_CONTAINER(lap->hboxControl), GTK_WIDGET(lap->labelOctave));
	gtk_container_add(GTK_CONTAINER(lap->hboxControl), GTK_WIDGET(lap->octave));
	gtk_container_add(GTK_CONTAINER(lap->hboxControl), GTK_WIDGET(lap->labelTranspose));
	gtk_container_add(GTK_CONTAINER(lap->hboxControl), GTK_WIDGET(lap->transpose));
	gtk_container_add(GTK_CONTAINER(lap->hboxControl), GTK_WIDGET(lap->labelFine));
	gtk_container_add(GTK_CONTAINER(lap->hboxControl), GTK_WIDGET(lap->fine));
	gtk_container_add(GTK_CONTAINER(lap->hboxControl), GTK_WIDGET(lap->buttonPanic));

	gtk_container_add(GTK_CONTAINER(lap->hboxPiano), GTK_WIDGET(lap->piano));
	gtk_widget_set_size_request(lap->piano, PIANO_WIDTH, PIANO_HEIGTH);

	g_signal_connect(lap->piano,  "draw",    G_CALLBACK(LAOnPianoDraw), lap);
	g_signal_connect(lap->buttonPanic, "clicked", G_CALLBACK(LAOnPianoPanic), lap);
	g_signal_connect(lap->window, "key-press-event", G_CALLBACK(LAOnKeyPressPiano), lap);

	gtk_widget_set_can_focus(lap->window, TRUE);
	gtk_widget_grab_focus(lap->window);
	gtk_widget_show_all(lap->window);
	gtk_dialog_run(GTK_DIALOG(lap->window));
	gtk_widget_destroy(lap->window);
	(void) widget;
}
