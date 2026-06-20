#ifndef LIB_LOGIC_ANALYZER_H
#define LIB_LOGIC_ANALYZER_H

#include <gtk/gtk.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>

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
#define LA_SERIAL_V2_DATA_LENGTH 257
#define LA_SERIAL_V2_FRAME_LENGTH 261
#define LA_SERIAL_V2R_DATA_LENGTH 1025
#define LA_SERIAL_V2R_FRAME_LENGTH 1030
#define CLK_NAMES_COUNT 12
#define CLK_NAMES_LENGTH 16
#define SCOPE_LABEL_TIME_LEFT_X 8
#define SCOPE_LABEL_TIME_RIGHT_X (SCOPE_WIDTH - 32)
#define SCOPE_NSEC_TIME_RIGHT 64
#define SCOPE_LABEL_TIME_Y (SCOPE_HEIGTH - 8)
#define SCOPE_LABEL_TIME_DY 12
#define SCOPE_BUFFER_END_TEXT_DX 64
#define SCOPE_BUFFER_START_TEXT_DX 8
#define LA_EPS 1e-6

#define DEFAULT_POLLING_RATE 1000
#define DEFAULT_READ_ENABLE  1
#define DEFAULT_VIRTUAL_BUFFER_SIZE 128

#define LA_READ_BUFFER_LENGTH 8192

#define DEFAULT_BUCKET_LENGTH 0
#define DEFAULT_BUCKET_CAPACITY 524288

#define LA_ALL_CHANNELS_MASK 0xFF
#define LA_TAG_END 0xFFFF

#define LA_MATRIX_INDEX(c, r, h) ((r)*(h) + (c))

typedef enum {
	LA_COMMAND_NOP			= 0,
	LA_COMMAND_POLLING_RATE = 1,
	LA_COMMAND_READ_ENABLE  = 2,
	LA_COMMAND_BUFFER_SIZE  = 3,
	LA_COMMAND_TEST_CLOCK1_RATE 	= 4,
	LA_COMMAND_TEST_CLOCK2_RATE 	= 5,
	LA_COMMAND_TEST_NOISE_RATE 		= 6,
	LA_COMMAND_TEST_NOISE_MODE 		= 7,
  	LA_COMMAND_TEST_DAC_VALUE     	= 8,
  	LA_COMMAND_TEST_DAC_MODE      	= 9,
  	LA_COMMAND_TEST_DAC_SPEED     	= 10,
  	LA_COMMAND_TEST_DAC_WRAP      	= 11,
  	LA_COMMAND_TEST_DAC_WAVE      	= 12,
  	LA_COMMAND_OUT_VALUE          	= 13,
	LA_COMMAND_TEST_CLOCK1_MUTE 	= 14,
	LA_COMMAND_TEST_CLOCK2_MUTE 	= 15,
	LA_COMMAND_TEST_NOISE_MUTE 		= 16,
	LA_COMMAND_TEST_DAC_MUTE 		= 17,
  	LA_COMMAND_TEST_DAC_WAVE_FULL  	= 18,
	LA_COMMAND_SEND_DAC_STREAM		= 19
} LACommand;

typedef enum {
	LA_RX_COMMAND_NOP				= 0,
	LA_RX_COMMAND_ACK				= 1,
	LA_RX_COMMAND_CAPTURE			= 2
} LARecvCommand;

