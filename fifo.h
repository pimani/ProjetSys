// Interface pour la spécification d'une pile polymorphe

#ifndef FILE_H
#define FILE_H

#include <stdbool.h>
#include <stddef.h>

// Structure permettant de gérer une file avec des éléments quelconques.
// Les données ne sont pas directement stocké mais, seulement des address de
// type voide *, la gestion de la cohérence de se pointer est laisser à la
// discrétion de l'utilisateur durant la vie de la structure.
typedef struct file file;

// Créer une structure correspondant à une file vide.
// Renvois NULL en cas de dépassement de capacité mémoire sinon renvois l'addresse
// de la structure
extern file *file_vide(void);

// ajoute l'éléments pointer par ptr à la fin de la file s.
// Renvois NULL si ptr == null ou en cas de dépassement de capacité mémoire,
// sinon renvois ptr;
extern const void *file_ajout(file *f, const void *ptr);

// Défile la file s, renvois nulle si la file et vide sinon renvois l'address
// de l'élément qui était au début de la file
extern const void *file_retirer(file *f);

// Renvois vrais ou faux si la file s est vide.
extern bool file_est_vide(const file *f);

// Renvois la hauteur de la file S.
extern size_t file_hauter(const file *f);

// SI *sptr n'est pas NULL, libére la mémoire alouer à la pile puis donne à
// *sptr la valeur NULL
extern void file_vider(stack **fptr);
