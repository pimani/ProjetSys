// Interface pour le lanceur

#ifndef LANCEUR_H
#define LANCEUR_H

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define NOM_SHM "/shm_file_synchro10101000101"
#define MAX_NOM_SIZE 32

// Structure permettant de gérer toutes les variables d'arguments et
// d'environnements
typedef struct argsc argsc;

//Exécute le thread avec les arguments passés dans *arg
extern void * run(argsc * arg);

#endif
