#ifndef LA_CLOCK_SYNC
#define LA_CLOCK_SYNC

#ifndef LABufferSelect
  typedef enum _la_buffer_select LABufferSelect;
#endif

typedef struct _la_clocksync_node{
	uint8_t value;
	uint32_t duration;
	uint8_t ignore;
	struct _la_clocksync_node *prev;
	struct _la_clocksync_node *next;
} LAClockSyncNode;

typedef struct {
	GtkWidget *window;
	GtkWidget *content;
	GtkWidget *vbox;
	GtkWidget *hbox;

	GtkWidget *channelSelect;
	GtkWidget *bufferSelect;
	GtkWidget *optionsSelect;

	GtkWidget *chSelLabel;
	GtkWidget *chSelDropdown;

	GtkWidget *buSelLabel;
	GtkWidget *buSelDropdown;
	GtkWidget *buSelEntry;
	GtkWidget *buSelLabel2;
	GtkWidget *buSelDescView;
	GtkTextBuffer *buSelDescBuffer;
	GtkWidget *buSelSep;
	GtkWidget *buSelResultTile;
	GtkWidget *buSelResultValue;
	GtkWidget *buSelResultValue2;
	GtkWidget *buSelSep2;
	GtkWidget *buSelResultTip;
	
	GtkWidget *opLabel;
	GtkWidget *opFallingEdge;
	GtkWidget *opRisingEdge;
	GtkWidget *opDiscardOutliers;
	GtkWidget *opDiscardOutliersLabel;
	GtkWidget *opDiscardOutliersSB;
	GtkWidget *opDiscardOutliersHbox;
	GtkWidget *opPeriodLess;
	GtkWidget *opPeriodLessLabel;
	GtkWidget *opPeriodLessSB;
	GtkWidget *opPeriodLessHbox;
	GtkWidget *opPeriodMore;
	GtkWidget *opPeriodMoreLabel;
	GtkWidget *opPeriodMoreSB;
	GtkWidget *opPeriodMoreHbox;
	GtkWidget *opExperimentalLabel;
	GtkWidget *opAutocorrelation;

	GtkWidget *confirmHbox;
	GtkWidget *okay;

	uint8_t isActive;
	uint32_t pollingTime;
	
	uint8_t channelSelectValue;
	uint8_t bufferSelectValue;
	uint32_t bufferValue;
	uint8_t useFallingEdge;
	uint8_t useRisingEdge;
	uint8_t discardOutliers;
	double sigmaValue;
	uint8_t discardLower;
	uint32_t lowerValue;
	uint8_t discardUpper;
	uint32_t upperValue;
	uint8_t useCorrelation;

	uint8_t *buffer;
	uint32_t bufferSize;
	LAClockSyncNode *splitNodeStart;
	double mean;
	double std;
	int response;
} LAZoomClockSyncWindow;

// clocksync.c
int8_t LAGetClockChannel(LAWindow *law);
void LACreateZoomClockSyncWindow(LAWindow *law, LAZoomClockSyncWindow *laz, const char *title);
void LAOpenClockFreqAnalyzer(GtkWidget *widget, LAWindow *law);
LAErrorCode LAGetFromCircularBuffer(LAWindow *law, uint8_t **buffer, uint32_t *bufSize, LABufferSelect bufSel, uint32_t bufTargetSize);

#endif //LA_CLOCK_SYNC
