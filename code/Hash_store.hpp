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
	constexpr uint8_t maximum_number_of_hash_stores = 15;

	struct Sorted_table_entry
	{
		SILT_key key;
		uint16_t offset;
		uint8_t operation; // zapamiętuje wartość operation_bit
	};

	template<typename Value>
	class Hash_store final
	{
		private:
			uint16_t** hash_table;
			const uint8_t log_file_entry_size;
			static uint8_t first_free_id;
			char file_name[32];
			FILE* hash_store_file;

		public:
			Hash_store(Log_store<Value>* log_store);
			~Hash_store(void);

			Value* get_value(const SILT_key& key, bool* reason) const; /* ma
			zdefiniowane zachowanie do momentu posortowania danych w pliku */
			void sort(void);

		private:
			char* next_name(void);
			void radix_sort(Sorted_table_entry* buckets_table,
			Sorted_table_entry* sorted_table, uint32_t sorted_table_size,
			uint16_t* count);
			void counting_sort(Sorted_table_entry* buckets_table,
			Sorted_table_entry* sorted_table, uint32_t sorted_table_size,
			uint8_t fragment, uint16_t* count);
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
			uint8_t size;

		public:
			Hash_store_list(void);
			~Hash_store_list(void);

			Hash_store_list_node<Value>* get_head(void) const;
			void prepend(Hash_store_list<Value>* node);
			void remove(Hash_store_list<Value>* node);
			uint8_t get_size(void) const;
	};
}

#include "Hash_store.tpp" // implementacja szablonu

#endif // HASH_STORE_HPP_INCLUDED
