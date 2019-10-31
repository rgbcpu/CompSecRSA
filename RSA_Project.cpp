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
#include <chrono> 


//aid in digraph calculation
using namespace std;
using namespace std::chrono; 
vector<double>timez;
string line = "";
vector<int> quad;
vector<int> modpush;
//vector<int> mod2;
int array_size_lg = 40;
mpz_t n, e, d, p, q, tot, l, C;
mpz_t pow2g[40];
mpz_t quoti[40];
mpz_t modtwo[40];

void makeprime (mpz_t rand, mpz_t l, unsigned long floor, unsigned long ceiling)
{
    unsigned long seed; //floor, ceiling;
    seed = std::rand();    
    // calculate the random number floor
    mpz_ui_pow_ui(l, 2, floor);
    // initilze the state object for the random generator functions
    gmp_randstate_t rstate;
    // initialize state for a Mersenne Twister algorithm.
    gmp_randinit_default(rstate);
    gmp_randseed_ui(rstate, seed);
    // find a uniformly distributed random number in the range 0 to n-1
    do {
        mpz_urandomb(rand, rstate, ceiling);
        // add the random number to the low number, which will make sure the random number is between the low and high ranges
        mpz_add(rand, rand, l);
    } while (!(mpz_probab_prime_p(rand, 25)));        
    // empty the memory location for the random generator state
    gmp_randclear(rstate);
}

void findTotient(mpz_t p, mpz_t q, mpz_t tot){
	mpz_t qm, pm;
	mpz_inits(qm,pm,NULL);
	mpz_sub_ui(qm, q, 1);
	mpz_sub_ui(pm,p, 1);
	mpz_mul(tot, qm, pm);
}

//finds d using modinverse
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
//makes a prime and ensures that its not a factor of totient
	void findE(mpz_t tot, mpz_t e, mpz_t l, int keysize){
	mpz_t tmp;
	mpz_inits(tmp, NULL);
	makeprime(e, l, keysize, keysize+8);
	mpz_mod(tmp, e, tot);
	mpz_mod(tmp, e, tot);
	while(mpz_cmp_ui(tmp, 0) == 0){
		makeprime(e, l, keysize, keysize+8);
		mpz_mod(tmp, e, tot);
	}
}

//using pow2, it finds the instances of mod that are true,
void modAmnt(int type){
	mpz_t key;
	mpz_inits(key, NULL);
	if(type == 0)
	{
		mpz_set(key, e);
	}
	else
		mpz_set(key,d);
	
	mpz_set_ui(pow2g[0],1);
	mpz_set(quoti[0], key);
	modpush.push_back(1);
	int i = 1;
	do{
		mpz_mul_ui(pow2g[i],pow2g[i-1],2);
		mpz_tdiv_q_ui(quoti[i], quoti[i-1], 2);
		modpush.push_back(mpz_get_ui(quoti[i]) % 2);
		i++;
	}while(mpz_get_ui(quoti[i-1]) != 1);
}

