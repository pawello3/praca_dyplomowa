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
	DEBUG(PRINT_UINT_32(representation_size));
	/*for(uint32_t i = 0; i < representation_size; i++)
	{
		DEBUG(PRINT_UINT_32(representation[i]));
	}*/
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
	for(uint32_t i = 0; i < file_size; i++)
	{
		fseek(sorted_store_file, i * (sizeof(SILT_key) + sizeof(Value)),
		SEEK_SET);
		if(fread((void*) &array_to_be_indexed[i], sizeof(SILT_key), 1,
		sorted_store_file) != 1)
		{
			fprintf(stderr, "fread error\n");
			exit(1);
		}
	}
}

template<typename Value>
void SILT::Trie<Value>::build_trie(uint8_t depth, uint32_t left,
uint32_t right)
{
	uint32_t middle = partition_keys(depth, left, right);
	if(representation_size + 2 > representation_array_size)
	{
		DEBUG(PRINT_TEXT("Podwajanie"));
		double_size();
	}
	if(middle == left) // same jedynki
	{
		representation[representation_size++] = 0;
		build_trie(depth + 1, left, right);
	}
	else if(middle == right) // same zera
	{
		representation[representation_size++] = right - left;
		build_trie(depth + 1, left, right);
	}
	else
	{
		if(left + 1 != middle) // w lewym synu jest poddrzewo
		{
			representation[representation_size++] = middle - left;
			build_trie(depth + 1, left, middle);
		}
		else // lewy syn to liść
		{
			representation[representation_size++] = 1;
		}
		if(middle + 1 != right) // w prawym synu jest poddrzewo
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
	// right zawiera teraz pierwszy indeks tablicy, w którym jest 0

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

		DEBUG(PRINT_UINT_32(left));
		DEBUG(PRINT_UINT_32(right));
		DEBUG(PRINT_UINT_32(range_left));
		DEBUG(PRINT_UINT_32(range_right));
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
    memset(new_array, 0, representation_array_size << 2);
    memcpy(new_array, representation, ((representation_array_size >> 1) << 2));
    delete[] representation;
    representation = new_array;
}

template<typename Value>
uint32_t SILT::Trie<Value>::get_offset(const SILT_key& key) const
{
	DEBUG(for(uint32_t i = 0; i < 15; i++)
	{
		PRINT_UINT_32(representation[i]);
	});
	return get_offset(key, 0, 0, file_size - representation[0]);
}

template<typename Value>
uint32_t SILT::Trie<Value>::get_offset(const SILT_key& key, uint32_t depth,
uint32_t cursor, uint32_t nodes_on_right) const
{
	assert(cursor < representation_size);
	DEBUG(PRINT_TEXT("Kursor:"); PRINT_UINT_32(cursor);
	PRINT_TEXT("Liści na lewo:"); PRINT_UINT_32(representation[cursor]);
	PRINT_TEXT("Liści na prawo:"); PRINT_UINT_32(nodes_on_right);
	PRINT_TEXT("Głębokość:"); PRINT_UINT_32(depth);
	PRINT_TEXT("Bit:"); PRINT_UINT_32(key[depth]));
	if(key[depth] == 0)
	{
		if(representation[cursor] == 0 || representation[cursor] == 1) /* lewy
		liść na najniższym poziomie */
			return 0;
		return get_offset(key, depth + 1, cursor + 1, representation[cursor]
		- representation[cursor + 1]);
	}
	else // key[depth] == 1
	{
		if(nodes_on_right == 0 || nodes_on_right == 1) /* prawy	liść na
		najniższym poziomie */
			return representation[cursor];
		if(representation[cursor] < 2)
		{
			return representation[cursor] + get_offset(key, depth + 1,
			cursor + 1, nodes_on_right - representation[cursor + 1]);
		}
		else
		{
			uint32_t old_cursor = cursor;
			uint32_t new_cursor = discard_subtrie(cursor + 1,
			representation[cursor]) + 1;
			return representation[old_cursor] + get_offset(key, depth + 1,
			new_cursor, nodes_on_right - representation[new_cursor]);
		}
	}
}

template<typename Value>
uint32_t SILT::Trie<Value>::discard_subtrie(uint32_t cursor,
uint32_t nodes_to_discard) const
{
	if(nodes_to_discard == 0 || nodes_to_discard == 1)
		return cursor - 1;
	uint32_t left_son = representation[cursor];
	if(nodes_to_discard - left_son > 1)
	{
		cursor = discard_subtrie(cursor + 1, left_son);
		return discard_subtrie(cursor + 1, nodes_to_discard - left_son);
	}
	return discard_subtrie(cursor + 1, left_son);
	/* Wersja krótsza, ale może działać nieco wolniej
	if(nodes_to_discard == 0 || nodes_to_discard == 1)
		return cursor - 1;
	uint32_t left_son = representation[cursor];
	cursor = discard_subtrie(cursor + 1, left_son);
	return discard_subtrie(cursor + 1, nodes_to_discard - left_son);*/
}
