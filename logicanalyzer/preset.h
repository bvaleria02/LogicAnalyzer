#ifndef LA_PRESET
#define LA_PRESET

#include "error.h"
#include "types.h"
#include <stdint.h>
#include <stddef.h> 

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

struct _la_preset_file {
	LAPFHeader header;
	uint32_t dataLength;
	LAPFDataNode *dataStart;
	LAPFDataNode *dataLast;
};


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


// preseteditor.c
LAErrorCode LADeletePresetDataNodes(LAPFDataNode **start);
LAErrorCode LAPresetFileInit(LAPresetFile *file);
LAErrorCode LAPresetUpdateUI(LAPresetEditor *lap);
void LAOpenPresetEditor(GtkWidget *widget, LAWindow *law);
LAErrorCode LAPresetParseFile(LAPresetFile *preset, LAMappedFile *file, uint8_t *flags);

#endif //LA_PRESET