//takes text that was read in from a file, then encrypts it and stores in "encoded"
void makecipher(vector<int> pass){
	mpz_t pre [modpush.size()];
	mpz_t temp;
	
	mpz_inits(temp, NULL);
	ofstream outFile;
	for (int j = 0; j < modpush.size(); j++) {
	 mpz_init2(pre[j], 1024);
	 mpz_set_ui(pre[j], 1);
	}
	outFile.open("encoded");
	for(int z = 0; z < pass.size(); z++){
			mpz_set_ui(pre[0], pass[z]);
	for(int b = 0; b < modpush.size()-1; b++)
	{
		mpz_mul(pre[b+1], pre[b], pre[b]);
		mpz_mod(pre[b+1], pre[b+1], n);
	}
	for(int i = 1; i < modpush.size(); i++){
		if(modpush[i] == 1){
			mpz_mul(pre[i],pre[i],pre[i-1]);
			mpz_mod(pre[i],pre[i], n);
		}
		else if(modpush[i] == 0){
			mpz_set(pre[i], pre[i-1]);
		}

	}

	for(int k = 3; k >=0; k--){
		mpz_tdiv_q_ui(temp, pre[modpush.size()-1], (int)pow(93,k));
		mpz_mod_ui(temp,temp, 93);
		mpz_add_ui(temp,temp, 33);
		outFile << (char)mpz_get_ui(temp);
		
	}}
	outFile.close();
}
//reads in the ciphertext, then attempts to decrypt. not fully functional, only working with relatively small numbers, we think due to the 3 in 4 out requirement.
void decipher(){
	mpz_t temp, temp2;
	mpz_inits(temp, temp2, NULL);
	ifstream inFile;
	ofstream outFile;
	vector<unsigned long long> dec;
	long y;
	char x;
	int count = 0;
	long long numb = 0, temp3 = 0;
	inFile.open("encoded");
	outFile.open("decoded");
	

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

//calls functions, gives user choice of gen primes, encrypt, decrypt
int main(){
	int primes, esize;
	for (int j = 0; j < array_size_lg; j++) {
		mpz_init2(pow2g[j], 1024);
		mpz_init2(quoti[j], 1024);
		mpz_init2(modtwo[j], 1024);
	}
	mpz_inits(n, e, d, p, q, tot, l, NULL);
int choice = 3, choice2 = 3;
cout << "Do you want to time prime generation, or attempt encryption? " << endl << "1. for encrypt, or decrypt and 2. for prime generation ";
cin >> choice;
if(choice == 1){
	choice =0;
	cout << "Are you trying to encrypt or decrypt? 1 encrypt 2 decrypt ";
	cin >> choice2;
	cout << endl;
if(choice2 == 1){
	ifstream inFile;
	long y;
	vector<int> teest;
	char x;
	inFile.open("testread.enc");
	int count = 0, numb = 0;
	//readinfromafile
	while(inFile >> x){
		y = int(x) - 33;
		if (count == 0){
			numb = y * (int)pow(93,2);
			count += 1;
		}
		else if (count == 1){
			numb += y * 93;
			count += 1;
		}
		else if (count == 2){
			numb += y;
			count = 0;
			teest.push_back(numb);
			numb = 0;
		}	
	}
	cout << "Size of primes? 2^n ";
	cin >> primes;
	makeprime(p, l, (int)primes, (int)primes+10);
	makeprime(q, l, (int)primes, (int)primes+10);
	mpz_mul(n,p,q);
	findTotient(p, q, tot);
	cout << "Size of E? Start with 2^n ";
	cin >> esize;
	findE(tot, e, l, esize);
	findD(d, e, tot);
	cout << endl << endl << endl << "N: ";
	mpz_out_str(stdout, 10, n);
	cout << endl << endl << endl << "D: ";
	mpz_out_str(stdout, 10, d);
	cout << endl << endl << endl << "E: ";
	mpz_out_str(stdout, 10, e);
	auto start = high_resolution_clock::now();
	modAmnt(0);
	makecipher(teest);
	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<microseconds>(stop - start); 
	cout << endl << "It took " << duration.count() << " microseconds " << "to encrypt ";
	FILE *fptr = fopen("keyN", "w"); 
	gmp_fprintf(fptr, "%Zd\n%Zd\n", n, d);
	fclose(fptr); 
	
	}
	else{
ifstream keyFile;
keyFile.open("keyN");
keyFile >> n >> d;
cout << endl << endl << endl << "N: ";
mpz_out_str(stdout, 10, n);
cout << endl << endl << endl << "D: ";
mpz_out_str(stdout, 10, d);
auto start = high_resolution_clock::now();
decipher();
auto stop = high_resolution_clock::now();
auto duration = duration_cast<microseconds>(stop - start); 
cout << endl << "It took " << duration.count() << " microseconds " << "to decrypt ";
}
}
if(choice == 2)
{
	double total = 0.0;
	int bound, iterations, count = 1;
	cout << "Size prime? (Lower bound 2^N) ";
	cin >> bound;
	cout << endl << "How many to generate?: ";
	cin >> iterations;
	
	while(iterations > 0){
	auto start = high_resolution_clock::now(); 
	makeprime(p, l, bound, bound + 10);
	auto stop = high_resolution_clock::now(); 
	auto duration = duration_cast<milliseconds>(stop - start); 
	cout << endl << "It took " << duration.count() << " milliseconds " << "for prime " << count;
	timez.push_back((double)duration.count());
	count++;
	iterations--;
	}
	count--;
	for(int i = 0; i < timez.size(); i++){
		total += timez[i];
	}
	total = total / count;
	cout << endl << endl << "Average runtime for " << count << " number generations " <<  total << " millseconds";
}
return 0;
}

