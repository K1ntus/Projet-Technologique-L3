//: C02:Numconv.cpp
// Convertit une notation décimale en octale et hexadécimale
#include <iostream>
using namespace std;

int main() {
  int number;
  cout << "Entrez un nombre décimal : ";
  cin >> number;  //Entrée d'une valeur (cin -> Console INput)
  cout << "Valeur en octal = 0"
       << oct << number << endl;
  cout << "Valeur en hexadécimal = 0x"
       << hex << number << endl;
} ///:~
