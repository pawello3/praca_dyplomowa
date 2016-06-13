/*
	Paweł Guzewicz, 263664
	Indywidualne Studia Informatyczno-Matematyczne, Uniwersytet Wrocławski
	Praca licencjacka: Analiza teoretyczna i porównawcza efektywnej pamięciowo,
	wysoce wydajnej tablicy asocjacyjnej SILT (Small Index Large Table)
*/

#ifndef TRIE_HPP_INCLUDED
#define TRIE_HPP_INCLUDED

#include <cinttypes>
#include "SILT_key.hpp"

namespace SILT
{
	constexpr uint32_t initial_size = 1 << 16;

	class Trie final
	{
		private:
			uint8_t* representation;
			uint32_t size;

		public:
			Trie(void);
			~Trie(void);

			void get_offset(const SILT_key& key) const;

		private:
			void build(void);
			void double_size(void);
			void get_offset(const SILT_key& key, uint32_t cursor) const;
	};
}

#endif // TRIE_HPP_INCLUDED
