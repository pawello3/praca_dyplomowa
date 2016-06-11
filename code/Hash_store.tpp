/*
	Paweł Guzewicz, 263664
	Indywidualne Studia Informatyczno-Matematyczne, Uniwersytet Wrocławski
	Praca licencjacka: Analiza teoretyczna i porównawcza efektywnej pamięciowo,
	wysoce wydajnej tablicy asocjacyjnej SILT (Small Index Large Table)
*/

#include <cinttypes>

template<typename Value>
uint8_t SILT::Hash_store<Value>::first_free_id = 0;

template<typename Value>
SILT::Hash_store<Value>::Hash_store(SILT::Log_store<Value>* log_store)
	:log_file_entry_size(log_store->log_file_entry_size)
	,hash_store_file(fopen(next_name(), "wb+"))
{
	assert(log_store != nullptr);
	if(hash_store_file == nullptr)
	{
		fprintf(stderr, "fopen error\n");
		exit(1);
	}
	hash_table = new uint16_t*[hash_table_size];
	SILT_key key;
	Value value;
	fseek(hash_store_file, 0, SEEK_SET);
	for(uint16_t i = 0; i < hash_table_size; i++)
	{
		hash_table[i] = new uint16_t[bucket_size];
		for(uint8_t j = 0; j < bucket_size; j++)
		{
			hash_table[i][j] = log_store->hash_table[i][j].tag;
			if((hash_table[i][j] & valid_bit) != 0)
			{
				fseek(log_store->log_store_file,
				log_store->hash_table[i][j].offset * log_file_entry_size,
				SEEK_SET);
				if(fread((void*) &key, sizeof(SILT_key), 1,
				log_store->log_store_file) != 1)
				{
					fprintf(stderr, "fread error\n");
					exit(1);
				}
				if(fread((void*) &value, sizeof(Value), 1,
				log_store->log_store_file) != 1)
				{
					fprintf(stderr, "fread error\n");
					exit(1);
				}
			}
			else
			{
				memset(&key, 0, sizeof(SILT_key));
				memset(&value, 0, sizeof(Value));
			}
			fwrite((void*) &key, sizeof(SILT_key), 1, hash_store_file);
			fwrite((void*) &value, sizeof(Value), 1, hash_store_file);
		}
	}
}

template<typename Value>
SILT::Hash_store<Value>::~Hash_store(void)
{
	assert(!hash_table); /* powinna zostać usunięta podczas sortowania, czyli
	nie posortowano tego hash stora */
	fclose(hash_store_file);
	if(std::remove(file_name) != 0)
	{
		fprintf(stderr, "remove error\n");
		exit(1);
	}
}

template<typename Value>
char* SILT::Hash_store<Value>::next_name(void)
{
	sprintf(file_name, "Hash_store_%" PRIu32 ".dat", first_free_id);
	first_free_id = (first_free_id + 1) % maximum_number_of_hash_stores;
	return file_name;
}

template<typename Value>
Value* SILT::Hash_store<Value>::get_value(const SILT_key& key, bool* reason)
const
{
	assert(hash_table); // czy już nie została posortowana
	*reason = false;
	uint16_t h1 = key.h4 & h1_mask;
	uint16_t h2 = (key.h4 & h2_mask) >> 16;

	for(uint8_t i = 0; i < bucket_size; i++) // kubełek h1
	{
		if((hash_table[h1 >> 2][i] & ~operation_bit) == (h2 | valid_bit))
		{
			if((hash_table[h1 >> 2][i] & operation_bit) == 0)
			{
				*reason = true;
				return nullptr; // wpis usuwający
			}
			fseek(hash_store_file, ((h1 >> 2) * bucket_size + i)
			* log_file_entry_size, SEEK_SET);
			SILT_key found_key;
			if(fread((void*) &found_key, sizeof(SILT_key), 1, hash_store_file)
			!= 1)
			{
				fprintf(stderr, "fread error\n");
				exit(1);
			}
			if(found_key == key)
			{
				Value* returned_value = new Value();
				if(fread((void*) returned_value, sizeof(Value), 1,
				hash_store_file) != 1)
				{
					fprintf(stderr, "fread error\n");
					exit(1);
				}
				return returned_value;
			}
		}
	}

	for(uint8_t i = 0; i < bucket_size; i++) // kubełek h2
	{
		if((hash_table[h2 >> 2][i] & ~operation_bit) == (h1 | valid_bit))
		{
			if((hash_table[h2 >> 2][i] & operation_bit) == 0)
			{
				*reason = true;
				return nullptr; // wpis usuwający
			}
			fseek(hash_store_file, ((h2 >> 2) * bucket_size + i)
			* log_file_entry_size, SEEK_SET);
			SILT_key found_key;
			if(fread((void*) &found_key, sizeof(SILT_key), 1, hash_store_file)
			!= 1)
			{
				fprintf(stderr, "fread error\n");
				exit(1);
			}
			if(found_key == key)
			{
				Value* returned_value = new Value();
				if(fread((void*) returned_value, sizeof(Value), 1,
				hash_store_file) != 1)
				{
					fprintf(stderr, "fread error\n");
					exit(1);
				}
				return returned_value;
			}
		}
	}

	return nullptr;
}

