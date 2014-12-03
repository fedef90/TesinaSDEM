#include "LZ78Encode.h"
#include "LZ78Decode.h"
#include <time.h>
#include <sys/stat.h>

int main(int argc, char* argv[])
{

	double tempo;
	clock_t start, end;


	start = clock();

	if (argc != 4)
	{
		cout << "errore: numero argomenti passati sbagliati" << endl;
		return EXIT_FAILURE;
	}

	//controlli per il parametro maxbit
	unsigned maxbit = atoi(argv[1]);

	if (maxbit < 1 || maxbit > 31)
	{
		cout << "errore nell'inserimento dell'argomento maxbit" << endl;
		return EXIT_FAILURE;
	}

	//memorizzo nome file input e output per la codifica
	string file_input = argv[2];
	string file_output = argv[3];
	
	//apro gli stream in lettura e scrittura
	ifstream in(file_input,ios::binary);
	if (!in){
		cout << "errore apertura file" << endl;
		return EXIT_FAILURE;
	}
	ofstream out(file_output, ios::binary);
	
	LZ78Encode comprimi(maxbit);
	comprimi.encode(in, out);

	in.close();
	out.close();
	end = clock();
	tempo = ((double)(end - start));
	cout << "tempo codifica: " << tempo/1000 << endl;

	cout << "inizia la decompressione..." << endl;
	
	ifstream input(file_output,ios::binary);
	cout << "inserisci nome per il file output" << endl;
	string s="test-bibbia.txt";
	ofstream output(s,ios::binary);
	LZ78Decode decomprimi;
	decomprimi.Decode(input, output);

	
	
	



}