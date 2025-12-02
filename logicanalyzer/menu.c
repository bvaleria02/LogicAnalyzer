#include <gtk/gtk.h>
#include <cairo.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "liblogicanalyzer.h"
#include <time.h>
#include <pthread.h>

void LAZoomLess(GtkWidget *widget, LAWindow *law){
	/*
	if(law->rd.zoom > 0){
		law->rd.zoom -= 1;
	}*/
	law->rd.zoom -= 1;
	LARedrawAllScopes(law);
}

void LAZoomMore(GtkWidget *widget, LAWindow *law){
	law->rd.zoom += 1;
	LARedrawAllScopes(law);
}

void LAZoomReset(GtkWidget *widget, LAWindow *law){
	law->rd.zoom = 0;
	LARedrawAllScopes(law);
}

void LAZoomSet(GtkWidget *widget, LAWindow *law){
	LAZoomSetWindow laz;
	LACreateZoomSetWindow(law, &laz);
	LARedrawAllScopes(law);
}

void LAZoomClk(GtkWidget *widget, LAWindow *law){
	/*
	law->rd.zoom = 0;
	int8_t channel = LAGetClockChannel(law);
	g_print("Channel: %i\n", channel);
*/
	LAZoomClockSyncWindow lazf;
	LAZoomClockSyncWindow *laz = &lazf;

	LACreateZoomClockSyncWindow(law, &lazf);
	
	LARedrawAllScopes(law);
}
void LAMicroRewind(GtkWidget *widget, LAWindow *law){
	law->rd.scopeOffset -= (LA_SMALL_INCREMENT_SCOPE / LAGetZoomMultiplier(law));
	LARedrawAllScopes(law);
}

void LAMicroAdvance(GtkWidget *widget, LAWindow *law){
	law->rd.scopeOffset += (LA_SMALL_INCREMENT_SCOPE / LAGetZoomMultiplier(law));
	LARedrawAllScopes(law);
}


void LARewind(GtkWidget *widget, LAWindow *law){
	law->rd.scopeOffset -= (LA_BUFFER_SIZE / LAGetZoomMultiplier(law));
	LARedrawAllScopes(law);
}

void LAAdvance(GtkWidget *widget, LAWindow *law){
	law->rd.scopeOffset += (LA_BUFFER_SIZE / LAGetZoomMultiplier(law));
	LARedrawAllScopes(law);
}

void LAGoToStart(GtkWidget *widget, LAWindow *law){
	law->rd.scopeOffset = LA_BUFFER_SIZE;
	LARedrawAllScopes(law);
}

void LAGoToEnd(GtkWidget *widget, LAWindow *law){
	law->rd.scopeOffset = 0;
	LARedrawAllScopes(law);
}

void LAGoToUserDefined(GtkWidget *widget, LAWindow *law){
	law->rd.scopeOffset = 0;
	LARedrawAllScopes(law);
}

void LAPauseUpdate(GtkWidget *widget, LAWindow *law){
	law->rd.dontWrite = !(law->rd.dontWrite);	
	LARedrawAllScopes(law);
}

void LAPauseScroll(GtkWidget *widget, LAWindow *law){
	law->rd.addDataOffset = !(law->rd.addDataOffset);	
	LARedrawAllScopes(law);
}

void LAToggleTimeRelative(GtkWidget *widget, LAWindow *law){
	law->rd.showRelativeTime = !(law->rd.showRelativeTime);	
	LARedrawAllScopes(law);
}

void LAToggleTimeAbsolute(GtkWidget *widget, LAWindow *law){
	law->rd.showAbsoluteTime = !(law->rd.showAbsoluteTime);	
	timespec_get(&(law->rd.startTime), TIME_UTC);
	LARedrawAllScopes(law);
}
void LAToggleSampleRelative(GtkWidget *widget, LAWindow *law){
	law->rd.showRelativeSample = !(law->rd.showRelativeSample);	
	LARedrawAllScopes(law);
}

void LAToggleSampleAbsolute(GtkWidget *widget, LAWindow *law){
	law->rd.showAbsoluteSample = !(law->rd.showAbsoluteSample);	
	LARedrawAllScopes(law);
}

void LAToggleBufferEnd(GtkWidget *widget, LAWindow *law){
	law->rd.showBufferEnd = !(law->rd.showBufferEnd);	
	LARedrawAllScopes(law);
}

void LAToggleRulerBuffer(GtkWidget *widget, LAWindow *law){
	law->rd.showBufferRuler = !(law->rd.showBufferRuler);	
	LARedrawAllScopes(law);
}

