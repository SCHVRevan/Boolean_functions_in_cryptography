#include <iostream>
#include <cstring>
#include <fstream>
#include <time.h>
#include <cstdlib>
using namespace std;

class BoolF {
	unsigned int *f;
	int n = 0, nw = 0;
public:
	BoolF(int n = 4, int type = 0); // type - нулевая функция, единичная, случайная
	BoolF(const char *s); // конструктор по строке
	BoolF(const BoolF &); // конструктор копирования
	~BoolF();	// деструктор

	BoolF operator = (const BoolF &);
	bool operator == (const BoolF &);
	bool operator != (const BoolF &);

	int weight();	// вес
	int get_n();
	void Mobius();
	void ANF();
	int deg_calc();

	friend ostream& operator << (ostream &, const BoolF &);
	friend istream& operator >> (istream &, BoolF &);
};

// Конструктор по длине и флагу / по умолчанию
BoolF::BoolF(int n1, int type) {
	n = n1;
	nw = (((unsigned int)1 << n) + 31) >> 5;
	f = new unsigned int[nw];
	
	if (type == 0) {
		for (int i = 0; i < nw; i++) {
			f[i] = 0;
		}
	}

	else if (type == 1) {
		if (n >= 5) {
			for (int i = 0; i < nw; i++) {
				f[i] = -1;
			}
		}
		else {
			f[0] = (1 << (1 << n)) - 1;
		}
	}

	else if (type == 2) {
		if (n >= 5) {
			for (int i = 0; i < nw; i++) {
				f[i] = (unsigned int)(rand() - rand());
			}
		}
		else {
			f[0] = (unsigned int)(rand() - rand()) << (32 - (1 << n));
			f[0] >>= 32 - (1 << n);
		}
	}
	else {
		cout << "Incorrect parametter type!\n";
		exit(EXIT_FAILURE);
	}
}

// Конструктор по строке
BoolF::BoolF(const char *s) {
	int len = strlen(s);
	if ((len & (len-1)) != 0) {
		cout << "Uncorrect lenght of function!\n";
		exit(EXIT_FAILURE);
	}

	for (int i = 0; !((len >> i) & 1); i++) {
		n++;
	}
	
	nw = (((unsigned int)1 << n) + 31) >> 5;
	f = new unsigned int[nw];

	for (int i = 0; i < len; i++) {
		if (s[i] == '1') {
			f[i / 32] |= 1 << (i % 32);
		}
		else if (s[i] == '0') {
			f[i / 32] &= ~(1 << (i % 32));
		}
		else {
			cout << "Incorrect input!\n";
			exit(EXIT_FAILURE);
		}
	}
}

// Конструктор копирования
BoolF::BoolF(const BoolF &b) {
	if (this != &b) {
		n = b.n;
		nw = b.nw;
		f = new unsigned int[nw];
		for (int i = 0; i < nw; i++) {
			f[i] = b.f[i];
		}
	}
}

// Деструктор
BoolF::~BoolF() {
	if (f) delete[] f;
	f = NULL;
}

// Присвоение
BoolF BoolF::operator = (const BoolF &b) {
	if (this != &b) {
		delete[] f;
		n = b.n;
		nw = b.nw;
		f = new unsigned int[nw];
		for (int i = 0; i < nw; i++) {
			f[i] = b.f[i];
		}
	}
	return *this;
}

// Равенство
bool BoolF::operator == (const BoolF &b) {
	if (n != b.n) return false;
	if (nw != b.nw) return false;
	for (int i = 0; i < nw; i++) {
		if (f[i] != b.f[i]) return false;
	}
	return true;
}

// Неравенство
bool BoolF::operator != (const BoolF &b) {
	if (n != b.n) return true;
	if (nw != b.nw) return true;
	for (int i = 0; i < nw; i++) {
		if (f[i] != b.f[i]) return true;
	}
	return false;
}

// Подсчёт веса
int BoolF::weight() {
	int wght = 0;
	unsigned int tmp;
	for (int i = 0; i < nw; i++) {
		tmp = f[i];
		while (tmp != 0) {
			tmp &= tmp - 1;
			wght++;
		}
	}
	return wght;
}

int BoolF::get_n() {
	return n;
}

