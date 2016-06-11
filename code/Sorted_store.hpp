/*
	Paweł Guzewicz, 263664
	Indywidualne Studia Informatyczno-Matematyczne, Uniwersytet Wrocławski
	Praca licencjacka: Analiza teoretyczna i porównawcza efektywnej pamięciowo,
	wysoce wydajnej tablicy asocjacyjnej SILT (Small Index Large Table)
*/

#ifndef SORTED_STORE_HPP_INCLUDED
#define SORTED_STORE_HPP_INCLUDED

#include "SILT_key.hpp"
#include "Hash_store.hpp"

namespace SILT
{
	template<typename Value>
	class Sorted_store final
	{
		private:
			FILE* const sorted_store_file;

		public:
			Sorted_store(Hash_store_list<Value>* hash_store_list,
			Sorted_store<Value>* old_sorted_store);
			~Sorted_store(void);

			Value* get_value(const SILT_key& key) const;
	};
}

#include "Sorted_store.tpp" // implementacja szablonu

#endif // SORTED_STORE_HPP_INCLUDED
