
#include <iostream>
#include <vector>
#include <bitset>
#include <unordered_set>
#include <set>
#include <fstream>
#include <algorithm>

struct K
{
	std::vector<int>  K_index;
	std::string K_bool;
	bool is_active;
	int deg;
	K();
	K(std::vector<int> index, std::string boolean)
	{
		K_index = index;
		K_bool = boolean;
		is_active = true;
		int count = 0;
		for (size_t i = 0; i < index.size(); i++)
		{
			if (index[i] == 1) count++;
		}
		deg = count;
	}
	bool operator==(const K& rhs)
	{
		for (size_t i = 0; i < K_index.size(); i++)
		{
			if (rhs.K_index[i] != K_index[i]) return false;
			if (K_index[i] == 1)
			{
				if (K_bool[i] != rhs.K_bool[i]) return false;
			}
		}
		return true;;
	}
};

void initialisation_function_numers(std::vector<std::string>& function_numers)
{
	for (size_t i = 0; i < 64; i++)
	{
		std::bitset<6> V = i;
		std::string number;
		for (size_t j = V.size(); j > 0; j--)
		{
			if (V[j - 1]) number.push_back('1');
			else number.push_back('0');
		}
		function_numers.push_back(number);
	}
}

void initialisation_string_coefficients(std::vector<std::vector<K>>& string_coefficients, std::vector<std::string>& function_numers)
{
	for (int counter = 0; counter < 64; counter++)
	{
		std::vector<K> V_K;
		for (size_t i = 0; i < 2; i++)
		{
			for (size_t j = 0; j < 2; j++)
			{
				for (size_t k = 0; k < 2; k++)
				{
					for (size_t l = 0; l < 2; l++)
					{
						for (size_t m = 0; m < 2; m++)
						{
							for (size_t n = 0; n < 2; n++)
							{
								if (i == 0 && j == 0 && k == 0 && l == 0 && m == 0 && n == 0)continue;
								std::vector<int> V;
								V.push_back(i);
								V.push_back(j);
								V.push_back(k);
								V.push_back(l);
								V.push_back(m);
								V.push_back(n);
								K coef(V, function_numers[counter]);
								V_K.push_back(coef);

							}
						}
					}
				}
			}
		}
		string_coefficients.push_back(V_K);
	}
}

void find_delete_K(std::vector<std::vector<K>>& string_coefficients, std::vector<std::string>& function_numbers, std::unordered_set<std::string>& is_true_in_DNF)
{
	std::vector<K> zeros_K;
	// ищем и запоминаем нулевые коэффициенты
	for (size_t i = 0; i < string_coefficients.size(); i++)
	{
		if (is_true_in_DNF.find(function_numbers[i]) != is_true_in_DNF.end()) continue;
		else
		{
			for (size_t j = 0; j < string_coefficients[i].size(); j++)
			{
				zeros_K.push_back(string_coefficients[i][j]);
			}
		}
	}

	// удаляем коэффициенты
	for (size_t i = 0; i < string_coefficients.size(); i++)
	{
		for (size_t j = 0; j < string_coefficients[i].size(); j++)
		{
			if (std::find(zeros_K.begin(), zeros_K.end(), string_coefficients[i][j]) != zeros_K.end())
			{
				string_coefficients[i][j].is_active = false;
			}

		}
	}
}

// проверка, есть ли еще непокрытые уравнения
bool not_yet(std::vector<std::vector<K>>& string_coefficients)
{
	for (size_t i = 0; i < string_coefficients.size(); i++)
	{
		for (size_t j = 0; j < string_coefficients[i].size(); j++)
		{
			if (string_coefficients[i][j].is_active) return true;
		}
	}
	return false;
}

int count_terms(std::vector<K> A)
{
	int counter = 0;
	for (size_t j = 0; j < A.size(); j++)
	{
		if (A[j].is_active) counter++;
	}
	return counter;
}

void print(std::vector<K>& result_implicants, std::ofstream &fout)
{
	for (size_t i = 0; i < result_implicants.size(); i++)
	{
		for (int j = 0; j < result_implicants[i].K_index.size(); j++)
		{
			if (result_implicants[i].K_index[j] == 1)
			{
				if (result_implicants[i].K_bool[j] == '1')
					fout << 'X' << j +1  << ' ';
				else
					fout << "-" << 'X' << j +1  << ' ';
			}
		}
		fout << std::endl;
	}
}

int find_deg(std::vector<K>& A)
{
	int min_deg = 10;
	for (size_t j = 0; j < A.size(); j++)
	{
		if (A[j].is_active && A[j].deg < min_deg)
		{
			min_deg = A[j].deg;
		}
	}
	return min_deg;
}

