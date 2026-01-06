#include <gtk/gtk.h>
#include <cairo.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "liblogicanalyzer.h"

#define GRAPH_MINI_WIDTH 	256
#define GRAPH_MINI_HEIGHT 	128

#define GRAPH_WIDTH 	768
#define GRAPH_HEIGHT 	384

typedef struct {
	GtkWidget *window;
	GtkWidget *vbox;

	GtkWidget *hboxMain;
	GtkWidget *mainTypeLabel;
	GtkWidget *mainTypeSelector;
	GtkWidget *mainSizeLabel;
	GtkWidget *mainSizeSelector;

	GtkWidget 		*hbox;
	GtkWidget 		*vboxFIR;
	GtkWidget 		*firLabel;
	GtkWidget 		*firTypeLabel;
	GtkWidget 		*firTypeSelector;
	GtkWidget 		*firFreqLabel;
	GtkAdjustment 	*firFreqAdj;
	GtkWidget 		*firFreqSB;
	GtkWidget 		*firFreqGraph;
	GtkWidget 		*firWindowLabel;
	GtkWidget 		*firWindowSelector;
	GtkWidget 		*firWindowGraph;
	GtkWidget 		*firParam1Label;
	GtkAdjustment 	*firParam1Adj;
	GtkWidget 		*firParam1SB;
	GtkWidget 		*firParam2Label;
	GtkAdjustment 	*firParam2Adj;
	GtkWidget 		*firParam2SB;
	GtkWidget 		*firParam3Label;
	GtkAdjustment 	*firParam3Adj;
	GtkWidget 		*firParam3SB;

	GtkWidget 		*vboxIIR;
	GtkWidget 		*iirLabel;
	GtkWidget 		*iirIRGraph;

	GtkWidget 		*vboxMA;
	GtkWidget 		*maLabel;
	GtkWidget 		*maLengthLabel;
	GtkAdjustment 	*maLengthAdj;
	GtkWidget 		*maLengthSB;

	GtkWidget 		*vboxIR;
	GtkWidget 		*irLabel;
	GtkWidget 		*irGraph;

	GtkWidget 		*hboxButtons;
	GtkWidget 		*apply;
	GtkWidget 		*cancel;

} LAFilterWindow;