typedef enum {
	LA_NO_ERROR						= 0,
	LA_ERROR_NULLPTR				= 1,
	LA_ERROR_MALLOC					= 2,
	LA_ERROR_NOBUCKET				= 3,
	LA_ERROR_FILE					= 4,
	LA_ERROR_VALUEREAD				= 5,
	LA_ERROR_OUTOFRANGE				= 6,
	LA_ERROR_FILEREAD				= 7,
	LA_ERROR_INCORRECTVALUE			= 8,
	LA_ERROR_FILENOTFOUND			= 9,
	LA_ERROR_MMAP					= 10,
	LA_ERROR_MUNMAP					= 11,
	LA_ERROR_INVALIDSYNTAX			= 12,
	LA_ERROR_INVALIDVALUE			= 13,
	LA_ERROR_ZEROLENGTH				= 14,
	LA_ERROR_MATRIX					= 15,
	LA_ERROR_NONMATCHING_DIMENSION 	= 16,
	LA_ERROR_PERMISSIONS 			= 17,
	LA_ERROR_NONINVERTIBLE_MATRIX   = 18,
	LA_ERROR_BIG_INT				= 19,
	LA_ERROR_ZERODIV				= 20,
	LA_ERROR_OPENGL_SHADERS			= 21
} LAErrorCode;

typedef enum {
	LA_RECORD_DATA_MODE_ALL		= 0,
	LA_RECORD_DATA_MODE_VISIBLE	= 1,
	LA_RECORD_DATA_MODE_CUSTOM	= 2
} LARecordDataMode;

typedef enum {
	LA_CHANNEL_SELECT_AUTODETECT_CLOCK	= 0,
	LA_CHANNEL_SELECT_0					= 1,
	LA_CHANNEL_SELECT_1					= 2,
	LA_CHANNEL_SELECT_2					= 3,
	LA_CHANNEL_SELECT_3					= 4,
	LA_CHANNEL_SELECT_4					= 5,
	LA_CHANNEL_SELECT_5					= 6,
	LA_CHANNEL_SELECT_6					= 7,
	LA_CHANNEL_SELECT_7					= 8
} LAChannelSelectorClockSync;

typedef enum {
	LA_BUFFER_SELECT_VISIBLE	= 0,
	LA_BUFFER_SELECT_ALL		= 1,
	LA_BUFFER_SELECT_CUSTOM		= 2,
	LA_BUFFER_SELECT_003		= 3 
} LABufferSelect;

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

#define LA_CLAMP_VALUE(__value, __min, __max) do{	\
	if((__value) < (__min)) (__value) = (__min);	\
	if((__value) > (__max)) (__value) = (__max);	\
} while(0)

#define LA_GET_PARAMETER(__index, __paramList, __paramCount) ((__index) < (__paramCount) && (__index) >= 0) ? ((__paramList)[__index]) : 0

#define LA_USE_VAR(__var) (void) (__var)

#define LA_PROFILER(__func, __name) do{							\
	{													\
		struct timespec __startTime;					\
		clock_gettime(CLOCK_MONOTONIC, &__startTime);	\
		struct timespec __endTime;						\
		{__func}										\
		clock_gettime(CLOCK_MONOTONIC, &__endTime);		\
		printf("Time spent (%s): %le seconds\n", __name, ((__endTime.tv_sec + (__endTime.tv_nsec / (double) 1000000000)) - (__startTime.tv_sec + (__startTime.tv_nsec / (double) 1000000000))));	\
	}													\
} while(0)

typedef struct {
	uint8_t command;
	uint32_t value;
	uint8_t frames[LA_SERIAL_FRAME_LENGTH];
} LASerialProtocol;

typedef struct {
	uint8_t command;
	uint16_t length;
	uint16_t fcs;
	uint8_t data[LA_SERIAL_V2_DATA_LENGTH];
	uint8_t frames[LA_SERIAL_V2_FRAME_LENGTH];
	uint16_t frameLength;
	uint8_t isReady;
} LASerialV2Protocol;

typedef struct {
	uint8_t command;
	uint16_t length;
	uint16_t fcs;
	uint8_t data[LA_SERIAL_V2R_DATA_LENGTH];
	uint8_t frames[LA_SERIAL_V2R_FRAME_LENGTH];
	uint16_t frameLength;
	uint8_t isReady;
	uint16_t offset;
} LASerialV2RecvProtocol;

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

