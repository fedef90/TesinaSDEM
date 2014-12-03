#include "LZ78Decode.h"

int LZ78Decode::Decode(istream& in, ostream& out)
{
	//lettura dell'header
	string header;
	header.resize(4);
	in.read(reinterpret_cast<char*>(&header), 4);

	if (header != "LZ78")
	{
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
	while (continua)
	{
		int bitpos = ceil(log2(dictionary.size() + 1)); //numero bit da leggere per la posizione
		
		unsigned pos = bitreader(in, bitpos);
		unsigned char car = (unsigned char) bitreader(in, 8);

		if (continua == false)
			break;

		if (pos == 0)
		{
			out.put(car);
			s.push_back(car);
			dictionary.push_back(s);
		}
		else
		{
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

unsigned LZ78Decode::bitreader(istream& in, unsigned n){
	unsigned pos = n - 1;
	unsigned buffer = 0;
	for (int i = 0; i < n;i++)
	{
		unsigned mask = byte >> conta - 1;
		mask = mask & 1;
		mask = mask << pos;
		buffer = buffer | mask;
		conta = conta - 1;
		pos = pos - 1;
		CheckRead(in);
		if (continua == false)
			break;
	}
	return buffer;

}

void LZ78Decode::CheckRead(istream& in){
	if (conta == 0)
	{
		char c;
		if (in.get(c))
		{
			byte = c;
			conta = 8;
		}
		else
		{
			continua = false;
		}

	}

}