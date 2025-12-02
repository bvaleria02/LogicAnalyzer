#include <gtk/gtk.h>
#include <cairo.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "liblogicanalyzer.h"

void LAWindowCreateChannels(LAWindow *law){
	for(uint8_t i = 0; i < MAX_CHANNEL_COUNT; i++){
		LAWindowCreateChannel(&(law->channel[i]), i);
		gtk_container_add(GTK_CONTAINER(law->container), GTK_WIDGET(law->channel[i].container));
	}
/*
	gtk_container_add(GTK_CONTAINER(law->vbox), GTK_WIDGET(law->scrollableChannels));
	gtk_container_add(GTK_CONTAINER(law->scrollableChannels), GTK_WIDGET(law->container));
*/
	
	gtk_container_add(GTK_CONTAINER(law->vbox), GTK_WIDGET(law->container));
}


gboolean LAOnMouseScroll(GtkWidget *widget, GdkEventScroll *event, gpointer indexp){
	uint8_t index = CONVERT_GPOINTER_TO_INT(index);

	double dx = 0.0;
	double dy = 0.0;
	int direction = 0;
	gdk_event_get_scroll_deltas((GdkEvent *)event, &dx, &dy);
	if(event->direction == GDK_SCROLL_UP || dy < 0)				direction = 1;
	else if(event->direction == GDK_SCROLL_DOWN || dy > 0)		direction = -1;

	guint state    = event->state;
	gboolean ctrl  = state & GDK_CONTROL_MASK;
	gboolean shift = state & GDK_SHIFT_MASK;
	gboolean alt   = state & GDK_MOD1_MASK;

	if(ctrl){
		lawp->rd.zoom += direction;
	} else if(shift){
		lawp->rd.scopeOffset += LA_SMALL_INCREMENT_SCOPE * direction / LAGetZoomMultiplier(lawp);
	}
	
	LARedrawAllScopes(lawp);
	return FALSE;
}

void LAWindowCreateChannel(LAChannel *channel, uint8_t index){
	channel->bit = index;
	channel->container = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 16);

	channel->control 	= gtk_grid_new();
	//channel->scrollable = gtk_scrolled_window_new(NULL, NULL);
	channel->scope   	= gtk_drawing_area_new();
//	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(channel->scrollable), GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
	gtk_widget_set_size_request(channel->scope, SCOPE_WIDTH, SCOPE_HEIGTH);
	//gtk_widget_set_size_request(channel->scrollable, SCOPE_WIDTH, SCOPE_HEIGTH);

	channel->entry 			= gtk_entry_new();
	channel->muteButton 	= gtk_button_new_with_label("M");
	channel->soloButton 	= gtk_button_new_with_label("S");
	channel->colorButton 	= gtk_button_new_with_label("C");
	channel->label			= gtk_label_new("Channel:");

	channel->dropdown 		= gtk_combo_box_text_new();
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(channel->dropdown), "0");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(channel->dropdown), "1");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(channel->dropdown), "2");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(channel->dropdown), "3");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(channel->dropdown), "4");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(channel->dropdown), "5");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(channel->dropdown), "6");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(channel->dropdown), "7");
	gtk_combo_box_set_active(GTK_COMBO_BOX(channel->dropdown), channel->bit);

	gtk_grid_attach(GTK_GRID(channel->control), channel->entry,      	0, 1, 5, 1);
	gtk_grid_attach(GTK_GRID(channel->control), channel->muteButton, 	2, 0, 1, 1);
	gtk_grid_attach(GTK_GRID(channel->control), channel->soloButton, 	3, 0, 1, 1);
	gtk_grid_attach(GTK_GRID(channel->control), channel->colorButton, 	0, 0, 1, 1);
	gtk_grid_attach(GTK_GRID(channel->control), channel->label, 		0, 2, 2, 1);
	gtk_grid_attach(GTK_GRID(channel->control), channel->dropdown, 		2, 2, 3, 1);

	g_signal_connect(channel->muteButton, "clicked", G_CALLBACK(LAChannelMute), channel);
	g_signal_connect(channel->soloButton, "clicked", G_CALLBACK(LAChannelSolo), CONVERT_INT_TO_GPOINTER(index));
	g_signal_connect(channel->colorButton, "clicked", G_CALLBACK(LAChannelColorPicker), CONVERT_INT_TO_GPOINTER(index));
	g_signal_connect(channel->dropdown, "changed", G_CALLBACK(LAChannelChangeBit), channel);
	g_signal_connect(channel->scope,    "draw",    G_CALLBACK(LAChannelOnDraw), CONVERT_INT_TO_GPOINTER(index));
	gtk_widget_set_events(channel->scope, gtk_widget_get_events(channel->container) | GDK_SCROLL_MASK);
	g_signal_connect(channel->scope,    "scroll-event",    G_CALLBACK(LAOnMouseScroll), CONVERT_INT_TO_GPOINTER(index));


	gtk_container_add(GTK_CONTAINER(channel->container), GTK_WIDGET(channel->control));
	gtk_container_add(GTK_CONTAINER(channel->container), channel->scope);