void LAToggleRulerClock(GtkWidget *widget, LAWindow *law){
	law->rd.showClockRuler = !(law->rd.showClockRuler);	
	LARedrawAllScopes(law);
}

void LAMuteAllMenu(GtkWidget *widget, LAWindow *law){
	LAMuteAllChannels(law);
	LARedrawAllScopes(law);
}

void LACircularBufferView(GtkWidget *widget, LAWindow *law){
	LACreateHexView(
		law,
		law,
		LA_LARGE_BUFFER_SIZE,
		LACallbackCircularBufferView,
		&(law->mutexes.dataBufferAccess)
	);
}

void LAMenuConfigView(GtkWidget *widget, LAWindow *law){
	LAConfigHeader lac;
	LACreateConfigHeader(law, &lac);
	LACreateHexView(
		law,
		&lac,
		sizeof(LAConfigHeader),
		LACallbackNormalBuffer,
		NULL
	);
}

void LAMenuRecordUpdateConfig(LAWindow *law, LARecordDataMode mode){
	switch(mode){
		case LA_RECORD_DATA_MODE_ALL :	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(law->menu.menuRecordAllCh), 		TRUE);
										gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(law->menu.menuRecordVisibleCh), 	FALSE);
										gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(law->menu.menuRecordCustomCh), 	FALSE);
										break;
		case LA_RECORD_DATA_MODE_VISIBLE :	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(law->menu.menuRecordAllCh), 	FALSE);
										gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(law->menu.menuRecordVisibleCh), 	TRUE);
										gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(law->menu.menuRecordCustomCh), 	FALSE);
										break;
		case LA_RECORD_DATA_MODE_CUSTOM :	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(law->menu.menuRecordAllCh), 	FALSE);
										gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(law->menu.menuRecordVisibleCh), 	FALSE);
										gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(law->menu.menuRecordCustomCh), 	TRUE);
										break;
	}

	if(mode == LA_RECORD_DATA_MODE_CUSTOM){
		gtk_widget_set_sensitive(law->menu.menuRecordCustomCh0, TRUE);
		gtk_widget_set_sensitive(law->menu.menuRecordCustomCh1, TRUE);
		gtk_widget_set_sensitive(law->menu.menuRecordCustomCh2, TRUE);
		gtk_widget_set_sensitive(law->menu.menuRecordCustomCh3, TRUE);
		gtk_widget_set_sensitive(law->menu.menuRecordCustomCh4, TRUE);
		gtk_widget_set_sensitive(law->menu.menuRecordCustomCh5, TRUE);
		gtk_widget_set_sensitive(law->menu.menuRecordCustomCh6, TRUE);
		gtk_widget_set_sensitive(law->menu.menuRecordCustomCh7, TRUE);
	} else {

		gtk_widget_set_sensitive(law->menu.menuRecordCustomCh0, FALSE);
		gtk_widget_set_sensitive(law->menu.menuRecordCustomCh1, FALSE);
		gtk_widget_set_sensitive(law->menu.menuRecordCustomCh2, FALSE);
		gtk_widget_set_sensitive(law->menu.menuRecordCustomCh3, FALSE);
		gtk_widget_set_sensitive(law->menu.menuRecordCustomCh4, FALSE);
		gtk_widget_set_sensitive(law->menu.menuRecordCustomCh5, FALSE);
		gtk_widget_set_sensitive(law->menu.menuRecordCustomCh6, FALSE);
		gtk_widget_set_sensitive(law->menu.menuRecordCustomCh7, FALSE);
	}
}

void LAMenuRecordUpdateDataMask(LAWindow *law, LARecordDataMode mode){
	uint8_t mask = 0;
	switch(mode){
		case LA_RECORD_DATA_MODE_ALL:		mask = LA_ALL_CHANNELS_MASK;
											break;
		case LA_RECORD_DATA_MODE_VISIBLE:	mask |= ((!(law->channel[0].muted)) << 0);
											mask |= ((!(law->channel[1].muted)) << 1);
											mask |= ((!(law->channel[2].muted)) << 2);
											mask |= ((!(law->channel[3].muted)) << 3);
											mask |= ((!(law->channel[4].muted)) << 4);
											mask |= ((!(law->channel[5].muted)) << 5);
											mask |= ((!(law->channel[6].muted)) << 6);
											mask |= ((!(law->channel[7].muted)) << 7);
											break;
		case LA_RECORD_DATA_MODE_CUSTOM:	mask |= (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(law->menu.menuRecordCustomCh0)) << 0);
											mask |= (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(law->menu.menuRecordCustomCh1)) << 1);
											mask |= (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(law->menu.menuRecordCustomCh2)) << 2);
											mask |= (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(law->menu.menuRecordCustomCh3)) << 3);
											mask |= (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(law->menu.menuRecordCustomCh4)) << 4);
											mask |= (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(law->menu.menuRecordCustomCh5)) << 5);
											mask |= (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(law->menu.menuRecordCustomCh6)) << 6);
											mask |= (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(law->menu.menuRecordCustomCh7)) << 7);
											break;
	}

	//g_print("Mask: %i\n", mask);
	law->bd.dataMask = mask;
}

