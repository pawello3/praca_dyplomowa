/*
	Paweł Guzewicz, 263664
	Indywidualne Studia Informatyczno-Matematyczne, Uniwersytet Wrocławski
	Praca licencjacka: Analiza teoretyczna i porównawcza efektywnej pamięciowo,
	wysoce wydajnej tablicy asocjacyjnej SILT (Small Index Large Table)
*/

#ifndef MERGE_HEAP_HPP_INCLUDED
#define MERGE_HEAP_HPP_INCLUDED

#include "Hash_store.hpp"

namespace SILT
{
	class Merge_heap final
	{
		private:
			Sorted_hash_store_entry* heap_array;
			uint8_t array_size;
			uint8_t heap_size;

		public:
			Merge_heap(uint8_t size);
			~Merge_heap(void);

			void insert(const Sorted_hash_store_entry& element);
			uint8_t get_size(void);
			Sorted_hash_store_entry& get_top(void);
			bool remove_duplicate(Sorted_hash_store_entry& element,
			uint8_t* hash_store_number); /* usuwa pierwszy element w kopcu,
			który ma taki sam klucz jak zmienna element w argumencie i zwraca
			true, w argumencie hash_store_number zapisywany jest numer
			hash_store'u, z którego do kopca powinno się dołożyć nowy element;
			jeśli nie ma nic do usunięcia, to zwraca false i naprawia własność
			kopca, a wartość zmiennej hash_store_number jest nie zdefiniowana */

		private:
			static uint8_t left_child(uint8_t position);
			static uint8_t right_child(uint8_t position);
			static int16_t parent(int16_t position);
			void swap(uint8_t position1, uint8_t position2);
			void move_up(uint8_t position);
			void move_down(uint8_t position);
			void build_heap();
	};
}

#endif // MERGE_HEAP_HPP_INCLUDED
