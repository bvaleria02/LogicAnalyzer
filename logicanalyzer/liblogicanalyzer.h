#ifndef LIB_LOGIC_ANALYZER_H
#define LIB_LOGIC_ANALYZER_H

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

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

typedef struct {
	uint8_t *data;
	size_t capacity;
	size_t length;
	size_t readOffset;
} LAMappedFile;

#define LA_FIR_FILTER_PARAMS 3
#define GRAPH_MINI_WIDTH 	256
#define GRAPH_MINI_HEIGHT 	128

#define GRAPH_WIDTH 	768
#define GRAPH_HEIGHT 	192

#ifndef LAWindow
	typedef struct _la_window LAWindow;
#endif

#ifndef LAErrorCode
	typedef enum _la_error_code LAErrorCode;
#endif

#ifndef LABucket
	typedef struct _la_bucket LABucket;
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

#ifndef LABufferSelect
  typedef enum _la_buffer_select LABufferSelect;
#endif

#ifndef LAACK
  typedef struct _la_ack LAACK;
#endif

extern double defaultChannelColors[MAX_CHANNEL_COUNT][RGB_COUNT];
extern double defaultMidLineColor[RGB_COUNT];
extern const char validClockNames[CLK_NAMES_COUNT][CLK_NAMES_LENGTH];

#define CAIRO_COLOR_FROM_ARRAY(_cr, _a) cairo_set_source_rgb(_cr, _a[0], _a[1], _a[2])
#define CONVERT_INT_TO_GPOINTER(__value) ((gpointer) ((uintptr_t) ((uint8_t)__value))) 
#define CONVERT_GPOINTER_TO_INT(__value) ((uint8_t) ((uintptr_t)__value))

extern LAWindow *lawp;

#endif // LIB_LOGIC_ANALYZER_H
