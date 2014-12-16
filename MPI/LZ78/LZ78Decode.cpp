#include "LZ78Decode.h"

/** Funzione di decodifica.
*
* Questa funzione esegue l'algoritmo di decompressione LZ78 sul file di input creando il relativo file decompresso.
*
* @param[in] input Stringa contenente il nome del file di input
* @param[in] output Stringa contentente il nome del file di output
*/
int LZ78Decode::decode(string input, string output){
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);


	//Apertura degli stream in lettura e scrittura
	ifstream in(input, ios::binary);
	if (!in){
		if (rank == 0){
			cout << "Errore apertura file di input \n";
		}
		return EXIT_FAILURE;
	}



	//lettura dell'header
	string header;
	header.resize(4);
	in.read(reinterpret_cast<char*>(&header), 4);

	if (header != "LZ78"){
		if (rank == 0){
			cout << "Header non valido" << endl;
		}
		return EXIT_FAILURE;
	}


	//lettura maxbit (8)
	char c;
	//byte = c;
	//in.get(c);

	//maxbit = bitreader(in, 5);
	in.read(reinterpret_cast<char*>(&maxbit), 1);
	max_size = pow(2, maxbit);
	cout << "maxbit " << maxbit << endl;

	//lettura nproc
	int nproc = 0;
	in.read(reinterpret_cast<char *>(&nproc), 1);
	cout << "nproc " << nproc << endl;
	if (nproc < size){
		if (rank == 0){
			cout << "numero di processi in decodifica deve essere <= a " << nproc << endl;
		}
		return EXIT_FAILURE;
	}


	vector <unsigned char> file_out;
	unsigned ndata = 0, seek = 0;
	vector <unsigned> nxdata;
	vector<unsigned char> data;


	if (size > 1){

		if (rank == 0){
			
			unsigned next = 0, ndata0 = 0;
			short proc = 1;
			in.read(reinterpret_cast<char*>(&ndata0), sizeof(unsigned));
			seek = in.tellg();

			next = in.tellg(); next += ndata0;
			in.seekg(next);
		//	cout << "processo " << rank << " leggo nddata==" << ndata0  << endl;
		//	cout << "processo " << rank << " next====" << next << endl;




			for (unsigned step = 1; step < nproc; step++){
				in.read(reinterpret_cast<char*>(&ndata), sizeof(unsigned));
				
				MPI_Send(&ndata, 1, MPI_UNSIGNED, step, step, MPI_COMM_WORLD);
			//	cout << "processo " << rank << " ho mandato nddata==" << ndata << " a " << proc << endl;

				next = in.tellg();


				MPI_Send(&next, 1, MPI_UNSIGNED, step, step * 100, MPI_COMM_WORLD);
			//	cout << "processo " << rank << " ho mandato next==" << next << " a " << proc << endl;


				next += ndata;
				in.seekg(next);
			//	cout << "nextdopo== " << next << endl;
				
			//	if (proc == size - 1){ proc = 1; }
			//	else{ proc++; }

			}

			ndata = ndata0;

		}

		if (rank != 0){
		//	cout << "processo " << rank << " aspetto ndata" << endl;
			MPI_Recv(&ndata, 1, MPI_UNSIGNED,0, rank, MPI_COMM_WORLD, &status);
		//	cout << "processo " << rank << " ricevo ndata" << endl;
			MPI_Recv(&seek, 1, MPI_UNSIGNED, 0, rank*100, MPI_COMM_WORLD, &status);
		//	cout << "processo " << rank << " ricevo seek" << endl;
		
		}

		in.clear();
		in.seekg(in.beg);
		in.seekg(seek);

		data.clear();
		data.resize(ndata);
		in.read(reinterpret_cast<char*>(data.data()), ndata);

	//	cout << "proc " << rank << "  letto" << endl;

		string s;
		unsigned indice_data = 0;
		dictionary.clear();
		conta = 8;
		continua = true;
		byte = data[indice_data];
		indice_data = indice_data + 1;
		//lettura del primo byte dei dati da leggere
		//vector<unsigned char>::iterator tmp_it = data.begin();
		//byte = *tmp_it;
		//data.erase(tmp_it);


		while (continua){
			//cout << "indice data: " << indice_data << endl;
			int bitpos = ceil(log2(dictionary.size() + 1)); //numero bit da leggere per la posizione

			unsigned pos = bitreader(data, bitpos, indice_data);
			unsigned char car = (unsigned char)bitreader(data, 8, indice_data);

			if (continua == false)
				break;

			if (pos == 0){
				//out.put(car);
				file_out.push_back(car);
				s.push_back(car);
				dictionary.push_back(s);
			}
			else{
				s = dictionary[pos - 1];
				s.push_back(car);
				for (int i = 0; i < s.length(); i++)
					//out.put(s[i]);
					file_out.push_back(s[i]);
				dictionary.push_back(s);
			}
			s.clear();
			if (dictionary.size() >= max_size)
				dictionary.clear();
		}


	//	cout << "proc " << rank << "  chiamate" << endl;

		if (rank != 0){
			unsigned dim = file_out.size();

			MPI_Send(&dim, 1, MPI_UNSIGNED, 0, rank*100, MPI_COMM_WORLD);
			MPI_Send(&file_out[0], dim, MPI_UNSIGNED_CHAR, 0, rank , MPI_COMM_WORLD);

		//	cout << "processo " << rank << "mandato out" << endl;
		}

		if (rank == 0){
			
			ofstream out(output, ios::binary);
			vector <unsigned char> fout;
			out.write(reinterpret_cast<char*>(file_out.data()), file_out.size());

			unsigned dim_r = 0;
			for (short i = 1; i < size ; i++){
				dim_r = 0;
				MPI_Recv(&dim_r, 1, MPI_UNSIGNED, i, i * 100, MPI_COMM_WORLD, &status);
				fout.clear();
				fout.resize(dim_r);
				MPI_Recv(&fout[0], dim_r, MPI_UNSIGNED_CHAR, i, i, MPI_COMM_WORLD, &status);
				out.write(reinterpret_cast<char*>(fout.data()), fout.size());
			
			}
		
		}

	}
	else{
		for (unsigned step = 0; step < nproc; step++){
			in.read(reinterpret_cast<char*>(&ndata), sizeof(unsigned));
		//	cout << "dati da leggere " << ndata << endl;
			data.clear();
			data.resize(ndata);
			in.read(reinterpret_cast<char*>(data.data()), ndata);


			//------PARTE DI DECODIFICA-----------//

			string s;
			unsigned indice_data = 0;
			dictionary.clear();
			conta = 8;
			continua = true;
			byte = data[indice_data];
			indice_data = indice_data + 1;
			//lettura del primo byte dei dati da leggere
			//vector<unsigned char>::iterator tmp_it = data.begin();
			//byte = *tmp_it;
			//data.erase(tmp_it);


			while (continua){
				//cout << "indice data: " << indice_data << endl;
				int bitpos = ceil(log2(dictionary.size() + 1)); //numero bit da leggere per la posizione

				unsigned pos = bitreader(data, bitpos, indice_data);
				unsigned char car = (unsigned char)bitreader(data, 8, indice_data);

				if (continua == false)
					break;

				if (pos == 0){
					//out.put(car);
					file_out.push_back(car);
					s.push_back(car);
					dictionary.push_back(s);
				}
				else{
					s = dictionary[pos - 1];
					s.push_back(car);
					for (int i = 0; i < s.length(); i++)
						//out.put(s[i]);
						file_out.push_back(s[i]);
					dictionary.push_back(s);
				}
				s.clear();
				if (dictionary.size() >= max_size)
					dictionary.clear();
			}
			//return EXIT_SUCCESS
		}
	
		//creazione stream di output
		ofstream out(output, ios::binary);
		out.write(reinterpret_cast<char*>(file_out.data()), file_out.size());
	
	
	}




}


	



