#include<iostream>
#include<fstream>
using namespace std;
int main(){
char ch;
fstream fin("Encryptme.txt", fstream::in);
ofstream fout("Inclear.txt", ofstream::out);
ofstream ready("ASCII.txt", ofstream::out);
int ascii;
int temp;
int shift;
int n;
while (fin >> noskipws >> ch) {
    ascii=0;
    shift=8;
    //cout << ch;
    //cast to int
    ascii=int(ch);
    //write to file
    fout<<ch;
    if(fin.eof())
        {ch='?';
        temp=int(ch);
        temp=temp<<shift;
        ascii+=ch;
        //cout << ch;
    }
    else{
        fin.get(ch);
        temp=int(ch);
        temp=temp<<shift;
        ascii+=temp;
        //cout << ch;
        }
    shift+=8;
    fout<<ch;
    if(fin.eof())
        {ch='.';
        temp=int(ch);
        temp=temp<<shift;
        ascii+=temp;
        //cout << ch;
    }
    else{
        fin.get(ch);
        temp=int(ch);
        temp=temp<<shift;
        ascii+=temp;
        //cout << ch;
    }
    fout<<ch;
    //encrypt
    //write to file
    ready<<ascii;
    ready<<"\n";
    //decrypt
    //read from file
    n=ascii%256;
    ch=char(n);
    cout<<ch;
    ascii=ascii/256;
    n=ascii%256;
    ch=char(n);
    cout<<ch;
    ascii=ascii/256;
    ch=char(ascii);
    cout<<ch;
    }
cout<<" OK, done";
}
