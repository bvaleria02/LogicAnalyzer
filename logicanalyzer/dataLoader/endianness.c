#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <gtk/gtk.h>
#include "../liblogicanalyzer.h"
#include "dataLoader.h"

LAErrorCode LADataLoaderComboBoxEndiannessFiller(GtkWidget *widget){
	LA_HANDLE_NULLPTR(widget, LA_PROPAGATE_ERROR);

	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widget), "Little Endian");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widget), "Big Endian");
	gtk_combo_box_set_active(GTK_COMBO_BOX(widget), 0);

	return LA_NO_ERROR;
}
