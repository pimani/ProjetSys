#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <linux/limits.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
#include <errno.h>

#include "fifosm.h"
#include "lanceur.h"
#include "client.h"

int main(void) {
  printf("Démarage du client : \n");
  printf("Ouverture mémoire partager '%s'\n", NOM_FILE);
  const info *descriptor = file_ouvre(NOM_FILE);
  if (descriptor == NULL) {
    perror("ouverture file");
    return EXIT_FAILURE;
  }
  char wtubename[MAX_NOM_SIZE];
  strcpy(wtubename, NOM_FIFOW);
  sprintf(wtubename + strlen(wtubename), "%d", getpid());
  char rtubename[MAX_NOM_SIZE];
  strcpy(rtubename, NOM_FIFOR);
  sprintf(rtubename + strlen(rtubename), "%d", getpid());

  argsc cmd;
  printf("Donner vos argument, le premier étant la commande :\n");
  for (int i = 0; i < 3; ++i) {
    printf("Argument numéro %d : ", i);
    if (fgets(cmd.argv[i], sizeof(cmd.argv[i]), stdin) == NULL) {
      perror("fgets");
      exit(EXIT_FAILURE);
    }
  }
  printf("Donner vos variables d'environements:");
  if (fgets(cmd.envp, sizeof(cmd.envp), stdin) == NULL) {
    perror("fgets");
    exit(EXIT_FAILURE);
  }
  strcpy(cmd.tube_in, wtubename);
  strcpy(cmd.tube_out, rtubename);

  file_ajout((info *) descriptor, &cmd);
  printf("Envois du méssage et remplacement des entrée et sortie standards\n");
  printf("Creation des tubes :\nCreation tube écriture\n");

  if (mkfifo(wtubename, S_IRUSR | S_IWUSR) == -1) {
    perror("mkfifo wtube");
    exit(EXIT_FAILURE);
  }

  printf("Creation tube lecture\n");
  if (mkfifo(rtubename, S_IRUSR | S_IWUSR) == -1) {
    perror("mkfifo rtube");
    exit(EXIT_FAILURE);
  }

  printf("ouverture tube ecriture\n");
  int fdw = open(wtubename, O_WRONLY);
  if (fdw == -1) {
    perror("open wtube");
    exit(EXIT_FAILURE);
  }

  int fdr = open(rtubename, O_RDONLY);
  if (fdr == -1) {
    perror("open rtube");
    exit(EXIT_FAILURE);
  }
  printf("chgmt E/S");
  dup2(STDIN_FILENO, fdw);
  dup2(STDOUT_FILENO, fdr);
  printf("fin");

  return EXIT_SUCCESS;
}
