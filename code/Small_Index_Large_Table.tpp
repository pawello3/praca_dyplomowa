/*
	Paweł Guzewicz, 263664
	Indywidualne Studia Informatyczno-Matematyczne, Uniwersytet Wrocławski
	Praca licencjacka: Analiza teoretyczna i porównawcza efektywnej pamięciowo,
	wysoce wydajnej tablicy asocjacyjnej SILT (Small Index Large Table)
*/

template<typename Key, typename Value>
SILT::Small_Index_Large_Table<Key, Value>::Small_Index_Large_Table(void)
	:log_store(new Log_store<Value>())
	,hash_stores_array(new Hash_store<Value>*[maximum_number_of_hash_stores])
	,sorted_store(nullptr)
	,number_of_hash_stores(0)
{
}

template<typename Key, typename Value>
SILT::Small_Index_Large_Table<Key, Value>::~Small_Index_Large_Table(void)
{
	delete log_store;
	for(uint8_t i = 0; i < number_of_hash_stores; i++)
		delete hash_stores_array[i];
	delete[] hash_stores_array;
	delete sorted_store;
}

template<typename Key, typename Value>
void SILT::Small_Index_Large_Table<Key, Value>::insert(const Key& key,
const Value& value)
{
	SILT_key sha_key;
	SHA_1(key, &sha_key);
	bool successful = log_store->insert(sha_key, value);
	if(!successful)
	{
		assert(number_of_hash_stores <= maximum_number_of_hash_stores);
		if(number_of_hash_stores == maximum_number_of_hash_stores)
		{
			for(uint8_t i = 0; i < number_of_hash_stores; i++)
				hash_stores_array[i]->sort();
			Sorted_store<Value>* tmp =
			new Sorted_store<Value>(hash_stores_array, sorted_store);
			delete sorted_store;
			sorted_store = tmp;
			for(uint8_t i = 0; i < number_of_hash_stores; i++)
				delete hash_stores_array[i];
			number_of_hash_stores = 0;
			Hash_store<Value>::reset();
		}
		hash_stores_array[number_of_hash_stores++]
		= new Hash_store<Value>(log_store);
		delete log_store;
		log_store = new Log_store<Value>();
		successful = log_store->insert(sha_key, value);
		assert(successful);
	}
}

template<typename Key, typename Value>
void SILT::Small_Index_Large_Table<Key, Value>::remove(const Key& key)
{
	SILT_key sha_key;
	SHA_1(key, &sha_key);
	log_store->remove(sha_key);
}

template<typename Key, typename Value>
Value* SILT::Small_Index_Large_Table<Key, Value>::operator[](const Key& key)
const
{
	SILT_key sha_key;
	SHA_1(key, &sha_key);

	Value* returned_value = nullptr;

	bool reason;

	// sprawdzenie w Log store'rze
	returned_value = log_store->get_value(sha_key, &reason);
	if(returned_value != nullptr || reason == true)
		return returned_value;

	// sprawdzenie w Hash store'ach
	for(uint8_t i = 0; i < number_of_hash_stores; i++)
	{
		returned_value = hash_stores_array[i]->get_value(sha_key, &reason);
		if(returned_value != nullptr || reason == true)
			return returned_value;
	}

	// sprawdzenie w Sorted store'rze
	if(sorted_store != nullptr)
		return sorted_store->get_value(sha_key);

	// nie znaleziono
	return returned_value;
}
