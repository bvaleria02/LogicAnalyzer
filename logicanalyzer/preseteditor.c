#include <gtk/gtk.h>
#include <cairo.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "liblogicanalyzer.h"
#include "compiler.h"
#include <time.h>
#include <pthread.h>
#include <stdatomic.h>

#define DEFAULT_HEADER_HEIGHT 	7
#define DEFAULT_HEADER_WIDTH 	5

typedef struct {
	GtkWidget *window;
	GtkWidget *content;
	GtkWidget *vbox;

	GtkWidget *frameFile;
	GtkWidget *hboxFile;
	GtkWidget *fileOpenButton;
	GtkWidget *fileOpenFilename;
	
	GtkWidget *frameHeader;
	GtkWidget *gridHeader;
	GtkWidget *headerMagicLabel;
	GtkWidget *headerMagicValue;
	GtkWidget *headerVersionLabel;
	GtkWidget *headerVersionValue;
	GtkWidget *headerPluginIdLabel;
	GtkWidget *headerPluginIdValue;
	GtkWidget *headerNameLabel;
	GtkWidget *headerNameValue;
	GtkWidget *headerNameApply;
	GtkWidget *headerNameLengthLabel;
	GtkWidget *headerNameLengthValue;
	GtkWidget *headerCRC32ALabel;
	GtkWidget *headerCRC32AValue;
	GtkWidget *headerCRC32BLabel;
	GtkWidget *headerCRC32BValue;

	GtkWidget *frameData;
	GtkWidget *vboxData;
	GtkWidget *dataLengthHbox;
	GtkWidget *dataLengthLabel;
	GtkWidget *dataLengthValue;
	GtkWidget *dataContainer;
	GtkWidget *dataView;
	GtkListStore *dataStore;
	GtkCellRenderer *dataRenderer;
	GtkTreeIter dataIter;
	GtkWidget *hboxData;
	GtkWidget *dataType;
	GtkWidget *dataValue;
	GtkWidget *dataButtonAdd;

} LAPresetEditor;

