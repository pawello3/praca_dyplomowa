/*
	Paweł Guzewicz, 263664
	Indywidualne Studia Informatyczno-Matematyczne, Uniwersytet Wrocławski
	Praca licencjacka: Analiza teoretyczna i porównawcza efektywnej pamięciowo,
	wysoce wydajnej tablicy asocjacyjnej SILT (Small Index Large Table)
*/

#include "debug.hpp"
#include <chrono>
#include "Small_Index_Large_Table.hpp"
#include <unordered_map>

void test_SILT_small(void);
void test_SILT_medium(void);
void test_SILT_large(void);
void test_STL_small(void);
void test_STL_medium(void);
void test_STL_large(void);

int main()
{
	std::chrono::high_resolution_clock::time_point t1, t2;
	std::chrono::duration<double> time_span;

	t1 = std::chrono::high_resolution_clock::now();
	test_SILT_small();
	t2 = std::chrono::high_resolution_clock::now();
	time_span
	= std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
	printf("Small SILT: %lf\n", time_span.count());

	t1 = std::chrono::high_resolution_clock::now();
	test_SILT_medium();
	t2 = std::chrono::high_resolution_clock::now();
	time_span
	= std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
	printf("Medium SILT: %lf\n", time_span.count());

	t1 = std::chrono::high_resolution_clock::now();
	test_SILT_large();
	t2 = std::chrono::high_resolution_clock::now();
	time_span
	= std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
	printf("Large SILT: %lf\n", time_span.count());

	t1 = std::chrono::high_resolution_clock::now();
	test_STL_small();
	t2 = std::chrono::high_resolution_clock::now();
	time_span
	= std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
	printf("Small STL map: %lf\n", time_span.count());

	t1 = std::chrono::high_resolution_clock::now();
	test_STL_medium();
	t2 = std::chrono::high_resolution_clock::now();
	time_span
	= std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
	printf("Medium STL map: %lf\n", time_span.count());

	t1 = std::chrono::high_resolution_clock::now();
	test_STL_large();
	t2 = std::chrono::high_resolution_clock::now();
	time_span
	= std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
	printf("Large STL map: %lf\n", time_span.count());

	return 0;
}

void test_SILT_small(void)
{
	SILT::Small_Index_Large_Table<int32_t, double>* silt
	= new SILT::Small_Index_Large_Table<int32_t, double>();
	FILE* file = fopen("random_data_small.txt", "r");
	int32_t key;
	double value;
	int32_t first_key;
	if(fscanf(file, "%" SCNd32 "%lf", &key, &value))
	{
		silt->insert(key, value);
		first_key = key;
	}
	else
	{
		fprintf(stderr, "fscanf error\n");
		exit(1);
	}
	for(uint32_t i = 1; i < 50000; i++)
	{
		if(fscanf(file, "%" SCNd32 "%lf", &key, &value))
		{
			silt->insert(key, value);
		}
		else
		{
			fprintf(stderr, "fscanf error\n");
			exit(1);
		}
	}

	double* d = (*silt)[first_key];
	if(d != nullptr)
		printf("%lf\n", *d);
	else
		printf("Key not found\n");
	fclose(file);
	delete silt;
}

void test_SILT_medium(void)
{
	SILT::Small_Index_Large_Table<int32_t, double>* silt
	= new SILT::Small_Index_Large_Table<int32_t, double>();
	FILE* file = fopen("random_data_medium.txt", "r");
	int32_t key;
	double value;
	int32_t first_key;
	if(fscanf(file, "%" SCNd32 "%lf", &key, &value))
	{
		silt->insert(key, value);
		first_key = key;
	}
	else
	{
		fprintf(stderr, "fscanf error\n");
		exit(1);
	}
	for(uint32_t i = 1; i < 530000; i++)
	{
		if(fscanf(file, "%" SCNd32 "%lf", &key, &value))
		{
			silt->insert(key, value);
		}
		else
		{
			fprintf(stderr, "fscanf error\n");
			exit(1);
		}
	}

	double* d = (*silt)[first_key];
	if(d != nullptr)
		printf("%lf\n", *d);
	else
		printf("Key not found\n");
	fclose(file);
	delete silt;
}