/** Funzione di lettura a bit.
*
* Lettura di n bit dal vettore di caratteri data attraverso la funzione check_read 
*
* @param[in] data Vettore di input
* @param[in] n Numero di bit da leggere
* @param[in] indice_data Indice del vettore data del byte di cui si stanno estraendo i bit
*/
unsigned LZ78Decode::bitreader(vector<unsigned char> &data, unsigned n, unsigned& indice_data){
	unsigned pos = n - 1;
	unsigned buffer = 0;
	for (int i = 0; i < n; i++){
		unsigned mask = byte >> conta - 1;
		mask = mask & 1;
		mask = mask << pos;
		buffer = buffer | mask;
		conta = conta - 1;
		pos = pos - 1;
		check_read(data,indice_data);
		if (continua == false)
			break;
	}
	return buffer;
}

/** Funzione di controllo per l'estrazione dei bit. Quando viene completata l'estrazione dell'intero byte 
*corrente, viene letto il byte successivo dal vettore data
*
* @param[in] data Vettore di input
* @param[in] indice_data Indice del vettore data del byte di cui si stanno estraendo i bit
*/
void LZ78Decode::check_read(vector<unsigned char> &data, unsigned &indice_data){
	if (conta == 0){
		char c;
		if (indice_data<data.size()){
			//vector<unsigned char>::iterator c = data.begin();
			//byte = *c; //in byte inserisco il primo elemento del vettore data
			byte = data[indice_data];
			indice_data = indice_data + 1;
			conta = 8;
			//dopo aver memorizzato il contenuto del primo elemento in byte, lo cancello dall'array
			//data.erase(c);
			//cout << "dovrei aver cancellato.." << endl;
		}
		else{
			cout << "sono entrata nell'else.. devo terminare" << endl;
			continua = false;
		}
	}
}