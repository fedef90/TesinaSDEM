#ifndef LZ78ENCODE
#define LZ78ENCODE

/*
#include <mpi.hpp>
#include <mpi/environment.hpp>
#include <mpi/communicator.hpp>
namespace mpi = boost::mpi;
*/

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <mpi.h>

using namespace std;

/** Classe che realizza la compressione di un file attraverso l'algoritmo LZ78.
*
*@param[in] maxbits Numero m./assimo di bits utilizzati per stabilire la dimensione del dizionario in fase di codifica
*/
class LZ78Encode{
	//dizionario
	vector<string> dictionary;

	unsigned maxbit;
	const unsigned max_size = pow(2, maxbit);
	
	//per la scrittura su file
	unsigned char byte; //byte in attesa di essere scritto in output
	unsigned conta; //contatore che mi indica quanti bit devo ancora inserire prima di scrivere su output

	//variabili mpi
	int rank, size;
	MPI_Status status;

	//scrittura di num bit di x
	void bitwriter(unsigned x, unsigned num, vector<unsigned char> & file_out);

	//controlla se � necessario scrivere il byte in output
	void scrivi_byte(vector<unsigned char> & file_out);

	//controlla se la stringa s � presente nel dizionario e ritorno la posizione
	// -1 se non la trovo.
	int check_dictionary(string s, int last_position);

public:
	/** Costruttore della classe LZ78Encode.
	*/
	LZ78Encode(unsigned bitmax) : maxbit(bitmax),byte(0),conta(8){}

	int encode(string input, string output);
	
	/** Distruttore della classe LZ78Encode.
	*/
	~LZ78Encode(){};
};


#endif