/*
	Paweł Guzewicz, 263664
	Indywidualne Studia Informatyczno-Matematyczne, Uniwersytet Wrocławski
	Praca licencjacka: Analiza teoretyczna i porównawcza efektywnej pamięciowo,
	wysoce wydajnej tablicy asocjacyjnej SILT (Small Index Large Table)
*/

#include <cinttypes>

template<typename Value>
uint32_t SILT::Hash_store<Value>::first_free_id = 0;

template<typename Value>
SILT::Hash_store<Value>::Hash_store(SILT::Log_store<Value>* log_store)
	:log_entry_size(log_store->log_entry_size)
	,hash_store_file(fopen(next_id(), "wb+"))
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
	for(uint32_t i = 0; i < hash_table_size; i++)
	{
		hash_table[i] = new uint16_t[bucket_size];
		for(uint8_t j = 0; j < bucket_size; j++)
		{
			hash_table[i][j] = log_store->hash_table[i][j].tag;
			if((hash_table[i][j] & valid_bit) != 0)
			{
				fseek(log_store->log_store_file,
				log_store->hash_table[i][j].offset * log_store->log_entry_size,
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
	for(uint32_t i = 0; i < hash_table_size; i++)
		delete[] hash_table[i];
	delete[] hash_table;
	fclose(hash_store_file);
	if(std::remove(id) != 0)
	{
		fprintf(stderr, "remove error\n");
		exit(1);
	}
}

template<typename Value>
Value* SILT::Hash_store<Value>::get_value(const SILT_key& key, bool* reason)
const
{
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
			* log_entry_size, SEEK_SET);
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
			* log_entry_size, SEEK_SET);
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
char* SILT::Hash_store<Value>::next_id(void)
{
	sprintf(id, "Hash_store_%" PRIu32 ".dat", first_free_id);
	first_free_id++;
	return id;
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
SILT::Hash_store<Value>* SILT::Hash_store_list_node<Value>
::get_data(void)
const
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
SILT::Hash_store_list_node<Value>* SILT::Hash_store_list<Value>
::get_head(void) const
{
	return head;
}

template<typename Value>
void SILT::Hash_store_list<Value>
::prepend(Hash_store_list<Value>* node)
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
void SILT::Hash_store_list<Value>
::remove(Hash_store_list<Value>* node)
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
uint32_t SILT::Hash_store_list<Value>::get_size(void) const
{
	return size;
}
