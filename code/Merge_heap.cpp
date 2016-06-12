/*
	Paweł Guzewicz, 263664
	Indywidualne Studia Informatyczno-Matematyczne, Uniwersytet Wrocławski
	Praca licencjacka: Analiza teoretyczna i porównawcza efektywnej pamięciowo,
	wysoce wydajnej tablicy asocjacyjnej SILT (Small Index Large Table)
*/

#include "Merge_heap.hpp"

SILT::Merge_heap::Merge_heap(uint8_t size)
	:heap_array(new Sorted_hash_store_entry[size])
	,array_size(size)
	,heap_size(0)
{
}

SILT::Merge_heap::~Merge_heap(void)
{
	assert(heap_size == 0);
	delete[] heap_array;
}

void SILT::Merge_heap::insert(const Sorted_hash_store_entry& element)
{
	assert(heap_size < array_size);
	heap_array[heap_size] = element;
	move_up(heap_size);
	heap_size++;
}

uint8_t SILT::Merge_heap::get_size(void)
{
	return heap_size;
}

SILT::Sorted_hash_store_entry& SILT::Merge_heap::get_top(void)
{
	heap_size--;
	swap(0, heap_size);
	move_down(0);
	return heap_array[heap_size];
}

bool SILT::Merge_heap::remove_duplicate(Sorted_hash_store_entry& element,
uint8_t* hash_store_number)
{
	for(uint8_t i = 0; i < heap_size; i++)
	{
		if(heap_array[i].key == element.key)
		{
			*hash_store_number
			= heap_array[i].hash_store_number_and_operation >> 1;
			swap(i, heap_size - 1);
			heap_size--;
			return true;
		}
	}
	build_heap();
	return false;
}

uint8_t SILT::Merge_heap::left_child(uint8_t position)
{
	return (position << 1) + 1;
}

uint8_t SILT::Merge_heap::right_child(uint8_t position)
{
	return (position << 1) + 2;
}

int16_t SILT::Merge_heap::parent(int16_t position)
{
	return (position + 1) / 2 - 1;
}

void SILT::Merge_heap::swap(uint8_t position1, uint8_t position2)
{
	Sorted_hash_store_entry temporary = heap_array[position1];
	heap_array[position1] = heap_array[position2];
	heap_array[position2] = temporary;
}

void SILT::Merge_heap::move_up(uint8_t position)
{
	int16_t parent_position = parent(position);
	if(parent_position == -1)
		return;
	if(heap_array[parent_position] > heap_array[position])
	{
		swap(position, parent_position);
		move_up(parent_position);
	}
}

void SILT::Merge_heap::move_down(uint8_t position)
{
	uint8_t left = left_child(position);
	uint8_t right = right_child(position);
	if(right < heap_size) // 2 synów
	{
		uint8_t top = position;
		if(heap_array[top] > heap_array[left])
			top = left;
		if(heap_array[top] > heap_array[right])
			top = right;
		if(top != position)
		{
			swap(position, top);
			move_down(top);
		}
	}
	else if(left < heap_size) // 1 syn
	{
		if(heap_array[position] > heap_array[left])
		{
			swap(position, left);
			move_down(left);
		}
	}
	else // brak synów
	{
		return;
	}
}

void SILT::Merge_heap::build_heap()
{
	for(int16_t i = heap_size / 2; i >= 0; i--)
		move_down(i);
}
