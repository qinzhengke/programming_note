#include <iostream>
#include <fstream>
using namespace std;

#pragma pack(1)
typedef struct _A
{
    int index,time_stamp;
} A;

typedef struct _B
{
   float data1,data2,data3;
}B;
#pragma pack()


int write_file_01()
{
    const int N = 1;
    A a[N];
    B b[N];

    for(int i=0; i<N; i++)
    {
        a[i].index = i;
        a[i].time_stamp = i*2;
        b[i].data1 = i;
        b[i].data1 = i*i;
        b[i].data3 = i*i*i;
    }

    ofstream out_file;
    out_file.open("file_01",ios::binary);
    for(int i=0; i<N; i++)
    {
        out_file.write((char*)(&(a[i])), sizeof(A));
        out_file.write((char*)(&(b[i])), sizeof(B));
    }
    out_file.close();

    return 1;
}

int read_file_01()
{
    A aa;
    B bb;
    ifstream in_file;
    in_file.open("file_01", ios::binary);
    while(!in_file.eof())
    {
        in_file.read((char*)(&aa), sizeof(A));
        cout<<"aa.index="<<aa.index<<" aa.time_stamp="<<aa.time_stamp<<endl;
        cout<<"eofbit = "<<in_file.eof() << ", failbit = "<<in_file.fail()<<", badbit = "<<in_file.bad()<< endl;
        in_file.seekg(sizeof(B),ios::cur);
        cout<<"eofbit = "<<in_file.eof() << ", failbit = "<<in_file.fail()<<", badbit = "<<in_file.bad()<< endl;
    }
    in_file.close();
    return 1;
}

int read_file_01_correct()
{
    A aa;
    B bb;
    ifstream in_file;
    in_file.open("file_01", ios::binary);
    while(!in_file.eof() && !in_file.fail())
    {
        in_file.read((char*)(&aa), sizeof(A));
        cout<<"aa.index="<<aa.index<<" aa.time_stamp="<<aa.time_stamp<<endl;
        cout<<"eofbit = "<<in_file.eof() << ", failbit = "<<in_file.fail()<<", badbit = "<<in_file.bad()<< endl;
        in_file.seekg(sizeof(B),ios::cur);
        cout<<"eofbit = "<<in_file.eof() << ", failbit = "<<in_file.fail()<<", badbit = "<<in_file.bad()<< endl;
    }
    in_file.close();
    return 1;
}

inline bool check_eof(ifstream & file)
{
    char bit;
    bool s = false;
    file.read(&bit, sizeof(char));
    if(file.eof() || file.fail())
        s = true;
    file.seekg(-1, ios::cur);
    return s;
}

int read_file_01_perfect()
{
    A aa;
    ifstream in_file;
    in_file.open("file_01", ios::binary);
    while(!check_eof(in_file))
    {
        in_file.read((char*)(&aa), sizeof(A));
        cout<<"aa.index="<<aa.index<<" aa.time_stamp="<<aa.time_stamp<<endl;
        cout<<"e, f, b = "<<in_file.eof()<<", "<<in_file.fail()<<", "<<in_file.bad()<<endl;
        in_file.seekg(sizeof(B), ios::cur);
        cout<<"e, f, b = "<<in_file.eof()<<", "<<in_file.fail()<<", "<<in_file.bad()<<endl;
    }
    in_file.close();
    return 1;
}

void write_file_02()
{
    ofstream out_file;
    out_file.open("file_02", ios::binary);
    const int N = 10;
    for(int i=0; i<N; i++)
        out_file.write((char*)(&i), sizeof(char));
    out_file.close();
}

void read_file_02()
{
    ifstream in_file;
    char a;
    in_file.open("file_02", ios::binary);
    cout<<"init: "<<in_file.eof()<<" "<<in_file.fail()<<" "<<in_file.bad()<<endl;
    in_file.seekg(10, ios::cur);
    cout<<"after seeking: "<<in_file.eof()<<" "<<in_file.fail()<<" "<<in_file.bad()<<endl;
    in_file.read(&a, sizeof(char));
    cout<<"after reading: "<<in_file.eof()<<" "<<in_file.fail()<<" "<<in_file.bad()<<endl;
}

int main(int argc, char *argv[])
{
    write_file_01();
//    read_file_01_correct();
    read_file_01_perfect();

    //write_file_02();
    //read_file_02();

    return 0;
}
