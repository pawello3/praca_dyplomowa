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
	:hash_store_file(fopen(next_id(), "wb+"))
{
	if(hash_store_file == nullptr)
	{
		fprintf(stderr, "fopen error\n");
		exit(1);
	}
}

template<typename Key, typename Value>
SILT::Hash_store<Key, Value>::~Hash_store(void)
{
}

template<typename Key, typename Value>
char* SILT::Hash_store<Key, Value>::next_id(void)
{
	char* id;
	sprintf(id, "Hash_store_%" PRIu32 ".dat", first_free_id);
	first_free_id++;
	return id;
}
