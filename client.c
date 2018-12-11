#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/limits.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>

#include "fifosm.h"
#include "lanceur.h"
#include "client.h"

#define NOM_FIFOW "write_tube_123456"
#define NOM_FIFOR "read_tube_1234567"

int main(void) {
  if (mkfifo(NOM_FIFOW, S_IRUSR | S_IWUSR) == -1) {
    perror("mkfifo wtube");
    exit(EXIT_FAILURE);
  }
  if (mkfifo(NOM_FIFOR, S_IRUSR | S_IWUSR) == -1) {
    perror("mkfifo rtube");
    exit(EXIT_FAILURE);
  }
  
  fdw = open(NOM_FIFO, O_WRONLY);
  if (fdw == -1) {
    perror("open wtube");
    exit(EXIT_FAILURE);
  }
  
  fdr = open(NOM_FIFO, O_RDONLY);
  if (fdr == -1) {
    perror("open rtube");
    exit(EXIT_FAILURE);
  }
  dup2(fdw,0);
  
  argsc cmd;
  for (int i = 0; i < 3; ++i) {
    if (fgets(cmd->argv[i], sizeof(cmd->argv[i]), stdin) == NULL) {
      perror("fgets");
      exit(EXIT_FAILURE);
    }
  }
  if (fgets(cmd->envp, sizeof(cmd->envp), stdin) == NULL) {
    perror("fgets");
    exit(EXIT_FAILURE);
  }
}
