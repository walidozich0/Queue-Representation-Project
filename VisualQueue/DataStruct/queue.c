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
        return -1;
    }

    file->premier = NULL;
    file->dernier = NULL;
    file->nb_elements = 0;

    return file;
}

// La procedure enfiler 
void enfiler(File* file, int nvNombre)
{
    Element* nouveau = malloc(sizeof(*nouveau));

    if (file == NULL || nouveau == NULL) //verifier l'existance de la file
    {
        return -1;
    }

    nouveau->nombre = nvNombre;
    nouveau->suivant = NULL;

    if (file->premier != NULL) /* La file n'est pas vide */
    {
        
        file->dernier->suivant = nouveau;
    }
    else /* La file est vide, notre élément est le premier */
    {
        file->premier = nouveau;
    }

    file->dernier = nouveau;
    file->nb_elements++;
}

// La procedure defiler 
int defiler(File* file) 
{
    if (file == NULL) //verifier l'existance de la file
    {
        return -1;
    }

    int nombreDefile = -1;

 
    if (file->premier != NULL)   /* On vérifie s'il y a quelque chose à défiler */
    {
        Element* elementDefile = file->premier;

        nombreDefile = elementDefile->nombre;
        file->premier = elementDefile->suivant;
        free(elementDefile);
        file->nb_elements--;
    }

    return nombreDefile;
}

int peek(File* file)
{
    if (file == NULL) //verifier l'existance de la file
    {
        return -1;
    }

    if (file->premier != NULL)   /* On vérifie s'il y a quelque chose à défiler */
    {
        Element* elementDefile = file->premier;
        return elementDefile->nombre;
    }
    else
        return -1;
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
            printf("-> (@=%x) %d (@suiv=%x)",(UINT)element, element->nombre, (UINT)element->suivant); // representer le contenu de l'element avec son adresse et l adresse du suivant
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
        return -1;
    }


    Element* premierE = file->premier;
    Element* dernierE = file->dernier;

    //affichage
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 113);
    printf("\n Head: %d \n\n", premierE->nombre);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 116);
    printf(" TaiL: %d \n\n", dernierE->nombre);
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

    file->nb_elements = 0;

    file->premier = NULL;
}

// Fonction pour compter le nombre d'éléments dans la file
int file_nombre_elements(File* file) {
    return file->nb_elements;
}

// Fonction pour ajouter des valeurs aléatoires à la file
void file_ajouter_aleatoires(File* file, int nb_elements) {
    file_vider(file);
    srand(time(NULL));
    for (int i = 0; i < nb_elements; i++) {
        int valeur_aleatoire = rand() % 100 ; // générer une valeur aléatoire entre 1 et 200
        enfiler(file, valeur_aleatoire);     // ajouter la valeur aléatoire à la file
    }
}