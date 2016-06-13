/*
	Paweł Guzewicz, 263664
	Indywidualne Studia Informatyczno-Matematyczne, Uniwersytet Wrocławski
	Praca licencjacka: Analiza teoretyczna i porównawcza efektywnej pamięciowo,
	wysoce wydajnej tablicy asocjacyjnej SILT (Small Index Large Table)
*/

#ifndef SORTED_STORE_HPP_INCLUDED
#define SORTED_STORE_HPP_INCLUDED

#include "SILT_key.hpp"
#include "Hash_store.hpp"
#include "Merge_heap.hpp"
#include "Trie.hpp"

namespace SILT
{
	template<typename Value>
	class Sorted_store final
	{
		private:
			char file_name[32];
			FILE* const sorted_store_file;
			uint32_t file_size; // liczba wpisów w pliku
			const uint8_t sorted_hash_store_entry_size;
			const uint8_t sorted_store_entry_size;
			Trie* trie_buckets; // 2^16 kubełków

		public:
			Sorted_store(Hash_store<Value>** hash_stores_array,
			Sorted_store<Value>* old_sorted_store);
			~Sorted_store(void);

			Value* get_value(const SILT_key& key) const;

		private:
			void merge(Hash_store<Value>** hash_stores_array,
			Sorted_store<Value>* old_sorted_store);
			void insert_into_heap(Hash_store<Value>** hash_stores_array,
			Sorted_store<Value>* old_sorted_store, Merge_heap* heap,
			Sorted_hash_store_entry* array, uint32_t* last_read_position,
			uint8_t number);
			uint8_t get_top_of_heap_and_append_to_file(Hash_store<Value>**
			hash_stores_array, Sorted_store<Value>* old_sorted_store,
			Merge_heap* heap, uint32_t* first_unread_position,
			Sorted_hash_store_entry* returned_entry);
			void build_trie_indexing(void);
	};
}

#include "Sorted_store.tpp" // implementacja szablonu

#endif // SORTED_STORE_HPP_INCLUDED
