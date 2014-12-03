#include "LZ78Encode.h"


//codifica LZ78
int LZ78Encode::encode(string input, string output)
{
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
	while (in.get(c))
	{
		if (dictionary.size() >= max_size)
			dictionary.clear();
		//numero bit da scrivere per la codifica della posizione
		int bitpos = ceil(log2(dictionary.size() + 1));
		
		//controllo se il carattere è presente nel dizionario
		s.push_back(c);
		
		int pos = CheckDictionary(s,last_position);

		if (pos == -1)
		{
			if (s.length() == 1)
			{
				bitwriter(0, bitpos, out);
				dictionary.push_back(s);

			}
			else
			{
				//s.pop_back();
				//pos = CheckDictionary(s);
				bitwriter(last_position+1, bitpos, out);
				//s.push_back(c);
				dictionary.push_back(s);
			}

			bitwriter(c, 8, out);
			s.clear();
			last_position = 0;
		}
		else
		{
			//memorizzo la posizione della stringa che mi ha trovato
			last_position = pos;
		}
		
	}
	
	if (dictionary.size() >= max_size)
		dictionary.clear();
	if (s.length() > 0)
	{
		s.pop_back();

		int pos = CheckDictionary(s,0);

		int bitpos = ceil(log2(dictionary.size() + 1));

		bitwriter(pos+1, bitpos, out);
		bitwriter(c, 8, out);
	}
	if (conta != 0)
		out.put(byte);

	return EXIT_SUCCESS;
}

/**
 * scrittura di num bit di x nel file out
 * 
*/
void LZ78Encode::bitwriter(unsigned x, unsigned num, ostream& out)
{
	for (int i = num - 1; i >= 0; i--)
	{
		unsigned mask = x >> i;
		mask = mask & 1;
		mask = mask << conta - 1;
		byte = byte | mask;
		conta--;
		ScriviByte(out);
	}


}

void LZ78Encode::ScriviByte(ostream& out)
{
	if (conta == 0)
	{
		out.put(byte);
		conta = 8;
		byte = 0;
	}

}

int LZ78Encode::CheckDictionary(string s, int last_position)
{
	for (int i = last_position; i < dictionary.size(); i++)
	{
		if (dictionary[i] == s)
			return i;

	}
	return -1;


}