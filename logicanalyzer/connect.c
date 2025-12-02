#include <gtk/gtk.h>
#include <cairo.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "liblogicanalyzer.h"

#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#include <pthread.h>
#include <stdatomic.h>

#define COM_RATE B921600
//#define COM_RATE B2000000

void LAWindowCreateConnect(LAWindow *law){
	
}

void LAHandleDisconnect(LAWindow *law, LAConnectWindow *lac){
	if(law->connect.fd >= 0){
		LACloseDevice(law);
		atomic_store(&(law->connect.readThread), -1);
	}
	
	law->connect.internalDeviceId = 0;
	atomic_store(&(law->connect.isConnected), 0);
	gtk_text_buffer_set_text(GTK_TEXT_BUFFER(lac->textBuffer), "Device disconnected", -1);
	gtk_button_set_label(GTK_BUTTON(lac->connect), "Connect");
}

void LAHandleConnect(LAWindow *law, LAConnectWindow *lac){
	if(law->connect.readThread != -1){
		g_print("Return aaaaaa\n");
		return;
	}

	gchar *device = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(lac->dropdown));
	int internalDevice = gtk_combo_box_get_active(GTK_COMBO_BOX(lac->dropdown));
	if(device == NULL){
		return;
	}

	g_print("Device: %s\n", device);
	uint8_t r = LAConnectDevice(law, device);
	if(r){
		gtk_text_buffer_set_text(GTK_TEXT_BUFFER(lac->textBuffer), "An error ocurred opening the selected device.", -1);
		g_print("An error ocurred during connect\n");
		return;
	}

	law->connect.internalDeviceId = internalDevice + 1;
	atomic_store(&(law->connect.breakReadLoop), 0);

	LASendBasicSerial(law, LA_COMMAND_POLLING_RATE, DEFAULT_POLLING_RATE);
	LASendBasicSerial(law, LA_COMMAND_READ_ENABLE, DEFAULT_READ_ENABLE);
	LASendBasicSerial(law, LA_COMMAND_BUFFER_SIZE, DEFAULT_VIRTUAL_BUFFER_SIZE);

	int rc1 = pthread_create(&(law->connect.readThread), NULL, LAReadThread, law);
	g_print("Pthread rc1: %i\n", rc1);

	atomic_store(&(law->connect.isConnected), 1);
	gtk_text_buffer_set_text(GTK_TEXT_BUFFER(lac->textBuffer), "Device connected succesfully!", -1);
	gtk_button_set_label(GTK_BUTTON(lac->connect), "Disconnect");
}


void LAButtonConnectCallback(GtkWidget *widget, LAConnectWindow *lac){
	if(lawp->connect.isConnected){
		LAHandleDisconnect(lawp, lac);
	} else {
		LAHandleConnect(lawp, lac);
	}

	if(lawp->rd.hasRenderFunctionAdded == 0){
		lawp->rd.renderSourceId = g_timeout_add(50, LAWindowUpdateLoopConnector, lawp);			
		lawp->rd.hasRenderFunctionAdded = 1;
	}
	return;
}

uint8_t LAConnectDevice(LAWindow *law, gchar *device){
	law->connect.device = device;
	law->connect.fd = open(device, O_RDWR | O_NOCTTY | O_NDELAY);

	if(law->connect.fd < 0){
		g_print("Error %i from open: %s\n", errno, strerror(errno));
		return 1;
	}

	struct termios tty;
	if(tcgetattr(law->connect.fd, &tty) != 0){
		g_print("Error %i from tcgetattr: %s\n", errno, strerror(errno));
		return 1;
	}

	cfsetispeed(&tty, COM_RATE);
	cfsetospeed(&tty, COM_RATE);

	tty.c_cflag &= ~(PARENB | PARODD);	
	tty.c_cflag &= ~CSTOPB;
	tty.c_cflag &= ~CSIZE;
	tty.c_cflag |= CS8;
	tty.c_cflag |= CREAD | CLOCAL;

	tty.c_cc[VMIN] = 1;
	tty.c_cc[VTIME] = 0;

	if(tcsetattr(law->connect.fd, TCSANOW, &tty) != 0){
		printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
		return 1;
	}

	g_print("fd: %i\n", law->connect.fd);
	return 0;
}
	
