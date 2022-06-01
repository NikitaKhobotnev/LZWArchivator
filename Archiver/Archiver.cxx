export module Archiver;

import <string>;
import <iostream>;
import <fstream>;
import <map>;
import <future>;
import <thread>;
import <vector>;
import <bitset>;
import <cmath>;
import <algorithm>;

import MethodsArchiver; //Функции архиватора




using namespace std;

export class LZWArchivator {
private:
	union IntChar {
		uint32_t Integer;
		char Str[5];
	};
	union ByteChar {
		uint8_t Integer;
		char Str[2];
	};


	string SoursePath;     // путь к входной цели
	string DestinPath; // путь к выходной цели



	string filenameExtension; //расширение файла
	//!! Проблема: 2 вектора потоков. Вероятно можно переделать в один с void.
	vector<future<vector<uint32_t>>> CompressThreads;
	vector<future<string>> DecompressThreads;
	//!!


	vector<vector<uint32_t>> _compressStr; //массивы чисел - сам архив.
	vector<vector<uint32_t>> _metaCompressStr; //метаинформация о массивах чисел. 

	vector<string> _uncompressedStr; //строки томов
	size_t NumOfThreads;

	string GetFileName(const string path_to_file) const {
		return path_to_file.substr(path_to_file.find_last_of("\\") + 1, path_to_file.find_last_of(".")
			- path_to_file.find_last_of("\\") - 1);
	}
	string GetFileExtension(string path_to_file) {
		return (filenameExtension = path_to_file.substr(
			path_to_file.find_last_of(".") + 1, path_to_file.size() - path_to_file.find_last_of(".")));
	}

	template<uint32_t numOfBits>
	uint64_t binaryStrToLong(string str)
	{
		bitset<numOfBits> _bits(str);
		return _bits.to_ulong();
	}


	void splitToIntVolumes()
	{
		ifstream file(SoursePath, std::ios_base::in | std::ios_base::binary);
		ByteChar lengthOfFlExtens, ByteToChar;

		//считать расширение
		file.read(lengthOfFlExtens.Str, 1);
		//тут возможна утечка. 

		char* __FL_NM_EXT__ = new char[lengthOfFlExtens.Integer + 1];
		file.read(__FL_NM_EXT__, lengthOfFlExtens.Integer);
		filenameExtension = string(__FL_NM_EXT__, lengthOfFlExtens.Integer);

		delete[] __FL_NM_EXT__;


		//считать кол-во томов
		file.read(ByteToChar.Str, 1);
		NumOfThreads = ByteToChar.Integer;
		_metaCompressStr.resize(NumOfThreads);
		_compressStr.resize(NumOfThreads);

		//считать информацию о томах.
		IntChar temp;
		for (size_t i = 0; i < NumOfThreads; i++)
		{
			file.read(temp.Str, 4);
			uint32_t _length = temp.Integer;
			size_t j = 0;
			while (j < _length && file.read(temp.Str, 4)) // read не выполнится, если j>=_length. Благо операция ленива.
			{
				_metaCompressStr[i].push_back(temp.Integer);
				j++;
			}
		}


		//сначала считываю 1-байтовые слова.
		vector<uint8_t> init;
		while (file.read(ByteToChar.Str, 1))
		{
			init.push_back(ByteToChar.Integer);
		}
		string binRepres = "";
		for (auto iter = init.begin(); iter < init.end(); iter++)
			binRepres += BinRepres<uint8_t>(*iter);
		

		init.clear();
		size_t index = 0;
		for (size_t i = 0; i < NumOfThreads; i++)
		{
			size_t numOfBit = 8;
			auto fromThis = _metaCompressStr[i].begin();
			/*_metaCompressStr.resize(NumOfThreads);
			CompressStr.resize(NumOfThreads);*/
			for (size_t j = 0;
				j < *(_metaCompressStr[i].end() - 1); j++, index += numOfBit)
			{
				if (find(fromThis, _metaCompressStr[i].end(), j)
					!= _metaCompressStr[i].end())
				{
					numOfBit++, fromThis++;
				}
				bitset<32> _temp = bitset<32>(binRepres.substr(index, numOfBit));
				_compressStr[i].push_back((uint32_t)_temp.to_ulong());
			}
		}
	}

	template<typename T>
	string BinRepres(T Number, size_t numOfBits = 0)
	{
		if (numOfBits == 0)
			numOfBits = sizeof(T) * 8;
		string binaryStr = "";
		for (size_t i = 0; i < numOfBits; i++)
		{
			binaryStr +=
				((0x1 << numOfBits - i-1) & (Number)) ? "1" : "0";
		}
		



		return binaryStr;
	}
	void splitToStringVolumes()
	{
		ifstream file(SoursePath, std::ios_base::in);
		string initStr;
		char tmp[2];
		while (file.read(tmp, 1))
			initStr += tmp[0];

		size_t lenghtOfFile = initStr.size();
		size_t lenOfVolume = lenghtOfFile / NumOfThreads;


		//!!!!
		_uncompressedStr.resize(NumOfThreads);
		size_t n = 0;
		for (auto iter = initStr.begin();
			n < NumOfThreads; iter += lenOfVolume, n++)
		{
			if (n < NumOfThreads-1)
				_uncompressedStr[n] = string(iter, iter + lenOfVolume);
			else
				_uncompressedStr[n] = string(iter, initStr.end());
		}
	}


	//vector<vector<uint32_t>> _compressStr; //массивы чисел - сам архив.
	//vector<vector<uint32_t>> _metaCompressStr; //метаинформация о массивах чисел. 

