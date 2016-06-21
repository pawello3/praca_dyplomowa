/*
	Paweł Guzewicz, 263664
	Indywidualne Studia Informatyczno-Matematyczne, Uniwersytet Wrocławski
	Praca licencjacka: Analiza teoretyczna i porównawcza efektywnej pamięciowo,
	wysoce wydajnej tablicy asocjacyjnej SILT (Small Index Large Table)
*/

#include "Small_Index_Large_Table.hpp"

int main()
{
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

	double* d = (*silt)[1142305634];
	if(d != nullptr)
		printf("%lf\n", *d);
	else
		printf("Key not found\n");
	fclose(file);
	delete silt;
	return 0;
}
