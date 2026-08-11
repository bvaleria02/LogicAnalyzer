#include <gtk/gtk.h>
#include "liblogicanalyzer.h"
#include "error.h"
#include "types.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "fixedpoint/fixedpoint.h"
#include <math.h>

#define DEFAULT_TEXT "Choose one"
#define NON_SELECTED_VALUE 0xFFFFFFFFFFFFFFFF

gboolean LAOnBinarySizeWidgetChange(GtkWidget *widget, LABinarySizeWidget *lab){
	size_t value = gtk_combo_box_get_active(GTK_COMBO_BOX(lab->combobox));

	if(value == 0){
		lab->value = NON_SELECTED_VALUE;
	} else if (value > 0 && value <= lab->length){
		lab->value = lab->start + value - 1;
	} else{
		lab->value = lab->end;
	}

	if(lab->externalWidget != NULL){
		gtk_widget_queue_draw(lab->externalWidget);
	}

	(void) widget;
	return TRUE;
}

LAErrorCode LABinarySizeWidgetInit(LABinarySizeWidget *lab, char *name, size_t start, size_t end, char *unit){
	LA_HANDLE_NULLPTR(lab, 	LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(name, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(unit, LA_PROPAGATE_ERROR);

	lab->vbox 			= gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
	lab->name 			= gtk_label_new(name);
	lab->combobox 		= gtk_combo_box_text_new();
	lab->externalWidget = NULL;

	//	start = 2  end = 3 -> 3 - 2 = 1
	//  Blank + (end - start + 1)
	lab->length   = 1 + (end - start + 1);

	char buffer[LA_SMALL_BUFFER_SIZE];
	for(size_t i = 0; i <= lab->length; i++){
		if(i == 0){
			gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(lab->combobox), DEFAULT_TEXT);
		} else {
			snprintf(buffer, LA_SMALL_BUFFER_SIZE, "%li %s", (uint64_t) powl(2, start + i - 1), unit);
			gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(lab->combobox), buffer);
		}
	}

	gtk_combo_box_set_active(GTK_COMBO_BOX(lab->combobox), 0);

	gtk_container_add(GTK_CONTAINER(lab->vbox), GTK_WIDGET(lab->name));
	gtk_container_add(GTK_CONTAINER(lab->vbox), GTK_WIDGET(lab->combobox));

	g_signal_connect(lab->combobox,	"changed", G_CALLBACK(LAOnBinarySizeWidgetChange), lab);

	return LA_NO_ERROR;
}

LAErrorCode LABinarySizeWidgetAdd(LABinarySizeWidget *lab, GtkWidget *container){
	LA_HANDLE_NULLPTR(lab, 			LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(container, 	LA_PROPAGATE_ERROR);

	gtk_container_add(GTK_CONTAINER(container), GTK_WIDGET(lab->vbox));

	return LA_NO_ERROR;
}

LAErrorCode LABinarySizeWidgetConnect(LABinarySizeWidget *lab, GtkWidget *externalWidget){
	LA_HANDLE_NULLPTR(lab, 				LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(externalWidget, 	LA_PROPAGATE_ERROR);

	lab->externalWidget = externalWidget;

	return LA_NO_ERROR;
}
