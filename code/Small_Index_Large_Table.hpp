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
	class Hash_store_list_node
	{
		private:
			Hash_store<Key, Value>* data;
			Hash_store_list_node<Key, Value>* next;

		public:
			Hash_store_list<Key, Value>(void);
			~Hash_store_list<Key, Value>(void);

			Hash_store_list<Key, Value>* get_data(void) const;
			Hash_store_list_node<Key, Value>* get_next(void) const;
	};

	template<typename Key, typename Value>
	class Hash_store_list final
	{
		private:
			Hash_store_list_node<Key, Value>* head;
			Hash_store_list_node<Key, Value>* last;
			uint32_t size;

		public:
			Hash_store_list(void);
			~Hash_store_list(void);

			Hash_store_list_node<Key, Value>* get_head(void) const;
			Hash_store_list_node<Key, Value>* get_last(void) const;
			void prepend(Hash_store_list* new_node);
			void remove(Hash_store_list* node);
			uint32_t get_size(void) const;
	};

	template<typename Key, typename Value>
	class Small_Index_Large_Table final
	{
		private:
			Log_store<Key, Value>* log_store;
			Hash_store_list<Key, Value>* hash_store_list;
			Sorted_store<Key, Value>* sorted_store;

		public:
			Small_Index_Large_Table(void);
			~Small_Index_Large_Table(void);

			Value& operator[](Key key) const;
			void insert(Key key, Value value);
			void remove(Key key);
	};
}

#include "Small_Index_Large_Table.tpp" // implementacja szablonu

#endif // SMALL_INDEX_LARGE_TABLE_HPP_INCLUDED
