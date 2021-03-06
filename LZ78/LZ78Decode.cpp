#include "LZ78Decode.h"

/** Funzione di decodifica.
*
* Questa funzione esegue l'algoritmo di decompressione LZ78 sul file di input creando il relativo file decompresso.
*
* @param[in] input Stringa contenente il nome del file di input
* @param[in] output Stringa contentente il nome del file di output
*/
int LZ78Decode::decode(string input, string output){
	//Apertura degli stream in lettura e scrittura
	ifstream in(input, ios::binary);
	if (!in){
		cout << "Errore apertura file di input \n";
		return EXIT_FAILURE;
	}

	//creazione stream di output
	ofstream out(output, ios::binary);

	//lettura dell'header
	string header;
	header.resize(4);
	in.read(reinterpret_cast<char*>(&header), 4);

	if (header != "LZ78"){
		cout << "Header non valido" << endl;
		return EXIT_FAILURE;
	}

	//lettura maxbit (5)
	char c;
	in.get(c);
	byte = c;

	maxbit = bitreader(in, 5);
	max_size = pow(2, maxbit);
	string s;
	while (continua){
		int bitpos = ceil(log2(dictionary.size() + 1)); //numero bit da leggere per la posizione
		
		unsigned pos = bitreader(in, bitpos);
		unsigned char car = (unsigned char) bitreader(in, 8);

		if (continua == false)
			break;

		if (pos == 0){
			out.put(car);
			s.push_back(car);
			dictionary.push_back(s);
		}
		else{
			s = dictionary[pos - 1];
			s.push_back(car);
			for (int i = 0; i < s.length(); i++)
				out.put(s[i]);
			dictionary.push_back(s);
		}
		s.clear();
		if (dictionary.size() >= max_size)
			dictionary.clear();
	}
	return EXIT_SUCCESS;
}

/** Funzione di lettura a bit.
*
* Lettura di n bit dal file di input attraverso la funzione check_read che estrae i byte dal file quando
* pi&egrave; necessario.
*
* @param[in] in Stream di input
* @param[in] n Numero di bit da leggere
*/
unsigned LZ78Decode::bitreader(istream& in, unsigned n){
	unsigned pos = n - 1;
	unsigned buffer = 0;
	for (int i = 0; i < n; i++){
		unsigned mask = byte >> conta - 1;
		mask = mask & 1;
		mask = mask << pos;
		buffer = buffer | mask;
		conta = conta - 1;
		pos = pos - 1;
		check_read(in);
		if (continua == false)
			break;
	}
	return buffer;
}

/** Funzione di lettura dei byte dal file di input.
*
* @param[in] in Stream di input
*/
void LZ78Decode::check_read(istream& in){
	if (conta == 0){
		char c;
		if (in.get(c)){
			byte = c;
			conta = 8;
		}
		else{
			continua = false;
		}
	}
}