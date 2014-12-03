#include "LZ78Encode.h"

/** Funzione di codifica.
*
* Questa funzione esegue l'algoritmo di compressione LZ78 sul file di input creando il relativo file compresso.
* L'intestazione del file compresso comprende:
* - il magic number "LZ78" (4 byte)
* - un valore intero a 5 bit senza segno che indica il numero massimo di bit utilizzati per il dizionario durante la codifica
*
* @param[in] input Stringa contenente il nome del file di input
* @param[in] output Stringa contentente il nome del file di output
*/
int LZ78Encode::encode(string input, string output){
	//Apertura degli stream in lettura e scrittura
	ifstream in(input, ios::binary);
	if (!in){
		cout << "Errore apertura file di input \n";
		return EXIT_FAILURE;
	}

	//creazione stream di output
	ofstream out(output, ios::binary);

	//scrittura header
	out << "LZ78";
	
	//scrittura dei 5 bit che indicano maxbits
	bitwriter(maxbit, 5, out);
	
	char c;
	string s;
	int last_position=0; //ultima posizione trovata nel dizionario
	while (in.get(c)){
		if (dictionary.size() >= max_size)
			dictionary.clear();
		//numero bit da scrivere per la codifica della posizione
		int bitpos = ceil(log2(dictionary.size() + 1));
		
		//controllo se il carattere è presente nel dizionario
		s.push_back(c);
		
		int pos = check_dictionary(s, last_position);

		if (pos == -1){
			if (s.length() == 1){
				bitwriter(0, bitpos, out);
				dictionary.push_back(s);
			}
			else{
				bitwriter(last_position+1, bitpos, out);
				dictionary.push_back(s);
			}
			bitwriter(c, 8, out);
			s.clear();
			last_position = 0;
		}
		else{
			//memorizzo la posizione della stringa che mi ha trovato
			last_position = pos;
		}		
	}
	if (dictionary.size() >= max_size)
		dictionary.clear();
	if (s.length() > 0){
		s.pop_back();

		int pos = check_dictionary(s,0);

		int bitpos = ceil(log2(dictionary.size() + 1));

		bitwriter(pos+1, bitpos, out);
		bitwriter(c, 8, out);
	}
	if (conta != 0)
		out.put(byte);

	return EXIT_SUCCESS;
}

/** Funzione di scrittura a bit.
 * 
 * Scrittura di num bit di x sul file out chiamando la funzione scrivi_byte.
 *
 * @param[in] x Elemento che contiene i bit da scrivere sul file di output
 * @param[in] num Numero di bit di x che si scrivono sul file di output
 * @param[in] out Stream di output
*/
void LZ78Encode::bitwriter(unsigned x, unsigned num, ostream& out){
	for (int i = num - 1; i >= 0; i--){
		unsigned mask = x >> i;
		mask = mask & 1;
		mask = mask << conta - 1;
		byte = byte | mask;
		conta--;
		scrivi_byte(out);
	}
}

/** Funzione di scrittura dei byte sul file di output.
*
* @param[in] out Stream di output
*/
void LZ78Encode::scrivi_byte(ostream& out){
	if (conta == 0){
		out.put(byte);
		conta = 8;
		byte = 0;
	}
}

/** Funzione di ricerca di una stringa nel dizionario.
*
* Questa funzione ricerca la stringa s all'interno del dizionario partendo dall'ultima posizione corrispondente alla stringa
* trovata nella ricerca precedente.
*
* @param[in] s Stringa da cercare nel dizionario
* @param[in] last_position Ultima posizione trovata nella ricerca precedente
* @return Il valore di ritorno contiene la posizione del dizionario relativa alla stringa s altrimenti -1
*/
int LZ78Encode::check_dictionary(string s, int last_position){
	for (int i = last_position; i < dictionary.size(); i++){
		if (dictionary[i] == s)
			return i;
	}
	return -1;
}