#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../liblogicanalyzer.h"
#include "../error.h"
#include "../utils.h"
#include "matrix.h"

LAErrorCode LAMatPrint(LAMat_t *mat){
	LA_HANDLE_NULLPTR(mat, LA_PROPAGATE_ERROR);

	LAErrorCode code = LAMatPrintBackend(mat, LA_MAT_PRINT_SHOW_META | LA_MAT_PRINT_SHOW_DATA | LA_MAT_PRINT_SHOW_VIEWDATA, NULL);
	return code;
}

LAErrorCode LAMatPrintWithLabel(LAMat_t *mat, char *label){
	LA_HANDLE_NULLPTR(mat, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(label, LA_PROPAGATE_ERROR);

	LAErrorCode code = LAMatPrintBackend(mat, LA_MAT_PRINT_SHOW_META | LA_MAT_PRINT_SHOW_DATA | LA_MAT_PRINT_SHOW_LABEL | LA_MAT_PRINT_SHOW_VIEWDATA, label);
	return code;
}

LAErrorCode LAMatPrintBackend(LAMat_t *mat, LAMatPrintFlags flags, char *label){
	LA_HANDLE_NULLPTR(mat, LA_PROPAGATE_ERROR);

	LAErrorCode code = LA_NO_ERROR;

	size_t width = LAMatGetWidth(mat);
	if(width == LA_MAT_INDEX_FAIL) return LA_ERROR_MATRIX;

	size_t height = LAMatGetHeight(mat);
	if(height == LA_MAT_INDEX_FAIL) return LA_ERROR_MATRIX;

	if(label != NULL && LA_MAT_PRINT_SHOW_LABEL){
		printf("==================\n");
		printf("\t%s\n", label);
		printf("==================\n");
	}

	if(flags & LA_MAT_PRINT_SHOW_META){
		printf("Flags: %02X\t", mat->flags);
		printf("Width: %lu\t", width);
		printf("Height: %lu\n", height);
		if(flags & LA_MAT_PRINT_SHOW_VIEWDATA){
			printf("View data: \t");
			printf("Row offset: %lu\t", mat->viewData.rowOffset);
			printf("Col offset: %lu\t", mat->viewData.colOffset);
			printf("View Height: %lu\t", mat->viewData.virtualHeight);
			printf("View Width: %lu\t", mat->viewData.virtualWidth);
			printf("Real Height: %lu\t", mat->row);
			printf("Real Width: %lu\n", mat->col);
		}
	}

	double value = 0;

	if(flags & LA_MAT_PRINT_SHOW_DATA){
		printf("Data: [\n");
		for(size_t r = 0; r < height; r++){
			printf("\t");
			for(size_t c = 0; c < width; c++){
				code = LAMatGet(mat, r, c, &value);
				if(code){
					printf("Error\n");
					break;
				}
				printf("%0.4lf\t", value);
			}
			printf("\n");
		}
		printf("]\n");
	}

	return LA_NO_ERROR;
}
