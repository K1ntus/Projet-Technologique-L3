//: C02:HelloStrings.cpp
// Les bases de la classe string du Standard C++
#include <string>
#include <iostream>
using namespace std;

int main() {
  string s1, s2; // Chaînes de caractères vides
  string s3 = "Bonjour, Monde !"; // Initialisation
  string s4("J'ai"); // Egalement une initialisation
  s2 = "ans aujourd'hui"; // Affectation à une chaîne de caractères
  s1 = s3 + " " + s4; // Combinaison de chaînes de caractères
  s1 += " 8 "; // Ajout à une chaîne de caractères
  cout << s1 + s2 + " !" << endl;
} ///:~