void get_result(std::vector<std::vector<K>>& string_coefficients , std::ofstream &fout)
{

	std::vector<K> result_implicants;
	while (not_yet(string_coefficients))
	{
		//сортируем строки по возрастанию длины
		std::sort(string_coefficients.begin(), string_coefficients.end(), [](std::vector<K> A, std::vector<K> B)
			{
				if (count_terms(A) == count_terms(B))
				{
					return (find_deg(A) < find_deg(B));
				}
				return (count_terms(A) < count_terms(B));
			}
		);	
		std::vector<std::pair<K, int>> implicant_and_rang;
		// ищем минимальную степень набора
		std::vector<int> index_lines_onece_degs;
		int local_min=10, min_deg =10;
		for (size_t i = 0; i < string_coefficients.size(); i++)
		{
			local_min = 10;
			for (size_t j = 0; j < string_coefficients[i].size(); j++)
			{
				if (string_coefficients[i][j].is_active && string_coefficients[i][j].deg < local_min)
				{
					local_min = string_coefficients[i][j].deg;

				}
			}
			if (count_terms(string_coefficients[i]) == 0) continue;
			if (local_min== min_deg)
			{
				index_lines_onece_degs.push_back(i);
			}
			if (local_min < min_deg)
			{
				index_lines_onece_degs.clear();
				index_lines_onece_degs.push_back(i);
				min_deg = local_min;
			}
			if (i < string_coefficients.size()-1 && count_terms(string_coefficients[i]) != count_terms(string_coefficients[i + 1]))
			{
				break;
			}
		}
		// сохраняем наборы минимальной степени
		bool is_check = false;
		for (size_t i = 0; i < index_lines_onece_degs.size(); i++)
		{
			for (size_t j = 0; j < string_coefficients[i].size(); j++)
			{
				if (string_coefficients[index_lines_onece_degs[i]][j].is_active && string_coefficients[index_lines_onece_degs[i]][j].deg == min_deg)
				{
					implicant_and_rang.push_back(std::make_pair(string_coefficients[index_lines_onece_degs[i]][j], 0));
					is_check = true;
				}
			}
		}
		// подсчитываем рейтинги наборов
		for (size_t i = 0; i < string_coefficients.size(); i++)
		{
			for (size_t j = 0; j < string_coefficients[i].size(); j++)
			{
				if (string_coefficients[i][j].is_active)
				{
					for (size_t k = 0; k < implicant_and_rang.size(); k++)
					{
						if (implicant_and_rang[k].first == string_coefficients[i][j])
						{
							implicant_and_rang[k].second++;
						}
					}
				}
			}
		}
		//выбираем набор максимального ранга
		int max_reit = 0;
		int index_max_el;
		for (size_t i = 0; i < implicant_and_rang.size(); i++)
		{
			if (implicant_and_rang[i].second > max_reit)
			{
				max_reit = implicant_and_rang[i].second;
				index_max_el = i;
			}
		}
		//удаляем из таблицы строки с набором макс ранга
		result_implicants.push_back(implicant_and_rang[index_max_el].first);
		for (size_t i = 0; i < string_coefficients.size(); i++)
		{
			for (size_t j = 0; j < string_coefficients[i].size(); j++)
			{
				if (string_coefficients[i][j].is_active && string_coefficients[i][j] == implicant_and_rang[index_max_el].first)
				{
					for (size_t k = 0; k < string_coefficients[i].size(); k++)
					{
						string_coefficients[i][k].is_active = false;
					}
					break;
				}
			}
		}
	}
	print(result_implicants, fout);
}

int main()
{
	std::string input;
	std::cout << "Name of the input file (***.txt): ";
	std::cin >> input;
	std::cout << "wait..." <<std::endl;
	std::ifstream fin(input);
	if (!fin.is_open())
	{
		std::cout << "invalid file name";
		std::cin.get();
		return 0;
	}
	std::ofstream fout("result.txt");
	std::vector<std::vector<K>> string_coefficients;
	std::vector<std::string> function_numbers;
	std::vector<bool> function_value;
	initialisation_function_numers(function_numbers);
	initialisation_string_coefficients(string_coefficients, function_numbers);
	// считывание ДНФ
	std::unordered_set<std::string> is_true_in_DNF;
	std::string str;
	while (fin >> str)
	{
		is_true_in_DNF.insert(str);
	}
	// поиск нулевых коэффициентов и их удаление
	find_delete_K(string_coefficients, function_numbers, is_true_in_DNF);
	// минимизируем систему
	get_result(string_coefficients, fout);
	std::cin.get();
	return 0;
}
