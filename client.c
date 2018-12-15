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
  for (int i = 0; i < 3; ++i) {
    printf("Argument numéro %d : ", i);
    if (fgets(cmd.argv[i], sizeof(cmd.argv[i]), stdin) == NULL) {
      perror("fgets");
      exit(EXIT_FAILURE);
    }
    if(cmd.argv[i][strlen(cmd.argv[i]) - 1] == '\n') {
      cmd.argv[i][strlen(cmd.argv[i]) - 1] = 0;
    }
  }
  printf("Donner vos variables d'environements:");
  if (fgets(cmd.envp, sizeof(cmd.envp), stdin) == NULL) {
    perror("fgets");
    exit(EXIT_FAILURE);
  }
  if(cmd.envp[strlen(cmd.envp) - 1] == '\n') {
    cmd.envp[strlen(cmd.envp) - 1] = 0;
  }
  strcpy(cmd.tube_in, wtubename);
  strcpy(cmd.tube_out, rtubename);
  file_ajout((info *) descriptor, &cmd);

  if (mkfifo(wtubename, S_IRUSR | S_IWUSR) == -1) {
    perror("mkfifo wtube");
    exit(EXIT_FAILURE);
  }

  if (mkfifo(rtubename, S_IRUSR | S_IWUSR) == -1) {
    perror("mkfifo rtube");
    exit(EXIT_FAILURE);
  }

  printf("ouverture tube ecriture\n");
  int fdr = open(rtubename, O_RDONLY);
  if (fdr == -1) {
    perror("open rtube");
    exit(EXIT_FAILURE);
  }

  /*
  int fdw = open(wtubename, O_WRONLY);
  if (fdw == -1) {
    perror("open wtube");
    exit(EXIT_FAILURE);
  }
  if (unlink(wtubename) == -1) {
    perror("unlink");
    exit(EXIT_FAILURE);
  }*/
  printf("chgmt E/S");
  //dup2(fdw, STDIN_FILENO);
  dup2(STDOUT_FILENO, fdr);
  //close(fdw);
  if (close(fdr) == -1) {
    perror("close(fdr)");
    exit(EXIT_FAILURE);
  }

  return EXIT_SUCCESS;
}
