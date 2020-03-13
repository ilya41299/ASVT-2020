#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <string>
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <direct.h>
#include <filesystem>

class MyClass
{
public:
	MyClass(int n, std::vector<std::string> DNF);
	void finding_implicant(); // поиск имликант 
	void print(std::ofstream &fout); // печать таблицы
	void alignment_marks(std::ofstream& fout_matrix, std::ofstream& fout_impl, std::ofstream& res_impl); // расстановка меток и их минимизация
private:
	std::vector<std::string> start_minterms; // наборы ДНФ
	std::set<std::string> result_implicants; // импликанты получаемые при склейке 
	std::map<int, std::vector<std::string>> numbers_group; // вектора сгруппированные по весу. i = вес
	int N; // размерность 
	int weight(std::string &V); // весовая функция набора
	bool splice(); // получение конечных склеек
	bool is_splice(std::string &S1, std::string &S2); // проверка возможности склейки
	std::string make_splice(std::string &S1, std::string &S2); // склейка двух наборов
	bool is_absorb(std::string S1, std::string S2); // проверка на возможностть склейки
	void print_res(std::ofstream& fout_matrix, std::ofstream& fout_impl, bool** matrix); // печать таблицы 
};

MyClass::MyClass(int n, std::vector<std::string> DNF)
{
	N = n;
	for (size_t i = 0; i < DNF.size(); i++)
	{
		numbers_group[weight(DNF[i])].push_back(DNF[i]);
	}
	start_minterms = DNF;
}

void MyClass::finding_implicant()
{
	while (splice())
	{

	}
	std::set<std::string> implicants;
	for (size_t i = 0; i < numbers_group.size(); i++)
	{
		for (size_t j = 0; j < numbers_group[i].size(); j++)
		{
			implicants.insert(numbers_group[i][j]);
		}
	}
	result_implicants = implicants;
}

void MyClass::print(std::ofstream &fout)
{
	fout << '+';
	for (auto i = 0; i < start_minterms.size() + 1; i++)
	{
		for (size_t j = 0; j < N; j++)
		{
			fout << '-';
		}
		fout << '+';
	}
	fout << std::endl;
	fout << '|';
	for (auto i = 0; i < start_minterms.size() + 1; i++)
	{
		if (i == 0)
		{
			fout << std::setw(N) << ' ' << '|';
		}
		else 
		{
			fout << std::setw(N) << start_minterms[i - 1] << '|';
		}
	}
	fout << std::endl;
	fout << '+';
	for (auto i = 0; i < start_minterms.size() + 1; i++)
	{
		for (size_t j = 0; j < N; j++)
		{
			fout << '-';
		}
		fout << '+';
	}
	fout << std::endl;
	for (auto& el : result_implicants) 
	{
		fout << '|';
		fout << std::setw(N) << el << '|';
		for (auto j = 0; j < start_minterms.size(); j++) {
			if (is_absorb(el, start_minterms[j]))
			{
				fout << std::setw(N) << is_absorb(el, start_minterms[j]) << '|';
			}
			else fout << std::setw(N) << ' ' << '|';
			
		}
		fout << std::endl;
		fout << '+';
		for (auto i = 0; i < start_minterms.size() + 1; i++)
		{
			for (size_t j = 0; j < N; j++)
			{
				fout << '-';
			}
			fout << '+';
		}
		fout << std::endl;
	}
}

int MyClass::weight(std::string &V)
{
	int weight_V = 0;
	for (size_t i = 0; i < V.size(); i++)
	{
		if (V[i] == '1') weight_V++;
	}
	return weight_V;
}

bool MyClass::splice()
{
	std::vector<std::string> splices;
	std::set<std::string> numbers_for_delete;
	std::string splice_string;
	bool can_splices = false;
	for (size_t index_weight = 0; index_weight < N; index_weight++)
	{
		for (size_t i = 0; i < numbers_group[index_weight].size(); i++)
		{
			for (size_t j = 0; j < numbers_group[index_weight+1].size(); j++)
			{
				if (is_splice(numbers_group[index_weight][i], numbers_group[index_weight + 1][j]))
				{
					splice_string = make_splice(numbers_group[index_weight][i], numbers_group[index_weight + 1][j]);
					splices.push_back(splice_string);
					numbers_for_delete.insert(numbers_group[index_weight][i]);
					numbers_for_delete.insert(numbers_group[index_weight + 1][j]);
					can_splices = true;
				}
			}
		}
	}
	for (size_t i = 0; i < splices.size(); i++)
	{
		numbers_group[weight(splices[i])].push_back(splices[i]);
	}
	for(auto el : numbers_for_delete)
	{
		int weight_el = weight(el);
		numbers_group[weight_el].erase(std::find(numbers_group[weight_el].begin(), numbers_group[weight_el].end(), el));
	}
	return can_splices;
}

bool MyClass::is_splice(std::string &S1, std::string &S2)
{
	int count_difference = 0;
	for (size_t i = 0; i < N; i++)
	{
		if (S1[i] == S2[i]) continue;
		else count_difference++;
		if (count_difference > 1) return false;
	}
	return true;
}

