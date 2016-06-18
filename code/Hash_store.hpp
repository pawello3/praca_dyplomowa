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
	constexpr uint8_t maximum_number_of_hash_stores = 15; /* nie więcej niż
	2^7 - 1, patrz: Sorted_hash_store_entry */

	struct Sorted_table_entry
	{
		SILT_key_16 key_16;
		uint16_t offset;
		uint8_t operation; // zapamiętuje wartość operation_bit
	};

	constexpr uint8_t entry_operation_bit = 0x01;

	struct Sorted_hash_store_entry
	{
		SILT_key key;
		uint8_t hash_store_number_and_operation;
		/*
			format (8): LLLLLLLO
			(7) L - bity numeru Hash store'u (maksymalny 2^7 - 1)
			(1) O - bit rodzaju operacji (0 - usuwanie, 1 - wstawianie)
		*/
		friend bool operator<(const Sorted_hash_store_entry& lhs,
		const Sorted_hash_store_entry& rhs);
		friend bool operator>(const Sorted_hash_store_entry& lhs,
		const Sorted_hash_store_entry& rhs);
	};

	inline bool operator<(const Sorted_hash_store_entry& lhs,
	const Sorted_hash_store_entry& rhs)
	{
		if(lhs.key != rhs.key)
			return lhs.key < rhs.key;
		return (lhs.hash_store_number_and_operation >> 1)
		< (rhs.hash_store_number_and_operation >> 1);
	}

	inline bool operator>(const Sorted_hash_store_entry& lhs,
	const Sorted_hash_store_entry& rhs)
	{
		return rhs < lhs;
	}

	template<typename Value>
	class Sorted_store;

	template<typename Value>
	class Hash_store final
	{
		friend class Sorted_store<Value>;
		private:
			uint16_t** hash_table;
			const uint8_t log_file_entry_size;
			static uint8_t first_free_id;
			char file_name[32];
			FILE* hash_store_file;
			uint8_t file_size;

		public:
			Hash_store(Log_store<Value>* log_store);
			~Hash_store(void);

			static void reset(void);

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
}

#include "Hash_store.tpp" // implementacja szablonu

#endif // HASH_STORE_HPP_INCLUDED
