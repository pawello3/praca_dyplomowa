/*
	Paweł Guzewicz, 263664
	Indywidualne Studia Informatyczno-Matematyczne, Uniwersytet Wrocławski
	Praca licencjacka: Analiza teoretyczna i porównawcza efektywnej pamięciowo,
	wysoce wydajnej tablicy asocjacyjnej SILT (Small Index Large Table)
*/

template<typename Value>
SILT::Sorted_store<Value>::Sorted_store(Hash_store<Value>** hash_stores_array,
Sorted_store<Value>* old_sorted_store)
	:sorted_store_file(fopen("Sorted_store.dat", "wb+"))
	,file_size(0)
	,sorted_hash_store_entry_size(sizeof(SILT_key) + sizeof(Value)
	+ sizeof(uint8_t))
	,sorted_store_entry_size(sizeof(SILT_key) + sizeof(Value))
{
	if(sorted_store_file == nullptr)
	{
		fprintf(stderr, "fopen error\n");
		exit(1);
	}
	strcpy(file_name, "Sorted_store.dat");
	merge(hash_stores_array, old_sorted_store);
	build_trie_indexing();
}

template<typename Value>
SILT::Sorted_store<Value>::~Sorted_store(void)
{
	//TODO (usuwanie wszystkich kubełków)
	/*trie_buckets[0].~Trie();
	operator delete[](static_cast<void*>(trie_buckets));*/
	delete trie;
	fclose(sorted_store_file);
	if(std::remove(file_name) != 0)
	{
		fprintf(stderr, "remove error\n");
		exit(1);
	}
}

template<typename Value>
void SILT::Sorted_store<Value>::merge(Hash_store<Value>** hash_stores_array,
Sorted_store<Value>* old_sorted_store)
{
	/* zakładamy, że rozmiar tablicy hash_stores_array to
	maximum_number_of_hash_stores */
	bool merging_with_sorted_store = old_sorted_store == nullptr ? false : true;

	Merge_heap* heap = new Merge_heap(maximum_number_of_hash_stores + 1);
	uint32_t first_unread_position[maximum_number_of_hash_stores + 1] = {0};
	Sorted_hash_store_entry array[maximum_number_of_hash_stores + 1];

	// inicjalizacja tablicy do scalania
	for(uint8_t i = 0; i < maximum_number_of_hash_stores; i++)
	{
		insert_into_heap(hash_stores_array, old_sorted_store, heap, array,
		first_unread_position, i);
		first_unread_position[i]++;
	}
	if(merging_with_sorted_store)
	{
		// inicjalizacja ostatniej komórki tablicy
		insert_into_heap(hash_stores_array, old_sorted_store, heap, array,
		first_unread_position, maximum_number_of_hash_stores);
		first_unread_position[maximum_number_of_hash_stores]++;
	}
	fseek(sorted_store_file, 0, SEEK_SET);
	uint8_t number;
	Sorted_hash_store_entry entry;
	while(heap->get_size() > 0)
	{
		number = get_top_of_heap_and_append_to_file(hash_stores_array,
		old_sorted_store, heap, first_unread_position, &entry);

		insert_into_heap(hash_stores_array, old_sorted_store, heap, array,
		first_unread_position, number);
		first_unread_position[number]++;
		while(heap->remove_duplicate(entry, &number))
		{
			insert_into_heap(hash_stores_array, old_sorted_store, heap, array,
			first_unread_position, number);
			first_unread_position[number]++;
		}
	}
	delete heap;
}

template<typename Value>
void SILT::Sorted_store<Value>::insert_into_heap(Hash_store<Value>**
hash_stores_array, Sorted_store<Value>* old_sorted_store, Merge_heap* heap,
Sorted_hash_store_entry* array, uint32_t* first_unread_position, uint8_t number)
{
	if(number == maximum_number_of_hash_stores)
	{
		// nie przeczytano całego pliku
		if(first_unread_position[number] < old_sorted_store->file_size)
		{
			fseek(old_sorted_store->sorted_store_file,
			first_unread_position[number] * sorted_store_entry_size, SEEK_SET);
			if(fread((void*) &array[number].key, sizeof(SILT_key), 1,
			old_sorted_store->sorted_store_file) != 1)
			{
				fprintf(stderr, "fread error\n");
				exit(1);
			}
			array[number].hash_store_number_and_operation = (number << 1)
			| entry_operation_bit;
			heap->insert(array[number]);
		}
	}
	else
	{
		// nie przeczytano całego pliku
		if(first_unread_position[number] < hash_stores_array[number]->file_size)
		{
			fseek(hash_stores_array[number]->hash_store_file,
			first_unread_position[number] * sorted_hash_store_entry_size,
			SEEK_SET);
			if(fread((void*) &array[number].key, sizeof(SILT_key), 1,
			hash_stores_array[number]->hash_store_file) != 1)
			{
				fprintf(stderr, "fread error\n");
				exit(1);
			}
			fseek(hash_stores_array[number]->hash_store_file,
			first_unread_position[number] * sorted_hash_store_entry_size
			+ sizeof(SILT_key) + sizeof(Value), SEEK_SET);
			uint8_t operation;
			if(fread((void*) &operation, sizeof(uint8_t), 1,
			hash_stores_array[number]->hash_store_file) != 1)
			{
				fprintf(stderr, "fread error\n");
				exit(1);
			}
			array[number].hash_store_number_and_operation = (number << 1)
			| (operation ? entry_operation_bit : 0);
			heap->insert(array[number]);
		}
	}
}

