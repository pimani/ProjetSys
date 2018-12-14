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

const info *descriptor;

#define NOM_FILE "/ma_file_a_moi_786432985365428"

int main(void) {

  struct sigaction actionInt;
  actionInt.sa_handler = handlerStop;
  actionInt.sa_flags = 0;
  if (sigfillset(&actionInt.sa_mask) == -1) {
    perror("sigfilltset");
    exit(EXIT_FAILURE);
  }

  if (sigaction(SIGINT, &actionInt, NULL) == -1) {
    perror("sigaction");
    exit(EXIT_FAILURE);
  }

  if (sigaction(SIGHUP, &actionInt, NULL) == -1) {
    perror("sigaction");
    exit(EXIT_FAILURE);
  }

  struct sigaction actionChild;
  actionChild.sa_handler = handlerFils;
  actionChild.sa_flags = 0;
  if (sigfillset(&actionChild.sa_mask) == -1) {
    perror("sigfilltset");
    exit(EXIT_FAILURE);
  }
  if (sigaction(SIGCHLD, &actionChild, NULL) == -1) {
    perror("sigaction");
    exit(EXIT_FAILURE);
  }

  descriptor = file_vide(NOM_FILE, O_RDWR | O_CREAT | O_EXCL,
    S_IRUSR | S_IWUSR, sizeof(argsc));
  if (descriptor == NULL) {
    perror("création file");
    exit(EXIT_FAILURE);
  }
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


void * run(argsc *arg) {
  printf("Nouveau thread");
  if (arg == NULL) {
    fprintf(stderr, "Unexpected argument value\n");
    exit(EXIT_FAILURE);
  }
  char *temparg[ARG_NUMBER + 1];
  for (size_t i = 0; i < ARG_NUMBER; i += 1) {
    temparg[i] = arg -> argv[i];
  }
  temparg[ARG_NUMBER] = NULL;
  char *tempenv[2];
  tempenv[0] = arg -> envp;
  tempenv[1] = NULL;

  int fin;
  if ((fin = open(arg -> tube_in, O_RDWR)) == -1) {
    perror("Ouverture tube in");
    free(arg);
    exit(EXIT_FAILURE);
  }
  if (dup2(fin, STDOUT_FILENO) == -1) {
    perror("dup2");
    free(arg);
    exit(EXIT_FAILURE);
  }
  if (close(fin) == -1) {
    perror("close(fin)");
    exit(EXIT_FAILURE);
  }

  int fout;
  if ((fout = open(arg -> tube_out, O_RDWR)) == -1) {
    perror("Ouverture tube out");
    free(arg);
    exit(EXIT_FAILURE);
  }
  if (dup2(fout, STDIN_FILENO) == -1) {
    perror("dup2");
    free(arg);
    exit(EXIT_FAILURE);
  }
  if (close(fout) == -1) {
    perror("close(fout)");
    exit(EXIT_FAILURE);
  }
  switch (fork()) {
  case -1:
    perror("fork");
    exit(EXIT_FAILURE);
  case 0:
    break;
  default:
    // On exécute la commande
    execvpe(arg -> argv[0], temparg, tempenv);
    perror("execve");
    // fermer le tube en cas d'erreur
  }
  free(arg);
  return NULL;
}

void handlerFils(int signum) {
  if (signum != SIGCHLD) {
    fprintf(stderr, "Mauvais signal reçu.\n");
    exit(EXIT_FAILURE);
  }
  int errno_old = errno;
  int r;
  do {
    r = waitpid(-1, NULL, WNOHANG);
  } while (r > 0);
  if (r == -1) {
    if (errno != ECHILD) {
      perror("waitpid");
      exit(EXIT_FAILURE);
    }
  }

  errno = errno_old;
}

void handlerStop(int signum) {
  if (signum < 0) {
    fprintf(stderr, "Wrong signal number\n");
    exit(EXIT_FAILURE);
  }
  if (file_vider(descriptor) == -1) {
    perror("Fermeture espace mémoire partager");
    exit(EXIT_FAILURE);
  }
  exit(EXIT_SUCCESS);
}