std::string MyClass::make_splice(std::string& S1, std::string& S2)
{
	std::string result;
	int count_difference = 0, index_difference;
	for (size_t i = 0; i < N; i++)
	{
		if (S1[i] == S2[i]) result.push_back(S1[i]);
		else result.push_back('~');
	}
	return result;
}

bool MyClass::is_absorb(std::string implicant, std::string minterm)
{
	for (size_t i = 0; i < N; i++)
	{
		if (!((implicant[i] == minterm[i]) || implicant[i] == '~')) return false;
	}
	return true;
}

void MyClass::print_res(std::ofstream& fout_matrix, std::ofstream& fout_impl, bool** matrix)
{
	std::vector<int> pass_i;
	std::vector<int> pass_j;
	int counter=0;
	for (size_t i = 0; i < result_implicants.size(); i++)
	{
		for (size_t j = 0; j < start_minterms.size(); j++) 
		{
			if (matrix[i][j]) counter++;
		}
		if (counter==0)
		{
			pass_i.push_back(i);
		}
		counter = 0;
	}
	for (size_t i = 0; i < start_minterms.size(); i++)
	{
		for (size_t j = 0; j < result_implicants.size(); j++)
		{
			if (matrix[j][i]) counter++;
		}
		if (counter == 0)
		{
			pass_j.push_back(i);
		}
		counter = 0;
	}
	int l = 0;
	for (auto& el : result_implicants)
	{
		if (std::find(pass_i.begin(), pass_i.end(), l) != pass_i.end()) 
		{
			l++;
			continue;
		}
		fout_impl << el << std::endl;
		for (auto j = 0; j < start_minterms.size(); j++) {
			if (std::find(pass_j.begin(), pass_j.end(), j) == pass_j.end())
			{
				if (matrix[l][j])
				{
					fout_matrix << 1;
				}
				else fout_matrix << 0;
			}
		}
		fout_matrix << std::endl;
		l++;
	}
}

void MyClass::alignment_marks(std::ofstream& fout_matrix, std::ofstream& fout_impl, std::ofstream& res_impl)
{
	bool** matrix = new bool* [result_implicants.size()];
	
	for (int i = 0; i < result_implicants.size(); i++) 
	{
		matrix[i] = new bool[start_minterms.size()];
	}

	// расставляем пометки
	int counter = 0;
	for (auto& el : result_implicants)
	{
		for (size_t j = 0; j < start_minterms.size(); j++)
		{
			if (is_absorb(el, start_minterms[j]))
			{
				matrix[counter][j] = true;
			}
			else matrix[counter][j] = false;
		}
		counter++;
	}
	std::vector<int> number_implicants;
	int index_implicant;
	for (size_t j = 0; j < start_minterms.size(); j++)
	{
		counter = 0;
		for (size_t i = 0; i < result_implicants.size(); i++)
		{
			if (matrix[i][j]) 
			{
				counter++;
				index_implicant = i;
			}
		}
		if (counter == 1) 
		{
			number_implicants.push_back(index_implicant);
			for (size_t j = 0; j < start_minterms.size(); j++)
			{
				if (matrix[index_implicant][j])
				{
					for (size_t i = 0; i < result_implicants.size(); i++) 
					{
						matrix[i][j] = false;
					}
				}
				
			}
		}
	}
	counter = 0;
	// вывод минимизированной таблицы
	print_res(fout_matrix, fout_impl, matrix);
	// вывод импликант индексов импликант для мднф
	for (auto el : result_implicants)
	{
		for (size_t j = 0; j < number_implicants.size();j++)
		{
			if (number_implicants[j] == counter) 
			{
				res_impl << el << ' ';
			}				
		}
		counter++;
	}
	for (int i = 0; i < result_implicants.size(); i++)
	{
		delete[] matrix[i];
	}
	delete[]matrix;
}

int main()
{
	std::string input;
	std::cout << "Name of the input file (***.txt): ";
	std::cin >> input;
	std::ifstream fin(input);
	if (!fin.is_open())
	{
		std::cout << "invalid file name";
		system("pause");
		return 0;
	}
	std::ofstream fout("table.txt");
	std::ofstream fout_matrix("mat.txt");
	std::ofstream fout_impl("impls.txt");
	std::ofstream res_impl("res_impls.txt");
	int N;
	std::vector<std::string> My_V; // ДНФ
	fin >> N;
	std::string minterm;
	while (fin >> minterm)
	{
		My_V.push_back(minterm);
	}
	MyClass	obj(N, My_V);
	obj.finding_implicant();
	obj.print(fout);
	obj.alignment_marks(fout_matrix, fout_impl, res_impl);
	std::filesystem::path path = std::filesystem::current_path();
	std::string scr = path.generic_string();
	scr += "/script.py ";
	std::string command = "python " + scr + path.generic_string() + "/mat.txt " + path.generic_string() + "/impls.txt";
	std::system(command.c_str());
	fout.close();
	fout_matrix.close();
	fout_impl.close();
	res_impl.close();
	system("pause");
	return 0;
}
