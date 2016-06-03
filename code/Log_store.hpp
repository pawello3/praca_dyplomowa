/*
	Paweł Guzewicz, 263664
	Indywidualne Studia Informatyczno-Matematyczne, Uniwersytet Wrocławski
	Praca licencjacka: Analiza teoretyczna i porównawcza efektywnej pamięciowo,
	wysoce wydajnej tablicy asocjacyjnej SILT (Small Index Large Table)
*/

#ifndef LOG_STORE_HPP_INCLUDED
#define LOG_STORE_HPP_INCLUDED

#include <cstdio>
#include <cinttypes>

namespace SILT
{
	struct SILT_key
	{
		uint32_t h0;
		uint32_t h1;
		uint32_t h2;
		uint32_t h3;
		uint32_t h4;
	};

	constexpr uint32_t hash_table_size = 1 << 15; // znacznik 15-bitowy
	constexpr uint8_t bucket_size = 4;
	constexpr uint8_t maximum_number_of_displacements = 128;

	struct Hash_table_entry
	{
		uint16_t tag; // 15 bitów na znacznik i 1 bit (ostatni) zajętości
		uint32_t offset;
	};

	template<typename Key, typename Value>
	class Log_store final
	{
		private:
			char id[32];
			FILE* const log_store_file;
			uint32_t file_size;
			Hash_table_entry** hash_table;
			Hash_table_entry overload;

		public:
			Log_store(void);
			~Log_store(void);

			const Value* operator[](Key key) const;
			bool insert(Key key, Value value); /* zwraca false, gdy nie ma już
			miejsca na wstawienie nowego elementu; wówczas zamiast ponownego
			haszowania tablicy, tworzymy nowy log store; nie robi nic, gdy
			w tablicy znajduje się wartość dla danego klucza */
			void remove(Key key); // nie robi nic, jeśli nie ma takiego klucza

		private:
			static void SHA_1(Key key, uint32_t key_size, SILT_key*
			returned_key);
			bool insert_into_buckets(uint16_t h1, uint32_t h2,
			uint32_t log_file_offset);
	};
}

#include "Log_store.tpp" // implementacja szablonu

#endif // LOG_STORE_HPP_INCLUDED
