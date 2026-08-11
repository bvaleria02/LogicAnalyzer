#ifndef LA_BUCKET
#define LA_BUCKET

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifndef LABucket
	typedef struct _la_bucket LABucket;
#endif

struct _la_bucket {
	uint32_t length;
	uint32_t capacity;
	struct _la_bucket *next;
	uint8_t *data;
};

// bucket.c
LABucket *LACreateBucket();
LAErrorCode LABucketInsertData(LABucket **bucketp, uint8_t data);
int64_t LABucketGetSizeAll(LABucket *bucketStart);
LAErrorCode LABucketDestroy(LABucket *bucket);
LAErrorCode LABucketDestroyAll(LABucket *bucketStart);
LAErrorCode LABucketView(LABucket *bucket);
LAErrorCode LABucketViewAll(LABucket *bucketStart);
LAErrorCode LAWriteBucketsToFileBinary(LAWindow *law, gchar *filename);
LAErrorCode LAWriteBucketsToFileCSV(LAWindow *law, char *filename);
LAErrorCode LACallbackHexViewBucketAll(void *src, size_t srcSize, size_t offset, uint8_t *viewBuffer, size_t viewSize, size_t *bytesRead);

#endif //LA_BUCKET
