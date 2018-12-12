#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <mqueue.h>
#include <pthread.h>
#include <string.h>

#include "fifosm.h"
#include "lanceur.h"

#define TUBE_LENGHT 32
#define ARG_LENGHT 32
#define ARG_NUMBER 5
#define ENVIRONEMENT_LENGHT 20

// Structure de transmission de demande
struct argsc {
  char argv[ARG_NUMBER][ARG_LENGHT];
  char envp[ENVIRONEMENT_LENGHT];
  char tube_in[TUBE_LENGHT];
  char tube_out[TUBE_LENGHT];
};

#define NOM_FILE "/ma_file_a_moi_786432985365428"

info *info_p = NULL;

void * run(void * arg) {
  if (arg == NULL) {
    fprintf(stderr, "Unexpected argument value\n");
    exit(EXIT_FAILURE);
  }
  char* argv[] = {"/bin/ps", "-j", NULL};
  char* envp[] = {NULL};

  switch (fork()) {
  case -1:
    perror("fork");
    exit(EXIT_FAILURE);
  case 0:
    exit(EXIT_SUCCESS);
  default:
    // On exécute la commande
    execve(argv[0], argv, envp);
    perror("execve");
    // fermer le tube en cas d'erreur
  }

  return NULL;
}

int main(void) {
  // On commence par créer un segment de mémoire partagée

  const info *descriptor = file_vide(NOM_FILE, O_RDWR | O_CREAT | O_EXCL,
    S_IRUSR | S_IWUSR, sizeof(argsc));
  if (descriptor == NULL) {
    perror("création file");
    exit(EXIT_FAILURE);
  }
  // bloquant a lexe pas besoin de while et doc
  argsc *temp;
  while ((temp = (argsc *)file_retirer(descriptor)) != NULL) {
    pthread_t th;
    if (pthread_create(&th, NULL, run, temp) != 0) {
        fprintf(stderr, "Erreur\n");
        exit(EXIT_FAILURE);
    }
    pthread_exit(NULL);
  }
  return EXIT_SUCCESS;
}
