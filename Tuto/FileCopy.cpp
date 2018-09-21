//: C02:FileCopy.cpp
// Copie un fichier dans un autre, une ligne à la fois
#include <string>
#include <fstream>
using namespace std;

int main() {
  ifstream in("FileCopy.cpp"); // Ouvre en lecture
  ofstream out("Scopy2.cpp"); // Ouvre en écriture
  string s;
  while(getline(in, s)) // Ecarte le caractère nouvelle ligne...
    out << s << "\n"; // ... on doit donc l'ajouter
} ///:~
