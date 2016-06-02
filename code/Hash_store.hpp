/*
	Paweł Guzewicz, 263664
	Indywidualne Studia Informatyczno-Matematyczne, Uniwersytet Wrocławski
	Praca licencjacka: Analiza teoretyczna i porównawcza efektywnej pamięciowo,
	wysoce wydajnej tablicy asocjacyjnej SILT (Small Index Large Table)
*/

#ifndef HASH_STORE_HPP_INCLUDED
#define HASH_STORE_HPP_INCLUDED

#include <cstdio>
#include "Log_store.hpp"

namespace SILT
{
	template<typename Key, typename Value>
	class Hash_store final
	{
		private:
			static uint32_t first_free_id;
			FILE* const hash_store_file;

		public:
			Hash_store(Log_store<Key, Value>* log_store);
			~Hash_store(void);

			Value& operator[](Key key) const;
			void insert(Key key, Value value);
			void remove(Key key);

		private:
			char* next_id(void);
			bool look_up(const Key key) const;
	};

	template<typename Key, typename Value>
	class Hash_store_list_node final
	{
		private:
			Hash_store<Key, Value>* data;
			Hash_store_list_node<Key, Value>* next;

		public:
			Hash_store_list_node<Key, Value>(void);
			~Hash_store_list_node<Key, Value>(void);

			Hash_store<Key, Value>* get_data(void) const;
			Hash_store_list_node<Key, Value>* get_next(void) const;
			void set_next(Hash_store_list_node<Key, Value>* node);
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
			void prepend(Hash_store_list<Key, Value>* node);
			void remove(Hash_store_list<Key, Value>* node);
			uint32_t get_size(void) const;
	};
}

#include "Hash_store.tpp" // implementacja szablonu

#endif // HASH_STORE_HPP_INCLUDED
