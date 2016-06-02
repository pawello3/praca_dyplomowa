/*
	Paweł Guzewicz, 263664
	Indywidualne Studia Informatyczno-Matematyczne, Uniwersytet Wrocławski
	Praca licencjacka: Analiza teoretyczna i porównawcza efektywnej pamięciowo,
	wysoce wydajnej tablicy asocjacyjnej SILT (Small Index Large Table)
*/

#ifndef SMALL_INDEX_LARGE_TABLE_HPP_INCLUDED
#define SMALL_INDEX_LARGE_TABLE_HPP_INCLUDED

#include "Log_store.hpp"
#include "Hash_store.hpp"
#include "Sorted_store.hpp"

namespace SILT
{
	template<typename Key, typename Value>
	struct Hash_store_list_node
	{
		Hash_store<Key, Value>* data;
		Hash_store_list_node<Key, Value>* next;
	};

	template<typename Key, typename Value>
	class Hash_store_list
	{
		public:
		private:
			Hash_store_list_node<Key, Value>* head;
		public:
			Hash_store_list(void);
			~Hash_store_list(void);
			void prepend(Hash_store_list* new_node);
			void remove(Hash_store_list* node);
		private:
	};

	template<typename Key, typename Value>
	class Small_Index_Large_Table final
	{
		public:
		private:
			Hash_store_list<Key, Value>* hash_store_list;
		public:
			Small_Index_Large_Table(void);
			~Small_Index_Large_Table(void);
		private:
	};
}

#include "Small_Index_Large_Table.tpp" // implementacja szablonu

#endif // SMALL_INDEX_LARGE_TABLE_HPP_INCLUDED
