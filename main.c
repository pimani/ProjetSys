#include <stdio.h>
#include <stdlib.h>

void fibo(size_t num);

int main () {
  size_t n;
  printf("Entrez une valeur : ");
  if (scanf("%zu", &n) == -1) {
    printf("Erreur : valeur innatendue !");
    return EXIT_FAILURE;
  }
  if (n < 50) {
    fibo(n);
  } else {
    printf("Attention, très grand nombre !");
  }
  return EXIT_SUCCESS;
}

void fibo(size_t num) {
  int a = 1, b = 0, temp, val = (int) num;
  while (num > 0){
    temp = a;
    a = a + b;
    b = temp;
    num--;
  }
  printf("résultat de %d : %d\n", val, b);
}
