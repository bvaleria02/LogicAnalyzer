#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <gtk/gtk.h>
#include "../liblogicanalyzer.h"
#include "dataLoader.h"

LAErrorCode LADataLoaderComboBoxFiller(GtkWidget *widget){
	LA_HANDLE_NULLPTR(widget, LA_PROPAGATE_ERROR);

	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widget), "Select format");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widget), "Binary data (1 bit)");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widget), "Capture data (uint8_t)");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widget), "Unsigned 8 bits integer (uint8_t)");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widget), "Signed 8 bits integer (int8_t)");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widget), "Unsigned 16 bits integer (uint16_t)");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widget), "Signed 16 bits integer (int16_t)");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widget), "Unsigned 32 bits integer (uint32_t)");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widget), "Signed 32 bits integer (int32_t)");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widget), "Unsigned 64 bits integer (uint64_t)");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widget), "Signed 64 bits integer (int64_t)");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widget), "Single precision floating point (float)");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widget), "Double precision floating point (double)");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widget), "Unsigned q16.16 fixed point (ufixed32_t)");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widget), "Signed q16.16 fixed point (fixed32_t)");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widget), "Unsigned q32.32 fixed point (ufixed64_t)");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widget), "Signed q32.32 fixed point (fixed64_t)");
	gtk_combo_box_set_active(GTK_COMBO_BOX(widget), 0);

	return LA_NO_ERROR;
}
