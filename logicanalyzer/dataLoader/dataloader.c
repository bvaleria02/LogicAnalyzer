#include <gtk/gtk.h>
#include <cairo.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "../liblogicanalyzer.h"
#include "../error.h"
#include "../types.h"
#include "../utils.h"
#include "../gtk_funcs.h"
#include "../filter/windows.h"
#include "../filter/filter.h"
#include "../filterwindow.h"
#include "dataLoader.h"
#include <time.h>
#include <pthread.h>
#include <math.h>

LAErrorCode LAOpenDataLoader(LADataLoaderWindow *lad, LAWindow *law){
	LA_HANDLE_NULLPTR(lad, 		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(law, 		LA_PROPAGATE_ERROR);

	lad->window = gtk_dialog_new_with_buttons(
					"Data Loader",
					GTK_WINDOW(lawp->window),
					GTK_DIALOG_DESTROY_WITH_PARENT,
					"_Accept", GTK_RESPONSE_OK,
					"_Quit", GTK_RESPONSE_CANCEL,
					NULL
				);

	gtk_window_set_modal(GTK_WINDOW(lad->window), FALSE);
	gtk_container_set_border_width(GTK_CONTAINER(lad->window), 8);
	lad->content = gtk_dialog_get_content_area(GTK_DIALOG(lad->window));

	lad->vbox 	= gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
	gtk_container_add(GTK_CONTAINER(lad->content), GTK_WIDGET(lad->vbox));
	lad->hbox	= gtk_notebook_new();
	gtk_container_add(GTK_CONTAINER(lad->vbox), GTK_WIDGET(lad->hbox));

	lad->frameSource 		= gtk_frame_new("Source:");
	lad->sourceVbox			= gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
	gtk_container_set_border_width(GTK_CONTAINER(lad->sourceVbox), 8);
	gtk_container_add(GTK_CONTAINER(lad->frameSource), GTK_WIDGET(lad->sourceVbox));

	lad->sourceRadioCircBuffer 	= gtk_radio_button_new_with_label(NULL, "From circular buffer");
	lad->sourceRadioFile 		= gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(lad->sourceRadioCircBuffer), "From file");
	lad->sourceOpenFileButton	= gtk_button_new_with_label("Open File");
	lad->sourceFileLabel		= gtk_label_new("No file selected");
	lad->sourceHboxFile 		= gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
	gtk_container_add(GTK_CONTAINER(lad->sourceVbox), GTK_WIDGET(lad->sourceRadioCircBuffer));
	gtk_container_add(GTK_CONTAINER(lad->sourceVbox), GTK_WIDGET(lad->sourceRadioFile));
	gtk_container_add(GTK_CONTAINER(lad->sourceVbox), GTK_WIDGET(lad->sourceHboxFile));
	gtk_container_add(GTK_CONTAINER(lad->sourceHboxFile), GTK_WIDGET(lad->sourceOpenFileButton));
	gtk_container_add(GTK_CONTAINER(lad->sourceHboxFile), GTK_WIDGET(lad->sourceFileLabel));

	lad->frameData 			= gtk_frame_new("Data:");
	lad->dataVbox			= gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
	gtk_container_set_border_width(GTK_CONTAINER(lad->dataVbox), 8);
	gtk_container_add(GTK_CONTAINER(lad->frameData), GTK_WIDGET(lad->dataVbox));

	lad->dataLabel			= gtk_label_new("Data type:");
	lad->dataComboBox		= gtk_combo_box_text_new();
	LADataLoaderComboBoxFiller(lad->dataComboBox);

	lad->endianLabel		= gtk_label_new("Endianness");
	lad->endianComboBox		= gtk_combo_box_text_new();
	LADataLoaderComboBoxEndiannessFiller(lad->endianComboBox);

	lad->dataGrid			= gtk_grid_new();
	lad->fileSizeLabel		= gtk_label_new("Size:");
	gtk_label_set_xalign(GTK_LABEL(lad->fileSizeLabel), 0);
	lad->fileSizeValue		= gtk_label_new(" ");
	gtk_label_set_xalign(GTK_LABEL(lad->fileSizeValue), 0);
	lad->sampleCountLabel	= gtk_label_new("Samples:");
	gtk_label_set_xalign(GTK_LABEL(lad->sampleCountLabel), 0);
	lad->sampleCountValue	= gtk_label_new(" ");
	gtk_label_set_xalign(GTK_LABEL(lad->sampleCountLabel), 0);

	
	lad->dataRecalculate	= gtk_button_new_with_label("Recalculate sample count");
	LALabelSpinComboInit(&(lad->offset), 		"Offset", 0, 0, 2147483647, 1, 256, 0);
	LALabelSpinComboInit(&(lad->sampleCount), 	"Size",   0, 0, 2147483647, 1, 256, 0);

	gtk_container_add(GTK_CONTAINER(lad->dataVbox), GTK_WIDGET(lad->dataLabel));
	gtk_container_add(GTK_CONTAINER(lad->dataVbox), GTK_WIDGET(lad->dataComboBox));
	gtk_container_add(GTK_CONTAINER(lad->dataVbox), GTK_WIDGET(lad->endianLabel));
	gtk_container_add(GTK_CONTAINER(lad->dataVbox), GTK_WIDGET(lad->endianComboBox));
	gtk_container_add(GTK_CONTAINER(lad->dataVbox), GTK_WIDGET(lad->dataGrid));
	gtk_container_add(GTK_CONTAINER(lad->dataVbox), GTK_WIDGET(lad->dataRecalculate));
	LALabelSpinComboAdd(&(lad->offset), lad->dataVbox);
	LALabelSpinComboAdd(&(lad->sampleCount), lad->dataVbox);
	gtk_grid_attach(GTK_GRID(lad->dataGrid), lad->fileSizeLabel, 		0, 0, 2, 1);
	gtk_grid_attach(GTK_GRID(lad->dataGrid), lad->fileSizeValue, 		2, 0, 4, 1);
	gtk_grid_attach(GTK_GRID(lad->dataGrid), lad->sampleCountLabel, 	0, 1, 2, 1);
	gtk_grid_attach(GTK_GRID(lad->dataGrid), lad->sampleCountValue, 	2, 1, 4, 1);

	lad->frameClamp 			= gtk_frame_new("Clamp:");
	lad->clampHbox				= gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
	gtk_container_set_border_width(GTK_CONTAINER(lad->clampHbox), 8);
	gtk_container_add(GTK_CONTAINER(lad->frameClamp), GTK_WIDGET(lad->clampHbox));
	lad->clampVboxL				= gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
	gtk_container_add(GTK_CONTAINER(lad->clampHbox), GTK_WIDGET(lad->clampVboxL));
	lad->clampVboxR				= gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
	gtk_container_add(GTK_CONTAINER(lad->clampHbox), GTK_WIDGET(lad->clampVboxR));

	lad->clampGraph				= gtk_drawing_area_new();
	gtk_widget_set_size_request(lad->clampGraph, CLAMP_GRAPH_WIDTH, CLAMP_GRAPH_HEIGHT);
	gtk_container_add(GTK_CONTAINER(lad->clampVboxL), GTK_WIDGET(lad->clampGraph));
	lad->clampLabel				= gtk_label_new("Clamp type");
	gtk_container_add(GTK_CONTAINER(lad->clampVboxR), GTK_WIDGET(lad->clampLabel));
	lad->clampComboBox			= gtk_combo_box_text_new();
	LADataLoaderComboBoxClampFiller(lad->clampComboBox);
	gtk_container_add(GTK_CONTAINER(lad->clampVboxR), GTK_WIDGET(lad->clampComboBox));
	lad->clampInstLabel			= gtk_label_new("Preview signal");
	gtk_container_add(GTK_CONTAINER(lad->clampVboxR), GTK_WIDGET(lad->clampInstLabel));
	lad->clampInstComboBox			= gtk_combo_box_text_new();
	LADataLoaderComboBoxClampInstFiller(lad->clampInstComboBox);
	gtk_container_add(GTK_CONTAINER(lad->clampVboxR), GTK_WIDGET(lad->clampInstComboBox));

	for(uint8_t i = 0; i < LA_CLAMP_PARAMETER_COUNT; i++){
		LALabelSpinComboInit(&(lad->clampParam[i]), "Parameter:", 0, -1, 1, 0.001, 0.01, 3);
		LALabelSpinComboAdd(&(lad->clampParam[i]), lad->clampVboxR);
		LALabelSpinComboConnect(&(lad->clampParam[i]), lad->clampGraph);
	}

	lad->clampDFTLabel			= gtk_label_new("Spectrogram");
	gtk_container_add(GTK_CONTAINER(lad->clampVboxL), GTK_WIDGET(lad->clampDFTLabel));
	LALabelSpinComboInit(&(lad->clampGain), "Gain", 1, 0, 32767, 0.001, 0.01, 3);
	LALabelSpinComboAdd(&(lad->clampGain), lad->clampVboxR);
	LALabelSpinComboConnect(&(lad->clampGain), lad->clampGraph);

	lad->clampGraphDFT				= gtk_drawing_area_new();
	gtk_widget_set_size_request(lad->clampGraphDFT, CLAMP_GRAPH_WIDTH, CLAMP_GRAPH_HEIGHT);
	gtk_container_add(GTK_CONTAINER(lad->clampVboxL), GTK_WIDGET(lad->clampGraphDFT));
	lad->clampGraphInfo				= gtk_drawing_area_new();
	gtk_widget_set_size_request(lad->clampGraphInfo, CLAMP_INFO_GRAPH_WIDTH, CLAMP_INFO_GRAPH_HEIGHT);
	gtk_container_add(GTK_CONTAINER(lad->clampVboxL), GTK_WIDGET(lad->clampGraphInfo));

	g_signal_connect(lad->clampGraph,  	 "draw", 	G_CALLBACK(LAOnDataLoaderClampDraw),    lad);
	g_signal_connect(lad->clampGraphDFT, "draw", 	G_CALLBACK(LAOnDataLoaderClampDFTDraw), lad);
	g_signal_connect(lad->clampGraphInfo, "draw", 	G_CALLBACK(LAOnDataLoaderClampInfoDraw), lad);
	g_signal_connect(lad->clampComboBox, "changed", G_CALLBACK(LAOnDataLoaderClampChanged), lad);
	g_signal_connect(lad->clampInstComboBox, "changed", G_CALLBACK(LAOnDataLoaderClampInstChanged), lad);

	lad->frameNorm 				= gtk_frame_new("Normalize:");
	lad->normVbox				= gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
	gtk_container_set_border_width(GTK_CONTAINER(lad->normVbox), 8);
	gtk_container_add(GTK_CONTAINER(lad->frameNorm), GTK_WIDGET(lad->normVbox));
	lad->normLabel				= gtk_label_new("Norm type");
	gtk_container_add(GTK_CONTAINER(lad->normVbox), GTK_WIDGET(lad->normLabel));
	lad->normComboBox			= gtk_combo_box_text_new();
	LADataLoaderComboBoxNormFiller(lad->normComboBox);
	gtk_container_add(GTK_CONTAINER(lad->normVbox), GTK_WIDGET(lad->normComboBox));

	for(uint8_t i = 0; i < LA_NORM_PARAMETER_COUNT; i++){
		LALabelSpinComboInit(&(lad->normParam[i]), "Parameter:", 0, -1, 1, 0.001, 0.01, 3);
		LALabelSpinComboAdd(&(lad->normParam[i]), lad->normVbox);
	}

	g_signal_connect(lad->normComboBox, "changed", G_CALLBACK(LAOnDataLoaderNormChanged), lad);

	lad->frameMisc 				= gtk_frame_new("Miscellaneous:");
	lad->miscVbox				= gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
	gtk_container_set_border_width(GTK_CONTAINER(lad->miscVbox), 8);
	gtk_container_add(GTK_CONTAINER(lad->frameMisc), GTK_WIDGET(lad->miscVbox));

	gtk_notebook_append_page(GTK_NOTEBOOK(lad->hbox), lad->frameSource, gtk_label_new("Source"));
	gtk_notebook_append_page(GTK_NOTEBOOK(lad->hbox), lad->frameData, gtk_label_new("Data"));
	gtk_notebook_append_page(GTK_NOTEBOOK(lad->hbox), lad->frameClamp, gtk_label_new("Clamp"));
	gtk_notebook_append_page(GTK_NOTEBOOK(lad->hbox), lad->frameNorm, gtk_label_new("Normalization"));
	gtk_notebook_append_page(GTK_NOTEBOOK(lad->hbox), lad->frameMisc, gtk_label_new("Misc"));


	gtk_widget_show_all(lad->window);
	gtk_dialog_run(GTK_DIALOG(lad->window));
	gtk_widget_destroy(lad->window);
	
	return LA_NO_ERROR;
}
