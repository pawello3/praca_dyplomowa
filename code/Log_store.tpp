/*
	Paweł Guzewicz, 263664
	Indywidualne Studia Informatyczno-Matematyczne, Uniwersytet Wrocławski
	Praca licencjacka: Analiza teoretyczna i porównawcza efektywnej pamięciowo,
	wysoce wydajnej tablicy asocjacyjnej SILT (Small Index Large Table)
*/

#include "debug.hpp"
#include <arpa/inet.h>
#include <cstring>
#include <type_traits>

#define LEFTROTATE_32(a, b)\
ntohl((((htonl(a) << (b))) | (htonl(a) >> (32-(b)))))

template<typename Key, typename Value>
void SILT::Log_store<Key, Value>::SHA_1(Key key, uint32_t key_size,
SILT::SILT_key* returned_key)
{
	returned_key->h0 = ntohl(0x67452301);
	returned_key->h1 = ntohl(0xEFCDAB89);
	returned_key->h2 = ntohl(0x98BADCFE);
	returned_key->h3 = ntohl(0x10325476);
	returned_key->h4 = ntohl(0xC3D2E1F0);

	uint32_t number_of_bits = key_size << 3;
	number_of_bits++; // dodawanie '1' na koniec
	if(number_of_bits % 512 < 448)
		number_of_bits += 448 - number_of_bits % 512;
	else if(number_of_bits % 512 > 448)
		number_of_bits += 512 - number_of_bits % 512 + 448;
	number_of_bits += 64;
	DEBUG(assert(number_of_bits % 512 == 0));

	uint8_t* array = new uint8_t[64];
	memset((void*) array, 0, 64); // zerowanie
	memcpy((void*) array, (const void*) key, key_size); /* kopiowanie klucza na
	początek tablicy */
	array[key_size] = 0x80; // ustawienie '1'
	uint64_t size = key_size << 3;
	array[62] = (size & 0xFFFFFFFF00000000) >> 32; // dodanie długości na koniec
	array[63] = size & 0x00000000FFFFFFFF; // dodanie długości na koniec

	uint32_t number_of_chunks = number_of_bits / 512;
	uint32_t** chunks = new uint32_t*[number_of_chunks];
	for(uint32_t i = 0; i < number_of_chunks; i++)
	{
		chunks[i] = new uint32_t[80]; // kopiowanie do 16 słów, reszta później
		memset(chunks[i], 0, 320);
		memcpy(chunks[i], (void*) (array + i * 64), 64);
	}

	uint32_t word;
	for(uint32_t i = 0; i < number_of_chunks; i++)
	{
		for(uint32_t j = 16; j < 80; j++) // roszerzanie do 80 słów
		{
			word = (((chunks[i][j - 3] ^ chunks[i][j - 8]) ^ chunks[i][j - 14])
			^ chunks[i][j - 16]);
			chunks[i][j] = LEFTROTATE_32(word, 1);
		}
	}

	uint32_t a, b, c, d, e, f, k, temp;
	for(uint8_t i = 0; i < number_of_chunks; i++) // iteracja po kawałkach
	{
		a = returned_key->h0;
		b = returned_key->h1;
		c = returned_key->h2;
		d = returned_key->h3;
		e = returned_key->h4;
		for(uint8_t j = 0; j < 20; j++) // główna pętla dla konkretnego kawałka
		{
			f = (b & c) | ((~b) & d);
			k = ntohl(0x5A827999);
			temp = ntohl(htonl(LEFTROTATE_32(a, 5)) + htonl(f) + htonl(e)
			+ htonl(k) + htonl(chunks[i][j]));
			e = d;
			d = c;
			c = LEFTROTATE_32(b, 30);
			b = a;
			a = temp;
		}
		for(uint8_t j = 20; j < 40; j++)
		{
			f = b ^ c ^ d;
			k = ntohl(0x6ED9EBA1);
			temp = ntohl(htonl(LEFTROTATE_32(a, 5)) + htonl(f) + htonl(e)
			+ htonl(k) + htonl(chunks[i][j]));
			e = d;
			d = c;
			c = LEFTROTATE_32(b, 30);
			b = a;
			a = temp;
		}
		for(uint8_t j = 40; j < 60; j++)
		{
			f = (b & c) | (b & d) | (c & d);
			k = ntohl(0x8F1BBCDC);
			temp = ntohl(htonl(LEFTROTATE_32(a, 5)) + htonl(f) + htonl(e)
			+ htonl(k) + htonl(chunks[i][j]));
			e = d;
			d = c;
			c = LEFTROTATE_32(b, 30);
			b = a;
			a = temp;
		}
		for(uint8_t j = 60; j < 80; j++)
		{
			f = b ^ c ^ d;
			k = ntohl(0xCA62C1D6);
			temp = ntohl(htonl(LEFTROTATE_32(a, 5)) + htonl(f) + htonl(e)
			+ htonl(k) + htonl(chunks[i][j]));
			e = d;
			d = c;
			c = LEFTROTATE_32(b, 30);
			b = a;
			a = temp;
		}
		returned_key->h0 = ntohl(htonl(a) + htonl(returned_key->h0));
		returned_key->h1 = ntohl(htonl(b) + htonl(returned_key->h1));
		returned_key->h2 = ntohl(htonl(c) + htonl(returned_key->h2));
		returned_key->h3 = ntohl(htonl(d) + htonl(returned_key->h3));
		returned_key->h4 = ntohl(htonl(e) + htonl(returned_key->h4));
	}

	delete[] array;
	for(uint32_t i = 0; i < number_of_chunks; i++)
		delete[] chunks[i];
	delete[] chunks;
}
