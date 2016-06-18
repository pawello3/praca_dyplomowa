/*
	Paweł Guzewicz, 263664
	Indywidualne Studia Informatyczno-Matematyczne, Uniwersytet Wrocławski
	Praca licencjacka: Analiza teoretyczna i porównawcza efektywnej pamięciowo,
	wysoce wydajnej tablicy asocjacyjnej SILT (Small Index Large Table)
*/

#include <new>

template<typename Value>
SILT::Trie<Value>::Trie(FILE* const sorted_store_file, uint32_t file_size)
	:sorted_store_file(sorted_store_file)
	,file_size(file_size)
	,representation(new uint32_t[initial_size])
	,representation_size(0)
	,representation_array_size(initial_size)
{
	load_file_content_to_array();
	build_trie(0, 0, file_size);
	delete[] array_to_be_indexed;
}

template<typename Value>
SILT::Trie<Value>::~Trie(void)
{
	delete[] representation;
}

template<typename Value>
void SILT::Trie<Value>::load_file_content_to_array(void)
{
	array_to_be_indexed = new(std::nothrow) SILT_key[file_size];
	if(array_to_be_indexed == nullptr)
	{
		fprintf(stderr, "Sorted store too big, not enough RAM space\n");
		exit(1);
	}
	SILT_key key;
	for(uint32_t i = 0; i < file_size; i++)
	{
		fseek(sorted_store_file, i * (sizeof(SILT_key) + sizeof(Value)),
		SEEK_SET);
		if(fread((void*) &key, sizeof(SILT_key), 1, sorted_store_file) != 1)
		{
			fprintf(stderr, "fread error\n");
			exit(1);
		}
		array_to_be_indexed[i] = key;
	}
}

template<typename Value>
void SILT::Trie<Value>::build_trie(uint8_t depth, uint32_t left,
uint32_t right)
{
	uint32_t middle = partition_keys(depth, left, right);
	if(representation_size + 2 > representation_array_size)
		double_size();
	if(middle == left)
	{
		representation[representation_size++] = 0;
		build_trie(depth + 1, left, right);
	}
	else if(middle == right)
	{
		representation[representation_size++] = right - left;
		build_trie(depth + 1, left, right);
	}
	else
	{
		if(left + 1 != middle)
		{
			representation[representation_size++] = middle - left;
			build_trie(depth + 1, left, middle);
		}
		else
		{
			representation[representation_size++] = 1;
		}
		if(middle + 1 != right)
		{
			build_trie(depth + 1, middle, right);
		}
	}
}

template<typename Value>
uint32_t SILT::Trie<Value>::partition_keys(uint8_t i, uint32_t range_left,
uint32_t range_right)
{
	uint32_t left = range_left, right = range_right - 1;
	assert(left < right);
	SILT_key tmp;
	while(array_to_be_indexed[left][i] == 0)
	{
		if(left == right)
			return range_right;
		left++;
	}
	// left zawiera teraz pierwszy indeks tablicy, w którym jest 1
	while(array_to_be_indexed[right][i] == 1)
	{
		if(right == range_left)
			return range_left;
		right--;
	}

	while(true)
	{
		while(array_to_be_indexed[left][i] == 0)
			left++;
		// left zawiera teraz pierwszy indeks tablicy, w którym jest 1

		while(array_to_be_indexed[right][i] == 1)
			right--;
		// right zawiera teraz pierwszy indeks tablicy, w którym jest 0

		assert(left != right);
		if(left > right)
			return left;

		assert(false); // nie powinien tu wejść, gdyż klucze są posortowane

		// zamiana kluczy pod indeksami left i right
		tmp = array_to_be_indexed[left];
		array_to_be_indexed[left] = array_to_be_indexed[right];
		array_to_be_indexed[right] = tmp;
	}
}

template<typename Value>
void SILT::Trie<Value>::double_size(void)
{
	assert((representation_array_size << 1) > representation_array_size);
	representation_array_size <<= 1;
    uint32_t* new_array = new uint32_t[representation_array_size];
    memset(new_array, 0, representation_array_size);
    memcpy(new_array, representation, (representation_array_size >> 1));
    delete[] representation;
    representation = new_array;
}

template<typename Value>
uint32_t SILT::Trie<Value>::get_offset(const SILT_key& key) const
{
	return get_offset(key, 0, 0);
}

template<typename Value>
uint32_t SILT::Trie<Value>::get_offset(const SILT_key& key, uint32_t cursor,
uint32_t depth) const
{
	if(key[depth] == 0)
	{
		if(representation[cursor] == 0 || representation[cursor] == 1)
			return 0;
		return get_offset(key, cursor + 1, depth + 1);

	}
	else // key[depth] == 1
	{
		uint32_t left_distance = representation[cursor];
		cursor = discard_left_subtrie(cursor);
		return left_distance + get_offset(key, cursor, depth + 1);
	}
}

template<typename Value>
uint32_t SILT::Trie<Value>::discard_left_subtrie(uint32_t cursor) const
{
	if(representation[cursor] == 0 || representation[cursor] == 1)
		return cursor + 1;
	cursor = discard_left_subtrie(cursor + 1);
	return discard_left_subtrie(cursor + 1);
}
