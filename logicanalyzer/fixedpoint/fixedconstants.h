/*
 * This file is compile-time generated
 * Don't bother deleting it, it will re-generate.
 */

#ifndef FIXED_CONSTANTS_H
#define FIXED_CONSTANTS_H 1

#define RECIPROCAL_F32_COUNT 40
extern const ufixed32_t reciprocalF32[RECIPROCAL_F32_COUNT];

#define RECIPROCAL_F64_COUNT 40
extern const ufixed64_t reciprocalF64[RECIPROCAL_F64_COUNT];

#define RECIPROCAL_2_F32_COUNT 40
extern const ufixed32_t reciprocal2F32[RECIPROCAL_2_F32_COUNT];

#define RECIPROCAL_2_F64_COUNT 40
extern const ufixed64_t reciprocal2F64[RECIPROCAL_2_F64_COUNT];

#define RECIPROCAL_FACT_F32_COUNT 12
extern const ufixed32_t reciprocalFactF32[RECIPROCAL_FACT_F32_COUNT];

#define RECIPROCAL_FACT_F64_COUNT 12
extern const ufixed64_t reciprocalFactF64[RECIPROCAL_FACT_F64_COUNT];

#define ARCTAN_F32_COUNT 24
extern const ufixed32_t arctanF32[ARCTAN_F32_COUNT];

#define ARCTAN_F64_COUNT 24
extern const ufixed64_t arctanF64[ARCTAN_F64_COUNT];

#define TAN_F32_SHIFT 10
#define TAN_F32_COUNT 1609
extern const ufixed32_t tanLUTF32[TAN_F32_COUNT];

#define TAN_F64_SHIFT 10
#define TAN_F64_COUNT 1609
extern const ufixed64_t tanLUTF64[TAN_F64_COUNT];

#define BIT_LENGTH_COUNT 256
extern const uint8_t bitLengthLUT[BIT_LENGTH_COUNT];

#define LOWEST_BIT_COUNT 256
extern const uint8_t lowestBitLUT[LOWEST_BIT_COUNT];

#endif // FIXED_CONSTANTS_H
