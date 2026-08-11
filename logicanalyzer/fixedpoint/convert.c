#include "fixedpoint.h"

fixed32_t convertFloatToF32(float x){
	return (fixed32_t)(x * F32_SCALE);
}

fixed64_t convertFloatToF64(float x){
	return (fixed64_t)(x * F64_SCALE);
}

ufixed32_t convertFloatToUF32(float x){
	return (ufixed32_t)(x * F32_SCALE);
}

ufixed64_t convertFloatToUF64(float x){
	return (ufixed64_t)(x * F64_SCALE);
}

fixed32_t convertDoubleToF32(double x){
	return (fixed32_t)(x * F32_SCALE);
}

fixed64_t convertDoubleToF64(double x){
	return (fixed64_t)(x * F64_SCALE);
}

ufixed32_t convertDoubleToUF32(double x){
	return (ufixed32_t)(x * F32_SCALE);
}

ufixed64_t convertDoubleToUF64(double x){
	return (ufixed64_t)(x * F64_SCALE);
}

fixed32_t convertIntToF32(int32_t x, int8_t point){
	uint64_t sgn      = GET_SIGN(x, I32_SIGN);
	uint64_t mantissa = GET_MANT(x, I32_MANTISSA);
	fixed32_t fixed   = 0;
	ASSEMBLE_FIXED(F32_SIZE, sgn, mantissa, point, fixed, I32_MANTISSA);
	return fixed;
}

fixed64_t convertIntToF64(int64_t x, int8_t point){
	uint64_t sgn      = GET_SIGN(x, I64_SIGN);
	uint64_t mantissa = GET_MANT(x, I64_MANTISSA);
	fixed64_t fixed   = 0;
	ASSEMBLE_FIXED(F64_SIZE, sgn, mantissa, point, fixed, I64_MANTISSA);
	return fixed;
}

ufixed32_t convertIntToUF32(uint32_t x, int8_t point){
	fixed32_t fixed   = 0;
	ASSEMBLE_UFIXED(F32_SIZE, x, point, fixed);
	return fixed;
}

ufixed64_t convertIntToUF64(uint64_t x, int8_t point){
	fixed64_t fixed   = 0;
	ASSEMBLE_UFIXED(F64_SIZE, x, point, fixed);
	return fixed;
}

float convertF32ToFloat(fixed32_t x){
	return (float) ((int32_t) x / (float) F32_SCALE);
}

float convertF64ToFloat(fixed64_t x){
	return (float) ((int64_t) x / (float) F64_SCALE);
}

float convertUF32ToFloat(ufixed32_t x){
	return (float) ((uint32_t) x / (float) F32_SCALE);
}

float convertUF64ToFloat(ufixed64_t x){
	return (float) ((uint64_t) x / (float) F64_SCALE);
}

double convertF32ToDouble(fixed32_t x){
	return (double) ((int32_t) x / (double) F32_SCALE);
}

double convertF64ToDouble(fixed64_t x){
	return (double) ((int64_t) x / (double) F64_SCALE);
}

double convertUF32ToDouble(ufixed32_t x){
	return (double) ((uint32_t) x / (double) F32_SCALE);
}

double convertUF64ToDouble(ufixed64_t x){
	return (double) ((uint64_t) x / (double) F64_SCALE);
}

int32_t convertF32ToInt(fixed32_t x, int8_t point){
	return (int32_t) x / (F32_SCALE >> point);
}

uint32_t convertUF32ToInt(ufixed32_t x, int8_t point){
	return (uint32_t) x / (F32_SCALE >> point);
}

int64_t convertF64ToInt(fixed64_t x, int8_t point){
	return (int64_t) x / (F64_SCALE >> point);
}

uint64_t convertUF64ToInt(ufixed64_t x, int8_t point){
	return (uint64_t) x / (F64_SCALE >> point);
}
