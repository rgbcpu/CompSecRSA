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

//noted stoopids
/*
 AFAIK there's not an accurate size function for mpz_t arrays, 
 so it seems like best practice to keep a counter of the amount of
 values that are filled in the array
 */


using namespace std;
void makeprime (mpz_t rand, mpz_t l, unsigned long floor, unsigned long ceiling)
{
    // *********************** VARIABLE DECLARATION *********************** //
    unsigned long seed; //floor, ceiling;
    seed = std::rand();
   // floor = 10;   //larger int than 100 digi 2^329
    //ceiling = 18; //larger int than 100 digi 2^342
    
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

void findD(mpz_t d, mpz_t e, mpz_t tot) 
{ 
	long i = 1;
	mpz_t tst, zro;
	mpz_inits(tst, zro, NULL);
	mpz_add_ui(tst,tot,1);
	mpz_mod(tst, tst, e);
	if(mpz_cmp(tst, zro) == 0)	mpz_cdiv_q(d, tst, e);
	do{
		i++;
		mpz_mul_si(tst, tot, i);
		mpz_add_ui(tst,tst,1);
		mpz_cdiv_q(d, tst, e);
		mpz_mod(tst, tst, e);
			}while(mpz_cmp(tst, zro) != 0);
	}
	


void encrypt(mpz_t n, mpz_t M, mpz_t e, mpz_t C){
	//C = m ^ e(mod n)
	mpz_inits(C, NULL);
	mpz_powm(C,M,e, n);
	//mpz_mod(C, C, n);
}

void decrypt(mpz_t n, mpz_t C, mpz_t d, mpz_t e, mpz_t M){
	//M=C ^ d(mod n)
	mpz_inits(M, NULL);
	mpz_powm(M,C,d,n);
	//mpz_mod(M,M,n);
}

void findE(mpz_t n, mpz_t e, mpz_t l){
	mpz_t tmp;
	mpz_inits(tmp, NULL);
	makeprime(e, l, 6, 14);
	mpz_mod(tmp, e, n);
	while(mpz_cmp_ui(tmp, 0) == 0){
		makeprime(e, l, 6, 14);
		mpz_mod(tmp, e, n);
	}
	mpz_clear(tmp);
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
	int count = 0, loc = 0, numb = 0;
	cout << " File to encrypt: ";
	cin >> filename;

	inFile.open(filename);
	if (!inFile){
		cerr << "Unable to open file " << filename;
		exit(1);
	}
	while (inFile >> x) {
		y = int(x) - 33;
		if (count == 0){
			numb = y * pow(93, 2);
			count += 1;
		}
		else if (count == 1){
			numb += y * 93;
			count += 1;
		}
		else if (count == 2){
			numb += y;
			count = 0;
			tri.push_back(numb);
			loc += 1;
		}
		else{
			cout << "something went wrong" << endl;
		}
	}
	if (count != 0){
		tri.push_back(numb);
	}
	inFile.close();
	outFile.open(filename + ".enc");
	for(int k = 0; k < tri.size(); k++){
		mpz_set_ui(temp, tri[k]);
		mpz_powm(M, temp, e, n);
		for(int i = 3; i >= 0; i -= 1){
			mpz_tdiv_q_ui(temp, M, (int)pow(93,i));//t = quad[l] / pow(93, i);
			mpz_mod_ui(temp,temp,93);//t = (t % 93) + 33;
			mpz_add_ui(temp,temp,33);
			//gmp_fprintf(outFile, "%Zd\n", temp);
			
			outFile << (char)mpz_get_si(temp);
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
	vector<int> dec;
	long y;
	char x;
	inFile.open("testread.enc");
	int count = 0, numb = 0;
	while(inFile >> x){
		y = int(x) - 33;
		if (count == 0){
			mpz_set_ui(temp, y);
			mpz_mul_si(temp, temp,(int)pow(93,3));//numb = x * pow(93,3);
			count += 1;
		}
		else if (count == 1){
			mpz_set_ui(temp2, y);
			mpz_mul_si(temp2, temp2,(int)pow(93,3));
			mpz_add(temp,temp, temp2);//numb += x * pow(93,2);
			count += 1;
		}
		else if (count == 2){
			mpz_set_ui(temp2, y);
			mpz_mul_si(temp2, temp2, 93);
			mpz_add(temp,temp,temp2);//numb += x * 93;
			count += 1;
		}
		else if (count == 3){
			mpz_add_ui(temp,temp,y);
			count = 0;
			/*mpz_powm(temp, temp, d, n);
			mpz_tdiv_q_ui(C, temp, (int)pow(93,2));//t = quad[l] / pow(93, i);
			mpz_out_str(stdout, 10, C);
			mpz_add_ui(temp,temp,33);
			//mpz_out_str(stdout, 10, C);//outFile << (char)mpz_get_ui(C);
			mpz_tdiv_q_ui(C, temp, 93);
			mpz_mod_ui(C,temp,93);//t = (t % 93) + 33;
			mpz_add_ui(C,temp,33);
			//mpz_out_str(stdout, 10, C);//outFile << (char)mpz_get_ui(C);
			mpz_clear(C);
			mpz_mod_ui(C,temp,93);
			mpz_add_ui(C,temp,33);
			//mpz_out_str(stdout, 10, C);
			//outFile << (char)mpz_get_ui(C);
			*/
		}
	}
	//for(int i = 0; i < dec.size(); i++) cout << dec[i] << " ";
	//inFile.close();
	/*outFile.open(filename + ".dec");
	for(int k = 0; k < dec.size(); k++){
		mpz_set_ui(temp, dec[k]);
		mpz_powm(temp, temp, d, n);
		mpz_tdiv_q_ui(C, temp, (int)pow(93,2));//t = quad[l] / pow(93, i);
		mpz_add_ui(temp,temp,33);
		outFile << (char)mpz_get_ui(C);
		mpz_tdiv_q_ui(C, temp, 93);
		mpz_mod_ui(C,temp,93);//t = (t % 93) + 33;
		mpz_add_ui(C,temp,33);
		outFile << (char)mpz_get_ui(C);
		mpz_clear(C);
		mpz_mod_ui(C,temp,93);
		mpz_add_ui(C,temp,33);
		outFile << (char)mpz_get_ui(C);
		mpz_clear(C);
		mpz_clear(temp);
	}
		outFile.close();*/
	//outFile.close();
	inFile.close();
}

int main(){
	mpz_t p, q, l ,n, tot, e, d, M, C, test;
	mpz_inits(p, q, l, n, tot, e, d, M, C, test, NULL);
	
	//  //cout << " n:" << endl;  //mpz_out_str(stdout, 10, n); //print M
	int option;
	cout << "1. Encrypt " << endl << "2. Decrypt" << endl;
	cout << "Choose 1 to encrypt 2 to decrypt: ";
	cin >> option;
	
	if ( option == 1){
		//do the encrypt thing
		makeprime(p, l, 20, 30);
		makeprime(q, l, 20, 20);
		//mpz_set_ui(p, 941);
		//mpz_set_ui(q, 947);
		//mpz_set_ui(e, 7);
		//mpz_set_ui(d, 381103);
		mpz_mul(n,p,q);
		//mpz_clear(l);
		findTotient(p, q, tot);
		findE(n, e, l);
		findD(d, e, tot);
		encode(n, e, M);
		cout << endl << endl << endl << "N: ";
		mpz_out_str(stdout, 10, n);
		cout << endl << endl << endl << "D: ";
		mpz_out_str(stdout, 10, d);
		//decode(C, n, d);
		//encrypt(n, M, e, C);
		//cout << endl;
		//cout << endl;
		//mpz_out_str(stdout, 10, M);
		//cout << endl;
		//mpz_clear(M);*/
		
	}
	else{
		cout << "Enter required variables to decrypt message" << endl;
		cout << "n :"; cin >> C;
		decrypt(n, C, d, e, M);
		cout << endl;
		mpz_out_str(stdout, 10, M);
		
		//do the decrypt thing
	}
    
	return 0;
}
