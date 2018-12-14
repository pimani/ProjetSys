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

// Remplace le comportement par défaut pour les signaux d'intéruptions
extern void handlerStop(int signum);

// Gérer les signaux de fin de vie des enfants pour gérer les procéssus zombies
extern void handlerFils(int signum);

#endif