template<typename Value>
void SILT::Hash_store<Value>::sort()
{
	/* deklaracja i wypełnienie wartościami z pliku (zamiast value wpisujemy
	przesunięcie w pliku hash_store_file - numer pary klucz-wartość)
	tablicy sorted_table typu Sorted_table_entry */
	uint32_t sorted_table_size = hash_table_size * bucket_size;
	Sorted_table_entry* sorted_table
	= new Sorted_table_entry[sorted_table_size];
	sorted_table_size = 0;
	for(uint16_t i = 0; i < hash_table_size; i++)
	{
		for(uint8_t j = 0; j < bucket_size; j++)
		{
			if((hash_table[i][j] & (valid_bit)) != 0)
			{
				fseek(hash_store_file, (i * bucket_size + j)
				* log_file_entry_size, SEEK_SET);
				if(fread((void*) &sorted_table[sorted_table_size].key,
				sizeof(SILT_key), 1, hash_store_file) != 1)
				{
					fprintf(stderr, "fread error\n");
					exit(1);
				}
				sorted_table[sorted_table_size].offset = i * bucket_size + j;
				sorted_table[sorted_table_size].operation = (hash_table[i][j]
				& operation_bit) ? 0x1 : 0x0;
				sorted_table_size++;
			}
		}
	}

	// usunięcie tablicy z haszowaniem
	for(uint16_t i = 0; i < hash_table_size; i++)
		delete[] hash_table[i];
	delete[] hash_table;
	hash_table = nullptr;

	// deklaracja pomocniczych tablic do sortowania kubełkowego
	Sorted_table_entry* buckets_table
	= new Sorted_table_entry[sorted_table_size];
	uint16_t* count = new uint16_t[hash_table_size * bucket_size];

	// sortowanie pozycyjne tablicy sorted_table
    radix_sort(buckets_table, sorted_table, sorted_table_size, count);

	// deklaracja i otwarcie nowego pliku sorted_hash_store_file
	char buffer[32];
	sprintf(buffer, "Sorted_h%s", file_name + 1);
	FILE* sorted_hash_store_file = fopen(buffer, "wb+");

	// zapisanie sorted_table do nowego pliku i usunięcie jej z pamięci
	Value value;
	fseek(sorted_hash_store_file, 0, SEEK_SET);
	for(uint32_t i = 0; i < sorted_table_size; i++)
	{
		fseek(hash_store_file, sorted_table[i].offset * log_file_entry_size
		+ sizeof(Value), SEEK_SET);
		if(fread((void*) &value, sizeof(Value), 1, hash_store_file) != 1)
		{
			fprintf(stderr, "fread error\n");
			exit(1);
		}
		fwrite((void*) &sorted_table[i].key, sizeof(SILT_key), 1,
		sorted_hash_store_file);
		fwrite((void*) &value, sizeof(Value), 1, sorted_hash_store_file);
		fwrite((void*) &sorted_table[i].operation, sizeof(uint8_t), 1,
		sorted_hash_store_file);
	}
	delete[] sorted_table;

	// usunięcie starego pliku
	fclose(hash_store_file);
	if(std::remove(file_name) != 0)
	{
		fprintf(stderr, "remove error\n");
		exit(1);
	}

	// podmiana wskaźnika hash_store_file i nazwy pliku
	hash_store_file = sorted_hash_store_file;
	strcpy(file_name, buffer);
}

