#ifndef LA_GTK_FUNCS
#define LA_GTK_FUNCS

// window.c
void destroyWindow(GtkWidget *widget, gpointer *pointer);
void LAWindowCreate(LAWindow *law);
void LAWindowRun(LAWindow *law);
void LATerminateWindow(GtkWidget *widget, GtkWidget **window);
void LATerminateZoomSetWindow(GtkWidget *widget, LAZoomSetWindow *laz);
void LACloseWindow(GtkWidget *widget, GtkWidget *window);
double LAGetZoomMultiplier(LAWindow *law);
gboolean LAHandleMainKeyPress(GtkWidget *widget, GdkEventKey *key, LAWindow *law);

// channel.c
void LAWindowCreateChannels(LAWindow *law);
void LAWindowCreateChannel(LAChannel *channel, uint8_t index);
void LAChannelChangeBit(GtkWidget *widget, LAChannel *channel);
void LAChannelMute(GtkWidget *widget, LAChannel *channel);
void LAChannelSolo(GtkWidget *widget, uintptr_t indexp);
void LAChannelColorPicker(GtkWidget *widget, uintptr_t indexp);
void LAMuteAllChannels(LAWindow *law);
void LAEnableAllChannels(LAWindow *law);
void LARedrawAllScopes(LAWindow *law);

// scope.c
void LAChannelOnDraw(GtkWidget *widget, cairo_t *cr, uintptr_t indexp);

// menu.c
void LAWindowCreateMenu(LAWindow *law);
void LAMicroRewind(GtkWidget *widget, LAWindow *law);
void LAMicroAdvance(GtkWidget *widget, LAWindow *law);
void LARewind(GtkWidget *widget, LAWindow *law);
void LAAdvance(GtkWidget *widget, LAWindow *law);
void LAZoomLess(GtkWidget *widget, LAWindow *law);
void LAZoomMore(GtkWidget *widget, LAWindow *law);
void LAGoToStart(GtkWidget *widget, LAWindow *law);
void LAGoToEnd(GtkWidget *widget, LAWindow *law);

// connect.c
void LAWindowCreateConnect(LAWindow *law);
void LAButtonConnectCallback(GtkWidget *widget, LAConnectWindow *lac);
uint8_t LAConnectDevice(LAWindow *law, gchar *device);
void LACloseDevice(LAWindow *law);
void  LAButtonCommandSendCallback(GtkWidget *widget, LACommandWindow *lac);
void LASendBasicSerial(LAWindow *law, uint8_t command, uint32_t value);
int LACreateConnectWindow(GtkWidget *widget, LAWindow *law);
int LACreateCommandWindow(GtkWidget *widget, gpointer *commandp);
void LASendSerialV2(LAWindow *law, LASerialV2Protocol *p);

// record.c
void LARecordStart(GtkWidget *widget, LAWindow *law);
void LARecordPause(GtkWidget *widget, LAWindow *law);
void LARecordStop(GtkWidget *widget, LAWindow *law);
void LARecordDelete(GtkWidget *widget, LAWindow *law);
void LARecordView(GtkWidget *widget, LAWindow *law);
void LARecordView2(GtkWidget *widget, LAWindow *law);
void LARecordSaveBin(GtkWidget *widget, LAWindow *law);
void LARecordSaveCSV(GtkWidget *widget, LAWindow *law);

// fileio.c
LAErrorCode LACreateConfigHeader(LAWindow *law, LAConfigHeader *lac);
LAErrorCode LAGetChannelConfig(LAChannel *lac, LAConfigChannel *lax);
LAErrorCode LASetMagicConfigHeader(LAConfigHeader *lac);
LAErrorCode LAGetConfigHeader(LAWindow *law, LAConfigHeader *lac);
LAErrorCode LASetMagicConfigChannel(LAConfigChannel *lac);
gboolean LASaveSession(GtkWidget *widget, LAWindow *law);
gboolean LALoadSession(GtkWidget *widget, LAWindow *law);

// pianotest.c
void LAOpenTestPiano(GtkWidget *widget, LAWindow *law);

// waveformeditor.c
void LAOpenWaveformEditor(GtkWidget *widget, LAWindow *law);

// stramfile.c
void LAOpenFileStreamer(GtkWidget *widget, LAWindow *law);

// filterwindowfunc.c
LAErrorCode LAFilterWindowComboBox(GtkWidget **widget);

// spectrumanalyzer.c
gboolean LAOpenSpectrumAnalyzer(GtkWidget *widget, LAWindow *law);

// windowWidget.c
LAErrorCode LAWindowWidgetInit(LAFilterWindowWidget *widget);
LAErrorCode LAWindowWidgetAdd(GtkWidget *container, LAFilterWindowWidget *widget);
LAErrorCode LAWindowWidgetConnect(LAFilterWindowWidget *widget, GtkWidget *extWidget);

// binarysizewidget.c
LAErrorCode LABinarySizeWidgetInit(LABinarySizeWidget *lab, char *name, size_t start, size_t end, char *unit);
LAErrorCode LABinarySizeWidgetAdd(LABinarySizeWidget *lab, GtkWidget *container);
LAErrorCode LABinarySizeWidgetConnect(LABinarySizeWidget *lab, GtkWidget *externalWidget);

// labelspincombo.c
LAErrorCode LALabelSpinComboSetMin(LALabelSpinCombo *lal, double min);
LAErrorCode LALabelSpinComboSetMax(LALabelSpinCombo *lal, double max);
LAErrorCode LALabelSpinComboSetVisibility(LALabelSpinCombo *lal, bool visibility);
LAErrorCode LALabelSpinComboInit(LALabelSpinCombo *lal, char *label, double value, double min, double max, double step, double page, size_t digits);
LAErrorCode LALabelSpinComboAdd(LALabelSpinCombo *lal, GtkWidget *container);
LAErrorCode LALabelSpinComboConnect(LALabelSpinCombo *lal, GtkWidget *widget);

// status.c
void LAPlaceStatusBar(LAWindow *law);
void LAUpdateStatusBar(LAWindow *law);

#endif //LA_GTK_FUNCS
