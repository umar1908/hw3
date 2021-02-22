#include <iostream>
#include <iomanip>
using namespace std;
#include <functional>
// #include <algorithm>
// #include <vector>
// #include <string>
// #include <cmath>
// #include <cstdlib>
// #include<utility>
// #include <exception>
// #include <bitset>
#include<chrono>
const double MAX_HASH=4294967295;
const unsigned int STR_LENGTH=10;


class Timer{ //класс таймаре
private:
  chrono::time_point<chrono::steady_clock> start_point;
  chrono::time_point<chrono::steady_clock> stop_point;
  chrono::steady_clock::duration result;
public:
  Timer ()=default;
  chrono::time_point<chrono::steady_clock> start () {
    start_point=chrono::steady_clock::now();
    return start_point;
  }
  chrono::time_point<chrono::steady_clock> stop () {
    stop_point=chrono::steady_clock::now();
    result=stop_point-start_point;
    return stop_point;
  }
  auto get() const{
    return chrono::duration_cast<chrono::milliseconds>(result).count();
  }
};








template<typename Type,typename Generator,unsigned long long int N>
class Hash_tester{
private:
  vector<Type> data;
  Generator generator;
  Timer timer;
  vector<size_t> hashes;
public:
  Hash_tester (): generator(Generator()), timer(Timer()){
    data.resize(N);
    hashes.resize(N);
    for (size_t i = 0; i < N; i++) {
      data.at(i)=(generator.gen());
    }
    cout<<"Tester is ready. Generated "<<N<<" random values\n\n\n";
  }

  void test(function<unsigned int(Type,unsigned int)> hasher){
    auto iter= hashes.begin();
    timer.start();
    for(const Type& elem: data){
      *iter=hasher(elem,STR_LENGTH);
      // cout<<*iter<<endl;
      iter++;
    }
    timer.stop();
    cout<<"Work time="<<timer.get()<<" ms"<<endl;
    sort(hashes.begin(),hashes.end());
    auto new_end =unique(hashes.begin(),hashes.end());
    int unique_size= new_end-hashes.begin();
    // cout<<unique_size;
    cout<<"Collision persent="<<100-(static_cast<double>(unique_size)/N)*100<<"%\n";
    int distance=0;
    vector<bool> sections(unique_size+1,false);
    double reference_distance=MAX_HASH/(unique_size+1);
    for(auto elem: hashes){
      int section_number=static_cast<int>( elem/reference_distance );
      sections.at(section_number)=true;
    }
    auto filled_sections=count(sections.begin(),sections.end(),true);
    cout<<"Coverage present="<<static_cast<double>(filled_sections)/(unique_size+1)*100<<"%\n";

  }

};



unsigned int RSHash(const char* str, unsigned int length=STR_LENGTH)
{
   unsigned int b    = 378551;
   unsigned int a    = 63689;
   unsigned int hash = 0;
   unsigned int i    = 0;

   for (i = 0; i < length; ++str, ++i)
   {
      hash = hash * a + (*str);
      a    = a * b;
   }

   return hash;
}

unsigned int JSHash(const char* str, unsigned int length=STR_LENGTH)
{
   unsigned int hash = 1315423911;
   unsigned int i    = 0;

   for (i = 0; i < length; ++str, ++i)
   {
      hash ^= ((hash << 5) + (*str) + (hash >> 2));
   }

   return hash;
}

unsigned int PJWHash(const char* str, unsigned int length=STR_LENGTH)
{
   const unsigned int BitsInUnsignedInt = (unsigned int)(sizeof(unsigned int) * 8);
   const unsigned int ThreeQuarters     = (unsigned int)((BitsInUnsignedInt  * 3) / 4);
   const unsigned int OneEighth         = (unsigned int)(BitsInUnsignedInt / 8);
   const unsigned int HighBits          =
                      (unsigned int)(0xFFFFFFFF) << (BitsInUnsignedInt - OneEighth);
   unsigned int hash = 0;
   unsigned int test = 0;
   unsigned int i    = 0;

   for (i = 0; i < length; ++str, ++i)
   {
      hash = (hash << OneEighth) + (*str);

      if ((test = hash & HighBits) != 0)
      {
         hash = (( hash ^ (test >> ThreeQuarters)) & (~HighBits));
      }
   }

   return hash;
}

