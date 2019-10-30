#include <fstream>
#include <iostream>
#include <string>
#include <stdio.h>
#include <ctype.h>
#include <vector>
#include <math.h>
#include <algorithm>

using namespace std;

int main(){
	unsigned long long int p, q, tot, e = 7, j = 1, t;
	unsigned long long int temp, ntot, d, n;
	ifstream inFile;
	ofstream outFile;
	string filename;
	char x;
	vector<unsigned long long int> tri;
	vector<unsigned long long int> dec;
	vector<unsigned long long int> quad;
	int count = 0, loc = 0, numb = 0;

	cout << " File to encrypt: ";
	cin >> filename;

	inFile.open(filename);
	if (!inFile){
		cerr << "Unable to open file " << filename;
		exit(1);
	}

	while (inFile >> x) {
		//cout << x;
		//cout << toupper(int(x)); //putchar to change to char
		x = int(x) - 33;
		if (count == 0){
			numb = x * pow(93, 2);
			//cout << "1st num: " << numb << endl;
			count += 1;
		}
		else if (count == 1){
			numb += x * 93;
			//cout << "2nd numb: " << numb << endl;
			count += 1;
		}
		else if (count == 2){
			numb += x;
			//cout << "tri: " << numb << endl;
			count = 0;
			tri.push_back(numb);
			//cout << "vector: " << tri[loc] << endl;
			loc += 1;
		}
		else{
			cout << "something went wrong" << endl;
		}

	}
	if (count != 0){
		tri.push_back(numb);
		//cout << "vector: " << tri[loc] << endl;
	}
	inFile.close();

	cout << "choose P: ";
	cin >> p;
	cout << "choose q: ";
	cin >> q;
	n = p * q;
	tot = (p - 1) * (q - 1);
	//e = 3457;
	//cout << __gcd(tot, e) << endl;
	while (__gcd(tot, e) != 1){
		e += 1;
	}
	ntot = tot;
	while(((ntot) + 1) % e != 0){
		ntot = tot;
		j += 1;
		ntot = ntot * j;
	}
	d = ((ntot) + 1) / e;
	cout << "d: " << d << endl;
	cout << "public key: " << e << ", " << n << endl;
	cout << "private key: " << d << ", " << n << endl;

	for(int k = 0; k < tri.size(); k += 1){
		temp = tri[k];
		//cout << "temp: " << temp << endl;
		for(int i = 0; i < e - 1; i++){
			temp = temp * tri[k];
			//cout << "temp" << i << ": " << temp << endl;
			temp = temp % n;
			//cout << "temp" << i << "mod: " << temp << endl;
		}
		//cout << "write to quad?" << endl;
		quad.push_back(temp);
		//quad[k] = pow(tri[k], e);
		//quad[k] = quad[k] % n;
		//cout << "quad" << k << ": " << quad[k] << endl;
	}
	//cout << "after quad" << endl;

	outFile.open(filename + ".enc");
	
	for(int l = 0; l < quad.size(); l +=1){
		for(int i = 3; i >= 0; i -= 1){
			//cout << "quad " << quad[l] << endl;
			t = quad[l] / pow(93, i);
			//cout << quad[l] << " / " << pow(93, i) << " = " << t << endl;
			//cout << "t: " << t << endl;
			t = (t % 93) + 33;
			//cout << "t again: " << t << endl;
			outFile << char(t);
		}
		//outFile << char(((quad[l] /(pow(26,3))) % 26) + 65);
		//outFile << char(((quad[l] /(pow(26,2))) % 26) + 65);
		//outFile << char(((quad[l] / 26) % 26) + 65);
		//outFile << char((quad[l] % 26) + 65);
	}
	outFile.close();

	inFile.open(filename + ".enc");
	count = 0;
	while(inFile >> x){
		//cout << "heres x: " << x << endl;
		x = int(x) - 33;
		if (count == 0){
			numb = x * pow(93,3);
			count += 1;
		}
		else if (count == 1){
			numb += x * pow(93,2);
			count += 1;
		}
		else if (count == 2){
			numb += x * 93;
			count += 1;
		}
		else if (count == 3){
			numb += x;
			count = 0;
			//cout << "decoded: " << numb << endl;
			dec.push_back(numb);
		}
	}

	inFile.close();
	for(int i = 0; i < dec.size(); i++){
		temp = dec[i];
		for(int j = 0; j < d - 1; j++){//decoding m = e^d mod n
			temp = temp * dec[i];
			temp = temp % n;
		}
		cout << char((temp / pow(93,2)) + 33);//1st char
		t = temp / 93;
		t = (t % 93) + 33;
		cout << char(t);
		cout << char((temp % 93) + 33);
	}
	cout << endl;
	return 0;
}
