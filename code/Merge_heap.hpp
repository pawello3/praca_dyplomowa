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
	struct Merge_heap_element
	{
		Sorted_table_entry entry;
		uint8_t list_number;

		friend bool operator<(const Merge_heap_element& lhs,
		const Merge_heap_element& rhs);
		friend bool operator>(const Merge_heap_element& lhs,
		const Merge_heap_element& rhs);
	};

	inline bool operator<(const Merge_heap_element& lhs,
	const Merge_heap_element& rhs)
	{
		if(lhs.entry.key != rhs.entry.key)
			return lhs.entry.key < rhs.entry.key;
		return lhs.list_number < rhs.list_number;
	}

	inline bool operator>(const Merge_heap_element& lhs,
	const Merge_heap_element& rhs)
	{
		return rhs < lhs;
	}

	class Merge_heap final
	{
		private:
			Merge_heap_element* heap_array;
			uint8_t array_size;
			uint8_t heap_size;

		public:
			Merge_heap(uint8_t size);
			~Merge_heap(void);

			void insert(Merge_heap_element& element);
			Merge_heap_element& get_top(void);
			bool remove_duplicate(Merge_heap_element& element,
			uint8_t* list_number); /* usuwa pierwszy element w kopcu, który ma
			taki sam klucz jak zmienna element w argumencie i zwraca true,
			w argumencie list_number zapisywany jest numer listy, z której do
			kopca powinno się dołożyć nowy element; jeśli nie ma nic do
			usunięcia, to zwraca false, a wartość zmiennej list_number jest
			nie zdefiniowana */

		private:
			uint8_t left_child(uint8_t position);
			uint8_t right_child(uint8_t position);
			uint8_t parent(unsigned int position);
			void swap(uint8_t position1, uint8_t position2);
			void move_up(uint8_t position);
			void move_down(uint8_t position);
			void build_heap();
	};
}

#endif // MERGE_HEAP_HPP_INCLUDED