gboolean LARecordAllChannels(GtkWidget *widget, LAWindow *law){
	if(!gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(law->menu.menuRecordAllCh))){
		return TRUE;
	}

	LAMenuRecordUpdateConfig(law, LA_RECORD_DATA_MODE_ALL);
	LAMenuRecordUpdateDataMask(law, LA_RECORD_DATA_MODE_ALL);
	law->bd.dataMode = LA_RECORD_DATA_MODE_ALL;
	return TRUE;
}

gboolean LARecordVisibleChannels(GtkWidget *widget, LAWindow *law){
	if(!gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(law->menu.menuRecordVisibleCh))){
		return TRUE;
	}

	LAMenuRecordUpdateConfig(law, LA_RECORD_DATA_MODE_VISIBLE);
	LAMenuRecordUpdateDataMask(law, LA_RECORD_DATA_MODE_VISIBLE);
	law->bd.dataMode = LA_RECORD_DATA_MODE_VISIBLE;
	return TRUE;
}

gboolean LARecordCustomChannels(GtkWidget *widget, LAWindow *law){
	if(!gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(law->menu.menuRecordCustomCh))){
		return TRUE;
	}

	LAMenuRecordUpdateConfig(law, LA_RECORD_DATA_MODE_CUSTOM);
	LAMenuRecordUpdateDataMask(law, LA_RECORD_DATA_MODE_CUSTOM);
	law->bd.dataMode = LA_RECORD_DATA_MODE_CUSTOM;
	return TRUE;
}

gboolean LARecordCustomSingleChannel(GtkWidget *widget, LAWindow *law){
	if(law->bd.dataMode != LA_RECORD_DATA_MODE_CUSTOM){
		return TRUE;
	}

	LAMenuRecordUpdateDataMask(law, LA_RECORD_DATA_MODE_CUSTOM);
	return TRUE;
}

void LAMenuCustomFileView(GtkWidget *widget, LAWindow *law){
	gchar *filename = LADialogOpenFile(law, "Open file", "All files");
	if(filename == NULL){
		g_print("No file\n");
		return;
	}

	FILE *fp = fopen(filename, "rb");
	if(fp == NULL){
		g_print("fopen error\n");
		if(filename != NULL){
			g_free(filename);
		}
		return;
	}

	fseek(fp, 0l, SEEK_END);
	size_t length = ftell(fp);
	fseek(fp, 0l, SEEK_SET);

	LACreateHexView(
		law,
		fp,
		length,
		LACallbackFileView,
		NULL
	);

	fclose(fp);
}

