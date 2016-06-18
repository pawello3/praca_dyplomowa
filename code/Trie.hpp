/*
	Paweł Guzewicz, 263664
	Indywidualne Studia Informatyczno-Matematyczne, Uniwersytet Wrocławski
	Praca licencjacka: Analiza teoretyczna i porównawcza efektywnej pamięciowo,
	wysoce wydajnej tablicy asocjacyjnej SILT (Small Index Large Table)
*/

#ifndef TRIE_HPP_INCLUDED
#define TRIE_HPP_INCLUDED

#include <cinttypes>
#include <cstdio>
#include "SILT_key.hpp"

namespace SILT
{
	constexpr uint32_t initial_size = 1 << 16;

	template<typename Value>
	class Trie final
	{
		private:
			FILE* const sorted_store_file;
			uint32_t file_size;
			uint32_t* representation;
			uint32_t representation_size;
			uint32_t representation_array_size;
			SILT_key* array_to_be_indexed;

		public:
			Trie(FILE* const sorted_store_file, uint32_t file_name);
			~Trie(void);

			uint32_t get_offset(const SILT_key& key) const;

		private:
			void load_file_content_to_array(void);
			void build_trie(uint8_t depth, uint32_t left, uint32_t right);
			uint32_t partition_keys(uint8_t i, uint32_t range_left,
			uint32_t range_right); /* i - pozycja w kluczu 160-bitowym, względem
			której dzielimy klucze na dwa podzbiory, zakres indkesów z tablicy
			to [range_left, range_right); zwraca pozycję w tablicy, na której
			zaczynają się klucze z i-tą pozycją równą 1, gdy takich nie ma to
			zwraca rozmiar tablicy */
			void double_size(void);
			uint32_t get_offset(const SILT_key& key, uint32_t cursor,
			uint32_t depth) const;
			uint32_t discard_left_subtrie(uint32_t cursor) const;
	};
}

#include "Trie.tpp" // implementacja szablonu

#endif // TRIE_HPP_INCLUDED
