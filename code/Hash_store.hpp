/*
	Paweł Guzewicz, 263664
	Indywidualne Studia Informatyczno-Matematyczne, Uniwersytet Wrocławski
	Praca licencjacka: Analiza teoretyczna i porównawcza efektywnej pamięciowo,
	wysoce wydajnej tablicy asocjacyjnej SILT (Small Index Large Table)
*/

#ifndef HASH_STORE_HPP_INCLUDED
#define HASH_STORE_HPP_INCLUDED

namespace SILT
{
	template<typename Key, typename Value>
	class Hash_store final
	{
		public:

		private:

		public:
			Hash_store(Log_store* log_store);
			~Hash_store(void);

			Value& get(Key key) const;
			void insert(Key key, Value value);
			void remove(Key key);

		private:
			bool look_up(const Key key) const;
	};
}

#include "Hash_store.tpp" // implementacja szablonu

#endif // HASH_STORE_HPP_INCLUDED
