/*
	Paweł Guzewicz, 263664
	Indywidualne Studia Informatyczno-Matematyczne, Uniwersytet Wrocławski
	Praca licencjacka: Analiza teoretyczna i porównawcza efektywnej pamięciowo,
	wysoce wydajnej tablicy asocjacyjnej SILT (Small Index Large Table)
*/

#include "debug.hpp"
#include <cstring>
#include <cstdlib>
#include <ctime>

template<typename Value>
SILT::Log_store<Value>::Log_store(void)
	:log_entry_size(sizeof(SILT_key) + sizeof(Value))
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

template<typename Value>
SILT::Log_store<Value>::~Log_store(void)
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

template<typename Value>
bool SILT::Log_store<Value>::insert(const SILT_key& key, const Value& value)
{
	uint16_t h1 = key.h4 & h1_mask;
	uint16_t h2 = (key.h4 & h2_mask) >> 16;

	/* sprawdzamy czy klucz jest w tablicy, aby podmienić dane na dysku oraz
	rodzaj operacji i przesunięcie w pliku pamiętane w tablicy */
	uint8_t bucket_index;
	if(look_up(h1, h2, &bucket_index))
	{
		if(check_key_matching(key, hash_table[h1 >> 2][bucket_index].offset))
		{
			hash_table[h1 >> 2][bucket_index].tag |= operation_bit;
			modify_entry_in_log_store_file(key, value, hash_table[h1 >> 2]
			[bucket_index].offset);
			return true;
		}
	}
	if(look_up(h2, h1, &bucket_index))
	{
		if(check_key_matching(key, hash_table[h2 >> 2][bucket_index].offset))
		{
			hash_table[h2 >> 2][bucket_index].tag |= operation_bit;
			modify_entry_in_log_store_file(key, value, hash_table[h2 >> 2]
			[bucket_index].offset);
			return true;
		}
	}

	// sprawdzamy czy można wstawić do tablicy nowy klucz
	if(insert_into_bucket(h1, h2, file_size)
	|| insert_into_bucket(h2, h1, file_size))
	{
		append_entry_to_log_store_file(key, value);
		return true;
	}

	/*
		W tym miejscu stosujemy haszowanie kukułcze dla częściowych kluczy.
		Od teraz element X, który nie ma jeszcze swojej pozycji (wyrzucony),
		znajduje się w schowku reprezentowanym jako:
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

		Po wstawieniu elementu X ze schowka na jego miejsce, umieszczamy
		w schowku element Y.
	*/

	h1 = h1 | operation_bit | valid_bit;
	h2 = h2 | operation_bit | valid_bit;
	uint32_t log_file_offset = file_size;
	uint16_t tmp_h1;
	uint16_t tmp_h2;
	uint32_t tmp_log_file_offset;

	srand(time(nullptr));
	uint8_t random = rand() % 2;
	if(random) // zamień h1 z h2
	{
		tmp_h1 = h1;
		h1 = h2;
		h2 = tmp_h1;
	}

	uint8_t undo[maximum_number_of_displacements];
	// wstawiamy do kubełka h2
	for(uint8_t i = 0; i < maximum_number_of_displacements; i++)
	{
		if(insert_into_bucket(h2, h1, log_file_offset))
		{
			append_entry_to_log_store_file(key, value);
			return true;
		}
		undo[maximum_number_of_displacements - i - 1] = rand() % 4;
        tmp_h1 = h2;
        tmp_h2 = hash_table[h2 >> 2]
        [undo[maximum_number_of_displacements - i - 1]].tag;
        tmp_log_file_offset = hash_table[h2 >> 2]
        [undo[maximum_number_of_displacements - i - 1]].offset;
        hash_table[h2 >> 2][undo[maximum_number_of_displacements - i - 1]]
        .tag = h1;
        hash_table[h2 >> 2][undo[maximum_number_of_displacements - i - 1]]
        .offset = log_file_offset;
        h1 = tmp_h1;
        h2 = tmp_h2;
        log_file_offset = tmp_log_file_offset;
	}

	DEBUG(fprintf(stderr, "Przepełnienie, cofanie wstawienia\n"));

	// wstawiamy do kubełka h1
	for(uint8_t i = 0; i < maximum_number_of_displacements; i++)
	{
		tmp_h1 = hash_table[h1 >> 2][undo[i]].tag;
		tmp_h2 = h1;
		tmp_log_file_offset = hash_table[h1 >> 2][undo[i]].offset;
		hash_table[h1 >> 2][undo[i]].tag = h2;
		hash_table[h1 >> 2][undo[i]].offset = log_file_offset;
		h1 = tmp_h1;
		h2 = tmp_h2;
		log_file_offset = tmp_log_file_offset;
	}

	assert(random ? (h2 & 0xFFFC) == (key.h4 & h1_mask)
	: (h1 & 0xFFFC) == (key.h4 & h1_mask));
	assert(random ? (h1 & 0xFFFC) == ((key.h4 & h2_mask) >> 16)
	: (h2 & 0xFFFC) == ((key.h4 & h2_mask) >> 16));
	assert(log_file_offset == file_size);
	// jeśli któraś asercja się wywali, tzn., że źle odtwarzano stan tablicy
	return false;
}

