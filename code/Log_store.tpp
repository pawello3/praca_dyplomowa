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
	:log_entry_size(sizeof(Key) + sizeof(Value))
	,log_store_file(fopen("Log_store.dat", "wb+"))
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
bool SILT::Log_store<Key, Value>::insert(const Key& key, const Value& value)
/* haszowanie kukułcze dla częściowych kluczy */
{
	return insert(key, value, true);
}

template<typename Key, typename Value>
bool SILT::Log_store<Key, Value>::insert(const Key& key, const Value& value,
bool operation)
{
	SILT_key sha_key;
	SHA_1(key, sizeof(key), &sha_key);
	uint16_t h1 = sha_key.h4 & h1_mask;
	uint16_t h2 = (sha_key.h4 & h2_mask) >> 16;

	/*DEBUG(PRINT_BYTES_32(sha_key.h4));
	DEBUG(PRINT_BYTES_16(h1));
	DEBUG(PRINT_BYTES_16(h2));*/

	uint32_t log_file_offset = file_size;
	if(insert_into_buckets(h1, h2, log_file_offset, operation) == true)
	{
		fseek(log_store_file, file_size * log_entry_size, SEEK_SET);
		fwrite((void*) &key, sizeof(Key), 1, log_store_file);
		fwrite((void*) &value, sizeof(Value), 1, log_store_file);
		file_size++;
		return true;
	}
	/*
		Od teraz
		Element X, który nie ma jeszcze swojej pozycji (wyrzucony), znajduje się
		w schowku reprezentowanym jako:
			h1 - pierwszy kubełek (tam go odsyłamy) dla elementu wyrzuconego
			h2 - kubełek alternatywny (znajdzie się w znaczniku)
			log_file_offset - przesunięcie związane z tym elementem

		Wiemy, że nie mieści się on do żadnego miejsca w obu kubełkach. Losujemy
		jedno z tych miejsc i go tam wstawiamy. A wyrzucony w ten sposób
		element zapamiętujemy w schowku.

		Pomocniczne zmienne dla właśnie wyrzucanego elementu Y:
			tmp_h1 - pierwszy kubełek (tam go odsyłamy) dla elementu wyrzuconego
			tmp_h2 - kubełek alternatywny (znajdzie się w znaczniku)
			tmp_log_file_offset - przesunięcie związane z tym elementem

		Po wstawieniu elementu X ze schowka na jego miejsce, umieszczamy w nim
		element Y.
	*/

	h1 = ((operation == true) ? (h1 | operation_bit) : (h1 & ~operation_bit))
	| valid_bit;
	h2 = ((operation == true) ? (h2 | operation_bit) : (h2 & ~operation_bit))
	| valid_bit;
	srand(time(nullptr));
	uint8_t number = rand() % (bucket_size << 1);
	uint16_t tmp_h1;
	uint16_t tmp_h2;
	uint32_t tmp_log_file_offset;
	Undo_entry undo[maximum_number_of_displacements];
	for(uint8_t i = 0; i < maximum_number_of_displacements; i++)
	{
		if(number % 2 == 0) // kubełek h1
		/* poprzednio wyrzuciliśmy element z kubełka h2 i chcieliśmy go wrzucić
		do h1, ale się nie udało, więc wrzucamy na siłę i wyrzucamy inny */
		{
			tmp_h1 = hash_table[h1 >> 2][number >> 1].tag;
			tmp_h2 = h1;
			tmp_log_file_offset = hash_table[h1 >> 2][number >> 1].offset;
			hash_table[h1 >> 2][number >> 1].tag = h2;
			hash_table[h1 >> 2][number >> 1].offset = log_file_offset;
			undo[i].h = h1 >> 2;
			undo[i].number = number >> 1;
			undo[i].which = false;
			number = 1;
		}
		else // kubełek h2
		/* poprzednio wyrzuciliśmy element z kubełka h1 i chcieliśmy go wrzucić
		do h2, ale się nie udało, więc wrzucamy na siłę i wyrzucamy inny */
		{
			tmp_h1 = h2;
			tmp_h2 = hash_table[h2 >> 2][number >> 1].tag;
			tmp_log_file_offset = hash_table[h2 >> 2][number >> 1].offset;
			hash_table[h2 >> 2][number >> 1].tag = h1;
			hash_table[h2 >> 2][number >> 1].offset = log_file_offset;
			undo[i].h = h2 >> 2;
			undo[i].number = number >> 1;
			undo[i].which = true;
			number = 0;
		}
		h1 = tmp_h1;
		h2 = tmp_h2;
		log_file_offset = tmp_log_file_offset;
		if(insert_into_bucket(h1, h2, log_file_offset, operation) == true)
		{
			fseek(log_store_file, file_size * log_entry_size, SEEK_SET);
			fwrite((void*) &key, sizeof(Key), 1, log_store_file);
			fwrite((void*) &value, sizeof(Value), 1, log_store_file);
			file_size++;
			return true;
		}
		number += (rand() % bucket_size) << 1;
	}
	DEBUG(fprintf(stderr, "Przepełnienie, cofanie wstawienia\n"));
	assert(false); // TODO
	for(uint8_t i = maximum_number_of_displacements; i > 0; i--)
	{
		tmp_h1 = h1;
		tmp_h2 = h2;
		tmp_log_file_offset = hash_table[undo[i - 1].h][undo[i - 1].number].offset;
		if(undo[i].which == 0)
		{
			tmp_h2 = hash_table[undo[i - 1].h][undo[i - 1].number].tag;
			hash_table[undo[i - 1].h][undo[i - 1].number].tag = h1;
		}
		else
		{
			tmp_h1 = hash_table[undo[i - 1].h][undo[i - 1].number].tag;
			hash_table[undo[i - 1].h][undo[i - 1].number].tag = h2;
		}
		hash_table[undo[i - 1].h][undo[i - 1].number].offset = log_file_offset;
		h1 = tmp_h1;
		h2 = tmp_h2;
		log_file_offset = tmp_log_file_offset;

        //printf("%" PRIu16 " %" PRIu8 "\n", undo[i - 1].h, undo[i - 1].number);
	}
	assert((h1 & 0xFFFC) == (sha_key.h4 & h1_mask));
	assert((h2 & 0xFFFC) == ((sha_key.h4 & h2_mask) >> 16));
	assert(log_file_offset == file_size);
	// jeśli asercje nawalą, to znaczy, że źle odtwarzano
	return false;
}

