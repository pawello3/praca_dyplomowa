/*
	Paweł Guzewicz, 263664
	Indywidualne Studia Informatyczno-Matematyczne, Uniwersytet Wrocławski
	Praca licencjacka: Analiza teoretyczna i porównawcza efektywnej pamięciowo,
	wysoce wydajnej tablicy asocjacyjnej SILT (Small Index Large Table)
*/

#ifndef DEBUG_HPP_INCLUDED
#define DEBUG_HPP_INCLUDED

#include <cassert>
#include <arpa/inet.h>

//#define DEBUG_MODE
#ifdef DEBUG_MODE
	#define DEBUG(x)\
	do\
	{\
		fprintf(stderr, "Debug at line %d in function %s\n", __LINE__,\
		__func__);\
		x;\
		fprintf(stderr, "End of debug at line %d in function %s\n", __LINE__,\
		__func__);\
	}\
	while(0)

	#define PRINT_TEXT(x)\
	do\
	{\
		fprintf(stderr, "%s\n", x);\
	}\
	while(0)

	#define PRINT_BYTES_8(x)\
	do\
	{\
		uint8_t y = x;\
		for(uint8_t i = 0; i < 8; i++)\
		{\
			if(y & 0x80)\
				fprintf(stderr, "1");\
			else\
				fprintf(stderr, "0");\
			y <<= 1;\
		}\
		fprintf(stderr, "\n");\
	}\
	while(0)

	#define PRINT_BYTES_16(x)\
	do\
	{\
		uint16_t y = htons(x);\
		for(uint8_t i = 0; i < 16; i++)\
		{\
			if(y & 0x8000)\
				fprintf(stderr, "1");\
			else\
				fprintf(stderr, "0");\
			y <<= 1;\
		}\
		fprintf(stderr, "\n");\
	}\
	while(0)

	#define PRINT_BYTES_32(x)\
	do\
	{\
		uint32_t y = htonl(x);\
		for(uint8_t i = 0; i < 32; i++)\
		{\
			if(y & 0x80000000)\
				fprintf(stderr, "1");\
			else\
				fprintf(stderr, "0");\
			y <<= 1;\
		}\
		fprintf(stderr, "\n");\
	}\
	while(0)

	#define PRINT_UINT_8(x)\
	do\
	{\
		fprintf(stderr, "%" PRIu8 "\n", x);\
	}\
	while(0)

	#define PRINT_UINT_16(x)\
	do\
	{\
		fprintf(stderr, "%" PRIu16 "\n", x);\
	}\
	while(0)

	#define PRINT_UINT_32(x)\
	do\
	{\
		fprintf(stderr, "%" PRIu32 "\n", x);\
	}\
	while(0)

	#define PRINT_SILT_KEY(x)\
	do\
	{\
		fprintf(stderr, "%010" PRIu32 " %010" PRIu32 " %010" PRIu32 " %010"\
		PRIu32 " %010" PRIu32 "\n", x.h0, x.h1, x.h2, x.h3, x.h4);\
	}\
	while(0)

#else
	#define DEBUG(x)
	#define PRINT_BYTES_8(x)
	#define PRINT_BYTES_32(x)
	#define NDEBUG // wyłącza asercje z cassert
#endif

#endif // DEBUG_HPP_INCLUDED
