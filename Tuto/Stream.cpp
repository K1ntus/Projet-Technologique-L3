//: C02:Stream2.cpp
// Plus de fonctionnalités des flux
#include <iostream>
using namespace std;

int main() {
  // Spécifier des formats avec des manipulateurs :
  cout << "un nombre en notation décimale : "
       << dec << 15 << endl;
  cout << "en octale : " << oct << 15 << endl;
  cout << "en hexadécimale : " << hex << 15 << endl;
  cout << "un nombre à virgule flottante : "
       << 3.14159 << endl;
  cout << "un caractère non imprimable (échap) : "
       << char(27) << endl;
} ///:~
