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

	constexpr uint32_t hash_table_size = 1 << 14; // znacznik 14-bitowy
	constexpr uint8_t bucket_size = 4;
	constexpr uint8_t maximum_number_of_displacements = 128;
	constexpr uint32_t h1_mask = 0x0000FFFC;
	constexpr uint32_t h2_mask = 0xFFFC0000;
	// 2 ostatnie bity na 0
	constexpr uint16_t operation_bit = 0x0002;
	constexpr uint16_t valid_bit = 0x0001;

	struct Hash_table_entry
	{
		uint16_t tag;
		// format: TTTTTTTTTTTTTTDV
		// (14) T - bity znacznika
		// (1) D - bit rodzaju operacji (0 - usuwanie, 1 - wstawianie)
		// (1) V - bit zajętości miejsca w kubełku (0 - wolne, 1 - zajęte)
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

			Value* operator[](Key key) const; /* zwraca nullptr, gdy nie
			znaleziono klucza */
			bool insert(Key key, Value value); /* zwraca false, gdy nie ma już
			miejsca na wstawienie nowego elementu; wówczas zamiast ponownego
			haszowania tablicy, tworzymy nowy log store; wstawia nawet, gdy
			w tablicy znajduje się wartość dla danego klucza */
			void remove(Key key); // nie robi nic, jeśli nie ma takiego klucza

		private:
			static void SHA_1(Key key, uint32_t key_size, SILT_key*
			returned_key);
			bool insert_into_buckets(uint16_t h1, uint32_t h2,
			uint32_t log_file_offset, bool operation);
	};
}

#include "Log_store.tpp" // implementacja szablonu

#endif // LOG_STORE_HPP_INCLUDED
