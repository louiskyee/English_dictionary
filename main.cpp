#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
//#include <ctime>
#define TSIZE 100000
#define remainder 100000
#define  seed 9406
class Node {
public:
	char* str;
	Node* next;
	Node() :str(NULL), next(NULL) {};
	Node operator=(const Node& a) {
		str = a.str;
		next = a.next;
	}
};
uint32_t get_32bit_Block(std::string str, int index)
{
	return	 str[(index * 4) + 3] +
		(str[(index * 4) + 2] << 8) +
		(str[(index * 4) + 1] << 16) +
		(str[(index * 4) + 0] << 24);
}
uint32_t Murmur3_32(std::string data, const int length) {
	const uint32_t c1 = 0xcc9e2d51;
	const uint32_t c2 = 0x1b873593;
	const int r1 = 15;
	const int r2 = 13;
	const int m = 5;
	const uint32_t n = 0xe6546b64;	//3,864,292,196
	const int remaining_Bytes = length & 3; // data.length() % 4
	const int total_32bit_blocks = (length - remaining_Bytes) / 4;	//total 32bit blocks
	uint32_t hash = seed;
	uint32_t k;
	for (int i = 0; i < total_32bit_blocks; i++)
	{
		k = 0;
		k = get_32bit_Block(data, i);
		k *= c1;
		k = (k << r1) | (k >> (32 - r1));	//ROTL32 //unsigned short _rotl(unsigned short value, int count);
		k *= c2;

		hash ^= k;
		hash = (hash << r2) | (hash >> (32 - r2));	//ROTL32
		hash = hash * m + n;
	}
	k = 0;
	switch (remaining_Bytes)
	{
	case 3: k ^= data[(total_32bit_blocks * 4) + 2] << 16;
	case 2: k ^= data[(total_32bit_blocks * 4) + 1] << 8;
	case 1: k ^= data[(total_32bit_blocks * 4) + 0];
		k *= c1;
		k = (k << r1) | (k >> (32 - r1));
		k *= c2;
		hash ^= k;
	};

	hash ^= length;
	hash ^= hash >> 16;
	hash *= 0x85ebca6b;
	hash ^= hash >> 13;
	hash *= 0xc2b2ae35;
	hash ^= hash >> 16;

	return hash % remainder;
}
bool repeat(std::vector<std::string>& database, std::string str) {
	for (int i = 0; i < database.size(); ++i) {
		if (database[i] == str) return true;
	}
	return false;
}
bool searchTable(Node arr[TSIZE], uint32_t index, std::string key) {
	if (arr[index].str == NULL)return false;
	if (arr[index].str == key)return true;
	Node* temp = arr[index].next;
	while (temp != NULL) {
		if (temp->str == key)return true;
		temp = temp->next;
	}
	return false;
}
void mis_spelled(Node arr[TSIZE], std::vector<std::string>& output, std::string str, bool done) {
	for (int i = 0; i <= str.length(); ++i) {	//insert
		std::string str1 = str.substr(0, i);
		std::string str2 = str.substr(i, str.length() - i);
		for (char j = 'a'; j <= 'z'; ++j) {
			std::string _str = str1 + j + str2;
			if (searchTable(arr, Murmur3_32(_str, _str.length()), _str) && !repeat(output, _str)) { output.push_back(_str); }
			if (!done) { mis_spelled(arr, output, _str, true); }
		}
	}
	for (int i = 0; i < str.length(); ++i) {	//delete
		std::string str1 = str.substr(0, i);
		std::string str2 = str.substr(i + 1, str.length() - i - 1);
		std::string _str = str1 + str2;
		if (searchTable(arr, Murmur3_32(_str, _str.length()), _str) && !repeat(output, _str)) { output.push_back(_str); }
		if (!done) { mis_spelled(arr, output, _str, true); }
	}
	for (int i = 0; i < str.length(); ++i) {	//substitute
		for (char j = 'a'; j <= 'z'; ++j) {
			std::string _str = str;
			_str[i] = j;
			if (searchTable(arr, Murmur3_32(_str, _str.length()), _str) && !repeat(output, _str)) { output.push_back(_str); }
			if (!done) { mis_spelled(arr, output, _str, true); }
		}
	}
	for (int i = 0; i < str.length() - 1; ++i) {	//transpose
		std::string _str = str;
		std::swap(_str[i], _str[i + 1]);
		if (searchTable(arr, Murmur3_32(_str, _str.length()), _str) && !repeat(output, _str)) { output.push_back(_str); }
		if (!done) { mis_spelled(arr, output, _str, true); }
	}
}
bool compare(std::string a, std::string b) {		//check if a >= b, yes return false
	if (a.length() == b.length() || a.length() > b.length()) {
		for (int i = 0; i < b.length(); ++i) {
			if (a[i] > b[i]) return false;
			if (a[i] < b[i]) return true;
		}
		return false;
	}
	else {
		for (int i = 0; i < a.length(); ++i) {
			if (a[i] > b[i]) return false;
			if (a[i] < b[i]) return true;
		}
		return true;
	}
}
int main(void) {
	std::fstream file;
	std::fstream inputFile;
	std::fstream csvFile;
	std::string str;
	//clock_t start, finish;
	unsigned int index;
	Node arr[TSIZE];
	file.open("dictionary.txt", std::ios::in);
	if (file) {
		//start = clock();
		for (int i = 0; i < 56; ++i) {
			std::getline(file, str);
		}
		while (file >> str) {
			index = Murmur3_32(str, str.length());
			if (arr[index].str == NULL) {
				arr[index].str = new char[str.length() + 1];
				strncpy_s(arr[index].str, str.length() + 1, str.c_str(), str.length());
			}
			else {
				Node* cur = arr[index].next;
				Node* pre = NULL;
				while (cur != NULL) {
					pre = cur;
					cur = cur->next;
				}
				cur = new Node;
				cur->str = new char[str.length() + 1];
				strncpy_s(cur->str, str.length() + 1, str.c_str(), str.length());
				if (pre == NULL) arr[index].next = cur;
				else pre->next = cur;
			}
		}
		file.close();
		//============================================================================================================================
		inputFile.open("test.txt", std::ios::in);
		if (inputFile) {
			csvFile.open("answer.csv", std::ios::out);
			csvFile << "word" << ',' << "answer" << '\n';
			while (inputFile >> str) {
				csvFile << str << ',';
				std::vector<std::string> output;
				if (searchTable(arr, Murmur3_32(str, str.length()), str)) {		//printf OK
					csvFile << "OK\n";
				}
				else {
					mis_spelled(arr, output, str, false);
					if (output.size() != 0) {
						std::sort(output.begin(), output.end(), compare);
						for (int i = 0; i < output.size() - 1; ++i) {
							csvFile << output[i] << ' ';
						}
						csvFile << output[output.size() - 1] << '\n';
					}
					else { csvFile << "NONE\n"; }	//printf NONE
				}
			}
			csvFile.close();
			inputFile.close();
			//finish = clock();
			//double duration = (double)(finish - start) / CLOCKS_PER_SEC;
			//printf("insert seconds = %f\n", duration);
			//system("pause");
		}
		else {
			printf("Open input.txt error!!\n");
		}
	}
	else {
		printf("Open dictionary.txt error!!\n");
	}
	return 0;
}
