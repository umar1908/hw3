#include <iostream>
#include <iomanip>
using namespace std;
// #include <functional>
// #include <algorithm>
// #include <vector>
// #include <string>
// #include <cmath>
// #include <cstdlib>
// #include<utility>
// #include <exception>
// #include <bitset>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/random_access_index.hpp>
#include <boost/geometry/core/tag.hpp>


using namespace boost::multi_index;


struct Phone_Node{
  string name;
  string surname;
  long long int number;
  struct OrderedBySurname{};
  struct HashedByNumber{};
  struct HashedBySurname{};
  struct RandomAccess{};
  void print()const {
    cout<<"*******************\n"<<"Name:"<<name<<endl<<"Surname:"<<surname<<"\nNumber:"<<number<<endl<<"********************\n\n";
  }
};

using Phonebook = boost::multi_index_container<Phone_Node,
        indexed_by<
                    ordered_unique<
                      tag<Phone_Node::OrderedBySurname>, member<Phone_Node, string, &Phone_Node::surname>
                      >,
                    hashed_non_unique<
                      tag<Phone_Node::HashedByNumber>, member<Phone_Node, long long int, &Phone_Node::number>
                      >,
                      hashed_non_unique<
                        tag<Phone_Node::HashedBySurname>, member<Phone_Node, string, &Phone_Node::surname>
                        >,
                    random_access<
                      tag<Phone_Node::RandomAccess>
                      >
                  >
        >;



int main(){
  system("chcp 1251"); //для корректного отображения кирилицы
  cout<<endl;

  Phonebook book;
  book.insert({"Umar","B_Makhsudov",79853635441});
  book.insert({"Vasya","A_Pupkin",79998101908});
  book.insert({"Dmitriy","D_Petrov",73454565677});
  book.insert({"Andrey","C_Sorokin",78315010101});

  const auto& Ordered_Surname_index=book.get<Phone_Node::OrderedBySurname>();
  const auto&  Hashed_Surname_index=book.get<Phone_Node::HashedBySurname>();
  const auto& Hashed_Number_index=book.get<Phone_Node::HashedByNumber>();
  const auto& Random_index=book.get<Phone_Node::RandomAccess>();

cout<<"Доступ по индексу в цикле"<<endl;
  for(auto elem: Random_index){ //вывод всех элементов по индексу
  elem.print();
}
cout<<"Произвольный индекс по индексу"<<endl;
Random_index[0].print(); //доступ к отдельным элементам по индексу
Random_index[1].print();
Random_index[2].print();


cout<<"Доступ по индексу сортированному по фамилии в цикле"<<endl;
for(auto elem: Ordered_Surname_index){
  elem.print();
}

cout<<"Доступ к элементу по хэшу"<<endl;
Hashed_Number_index.find(79853635441)->print();
Hashed_Surname_index.find("D_Petrov")->print();







}