LAErrorCode LAOpenFilterWindow(LAFilterWindow *laf){

	laf->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(laf->window), "Filter Settings");
	gtk_container_set_border_width(GTK_CONTAINER(laf->window), 8);

	laf->vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
	gtk_container_add(GTK_CONTAINER(laf->window), GTK_WIDGET(laf->vbox));

	laf->hboxMain = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
	gtk_container_add(GTK_CONTAINER(laf->vbox), GTK_WIDGET(laf->hboxMain));

	laf->hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
	gtk_container_add(GTK_CONTAINER(laf->vbox), GTK_WIDGET(laf->hbox));
	laf->vboxFIR = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
	gtk_container_add(GTK_CONTAINER(laf->hbox), GTK_WIDGET(laf->vboxFIR));
	laf->vboxIIR = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
	gtk_container_add(GTK_CONTAINER(laf->hbox), GTK_WIDGET(laf->vboxIIR));
	laf->vboxMA  = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
	gtk_container_add(GTK_CONTAINER(laf->hbox), GTK_WIDGET(laf->vboxMA));
	laf->vboxIR  = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
	gtk_container_add(GTK_CONTAINER(laf->hbox), GTK_WIDGET(laf->vboxIR));

	laf->hboxButtons = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
	gtk_container_add(GTK_CONTAINER(laf->vbox), GTK_WIDGET(laf->hboxButtons));

	laf->mainTypeLabel 			= gtk_label_new("Filter type:");
	laf->mainTypeSelector 		= gtk_combo_box_text_new();
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->mainTypeSelector), "FIR");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->mainTypeSelector), "IIR");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->mainTypeSelector), "Moving Average");
	gtk_combo_box_set_active(GTK_COMBO_BOX(laf->mainTypeSelector), 0);
	laf->mainSizeLabel 			= gtk_label_new("Filter size:");
	laf->mainSizeSelector 		= gtk_combo_box_text_new();
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->mainSizeSelector), "2 samples");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->mainSizeSelector), "4 samples");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->mainSizeSelector), "8 samples");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->mainSizeSelector), "16 samples");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->mainSizeSelector), "32 samples");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->mainSizeSelector), "64 samples");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->mainSizeSelector), "128 samples");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->mainSizeSelector), "256 samples");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->mainSizeSelector), "512 samples");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->mainSizeSelector), "1024 samples");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->mainSizeSelector), "2048 samples");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->mainSizeSelector), "4096 samples");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->mainSizeSelector), "8192 samples");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->mainSizeSelector), "16384 samples");
	gtk_combo_box_set_active(GTK_COMBO_BOX(laf->mainSizeSelector), 0);
	gtk_container_add(GTK_CONTAINER(laf->hboxMain), GTK_WIDGET(laf->mainTypeLabel));
	gtk_container_add(GTK_CONTAINER(laf->hboxMain), GTK_WIDGET(laf->mainTypeSelector));

	laf->firLabel				= gtk_label_new("Finite Impulse Response (FIR)");
	laf->firTypeLabel			= gtk_label_new("Filter type:");
	laf->firTypeSelector 		= gtk_combo_box_text_new();
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->firTypeSelector), "Low pass");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->firTypeSelector), "High pass");
	gtk_combo_box_set_active(GTK_COMBO_BOX(laf->firTypeSelector), 0);
	laf->firFreqLabel			= gtk_label_new("Frequency:");
	laf->firFreqAdj				= gtk_adjustment_new(0, 0, 0, 0, 0, 0);
	laf->firFreqSB				= gtk_spin_button_new(laf->firFreqAdj, 1, 1);
	laf->firFreqGraph			= gtk_drawing_area_new();
	gtk_widget_set_size_request(laf->firFreqGraph, GRAPH_MINI_WIDTH, GRAPH_MINI_HEIGHT);
	laf->firWindowLabel			= gtk_label_new("Window type:");
	laf->firWindowSelector 		= gtk_combo_box_text_new();
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->firWindowSelector), "Rectangular");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->firWindowSelector), "Triangular");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->firWindowSelector), "Welch");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->firWindowSelector), "Hann");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->firWindowSelector), "Hamming");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->firWindowSelector), "Trapezoidal");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->firWindowSelector), "Circular");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->firWindowSelector), "Sinc");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->firWindowSelector), "Impulse");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->firWindowSelector), "Blackman");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->firWindowSelector), "Blackman-Harris");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->firWindowSelector), "Kaiser");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->firWindowSelector), "Gaussian");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->firWindowSelector), "Nutall");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->firWindowSelector), "Flattop");
	gtk_combo_box_set_active(GTK_COMBO_BOX(laf->firWindowSelector), 0);
	laf->firWindowGraph			= gtk_drawing_area_new();
	gtk_widget_set_size_request(laf->firWindowGraph, GRAPH_MINI_WIDTH, GRAPH_MINI_HEIGHT);
	laf->firParam1Label			= gtk_label_new("Parameter 1:");
	laf->firParam1Adj			= gtk_adjustment_new(0, 0, 0, 0, 0, 0);
	laf->firParam1SB			= gtk_spin_button_new(laf->firParam1Adj, 1, 1);
	laf->firParam2Label			= gtk_label_new("Parameter 2:");
	laf->firParam2Adj			= gtk_adjustment_new(0, 0, 0, 0, 0, 0);
	laf->firParam2SB			= gtk_spin_button_new(laf->firParam2Adj, 1, 1);
	laf->firParam3Label			= gtk_label_new("Parameter 3:");
	laf->firParam3Adj			= gtk_adjustment_new(0, 0, 0, 0, 0, 0);
	laf->firParam3SB			= gtk_spin_button_new(laf->firParam3Adj, 1, 1);

	gtk_container_add(GTK_CONTAINER(laf->vboxFIR), GTK_WIDGET(laf->firLabel));
	gtk_container_add(GTK_CONTAINER(laf->vboxFIR), GTK_WIDGET(laf->mainSizeLabel));
	gtk_container_add(GTK_CONTAINER(laf->vboxFIR), GTK_WIDGET(laf->mainSizeSelector));
	gtk_container_add(GTK_CONTAINER(laf->vboxFIR), GTK_WIDGET(laf->firTypeLabel));
	gtk_container_add(GTK_CONTAINER(laf->vboxFIR), GTK_WIDGET(laf->firTypeSelector));
	gtk_container_add(GTK_CONTAINER(laf->vboxFIR), GTK_WIDGET(laf->firFreqLabel));
	gtk_container_add(GTK_CONTAINER(laf->vboxFIR), GTK_WIDGET(laf->firFreqSB));
	gtk_container_add(GTK_CONTAINER(laf->vboxFIR), GTK_WIDGET(laf->firFreqGraph));
	gtk_container_add(GTK_CONTAINER(laf->vboxFIR), GTK_WIDGET(laf->firWindowLabel));
	gtk_container_add(GTK_CONTAINER(laf->vboxFIR), GTK_WIDGET(laf->firWindowSelector));
	gtk_container_add(GTK_CONTAINER(laf->vboxFIR), GTK_WIDGET(laf->firWindowGraph));
	gtk_container_add(GTK_CONTAINER(laf->vboxFIR), GTK_WIDGET(laf->firParam1Label));
	gtk_container_add(GTK_CONTAINER(laf->vboxFIR), GTK_WIDGET(laf->firParam1SB));
	gtk_container_add(GTK_CONTAINER(laf->vboxFIR), GTK_WIDGET(laf->firParam2Label));
	gtk_container_add(GTK_CONTAINER(laf->vboxFIR), GTK_WIDGET(laf->firParam2SB));
	gtk_container_add(GTK_CONTAINER(laf->vboxFIR), GTK_WIDGET(laf->firParam3Label));
	gtk_container_add(GTK_CONTAINER(laf->vboxFIR), GTK_WIDGET(laf->firParam3SB));

	laf->iirLabel				= gtk_label_new("Infinite Impulse Response (IIR)");
	laf->iirIRGraph				= gtk_drawing_area_new();
	gtk_widget_set_size_request(laf->iirIRGraph, GRAPH_MINI_WIDTH, GRAPH_MINI_HEIGHT);

	gtk_container_add(GTK_CONTAINER(laf->vboxIIR), GTK_WIDGET(laf->iirLabel));
	gtk_container_add(GTK_CONTAINER(laf->vboxIIR), GTK_WIDGET(laf->iirIRGraph));

	laf->maLabel				= gtk_label_new("Moving Average (MA)");
	laf->maLengthLabel			= gtk_label_new("Length:");
	laf->maLengthAdj			= gtk_adjustment_new(0, 0, 0, 0, 0, 0);
	laf->maLengthSB				= gtk_spin_button_new(laf->maLengthAdj, 1, 1);

	gtk_container_add(GTK_CONTAINER(laf->vboxMA), GTK_WIDGET(laf->maLabel));
	gtk_container_add(GTK_CONTAINER(laf->vboxMA), GTK_WIDGET(laf->maLengthLabel));
	gtk_container_add(GTK_CONTAINER(laf->vboxMA), GTK_WIDGET(laf->maLengthSB));

	laf->apply 					= gtk_button_new_with_label("Apply");
	laf->cancel					= gtk_button_new_with_label("Cancel");

	gtk_container_add(GTK_CONTAINER(laf->hboxButtons), GTK_WIDGET(laf->apply));
	gtk_container_add(GTK_CONTAINER(laf->hboxButtons), GTK_WIDGET(laf->cancel));

	laf->irLabel				= gtk_label_new("Impulse Response");
	laf->irGraph				= gtk_drawing_area_new();
	gtk_widget_set_size_request(laf->irGraph, GRAPH_WIDTH, GRAPH_HEIGHT);

	gtk_container_add(GTK_CONTAINER(laf->vboxIR), GTK_WIDGET(laf->irLabel));
	gtk_container_add(GTK_CONTAINER(laf->vboxIR), GTK_WIDGET(laf->irGraph));


	gtk_widget_set_can_focus(laf->window, TRUE);
	gtk_widget_grab_focus(laf->window);
	gtk_widget_show_all(laf->window);
	gtk_main();
}

void LAOnFilterButtonWave(GtkWidget *widget, LAWaveformEditor *lae){
	LAFilterWindow lafv;
	LAErrorCode code = LAOpenFilterWindow(&lafv);
}
