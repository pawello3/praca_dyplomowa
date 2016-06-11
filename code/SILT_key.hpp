/*
	Paweł Guzewicz, 263664
	Indywidualne Studia Informatyczno-Matematyczne, Uniwersytet Wrocławski
	Praca licencjacka: Analiza teoretyczna i porównawcza efektywnej pamięciowo,
	wysoce wydajnej tablicy asocjacyjnej SILT (Small Index Large Table)
*/

#ifndef SILT_KEY_HPP_INCLUDED
#define SILT_KEY_HPP_INCLUDED

#include <cinttypes>
#include <cstring>

namespace SILT
{
	struct SILT_key
	{
		uint32_t h0;
		uint32_t h1;
		uint32_t h2;
		uint32_t h3;
		uint32_t h4;

		friend bool operator==(const SILT_key& lhs, const SILT_key& rhs);
		friend bool operator!=(const SILT_key& lhs, const SILT_key& rhs);
		friend bool operator<(const SILT_key& lhs, const SILT_key& rhs);
		friend bool operator>(const SILT_key& lhs, const SILT_key& rhs);
	};

	inline bool operator==(const SILT::SILT_key& lhs, const SILT::SILT_key& rhs)
	{
		return !(memcmp(&lhs, &rhs, sizeof(SILT::SILT_key)));
	}

	inline bool operator!=(const SILT::SILT_key& lhs, const SILT::SILT_key& rhs)
	{
		return !(lhs == rhs);
	}

	inline bool operator<(const SILT::SILT_key& lhs, const SILT::SILT_key& rhs)
	{
		return memcmp(&lhs, &rhs, sizeof(SILT::SILT_key)) < 0;
	}

	inline bool operator>(const SILT::SILT_key& lhs, const SILT::SILT_key& rhs)
	{
		return rhs < lhs;
	}

	template<typename Key>
	static void SHA_1(Key key, SILT_key* returned_key);
}

#include "SILT_key.tpp" // implementacja szablonu

#endif // SILT_KEY_HPP_INCLUDED
