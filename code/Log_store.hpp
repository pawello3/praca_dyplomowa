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
#include "SILT_key.hpp"

namespace SILT
{
	constexpr uint32_t hash_table_size = 1 << 14; // znacznik 14-bitowy
	constexpr uint8_t bucket_size = 4;
	constexpr uint8_t maximum_number_of_displacements = 64;
	constexpr uint32_t h1_mask = 0x0000FFFC; // 2 ostatnie bity
	constexpr uint32_t h2_mask = 0xFFFC0000; // ustawione na 0
	constexpr uint16_t valid_bit = 0x0001;
	constexpr uint16_t operation_bit = 0x0002;

	struct Hash_table_entry
	{
		uint16_t tag;
		/*
			format (16): TTTTTTTTTTTTTTOV
			(14) T - bity znacznika
			(1) O - bit rodzaju operacji (0 - usuwanie, 1 - wstawianie)
			(1) V - bit zajętości miejsca w kubełku (0 - wolne, 1 - zajęte)
		*/
		uint32_t offset; // liczba par klucz-wartość poprzedzających dany wpis
	};

	struct Undo_entry
	{
		uint16_t h;
		uint8_t number;
		bool which;
	};

	template<typename Value>
	class Hash_store;

	template<typename Value>
	class Log_store final
	{
		friend class Hash_store<Value>;

		private:
			char id[32];
			Hash_table_entry** hash_table;
			const uint32_t log_entry_size;
			FILE* const log_store_file;
			uint32_t file_size;

		public:
			Log_store(void);
			~Log_store(void);

			bool insert(const SILT_key& key, const Value& value); /* zwraca
			false, gdy nie ma już miejsca na wstawienie nowego elementu;
			w przeciwnym przypadku wstawia, nawet, gdy w tablicy znajduje się
			wartość dla danego klucza (podmienia) */
			void remove(const SILT_key& key); /* jeśli klucz znajduje się
			w tablicy, to zaznacza go jako usunięty; w przeciwnym przypadku nie
			robi nic */
			Value* get_value(const SILT_key& key, bool* reason) const; /* zwraca
			nullptr, gdy nie znaleziono klucza i w zmiennej reason zapisuje
			przyczynę: 0 - brak klucza, 1 - usunięty; w przeciwnym przypadku
			zwraca wskaźnik do zmiennej przydzielonej na stercie zawierającej
			poszukiwaną wartość*/

		private:
			bool look_up(uint16_t h_index, uint16_t h_tag,
			uint8_t* bucket_index) const; /* zwraca true, gdy w kubełku
			o indeksie h_index znajduje się klucz o znaczniku h_tag, w zmiennej
			bucket_index zapisywana jest pozycja w kubełku; w przeciwnym
			przypadku zwraca false i zawartość zmiennej bucket_index jest nie
			zdefiniowana */
			bool check_key_matching(const SILT_key& key,
			uint32_t log_file_offset) const; /* zwraca true, gdy szukany klucz
			key jest zapamiętany w pliku na dysku z przesunięciem
			log_file_offset; w przeciwnym przypadku zwraca false */
			void modify_entry_in_log_store_file(const SILT_key& key,
			const Value& value, uint32_t log_file_offset); /* zmienia wpis
			w pliku na dysku na nową parę klucz-wartość */
			bool insert_into_bucket(uint16_t h_index, uint16_t h_tag,
			uint32_t log_file_offset); /* zwraca true, gdy udało się wstawić
			do kubełka o indeksie h_index klucz o tagu h_tag i przesunięciu
			w pliku równemu log_file_offset; w przeciwnym przypadku zwraca
			false */
			void append_entry_to_log_store_file(const SILT_key& key,
			const Value& value); /* dopisuje wpis w pliku na dysku z nową parą
			klucz-wartość */
	};
}

#include "Log_store.tpp" // implementacja szablonu

#endif // LOG_STORE_HPP_INCLUDED
