#include <fstream>
#include <iostream>
#include <vector>
#include <string>

using namespace std;

class LZ78{
	ifstream &_is;
	ofstream &_os;
	unsigned _maxbits;
	vector <unsigned char> _input;
	vector <string> _dictionary;
	const int _maxdim;
	unsigned _maxlength;
	unsigned char _buffer;
	int _bits;
	int _numbits;

	void write_bit(unsigned u, ofstream &os) {
		_buffer = (_buffer << 1) | (u & 1);
		if (++_bits == 8) {
			os.put(_buffer);
			_bits = 0;
			_buffer = 0;
		}
	}

	void bitwriter(unsigned x, unsigned num, ofstream &os){
		_numbits = num;
		while (_numbits > 0)
			write_bit(x >> --_numbits, os);
	}

	unsigned bitreader(unsigned char u, int bit){
		_numbits = bit;
		_buffer = 0;
		unsigned x;
		while (_numbits > 0)
		{
			x = u >> --_numbits;
			_buffer = (_buffer << 1) | (x & 1);
		}
		return _buffer;
	}

	bool trova_stringa(string st, unsigned &id) {
		for (unsigned i = 0; i < _dictionary.size(); i++){
			if (_dictionary[i].size() == st.size()){
				if (st.compare(_dictionary[i]) == 0){
					id = i;
					return true;
				}
			}
		}
		return false;
	}

public:
	LZ78(ifstream &is, ofstream &os, unsigned maxbits) :
		_is(is), _os(os), _maxbits(maxbits), _maxlength(0), _buffer(0), _bits(0), _numbits(0), _maxdim((int)(pow(2, maxbits))) {}

	void codifica(){
		unsigned char u;
		unsigned it = 0;
		while (_is >> u){
			_input.push_back(u);
		}

		//Intestazione del file di output
		_os.put('L');
		_os.put('Z');
		_os.put('7');
		_os.put('8');

		bitwriter(_maxbits, 5, _os);

		while (it < _input.size()){
			string app;
			if (_maxlength == 0){
				if (_dictionary.size()>0){
					bitwriter(0, (unsigned)(log2(_dictionary.size()) + 1), _os);
				}
				app.append(1, _input[it]);
				bitwriter(_input[it], 8, _os);
				cout << "- " << "num bit: - " << "\t" << _input[it];
				_dictionary.push_back(app);
				cout << "\t" << _dictionary.size() << "\t" << app << endl;
				it++;
				_maxlength++;
				app.clear();
			}
			else {
				int k;
				unsigned id;
				if (it + _maxlength < _input.size() - 1){
					app.clear();
					if (_dictionary.size() == _maxdim){
						_dictionary.clear();
						app.append(1, _input[it]);
						_maxlength = 1;
						_dictionary.push_back(app);
						bitwriter(_input[it], 8, _os);
						cout << "- " << "num bit: - " << "\t" << _input[it];
						cout << "\t" << _dictionary.size() << "\t" << app << endl;
						it++;
						app.clear();
					}
					else {
						for (k = _maxlength; k >= 0; k--){
							app.clear();
							for (unsigned i = it; i < it + k; i++){
								app.append(1, _input[i]);
							}
							if (trova_stringa(app, id) == true){
								bitwriter(id + 1, (unsigned)(log2(_dictionary.size()) + 1), _os);
								bitwriter(_input[it + app.size()], 8, _os);
								cout << id + 1 << " num bit: " << (unsigned)(log2(_dictionary.size()) + 1) << "\t" << _input[it + app.size()];
								app.append(1, _input[it + app.size()]);
								_dictionary.push_back(app);
								cout << "\t" << _dictionary.size() << "\t" << app << endl;
								it = it + app.size();
								if (app.size()>_maxlength){
									_maxlength = app.size();
								}
								app.clear();
								k = -1;
							}
							if (k == 0){
								app.clear();
								bitwriter(0, (unsigned)(log2(_dictionary.size()) + 1), _os);
								bitwriter(_input[it], 8, _os);
								cout << 0 << " num bit: " << (unsigned)(log2(_dictionary.size()) + 1) << "\t" << _input[it];
								app.append(1, _input[it]);
								_dictionary.push_back(app);
								cout << "\t" << _dictionary.size() << "\t" << app << endl;
								it++;
								if (app.size()>_maxlength){
									_maxlength = app.size();
								}
								app.clear();
								k = -1;
							}
						}
					}
				}
				else {
					unsigned j = _input.size() - it;
					for (k = j; k >= 0; k--){
						app.clear();
						for (unsigned i = it; i < it + k - 1; i++){
							app.append(1, _input[i]);
						}
						if (trova_stringa(app, id) == true){
							bitwriter(id + 1, (unsigned)(log2(_dictionary.size()) + 1), _os);
							bitwriter(_input[it + app.size()], 8, _os);
							cout << id + 1 << " num bit: " << (unsigned)(log2(_dictionary.size()) + 1) << "\t" << _input[it + app.size()];
							app.append(1, _input[it + app.size()]);
							_dictionary.push_back(app);
							cout << "\t" << _dictionary.size() << "\t" << app << endl;
							it = it + app.size();
							if (app.size() > _maxlength){
								_maxlength = app.size();
							}
							app.clear();
							k = -1;
						}
						if (k == 0){
							app.clear();
							bitwriter(0, (unsigned)(log2(_dictionary.size()) + 1), _os);
							bitwriter(_input[it], 8, _os);
							cout << 0 << " num bit: " << (unsigned)(log2(_dictionary.size()) + 1) << "\t" << _input[it];
							app.append(1, _input[it]);
							_dictionary.push_back(app);
							cout << "\t" << _dictionary.size() << "\t" << app << endl;
							it++;
							if (app.size() > _maxlength){
								_maxlength = app.size();
							}
							app.clear();
							k = -1;
						}
					}
				}
			}
		}

		

		//Padding di zero
		bitwriter(0, 8, _os);

	}

	void decodifica(string nome_file){
		ifstream in(nome_file, ios::binary);
		char c;
		while (in.get(c)){


		}
	}



	~LZ78(){}
};



int main(int argc, char *argv[]){
	if (argc != 4){
		cout << "argomenti insufficienti" << endl;
		cout << "lz78encode <max bits> <input filename> <output filename>" << endl;
		return -1;
	}

	string nomefilein, nomefileout;
	unsigned maxbits;

	maxbits = atoi(argv[1]);
	if (maxbits<1 || maxbits>31){
		cout << "maxbits dev'essere compreso tra 1 e 31" << endl;
		return -1;
	}

	nomefilein = argv[2];
	ifstream is(nomefilein, ios::binary);
	is.unsetf(ios::skipws);
	if (!is) {
		cout << "il file di input non esiste" << endl;
		return -1;
	}

	nomefileout = argv[3];
	ofstream os(nomefileout, ios::binary);
	if (!os) {
		cout << "errore file di output" << endl;
		return -1;
	}

	LZ78 lz(is, os, maxbits);
	lz.codifica();

}