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
#include <cassert>

namespace SILT
{
	struct SILT_key_16;

	struct SILT_key
	{
		uint32_t h0;
		uint32_t h1;
		uint32_t h2;
		uint32_t h3;
		uint32_t h4;

		SILT_key& operator=(const SILT_key_16& rhs);
		uint8_t operator[](const uint8_t index) const;
		friend bool operator==(const SILT_key& lhs, const SILT_key& rhs);
		friend bool operator!=(const SILT_key& lhs, const SILT_key& rhs);
		friend bool operator<(const SILT_key& lhs, const SILT_key& rhs);
		friend bool operator>(const SILT_key& lhs, const SILT_key& rhs);
	};

	inline uint8_t SILT_key::operator[](const uint8_t index) const
	{
		assert(index < 160);
		if(index < 96)
		{
			if(index < 64)
			{
				if(index < 32) // h0
				{
					return (h0 & (1 << (31 - index))) ? 0x01 : 0x00;
				}
				else // h1
				{
					return (h1 & (1 << (31 - (index - 32)))) ? 0x01 : 0x00;
				}
			}
			else // h2
			{
				return (h2 & (1 << (31 - (index - 64)))) ? 0x01 : 0x00;
			}
		}
		else
		{
			if(index < 128) // h3
			{
				return (h3 & (1 << (31 - (index - 96)))) ? 0x01 : 0x00;
			}
			else // h4
			{
				return (h4 & (1 << (31 - (index - 128)))) ? 0x01 : 0x00;
			}
		}
	}

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

	struct SILT_key_16 // SILT_key rozbity na 16-bitowe fragmenty
	{
		uint16_t h0_l;
		uint16_t h0_r;
		uint16_t h1_l;
		uint16_t h1_r;
		uint16_t h2_l;
		uint16_t h2_r;
		uint16_t h3_l;
		uint16_t h3_r;
		uint16_t h4_l;
		uint16_t h4_r;

		SILT_key_16& operator=(const SILT_key& rhs)
		{
			h0_l = (uint16_t) (rhs.h0 >> 16);
			h0_r = (uint16_t) (rhs.h0 & 0x0000FFFF);
			h1_l = (uint16_t) (rhs.h1 >> 16);
			h1_r = (uint16_t) (rhs.h1 & 0x0000FFFF);
			h2_l = (uint16_t) (rhs.h2 >> 16);
			h2_r = (uint16_t) (rhs.h2 & 0x0000FFFF);
			h3_l = (uint16_t) (rhs.h3 >> 16);
			h3_r = (uint16_t) (rhs.h3 & 0x0000FFFF);
			h4_l = (uint16_t) (rhs.h4 >> 16);
			h4_r = (uint16_t) (rhs.h4 & 0x0000FFFF);
			return *this;
		}
		friend bool operator==(const SILT_key_16& lhs, const SILT_key_16& rhs);
		friend bool operator!=(const SILT_key_16& lhs, const SILT_key_16& rhs);
		friend bool operator<(const SILT_key_16& lhs, const SILT_key_16& rhs);
		friend bool operator>(const SILT_key_16& lhs, const SILT_key_16& rhs);
	};

	inline SILT_key& SILT_key::operator=(const SILT_key_16& rhs)
	{
		h0 = (((uint32_t) (rhs.h0_l)) << 16) + ((uint32_t) (rhs.h0_r));
		h1 = (((uint32_t) (rhs.h1_l)) << 16) + ((uint32_t) (rhs.h1_r));
		h2 = (((uint32_t) (rhs.h2_l)) << 16) + ((uint32_t) (rhs.h2_r));
		h3 = (((uint32_t) (rhs.h3_l)) << 16) + ((uint32_t) (rhs.h3_r));
		h4 = (((uint32_t) (rhs.h4_l)) << 16) + ((uint32_t) (rhs.h4_r));
		return *this;
	}

	inline bool operator==(const SILT::SILT_key_16& lhs,
	const SILT::SILT_key_16& rhs)
	{
		return !(memcmp(&lhs, &rhs, sizeof(SILT::SILT_key_16)));
	}

	inline bool operator!=(const SILT::SILT_key_16& lhs,
	const SILT::SILT_key_16& rhs)
	{
		return !(lhs == rhs);
	}

	inline bool operator<(const SILT::SILT_key_16& lhs,
	const SILT::SILT_key_16& rhs)
	{
		return memcmp(&lhs, &rhs, sizeof(SILT::SILT_key_16)) < 0;
	}

	inline bool operator>(const SILT::SILT_key_16& lhs,
	const SILT::SILT_key_16& rhs)
	{
		return rhs < lhs;
	}

	template<typename Key>
	static void SHA_1(Key key, SILT_key* returned_key);
}

#include "SILT_key.tpp" // implementacja szablonu

#endif // SILT_KEY_HPP_INCLUDED
