//: C03:AutoIncrement.cpp
// montre l'utilisation des operateurs d'auto-incrémentation
// et auto-décrementation .
#include <iostream>
using namespace std;

int main() {
  int i = 0;
  int j = 0;
  cout << ++i << endl; // Pre-incrementation
  cout << j++ << endl; // Post-incrementation
  cout << --i << endl; // Pre-décrementation
  cout << j-- << endl; // Post décrementation
} ///:~
