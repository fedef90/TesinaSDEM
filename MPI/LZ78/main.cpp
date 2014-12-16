#include "LZ78Encode.h"
#include "LZ78Decode.h"
#include <time.h>
#include <sys/stat.h>



/** @mainpage <center> Documentazione </center>
* Questo programma implementa la compressione di un file attraverso la codifica LZ78 e la relativa
* decompressione, parallelizzando il calcolo attraverso l'esecuzione di pi&ugrave; processi. \n
*
* <p align = "left">
* Per l'esecuzione &egrave; necessario invocare il comando mpiexec da linea di comando indicando:
* <ul type = "square">
* <li> "-n" e il numero di processi su cui parallelizzare la codifica o la decodifica </li>
* <li> il nome del file eseguibile </li>
* <li> "--help" per la guida </li>
* <li> "--encode" per la codifica </li>
* <li> "--decode" per la decodifica </li>
* </ul> </p>
*
* <p align = "left">
* In caso di <B>codifica</B>:
* <ul type = "square">
* <li> "-i" e il nome del file da comprimere [obbligatorio] </li>
* <li> "-o" e il nome del file compresso [opzionale] </li>
* <li> - "-b" e il numero massimo di bit da utilizzare per salvare le posizioni e quindi anche per la dimensione massima 
* del dizionario [opzionale, default = 8] </li>
* </ul> </p>
*
* <p align = "left">
* In caso di <B>decodifica</B>: 
* <ul type = "square">
* <li> "-i" e il nome del file da decomprimere [obbligatorio] </li>
* <li> "-o" e il nome del file decompresso [opzionale] </li>
* </ul> </p>
*
* <p align = "left">
* Ad esempio per la compressione del file di input utilizzando 10 bit per la dimensione massima del dizionario e parallelizzando
* la codifica con 4 processi: \n 
* mpiexec -n 4 LZ78parallelo --encode -b 10 -i nomefileinput -o nomefileoutput \n </p>
*
*
* @authors Marcella Cornia
* @authors Federica Fergnani
* @authors Riccardo Gasparini
*/

/** Funzione main.
*
* La funzione crea un oggetto della classe LZ78Encode o un oggetto della classe LZ78Decode 
* e chiama la relativa funzione di codifica o decodifica.
* Infine restituisce il risultato ottenuto sotto forma di tempo di esecuzione impiegato e, in caso di codifica,
* di fattore di compressione ottenuto.
*
*/

int main(int argc, char* argv[]){
		int rank, size;
	
		MPI_Init(&argc, &argv);
		MPI_Comm_rank(MPI_COMM_WORLD, &rank);
		MPI_Comm_size(MPI_COMM_WORLD, &size);

		double tempo;
		clock_t start, end;
		unsigned maxbits = 8;
		string file_input;
		string file_output;

		bool help = false;
		bool bit = false;
		bool input = false;
		bool output = false;
		bool encode = false;
		bool decode = false;
		bool errore = true;
		bool controllo = false;

		start = clock();

		unsigned i = 1;
		while (i < argc){
			string arg = argv[i];
			if (arg =="--help"){
				help = true;
			}
			if (arg == "--encode"){
				if (encode){
					controllo = true;
				}
				encode = true;
			}
			if (arg == "--decode"){
				if (decode){
					controllo = true;
				}
				decode = true;
			}
			if (arg == "-b"){
				i++;
				if (i < argc){
					maxbits = atoi(argv[i]);
					if (bit){
						controllo = true;
					}
					bit = true;
				}
			}
			if (arg == "-i"){
				i++;
				if (i < argc){
					file_input = argv[i];
					if (input){
						controllo = true;
					}
					input = true;
				}
			}
			if (arg == "-o"){
				i++;
				if (i < argc){
					file_output = argv[i];
					if (output){
						controllo = true;
					}
					output = true;
				}
			}
			i++;
		}
		
		if (help && (argc==2)){
			errore = false;
			if (rank == 0){
				cout << "HELP LZ78parallelo \n \n";
				cout << "Per comprimere il file con la codifica LZ78: \n";
				cout << "LZ78parallelo --encode -i <nome file input> [-b <numero bit>] [-o <nome file output>] \n \n";
				cout << "Per decomprimere il file con la decodifica LZ78: \n";
				cout << "LZ78parallelo --decode -i <nome file input> [-o <nome file output>] \n \n";
				cout << "\t -i \t nome del file da comprimere/decomprimere \n";
				cout << "\t -o \t nome del file compresso/decompresso \n";
				cout << "\t -b \t numero di bit per la dimensione massima del dizionario (da specificare solo in caso di codifica) \n";
			}
			MPI_Finalize();
			return EXIT_FAILURE;
		}

		if (encode && input && !decode && !help && !controllo){
			if (maxbits > 0 && maxbits < 32){
				errore = false;
			}
		}

		if (decode && input && !encode && !bit && !help && !controllo){
			errore = false;
		}

		if (errore){
			if (rank == 0){
				cout << "Errore! Consulta la guida con: LZ78parallelo --help \n";
			}
			MPI_Finalize();
			return EXIT_FAILURE;
		}

		if (encode){
			cout << "Encoding ... " << rank << endl;
			LZ78Encode comprimi(maxbits);
			if (!output){
				file_output = file_input;
				unsigned found = file_output.find_last_of(".");
				file_output.replace(found + 1, 4, "lz78");
			}
			if (output){
				int found = -1;
				found = file_output.find_last_of(".");
				if (found == -1){
					file_output = file_output + ".lz78";
				}
				else{
					file_output.replace(found + 1, file_output.length() - found + 1, "lz78");
				}
			}

			comprimi.encode(file_input, file_output);

			if (rank == 0){
				struct stat sstr1, sstr2;
				int res1 = stat((char *)&file_input, &sstr1);
				int res2 = stat((char *)&file_output, &sstr2);
				double size1 = sstr1.st_size;
				double size2 = sstr2.st_size;
				double fat = size2 / size1;

				cout << "E' stato creato il file compresso " << file_output << "\n";
				cout << "Fattore di compressione: " << fat * 100 << "% \n\n";
			}
		}

		if (decode){
			cout << "Decoding ...\n";

			ifstream in(file_input, ios::binary);
			if (!in){
				if (rank == 0){
					cout << "File di input non trovato \n";
				}
				MPI_Finalize();
				return EXIT_FAILURE;
			}
			string header;
			header.resize(4);
			in.read(reinterpret_cast<char*>(&header), 4);
			char dim_est;
			in.get(dim_est);
			string estensione;
			estensione.resize(dim_est);
			in.read(reinterpret_cast<char*>(&estensione), dim_est);
			in.close();

			if (!output){
				file_output = file_input;
				unsigned found = file_output.find_last_of(".");
				file_output.replace(found, file_output.length() - found + 1, "-dec." + estensione);
			}
			if (output){
				int found = -1;
				found = file_output.find_last_of(".");
				if (found == -1){
					file_output = file_output + "." + estensione;
				}
				else{
					file_output.replace(found + 1, file_output.length() - found + 1, estensione);
				}
			}
			LZ78Decode decomprimi;
			decomprimi.decode(file_input, file_output);
			if (rank == 0){
				cout << "E' stato creato il file decompresso " << file_output << "\n";
			}
		}

		end = clock();
		tempo = ((double)(end - start));
		if (rank == 0){
			cout << "Tempo di esecuzione: " << tempo / 1000 << " s \n";
		}

		MPI_Finalize();
}