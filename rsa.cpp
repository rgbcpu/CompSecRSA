#include <fstream>
#include <vector>
#include <iostream>
#include <gmp.h>
#include <stdio.h> 
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <algorithm>
#include <ctype.h>
#include <string>

using namespace std;

void makeprime (mpz_t rand, mpz_t l, unsigned long floor, unsigned long ceiling){
	unsigned long seed;
	seed = std::rand();

	mpz_ui_pow_ui(l, 2, floor);

	gmp_randstate_t rstate;
	gmp_randinit_default(rstate);
	gmp_randseed_ui(rstate, seed);

	do{
		mpz_urandomb(rand, rstate, ceiling);
		mpz_add(rand, rand, l);
	}
	while (!(mpz_probab_prime_p(rand, 25)));

	gmp_randclear(rstate);
}

void findTotient(mpz_t p, mpz_t q, mpz_t tot){
	mpz_t qm, pm;
	mpz_inits(qm, pm, NULL);
	mpz_sub_ui(qm, q, 1);
	mpz_sub_ui(pm, p, 1);
	mpz_mul(tot, qm, pm);
}

void findD(mpz_t d, mpz_t e, mpz_t tot){
	int i = 1;
	mpz_t tst, zro;
		mpz_inits(tst, zro, NULL);
	mpz_add_ui(tst, tot, 1);
	mpz_mod(tst, tst, e);
	if(mpz_cmp(tst, zro) == 0){
		mpz_add_ui(tst, tot, 1);
		mpz_cdiv_q(d, tst, e);
	}
	else{
		do{
			i++;
			mpz_mul_si(tst, tot, i);
			mpz_add_ui(tst, tst, 1);
			mpz_cdiv_q(d, tst, e);
			mpz_mod(tst, tst, e);
		}while(mpz_cmp(tst, zro) != 0);
	}
}

void findE(mpz_t tot, mpz_t e, mpz_t l){
	mpz_t tmp;
	mpz_inits(tmp, NULL);
	makeprime(e, l, 6, 14);
	mpz_mod(tmp, e, tot);
	mpz_mod(tmp, e, tot);
	while(mpz_cmp_ui(tmp, 0) == 0){
		makeprime(e, l, 6, 14);
		mpz_mod(tmp, e, tot);
	}
}

void encode(mpz_t n, mpz_t e, mpz_t M){
	mpz_t temp;
	mpz_inits(temp, NULL);
	ifstream inFile;
	ofstream outFile;
	string filename;
	vector<int> tri;
	char x;
	long y;
	int count = 0, numb = 0;
	cout << "File to encrypt: ";
	cin >> filename;

	inFile.open(filename);
	if (!inFile){
		cerr << "Unable to open file " << filename;
		exit(1);
	}
	while (inFile >> x){
		y = int(x) - 33;
		
		if (count == 0){
			numb = y * pow(93, 2);
			count += 1;
			//cout << "numb: " << numb << endl;
		}
		
		else if (count == 1){
			numb += y * 93;
			count += 1;
			//cout << "numb: " << numb << endl;
		}

		else if (count == 2){
			numb += y;
			count = 0;
			//cout << "numb: " << numb << endl;
			tri.push_back(numb);
		}
	}
	
	if (count != 0){
		//cout <<" just in case numb: " << numb << endl;
		tri.push_back(numb);
	}

	inFile.close();
	outFile.open(filename + ".enc");
	for(int i = 0; i < tri.size(); i++){
		mpz_set_ui(temp, tri[i]);
		mpz_powm(M, temp, e, n);
		for(int j = 3; j >= 0; j -= 1){
			mpz_out_str(stdout, 10, M);
			cout << endl;
			mpz_tdiv_q_ui(temp, M, (int)pow(93, j));
			//mpz_out_str(stdout, 10, temp);
			//cout << endl;
			mpz_mod_ui(temp, temp, 93);
			mpz_add_ui(temp, temp, 33);
			//mpz_out_str(stdout, 10, temp);
			//cout << endl;
			outFile << char(mpz_get_si(temp));
		}
	}
	outFile.close();
}

void decode(mpz_t C, mpz_t n, mpz_t d){
	mpz_t temp, temp2;
	mpz_inits(temp, temp2, NULL);
	ifstream inFile;
	ofstream outFile;
	string filename;
	vector<unsigned long long> dec;
	long y;
	char x;
	int count = 0;
	long long numb = 0, temp3 = 0;

	cout << "File to decrypt: ";
	cin >> filename;
	inFile.open(filename);
	outFile.open(filename + ".dec");

	if (!inFile){
	cerr << "unable to open file " << filename;
	exit(1);
	}

	while(inFile >> x){
		y = int(x) - 33;
		if (count == 0){
			mpz_set_ui(temp, y);
			mpz_mul_si(temp, temp, int(pow(93, 3)));
			count += 1;
		}
		
		else if (count == 1){
			mpz_set_ui(temp2, y);
			mpz_mul_si(temp2, temp2, int(pow(93, 2)));
			mpz_add(temp, temp, temp2);
			count += 1;
		}

		else if (count == 2){
			mpz_set_ui(temp2, y);
			mpz_mul_si(temp2, temp2, 93);
			mpz_add(temp, temp, temp2);
			count += 1;
		}

		else if (count == 3){
			mpz_add_ui(temp, temp, y);
			mpz_out_str(stdout, 10, temp);
			cout << endl;
			count = 0;
			mpz_powm(C, temp, d, n);
			numb = mpz_get_ui(C);
			outFile << char((numb / pow(93, 2)) + 33);
			temp3 = numb / 93;
			temp3 = (temp3 % 93) + 33;
			outFile << char(temp3);
			outFile << char((numb % 93) + 33);
		}
	}
	inFile.close();
	outFile.close();
}

int main(){
	mpz_t p, q, l, n, tot, e, d, M, C, test;
	mpz_inits(p, q, l, n, tot, e, d, M, C, test, NULL);

	makeprime(p, l, 13, 16);
	cout << "P: ";
	mpz_out_str(stdout, 10, p);
	cout << endl;
	makeprime(q, l, 13, 16);
	cout << "Q: ";
	mpz_out_str(stdout, 10, q);
	cout << endl;
	mpz_mul(n, p, q);
	findTotient(p, q, tot);
	cout << "tot: ";
	mpz_out_str(stdout, 10, tot);
	cout << endl;
	findE(tot, e, l);
	findD(d, e, tot);
	encode(n, e, M);
	cout << "Public key: ";
	mpz_out_str(stdout, 10, e);
	cout << ", ";
	mpz_out_str(stdout, 10, n);
	cout << endl;
	cout << "Private key: ";
	mpz_out_str(stdout, 10, d);
	cout <<", ";
	mpz_out_str(stdout, 10, n);
	cout << endl;
	decode(C, n, d);
	
	return 0;
}

