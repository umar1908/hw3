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

const double MAX_HASH=4294967295; //максивальное значение size_t.boost использует
                                  //этот тип для стандартных хэш функций

//реализации этих функций будут в самом низу файла
int my_hash_double2(const double& key); //моя хэш-функция
int hash_with_bad_collision(const double& key); //пример функции с плохим показателем коллизий
int hash_with_bad_coverage(const double& key); //пример неравномерной функции
                                              //(с плохим покрытием пространства хэшей)


class Timer{ //класс таймера
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





/*Универсальный тестировщик хэш-функций. Этот класс с небольщими изменениями
используется во всех осталных задачах. Поэтому подробно прокомментирую его только тут.
Параметры шаблона:
Type- тип данных на которых будут тестироваться хэш-функции
Generator- класс генератор с единственным методом .gen() генерирующий случайный
объект типа Types
N - размер выборки для тестирования
*/

template<typename Type,typename Generator,unsigned long long int N>
class Hash_tester{
private:
  vector<Type> data;
  Generator generator;
  Timer timer;
  vector<size_t> hashes;
  ofstream out; // сгенерированные хэши для каждого теста буду записывать в файл для их дальнейшей
                //визуализации средствами Python (pandas, matplotlib)
public:
  Hash_tester (): generator(Generator()), timer(Timer()),out("common_data.txt"){
    out<<"[";
    data.resize(N);
    hashes.resize(N);
    for (size_t i = 0; i < N; i++) {
      data.at(i)=(generator.gen()); // заполняю вектор входных данных случайными занчениями
    }
    cout<<"Tester is ready. Generated "<<N<<" random values\n\n\n";
  }

  void test(function<int(Type)> hasher){ // функция тестирования. В качустве аргумента
                                        //принимает хэш функцию
    auto iter= hashes.begin();
    timer.start();  //замер времени работы
    for(const Type& elem: data){
      *iter=hasher(elem);
      // cout<<*iter<<endl;
      iter++;
    }
    timer.stop();
    out<<"["; //заполнение выходного файла хэшами
    for(auto elem:hashes){
      out<<elem<<",";
    }
    out<<"],";


    cout<<"Work time="<<timer.get()<<" ms"<<endl;

    sort(hashes.begin(),hashes.end()); //сортировка массива полученных хэшей
    auto new_end =unique(hashes.begin(),hashes.end()); // получение вектора уникальных хэшей
    int unique_size= new_end-hashes.begin(); // разница исходного размера и размера уникалльного = число коллизий
    // cout<<unique_size;
    cout<<"Collision persent="<<100-(static_cast<double>(unique_size)/N)*100<<"%\n";
    /* Теперь поподробнее про алгоритм вычисления равномерности (уровня покрытия).
    Идея такая. Допустим у нас есть 100 случайных чисел. Если генератор случайных чисел
    достаточно хороший, то эти 100 элементов должны быть равномерно распределены по множеству
    [min,max] где max/min- максимальное и минммальное возможные числа. Проверить равномерность
    распределения можно таким способом: условно разделить отвезок [min,max] на 100
    равных подотрезков, а дальше посчитать, во скольких подотрезках будет хотя бы один элемент
    из выбранных 100 чисел. При идеально равномерном распредедении каждый элмент будет находиться
    один в своем подотрезке. Если в каком-то подотрезке элмеентов больше одного, то значит
    что какие-то подотрезки остались без элементов т.к кол-во подотрезков и элементов равно.
    Отношение количества непустых подотрезков к их общему числу и будет показывать насколько
    полно(равномерно) покрывается множество всевозможных случайных чисел выбранными 100 числами.

    Такой подход я и применил для оценки равномерности хэш-функций. Если предположить
    что случайные элементы генерируются равномерно (вроде я использовал хорошие библиотечные генераторы)
    тогда равномерная хэш функция будет также равномено отображать эти числа на пространство хешей
    от минимального до максимального. Применяя метод разделения N подотрезков получим
    процент покрытия.

    ГРАФИКИ:
    Кроме того экспортировав полученные хэши в файл в питоне я построил графики плотностей
    вероятностей распредления хэшей. У стандартный функции boost этот график можно назвать
    почти эталонным- одинаковая вероятность на протядении всего диапазона хэшей.
    Моя функция- тоже достаточно равномерный резульат, но покрывается далеко не все множество
    возможных хэшей
    Хэш функции которые намеренно были сделаны плохими показывают плохой результат

    */

    //ниже идет описанный алогрити опредедения процента покрытия пространства хэшей
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

class Get_double{ //генератор double
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
  return my_hash_double2(key)%static_cast<int>(MAX_HASH/10); //диапазон значений умышленно сокращен в 10 раз
}

int hash_with_bad_collision(const double& key){
  return static_cast<int>(key)% static_cast<int>(MAX_HASH); //тупая функция возвращающая целую часть от double

}


int my_hash_double2(const double& key){
  unsigned long long int hash=0;
  bitset<sizeof(double)*8> bits;
  bitset<sizeof(int)*8> bits_part1;//битовое поле для первой половины double
  bitset<sizeof(int)*8> bits_part2; //битовое поле для второй половины double
  memcpy(&bits,&key,sizeof(key)); //побитовое копирование полученного значения в битовое
                                  // поле для свободной работы с битами

    for (size_t i = 0; i < 32; i++) {
      bits_part1[i]=bits[i]; //заполнение битового поля первой половиной double
      bits_part2[i]=bits[63-i]; //заполнение битового поля второй половиной double

    }

  hash=(bits_part1.to_ullong()>>3)+(bits_part2.to_ullong()<<3); // интерпритация двух полученных половинок
                                                                // как двух unsigned long int с небольшим битовым сдвигом
                                                                // для вненения асимметричности первой и второй половинке double
  return hash%static_cast<int>(MAX_HASH);

}