typedef struct {
	GtkWidget *window;
	GtkWidget *content;
	GtkWidget *vbox;

	GtkWidget *label;
	GtkWidget *spin;

	uint8_t isActive;
} LAZoomSetWindow;

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

#define LA_WAVEFORM_SIZE 256
#define LA_DEFAULT_WAVE_FLAT_VALUE 128

typedef struct _la_waveform{
	uint16_t size;
	uint8_t data[LA_WAVEFORM_SIZE];

	struct _la_waveform *prev;
	struct _la_waveform *next;
} LAWaveform;

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

typedef enum {
	LA_WAVE_PRESET_CUSTOM		= 0,
	LA_WAVE_PRESET_SQUARE		= 1,
	LA_WAVE_PRESET_SINE			= 2,
	LA_WAVE_PRESET_TRIANGLE		= 3,
	LA_WAVE_PRESET_SAW			= 4,
	LA_WAVE_PRESET_EXP			= 5,
	LA_WAVE_PRESET_NOISE		= 6,
	LA_WAVE_PRESET_PULSE25_0	= 7,
	LA_WAVE_PRESET_PULSE12_5	= 8
} LAWavePreset;

#define LA_PF_MAGIC_LENGTH 4
#define LA_PF_NAME_LENGTH 1024
#define LA_PF_PADDING_LENGTH 3

typedef struct {
	uint8_t magic[LA_PF_MAGIC_LENGTH];
	uint32_t version;
	uint32_t pluginId;
	uint16_t nameLength;
	uint8_t name[LA_PF_NAME_LENGTH];
	uint32_t crc32Header;
	uint32_t crc32Data;
	uint8_t padding[LA_PF_PADDING_LENGTH];
} LAPFHeader;

typedef struct _lapf_data_ {
	uint16_t tag;
	uint16_t length;
	uint8_t *value;
	struct _lapf_data_ *prev;
	struct _lapf_data_ *next;
} LAPFDataNode;

typedef struct {
	LAPFHeader header;
	uint32_t dataLength;
	LAPFDataNode *dataStart;
	LAPFDataNode *dataLast;
} LAPresetFile;

typedef struct {
	uint8_t *data;
	size_t capacity;
	size_t length;
	size_t readOffset;
} LAMappedFile;

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

	uint8_t isFileOpen;
	LAMappedFile file;
	LAPresetFile presetFile;
} LAPresetEditor;

#define LA_FIR_FILTER_PARAMS 3
#define GRAPH_MINI_WIDTH 	256
#define GRAPH_MINI_HEIGHT 	128

#define GRAPH_WIDTH 	768
#define GRAPH_HEIGHT 	192

typedef enum {
	LA_FILTER_WINDOW_RECTANGULAR		= 0,
	LA_FILTER_WINDOW_TRIANGULAR			= 1,
	LA_FILTER_WINDOW_WELCH				= 2,
	LA_FILTER_WINDOW_HANN				= 3,
	LA_FILTER_WINDOW_HAMMING			= 4,
	LA_FILTER_WINDOW_TRAPZ				= 5,
	LA_FILTER_WINDOW_CIRCULAR			= 6,
	LA_FILTER_WINDOW_SINC				= 7,
	LA_FILTER_WINDOW_IMPULSE			= 8,
	LA_FILTER_WINDOW_BLACKMAN			= 9,
	LA_FILTER_WINDOW_BLACKMAN_HARRIS	= 10,
	LA_FILTER_WINDOW_KAISER				= 11,
	LA_FILTER_WINDOW_GAUSSIAN			= 12,
	LA_FILTER_WINDOW_NUTALL				= 13,
	LA_FILTER_WINDOW_FLATTOP			= 14,
	LA_FILTER_WINDOW_PARZEN				= 15,
	LA_FILTER_WINDOW_COSINESUM			= 16,
	LA_FILTER_WINDOW_BLACKMAN_NUTALL	= 17,
	LA_FILTER_WINDOW_SINEPOWER			= 18,
	LA_FILTER_WINDOW_APPROX_GAUSSIAN	= 19,
	LA_FILTER_WINDOW_TUKEY				= 20,
	LA_FILTER_WINDOW_PLANK_TAPPER		= 21,
	LA_FILTER_WINDOW_POISSON			= 22,
	LA_FILTER_WINDOW_LANCZOS			= 23,
	LA_FILTER_WINDOW_NOISE				= 24,
	LA_FILTER_WINDOW_LOGISTICAL			= 25,
	LA_FILTER_WINDOW_LOGISTICAL_2		= 26,
	LA_FILTER_WINDOW_DAMPED				= 27,
	LA_FILTER_WINDOW_GAUSSINE			= 28,
	LA_FILTER_WINDOW_POLY_CHEBYSHEV		= 29,
	LA_FILTER_WINDOW_SMOOTH_TRAPZ		= 30,
	LA_FILTER_WINDOW_ROOT_CHEBYSHEV		= 31,
	LA_FILTER_WINDOW_COMPACT_SINE		= 32
} LAFilterWindowType;