template<typename Key, typename Value>
bool SILT::Log_store<Key, Value>::insert_into_buckets(uint16_t h1, uint32_t h2,
uint32_t log_file_offset, bool operation)
{
	uint8_t number = 0;
	while(number < (bucket_size << 1))
	{
		if(number % 2 == 0) // kubełek h1
		{
			if((hash_table[h1 >> 2][number >> 1].tag & valid_bit) == 0)
			{
				hash_table[h1 >> 2][number >> 1].tag
				= ((operation == true) ? (h2 | operation_bit)
				: (h2 & ~operation_bit)) | valid_bit;
				hash_table[h1 >> 2][number >> 1].offset = log_file_offset;
				return true;
			}
		}
		else // kubełek h2
		{
			if((hash_table[h2 >> 2][number >> 1].tag & valid_bit) == 0)
			{
				hash_table[h2 >> 2][number >> 1].tag
				= ((operation == true) ? (h1 | operation_bit)
				: (h1 & ~operation_bit)) | valid_bit;
				hash_table[h2 >> 2][number >> 1].offset = log_file_offset;
				return true;
			}
		}
		number++;
	}
	return false;
}

template<typename Key, typename Value>
bool SILT::Log_store<Key, Value>::insert_into_bucket(uint16_t h1, uint32_t h2,
uint32_t log_file_offset, bool operation)
{
	for(uint8_t number = 0; number < bucket_size; number++)
	{
		if((hash_table[h1 >> 2][number].tag & valid_bit) == 0)
		{
			hash_table[h1 >> 2][number].tag = ((operation == true)
			? (h2 | operation_bit) : (h2 & ~operation_bit)) | valid_bit;
			hash_table[h1 >> 2][number].offset = log_file_offset;
			return true;
		}
	}
	return false;
}

template<typename Key, typename Value>
bool SILT::Log_store<Key, Value>::remove(const Key& key)
{
	Value* value = new Value();
	return insert(key, *value, false);
}

template<typename Key, typename Value>
Value* SILT::Log_store<Key, Value>::get_value(const Key& key, bool* reason)
const
{
	*reason = false;
	SILT_key sha_key;
	SHA_1(key, sizeof(key), &sha_key);
	uint16_t h1 = sha_key.h4 & h1_mask;
	uint16_t h2 = (sha_key.h4 & h2_mask) >> 16;

	uint8_t number = 0;
	while(number < (bucket_size << 1))
	{
		// odwrotna kolejność przeglądania, najpierw kubełek h2 i od dołu
		if(number % 2 == 1) // kubełek h1
		{
			if((hash_table[h1 >> 2][bucket_size - 1 - (number >> 1)].tag
			& ~operation_bit) == (h2 | valid_bit))
			{
				if((hash_table[h1 >> 2][bucket_size - 1 - (number >> 1)].tag
				& operation_bit) == 0)
				{
					*reason = true;
					return nullptr; // wpis usuwający
				}
				fseek(log_store_file,
				hash_table[h1 >> 2][bucket_size - 1 - (number >> 1)]
				.offset * log_entry_size, SEEK_SET);
				Key found_key;
				if(fread((void*) &found_key, sizeof(Key), 1, log_store_file)
				!= 1)
				{
					fprintf(stderr, "fread error\n");
					exit(1);
				}
				if(found_key == key)
				{
					Value* returned_value = new Value();
					if(fread((void*) returned_value, sizeof(Value), 1,
					log_store_file) != 1)
					{
						fprintf(stderr, "fread error\n");
						exit(1);
					}
					return returned_value;
				}
			}
		}
		else // kubełek h2
		{
			if((hash_table[h2 >> 2][bucket_size - 1 - (number >> 1)].tag
			& ~operation_bit) == (h1 | valid_bit))
			{
				if((hash_table[h2 >> 2][bucket_size - 1 - (number >> 1)].tag
				& operation_bit) == 0)
				{
					*reason = true;
					return nullptr; // wpis usuwający
				}
				fseek(log_store_file,
				hash_table[h2 >> 2][bucket_size - 1 - (number >> 1)]
				.offset * log_entry_size, SEEK_SET);
				Key found_key;
				if(fread((void*) &found_key, sizeof(Key), 1, log_store_file)
				!= 1)
				{
					fprintf(stderr, "fread error\n");
					exit(1);
				}
				if(found_key == key)
				{
					Value* returned_value = new Value();
					if(fread((void*) returned_value, sizeof(Value), 1,
					log_store_file) != 1)
					{
						fprintf(stderr, "fread error\n");
						exit(1);
					}
					return returned_value;
				}
			}
		}
		number++;
	}
	return nullptr;
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
