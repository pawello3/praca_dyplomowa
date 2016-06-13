/*
	Paweł Guzewicz, 263664
	Indywidualne Studia Informatyczno-Matematyczne, Uniwersytet Wrocławski
	Praca licencjacka: Analiza teoretyczna i porównawcza efektywnej pamięciowo,
	wysoce wydajnej tablicy asocjacyjnej SILT (Small Index Large Table)
*/

#include "Trie.hpp"

SILT::Trie::Trie(void)
	:representation(new uint8_t[initial_size])
	,size(initial_size)
{
}

SILT::Trie::~Trie(void)
{
	delete[] representation;
}

void SILT::Trie::get_offset(const SILT_key& key) const
{
	get_offset(key, 0);
}

void SILT::Trie::build(void)
{
	assert(false); // TODO
}

void SILT::Trie::double_size(void)
{
	assert((size << 1) > size);
	size <<= 1;
    uint8_t* new_array = new uint8_t[size];
    memset(new_array, 0, size);
    memcpy(new_array, representation, (size >> 1));
    delete[] representation;
    representation = new_array;
}

void SILT::Trie::get_offset(const SILT_key& key, uint32_t cursor) const
{
	assert(false && key.h0 && cursor); // TODO
}
