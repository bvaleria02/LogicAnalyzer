#ifndef LA_HEX_VIEW
#define LA_HEX_VIEW

#include "types.h"
#include "error.h"
#include <stddef.h>
#include <stdint.h>

typedef LAErrorCode (*LAHexDumpCallback)(void *, size_t, size_t, uint8_t *, size_t, size_t*);

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

// HexView.c
LAErrorCode LACreateHexView(LAWindow *law, void *buffer, size_t size, LAHexDumpCallback callback, pthread_mutex_t *mutex);
LAErrorCode LACallbackHexViewSingleBucket(void *src, size_t srcSize, size_t offset, uint8_t *viewBuffer, size_t viewSize, size_t *bytesRead);
LAErrorCode LACallbackCircularBufferView(void *src, size_t srcSize, size_t offset, uint8_t *viewBuffer, size_t viewSize, size_t *bytesRead);
LAErrorCode LACallbackNormalBuffer(void *src, size_t srcSize, size_t offset, uint8_t *viewBuffer, size_t viewSize, size_t *bytesRead);
LAErrorCode LACallbackFileView(void *src, size_t srcSize, size_t offset, uint8_t *viewBuffer, size_t viewSize, size_t *bytesRead);

#endif //LA_HEX_VIEW
