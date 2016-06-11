/*
	Paweł Guzewicz, 263664
	Indywidualne Studia Informatyczno-Matematyczne, Uniwersytet Wrocławski
	Praca licencjacka: Analiza teoretyczna i porównawcza efektywnej pamięciowo,
	wysoce wydajnej tablicy asocjacyjnej SILT (Small Index Large Table)
*/

#include "Small_Index_Large_Table.hpp"

int main()
{
	SILT::Log_store<double> log_store = SILT::Log_store<double>();
	SILT::SILT_key key;
	SILT::SHA_1(6452452, &key);
	log_store.insert(key, 7.92);
	SILT::SHA_1(7, &key);
	log_store.insert(key, 7.93);
	SILT::SHA_1(41234, &key);
	log_store.insert(key, 7.94);
	SILT::SHA_1(245376, &key);
	log_store.insert(key, 7.95);
	SILT::SHA_1(5, &key);
	log_store.insert(key, 7.91);
	log_store.insert(key, 3.6);
	log_store.insert(key, 3.6233233);
	log_store.insert(key, 3.62222);
	log_store.insert(key, 3.65555);
	log_store.insert(key, 3.69999);
	log_store.insert(key, 3.688);
	log_store.insert(key, 3.635555);
	bool b = log_store.insert(key, 3.6336565);
	if(!b)
		printf("Błąd wstawiania\n");
	//log_store.remove(key);
	bool reason = false;
	double* d = log_store.get_value(key, &reason);
	if(d != nullptr)
		printf("%lf\n", *d);
	else
		printf("Key not found\n");
	SILT::Hash_store<double> hash_store = SILT::Hash_store<double>(&log_store);
	d = hash_store.get_value(key, &reason);
	if(d != nullptr)
		printf("%lf", *d);
	else
		printf("Key not found\n");
	hash_store.sort();
	//SILT::Sorted_store<double> sorted_store
	//= SILT::Sorted_store<double>(hash_store_list);
	//SILT::Small_Index_Large_Table<double> silt
	//= SILT::Small_Index_Large_Table<double>();
	return 0;
}
