/*
	Paweł Guzewicz, 263664
	Indywidualne Studia Informatyczno-Matematyczne, Uniwersytet Wrocławski
	Praca licencjacka: Analiza teoretyczna i porównawcza efektywnej pamięciowo,
	wysoce wydajnej tablicy asocjacyjnej SILT (Small Index Large Table)
*/

#include "Small_Index_Large_Table.hpp"

int main()
{
	SILT::Small_Index_Large_Table<int, double> silt
	= SILT::Small_Index_Large_Table<int, double>();
	SILT::Log_store<int, double> log_store = SILT::Log_store<int, double>();
	SILT::Hash_store<int, double> hash_store
	= SILT::Hash_store<int, double>(&log_store);
	//SILT::Sorted_store<int, double> sorted_store
	//= SILT::Sorted_store<int, double>(hash_store_list);
	return 0;
}