#define LA_FILTER_WINDOW_COUNT 33

typedef struct {
	char *name;
	double value;
	double min;
	double max;
	double stepIncrement;
	double pageIncrement;
	double digits;
} LAFilterWindowParameter;

typedef struct {
	char *name;
	LAFilterWindowParameter param[LA_FIR_FILTER_PARAMS];
	LAErrorCode (*callable)(double *, size_t, double *, size_t);
} LAFilterWindowDetails;

extern LAFilterWindowDetails LAWindowTypeDetails[LA_FILTER_WINDOW_COUNT];

#define LA_IIR_LENGTH_A 3
#define LA_IIR_LENGTH_B 3
#define LA_CS_COUNT 15

typedef struct {
	uint8_t filterType;

	uint8_t FIRFilterSize;
	uint8_t FIRFilterType;
	double	FIRFrequency;
	uint8_t FIRWindowType;
	double 	FIRParam1;
	double 	FIRParam2;
	double 	FIRParam3;

	double 	IIRa[LA_IIR_LENGTH_A];
	double 	IIRb[LA_IIR_LENGTH_B];

	uint64_t MALength;

	uint8_t csType;
	double csParam[LA_CS_COUNT];
	uint8_t csSolver;

	size_t cfgTest;
	bool cfgIn;
	bool cfgOut;
	bool cfgNorm;
} LAFilterValue;

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
void LASendSerialV2(LAWindow *law, LASerialV2Protocol *p);

// threads.c
void *LAReadThread(void *vlaw);
//void *LAWindowUpdateLoop(void *vlaw);
void LAWindowUpdateLoop(LAWindow *law);
int LAWindowUpdateLoopConnector(void *vlaw);

// protocol
void LAPrepareProtocol(LASerialProtocol *p, uint8_t command, uint32_t value);
void LAPrepareProtocolV2Basic(LASerialV2Protocol *p, uint8_t command, uint32_t value);
uint8_t LAFuncPopcount(uint8_t value);
uint16_t LACalculateFCSProtocolV2(LASerialV2Protocol *p);
void LACompileProtocolV2Frames(LASerialV2Protocol *p);
void LAPrepareProtocolV2Wave(LASerialV2Protocol *p, uint8_t bank, uint8_t *wave, uint16_t size);
void LAPrepareProtocolV2Stream(LASerialV2Protocol *p, uint8_t *data, uint16_t size);
LAErrorCode LAProtocolV2RInit(LASerialV2RecvProtocol *p);
LAErrorCode LAProtocolV2RFill(LASerialV2RecvProtocol *p, uint8_t *data, uint16_t size);
uint8_t LAProtocolV2RIsReady(LASerialV2RecvProtocol *p);
LAErrorCode LAProtocolV2RUnpack(LASerialV2RecvProtocol *p);

