/*
	Paweł Guzewicz, 263664
	Indywidualne Studia Informatyczno-Matematyczne, Uniwersytet Wrocławski
	Praca licencjacka: Analiza teoretyczna i porównawcza efektywnej pamięciowo,
	wysoce wydajnej tablicy asocjacyjnej SILT (Small Index Large Table)
*/

#include <cinttypes>

template<typename Key, typename Value>
uint32_t SILT::Hash_store<Key, Value>::first_free_id = 0;

template<typename Key, typename Value>
SILT::Hash_store<Key, Value>::Hash_store(SILT::Log_store<Key, Value>* log_store)
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
	Key key;
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
				if(fread((void*) &key, sizeof(Key), 1,
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
				memset(&key, 0, sizeof(Key));
				memset(&value, 0, sizeof(Value));
			}
			fwrite((void*) &key, sizeof(Key), 1, hash_store_file);
			fwrite((void*) &value, sizeof(Value), 1, hash_store_file);
		}
	}
}

template<typename Key, typename Value>
SILT::Hash_store<Key, Value>::~Hash_store(void)
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

template<typename Key, typename Value>
Value* SILT::Hash_store<Key, Value>::get_value(const Key& key, bool* reason)
const
{
	assert(false); // TODO
	return nullptr;
}

template<typename Key, typename Value>
char* SILT::Hash_store<Key, Value>::next_id(void)
{
	sprintf(id, "Hash_store_%" PRIu32 ".dat", first_free_id);
	first_free_id++;
	return id;
}

////////////////////////////////////////////////////////////////////////////////

template<typename Key, typename Value>
SILT::Hash_store_list_node<Key, Value>
::Hash_store_list_node(Hash_store<Key, Value>* data)
	:data(data)
	,previous(nullptr)
	,next(nullptr)
{
}

template<typename Key, typename Value>
SILT::Hash_store_list_node<Key, Value>::~Hash_store_list_node<Key, Value>(void)
{
}

template<typename Key, typename Value>
SILT::Hash_store<Key, Value>* SILT::Hash_store_list_node<Key, Value>
::get_data(void)
const
{
	return data;
}

template<typename Key, typename Value>
SILT::Hash_store_list_node<Key, Value>* SILT::Hash_store_list_node<Key, Value>
::get_next(void) const
{
	return next;
}

////////////////////////////////////////////////////////////////////////////////

template<typename Key, typename Value>
SILT::Hash_store_list<Key, Value>::Hash_store_list(void)
	:head(new Hash_store_list_node<Key, Value>(nullptr))
	,size(0)
{
}

template<typename Key, typename Value>
SILT::Hash_store_list<Key, Value>::~Hash_store_list(void)
{
	Hash_store_list_node<Key, Value>* tmp = head;
	while(head != nullptr)
	{
		tmp = head->next;
		delete head;
		head = tmp;
	}
}

template<typename Key, typename Value>
SILT::Hash_store_list_node<Key, Value>* SILT::Hash_store_list<Key, Value>
::get_head(void) const
{
	return head;
}

template<typename Key, typename Value>
void SILT::Hash_store_list<Key, Value>
::prepend(Hash_store_list<Key, Value>* node)
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

template<typename Key, typename Value>
void SILT::Hash_store_list<Key, Value>
::remove(Hash_store_list<Key, Value>* node)
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

template<typename Key, typename Value>
uint32_t SILT::Hash_store_list<Key, Value>::get_size(void) const
{
	return size;
}
