#ifndef LIB_LOGIC_ANALYZER_H
#define LIB_LOGIC_ANALYZER_H

#include <gtk/gtk.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>

#define MAX_CHANNEL_COUNT 8
#define CHANNEL_TABLE_HEIGHT 5
#define CHANNEL_TABLE_WIDTH 5
#define SCOPE_WIDTH 1700
#define SCOPE_HEIGTH 100
#define LA_SMALL_BUFFER_SIZE 32
#define LA_BUFFER_SIZE 1024
#define LA_LARGE_BUFFER_SIZE 32768
#define LA_SMALL_INCREMENT_SCOPE 8
#define RGB_COUNT 3
#define LA_SERIAL_FRAME_LENGTH 16
#define CLK_NAMES_COUNT 12
#define CLK_NAMES_LENGTH 16
#define SCOPE_LABEL_TIME_LEFT_X 8
#define SCOPE_LABEL_TIME_RIGHT_X (SCOPE_WIDTH - 32)
#define SCOPE_NSEC_TIME_RIGHT 64
#define SCOPE_LABEL_TIME_Y (SCOPE_HEIGTH - 8)
#define SCOPE_LABEL_TIME_DY 12
#define SCOPE_BUFFER_END_TEXT_DX 64
#define SCOPE_BUFFER_START_TEXT_DX 8

#define DEFAULT_POLLING_RATE 1000
#define DEFAULT_READ_ENABLE  1
#define DEFAULT_VIRTUAL_BUFFER_SIZE 128

#define DEFAULT_BUCKET_LENGTH 0
#define DEFAULT_BUCKET_CAPACITY 524288

#define LA_ALL_CHANNELS_MASK 0xFF

typedef enum {
	LA_COMMAND_NOP			= 0,
	LA_COMMAND_POLLING_RATE = 1,
	LA_COMMAND_READ_ENABLE  = 2,
	LA_COMMAND_BUFFER_SIZE  = 3,
	LA_COMMAND_TEST_RATE  	= 4
} LACommand;

typedef enum {
	LA_NO_ERROR					= 0,
	LA_ERROR_NULLPTR			= 1,
	LA_ERROR_MALLOC				= 2,
	LA_ERROR_NOBUCKET			= 3,
	LA_ERROR_FILE				= 4,
	LA_ERROR_VALUEREAD			= 5,
	LA_ERROR_OUTOFRANGE			= 6
} LAErrorCode;

typedef enum {
	LA_RECORD_DATA_MODE_ALL		= 0,
	LA_RECORD_DATA_MODE_VISIBLE	= 1,
	LA_RECORD_DATA_MODE_CUSTOM	= 2
} LARecordDataMode;

typedef LAErrorCode (*LAHexDumpCallback)(void *, size_t, size_t, uint8_t *, size_t, size_t*);

typedef const char *LAFunctionName;
typedef const char *LAFileName;
typedef int LALineNumber;

extern _Thread_local LAErrorCode	la_errno;
extern _Thread_local LAFunctionName la_funcname;
extern _Thread_local LAFileName 	la_filename;
extern _Thread_local LALineNumber 	la_linenumber;

#define LA_PROPAGATE_ERROR 	-1
#define LA_NO_RETURN 		-2
#define LA_VALUE_ERROR 		-3

#define LA_RAISE_ERROR(__errorCode) do{		\
	la_errno		=__errorCode;			\
	la_funcname 	= __func__;				\
	la_filename 	= __FILE__;				\
	la_linenumber 	= __LINE__;				\
} while(0)

#define LA_HANDLE_NULLPTR(__ptr, __returnValue) do{		\
	if(__ptr == NULL){									\
		LA_RAISE_ERROR(LA_ERROR_NULLPTR);				\
														\
		if(__returnValue == LA_NO_RETURN){				\
			return 0;									\
		} else if(__returnValue == LA_PROPAGATE_ERROR){	\
			return LA_ERROR_NULLPTR;					\
		}												\
		return __returnValue;							\
	}													\
} while(0)

typedef struct {
	uint8_t command;
	uint32_t value;
	uint8_t frames[LA_SERIAL_FRAME_LENGTH];
} LASerialProtocol;

typedef struct LA_BUCKET{
	uint32_t length;
	uint32_t capacity;
	struct LA_BUCKET *next;
	uint8_t *data;
} LABucket;

typedef struct{
	uint8_t visible;
	uint8_t bit;
	uint8_t muted;
		
	GtkWidget *container;
	GtkWidget *scrollable;
	GtkWidget *scope;
	double r;
	double g;
	double b;

	GtkWidget *control;
	GtkWidget *muteButton;
	GtkWidget *soloButton;
	GtkWidget *colorButton;
	GtkWidget *entry;

	GtkWidget *label;
	GtkWidget *dropdown;

} LAChannel;