	string makeMetaOfVect(size_t& numberOfVector)
		//С именем что-то не то
		//Он конечно создаёт мета-информацию, но так-то и возвращает строку.
	{
		uint32_t numOfBits = 8;

		uint32_t temp = 0;

		string binRepres = "";

		for (auto iter = _compressStr[numberOfVector].begin();
			iter < _compressStr[numberOfVector].end(); iter++)
		{
			if (*iter >= pow(2, numOfBits))
			{
				_metaCompressStr[numberOfVector].push_back(
					std::distance(_compressStr[numberOfVector].begin(), iter));
				numOfBits++;
			}
			binRepres += BinRepres<uint32_t>(*iter, numOfBits);
		}
		_metaCompressStr[numberOfVector].push_back(std::distance(_compressStr[numberOfVector].begin(),
			_compressStr[numberOfVector].end()));

		return binRepres;
	}

public:

	void set_SoursePath(string& Path)
	{
		SoursePath = Path;
	}
	void set_DestinPath(string& Path)
	{
		DestinPath = Path;
	}
	void set_NumOfThreads(size_t Num)
	{
		NumOfThreads = Num;
	}
	LZWArchivator(const string& soursePath, const string& destinPath, size_t numOfThreads) {
		SoursePath = soursePath;
		DestinPath = destinPath + "\\" + GetFileName(soursePath);
		NumOfThreads = numOfThreads;
	}
	LZWArchivator(const string& soursePath, const string& destinPath) :
		LZWArchivator(soursePath, destinPath, std::thread::hardware_concurrency()) {}

	LZWArchivator(const string& soursePath) :
		LZWArchivator(soursePath, soursePath.substr(0, soursePath.find_last_of("\\")),
			std::thread::hardware_concurrency()
		)
	{}
	LZWArchivator(const string& soursePath, size_t numOfThreads) :
		LZWArchivator(soursePath, soursePath.substr(0, soursePath.find_last_of("\\")), numOfThreads)
	{}
	//Просто напросто заглушка.
	LZWArchivator() :
		LZWArchivator(R"(C:\)", R"(C:\)", std::thread::hardware_concurrency())
	{}

	//нужно что бы архивировал в тот же путь, где и первоначальный файл

	void NewFile(const string& soursePath, const string& destinPath, size_t& numOfThreads) {
		SoursePath = soursePath;
		DestinPath = destinPath + "\\" + GetFileName(soursePath);
		NumOfThreads = numOfThreads;
	}
	void Compress() {
		DestinPath += ".lzwa";
		//ближе к процедурному стилю и файл открывается целых 2 раза. Ну и ладно.
		NumOfThreads = NumOfThreads ? NumOfThreads : 2; //Если произошла какая-то ошибка - вытянуть в 2 потока.
		splitToStringVolumes();
		_compressStr.resize(NumOfThreads);
		_metaCompressStr.resize(NumOfThreads);
		ofstream file(DestinPath, ios::out | ios::binary);

		for (int i = 0; i < NumOfThreads; i++)
		{
			CompressThreads.push_back(async(
				[uncompr = _uncompressedStr[i]](){
				return compressLZW(uncompr);
			}));
		}

		string _temp = "";
		for (size_t i = 0; i < NumOfThreads; i++)
		{
			CompressThreads[i].wait();
			_compressStr[i] = CompressThreads[i].get();
			_temp += makeMetaOfVect(i);
		}

		_uncompressedStr.clear(); // почистить вектор -> освободить память
		_compressStr.clear();
		_temp += string((_temp.size() % 8), '0'); //дозаполняет нулями до кратности 8


		GetFileExtension(SoursePath);

		file << (char)(filenameExtension.size()) << filenameExtension << (char)NumOfThreads;

		//пишет метаинформацию
		for (size_t i = 0; i < NumOfThreads; i++)
		{
			IntChar temp;
			temp.Integer = _metaCompressStr[i].size();
			file.write(temp.Str, 4);
			for (auto iter = _metaCompressStr[i].begin();
				iter < _metaCompressStr[i].end(); iter++)
			{
				temp.Integer = *iter;
				file.write(temp.Str, 4);
			}
		}

		//пишет сам архив
		bitset<8>* writedByte;
		//ByteChar temp;
		uint64_t index = 0;
		//брать по 8 символов
		for (uint64_t i = 0; i < _temp.size(); i += 8)
		{
			writedByte = new bitset<8>(_temp.substr(i, 8));
			file.put((char)(writedByte->to_ulong()));
			delete writedByte;
		}
	}

	void Decompress() {
		splitToIntVolumes();

		DestinPath += "." + filenameExtension;
		_uncompressedStr.resize(NumOfThreads);
		for (int i = 0; i < NumOfThreads; i++)
		{
			DecompressThreads.push_back(async(
				[compr = _compressStr[i]](){
				return decompressLZW(compr);
			}));
		}
		_compressStr.clear();
		ofstream file(DestinPath, ios::out | ios::binary);

		for (int i = 0; i < NumOfThreads; i++)
		{
			(DecompressThreads.at(i)).wait();
			_uncompressedStr[i] = (DecompressThreads.at(i)).get();
			for (int j = 0; j < (_uncompressedStr[i]).size(); j++)
			{
				// странная конструкция, зато убран последний CRLF
				// убран заодно и оттуда, где он нужен.
				// if (i == NumOfThreads - 1 && j == (_uncompressedStr[i]).size() - 1) break;
				file.put(_uncompressedStr[i].at(j));
			}
		}
	}
};
