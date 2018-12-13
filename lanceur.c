#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
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

// Structure de transmission de demande, exemple :
// char* argv[] = {"/bin/ps", "-j", NULL};
// char* envp[] = {NULL};
// char* tube_in = "mon tube pour savoir se qu'il se passe";
// char* tube_out = "mon tube pour envoyer mes info";
struct argsc {
  char argv[ARG_NUMBER][ARG_LENGHT];
  char envp[ENVIRONEMENT_LENGHT];
  char tube_in[TUBE_LENGHT];
  char tube_out[TUBE_LENGHT];
};

#define NOM_FILE "/ma_file_a_moi_786432985365428"

void * run(argsc *arg) {
  printf("Nouveau thread");
  if (arg == NULL) {
    fprintf(stderr, "Unexpected argument value\n");
    exit(EXIT_FAILURE);
  }
  printf("Exécute %s %s %s %s %s\n", arg -> argv[0], arg -> argv[1],
    arg -> argv[2], arg -> argv[3], arg -> argv[4]);

  char *temparg[ARG_NUMBER + 1];
  for (size_t i = 0; i < ARG_NUMBER; i += 1) {
    temparg[i] = arg -> argv[i];
  }
  temparg[ARG_NUMBER] = NULL;
  char *tempenv[2];
  tempenv[0] = arg -> envp;
  tempenv[1] = NULL;

  int fin;
  if (fin = open(arg -> tube_in, O_RDWR) == -1) {
    perror("Ouverture tube in");
    free(arg);
    exit(EXIT_FAILURE);
  }
  if (dup2(stdout, fin) == -1) {
    perror("dup2");
    free(arg);
    exit(EXIT_FAILURE);
  }

  int fout;
  if (fout = open(arg -> tube_out, O_RDWR) == -1) {
    perror("Ouverture tube out");
    free(arg);
    exit(EXIT_FAILURE);
  }
  if (dup2(stdin, fout) == -1) {
    perror("dup2");
    free(arg);
    exit(EXIT_FAILURE);
  }

  switch (fork()) {
  case -1:
    perror("fork");
    exit(EXIT_FAILURE);
  case 0:
    free(arg);
    exit(EXIT_SUCCESS);
  default:
    // On exécute la commande
    execvpe(arg -> argv[0], temparg, tempenv);
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
    printf("Nouvelle donnée");
    pthread_t th;
    if (pthread_create(&th, NULL,(void * (*)(void*))run, temp) != 0) {
        fprintf(stderr, "Erreur\n");
        exit(EXIT_FAILURE);
    }
    pthread_exit(NULL);
  }
  file_vider(descriptor);
  return EXIT_SUCCESS;
}