LAErrorCode LAOpenPresetEditorWindow(LAPresetEditor *lap, LAWindow *law){

	lap->window = gtk_dialog_new_with_buttons(
					"LAPF Preset Editor",
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

	lap->frameFile = gtk_frame_new("File:");
	gtk_container_add(GTK_CONTAINER(lap->vbox), GTK_WIDGET(lap->frameFile));
	lap->hboxFile = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
	gtk_container_add(GTK_CONTAINER(lap->frameFile), GTK_WIDGET(lap->hboxFile));
	gtk_container_set_border_width(GTK_CONTAINER(lap->hboxFile), 8);

	lap->frameHeader = gtk_frame_new("Header:");
	gtk_container_add(GTK_CONTAINER(lap->vbox), GTK_WIDGET(lap->frameHeader));
	lap->gridHeader = gtk_grid_new();
	gtk_container_add(GTK_CONTAINER(lap->frameHeader), GTK_WIDGET(lap->gridHeader));
	gtk_container_set_border_width(GTK_CONTAINER(lap->gridHeader), 8);

	lap->frameData = gtk_frame_new("Data:");
	gtk_container_add(GTK_CONTAINER(lap->vbox), GTK_WIDGET(lap->frameData));
	lap->vboxData = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
	gtk_container_add(GTK_CONTAINER(lap->frameData), GTK_WIDGET(lap->vboxData));
	gtk_container_set_border_width(GTK_CONTAINER(lap->vboxData), 8);

	lap->fileOpenButton		= gtk_button_new_with_label("Open file");
	lap->fileOpenFilename	= gtk_label_new("No file selected");
	gtk_container_add(GTK_CONTAINER(lap->hboxFile), GTK_WIDGET(lap->fileOpenButton));
	gtk_container_add(GTK_CONTAINER(lap->hboxFile), GTK_WIDGET(lap->fileOpenFilename));

	lap->headerMagicLabel		= gtk_label_new("Magic number:");
	gtk_label_set_xalign(GTK_LABEL(lap->headerMagicLabel), 0.0);
	lap->headerMagicValue		= gtk_label_new(" ");
	gtk_label_set_xalign(GTK_LABEL(lap->headerMagicValue), 0.0);
	lap->headerVersionLabel		= gtk_label_new("Version:");
	gtk_label_set_xalign(GTK_LABEL(lap->headerVersionLabel), 0.0);
	lap->headerVersionValue		= gtk_label_new(" ");
	gtk_label_set_xalign(GTK_LABEL(lap->headerVersionValue), 0.0);
	lap->headerPluginIdLabel	= gtk_label_new("Plugin Id:");
	gtk_label_set_xalign(GTK_LABEL(lap->headerPluginIdLabel), 0.0);
	lap->headerPluginIdValue 	= gtk_combo_box_text_new();
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(lap->headerPluginIdValue), "Filter editor");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(lap->headerPluginIdValue), "Waveform editor");
	gtk_combo_box_set_active(GTK_COMBO_BOX(lap->headerPluginIdValue), 0);
	lap->headerNameLabel		= gtk_label_new("Name:");
	gtk_label_set_xalign(GTK_LABEL(lap->headerNameLabel), 0.0);
	lap->headerNameValue		= gtk_entry_new();
	lap->headerNameApply		= gtk_button_new_with_label("Change");
	lap->headerNameLengthLabel	= gtk_label_new("Name Length:");
	gtk_label_set_xalign(GTK_LABEL(lap->headerNameLengthLabel), 0.0);
	lap->headerNameLengthValue	= gtk_label_new(" ");
	gtk_label_set_xalign(GTK_LABEL(lap->headerNameLengthValue), 0.0);
	lap->headerCRC32ALabel		= gtk_label_new("Header CRC32:");
	gtk_label_set_xalign(GTK_LABEL(lap->headerCRC32ALabel), 0.0);
	lap->headerCRC32AValue		= gtk_label_new(" ");
	gtk_label_set_xalign(GTK_LABEL(lap->headerCRC32AValue), 0.0);
	lap->headerCRC32BLabel		= gtk_label_new("Data CRC32:");
	gtk_label_set_xalign(GTK_LABEL(lap->headerCRC32BLabel), 0.0);
	lap->headerCRC32BValue		= gtk_label_new(" ");
	gtk_label_set_xalign(GTK_LABEL(lap->headerCRC32BValue), 0.0);

	gtk_grid_attach(GTK_GRID(lap->gridHeader), GTK_WIDGET(lap->headerMagicLabel), 		0, 0, 2, 1);
	gtk_grid_attach(GTK_GRID(lap->gridHeader), GTK_WIDGET(lap->headerMagicValue), 		2, 0, 4, 1);
	gtk_grid_attach(GTK_GRID(lap->gridHeader), GTK_WIDGET(lap->headerVersionLabel), 	0, 1, 2, 1);
	gtk_grid_attach(GTK_GRID(lap->gridHeader), GTK_WIDGET(lap->headerVersionValue),		2, 1, 4, 1);
	gtk_grid_attach(GTK_GRID(lap->gridHeader), GTK_WIDGET(lap->headerPluginIdLabel), 	0, 2, 2, 1);
	gtk_grid_attach(GTK_GRID(lap->gridHeader), GTK_WIDGET(lap->headerPluginIdValue), 	2, 2, 4, 1);
	gtk_grid_attach(GTK_GRID(lap->gridHeader), GTK_WIDGET(lap->headerNameLabel), 		0, 3, 2, 1);
	gtk_grid_attach(GTK_GRID(lap->gridHeader), GTK_WIDGET(lap->headerNameValue), 		2, 3, 8, 1);
	gtk_grid_attach(GTK_GRID(lap->gridHeader), GTK_WIDGET(lap->headerNameApply), 		10,3, 2, 1);
	gtk_grid_attach(GTK_GRID(lap->gridHeader), GTK_WIDGET(lap->headerNameLengthLabel), 	0, 4, 2, 1);
	gtk_grid_attach(GTK_GRID(lap->gridHeader), GTK_WIDGET(lap->headerNameLengthValue),	2, 4, 4, 1);
	gtk_grid_attach(GTK_GRID(lap->gridHeader), GTK_WIDGET(lap->headerCRC32ALabel), 		0, 5, 2, 1);
	gtk_grid_attach(GTK_GRID(lap->gridHeader), GTK_WIDGET(lap->headerCRC32AValue), 		2, 6, 4, 1);
	gtk_grid_attach(GTK_GRID(lap->gridHeader), GTK_WIDGET(lap->headerCRC32BLabel), 		0, 6, 2, 1);
	gtk_grid_attach(GTK_GRID(lap->gridHeader), GTK_WIDGET(lap->headerCRC32BValue), 		2, 6, 4, 1);

	lap->dataLengthHbox 	= gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
	gtk_container_add(GTK_CONTAINER(lap->vboxData), GTK_WIDGET(lap->dataLengthHbox));
	lap->dataLengthLabel	= gtk_label_new("Data length:");
	gtk_label_set_xalign(GTK_LABEL(lap->dataLengthLabel), 0.0);
	lap->dataLengthValue	= gtk_label_new(" ");
	gtk_label_set_xalign(GTK_LABEL(lap->dataLengthValue), 0.0);
	gtk_container_add(GTK_CONTAINER(lap->dataLengthHbox), GTK_WIDGET(lap->dataLengthLabel));
	gtk_container_add(GTK_CONTAINER(lap->dataLengthHbox), GTK_WIDGET(lap->dataLengthValue));

	lap->dataView			= gtk_tree_view_new();
	lap->dataStore			= gtk_list_store_new(5, G_TYPE_UINT, G_TYPE_UINT, G_TYPE_STRING, G_TYPE_UINT, G_TYPE_STRING);

	lap->dataRenderer		= gtk_cell_renderer_text_new();
	gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(lap->dataView), 0, "Index", lap->dataRenderer, "text", 0, NULL);
	lap->dataRenderer		= gtk_cell_renderer_text_new();
	gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(lap->dataView), 1, "Tag Id", lap->dataRenderer, "text", 1, NULL);
	lap->dataRenderer		= gtk_cell_renderer_text_new();
	gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(lap->dataView), 2, "Tag name", lap->dataRenderer, "text", 2, NULL);
	lap->dataRenderer		= gtk_cell_renderer_text_new();
	gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(lap->dataView), 3, "Length", lap->dataRenderer, "text", 3, NULL);
	lap->dataRenderer		= gtk_cell_renderer_text_new();
	gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(lap->dataView), 4, "Value", lap->dataRenderer, "text", 4, NULL);
	
	gtk_tree_view_set_model(GTK_TREE_VIEW(lap->dataView), GTK_TREE_MODEL(lap->dataStore));
	g_object_unref(lap->dataStore);

	lap->dataContainer		= gtk_scrolled_window_new(NULL, NULL);
	gtk_container_add(GTK_CONTAINER(lap->dataContainer), lap->dataView);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(lap->dataContainer), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_container_add(GTK_CONTAINER(lap->vboxData), GTK_WIDGET(lap->dataContainer));
	gtk_widget_set_size_request(lap->dataContainer, 512, 128);

	for(uint16_t i = 0; i < 32; i++){
		gtk_list_store_append(lap->dataStore, &(lap->dataIter));
 		gtk_list_store_set(lap->dataStore, &(lap->dataIter),
                      0, (long int)	i,
                      1, (long int) 2,
                      2, (gchar *) "No Operation",
                      3, (long int) 3,
                      4,  (gchar *)"A",
                      -1);
	}

	lap->hboxData 		= gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
	gtk_container_add(GTK_CONTAINER(lap->vboxData), GTK_WIDGET(lap->hboxData));
	lap->dataType		= gtk_combo_box_text_new();
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(lap->dataType), "0: No Operation");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(lap->dataType), "65535: End");
	gtk_combo_box_set_active(GTK_COMBO_BOX(lap->dataType), 0);
	lap->dataValue		= gtk_entry_new();
	lap->dataButtonAdd	= gtk_button_new_with_label("Add");
	gtk_container_add(GTK_CONTAINER(lap->hboxData), GTK_WIDGET(lap->dataType));
	gtk_container_add(GTK_CONTAINER(lap->hboxData), GTK_WIDGET(lap->dataValue));
	gtk_container_add(GTK_CONTAINER(lap->hboxData), GTK_WIDGET(lap->dataButtonAdd));

	gtk_widget_show_all(lap->window);
	int response = gtk_dialog_run(GTK_DIALOG(lap->window));
	gtk_widget_destroy(lap->window);
}

void LAOpenPresetEditor(GtkWidget *widget, LAWindow *law){
	LAPresetEditor lap;
	LAOpenPresetEditorWindow(&lap, law);
}