void LACloseDevice(LAWindow *law){
	atomic_store(&(law->connect.breakReadLoop), 1);
	close(law->connect.fd);
	law->connect.fd = -1;
}


void LASendSerial(LASerialProtocol *p, LAWindow *law){
	if(p == NULL){
		return;
	}

	if(law->connect.fd < 0){
		return;
	}

	int r = write(law->connect.fd, p->frames, LA_SERIAL_FRAME_LENGTH);
	g_print("Data send. response: %i\n", r);
}

void LASendBasicSerial(LAWindow *law, uint8_t command, uint32_t value){
	LASerialProtocol p;
	LAPrepareProtocol(&p, command, value);
	LASendSerial(&p, law);
}


void LASerialCommandLoopback(LAWindow *law, uint8_t command, uint32_t value){
	switch(command){
		case LA_COMMAND_POLLING_RATE		: law->rd.pollingTime = value;
											  break;
		case LA_COMMAND_BUFFER_SIZE			: law->rd.virtualBufferSize = value;
											  break;
	}
}

void  LAButtonCommandSendCallback(GtkWidget *widget, LACommandWindow *lac){
	if(lawp->connect.fd < 0){
		gtk_text_buffer_set_text(GTK_TEXT_BUFFER(lac->textBuffer), "Device not connected, aborting.", -1);
		return;
	}


	int command = gtk_combo_box_get_active(GTK_COMBO_BOX(lac->dropdown));
	g_print("Command: %i\n", command);

	uint32_t value = (uint32_t) gtk_spin_button_get_value(GTK_SPIN_BUTTON(lac->spinButton));

	g_print("Value: %i\n", value);
	LASendBasicSerial(lawp, command, value);
	LASerialCommandLoopback(lawp, command, value);
	gtk_text_buffer_set_text(GTK_TEXT_BUFFER(lac->textBuffer), "Command sent.", -1);
}

void LATerminateConnectWindow(GtkWidget *widget, LAConnectWindow *lac){
	gtk_window_close(GTK_WINDOW(lac->window));
	gtk_main_quit();
}

int LACreateConnectWindow(GtkWidget *widget, LAWindow *law){
	LAConnectWindow laConnectWindow;
	LAConnectWindow *lac = &laConnectWindow;

	lac->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(lac->window), "Connect");
	gtk_container_set_border_width(GTK_CONTAINER(lac->window), 8);

	lac->vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
	gtk_container_add(GTK_CONTAINER(lac->window), GTK_WIDGET(lac->vbox));

	lac->label			= gtk_label_new("Device:");
	lac->response 		= gtk_label_new("Response:");
	lac->dropdown 		= gtk_combo_box_text_new();
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(lac->dropdown), "/dev/ttyS0");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(lac->dropdown), "/dev/ttyUSB0");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(lac->dropdown), "/dev/ttyUSB1");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(lac->dropdown), "/dev/ttyUSB2");
	gtk_combo_box_set_active(GTK_COMBO_BOX(lac->dropdown), 1);

	lac->connect 		= gtk_button_new_with_label("Connect");
	lac->cancel 		= gtk_button_new_with_label("Cancel");
	lac->status			= gtk_text_view_new();
	lac->textBuffer		= gtk_text_view_get_buffer(GTK_TEXT_VIEW(lac->status));
	gtk_widget_set_sensitive(lac->status, FALSE);
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(lac->status), GTK_WRAP_WORD);
	gtk_widget_set_size_request(lac->status, 450, 100);

	gtk_container_add(GTK_CONTAINER(lac->vbox), GTK_WIDGET(lac->label));
	gtk_container_add(GTK_CONTAINER(lac->vbox), GTK_WIDGET(lac->dropdown));
	gtk_container_add(GTK_CONTAINER(lac->vbox), GTK_WIDGET(lac->response));
	gtk_container_add(GTK_CONTAINER(lac->vbox), GTK_WIDGET(lac->status));

	lac->hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
	gtk_container_add(GTK_CONTAINER(lac->vbox), GTK_WIDGET(lac->hbox));

	gtk_container_add(GTK_CONTAINER(lac->hbox), GTK_WIDGET(lac->connect));
	gtk_container_add(GTK_CONTAINER(lac->hbox), GTK_WIDGET(lac->cancel));

	g_signal_connect(lac->connect, "clicked", G_CALLBACK(LAButtonConnectCallback), lac);
	g_signal_connect(lac->window, "destroy", G_CALLBACK(LATerminateConnectWindow), lac);
	g_signal_connect(lac->cancel, "clicked", G_CALLBACK(LACloseWindow), lac->window);

	gtk_widget_show_all(lac->window);
	gtk_main();

	return TRUE;
}