unsigned int ELFHash(const char* str, unsigned int length=STR_LENGTH)
{
   unsigned int hash = 0;
   unsigned int x    = 0;
   unsigned int i    = 0;

   for (i = 0; i < length; ++str, ++i)
   {
      hash = (hash << 4) + (*str);

      if ((x = hash & 0xF0000000L) != 0)
      {
         hash ^= (x >> 24);
      }

      hash &= ~x;
   }

   return hash;
}

unsigned int BKDRHash(const char* str, unsigned int length=STR_LENGTH)
{
   unsigned int seed = 131; /* 31 131 1313 13131 131313 etc.. */
   unsigned int hash = 0;
   unsigned int i    = 0;

   for (i = 0; i < length; ++str, ++i)
   {
      hash = (hash * seed) + (*str);
   }

   return hash;
}

unsigned int SDBMHash(const char* str, unsigned int length=STR_LENGTH)
{
   unsigned int hash = 0;
   unsigned int i    = 0;

   for (i = 0; i < length; ++str, ++i)
   {
      hash = (*str) + (hash << 6) + (hash << 16) - hash;
   }

   return hash;
}


unsigned int DJBHash(const char* str, unsigned int length=STR_LENGTH)
{
   unsigned int hash = 5381;
   unsigned int i    = 0;

   for (i = 0; i < length; ++str, ++i)
   {
      hash = ((hash << 5) + hash) + (*str);
   }

   return hash;
}

unsigned int DEKHash(const char* str, unsigned int length=STR_LENGTH)
{
   unsigned int hash = length;
   unsigned int i    = 0;

   for (i = 0; i < length; ++str, ++i)
   {
      hash = ((hash << 5) ^ (hash >> 27)) ^ (*str);
   }

   return hash;
}

unsigned int APHash(const char* str, unsigned int length=STR_LENGTH)
{
   unsigned int hash = 0xAAAAAAAA;
   unsigned int i    = 0;

   for (i = 0; i < length; ++str, ++i)
   {
      hash ^= ((i & 1) == 0) ? (  (hash <<  7) ^ (*str) * (hash >> 3)) :
                               (~((hash << 11) + ((*str) ^ (hash >> 5))));
   }

   return hash;
}

template<int length>
class String_Gen{
private:
  string rand_string;
  vector<string*>  pointer_storage;
public:
  const char* gen(){
    string* rand_string=new string;
    pointer_storage.push_back(rand_string);
    for (size_t i = 0; i < length; i++) {
      int rand_num=rand()%26;
      char ch=rand_num+'a';
      *rand_string+=ch;
    }
    return rand_string->c_str();
  }
  ~String_Gen(){
    for(auto elem:pointer_storage){
      delete elem;
    }
    cout<<"freed!"<<endl;
  }

};


int main(){
  system("chcp 1251"); //для корректного отображения кирилицы
  cout<<endl;
  srand(time(0));

  Hash_tester<const char*,String_Gen<STR_LENGTH>,10000000> tester;
  cout<<"\nRSHAsh:\n";
  tester.test(RSHash);
  cout<<"\nJSHash:\n";
  tester.test(JSHash);
  cout<<"\nPJWHash:\n";
  tester.test(PJWHash);
  cout<<"\nELFHash:\n";
  tester.test(ELFHash);
  cout<<"\nBKDRHash:\n";
  tester.test(BKDRHash);
  cout<<"\nSDBMHash:\n";
  tester.test(SDBMHash);
  cout<<"\nDJBHash:\n";
  tester.test(DJBHash);
  cout<<"\nDEKHash:\n";
  tester.test(DEKHash);
  cout<<"\nAPHash:\n";
  tester.test(APHash);





}
