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
	
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	

	vector <unsigned char> file;
	vector <unsigned char> file_out;
	unsigned f_end = 0, offset = 0, p = 0, stop = 0;

	//Apertura degli stream in lettura e scrittura

	//creazione stream di output
	ofstream out(output, ios::binary);

//	MPI_File_open(MPI_COMM_WORLD, &output[0], MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &out_m);

		ifstream in(input, ios::binary);
		if (!in){
			cout << "Errore apertura file di input \n";
			return EXIT_FAILURE;
		}


		if (rank == 0){
			//scrittura header
			out << "LZ78";
			//out.put(size);
			out.write(reinterpret_cast<char*>(&maxbit), 1);
			out.write(reinterpret_cast<char*>(&size), 1);
			//scrittura dei 5 bit che indicano maxbits
			//bitwriter(maxbit, 5, file_out);
			
		}

		in.clear();
		in.seekg(0,in.end);
		f_end = in.tellg();
		offset = (int)(f_end / size); //quantità di byte che ogni PE deve leggere
		stop = (offset*rank) + offset;
		if ((rank == size - 1) && (rank>0)){
			if ((f_end%size) != 0){
				stop += f_end%size;
			}
		}
		offset *= rank;
		cout << "proc " << rank << "  file grande " << f_end <<"  inizio== "<<offset<<  "  stop== " << stop << endl;

		in.clear();
		in.seekg(in.beg);
		in.seekg(offset);
		unsigned char uc = 0;

	/*	while (offset<stop){
			offset++;
			in >> noskipws >> uc;
			file.push_back(uc);
		}
		*/

		file.clear();
		file.resize(stop - offset);
		in.read(reinterpret_cast<char*>(file.data()),stop-offset);


		cout << "proc " << rank << "  input grande " << file.size() << endl;

		in.clear();
		in.seekg(in.beg);


	unsigned char c;
	string s;
	int last_position=0; //ultima posizione trovata nel dizionario
	// while (in.get(c)){
	//while (p<offset){
	for (unsigned i = 0; i < file.size(); i++){
		c = file[i];
		//in.get(c); p++;
		if (dictionary.size() >= max_size)
			dictionary.clear();
		//numero bit da scrivere per la codifica della posizione
		int bitpos = ceil(log2(dictionary.size() + 1));
		
		//controllo se il carattere è presente nel dizionario
		s.push_back(c);
		
		int pos = check_dictionary(s, last_position);

		if (pos == -1){
			if (s.length() == 1){
				bitwriter(0, bitpos, file_out);
				dictionary.push_back(s);
			}
			else{
				bitwriter(last_position+1, bitpos, file_out);
				dictionary.push_back(s);
			}
			bitwriter(c, 8, file_out);
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

		bitwriter(pos+1, bitpos, file_out);
		bitwriter(c, 8, file_out);
	}



	if (conta != 0){
		file_out.push_back(byte);
		//out.put(byte);
		//MPI_File_write_at(out, offset_w, &byte, 1, MPI_CHAR, &status);
	}
	
	unsigned dim_s = file_out.size(), dim_r = 0;
		
	vector<unsigned char> fout;

	if (size > 1){

		if (rank != 0){
			MPI_Send(&dim_s, 1, MPI_INT, 0, rank * 100, MPI_COMM_WORLD);
	//		cout << rank << " ho mandato dim_s che è " << dim_s << endl;
			MPI_Send(&file_out[0], dim_s, MPI_CHAR, 0, rank, MPI_COMM_WORLD);
	//		cout << rank << " ho mandato file_out "  << endl;
		}

		if (rank == 0){
			//out.put(dim_s);
			cout << "dims " << dim_s << endl;
			out.write(reinterpret_cast<char*>(&dim_s), sizeof(unsigned));

			for (unsigned j = 0; j < file_out.size(); j++){
				out << noskipws << file_out[j];
			}
			for (unsigned i = 1; i <= size - 1; i++){
		//		cout << "sono 0 aspetto di ricevere\n";
				dim_r = 0;
				MPI_Recv(&dim_r, 1, MPI_INT, i, i*100, MPI_COMM_WORLD, &status);
				cout << rank << " ho ricevuto dim_r che è " << dim_r << endl;
				fout.clear();
				fout.resize(dim_r);
				cout << fout.size() << endl;
				MPI_Recv(&fout[0], dim_r, MPI_CHAR, i, i, MPI_COMM_WORLD, &status);
				cout << "ho ricevuto fout\n";

				//out.put(dim_r);
				out.write(reinterpret_cast<char*>(&dim_r), sizeof(unsigned));
				for (unsigned j = 0; j < fout.size(); j++){
					out << noskipws << fout[j];
				}
				
			}
		}

	}
	else{
		out.write(reinterpret_cast<char*>(&dim_s), sizeof(unsigned));

		for (unsigned j = 0; j < file_out.size(); j++){
			out << noskipws << file_out[j];
		}
	
	
	}
		
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
void LZ78Encode::bitwriter(unsigned x, unsigned num, vector<unsigned char> &file_out){
	for (int i = num - 1; i >= 0; i--){
		unsigned mask = x >> i;
		mask = mask & 1;
		mask = mask << conta - 1;
		byte = byte | mask;
		conta--;
		scrivi_byte(file_out);
	}
}

/** Funzione di scrittura dei byte sul file di output.
*
* @param[in] out Stream di output
*/
void LZ78Encode::scrivi_byte(vector<unsigned char> & file_out){
	if (conta == 0){
		file_out.push_back(byte);
		//out.put(byte);
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