/*
	Paweł Guzewicz, 263664
	Indywidualne Studia Informatyczno-Matematyczne, Uniwersytet Wrocławski
	Praca licencjacka: Analiza teoretyczna i porównawcza efektywnej pamięciowo,
	wysoce wydajnej tablicy asocjacyjnej SILT (Small Index Large Table)
*/

template<typename Key, typename Value>
SILT::Small_Index_Large_Table<Key, Value>::Small_Index_Large_Table(void)
	:log_store(new Log_store<Key, Value>())
	,hash_store_list(nullptr)
	,sorted_store(nullptr)
{
}

template<typename Key, typename Value>
SILT::Small_Index_Large_Table<Key, Value>::~Small_Index_Large_Table(void)
{
	delete log_store;
	delete hash_store_list;
	delete sorted_store;
}

template<typename Key, typename Value>
void SILT::Small_Index_Large_Table<Key, Value>::insert(const Key& key,
const Value& value)
{
	assert(false); // TODO
}

template<typename Key, typename Value>
void SILT::Small_Index_Large_Table<Key, Value>::remove(const Key& key)
{
	assert(false); // TODO
}

template<typename Key, typename Value>
Value* SILT::Small_Index_Large_Table<Key, Value>::operator[](const Key& key)
const
{
	Value* returned_value = nullptr;

	bool reason = false;

	// sprawdzenie w Log store'rze
	returned_value = log_store->get_value(key, &reason);
	if(returned_value != nullptr || reason == true)
		return returned_value;

	// sprawdzenie w Hash store'ach
	Hash_store_list_node<Key, Value>* node = hash_store_list->get_head();
	while(node != nullptr)
	{
		returned_value = node->get_data()->get_value(key, &reason);
		if(returned_value != nullptr || reason == true)
			return returned_value;
		node = node->get_next();
	}

	// sprawdzenie w Sorted store'rze
	if(sorted_store != nullptr)
		return sorted_store->get_value(key);

	// nie znaleziono
	return returned_value;
}
