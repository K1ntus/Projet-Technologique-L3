
#include <iostream>
using namespace std;

const float pi = 3.14159265359;

int main(void){
  int radius;
  printf("Entrez le rayon du cercle: ");
  cin >> radius;
  if (radius < 0){
    printf("Rayon invalide (négatif)");
    return EXIT_FAILURE;
  }
  printf("Rayon: %d\n",radius);
  float aire = radius*pi;
  printf("Aire du cercle: %.3f\n", aire);
  return EXIT_SUCCESS;
}
