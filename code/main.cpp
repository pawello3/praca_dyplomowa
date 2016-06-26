/*
	Paweł Guzewicz, 263664
	Indywidualne Studia Informatyczno-Matematyczne, Uniwersytet Wrocławski
	Praca licencjacka: Analiza teoretyczna i porównawcza efektywnej pamięciowo,
	wysoce wydajnej tablicy asocjacyjnej SILT (Small Index Large Table)
*/

#include "Small_Index_Large_Table.hpp"

#include "SILT_key.hpp"

#define NORMAL
//#define TEST1
//#define TEST2

int main()
{
	#ifdef NORMAL
	SILT::Small_Index_Large_Table<uint32_t, double>* silt
	= new SILT::Small_Index_Large_Table<uint32_t, double>();
	FILE* file = fopen("random_data.txt", "r");
	int32_t key;
	double value;
	for(uint32_t i = 0; i < 1050000; i++)
	{
		if(fscanf(file, "%" SCNd32 "%lf", &key, &value))
		{
			silt->insert(key, value);
		}
	}

	double* d = (*silt)[1444503063];
	if(d != nullptr)
		printf("%lf\n", *d);
	else
		printf("Key not found\n");
	fclose(file);
	delete silt;
	#endif
	#ifdef TEST1
	FILE* file = fopen("aaa.dat", "wb+");
	fseek(file, 0, SEEK_SET);
	SILT::SILT_key key;
	double value = 7;
	key.h0 = 0x80000000;
	key.h1 = 0;
	key.h2 = 0;
	key.h3 = 0;
	key.h4 = 0;
	fwrite(&key, sizeof(SILT::SILT_key), 1, file);
	fwrite(&value, sizeof(double), 1, file);
	key.h0 = 0x90000000;
	fwrite(&key, sizeof(SILT::SILT_key), 1, file);
	fwrite(&value, sizeof(double), 1, file);
	key.h0 = 0xA0000000;
	fwrite(&key, sizeof(SILT::SILT_key), 1, file);
	fwrite(&value, sizeof(double), 1, file);
	key.h0 = 0xC0000000;
	fwrite(&key, sizeof(SILT::SILT_key), 1, file);
	fwrite(&value, sizeof(double), 1, file);
	key.h0 = 0xD0000000;
	fwrite(&key, sizeof(SILT::SILT_key), 1, file);
	fwrite(&value, sizeof(double), 1, file);
	key.h0 = 0xD8000000;
	fwrite(&key, sizeof(SILT::SILT_key), 1, file);
	fwrite(&value, sizeof(double), 1, file);
	key.h0 = 0xE0000000;
	fwrite(&key, sizeof(SILT::SILT_key), 1, file);
	fwrite(&value, sizeof(double), 1, file);
	key.h0 = 0xE8000000;
	fwrite(&key, sizeof(SILT::SILT_key), 1, file);
	fwrite(&value, sizeof(double), 1, file);
	key.h0 = 0xF0000000;
	fwrite(&key, sizeof(SILT::SILT_key), 1, file);
	fwrite(&value, sizeof(double), 1, file);
	key.h0 = 0xF8000000;
	fwrite(&key, sizeof(SILT::SILT_key), 1, file);
	fwrite(&value, sizeof(double), 1, file);
	SILT::Trie<double>* trie = new SILT::Trie<double>(file, 10);
	printf("%" PRIu32 "\n", trie->get_offset(key));
	delete trie;
	#endif
	#ifdef TEST2
	FILE* file = fopen("aaa.dat", "wb+");
	fseek(file, 0, SEEK_SET);
	SILT::SILT_key key;
	double value = 7;
	key.h0 = 0;
	key.h1 = 0;
	key.h2 = 0;
	key.h3 = 0;
	key.h4 = 0;

	uint32_t number = 0;
	uint32_t which = 0;
	for(uint32_t i = 0; i < 65535; i++)
	{
		if(i % 7 == 0 || i % 5 == 0)
			continue;
		if(i == 33333)
			which = number;
		number++;
		key.h0 = i << 16;
		fwrite(&key, sizeof(SILT::SILT_key), 1, file);
		fwrite(&value, sizeof(double), 1, file);
	}
	key.h0 = 33333 << 16;
	SILT::Trie<double>* trie = new SILT::Trie<double>(file, number);
	printf("%" PRIu32 "\n", trie->get_offset(key));
	DEBUG(PRINT_UINT_32(which));
	delete trie;
	#endif
	return 0;
}