template<typename Value>
bool SILT::Log_store<Value>::look_up(uint16_t h_index, uint16_t h_tag,
uint8_t* bucket_index) const
{
	for(uint8_t i = 0; i < bucket_size; i++)
	{
		if((hash_table[h_index >> 2][i].tag & ~operation_bit) == (h_tag
		| valid_bit))
		{
			*bucket_index = i;
			return true;
		}
	}

	return false;
}

template<typename Value>
bool SILT::Log_store<Value>::check_key_matching(const SILT_key& key,
uint32_t log_file_offset) const
{
	fseek(log_store_file, log_file_offset * log_entry_size, SEEK_SET);
	SILT_key found_key;
	if(fread((void*) &found_key, sizeof(SILT_key), 1, log_store_file)
	!= 1)
	{
		fprintf(stderr, "fread error\n");
		exit(1);
	}
	return found_key == key;
}

template<typename Value>
void SILT::Log_store<Value>::modify_entry_in_log_store_file(const SILT_key& key,
const Value& value, uint32_t log_file_offset)
{
	fseek(log_store_file, log_file_offset * log_entry_size, SEEK_SET);
	fwrite((void*) &key, sizeof(SILT_key), 1, log_store_file);
	fwrite((void*) &value, sizeof(Value), 1, log_store_file);
}

template<typename Value>
bool SILT::Log_store<Value>::insert_into_bucket(uint16_t h_index,
uint16_t h_tag, uint32_t log_file_offset)
{
	for(uint8_t i = 0; i < bucket_size; i++)
	{
		if((hash_table[h_index >> 2][i].tag & valid_bit) == 0)
		{
			hash_table[h_index >> 2][i].tag = h_tag | operation_bit | valid_bit;
			hash_table[h_index >> 2][i].offset = log_file_offset;
			return true;
		}
	}
	return false;
}

template<typename Value>
void SILT::Log_store<Value>::append_entry_to_log_store_file(const SILT_key& key,
const Value& value)
{
	fseek(log_store_file, file_size * log_entry_size, SEEK_SET);
	fwrite((void*) &key, sizeof(SILT_key), 1, log_store_file);
	fwrite((void*) &value, sizeof(Value), 1, log_store_file);
	file_size++;
}

template<typename Value>
void SILT::Log_store<Value>::remove(const SILT_key& key)
{
	uint16_t h1 = key.h4 & h1_mask;
	uint16_t h2 = (key.h4 & h2_mask) >> 16;

	uint8_t bucket_index;
	if(look_up(h1, h2, &bucket_index)) // kubełek 1
	{
		if(check_key_matching(key, hash_table[h1 >> 2][bucket_index].offset))
		{
			hash_table[h1 >> 2][bucket_index].tag &= ~operation_bit;
			return;
		}
	}

	if(look_up(h2, h1, &bucket_index)) // kubełek 2
	{
		if(check_key_matching(key, hash_table[h2 >> 2][bucket_index].offset))
		{
			hash_table[h2 >> 2][bucket_index].tag &= ~operation_bit;
			return;
		}
	}
}

template<typename Value>
Value* SILT::Log_store<Value>::get_value(const SILT_key& key, bool* reason)
const
{
	*reason = false;
	uint16_t h1 = key.h4 & h1_mask;
	uint16_t h2 = (key.h4 & h2_mask) >> 16;

	uint8_t bucket_index;
	uint32_t log_file_offset;
	if(look_up(h1, h2, &bucket_index)) // kubełek 1
	{
		if((hash_table[h1 >> 2][bucket_index].tag & operation_bit) == 0)
		{
			*reason = true;
			return nullptr; // wpis usuwający
		}
		log_file_offset = hash_table[h1 >> 2][bucket_index].offset;
	}
	else if(look_up(h2, h1, &bucket_index)) // kubełek 2
	{
		if((hash_table[h2 >> 2][bucket_index].tag & operation_bit) == 0)
		{
			*reason = true;
			return nullptr; // wpis usuwający
		}
		log_file_offset = hash_table[h2 >> 2][bucket_index].offset;
	}
	else // w żadnym nie ma
	{
		return nullptr;
	}

	if(check_key_matching(key, log_file_offset))
	{
		Value* returned_value = new Value();
		if(fread((void*) returned_value, sizeof(Value), 1, log_store_file) != 1)
		{
			fprintf(stderr, "fread error\n");
			exit(1);
		}
		return returned_value;
	}

	return nullptr;
}