typedef struct{
	GtkWidget *menubar;

	GtkWidget *menuFile;
	GtkWidget *listFile;
	GtkWidget *menuFileNew;
	GtkWidget *menuFileLoad;
	GtkWidget *menuFileSave;
	GtkWidget *menuFileExit;

	GtkWidget *menuView;
	GtkWidget *listView;
	GtkWidget *menuViewZoomLess;
	GtkWidget *menuViewZoomMore;
	GtkWidget *menuViewZoomReset;
	GtkWidget *menuViewZoomSet;
	GtkWidget *menuViewZoomClk;
	GtkWidget *menuViewSep1;
	GtkWidget *menuViewAdvance;
	GtkWidget *menuViewRewind;
	GtkWidget *menuViewGotoStart;
	GtkWidget *menuViewGotoEnd;
	GtkWidget *menuViewGoto;
	GtkWidget *menuViewSep2;
	GtkWidget *menuViewPauseUpdate;
	GtkWidget *menuViewPauseScroll;
	GtkWidget *menuViewSep3;
	GtkWidget *menuViewTimeRelative;
	GtkWidget *menuViewTimeAbsolute;
	GtkWidget *menuViewSampleRelative;
	GtkWidget *menuViewSampleAbsolute;
	GtkWidget *menuViewBufferEnd;
	GtkWidget *menuViewRulerBuffer;
	GtkWidget *menuViewRulerClock;
	GtkWidget *menuViewSep4;
	GtkWidget *menuViewMuteAll;

	GtkWidget *menuDevice;
	GtkWidget *listDevice;
	GtkWidget *menuDeviceConnect;
	GtkWidget *menuDeviceSep1;
	GtkWidget *menuDeviceSetPolling;
	GtkWidget *menuDeviceSetRead;
	GtkWidget *menuDeviceSetLength;
	GtkWidget *menuDeviceSendCustom;

	GtkWidget *menuRecord;
	GtkWidget *listRecord;
	GtkWidget *menuRecordStart;
	GtkWidget *menuRecordStop;
	GtkWidget *menuRecordPause;
	GtkWidget *menuRecordDelete;
	GtkWidget *menuRecordView;
	GtkWidget *menuRecordSep1;
	GtkWidget *menuRecordAllCh;
	GtkWidget *menuRecordVisibleCh;
	GtkWidget *menuRecordCustomCh;
	GtkWidget *menuRecordCustomCh0;
	GtkWidget *menuRecordCustomCh1;
	GtkWidget *menuRecordCustomCh2;
	GtkWidget *menuRecordCustomCh3;
	GtkWidget *menuRecordCustomCh4;
	GtkWidget *menuRecordCustomCh5;
	GtkWidget *menuRecordCustomCh6;
	GtkWidget *menuRecordCustomCh7;
	GtkWidget *menuRecordSep2;
	GtkWidget *menuRecordLoad;
	GtkWidget *menuRecordSaveCSV;
	GtkWidget *menuRecordSaveBin;

	GtkWidget *menuTools;
	GtkWidget *listTools;
	GtkWidget *menuToolsBufferHex;
	GtkWidget *menuToolsConfigHex;
	GtkWidget *menuToolsFileHex;

	GtkWidget *menuAdv;
	GtkWidget *listAdv;
	GtkWidget *menuAdvSibelius;
	GtkWidget *menuAdvMusescore;
	GtkWidget *menuAdvAdvanced1;
	GtkWidget *menuAdvAdvanced2;
	GtkWidget *menuAdvAdvanced3;
	GtkWidget *menuAdvAdvanced4;
	GtkWidget *menuAdvAdvanced5;
} LAMenu;

typedef struct{
	GtkWidget *hbox;
	GtkWidget *dropdown;
	GtkWidget *connect;
	GtkWidget *label;
	GtkWidget *status;

	GtkWidget *command;
	GtkWidget *entry;
	GtkWidget *send;


	_Atomic uint8_t isConnected;
	int fd;
	char *device;
	uint8_t internalDeviceId;
	pthread_t readThread;
	_Atomic uint8_t breakReadLoop;
	_Atomic uint8_t dataHasChanged;
} LAConnect;


