#ifndef LZ78DECODE
#define LZ78DECODE

#include <vector>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

/** Classe che realizza la decompressione di un file attraverso l'algoritmo LZ78.
*
*/
class LZ78Decode{
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
	void check_read(istream& in);

public:
	/** Costruttore della classe LZ78Decode.
	*/
	LZ78Decode():byte(0),conta(8){}

	int decode(string input, string output);

	/** Distruttore della classe LZ78Decode.
	*/
	~LZ78Decode(){};

};

#endif