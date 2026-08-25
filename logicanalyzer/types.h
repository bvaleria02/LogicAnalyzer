#ifndef LA_TYPES
#define LA_TYPES

#include "structures/listStore.h"
#include "structures/dequeStore.h"
#include "threads/ack.h"
#include <pthread.h>

#ifndef LABucket
	typedef struct _la_bucket LABucket;
#endif

#ifndef LAWindow
	typedef struct _la_window LAWindow;
#endif

#ifndef LAPresetFile
	typedef struct _la_preset_file LAPresetFile;
#endif

#ifndef LASerialV2Protocol
	typedef struct _la_serial_v2_protocol LASerialV2Protocol;
#endif

#ifndef LAZoomSetWindow
	typedef struct _la_zoom_set_window LAZoomSetWindow;
#endif

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
	GtkWidget *menuToolsSep1;
	GtkWidget *menuToolsTestPiano;
	GtkWidget *menuToolsWaveformEditor;
	GtkWidget *menuToolsClockFreq;
	GtkWidget *menuToolsStreamFile;
	GtkWidget *menuToolsPresetEditor;
	GtkWidget *menuToolsSpectrumAnal;

	GtkWidget *menuAdv;
	GtkWidget *listAdv;
	GtkWidget *menuAdvSibelius;
	GtkWidget *menuAdvMusescore;
	GtkWidget *menuAdvAdvanced1;
	GtkWidget *menuAdvAdvanced2;
	GtkWidget *menuAdvAdvanced3;
	GtkWidget *menuAdvAdvanced4;
	GtkWidget *menuAdvAdvanced5;

	GtkWidget *menuHelp;
	GtkWidget *listHelp;
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


	uint8_t isWaitingACK;
	pthread_mutex_t lockACK;
	pthread_cond_t condACK;

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

typedef struct {
	pthread_t thread;
	pthread_cond_t cond;
	pthread_mutex_t mutex;
	bool dirty;
	bool close;
} LAThread;

typedef struct {
	LAThread thread;
	LADequeStore deque;
	pthread_mutex_t dequeMutex;
} LATXThread;

struct _la_window {
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

	LAACK ack;
	LATXThread tx;
};

typedef struct {
	GtkWidget *window;
	GtkWidget *content;
	GtkWidget *vbox;
	GtkWidget *dropdown;
	GtkWidget *connect;
	GtkWidget *label;
	GtkWidget *status;
	GtkTextBuffer *textBuffer;
	GtkWidget *response;
	GtkWidget *hbox;
} LAConnectWindow;

typedef struct {
	GtkWidget *window;
	GtkWidget *content;
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
} LACommandWindow;

typedef struct {
	GtkWidget *window;
	GtkWidget *content;
	GtkWidget *vbox;

	GtkWidget *hboxControl1;
	GtkWidget *waveSelectorLabel;
	GtkWidget *waveSelector;
	GtkAdjustment *waveSelectorAdj;
	GtkWidget *buttonNew;
	GtkWidget *buttonDelete;
	GtkWidget *buttonMoveLeft;
	GtkWidget *buttonMoveRight;
	GtkWidget *buttonDeleteAll;

	GtkWidget *hboxControl2;
	GtkWidget *waveSizeLabel;
	GtkWidget *waveSize;
	GtkAdjustment *waveSizeAdj;
	GtkWidget *flat;
	GtkWidget *presetsLabel;
	GtkWidget *presets;
	GtkWidget *filterButton;

	GtkWidget *hboxWave;
	GtkWidget *wave;

	GtkWidget *hboxHexview;
	GtkWidget *hexview;

	GtkWidget *hboxButtons;
	GtkWidget *sendThis;
	GtkWidget *sendAll;
	GtkWidget *open;
	GtkWidget *exportThis;
	GtkWidget *exportAll;

	LAWaveform *wavetable;
	LAWaveform *currentwave;
	uint8_t mouseClick;
} LAWaveformEditor;

typedef struct {
	GtkWidget *box;
	GtkWidget *label;
	GtkWidget *combobox;
	GtkWidget *graph;
	GtkWidget *graph2;

	GtkWidget *paramLabel[LA_FIR_FILTER_PARAMS];
	GtkAdjustment *paramAdj[LA_FIR_FILTER_PARAMS];
	GtkWidget *paramSb[LA_FIR_FILTER_PARAMS];

	uint8_t windowType;
	double paramValue[LA_FIR_FILTER_PARAMS];
	GtkWidget *externWidget;
} LAFilterWindowWidget;

typedef struct {
	GtkWidget *vbox;
	GtkWidget *name;
	GtkWidget *combobox;

	size_t start;
	size_t end;
	size_t value;
	size_t length;

	GtkWidget *externalWidget;
} LABinarySizeWidget;

typedef struct {
	GtkWidget *vbox;
	GtkWidget *label;
	GtkAdjustment *adjustment;
	GtkWidget *spinButton;

	double value;
	double min;
	double max;
	double step;
	double page;
	double digits;
	GtkWidget *externalWidget;
} LALabelSpinCombo;


#endif //LA_TYPESW
