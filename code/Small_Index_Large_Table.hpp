/*
	Paweł Guzewicz, 263664
	Indywidualne Studia Informatyczno-Matematyczne, Uniwersytet Wrocławski
	Praca licencjacka: Analiza teoretyczna i porównawcza efektywnej pamięciowo,
	wysoce wydajnej tablicy asocjacyjnej SILT (Small Index Large Table)
*/

#ifndef SMALL_INDEX_LARGE_TABLE_HPP_INCLUDED
#define SMALL_INDEX_LARGE_TABLE_HPP_INCLUDED

#include "Log_store.hpp"
#include "Hash_store.hpp"
#include "Sorted_store.hpp"

namespace SILT
{
	template<typename Key, typename Value>
	class Small_Index_Large_Table final
	{
		private:
			Log_store<Key, Value>* log_store;
			Hash_store_list<Key, Value>* hash_store_list;
			Sorted_store<Key, Value>* sorted_store;

		public:
			Small_Index_Large_Table(void);
			~Small_Index_Large_Table(void);

			Value& operator[](Key key) const;
			void insert(Key key, Value value);
			void remove(Key key);
	};
}

#include "Small_Index_Large_Table.tpp" // implementacja szablonu

#endif // SMALL_INDEX_LARGE_TABLE_HPP_INCLUDED
