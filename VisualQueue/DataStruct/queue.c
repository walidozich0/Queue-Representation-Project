#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "../DataStruct/queue.h"

// Fontion pour intialiser une file
File* initialiser()
{
    File* file = malloc(sizeof(*file));

    if (file == NULL)
    {
        // Gérer l'erreur de l'allocation de mémoire ici pour eviter l'erreur Warning C6011
        printf("Erreur lors de l'allocation de la mémoire\n");
        exit(1);
    }

    file->premier = NULL;

    return file;
}

// La procedure enfiler 
void enfiler(File* file, int nvNombre)
{
    Element* nouveau = malloc(sizeof(*nouveau));

    if (file == NULL || nouveau == NULL) //verifier l'existance de la file
    {
        exit(EXIT_FAILURE);
    }

    nouveau->nombre = nvNombre;
    nouveau->suivant = NULL;

    if (file->premier != NULL) /* La file n'est pas vide */
    {
        
        Element* elementActuel = file->premier;  /* On se positionne à la fin de la file */
        while (elementActuel->suivant != NULL)
        {
            elementActuel = elementActuel->suivant;
        }
        elementActuel->suivant = nouveau;
    }
    else /* La file est vide, notre élément est le premier */
    {
        file->premier = nouveau;
    }
}

// La procedure defiler 
int defiler(File* file) 
{
    if (file == NULL) //verifier l'existance de la file
    {
        exit(EXIT_FAILURE);
    }

    int nombreDefile = 0;

 
    if (file->premier != NULL)   /* On vérifie s'il y a quelque chose à défiler */
    {
        Element* elementDefile = file->premier;

        nombreDefile = elementDefile->nombre;
        file->premier = elementDefile->suivant;
        free(elementDefile);
    }

    return nombreDefile;
}


// Fonction pour vérifier si la file est vide
int file_est_vide(File* file) {
    if (file->premier == NULL) {
        return 1;
    }
    else {
        return 0;
    }
}


void afficherFile(File* file)
{
    if (file == NULL) //verifier l'existance de la file
    {
        exit(EXIT_FAILURE);
    }

    // Ma façon de presenter la file dans la console
    printf("\n      ------------------------------------------------------------\n");

    if (file_est_vide(file) == 1) {
        printf("\n        File vide\n");
        printf("\n       ");
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 113);
        printf(" Head -> Null ");
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
        printf("\n       ");
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 116);
        printf(" Tail -> Null \n");
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    }
    else {
        Element* element = file->premier;
        printf("\n       ");
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 113);
        printf(" head ");
        while (element != NULL)
        {
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
            printf("-> (@=%x) %d (@suiv=%x)",&element->nombre, element->nombre,element->suivant); // representer le contenu de l'element avec son adresse et l adresse du suivant
            element = element->suivant;
        }
        printf(" <- ");
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 116);
        printf(" tail ");
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
        printf("\n");
    }

    printf("\n      ------------------------------------------------------------\n");
}


void afficherTeteEtQueue(File* file)
{
    if (file == NULL)  //verifier l'existance de la file
    {
        exit(EXIT_FAILURE);
    }


    Element* premiere = file->premier;
    Element* derniere = premiere;

    while (derniere->suivant != NULL)
    {
        derniere = derniere->suivant;
    }

    //affichage
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 113);
    printf("\n Head: %d \n", premiere->nombre);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 116);
    printf(" TaiL: %d \n\n", derniere->nombre);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
}

// Fonction pour vider la file
void file_vider(File* file) {
    Element* courant = file->premier;
    Element* suivant;

    while (courant != NULL) {
        suivant = courant->suivant;
        free(courant);
        courant = suivant;
    }

    file->premier = NULL;
}

// Fonction pour compter le nombre d'éléments dans la file
int file_nombre_elements(File* file) {
    int compteur = 0;
    Element* courant = file->premier;

    while (courant != NULL) {
        compteur++;
        courant = courant->suivant;
    }

    return compteur;
}

// Fonction pour ajouter des valeurs aléatoires à la file
void file_ajouter_aleatoires(File* file, int nb_elements) {
    file_vider(file);
    for (int i = 0; i < nb_elements; i++) {
        int valeur_aleatoire = rand() % 100 + 1; // générer une valeur aléatoire entre 1 et 100
        enfiler(file, valeur_aleatoire);     // ajouter la valeur aléatoire à la file
    }
}