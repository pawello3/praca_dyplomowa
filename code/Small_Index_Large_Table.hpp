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

/*
	Zakładamy, że typ Key implementuje operator==, a Value posiada domyślny
	konstruktor bezargumentowy. W C++17 można takie wymagania łatwo wyrazić
	za pomocą konceptów.
*/

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

			void insert(const Key& key, const Value& value);
			void remove(const Key& key);
			Value* operator[](const Key& key) const; /* zwraca nullptr, gdy nie
			ma klucza w tablicy SILT */
	};
}

#include "Small_Index_Large_Table.tpp" // implementacja szablonu

#endif // SMALL_INDEX_LARGE_TABLE_HPP_INCLUDED
