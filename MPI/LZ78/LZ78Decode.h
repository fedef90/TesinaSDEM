#ifndef LZ78DECODE
#define LZ78DECODE

#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <mpi.h>

using namespace std;

/** Classe che realizza la decompressione di un file attraverso l'algoritmo LZ78.
*
*/
class LZ78Decode{
	//variabili mpi, id-proc e nproc
	int rank, size;

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
	unsigned bitreader(vector<unsigned char>& data, unsigned n, unsigned& indice_data);
	void check_read(vector<unsigned char>& in, unsigned &indice_data);

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