typedef struct {
	uint16_t dataOffset;
	int16_t scopeOffset;
	uint8_t dontWrite;
	int8_t zoom;
	uint8_t hasRenderFunctionAdded;
	int renderSourceId;
	uint8_t showRelativeTime;
	uint8_t showAbsoluteTime;
	uint8_t showRelativeSample;
	uint8_t showAbsoluteSample;
	uint8_t showBufferEnd;
	uint8_t showBufferRuler;
	uint8_t showClockRuler;

	uint8_t addDataOffset;
	int32_t pollingTime;
	int32_t virtualBufferSize;
	uint64_t sampleCounter;

	struct timespec startTime;
} LARenderDetails;

typedef struct {
	pthread_mutex_t bucketAccess;
	pthread_mutex_t dataBufferAccess;
} LAMutexes;

typedef struct {
	LABucket *bucketStart;
	LABucket *bucketCurrent;
	uint8_t bucketWrite;
	uint8_t hasStopped;
	uint8_t dataMask;
	uint8_t dataMode;
} LABucketDetails; 

typedef struct {
	GtkWidget *hbox;
	GtkWidget *pollingTime;
	GtkWidget *writeEnable;
	GtkWidget *virtualBufferSize;
	GtkWidget *capture;
	GtkWidget *device;
	GtkWidget *scopeWrite;
	GtkWidget *scopeScroll;
	GtkWidget *zoom;
} LAStatus;

typedef struct{
	LAChannel channel[MAX_CHANNEL_COUNT];
	uint8_t channelCount;

	LAMenu menu;
	LAConnect connect;
	LARenderDetails rd;
	LAMutexes mutexes;
	LABucketDetails bd;
	LAStatus status;

	pthread_t windowUpdateThread;
	uint8_t dataBuffer[LA_LARGE_BUFFER_SIZE];

	GtkWidget *window;
	GtkWidget *container;
	GtkWidget *scrollableChannels;
	GtkWidget *vbox;
} LAWindow;

typedef struct {
	GtkWidget *window;
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
	GtkWidget *cancel;

	uint8_t isActive;
} LAZoomClockSyncWindow;

typedef struct {
	GtkWidget *window;
	GtkWidget *vbox;
	GtkWidget *hbox;

	GtkWidget *label;
	GtkWidget *spin;
	GtkWidget *okay;
	GtkWidget *cancel;

	uint8_t isActive;
} LAZoomSetWindow;

typedef struct {
	GtkWidget *window;
	GtkWidget *vbox;
	GtkWidget *dropdown;
	GtkWidget *connect;
	GtkWidget *label;
	GtkWidget *status;
	GtkTextBuffer *textBuffer;
	GtkWidget *response;
	GtkWidget *hbox;
	GtkWidget *cancel;
} LAConnectWindow;

typedef struct {
	GtkWidget *window;
	GtkWidget *vbox;
	GtkWidget *label;
	GtkWidget *dropdown;
	GtkWidget *value;
	GtkWidget *spinButton;
	GtkWidget *response;
	GtkWidget *status;
	GtkTextBuffer *textBuffer;

	GtkWidget *hbox;
	GtkWidget *send;
	GtkWidget *cancel;
} LACommandWindow;

typedef struct {
	GtkWidget *window;
	GtkWidget *vbox;

	GtkWidget *offsetHbox;
	GtkWidget *offsetLabel;
	GtkWidget *offsetSpin;
	GtkWidget *offsetButton;

	GtkWidget *hexHbox;
	GtkWidget *hexOffsetView;
	GtkTextBuffer *hexOffsetBuffer;
	GtkWidget *hexDataView;
	GtkTextBuffer *hexDataBuffer;
	GtkWidget *hexCharView;
	GtkTextBuffer *hexCharBuffer;

	GtkWidget *infoHbox;
	GtkWidget *infoOffset;
	GtkWidget *infoRange;
	GtkWidget *infoSize;

	GtkWidget *buttonsHbox;
	GtkWidget *buttonExport;
	GtkWidget *buttonQuit;

	size_t offset;
	void *src;
	size_t srcSize;
	LAHexDumpCallback callback;
	pthread_mutex_t *mutex;
} LAHexView;

typedef struct __attribute__((packed)) {
	uint32_t headerStart;
	uint16_t fileVersion;
	uint8_t  device;
	uint8_t  zoom;
	uint32_t flags;
	uint32_t clockTime;
	uint32_t pollingTime;
	uint16_t readDetails;
	uint8_t captureChannelMode;
	uint8_t captureChannelMask;
	uint32_t headerEnd;
} LAConfigHeader;

typedef struct {
	uint8_t startByte;
	uint8_t channel;
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t nameLength;
	char *name;
	uint8_t endByte;
} LAConfigChannel;

