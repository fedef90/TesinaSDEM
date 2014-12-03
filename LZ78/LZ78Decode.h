#ifndef LZ78DECODE
#define LZ78DECODE

#include <vector>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

class LZ78Decode
{
	//dizionario
	vector<string> dictionary;

	//dimensione dizionario
	unsigned maxbit;
	unsigned max_size;

	//per la lettura
	unsigned conta;
	unsigned char byte;
	bool continua = true;


	//lettura dallo stream di in di n bit
	unsigned bitreader(istream& in, unsigned n);
	void CheckRead(istream& in);

public:
	//costruttore
	LZ78Decode():byte(0),conta(8){}

	//decodifica LZ78
	int Decode(istream& in, ostream& out);

	~LZ78Decode(){};



};

#endif