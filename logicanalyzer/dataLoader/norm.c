#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <gtk/gtk.h>
#include "../liblogicanalyzer.h"
#include "dataLoader.h"

LADataLoaderNormType LANormDetails[LA_NORM_TYPE_COUNT] = {
	{"No normalization", {
		{NULL,				0,		0,		0,		0,		0,		0},
		{NULL,				0,		0,		0,		0,		0,		0},
		{NULL,				0,		0,		0,		0,		0,		0}},
		NULL
	},
	{"Manual", {
		{"Pre gain offset",	0,		-32768,	32767,	0.001,	1,		3},
		{"Gain",			1,		-32768,	32767,	0.001,	1,		3},
		{"Post gain offset",0,		-32768,	32767,	0.001,	1,		3}},
		NULL
	},
	{"Mean Value", {
		{"Output gain",		1,		-32768,	32767,	0.001,	1,		3},
		{"Output offset",	0,		-32768,	32767,	0.001,	1,		3},
		{NULL,				0,		0,		0,		0,		0,		0}},
		NULL
	},
	{"Root Mean Square Value (RMS)", {
		{"Output gain",		1,		-32768,	32767,	0.001,	1,		3},
		{"Output offset",	0,		-32768,	32767,	0.001,	1,		3},
		{NULL,				0,		0,		0,		0,		0,		0}},
		NULL
	},
	{"Peak (min/max)", {
		{"Output gain",		1,		-32768,	32767,	0.001,	1,		3},
		{"Output offset",	0,		-32768,	32767,	0.001,	1,		3},
		{NULL,				0,		0,		0,		0,		0,		0}},
		NULL
	},
};

LAErrorCode LADataLoaderComboBoxNormFiller(GtkWidget *widget){
	LA_HANDLE_NULLPTR(widget, LA_PROPAGATE_ERROR);

	for(size_t i = 0; i < LA_NORM_TYPE_COUNT; i++){
		gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widget), LANormDetails[i].name);
	}

	gtk_combo_box_set_active(GTK_COMBO_BOX(widget), 0);

	return LA_NO_ERROR;
}


gboolean LAOnDataLoaderNormChanged(GtkWidget *widget, LADataLoaderWindow *lad){
	size_t index = gtk_combo_box_get_active(GTK_COMBO_BOX(lad->normComboBox));

	for(size_t i = 0; i < LA_NORM_PARAMETER_COUNT; i++){
		if(LANormDetails[index].param[i].name != NULL){
			gtk_adjustment_configure(GTK_ADJUSTMENT(lad->normParam[i].adjustment),
					LANormDetails[index].param[i].value,
					LANormDetails[index].param[i].min,
					LANormDetails[index].param[i].max,
					LANormDetails[index].param[i].stepIncrement,
					LANormDetails[index].param[i].pageIncrement,
					0
				);
			gtk_spin_button_set_value(GTK_SPIN_BUTTON(lad->normParam[i].spinButton),
					LANormDetails[index].param[i].value
				);
			gtk_spin_button_set_digits(GTK_SPIN_BUTTON(lad->normParam[i].spinButton),
					LANormDetails[index].param[i].digits
				);
			gtk_label_set_text(GTK_LABEL(lad->normParam[i].label), LANormDetails[index].param[i].name);
			gtk_widget_set_visible(lad->normParam[i].label, TRUE);
			gtk_widget_set_visible(lad->normParam[i].spinButton, TRUE);

		} else {
			gtk_widget_set_visible(lad->normParam[i].label, FALSE);
			gtk_widget_set_visible(lad->normParam[i].spinButton, FALSE);
		}
	}

	(void) widget;
	return TRUE;
}
