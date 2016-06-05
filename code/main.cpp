/*
	Paweł Guzewicz, 263664
	Indywidualne Studia Informatyczno-Matematyczne, Uniwersytet Wrocławski
	Praca licencjacka: Analiza teoretyczna i porównawcza efektywnej pamięciowo,
	wysoce wydajnej tablicy asocjacyjnej SILT (Small Index Large Table)
*/

#include "Small_Index_Large_Table.hpp"

int main()
{
	SILT::Log_store<int, double> log_store = SILT::Log_store<int, double>();
	log_store.insert(5, 7.91);
	log_store.insert(5, 3.6);
	log_store.insert(6452452, 7.92);
	log_store.insert(7, 7.93);
	log_store.insert(7432, 7.94);
	log_store.insert(73673, 7.95);
	log_store.insert(7252457, 7.96);
	double* d = log_store[7252457];
	if(d != nullptr)
		printf("%lf", *d);
	else
		printf("Key not found\n");
	//SILT::Hash_store<int, double> hash_store
	//= SILT::Hash_store<int, double>(&log_store);
	//SILT::Sorted_store<int, double> sorted_store
	//= SILT::Sorted_store<int, double>(hash_store_list);
	//SILT::Small_Index_Large_Table<int, double> silt
	//= SILT::Small_Index_Large_Table<int, double>();
	return 0;
}
