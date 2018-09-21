//: C03:Bitwise.cpp
//{L} printBinary
// Démonstration de la manipulation de bit
#include "printBinary.h"
#include <iostream>
using namespace std;

// Une macro pour éviter de la frappe
#define PR(STR, EXPR) \
  cout << STR; printBinary(EXPR); cout << endl;

int main() {
  unsigned int getval;
  unsigned char a, b;
  cout << "Entrer un nombre compris entre 0 et 255: ";
  cin >> getval; a = getval;
  PR("a in binary: ", a);
  cout << "Entrer un nombre compris entre 0 et 255: ";
  cin >> getval; b = getval;
  PR("b en binaire: ", b);
  PR("a | b = ", a | b);
  PR("a & b = ", a & b);
  PR("a ^ b = ", a ^ b);
  PR("~a = ", ~a);
  PR("~b = ", ~b);
  // Une configuration binaire intéressante:
  unsigned char c = 0x5A;
  PR("c en binaire: ", c);
  a |= c;
  PR("a |= c; a = ", a);
  b &= c;
  PR("b &= c; b = ", b);
  b ^= a;
  PR("b ^= a; b = ", b);
} ///:~
