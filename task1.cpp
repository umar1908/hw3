#include <iostream>
#include <iomanip>
using namespace std;
// #include <functional>
#include <algorithm>
// #include <vector>
// #include <string>
// #include <cmath>
// #include <cstdlib>
// #include<utility>
// #include <exception>
// #include <bitset>
#include<random>
#include<set>
#include <stack>
#include<chrono>
#include <list>



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






template <int N> //Класс тестировщк N-колличество данных для теста
class Tester{
private:
  stack<int> stack1;
  stack<int> stack2;
  stack<int> stack3;
  set<int> data1;
  vector<int> data2;
  array<int,N> data3;
  Timer timer1;
  Timer timer2;
  Timer timer3;
public:
  Tester(){
    srand(time(0));
    for (size_t i = 0; i < N; i++) {
      int rand_num=rand();
      stack1.push(rand_num); 
      stack2.push(rand_num);
      stack3.push(rand_num);
    }
  }

  void test(){
    timer1.start();
    for (size_t i = 0; i < N; i++) {
      data1.insert(stack1.top());
      stack1.pop();
    }
    timer1.stop();

    timer2.start();
    for (size_t i = 0; i < N; i++) {
      data2.push_back(stack2.top());
      stack2.pop();
    }
    sort(data2.begin(),data2.end());
    timer2.stop();

    timer3.start();
    for (size_t i = 0; i < N; i++) {
      data3.at(i)=(stack3.top());
      stack3.pop();
    }
    sort(data3.begin(),data3.end());
    timer3.stop();

    cout<<"N="<<N<<endl;
    cout<<"Set:"<<timer1.get()<<" ms"<<endl;
    cout<<"Vector:"<<timer2.get()<<" ms"<<endl;
    cout<<"Array:"<<timer3.get()<<" ms"<<endl<<endl;
  }

};


int main(){
  system("chcp 1251"); //для корректного отображения кирилицы
  cout<<endl;

    {Tester<10> test10;
    test10.test();}
    {Tester<100> test100;
    test100.test();}
    {Tester<1000> test1000;
    test1000.test();}
    {Tester<10000> test10000;
    test10000.test();}
    {Tester<100000> test100000;
    test100000.test();}
    {Tester<200000> test200000;
    test200000.test();}
    {Tester<300000> test300000;
    test300000.test();}
    {Tester<400000> test400000;
    test400000.test();}
    //создать array размером больше 400000 моя система не разрешает
    // до 400000 лидером по скорости является atd::array, на втором месте vector
    //и самым медленным является set

}