void LAWindowCreateMenu(LAWindow *law){
	LAMenu *lam = &(law->menu);

	lam->menubar		= gtk_menu_bar_new();
	
	lam->menuFile		= gtk_menu_item_new_with_label("File");
	lam->listFile		= gtk_menu_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(lam->menubar), lam->menuFile);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(lam->menuFile), lam->listFile);

	lam->menuView	    = gtk_menu_item_new_with_label("View");
	lam->listView		= gtk_menu_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(lam->menubar), lam->menuView);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(lam->menuView), lam->listView);

	lam->menuDevice	    = gtk_menu_item_new_with_label("Device");
	lam->listDevice		= gtk_menu_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(lam->menubar), lam->menuDevice);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(lam->menuDevice), lam->listDevice);

	lam->menuRecord	    = gtk_menu_item_new_with_label("Record");
	lam->listRecord		= gtk_menu_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(lam->menubar), lam->menuRecord);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(lam->menuRecord), lam->listRecord);

	lam->menuTools	    = gtk_menu_item_new_with_label("Tools");
	lam->listTools		= gtk_menu_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(lam->menubar), lam->menuTools);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(lam->menuTools), lam->listTools);

	lam->menuAdv		= gtk_menu_item_new_with_label("Advanced (advanced)");
	lam->listAdv		= gtk_menu_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(lam->menubar), lam->menuAdv);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(lam->menuAdv), lam->listAdv);

	lam->menuFileNew	= gtk_menu_item_new_with_label("New");
	lam->menuFileLoad	= gtk_menu_item_new_with_label("Load");
	lam->menuFileSave	= gtk_menu_item_new_with_label("Save");
	lam->menuFileExit	= gtk_menu_item_new_with_label("Quit");

	lam->menuViewZoomLess	= gtk_menu_item_new_with_label("Zoom -");
	lam->menuViewZoomMore	= gtk_menu_item_new_with_label("Zoom +");
	lam->menuViewZoomReset	= gtk_menu_item_new_with_label("Reset Zoom");
	lam->menuViewZoomSet	= gtk_menu_item_new_with_label("Set Zoom");
	lam->menuViewZoomClk    = gtk_menu_item_new_with_label("Zoom clock sync");
	lam->menuViewSep1		= gtk_separator_menu_item_new();
	lam->menuViewRewind		= gtk_menu_item_new_with_label("Rewind");
	lam->menuViewAdvance	= gtk_menu_item_new_with_label("Advance");
	lam->menuViewGotoStart  = gtk_menu_item_new_with_label("Go to start");
	lam->menuViewGotoEnd	= gtk_menu_item_new_with_label("Go to end");
	lam->menuViewGoto    	= gtk_menu_item_new_with_label("Go to...");
	lam->menuViewSep2		= gtk_separator_menu_item_new();
	lam->menuViewPauseUpdate= gtk_menu_item_new_with_label("Pause/Continue update");
	lam->menuViewPauseScroll= gtk_menu_item_new_with_label("Pause/Continue scroll");
	lam->menuViewSep3		= gtk_separator_menu_item_new();
	lam->menuViewTimeRelative  	= gtk_menu_item_new_with_label("Show relative time");
	lam->menuViewTimeAbsolute  	= gtk_menu_item_new_with_label("Show absolute time");
	lam->menuViewSampleRelative = gtk_menu_item_new_with_label("Show relative samples");
	lam->menuViewSampleAbsolute = gtk_menu_item_new_with_label("Show absolute samples");
	lam->menuViewBufferEnd 		= gtk_menu_item_new_with_label("Show buffer end");
	lam->menuViewRulerBuffer	= gtk_menu_item_new_with_label("Show buffer ruler");
	lam->menuViewRulerClock		= gtk_menu_item_new_with_label("Show clock ruler");
	lam->menuViewSep4			= gtk_separator_menu_item_new();
	lam->menuViewMuteAll		= gtk_menu_item_new_with_label("Mute all channels");

	lam->menuDeviceConnect		= gtk_menu_item_new_with_label("Connect");
	lam->menuDeviceSep1			= gtk_separator_menu_item_new();
	lam->menuDeviceSetPolling	= gtk_menu_item_new_with_label("Set polling time");
	lam->menuDeviceSetRead		= gtk_menu_item_new_with_label("Enable read");
	lam->menuDeviceSetLength	= gtk_menu_item_new_with_label("Set virtual buffer size");
	lam->menuDeviceSendCustom	= gtk_menu_item_new_with_label("Send custom command");

	lam->menuRecordStart		= gtk_menu_item_new_with_label("Start");
	lam->menuRecordStop			= gtk_menu_item_new_with_label("Stop");
	lam->menuRecordPause		= gtk_menu_item_new_with_label("Pause");
	lam->menuRecordDelete		= gtk_menu_item_new_with_label("Delete");
	lam->menuRecordView			= gtk_menu_item_new_with_label("View");
	lam->menuRecordSep1			= gtk_separator_menu_item_new();
	lam->menuRecordAllCh		= gtk_check_menu_item_new_with_label("All channels");
	lam->menuRecordVisibleCh	= gtk_check_menu_item_new_with_label("Visible channels");
	lam->menuRecordCustomCh		= gtk_check_menu_item_new_with_label("Custom channels");
	lam->menuRecordCustomCh0	= gtk_check_menu_item_new_with_label("Channel 0");
	lam->menuRecordCustomCh1	= gtk_check_menu_item_new_with_label("Channel 1");
	lam->menuRecordCustomCh2	= gtk_check_menu_item_new_with_label("Channel 2");
	lam->menuRecordCustomCh3	= gtk_check_menu_item_new_with_label("Channel 3");
	lam->menuRecordCustomCh4	= gtk_check_menu_item_new_with_label("Channel 4");
	lam->menuRecordCustomCh5	= gtk_check_menu_item_new_with_label("Channel 5");
	lam->menuRecordCustomCh6	= gtk_check_menu_item_new_with_label("Channel 6");
	lam->menuRecordCustomCh7	= gtk_check_menu_item_new_with_label("Channel 7");
	lam->menuRecordSep2			= gtk_separator_menu_item_new();
	lam->menuRecordLoad			= gtk_menu_item_new_with_label("Load");
	lam->menuRecordSaveCSV		= gtk_menu_item_new_with_label("Save as CSV");
	lam->menuRecordSaveBin		= gtk_menu_item_new_with_label("Save as binary");

	lam->menuToolsBufferHex		= gtk_menu_item_new_with_label("Circular Buffer HexView");
	lam->menuToolsConfigHex		= gtk_menu_item_new_with_label("Config HexView");
	lam->menuToolsFileHex		= gtk_menu_item_new_with_label("Open file HexView");

	lam->menuAdvSibelius	= gtk_menu_item_new_with_label("Quit Sibelius");
	lam->menuAdvMusescore	= gtk_menu_item_new_with_label("New All");
	lam->menuAdvAdvanced1	= gtk_menu_item_new_with_label("No, what are you doing?");
	lam->menuAdvAdvanced2	= gtk_menu_item_new_with_label("This is not advanced!");
	lam->menuAdvAdvanced3	= gtk_menu_item_new_with_label("I am the god of advancement, I won't let you do that");
	lam->menuAdvAdvanced4	= gtk_menu_item_new_with_label("THAT WILL NOT BE ADVANCED, THAT WILL NOT BE ADVANCED!");
	lam->menuAdvAdvanced5	= gtk_menu_item_new_with_label("T̶̛̼̥̯̹͐̐̐̈̿͆̇̇͜Ḩ̵͓͎̫̺̺̮̰̜̩̩͇̱͙͂͒͗Á̴̢͔̣̞̟̼̜͉̪̿̉͊͒͂̓͌̑̇͠͝͝Ṭ̶̩͍͈̜̖̇̆̄͌̃̏̀̊͋̕ ̵̡̧̤̘̰̱̋̏̾͐̈̒̈́W̸̧̛̲̥̱͎̼̖̳̿͜I̶̡̞̙̩̩̦̘͙̲͕̥͉̺͍͈̐͑̂̐͜͜Ĺ̵̡̼̊͝L̶̖̰̹͇̟̰̞̭̈́̀̉̓̀̿̊͆́͌͐͑͐͝͝͝ ̴̨̡̝̺͎͖̬̬̺̩̺͉͕̰͐͋̋ͅN̴̨̛̯̥̲͈̰̯̹͖̤̹̱͕̠̽̿̊̄͐͒̇̾̍͂̓͑̀͑̕͝Ỏ̶̡̠̰̻̜͜͜͜Ţ̴̡͙̯̭͙̖̼̖̇̇̽́̓̍͜ ̴̧̢̞̝͓͓͓͚̗̰͖̘̟͙̥͙̅̓̽̀͂̔̑̕̕͜Ḇ̸̡̨͚̖̲̺̝̆̋͑̀̄Ę̴̢̭̜̰̹̪̪̝͚̣̺̃͆̽̿̏̔͑͝͝͝͝ ̷̡̮̬̱̭̹̠̪̗̗̼̰̦̤̇̾͜ͅͅA̸̦̱̙̽͑D̸̙̬̋̒͋͌͆̎̚V̴̭̳̰̫̺̩͇͖̙̲̐̅̄̌̀̅̾̐̾͛̕̕͘͠Ả̶̡͎̓͑͆̄̈́͐̈́͂̚͠N̷̡̢̨͔͔̺͍̫̙̙̬̔͂͂͑͜C̴͖̱̞͙͖͉͎̝̭̘̜͍͕̥̘̰͌̉̂̈̊̕Ȩ̷̭͈̣̒̒̐̓͐̆̽̓͊̒̌̈̂̒͂͂̕ͅD̷̢̨̛̥̳͎̙̩̉̄");

	gtk_menu_shell_append(GTK_MENU_SHELL(lam->listFile), lam->menuFileNew);
	gtk_menu_shell_append(GTK_MENU_SHELL(lam->listFile), lam->menuFileLoad);
	gtk_menu_shell_append(GTK_MENU_SHELL(lam->listFile), lam->menuFileSave);
	gtk_menu_shell_append(GTK_MENU_SHELL(lam->listFile), lam->menuFileExit);

	gtk_menu_shell_append(GTK_MENU_SHELL(lam->listView), lam->menuViewZoomLess);
	gtk_menu_shell_append(GTK_MENU_SHELL(lam->listView), lam->menuViewZoomMore);
	gtk_menu_shell_append(GTK_MENU_SHELL(lam->listView), lam->menuViewZoomReset);
	gtk_menu_shell_append(GTK_MENU_SHELL(lam->listView), lam->menuViewZoomSet);
	gtk_menu_shell_append(GTK_MENU_SHELL(lam->listView), lam->menuViewZoomClk);
	gtk_menu_shell_append(GTK_MENU_SHELL(lam->listView), lam->menuViewSep1);
	gtk_menu_shell_append(GTK_MENU_SHELL(lam->listView), lam->menuViewRewind);
	gtk_menu_shell_append(GTK_MENU_SHELL(lam->listView), lam->menuViewAdvance);
	gtk_menu_shell_append(GTK_MENU_SHELL(lam->listView), lam->menuViewGotoStart);
	gtk_menu_shell_append(GTK_MENU_SHELL(lam->listView), lam->menuViewGotoEnd);
	gtk_menu_shell_append(GTK_MENU_SHELL(lam->listView), lam->menuViewGoto);
	gtk_menu_shell_append(GTK_MENU_SHELL(lam->listView), lam->menuViewSep2);
	gtk_menu_shell_append(GTK_MENU_SHELL(lam->listView), lam->menuViewPauseUpdate);
	gtk_menu_shell_append(GTK_MENU_SHELL(lam->listView), lam->menuViewPauseScroll);
	gtk_menu_shell_append(GTK_MENU_SHELL(lam->listView), lam->menuViewSep3);
	gtk_menu_shell_append(GTK_MENU_SHELL(lam->listView), lam->menuViewTimeRelative);
	gtk_menu_shell_append(GTK_MENU_SHELL(lam->listView), lam->menuViewTimeAbsolute);
	gtk_menu_shell_append(GTK_MENU_SHELL(lam->listView), lam->menuViewSampleRelative);
	gtk_menu_shell_append(GTK_MENU_SHELL(lam->listView), lam->menuViewSampleAbsolute);
	gtk_menu_shell_append(GTK_MENU_SHELL(lam->listView), lam->menuViewBufferEnd);
	gtk_menu_shell_append(GTK_MENU_SHELL(lam->listView), lam->menuViewRulerBuffer);
	gtk_menu_shell_append(GTK_MENU_SHELL(lam->listView), lam->menuViewRulerClock);
	gtk_menu_shell_append(GTK_MENU_SHELL(lam->listView), lam->menuViewSep4);
	gtk_menu_shell_append(GTK_MENU_SHELL(lam->listView), lam->menuViewMuteAll);

	gtk_menu_shell_append(GTK_MENU_SHELL(lam->listDevice), lam->menuDeviceConnect);
	gtk_menu_shell_append(GTK_MENU_SHELL(lam->listDevice), lam->menuDeviceSep1);
	gtk_menu_shell_append(GTK_MENU_SHELL(lam->listDevice), lam->menuDeviceSetPolling);
	gtk_menu_shell_append(GTK_MENU_SHELL(lam->listDevice), lam->menuDeviceSetRead);
	gtk_menu_shell_append(GTK_MENU_SHELL(lam->listDevice), lam->menuDeviceSetLength);
	gtk_menu_shell_append(GTK_MENU_SHELL(lam->listDevice), lam->menuDeviceSendCustom);

	gtk_menu_shell_append(GTK_MENU_SHELL(lam->listRecord), lam->menuRecordStart);
	gtk_menu_shell_append(GTK_MENU_SHELL(lam->listRecord), lam->menuRecordStop);
	gtk_menu_shell_append(GTK_MENU_SHELL(lam->listRecord), lam->menuRecordPause);
	gtk_menu_shell_append(GTK_MENU_SHELL(lam->listRecord), lam->menuRecordDelete);
	gtk_menu_shell_append(GTK_MENU_SHELL(lam->listRecord), lam->menuRecordView);
	gtk_menu_shell_append(GTK_MENU_SHELL(lam->listRecord), lam->menuRecordSep1);
	gtk_menu_shell_append(GTK_MENU_SHELL(lam->listRecord), lam->menuRecordAllCh);
	gtk_menu_shell_append(GTK_MENU_SHELL(lam->listRecord), lam->menuRecordVisibleCh);
	gtk_menu_shell_append(GTK_MENU_SHELL(lam->listRecord), lam->menuRecordCustomCh);
	gtk_menu_shell_append(GTK_MENU_SHELL(lam->listRecord), lam->menuRecordCustomCh0);
	gtk_menu_shell_append(GTK_MENU_SHELL(lam->listRecord), lam->menuRecordCustomCh1);
	gtk_menu_shell_append(GTK_MENU_SHELL(lam->listRecord), lam->menuRecordCustomCh2);
	gtk_menu_shell_append(GTK_MENU_SHELL(lam->listRecord), lam->menuRecordCustomCh3);
	gtk_menu_shell_append(GTK_MENU_SHELL(lam->listRecord), lam->menuRecordCustomCh4);
	gtk_menu_shell_append(GTK_MENU_SHELL(lam->listRecord), lam->menuRecordCustomCh5);
	gtk_menu_shell_append(GTK_MENU_SHELL(lam->listRecord), lam->menuRecordCustomCh6);
	gtk_menu_shell_append(GTK_MENU_SHELL(lam->listRecord), lam->menuRecordCustomCh7);
	gtk_menu_shell_append(GTK_MENU_SHELL(lam->listRecord), lam->menuRecordSep2);
	gtk_menu_shell_append(GTK_MENU_SHELL(lam->listRecord), lam->menuRecordLoad);
	gtk_menu_shell_append(GTK_MENU_SHELL(lam->listRecord), lam->menuRecordSaveCSV);
	gtk_menu_shell_append(GTK_MENU_SHELL(lam->listRecord), lam->menuRecordSaveBin);

	gtk_menu_shell_append(GTK_MENU_SHELL(lam->listTools), lam->menuToolsBufferHex);
	gtk_menu_shell_append(GTK_MENU_SHELL(lam->listTools), lam->menuToolsConfigHex);
	gtk_menu_shell_append(GTK_MENU_SHELL(lam->listTools), lam->menuToolsFileHex);

	gtk_menu_shell_append(GTK_MENU_SHELL(lam->listAdv), lam->menuAdvSibelius);
	gtk_menu_shell_append(GTK_MENU_SHELL(lam->listAdv), lam->menuAdvMusescore);
	gtk_menu_shell_append(GTK_MENU_SHELL(lam->listAdv), lam->menuAdvAdvanced1);
	gtk_menu_shell_append(GTK_MENU_SHELL(lam->listAdv), lam->menuAdvAdvanced2);
	gtk_menu_shell_append(GTK_MENU_SHELL(lam->listAdv), lam->menuAdvAdvanced3);
	gtk_menu_shell_append(GTK_MENU_SHELL(lam->listAdv), lam->menuAdvAdvanced4);
	gtk_menu_shell_append(GTK_MENU_SHELL(lam->listAdv), lam->menuAdvAdvanced5);

	// File is under construction	
	g_signal_connect(lam->menuFileSave,      "activate", G_CALLBACK(LASaveSession), law);
	g_signal_connect(lam->menuFileExit,      "activate", G_CALLBACK(destroyWindow), law);

	g_signal_connect(lam->menuViewZoomLess,  "activate", G_CALLBACK(LAZoomLess), law);
	g_signal_connect(lam->menuViewZoomMore,  "activate", G_CALLBACK(LAZoomMore), law);
	g_signal_connect(lam->menuViewZoomReset, "activate", G_CALLBACK(LAZoomReset), law);
	g_signal_connect(lam->menuViewZoomSet,   "activate", G_CALLBACK(LAZoomSet), law);
	g_signal_connect(lam->menuViewZoomClk,   "activate", G_CALLBACK(LAZoomClk), law);

	g_signal_connect(lam->menuViewRewind,     "activate", G_CALLBACK(LARewind), law);
	g_signal_connect(lam->menuViewAdvance,    "activate", G_CALLBACK(LAAdvance), law);
	g_signal_connect(lam->menuViewGotoStart,  "activate", G_CALLBACK(LAGoToStart), law);
	g_signal_connect(lam->menuViewGotoEnd,    "activate", G_CALLBACK(LAGoToEnd), law);
	g_signal_connect(lam->menuViewGoto,       "activate", G_CALLBACK(LAGoToUserDefined), law);

	g_signal_connect(lam->menuViewPauseUpdate,"activate", G_CALLBACK(LAPauseUpdate), law);
	g_signal_connect(lam->menuViewPauseScroll,"activate", G_CALLBACK(LAPauseScroll), law);

	g_signal_connect(lam->menuViewTimeRelative,	  "activate", G_CALLBACK(LAToggleTimeRelative), law);
	g_signal_connect(lam->menuViewTimeAbsolute,	  "activate", G_CALLBACK(LAToggleTimeAbsolute), law);
	g_signal_connect(lam->menuViewSampleRelative, "activate", G_CALLBACK(LAToggleSampleRelative), law);
	g_signal_connect(lam->menuViewSampleAbsolute, "activate", G_CALLBACK(LAToggleSampleAbsolute), law);
	g_signal_connect(lam->menuViewBufferEnd, 	  "activate", G_CALLBACK(LAToggleBufferEnd), law);
	g_signal_connect(lam->menuViewRulerBuffer, 	  "activate", G_CALLBACK(LAToggleRulerBuffer), law);
	g_signal_connect(lam->menuViewRulerClock, 	  "activate", G_CALLBACK(LAToggleRulerClock), law);
	g_signal_connect(lam->menuViewMuteAll,	  "activate", G_CALLBACK(LAMuteAllMenu), law);

	g_signal_connect(lam->menuDeviceConnect,	 	"activate", G_CALLBACK(LACreateConnectWindow), law);
	g_signal_connect(lam->menuDeviceSetPolling,	 	"activate", G_CALLBACK(LACreateCommandWindow), CONVERT_INT_TO_GPOINTER(LA_COMMAND_POLLING_RATE));
	g_signal_connect(lam->menuDeviceSetRead,	 	"activate", G_CALLBACK(LACreateCommandWindow), CONVERT_INT_TO_GPOINTER(LA_COMMAND_READ_ENABLE));
	g_signal_connect(lam->menuDeviceSetLength,	 	"activate", G_CALLBACK(LACreateCommandWindow), CONVERT_INT_TO_GPOINTER(LA_COMMAND_BUFFER_SIZE));
	g_signal_connect(lam->menuDeviceSendCustom,	 	"activate", G_CALLBACK(LACreateCommandWindow), CONVERT_INT_TO_GPOINTER(LA_COMMAND_NOP));

	g_signal_connect(lam->menuRecordStart,	 	"activate", G_CALLBACK(LARecordStart), law);
	g_signal_connect(lam->menuRecordPause,	 	"activate", G_CALLBACK(LARecordPause), law);
	g_signal_connect(lam->menuRecordStop,	 	"activate", G_CALLBACK(LARecordStop), law);
	g_signal_connect(lam->menuRecordDelete,	 	"activate", G_CALLBACK(LARecordDelete), law);
	g_signal_connect(lam->menuRecordView,	 	"activate", G_CALLBACK(LARecordView2), law);
	g_signal_connect(lam->menuRecordAllCh,	 	"activate", G_CALLBACK(LARecordAllChannels), law);
	g_signal_connect(lam->menuRecordVisibleCh,	"activate", G_CALLBACK(LARecordVisibleChannels), law);
	g_signal_connect(lam->menuRecordCustomCh,	"activate", G_CALLBACK(LARecordCustomChannels), law);
	g_signal_connect(lam->menuRecordCustomCh0,	"activate", G_CALLBACK(LARecordCustomSingleChannel), law);
	g_signal_connect(lam->menuRecordCustomCh1,	"activate", G_CALLBACK(LARecordCustomSingleChannel), law);
	g_signal_connect(lam->menuRecordCustomCh2,	"activate", G_CALLBACK(LARecordCustomSingleChannel), law);
	g_signal_connect(lam->menuRecordCustomCh3,	"activate", G_CALLBACK(LARecordCustomSingleChannel), law);
	g_signal_connect(lam->menuRecordCustomCh4,	"activate", G_CALLBACK(LARecordCustomSingleChannel), law);
	g_signal_connect(lam->menuRecordCustomCh5,	"activate", G_CALLBACK(LARecordCustomSingleChannel), law);
	g_signal_connect(lam->menuRecordCustomCh6,	"activate", G_CALLBACK(LARecordCustomSingleChannel), law);
	g_signal_connect(lam->menuRecordCustomCh7,	"activate", G_CALLBACK(LARecordCustomSingleChannel), law);
	g_signal_connect(lam->menuRecordSaveBin,	"activate", G_CALLBACK(LARecordSaveBin), law);
	g_signal_connect(lam->menuRecordSaveCSV,	"activate", G_CALLBACK(LARecordSaveCSV), law);

	g_signal_connect(lam->menuToolsBufferHex, 	"activate", G_CALLBACK(LACircularBufferView), law);
	g_signal_connect(lam->menuToolsConfigHex, 	"activate", G_CALLBACK(LAMenuConfigView), law);
	g_signal_connect(lam->menuToolsFileHex, 	"activate", G_CALLBACK(LAMenuCustomFileView), law);
	// Advanced (advanced) is not ready... for now. You are aware.

	gtk_box_pack_start(GTK_BOX(law->vbox), lam->menubar, FALSE, FALSE, 0);
}
