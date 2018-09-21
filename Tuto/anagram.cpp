#include <iostream>
using namespace std;

int main (void){
  string text_to_check;
  cout << "Entrez votre chaîne de caractère à analyser: ";
  cin >> text_to_check;

  for (unsigned int i = 0; i < text_to_check.size()/2; i++)
    if (text_to_check[i] != text_to_check[text_to_check.size() - i])
      break;



  return EXIT_SUCCESS;
}
