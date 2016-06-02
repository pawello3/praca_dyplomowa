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

	constexpr uint32_t hash_table_size = 16; // TODO: ustawić rozmiar

	template<typename Key, typename Value>
	class Log_store final
	{
		private:
			FILE* log_store_file;

		public:
			Log_store(void);
			~Log_store(void);

			Value& operator[](Key key) const;
			void insert(Key key, Value value);
			void remove(Key key);

		private:
			static void SHA_1(Key key, uint32_t key_size, SILT_key*
			returned_key);
	};
}

#include "Log_store.tpp" // implementacja szablonu

#endif // LOG_STORE_HPP_INCLUDED
