#include <iostream>
#include <iomanip>
#include <forward_list>
using namespace std;
// #include <boost>
#include <functional>
#include <algorithm>
#include <vector>
// #include <string>
// #include <cmath>
// #include <cstdlib>
// #include<utility>
// #include <exception>
// #include <bitset>
#include<random>
#include<typeinfo>
#include <utility>
#include <chrono>
#include <boost/functional/hash.hpp>
#include <fstream>
// #include <boost/multi_array.hpp>
// #include "Fraction.h"

const double MAX_HASH=4294967295;

int my_hash_double2(const double& key);
int hash_with_bad_collision(const double& key);
int hash_with_bad_coverage(const double& key);
// class Timer;

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
  ofstream out;
public:
  Hash_tester (): generator(Generator()), timer(Timer()),out("common_data.txt"){
    out<<"[";
    data.resize(N);
    hashes.resize(N);
    for (size_t i = 0; i < N; i++) {
      data.at(i)=(generator.gen());
    }
    cout<<"Tester is ready. Generated "<<N<<" random values\n\n\n";
  }

  void test(function<int(Type)> hasher){
    auto iter= hashes.begin();
    timer.start();
    for(const Type& elem: data){
      *iter=hasher(elem);
      // cout<<*iter<<endl;
      iter++;
    }
    timer.stop();
    out<<"[";
    for(auto elem:hashes){
      out<<elem<<",";
    }
    out<<"],";






    cout<<"Work time="<<timer.get()<<" ms"<<endl;
    sort(hashes.begin(),hashes.end());
    auto new_end =unique(hashes.begin(),hashes.end());
    int unique_size= new_end-hashes.begin();
    // cout<<unique_size;
    cout<<"Collision persent="<<100-(static_cast<double>(unique_size)/N)*100<<"%\n";
    int distance=0;
    vector<bool> sections(unique_size+1,false);
    double reference_distance=MAX_HASH/(unique_size+1);
    for (auto i=hashes.begin()  ; i != new_end; i++) {
      auto elem=*i;
      int section_number=static_cast<int>( elem/reference_distance );
      sections.at(section_number)=true;
    }
    auto filled_sections=count(sections.begin(),sections.end(),true);
    cout<<"Coverage present="<<static_cast<double>(filled_sections)/(unique_size+1)*100<<"%\n";

  }
  ~Hash_tester(){
    out<<"]";
  }

};

class Get_double{
private:
  std::random_device rd;
  std::mt19937 _mt;
  std::default_random_engine generator;
  std::uniform_real_distribution<double> _distribution;
public:
  Get_double():_distribution(uniform_real_distribution<double> (0,99999))  {
    srand(static_cast<unsigned int>(time(0)));
    _mt=std::mt19937(rand());
  };
  double gen(){
    return _distribution(_mt);
  }

};






int main () {

  boost::hash<double> boost_hash;
  Hash_tester <double, Get_double,1000>tester;
  cout<<"boost_hash:\n";
  tester.test(boost_hash);
  cout<<endl<<"hash_with_bad_coverage\n";
  tester.test(hash_with_bad_coverage);
  cout<<endl<<"hash_with_bad_collision\n";
  tester.test(hash_with_bad_collision);
  cout<<endl<<"my_hash_double2\n";
  tester.test(my_hash_double2);




}


int hash_with_bad_coverage(const double& key){
  return my_hash_double2(key)%static_cast<int>(MAX_HASH/10);
}

int hash_with_bad_collision(const double& key){
  return static_cast<int>(key)% static_cast<int>(MAX_HASH);
}

int my_hash_double2(const double& key){
  unsigned long long int hash=0;
  bitset<sizeof(double)*8> bits;
  bitset<sizeof(int)*8> bits_part1;
  bitset<sizeof(int)*8> bits_part2;
  memcpy(&bits,&key,sizeof(key));

    for (size_t i = 0; i < 32; i++) {
      bits_part1[i]=bits[i];
      bits_part2[i]=bits[63-i];

    }

  hash=(bits_part1.to_ullong()>>3)+(bits_part2.to_ullong()<<3);
  return hash%static_cast<int>(MAX_HASH);

}