// clocksync.c
int8_t LAGetClockChannel(LAWindow *law);
void LACreateZoomClockSyncWindow(LAWindow *law, LAZoomClockSyncWindow *laz, const char *title);
void LAOpenClockFreqAnalyzer(GtkWidget *widget, LAWindow *law);
LAErrorCode LAGetFromCircularBuffer(LAWindow *law, uint8_t **buffer, uint32_t *bufSize, LABufferSelect bufSel, uint32_t bufTargetSize);

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
void LADialogWarningGeneric(LAWindow *law, char *text);
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
gboolean LALoadSession(GtkWidget *widget, LAWindow *law);

// pianotest.c
void LAOpenTestPiano(GtkWidget *widget, LAWindow *law);

// waveformeditor.c
void LAOpenWaveformEditor(GtkWidget *widget, LAWindow *law);

// stramfile.c
void LAOpenFileStreamer(GtkWidget *widget, LAWindow *law);

// filterwindow.c
LAErrorCode LAFilterGraphDrawBG(cairo_t *cr, double width, double height, bool midLine);
LAErrorCode LAFilterGenerateFIRBuffer(LAFilterValue *laf, double **buffer, size_t *size);
LAErrorCode LAFilterGenerateFIRSinc(double *buffer, size_t size, double f, int mode);
// LAErrorCode LAFilterGraphDrawArray(cairo_t *cr, double *array, size_t size, double width, double height, double min, double max, bool changeColor);
LAErrorCode LAFilterCompute(double *x, size_t nx, double *y, size_t ny, LAFilterValue *laf, LAFilterWindowWidget *windowWidget);
void LAOnFilterButtonWave(GtkWidget *widget, LAWaveformEditor *lae);

// preseteditor.c
LAErrorCode LADeletePresetDataNodes(LAPFDataNode **start);
LAErrorCode LAPresetFileInit(LAPresetFile *file);
LAErrorCode LAPresetUpdateUI(LAPresetEditor *lap);
void LAOpenPresetEditor(GtkWidget *widget, LAWindow *law);
LAErrorCode LAPresetParseFile(LAPresetFile *preset, LAMappedFile *file, uint8_t *flags);

// compiler/filereader.c
bool LAMappedFileIsValid(LAMappedFile *file);
bool LAMappedFileCanRead(LAMappedFile *file, size_t bytes);
size_t LAMappedFileGetFreeBytes(LAMappedFile *file);
LAErrorCode LAMappedFileReadBytes(LAMappedFile *file, uint8_t *output, size_t bytesRequest, size_t *bytesRead, bool strictSize);
LAErrorCode LAMappedFileSeekBytes(LAMappedFile *file, size_t bytesRequest, bool strictSize);
uint64_t LAParseIntFromArray(uint8_t *array, size_t size);
LAErrorCode LAMappedFileReadI8(LAMappedFile *file, uint8_t *output);
LAErrorCode LAMappedFileReadI16(LAMappedFile *file, uint16_t *output);
LAErrorCode LAMappedFileReadI32(LAMappedFile *file, uint32_t *output);
LAErrorCode LAMappedFileReadI64(LAMappedFile *file, uint64_t *output);