extern LAWindow *lawp;
extern double defaultChannelColors[MAX_CHANNEL_COUNT][RGB_COUNT];
extern double defaultMidLineColor[RGB_COUNT];
extern const char validClockNames[CLK_NAMES_COUNT][CLK_NAMES_LENGTH];

#define CAIRO_COLOR_FROM_ARRAY(_cr, _a) cairo_set_source_rgb(_cr, _a[0], _a[1], _a[2])
#define CONVERT_INT_TO_GPOINTER(__value) ((gpointer) ((uintptr_t) ((uint8_t)__value))) 
#define CONVERT_GPOINTER_TO_INT(__value) ((uint8_t) ((uintptr_t)__value))

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

// threads.c
void *LAReadThread(void *vlaw);
//void *LAWindowUpdateLoop(void *vlaw);
void LAWindowUpdateLoop(LAWindow *law);
int LAWindowUpdateLoopConnector(void *vlaw);

// protocol
void LAPrepareProtocol(LASerialProtocol *p, uint8_t command, uint32_t value);

// clocksync.c
int8_t LAGetClockChannel(LAWindow *law);
void LACreateZoomClockSyncWindow(LAWindow *law, LAZoomClockSyncWindow *laz);

// clockset.c
void LACreateZoomSetWindow(LAWindow *law, LAZoomSetWindow *laz);

// bucket.c
LABucket *LACreateBucket();
LAErrorCode LABucketInsertData(LABucket **bucketp, uint8_t data);
int64_t LABucketGetSizeAll(LABucket *bucketStart);
LAErrorCode LABucketDestroy(LABucket *bucket);
LAErrorCode LABucketDestroyAll(LABucket *bucketStart);
LAErrorCode LABucketView(LABucket *bucket);
LAErrorCode LABucketViewAll(LABucket *bucketStart);
LAErrorCode LAWriteBucketsToFileBinary(LAWindow *law, gchar *filename);
LAErrorCode LAWriteBucketsToFileCSV(LAWindow *law, gchar *filename);
LAErrorCode LACallbackHexViewBucketAll(void *src, size_t srcSize, size_t offset, uint8_t *viewBuffer, size_t viewSize, size_t *bytesRead);

// record.c
void LARecordStart(GtkWidget *widget, LAWindow *law);
void LARecordPause(GtkWidget *widget, LAWindow *law);
void LARecordStop(GtkWidget *widget, LAWindow *law);
void LARecordDelete(GtkWidget *widget, LAWindow *law);
void LARecordView(GtkWidget *widget, LAWindow *law);
void LARecordView2(GtkWidget *widget, LAWindow *law);
void LARecordSaveBin(GtkWidget *widget, LAWindow *law);
void LARecordSaveCSV(GtkWidget *widget, LAWindow *law);

// dialogs.c
gchar *LADialogSaveFile(LAWindow *law, const char *title, const char *filterName);
gchar *LADialogOpenFile(LAWindow *law, const char *title, const char *filterName);
void LADialogErrorGeneric(LAWindow *law, char *text);
void LADialogNumericEntryError(LAWindow *law);

// status.c
void LAPlaceStatusBar(LAWindow *law);
void LAUpdateStatusBar(LAWindow *law);

// HexView.c
LAErrorCode LACreateHexView(LAWindow *law, void *buffer, size_t size, LAHexDumpCallback callback, pthread_mutex_t *mutex);
LAErrorCode LACallbackHexViewSingleBucket(void *src, size_t srcSize, size_t offset, uint8_t *viewBuffer, size_t viewSize, size_t *bytesRead);
LAErrorCode LACallbackCircularBufferView(void *src, size_t srcSize, size_t offset, uint8_t *viewBuffer, size_t viewSize, size_t *bytesRead);
LAErrorCode LACallbackNormalBuffer(void *src, size_t srcSize, size_t offset, uint8_t *viewBuffer, size_t viewSize, size_t *bytesRead);
LAErrorCode LACallbackFileView(void *src, size_t srcSize, size_t offset, uint8_t *viewBuffer, size_t viewSize, size_t *bytesRead);

// fileio.c
LAErrorCode LACreateConfigHeader(LAWindow *law, LAConfigHeader *lac);
LAErrorCode LAGetChannelConfig(LAChannel *lac, LAConfigChannel *lax);
LAErrorCode LASetMagicConfigHeader(LAConfigHeader *lac);
LAErrorCode LAGetConfigHeader(LAWindow *law, LAConfigHeader *lac);
LAErrorCode LASetMagicConfigChannel(LAConfigChannel *lac);
gboolean LASaveSession(GtkWidget *widget, LAWindow *law);

#endif // LIB_LOGIC_ANALYZER_H
