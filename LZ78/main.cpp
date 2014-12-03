#include "LZ78Encode.h"
#include "LZ78Decode.h"
#include <time.h>
#include <sys/stat.h>

int main(int argc, char* argv[])
{
	double tempo;
	clock_t start, end;
	const unsigned maxbits = 10;
	const unsigned numproc = 1;

	start = clock();

	if (argc != 4)
	{
		cout << "Numero di argomenti passati errato \n";
		cout << "LZ78parallelo <ENC/DEC> <input filename> <output filename> \n";
		return EXIT_FAILURE;
	}

	//Controllo per il flag di codifica o decodifica
	string flag = argv[1];
	if (flag != "ENC" && flag != "DEC"){
		cout << "Il primo argomento deve essere ENC (per la codifica) o DEC (per la decodifica) \n";
		return EXIT_FAILURE;
	}

	//Memorizzazione nome file input e output per la codifica
	string file_input = argv[2];
	string file_output = argv[3];
	
	if (flag == "ENC"){
		cout << "Encoding ...\n";
		LZ78Encode comprimi(maxbits);
		comprimi.encode(file_input, file_output);

		struct stat sstr1, sstr2;
		int res1 = stat(argv[2], &sstr1);
		int res2 = stat(argv[3], &sstr2);
		double size1 = sstr1.st_size;
		double size2 = sstr2.st_size;
		double fat = size2 / size1;

		cout << "Fattore di compressione: " << fat * 100 << "% \n\n";
	}

	if (flag == "DEC"){
		cout << "Decoding ...\n";
		LZ78Decode decomprimi;
		decomprimi.Decode(file_input, file_output);
	}

	end = clock();
	tempo = ((double)(end - start));
	cout << "Tempo di esecuzione: " << tempo / 1000 << " s \n";


}