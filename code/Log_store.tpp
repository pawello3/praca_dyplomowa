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
#include <cstdlib>
#include <ctime>

#define LEFTROTATE_32(a, b)\
ntohl((((htonl(a) << (b))) | (htonl(a) >> (32-(b)))))

template<typename Key, typename Value>
SILT::Log_store<Key, Value>::Log_store(void)
	:log_store_file(fopen("Log_store.dat", "wb+"))
	,file_size(0)
{
	if(log_store_file == nullptr)
	{
		fprintf(stderr, "fopen error\n");
		exit(1);
	}
	strcpy(id, "Log_store.dat");
	hash_table = new Hash_table_entry*[hash_table_size];
	for(uint32_t i = 0; i < hash_table_size; i++)
	{
		hash_table[i] = new Hash_table_entry[bucket_size];
		memset(hash_table[i], 0, sizeof(hash_table[0][0]) * bucket_size);
	}
	overload.tag = 0;
	overload.offset = 0;
}

template<typename Key, typename Value>
SILT::Log_store<Key, Value>::~Log_store(void)
{
	for(uint32_t i = 0; i < hash_table_size; i++)
		delete[] hash_table[i];
	delete[] hash_table;
	fclose(log_store_file);
	if(std::remove(id) != 0)
	{
		fprintf(stderr, "remove error\n");
		exit(1);
	}
}

template<typename Key, typename Value>
const Value* SILT::Log_store<Key, Value>::operator[](Key key) const
{
}

template<typename Key, typename Value>
bool SILT::Log_store<Key, Value>::insert(Key key, Value value) /* haszowanie
kukułcze dla częściowych kluczy */
{
	SILT_key sha_key;
	SHA_1(key, sizeof(key), &sha_key);
	uint16_t h1 = ntohs(htonl(sha_key.h4) & 0x0000FFFE);
	uint16_t h2 = ntohs((htonl(sha_key.h4) & 0xFFFE0000) >> 16);

	fseek(log_store_file, file_size, SEEK_SET);
	uint32_t log_file_offset = file_size;
	file_size += sizeof(Key) + sizeof(Value);
	fwrite(&key, sizeof(Key), 1, log_store_file);
	fwrite(&value, sizeof(Value), 1, log_store_file);

	srand(time(nullptr));
	uint8_t number;
	uint16_t tmp_h1;
	uint16_t tmp_h2;
	uint32_t tmp_log_file_offset;
	for(uint8_t number_of_displacements = 0;
	number_of_displacements < maximum_number_of_displacements;
	number_of_displacements++)
	{
		if(insert_into_buckets(h1, h2, log_file_offset) == false)
		{
			number = rand() % 8;
			if(number % 2 == 0)
			{
				tmp_h2 = hash_table[h1][number >> 1].tag;
				tmp_log_file_offset = hash_table[h1][number >> 1].offset;
				hash_table[h1 >> 1][number >> 1].tag = h2;
				hash_table[h1 >> 1][number >> 1].offset = log_file_offset;
				h2 = tmp_h2;
				log_file_offset = tmp_log_file_offset;
			}
			else
			{
				tmp_h1 = hash_table[h2][number >> 1].tag;
				tmp_log_file_offset = hash_table[h2][number >> 1].offset;
				hash_table[h2 >> 1][number >> 1].tag = h1;
				hash_table[h2 >> 1][number >> 1].offset = log_file_offset;
				h1 = tmp_h1;
				log_file_offset = tmp_log_file_offset;
			}
		}
		else
		{
			return true;
		}
	}
	overload.offset = log_file_offset;
	return false;
}

template<typename Key, typename Value>
bool SILT::Log_store<Key, Value>::insert_into_buckets(uint16_t h1, uint32_t h2,
uint32_t log_file_offset)
{
	uint8_t number = 0;
	while(number < 8)
	{
		if(number % 2 == 0) // h1
		{
			if((hash_table[h1 >> 1][number >> 1].tag & 0x0001) == 0)
			{
				hash_table[h1 >> 1][number >> 1].tag = h2 | 0x0001;
				hash_table[h1 >> 1][number >> 1].offset = log_file_offset;
				return true;
			}
		}
		else // h2
		{
			if((hash_table[h2 >> 1][number >> 1].tag & 0x0001) == 0)
			{
				hash_table[h2 >> 1][number >> 1].tag = h1 | 0x0001;
				hash_table[h2 >> 1][number >> 1].offset = log_file_offset;
				return true;
			}
		}
		number++;
	}
	return false;
}

template<typename Key, typename Value>
void SILT::Log_store<Key, Value>::remove(Key key)
{
}

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
	assert(number_of_bits % 512 == 0);

	uint8_t* array = new uint8_t[64];
	memset((void*) array, 0, 64); // zerowanie
	memcpy((void*) array, (const void*) &key, key_size); /* kopiowanie klucza na
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
