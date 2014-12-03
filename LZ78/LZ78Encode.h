#ifndef LZ78ENCODE

#define LZ78ENCODE

#include <vector>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

class LZ78Encode
{
	//dizionario
	vector<string> dictionary;

	unsigned maxbit;
	const unsigned max_size = pow(2, maxbit);
	
	//per la scrittura su file
	unsigned char byte; //byte in attesa di essere scritto in output
	unsigned conta; //contatore che mi indica quanti bit devo ancora inserire prima di scrivere su output

	//scrittura di num bit di x
	void bitwriter(unsigned x, unsigned num, ostream& out);

	//controlla se è necessario scrivere il byte in output
	void ScriviByte(ostream& out);

	//controlla se la stringa s è presente nel dizionario e ritorno la posizione
	// -1 se non la trovo.
	int CheckDictionary(string s, int last_position);

public:
	LZ78Encode(unsigned bitmax) : maxbit(bitmax),byte(0),conta(8){}
	int encode(string input, string output);
	







};




#endif