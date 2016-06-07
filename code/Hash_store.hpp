/*
	Paweł Guzewicz, 263664
	Indywidualne Studia Informatyczno-Matematyczne, Uniwersytet Wrocławski
	Praca licencjacka: Analiza teoretyczna i porównawcza efektywnej pamięciowo,
	wysoce wydajnej tablicy asocjacyjnej SILT (Small Index Large Table)
*/

#ifndef HASH_STORE_HPP_INCLUDED
#define HASH_STORE_HPP_INCLUDED

#include <cstdio>
#include "SILT_key.hpp"
#include "Log_store.hpp"

namespace SILT
{
	template<typename Value>
	class Hash_store final
	{
		private:
			static uint32_t first_free_id;
			char id[32];
			uint16_t** hash_table;
			const uint32_t log_entry_size;
			FILE* const hash_store_file;

		public:
			Hash_store(Log_store<Value>* log_store);
			~Hash_store(void);

			Value* get_value(const SILT_key& key, bool* reason) const;

		private:
			char* next_id(void);
	};

////////////////////////////////////////////////////////////////////////////////

	template<typename Value>
	class Hash_store_list;

	template<typename Value>
	class Hash_store_list_node final
	{
		friend class Hash_store_list<Value>;

		private:
			Hash_store<Value>* data;
			Hash_store_list_node<Value>* previous;
			Hash_store_list_node<Value>* next;

		public:
			Hash_store_list_node(Hash_store<Value>* data);
			~Hash_store_list_node(void);

			Hash_store<Value>* get_data(void) const;
			Hash_store_list_node<Value>* get_next(void) const;
	};

////////////////////////////////////////////////////////////////////////////////

	template<typename Value>
	class Hash_store_list final
	{
		private:
			Hash_store_list_node<Value>* head;
			uint32_t size;

		public:
			Hash_store_list(void);
			~Hash_store_list(void);

			Hash_store_list_node<Value>* get_head(void) const;
			void prepend(Hash_store_list<Value>* node);
			void remove(Hash_store_list<Value>* node);
			uint32_t get_size(void) const;
	};
}

#include "Hash_store.tpp" // implementacja szablonu

#endif // HASH_STORE_HPP_INCLUDED