template<typename Value>
uint8_t SILT::Sorted_store<Value>::get_top_of_heap_and_append_to_file(
Hash_store<Value>** hash_stores_array, Sorted_store<Value>* old_sorted_store,
Merge_heap* heap, uint32_t* first_unread_position,
Sorted_hash_store_entry* returned_entry)
{
	Sorted_hash_store_entry entry = heap->get_top();
	*returned_entry = entry;
	uint8_t number = entry.hash_store_number_and_operation >> 1;
	Value value;

	// operacja dodawania
	if(entry.hash_store_number_and_operation & entry_operation_bit)
	{
		if(number == maximum_number_of_hash_stores) // klucz z Sorted store
		{
			fseek(old_sorted_store->sorted_store_file,
			(first_unread_position[number] - 1) * sorted_store_entry_size
			+ sizeof(SILT_key), SEEK_SET);
			if(fread((void*) &value, sizeof(Value), 1,
			old_sorted_store->sorted_store_file) != 1)
			{
				fprintf(stderr, "fread error\n");
				exit(1);
			}
		}
		else
		{
			assert(number < maximum_number_of_hash_stores);
			fseek(hash_stores_array[number]->hash_store_file,
			(first_unread_position[number] - 1) * sorted_hash_store_entry_size
			+ sizeof(SILT_key), SEEK_SET);
			if(fread((void*) &value, sizeof(Value), 1,
			hash_stores_array[number]->hash_store_file) != 1)
			{
				fprintf(stderr, "fread error\n");
				exit(1);
			}
		}
		fwrite((void*) &entry.key, sizeof(SILT_key), 1, sorted_store_file);
		fwrite((void*) &value, sizeof(Value), 1, sorted_store_file);
		file_size++;
	}
	return number;
}

template<typename Value>
void SILT::Sorted_store<Value>::build_trie_indexing(void)
{
	trie = new Trie<Value>(sorted_store_file, file_size);
	// TODO (dodanie wielu kubełków)
	/*trie_buckets = static_cast<Trie<Value>*> (operator new[]
	(1 * sizeof(Trie<Value>)));
	new(&trie_buckets[0]) Trie<Value>(sorted_store_file, file_size);*/
}

template<typename Value>
Value* SILT::Sorted_store<Value>::get_value(const SILT_key& key) const
{
	// Sprawdzenie poprawności
	/*SILT_key key_to_match;
	fseek(sorted_store_file, 0, SEEK_SET);
	uint32_t i;
	for(i = 0; i < file_size; i++)
	{
		if(fread((void*) &key_to_match, sizeof(SILT_key), 1, sorted_store_file)
		!= 1)
		{
			fprintf(stderr, "fread error\n");
			exit(1);
		}
		if(key != key_to_match)
		{
			fseek(sorted_store_file, sizeof(Value), SEEK_CUR);
		}
		else
		{
			DEBUG(PRINT_SILT_KEY(key_to_match); PRINT_UINT_32(i);
			PRINT_UINT_32(file_size));
			break;
		}
	}*/

	// Zwracanie wartości używając indeksowania drzewami Trie
	// TODO (dodanie wielu kubełków)
	/*assert(trie_buckets[0].get_offset(key) < file_size);
	DEBUG(PRINT_UINT_32(trie_buckets[0].get_offset(key)));
	fseek(sorted_store_file, trie_buckets[0].get_offset(key)
	* sorted_store_entry_size, SEEK_SET);*/
	assert(trie->get_offset(key) < file_size);
	DEBUG(PRINT_UINT_32(trie->get_offset(key)));
	fseek(sorted_store_file, trie->get_offset(key)
	* sorted_store_entry_size, SEEK_SET);
	SILT_key key_to_match;
	if(fread((void*) &key_to_match, sizeof(SILT_key), 1, sorted_store_file)
	!= 1)
	{
		fprintf(stderr, "fread error\n");
		exit(1);
	}
	if(key != key_to_match)
	{
		return nullptr;
	}
	Value* returned_value = new Value();
	if(fread((void*) returned_value, sizeof(Value), 1, sorted_store_file) != 1)
	{
		fprintf(stderr, "fread error\n");
		exit(1);
	}
	return returned_value;

	// Zwracanie wartości bez indeksowania drzewami Trie
	/*SILT_key key_to_match;
	Value* returned_value;
	fseek(sorted_store_file, 0, SEEK_SET);
	for(uint32_t i = 0; i < file_size; i++)
	{
		if(fread((void*) &key_to_match, sizeof(SILT_key), 1, sorted_store_file)
		!= 1)
		{
			fprintf(stderr, "fread error\n");
			exit(1);
		}
		if(key != key_to_match)
		{
			fseek(sorted_store_file, sizeof(Value), SEEK_CUR);
			continue;
		}
		returned_value = new Value();
		if(fread((void*) returned_value, sizeof(Value), 1, sorted_store_file)
		!= 1)
		{
			fprintf(stderr, "fread error\n");
			exit(1);
		}
		return returned_value;
	}
	return nullptr;*/
}
