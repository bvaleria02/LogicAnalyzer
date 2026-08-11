#ifndef LA_ENUMS
#define LA_ENUMS

#ifndef LABufferSelect
  typedef enum _la_buffer_select LABufferSelect;
#endif

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

enum _la_buffer_select {
	LA_BUFFER_SELECT_VISIBLE	= 0,
	LA_BUFFER_SELECT_ALL		  = 1,
	LA_BUFFER_SELECT_CUSTOM		= 2,
	LA_BUFFER_SELECT_003		  = 3 
};


#endif //LA_ENUMS