istream &operator >> (istream &in, BoolF &b) {
	char *buff = new char[130];
	in.getline(buff, 130);
	b = BoolF(buff);
	delete[] buff;
	return in;
}

ostream &operator << (ostream &out, const BoolF &b) {
	if (b.n >= 5) {
		for (int i = 0; i < b.nw; i++) {
			for (int m = 0; m < 32; m++) {
					out << (((1 << m) & b.f[i]) ? '1' : '0');
			}
		}		
	}
	else {
		for (int m = 0; m < (1 << b.n); m++) {
			out << (((1 << m) & b.f[0]) ? '1' : '0');
		}
	}
	out << '\n';
	return out;
}

void BoolF::Mobius () {
	unsigned int g;
	for (int i = 0; i < nw; i++) {
		g = f[i];
		g = ((g << 1) & 0xAAAAAAAA) ^ g; // 1010 * 8
		g = ((g << 2) & 0xCCCCCCCC) ^ g; // 1100 * 8
		g = ((g << 4) & 0xF0F0F0F0) ^ g; // 1111 0000 * 4
		g = ((g << 8) & 0xFF00FF00) ^ g; // 1111 1111 0000 0000 * 2
		g = ((g << 16) & 0xFFFF0000) ^ g; // 1111 * 4 0000 * 4
		f[i] = g;
	}
	if (n > 5) {
		for (int k = 1; k < nw; k <<= 1) {
			for (int j = 0; j < nw; j += k << 1) {
				for (int s = j; s < j + k; s++) {
					f[s+k] ^= f[s];
				}
			}
		}
	}
}

void BoolF::ANF() {
	bool except = true;
	int gr = min(32, (1 << n));
	int ind = 0;
	for (int i = 0; i < nw; i++) {
		for (int j = 0; j < gr; j++) {
			if (f[i] & (1 << j)) {
				ind = (i << 5) + j;
				if (!except) {
					cout << "+ ";
				}
				if (ind == 0) cout << "1 ";
				for (int k = 0; k < n; k++) {
					if (ind & (1 << k))
						cout << "x" << (k + 1) << ' ';
				}
				except = false;
			}
		}
	}
	if (except) cout << "0";
	cout << '\n';
}

int BoolF::deg_calc() {
	int deg, max_deg = 0, ind = 0, gr = min(32, (1 << n));
	for (int i = 0; i < nw; i++) {
		for (int j = 0; j < gr; j++) {
			if (f[i] & (1 << j)) {
				ind = (i << 5) + j;
				deg = 0;
				while (ind != 0) {
					ind &= ind - 1;
					deg++;
				}
				if (deg > max_deg) max_deg = deg;
			}
		}
	}
	return max_deg;
}

int weight_test () {
	//srand(time(NULL));
	BoolF a, b(2, 1), c(3, 1), d(4, 2), e(4, 2);
	cout << "a = " << a << "\nb = " << b << "\nc = " << c << "\nd = " << d << "\ne = " << e << "\n";

	for (int i = 0; i < 32; i++) {
		BoolF rnd(i, 2);
		cout << i << ": ";
		cout << (double)(rnd.weight()) / (unsigned int)(1 << rnd.get_n()) << "\n";
	}
	return 0;
}

int Mobius_test () {
	//srand(time(NULL));
	BoolF exmpl(12, 2);
	BoolF tmp(exmpl);

	cout << "exmpl: " << exmpl << "\n";
	tmp.Mobius();
	cout << "tmp:   " << tmp << "\n";
	tmp.Mobius();
	cout << "tmp:   " << tmp << "\n";
	if (tmp == exmpl) cout << "All's good";
	else cout << "Smth's wrong";
	
	return 0;
}

int main () {
	srand(time(NULL));
	BoolF exmpl(6, 1);  // 1001  1011  1101  0100
	// 0001000100011110000100010001111000010001000111101110111011100001
	//cin >> exmpl;
	BoolF tmp(exmpl);
	cout << "   exmpl: " << exmpl;
	tmp.Mobius();
	cout << "μ(exmpl): " << tmp;
	tmp.ANF();
	cout << "Max degree: " << tmp.deg_calc() << '\n';
	return 0;
}
