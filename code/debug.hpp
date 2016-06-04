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

	#define PRINT_BYTES_8(x)\
	do\
	{\
		uint8_t y = x;\
		for(uint8_t i = 0; i < 8; i++)\
		{\
			if(y & 0x80)\
				printf("1");\
			else\
				printf("0");\
			y <<= 1;\
		}\
		printf("\n");\
	}\
	while(0)

	#define PRINT_BYTES_16(x)\
	do\
	{\
		uint16_t y = htons(x);\
		for(uint8_t i = 0; i < 16; i++)\
		{\
			if(y & 0x8000)\
				printf("1");\
			else\
				printf("0");\
			y <<= 1;\
		}\
		printf("\n");\
	}\
	while(0)

	#define PRINT_BYTES_32(x)\
	do\
	{\
		uint32_t y = htonl(x);\
		for(uint8_t i = 0; i < 32; i++)\
		{\
			if(y & 0x80000000)\
				printf("1");\
			else\
				printf("0");\
			y <<= 1;\
		}\
		printf("\n");\
	}\
	while(0)

	#define PRINT_UINT_8(x)\
	do\
	{\
		printf("%" PRIu8 "\n", x);\
	}\
	while(0)

	#define PRINT_UINT_16(x)\
	do\
	{\
		printf("%" PRIu16 "\n", x);\
	}\
	while(0)

	#define PRINT_UINT_32(x)\
	do\
	{\
		printf("%" PRIu32 "\n", x);\
	}\
	while(0)
#else
	#define DEBUG(x)
	#define PRINT_BYTES_8(x)
	#define PRINT_BYTES_32(x)
	#define NDEBUG // wyłącza asercje z cassert
#endif

#endif // DEBUG_HPP_INCLUDED