void test_SILT_large(void)
{
	SILT::Small_Index_Large_Table<int32_t, double>* silt
	= new SILT::Small_Index_Large_Table<int32_t, double>();
	FILE* file = fopen("random_data_large.txt", "r");
	int32_t key;
	double value;
	int32_t first_key;
	if(fscanf(file, "%" SCNd32 "%lf", &key, &value))
	{
		silt->insert(key, value);
		first_key = key;
	}
	else
	{
		fprintf(stderr, "fscanf error\n");
		exit(1);
	}
	for(uint32_t i = 1; i < 1050000; i++)
	{
		if(fscanf(file, "%" SCNd32 "%lf", &key, &value))
		{
			silt->insert(key, value);
		}
		else
		{
			fprintf(stderr, "fscanf error\n");
			exit(1);
		}
	}

	double* d = (*silt)[first_key];
	if(d != nullptr)
		printf("%lf\n", *d);
	else
		printf("Key not found\n");
	fclose(file);
	delete silt;
}

void test_STL_small(void)
{
	std::unordered_map<int32_t, uint32_t>* map
	= new std::unordered_map<int32_t, uint32_t>();
	FILE* file = fopen("random_data_small.txt", "r");
	int32_t key;
	double value;
	int32_t first_key;
	if(fscanf(file, "%" SCNd32 "%lf", &key, &value))
	{
		(*map)[key] = 0;
		first_key = key;
	}
	else
	{
		fprintf(stderr, "fscanf error\n");
		exit(1);
	}
	for(uint32_t i = 1; i < 50000; i++)
	{
		if(fscanf(file, "%" SCNd32 "%lf", &key, &value))
		{
			(*map)[key] = i;
		}
		else
		{
			fprintf(stderr, "fscanf error\n");
			exit(1);
		}
	}
	fclose(file);
	file = fopen("random_data_small.txt", "r");

	uint32_t offset = (*map)[first_key];
	double d;
	fseek(file, 0, SEEK_SET);
	for(uint32_t i = 0; i <= offset; i++)
	{
		if(fscanf(file, "%" SCNd32 "%lf", &key, &value))
		{
			d = value;
		}
		else
		{
			fprintf(stderr, "fscanf error\n");
			exit(1);
		}
	}
	printf("%lf\n", d);
	fclose(file);
	delete map;
}

void test_STL_medium(void)
{
	std::unordered_map<int32_t, uint32_t>* map
	= new std::unordered_map<int32_t, uint32_t>();
	FILE* file = fopen("random_data_medium.txt", "r");
	int32_t key;
	double value;
	int32_t first_key;
	if(fscanf(file, "%" SCNd32 "%lf", &key, &value))
	{
		(*map)[key] = 0;
		first_key = key;
	}
	else
	{
		fprintf(stderr, "fscanf error\n");
		exit(1);
	}
	for(uint32_t i = 1; i < 530000; i++)
	{
		if(fscanf(file, "%" SCNd32 "%lf", &key, &value))
		{
			(*map)[key] = i;
		}
		else
		{
			fprintf(stderr, "fscanf error\n");
			exit(1);
		}
	}
	fclose(file);
	file = fopen("random_data_medium.txt", "r");

	uint32_t offset = (*map)[first_key];
	double d;
	fseek(file, 0, SEEK_SET);
	for(uint32_t i = 0; i <= offset; i++)
	{
		if(fscanf(file, "%" SCNd32 "%lf", &key, &value))
		{
			d = value;
		}
		else
		{
			fprintf(stderr, "fscanf error\n");
			exit(1);
		}
	}
	printf("%lf\n", d);
	fclose(file);
	delete map;
}

void test_STL_large(void)
{
	std::unordered_map<int32_t, uint32_t>* map
	= new std::unordered_map<int32_t, uint32_t>();
	FILE* file = fopen("random_data_large.txt", "r");
	int32_t key;
	double value;
	int32_t first_key;
	if(fscanf(file, "%" SCNd32 "%lf", &key, &value))
	{
		(*map)[key] = 0;
		first_key = key;
	}
	else
	{
		fprintf(stderr, "fscanf error\n");
		exit(1);
	}
	for(uint32_t i = 1; i < 1050000; i++)
	{
		if(fscanf(file, "%" SCNd32 "%lf", &key, &value))
		{
			(*map)[key] = i;
		}
		else
		{
			fprintf(stderr, "fscanf error\n");
			exit(1);
		}
	}
	fclose(file);
	file = fopen("random_data_large.txt", "r");

	uint32_t offset = (*map)[first_key];
	double d;
	fseek(file, 0, SEEK_SET);
	for(uint32_t i = 0; i <= offset; i++)
	{
		if(fscanf(file, "%" SCNd32 "%lf", &key, &value))
		{
			d = value;
		}
		else
		{
			fprintf(stderr, "fscanf error\n");
			exit(1);
		}
	}
	printf("%lf\n", d);
	fclose(file);
	delete map;
}
