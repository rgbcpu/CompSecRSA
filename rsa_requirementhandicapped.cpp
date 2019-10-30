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

string line = "";
vector<int> quad;
vector<int> modpush;
//vector<int> mod2;
int array_size_lg = 40;
mpz_t n, e, d, p, q, tot, l;
mpz_t pow2g[40];
mpz_t quoti[40];
mpz_t modtwo[40];

void makeprime (mpz_t rand, mpz_t l, unsigned long floor, unsigned long ceiling)
{
    // *********************** VARIABLE DECLARATION *********************** //
    unsigned long seed; //floor, ceiling;
    seed = std::rand();    
    // calculate the random number floor
    mpz_ui_pow_ui(l, 2, floor);
    // initilze the state object for the random generator functions
    gmp_randstate_t rstate;
    // initialize state for a Mersenne Twister algorithm. This algorithm is fast and has good randomness properties.
    gmp_randinit_default(rstate);
    // create the generator seed for the random engine to reference 
    gmp_randseed_ui(rstate, seed);
    /*
    Function:
    int mpz_probab_prime_p (const mpz_t n, int reps)

    Determine whether n is prime. Return 2 if n is definitely prime, return 1 if n is probably prime (without being certain), 
    or return 0 if n is definitely composite.
    */
    do {
        // return a uniformly distributed random number in the range 0 to n-1, inclusive.
        mpz_urandomb(rand, rstate, ceiling);
        // add the random number to the low number, which will make sure the random number is between the low and high ranges
        mpz_add(rand, rand, l);
       // cout << "make prime" << endl;
    } while (!(mpz_probab_prime_p(rand, 25)));        
    // *********************** GARBAGE COLLECTION *********************** //
    // empty the memory location for the random generator state
    gmp_randclear(rstate);
    // clear the memory locations for the variables used to avoid leaks
    //mpz_clear(l);
	//mpz_clear(rand);
}
void findTotient(mpz_t p, mpz_t q, mpz_t tot){
	mpz_t qm, pm;
	mpz_inits(qm,pm,NULL);
	mpz_sub_ui(qm, q, 1);
	mpz_sub_ui(pm,p, 1);
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
	//mpz_set_ui(modtwo[0], 1);
	modpush.push_back(1);
	int i = 1;
	do{
		mpz_mul_ui(pow2g[i],pow2g[i-1],2);
		mpz_tdiv_q_ui(quoti[i], quoti[i-1], 2);
		modpush.push_back(mpz_get_ui(quoti[i]) % 2);
		//mpz_out_str(stdout, 10, quoti[i]);
		//mpz_set_ui(modtwo[i], 200);
		//cout << mpz_fdiv_r_ui(modtwo[i],quoti[i], 2);
		//mpz_mod_ui(modtwo[i],quoti[i], 2);
		i++;
	}while(mpz_get_ui(quoti[i-1]) != 1);
	for(int j = 0; j < modpush.size(); j++){
		//mpz_out_str(stdout, 10, modtwo[j]);
		cout << modpush[j] << " : ";
	}
}

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
		mpz_tdiv_q_ui(temp, pre[modpush.size()-1], (int)pow(26,k));
		mpz_mod_ui(temp,temp, 26);
		mpz_add_ui(temp,temp, 65);
		outFile << (char)mpz_get_ui(temp);
		
	}}
	//line += (char)mpz_get_ui(temp);
	//cout << "LINE: " <<  pass[0] << endl;
	outFile.close();
}
void decipher(){
	mpz_t temp, temp2;
	mpz_inits(temp, temp2, NULL);
	ifstream inFile;
	ofstream outFile;
	mpz_t dec[modpush.size()];
	for (int j = 0; j < modpush.size(); j++) {
		mpz_init2(dec[j], 1024);
	}
	char x;
	long y;
	int count = 0;
	inFile.open("encoded");
	while(inFile >> x){
		y = int(x) - 65;
		if (count == 0){
			mpz_set_ui(temp, y);
			mpz_mul_si(temp, temp,(int)pow(26,3));//numb = x * pow(93,3);
			count += 1;
		}
		else if (count == 1){
			mpz_set_ui(temp2, y);
			mpz_mul_si(temp2, temp2,(int)pow(26,2));
			mpz_add(temp,temp, temp2);//numb += x * pow(93,2);
			count += 1;
		}
		else if (count == 2){
			mpz_set_ui(temp2, y);
			mpz_mul_si(temp2, temp2, 26);
			mpz_add(temp,temp,temp2);//numb += x * 93;
			count += 1;
		}
		else if (count == 3){
			mpz_add_ui(temp,temp,y);
			count = 0;	
			quad.push_back(mpz_get_ui(temp));
		}
	}
	inFile.close();
	outFile.open("decrypted");
	for(int j = 0; j < quad.size(); j++){
	mpz_set_ui(dec[0], quad[j]);
	for(int i = 0; i < modpush.size()-1; i++)
	{
		mpz_mul(dec[i+1],dec[i],dec[i]);
		mpz_mod(dec[i+1], dec[i+1], n);
	}
	for(int p = 1; p < modpush.size(); p++){
		if(modpush[p] == 1){
			mpz_mul(dec[p],dec[p],dec[p-1]);
			mpz_mod(dec[p],dec[p], n);
		}
		else if(modpush[p] == 0){
			mpz_set(dec[p], dec[p-1]);
			}
		
	}
		for(int k = 2; k >=0; k--){
		mpz_tdiv_q_ui(temp, dec[modpush.size()-1], (int)pow(26,k));
		mpz_mod_ui(temp,temp, 26);
		mpz_add_ui(temp,temp, 65);
		outFile << (char)mpz_get_ui(temp);
		}
}
	outFile.close();
}