void LATerminateCommandWindow(GtkWidget *widget, LACommandWindow *lac){
	gtk_window_close(GTK_WINDOW(lac->window));
	gtk_main_quit();
}

int LACreateCommandWindow(GtkWidget *widget, gpointer *commandp){
	uint8_t command = CONVERT_GPOINTER_TO_INT(commandp);

	LACommandWindow laCommandWindow;
	LACommandWindow *lac = &laCommandWindow;

	lac->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(lac->window), "Send Command");
	gtk_container_set_border_width(GTK_CONTAINER(lac->window), 8);

	lac->vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
	gtk_container_add(GTK_CONTAINER(lac->window), GTK_WIDGET(lac->vbox));

	lac->label			= gtk_label_new("Command");
	lac->value			= gtk_label_new("Value:");
	lac->response 		= gtk_label_new("Response:");

	lac->dropdown 		= gtk_combo_box_text_new();
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(lac->dropdown), "Select command");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(lac->dropdown), "Set polling time (μs)");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(lac->dropdown), "Set read enable");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(lac->dropdown), "Set buffer size (samples)");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(lac->dropdown), "Set test time (μs)");
	gtk_combo_box_set_active(GTK_COMBO_BOX(lac->dropdown), command);

	GtkAdjustment *adjustValue  = gtk_adjustment_new(1, 0,  2147483647, 1, 100, 0);
	lac->spinButton				= gtk_spin_button_new(adjustValue, 1, 0);

	lac->status			= gtk_text_view_new();
	lac->textBuffer		= gtk_text_view_get_buffer(GTK_TEXT_VIEW(lac->status));
	gtk_widget_set_sensitive(lac->status, FALSE);
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(lac->status), GTK_WRAP_WORD);
	gtk_widget_set_size_request(lac->status, 450, 100);

	gtk_container_add(GTK_CONTAINER(lac->vbox), GTK_WIDGET(lac->label));
	gtk_container_add(GTK_CONTAINER(lac->vbox), GTK_WIDGET(lac->dropdown));
	gtk_container_add(GTK_CONTAINER(lac->vbox), GTK_WIDGET(lac->value));
	gtk_container_add(GTK_CONTAINER(lac->vbox), GTK_WIDGET(lac->spinButton));
	gtk_container_add(GTK_CONTAINER(lac->vbox), GTK_WIDGET(lac->response));
	gtk_container_add(GTK_CONTAINER(lac->vbox), GTK_WIDGET(lac->status));

	lac->send 			= gtk_button_new_with_label("Send");
	lac->cancel 		= gtk_button_new_with_label("Cancel");
	lac->hbox 			= gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
	gtk_container_add(GTK_CONTAINER(lac->vbox), GTK_WIDGET(lac->hbox));
	gtk_container_add(GTK_CONTAINER(lac->hbox), GTK_WIDGET(lac->send));
	gtk_container_add(GTK_CONTAINER(lac->hbox), GTK_WIDGET(lac->cancel));

	g_signal_connect(lac->window, "destroy", G_CALLBACK(LATerminateCommandWindow), lac);
	g_signal_connect(lac->send, "clicked", G_CALLBACK(LAButtonCommandSendCallback), lac);
	g_signal_connect(lac->cancel, "clicked", G_CALLBACK(LACloseWindow), lac->window);

	gtk_widget_show_all(lac->window);
	gtk_main();

	return TRUE;
}
