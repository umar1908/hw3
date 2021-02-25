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
};

using Phonebook = boost::multi_index_container<Phone_Node,
        indexed_by<
                    ordered_unique<tag< Phone_Node::OrderedBySurname>, member<Phone_Node, string, &Phone_Node::surname> >,
                    hashed_non_unique<tag< Phone_Node::HashedByNumber>, member<Phone_Node, long long int, &Phone_Node::number> >,
                    random_access<>
                  >
        >;



int main(){
  system("chcp 1251"); //для корректного отображения кирилицы
  cout<<endl;

  Phonebook book;
  book.insert({"Umar","Makhsudov",79853635441});
  book.insert({"Vasya","Pupkin",79998101908});
  book.insert({"Dmitriy","Petrov",73454565677});
  book.insert({"Andrey","Sorokin",78315010101});

  auto Ordered_Surname_index=book.get<OrderedBySurname>();
  auto Hashed_Surname_index=book.get<HashedBySurname>();
  auto Hashed_Number_index=book.get<HashedByNumber>();
  auto Random_index=bool.get<RandomAccess>();




}
