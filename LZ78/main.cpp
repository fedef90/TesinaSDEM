#include "LZ78Encode.h"
#include "LZ78Decode.h"
#include <time.h>
#include <sys/stat.h>

/**@mainpage <center> Compressore LZ78 Parallelo </center>
*
* Questo programma implementa la compressione di un file attraverso l'algoritmo LZ78 e la relativa 
* decompressione, parallelizzando il calcolo su pi&ugrave; processori.
*
* @authors Marcella Cornia
* @authors Federica Fergnani
* @authors Riccardo Gasparini
*/

/** Funzione main.
*
* Gli argomenti passati da linea di comando devono essere:
* - "ENC" o "DEC" rispettivamente per codifica e decodifica
* - nome del file di input
* - nome del file di output
*
* La funzione crea un oggetto della classe LZ78Encode o un oggetto della classe LZ78Decode a seconda  che il primo
* argomento passato sia "ENC" o "DEC" e chiama la relativa funzione di codifica o decodifica.
* Infine restituisce il risultato ottenuto sotto forma di tempo di esecuzione impiegato e, in caso di codifica, 
* di fattore di compressione ottenuto.
*
*/

int main(int argc, char* argv[])
{
	double tempo;
	clock_t start, end;
	const unsigned maxbits = 8;
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
		decomprimi.decode(file_input, file_output);
	}

	end = clock();
	tempo = ((double)(end - start));
	cout << "Tempo di esecuzione: " << tempo / 1000 << " s \n";
}