// filterwindowfunc.c
LAErrorCode LAFilterGenerateWindowArray(double *buffer, size_t size, LAFilterWindowType windowType, double *params, size_t paramCount);
LAErrorCode LAFilterGenerateRectWindow(double *array, size_t size, double *params, size_t paramCount);
LAErrorCode LAFilterGenerateTriWindow(double *array, size_t size, double *params, size_t paramCount);
LAErrorCode LAFilterGenerateWelchWindow(double *array, size_t size, double *params, size_t paramCount);
LAErrorCode LAFilterGenerateHannWindow(double *array, size_t size, double *params, size_t paramCount);
LAErrorCode LAFilterGenerateHammingWindow(double *array, size_t size, double *params, size_t paramCount);
LAErrorCode LAFilterGenerateTrapzWindow(double *array, size_t size, double *params, size_t paramCount);
LAErrorCode LAFilterGenerateCircWindow(double *array, size_t size, double *params, size_t paramCount);
LAErrorCode LAFilterGenerateSincWindow(double *array, size_t size, double *params, size_t paramCount);
LAErrorCode LAFilterGenerateImpulseWindow(double *array, size_t size, double *params, size_t paramCount);
LAErrorCode LAFilterGenerateBlackmanWindow(double *array, size_t size, double *params, size_t paramCount);
LAErrorCode LAFilterGenerateBlackmanHarrisWindow(double *array, size_t size, double *params, size_t paramCount);
LAErrorCode LAFilterGenerateKaiserWindow(double *array, size_t size, double *params, size_t paramCount);
LAErrorCode LAFilterGenerateGaussianWindow(double *array, size_t size, double *params, size_t paramCount);
LAErrorCode LAFilterGenerateNutallWindow(double *array, size_t size, double *params, size_t paramCount);
LAErrorCode LAFilterGenerateFlattopWindow(double *array, size_t size, double *params, size_t paramCount);
LAErrorCode LAFilterGenerateParzenWindow(double *array, size_t size, double *params, size_t paramCount);
LAErrorCode LAFilterGenerateCosineSumWindow(double *array, size_t size, double *params, size_t paramCount);
LAErrorCode LAFilterGenerateBlackmanNutallWindow(double *array, size_t size, double *params, size_t paramCount);
LAErrorCode LAFilterGenerateSinePowerWindow(double *array, size_t size, double *params, size_t paramCount);
LAErrorCode LAFilterGenerateApproxGaussianWindow(double *array, size_t size, double *params, size_t paramCount);
LAErrorCode LAFilterGenerateTukeyWindow(double *array, size_t size, double *params, size_t paramCount);
LAErrorCode LAFilterGeneratePlankTapperWindow(double *array, size_t size, double *params, size_t paramCount);
LAErrorCode LAFilterGeneratePoissonWindow(double *array, size_t size, double *params, size_t paramCount);
LAErrorCode LAFilterGenerateLanczosWindow(double *array, size_t size, double *params, size_t paramCount);
LAErrorCode LAFilterGenerateNoiseWindow(double *array, size_t size, double *params, size_t paramCount);
LAErrorCode LAFilterGenerateLogisticalWindow(double *array, size_t size, double *params, size_t paramCount);
LAErrorCode LAFilterGenerateLogistical2Window(double *array, size_t size, double *params, size_t paramCount);
LAErrorCode LAFilterGenerateDampedWindow(double *array, size_t size, double *params, size_t paramCount);
LAErrorCode LAFilterGenerateGaussineWindow(double *array, size_t size, double *params, size_t paramCount);
LAErrorCode LAFilterGeneratePolyChebyshevWindow(double *array, size_t size, double *params, size_t paramCount);
LAErrorCode LAFilterGenerateSmoothTrapezoidalWindow(double *array, size_t size, double *params, size_t paramCount);
LAErrorCode LAFilterGenerateRootChebyshevSmoothWindow(double *array, size_t size, double *params, size_t paramCount);
LAErrorCode LAFilterGenerateCompactSineWindow(double *array, size_t size, double *params, size_t paramCount);
LAErrorCode LAFilterWindowComboBox(GtkWidget **widget);

// filteriir.c
LAErrorCode LAFilterIIRCompile(double *x, size_t nx, double *y, size_t ny, double *a, size_t na, double *b, size_t nb);
LAErrorCode LAFilterMovingAverageCompile(double *x, size_t nx, double *y, size_t ny, size_t l);

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

// linspace.c
LAErrorCode LALinspace(double *x, double min, double max, size_t points);

#endif // LIB_LOGIC_ANALYZER_H