int main(){
	for (int j = 0; j < array_size_lg; j++) {
		mpz_init2(pow2g[j], 1024);
		mpz_init2(quoti[j], 1024);
		mpz_init2(modtwo[j], 1024);
	}
	mpz_inits(n, e, d, p, q, tot, l, NULL);
	//mpz_set_ui(n, 34571);
	//mpz_set_ui(e, 143);
	//mpz_set_ui(d, 20807);
int choice = 3;
cout << "Do you want to time prime generation, or attempt encryption? " << endl << "1. for encrypt, 2. for prime generation ";
cin >> choice;
if(choice == 1){
	cout << endl;
	ifstream inFile;
	long y;
	vector<int> teest;
	char x;
	inFile.open("testread.enc");
	int count = 0, numb = 0;
	//readinfromafile
	while(inFile >> x){
		y = int(x) - 65;
		if (count == 0){
			numb = y * (int)pow(26,2);
			count += 1;
		}
		else if (count == 1){
			numb += y * 26;
			count += 1;
		}
		else if (count == 2){
			numb += y;
			count = 0;
			teest.push_back(numb);
			numb = 0;
		}	
	}

makeprime(p, l, 12, 19);
makeprime(q, l, 12, 19);
mpz_mul(n,p,q);
findTotient(p, q, tot);
findE(tot, e, l);
findD(d, e, tot);
cout << endl << endl << endl << "N: ";
mpz_out_str(stdout, 10, n);
cout << endl << endl << endl << "D: ";
mpz_out_str(stdout, 10, d);
cout << endl << endl << endl << "E: ";
mpz_out_str(stdout, 10, e);
modAmnt(1);
//makecipher(teest);
decipher();
}
if(choice == 2)
{
	vector<double>timez;
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
	auto duration = duration_cast<microseconds>(stop - start); 
	cout << endl << "It took " << duration.count() << " microseconds " << "for prime " << count;
	timez.push_back((double)duration.count());
	//mpz_out_str(stdout, 10, p);
	count++;
	iterations--;
	}
	count--;
	for(int i = 0; i < timez.size(); i++){
		total += timez[i];
	}
	total = total / count;
	
	cout << endl << endl << "Average runtime for " << count << " number generations " <<  total << " microseconds";
	
	
	
	
}
return 0;
}