template<typename Value>
void SILT::Hash_store<Value>::radix_sort(Sorted_table_entry* buckets_table,
Sorted_table_entry* sorted_table, uint32_t sorted_table_size, uint16_t* count)
{
	for(uint8_t i = 0; i < 10; i++)
	{
		memset(count, 0, (hash_table_size * bucket_size) << 1);
		counting_sort(buckets_table, sorted_table, sorted_table_size,
		10 - i - 1, count);
		memcpy(sorted_table, buckets_table, sorted_table_size);
	}
}

template<typename Value>
void SILT::Hash_store<Value>::counting_sort(Sorted_table_entry* buckets_table,
Sorted_table_entry* sorted_table, uint32_t sorted_table_size, uint8_t fragment,
uint16_t* count)
{
	uint16_t key;
	/*for(uint32_t i = 0; i < sorted_table_size; i++)
	{
		memcpy(&key, ((uint8_t*) (sorted_table + i)) + (fragment << 1), 2);
		PRINT_UINT_16(key);
		printf("\n");
	}
	printf("KONIEC\n\n");*/
	for(uint32_t i = 0; i < sorted_table_size; i++)
	{
		memcpy(&key, ((uint8_t*) (sorted_table + i)) + (fragment << 1), 2);
		count[key]++;
	}
	uint16_t sum = 0, old_count;
	for(uint32_t i = 0; i < hash_table_size * bucket_size; i++)
	{
		old_count = count[i];
		count[i] = sum;
		sum += old_count;
	}
	for(uint32_t i = 0; i < sorted_table_size; i++)
	{
		memcpy(&key, ((uint8_t*) (sorted_table + i)) + (fragment << 1), 2);
		buckets_table[count[key]] = sorted_table[i];
		count[key]++;
	}
	/*for(uint32_t i = 0; i < sorted_table_size; i++)
	{
		memcpy(&key, ((uint8_t*) (buckets_table + i)) + (fragment << 1), 2);
		PRINT_UINT_16(key);
		printf("\n");
	}*/
}

////////////////////////////////////////////////////////////////////////////////

template<typename Value>
SILT::Hash_store_list_node<Value>
::Hash_store_list_node(Hash_store<Value>* data)
	:data(data)
	,previous(nullptr)
	,next(nullptr)
{
}

template<typename Value>
SILT::Hash_store_list_node<Value>::~Hash_store_list_node<Value>(void)
{
}

template<typename Value>
SILT::Hash_store<Value>* SILT::Hash_store_list_node<Value>::get_data(void) const
{
	return data;
}

template<typename Value>
SILT::Hash_store_list_node<Value>* SILT::Hash_store_list_node<Value>
::get_next(void) const
{
	return next;
}

////////////////////////////////////////////////////////////////////////////////

template<typename Value>
SILT::Hash_store_list<Value>::Hash_store_list(void)
	:head(new Hash_store_list_node<Value>(nullptr))
	,size(0)
{
}

template<typename Value>
SILT::Hash_store_list<Value>::~Hash_store_list(void)
{
	Hash_store_list_node<Value>* tmp = head;
	while(head != nullptr)
	{
		tmp = head->next;
		delete head;
		head = tmp;
	}
}

template<typename Value>
SILT::Hash_store_list_node<Value>* SILT::Hash_store_list<Value>::get_head(void)
const
{
	return head;
}

template<typename Value>
void SILT::Hash_store_list<Value>::prepend(Hash_store_list<Value>* node)
{
	if(head == nullptr)
	{
		node->previous = node;
		node->next = node;
	}
	else
	{
		node->next = head;
		node->previous = head->previous;
		node->prevoius->next = node;
		node->next->previous = node;
	}
	head = node;
	size++;
}

template<typename Value>
void SILT::Hash_store_list<Value>::remove(Hash_store_list<Value>* node)
{
	assert(size > 0);
	if(node->previous == node->next)
	{
		head = nullptr;
	}
	else
	{
		head = node->next;
		node->previous->next = node->next;
		node->next->previous = node->previous;
	}
	delete node;
	size--;
}

template<typename Value>
uint8_t SILT::Hash_store_list<Value>::get_size(void) const
{
	return size;
}