//	gtk_container_add(GTK_CONTAINER(channel->container), GTK_WIDGET(channel->scrollable));

	channel->r = defaultChannelColors[index][0];
	channel->g = defaultChannelColors[index][1];
	channel->b = defaultChannelColors[index][2];
}

void LAChannelChangeBit(GtkWidget *widget, LAChannel *channel){
	gchar *text = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(widget));
	channel->bit = atoi((const char *)text);
	g_free(text);
	gtk_widget_queue_draw(channel->scope);
}

void LAChannelMute(GtkWidget *widget, LAChannel *channel){
	channel->muted = !(channel->muted);
	gtk_widget_queue_draw(channel->scope);
}

void LAMuteAllChannels(LAWindow *law){
	for(uint8_t i = 0; i < MAX_CHANNEL_COUNT; i++){
		law->channel[i].muted = 1;
	}
}

void LAEnableAllChannels(LAWindow *law){
	for(uint8_t i = 0; i < MAX_CHANNEL_COUNT; i++){
		law->channel[i].muted = 0;
	}
}

void LARedrawAllScopes(LAWindow *law){
	for(uint8_t i = 0; i < MAX_CHANNEL_COUNT; i++){
		gtk_widget_queue_draw(lawp->channel[i].scope);
	}
}

uint8_t LAIsChannelSolo(LAWindow *law, uint8_t index){
	for(uint8_t i = 0; i < MAX_CHANNEL_COUNT; i++){
		if(law->channel[i].muted == 1 && i != index){
			continue;
		} else if(law->channel[i].muted == 0 && i == index){
			continue;
		}

		return 0;
	}
	return 1;
}

void LAChannelSolo(GtkWidget *widget, uintptr_t indexp){
	uint8_t index = CONVERT_GPOINTER_TO_INT(indexp);
	
	uint8_t isSolo = LAIsChannelSolo(lawp, index);
	if(isSolo){
		LAEnableAllChannels(lawp);
	} else {
		LAMuteAllChannels(lawp);
		lawp->channel[index].muted = 0;
	}

	LARedrawAllScopes(lawp);
}

void LAChannelColorPicker(GtkWidget *widget, uintptr_t indexp){
	uint8_t index = CONVERT_GPOINTER_TO_INT(indexp);
	LAChannel *channel = &(lawp->channel[index]);
	GdkRGBA color = {channel->r, channel->g, channel->b, 1.0};
	
	GtkWidget *dialog = gtk_color_chooser_dialog_new("Color picker", GTK_WINDOW(lawp->window));
	gtk_color_chooser_set_rgba(GTK_COLOR_CHOOSER(dialog), &color);
	
	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_OK){
      	gtk_color_chooser_get_rgba(GTK_COLOR_CHOOSER (dialog), &color);
		channel->r = color.red;
		channel->g = color.green;
		channel->b = color.blue;
		gtk_widget_queue_draw(channel->scope);
    }

  gtk_widget_destroy(dialog);
}
