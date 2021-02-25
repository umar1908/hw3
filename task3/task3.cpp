#include <iostream>
#include <iomanip>
using namespace std;
#include <functional>
// #include <algorithm>
#include <vector>
// #include <string>
// #include <cmath>
// #include <cstdlib>
// #include<utility>
// #include <exception>
// #include <bitset>
#include<random>
#include <chrono>
#include <boost/functional/hash.hpp>
#include <map>

const double MAX_HASH=4294967295;


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


template<typename Type,typename Generator>
class Hash_tester{
private:
  unsigned long long int N;
  vector<Type> data;
  Generator generator;
  Timer timer;
  vector<size_t> hashes;
public:
  Hash_tester (unsigned long long int _N): generator(Generator()), timer(Timer()){
    N=_N;
    data.resize(N);
    hashes.resize(N);
    for (size_t i = 0; i < N; i++) {
      data.at(i)=(generator.gen());
    }
    // cout<<"Tester is ready. Generated "<<N<<" random values\n\n\n";
  }

  int test(function<unsigned int(Type)> hasher,bool info=false){
    auto iter= hashes.begin();
    timer.start();
    for(const Type& elem: data){
      *iter=hasher(elem);
      // cout<<*iter<<endl;
      iter++;
    }
    timer.stop();
    // cout<<"Work time="<<timer.get()<<" ms"<<endl;
    sort(hashes.begin(),hashes.end());
    auto new_end =unique(hashes.begin(),hashes.end());
    int unique_size= new_end-hashes.begin();
    // cout<<unique_size;
    // cout<<"Collision persent="<<100-(static_cast<double>(unique_size)/N)*100<<"%\n";
    int distance=0;
    vector<bool> sections(unique_size+1,false);
    double reference_distance=MAX_HASH/(unique_size+1);
    for (auto i=hashes.begin()  ; i != new_end; i++) {
      auto elem=*i;
      int section_number=static_cast<int>( elem/reference_distance );
      sections.at(section_number)=true;
    }
    auto filled_sections=count(sections.begin(),sections.end(),true);
    // cout<<"Coverage present="<<static_cast<double>(filled_sections)/(unique_size+1)*100<<"%\n";
    if (info){
       cout<<"Work time="<<timer.get()<<" ms"<<endl;
       cout<<"Collision persent="<<100-(static_cast<double>(unique_size)/N)*100<<"%\n";
       cout<<"Coverage present="<<static_cast<double>(filled_sections)/(unique_size+1)*100<<"%\n";

    }
    return N-unique_size;
  }

};



struct My_struct{
  int a;
  double b;
  char c;
  void print(){
    cout<<a<<" "<<b<<" "<<c<<endl;
  }


  friend std::size_t hash_value(const My_struct& p);
};

std::size_t hash_value(const My_struct& p){
  std::size_t seed = 0;
  boost::hash_combine(seed, p.a);
  boost::hash_combine(seed, p.b);
  boost::hash_combine(seed, p.c);
  return seed;
}

class My_struct_Gen{
private:
  std::random_device rd;
  std::mt19937 _mt;
  std::default_random_engine generator;
  std::uniform_real_distribution<double> real_distribution;
  std::uniform_int_distribution<int> int_distribution;

public:
    My_struct_Gen():real_distribution(uniform_real_distribution<double> (0,99999)),
                int_distribution(uniform_int_distribution<int> (0,10000000))  {
      srand(static_cast<unsigned int>(time(0)));
      _mt=std::mt19937(rand());
  }
  My_struct gen(){

    My_struct new_struct;
    new_struct.a=int_distribution(_mt);
    new_struct.b=real_distribution(_mt);
    new_struct.c=rand()%255;
    return new_struct;
  }

};



int main(){
  system("chcp 1251"); //для корректного отображения кирилицы
  cout<<endl;
map<int,int> results;
int colls;
int N;

cout<<"Проведение тестов с колличеством элементов: (максимальное значение 1000000. Запаситесь терпением)\n";
for (size_t i = 0; i < 1000000; i+=10000) {
  cout<<i<<endl;
  Hash_tester<My_struct,My_struct_Gen> tester(i);
  colls=tester.test(hash_value);
  results.insert(pair<int,int>(i,colls));
}
//эти данные будут использоваться для построения графика.
cout<<"Количесово коллизий для каждого теста\n";
for(auto i=results.begin(); i!=results.end();i++){
cout<<i->second<<endl;
}
  //данные о равномерности этой хэш-функции
  Hash_tester<My_struct,My_struct_Gen> tester2(100000);
  tester2.test(hash_value,true);




}
