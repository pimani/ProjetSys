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

const info *descriptor;

int main(void) {
  printf("Serveur Start :\n");
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

  printf("Ouverture de la file '%s'\n", NOM_FILE);
  descriptor = file_vide(NOM_FILE, O_CREAT | O_RDWR | O_EXCL, S_IRWXU | S_IRWXG | S_IRWXO, sizeof(argsc));
  if (descriptor == NULL) {
    perror("création file");
    exit(EXIT_FAILURE);
  }
  argsc *temp;
  printf("En attente de donnée\n");
  while ((temp = (argsc *)file_retirer(descriptor)) != NULL) {
    printf("Nouvelle donnée\n");
    pthread_t th;
    if (pthread_create(&th, NULL,(void * (*)(void*))run, temp) != 0) {
        fprintf(stderr, "Erreur\n");
        exit(EXIT_FAILURE);
    }
  }
  file_vider(descriptor);
  return EXIT_SUCCESS;
}


void * run(argsc *arg) {
  printf("Nouveau thread :\n");
  if (arg == NULL) {
    fprintf(stderr, "Unexpected argument value\n");
    exit(EXIT_FAILURE);
  }
  printf("Commande a envoyer [%s]\n", arg -> argv[0]);

  //char *temparg[ARG_NUMBER + 1];
  //for (size_t i = 0; i < ARG_NUMBER; i += 1) {
  //  temparg[i] = arg -> argv[i];
  //}
  //temparg[ARG_NUMBER] = NULL;
  char *tempenv[2];
  tempenv[0] = arg -> envp;
  tempenv[1] = NULL;

  int fin;
  if ((fin = open(arg -> tube_in, O_RDWR)) == -1) {
    perror("Ouverture tube in");
    free(arg);
    return NULL;
  }
  if (dup2(fin, STDOUT_FILENO) == -1) {
    perror("dup2");
    free(arg);
    return NULL;
  }
  if (close(fin) == -1) {
    perror("close(fin)");
    return NULL;
  }

  int fout;
  if ((fout = open(arg -> tube_out, O_RDWR)) == -1) {
    perror("Ouverture tube out");
    free(arg);
    return NULL;
  }
  if (dup2(fout, STDIN_FILENO) == -1) {
    perror("dup2");
    free(arg);
    return NULL;
  }
  if (close(fout) == -1) {
    perror("close(fout)");
    return NULL;
  }
  switch (fork()) {
  case -1:
    perror("fork");
    return NULL;
  case 0:
    // On exécute la commande
    execle("/bin/ls", "/bin/ls", NULL, tempenv);
    perror("execve");
    free(arg);
    close(fout);
    // fermer le tube en cas d'erreur
    break;
  default:
    free(arg);
  }
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
