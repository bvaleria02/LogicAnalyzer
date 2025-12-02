#include <gtk/gtk.h>
#include <cairo.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "liblogicanalyzer.h"
#include <string.h>
#include <time.h>

LABucket *LACreateBucket(){
	LABucket *bucket = (LABucket *)malloc(sizeof(LABucket));
	if(bucket == NULL)	goto handle_error;

	bucket->data		= (uint8_t *)malloc(sizeof(uint8_t) * DEFAULT_BUCKET_CAPACITY);
	if(bucket->data == NULL)	goto handle_error;

	bucket->length 		= DEFAULT_BUCKET_LENGTH;
	bucket->capacity 	= DEFAULT_BUCKET_CAPACITY;
	bucket->next		= NULL;
	g_print("Bucket created at %p\n", bucket);
	return bucket;

handle_error:
	if(bucket != NULL && bucket->data != NULL){
		free(bucket->data);
		bucket->data = NULL;
	}

	if(bucket != NULL){
		free(bucket);
	}

	bucket = NULL;
	LA_RAISE_ERROR(LA_ERROR_MALLOC);
	return NULL;
}

LAErrorCode LABucketInsertData(LABucket **bucketp, uint8_t data){
	LA_HANDLE_NULLPTR(bucketp, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR((*bucketp), LA_PROPAGATE_ERROR);

	LABucket *bucket 	= (*bucketp);
	LABucket *newBucket = NULL ;

	if(bucket->length >= bucket->capacity){
		newBucket = LACreateBucket();
		LA_HANDLE_NULLPTR(newBucket, LA_PROPAGATE_ERROR);
		newBucket->next = bucket->next;
		bucket->next = newBucket;
		bucket = newBucket;
	}

	bucket->data[bucket->length] = data;
	bucket->length += 1;
	(*bucketp) = bucket;
	return LA_NO_ERROR;
}

int64_t LABucketGetSizeAll(LABucket *bucketStart){
	LA_HANDLE_NULLPTR(bucketStart, LA_VALUE_ERROR);

	LABucket *bucket = bucketStart;
	uint64_t size = 0;

	while(bucket != NULL){
		size += bucket->length;
		bucket = bucket->next;
	}

	return size;
}

LAErrorCode LABucketDestroy(LABucket *bucket){
	LA_HANDLE_NULLPTR(bucket, LA_PROPAGATE_ERROR);

	if(bucket->data != NULL){
		free(bucket->data);
		bucket->data = NULL;
	}

	if(bucket != NULL){
		free(bucket);
	}

	return LA_NO_ERROR;
}

LAErrorCode LABucketDestroyAll(LABucket *bucketStart){
	LA_HANDLE_NULLPTR(bucketStart, LA_PROPAGATE_ERROR);

	LABucket *bucket = bucketStart;
	LABucket *next = NULL;;
	LAErrorCode response;

	while(bucket != NULL){
		next = bucket->next;
		response = LABucketDestroy(bucket);
		g_print("LABucketDestroyAll | Response: %i\n", response);
		bucket = next;
	}

	return LA_NO_ERROR;
}

LAErrorCode LABucketView(LABucket *bucket){
	LA_HANDLE_NULLPTR(bucket, LA_PROPAGATE_ERROR);

	for(uint32_t i = 0; i < (bucket->length >> 4); i++){
		printf("%08X : ", i << 4);
		for(uint8_t j = 0; j < 0x10; j++){
			if(((i << 4) + j) >= bucket->length){
				break;
			}
			printf("%02X ", bucket->data[(i << 4) + j]);
		}
		printf("\n");
	}

	return LA_NO_ERROR;
}

LAErrorCode LABucketViewAll(LABucket *bucketStart){
	LA_HANDLE_NULLPTR(bucketStart, LA_PROPAGATE_ERROR);
	LABucket *bucket = bucketStart;

	while(bucket != NULL){
		LABucketView(bucket);
		bucket = bucket->next;
	}
	
	return LA_NO_ERROR;
}

LAErrorCode LAWriteBucketsToFileBinary(LAWindow *law, gchar *filename){
	LA_HANDLE_NULLPTR(law, 		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(filename, LA_PROPAGATE_ERROR);

	LABucket *bucket = law->bd.bucketStart;
	if(bucket == NULL){
		LA_RAISE_ERROR(LA_ERROR_NOBUCKET);
		return LA_ERROR_NOBUCKET;
	}


	FILE *fp = fopen(filename, "wb+");
	if(fp == NULL){
		LA_RAISE_ERROR(LA_ERROR_FILE);
		return LA_ERROR_FILE;
	}

	while(bucket != NULL){
		fwrite(bucket->data, bucket->length, sizeof(uint8_t), fp);
		bucket = bucket->next;
	}

	fclose(fp);
	return LA_NO_ERROR;
}

LAErrorCode LAWriteBucketsToFileCSV(LAWindow *law, gchar *filename){
	LA_HANDLE_NULLPTR(law, 		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(filename, LA_PROPAGATE_ERROR);

	LABucket *bucket = law->bd.bucketStart;
	if(bucket == NULL){
		LA_RAISE_ERROR(LA_ERROR_NOBUCKET);
		return LA_ERROR_NOBUCKET;
	}

	FILE *fp = fopen(filename, "w+");
	if(fp == NULL){
		LA_RAISE_ERROR(LA_ERROR_FILE);
		return LA_ERROR_FILE;
	}

	for(uint8_t i = 0; i < MAX_CHANNEL_COUNT; i++){
		fprintf(fp, "Channel%i", i);
		if(i < (MAX_CHANNEL_COUNT - 1)){
			fprintf(fp, ",");
		}
	}
	fprintf(fp, "\n");

	while(bucket != NULL){
		for(uint32_t i = 0; i < bucket->length; i++){
			for(uint8_t j = 0; j < 8; j++){
				fprintf(fp, "%1X", (bucket->data[i] >> j) & 0x1);
				if(j < 7){
					fprintf(fp, ",");
				}
			}
			fprintf(fp, "\n");
		}
		fwrite(bucket->data, bucket->length, sizeof(uint8_t), fp);
		bucket = bucket->next;
	}

	fclose(fp);
	return LA_NO_ERROR;
}

LAErrorCode LACallbackHexViewBucketAll(void *src, size_t srcSize, size_t offset, uint8_t *viewBuffer, size_t viewSize, size_t *bytesRead){
	LA_HANDLE_NULLPTR(src, 			LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(viewBuffer, 	LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(bytesRead, 	LA_PROPAGATE_ERROR);

	LABucket *bucket = (LABucket *)src;
	(*bytesRead) = 0;
	size_t lowerIndex = 0;
	size_t upperIndex = 0;
	while(bucket != NULL){
		upperIndex += bucket->length;
		if(offset >= lowerIndex && offset < upperIndex){
			break;
		}

		lowerIndex += bucket->length;
		bucket = bucket->next;
	}
	if(offset >= upperIndex){
		LA_RAISE_ERROR(LA_ERROR_OUTOFRANGE);
		return LA_ERROR_OUTOFRANGE;
	}

	g_print("Min: %016lx\tMax: %016lx\tOffset: %016lx\n", lowerIndex, upperIndex, offset);
	size_t bucketIndex = offset - lowerIndex;
	g_print("bufferIndex: %016lx\n", bucketIndex);

	for(size_t i = 0; i < viewSize; i++){
		if(bucketIndex >= bucket->length){
			bucketIndex = 0;
			bucket = bucket->next;
		}

		if(bucket == NULL){
			break;
		}
	
		viewBuffer[i] = bucket->data[bucketIndex];
		(*bytesRead) = (*bytesRead) + 1;
		bucketIndex += 1;
	}

	return LA_NO_ERROR;
}
