/*
	Paweł Guzewicz, 263664
	Praca licencjacka: Analiza teoretyczna i porównawcza efektywnej pamięciowo,
	wysoce wydajnej tablicy asocjacyjnej SILT (Small Index Large Table)
*/

#ifndef LOG_STORE_HPP_INCLUDED
#define LOG_STORE_HPP_INCLUDED

#include <cstdio>
#include <cinttypes>

namespace SILT
{
	#define LEFTROTATE_32(a, b) ntohl((((htonl(a) << (b))) | (htonl(a) >> (32-(b)))))
	struct SILT_key
	{
		uint32_t h0;
		uint32_t h1;
		uint32_t h2;
		uint32_t h3;
		uint32_t h4;
	};

	constexpr uint32_t hash_table_size = 16; // TODO: ustawić rozmiar

	template <typename Key, typename Value>
	class Log_store
	{
		public:

		private:
			FILE* log_file;

		public:
			Log_store(void);
			~Log_store(void);

			Value& get(Key key) const;
			void insert(Key key, Value value);
			void remove(Key key);

		private:
			static void SHA_1(Key key, uint32_t key_size, SILT_key*
			returned_key);
			bool look_up(const Key key) const;
	};

}

#endif // LOG_STORE_HPP_INCLUDED
