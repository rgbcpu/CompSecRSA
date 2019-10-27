#include <iostream>
#include <gmp.h>
#include <stdio.h> 
#include <stdlib.h>
#include <time.h>

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
	//mpz_out_str(stdout, 10, tot); 
	//cout << "totient" << endl;
}

void findD(mpz_t d, mpz_t e, mpz_t tot) 
{ 
	int i = 2;
	mpz_t tst, zro;
	mpz_inits(tst, zro, NULL);
	mpz_add_ui(tst,tot,1);
	mpz_mod(tst, tst, e);
	if(mpz_cmp(tst, zro) == 0)	mpz_cdiv_q(d, tst, e);
	//mpz_out_str(stdout, 10, d);
	do{
		i++;
		if(i == 4)cout << "hellowrold";
		cout << i << endl;
		mpz_mul_si(tst, tot, i);
		mpz_add_ui(tst,tst,1);
		mpz_mod(tst, tst, e);
		
		if(mpz_cmp(tst, zro) == 0)	mpz_cdiv_q(d, tst, e);
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
	makeprime(e, l, 10, 12);
	mpz_mod(tmp, e, n);
	while(mpz_cmp_ui(tmp, 0) == 0){
		makeprime(e, l, 10, 12);
		mpz_mod(tmp, e, n);
	}
	mpz_clear(tmp);
}

int main(){
	//declare and init p, q, floor, n, totient, e, d
	mpz_t p, q, l ,n, tot, e, d, M, C;
	mpz_inits(p, q, l, n, tot, e, d, M, C, NULL);
	
	//generate p and q
	makeprime(p, l, 14, 17);
	makeprime(q, l, 14, 17);
    //find n
    mpz_mul(n,p,q);
    mpz_clear(l);
    findTotient(p, q, tot);
    findE(n, e, l);
    findD(d, e, tot);
    //mpz_out_str(stdout, 10, p); //print M
    //cout << " q:" << endl;
    //mpz_out_str(stdout, 10, q); //print M
    //cout << " e:" << endl;
    //mpz_out_str(stdout, 10, e); //print M
    //cout << " d:" << endl;
    //mpz_out_str(stdout, 10, d); //print M
    //cout << " n:" << endl;
    //mpz_out_str(stdout, 10, n); //print M
    //mpz_ui_pow_ui(M, 2, 5);
    //mpz_out_str(stdout, 10, M); //print M
	//encrypt(n, M, e, C);
	//decrypt(n, C, d, e, M);
	//mpz_out_str(stdout, 10, M); //print M
    //mpz_out_str(stdout, 10, n); //test if prime fn worked
	return 0;
}
