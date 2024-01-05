#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "../DataStruct/queue.h"



// Fontion pour intialiser une file
Queue* QueueInit()
{
    Queue* queue = malloc(sizeof(*queue));

    if (queue == NULL)
    {
        return -1;
    }

    queue->m_pHeadEntry = NULL;
    queue->m_pTailEntry = NULL;
    queue->m_nItemsCount = 0;

    return queue;
}

// La procedure enfiler 
void Enqueue(Queue* queue, int nEntryValue)
{
    QueueEntry* pNewEntry = malloc(sizeof(*pNewEntry));

    if (queue == NULL || pNewEntry == NULL) //verifier l'existance de la file
    {
        return -1;
    }

    pNewEntry->m_nData = nEntryValue;
    pNewEntry->m_pNextEntry = NULL;

    if (queue->m_pHeadEntry != NULL) /* La file n'est pas vide */
    {
        
        queue->m_pTailEntry->m_pNextEntry = pNewEntry;
    }
    else /* La file est vide, notre élément est le premier */
    {
        queue->m_pHeadEntry = pNewEntry;
    }

    queue->m_pTailEntry = pNewEntry;
    queue->m_nItemsCount++;
}

// La procedure defiler 
int Dequeue(Queue* queue) 
{
    if (queue == NULL) //verifier l'existance de la file
    {
        return -1;
    }

    int nombreDefile = -1;

 
    if (queue->m_pHeadEntry != NULL)   /* On vérifie s'il y a quelque chose à défiler */
    {
        QueueEntry* elementDefile = queue->m_pHeadEntry;

        nombreDefile = elementDefile->m_nData;
        queue->m_pHeadEntry = elementDefile->m_pNextEntry;
        free(elementDefile);
        queue->m_nItemsCount--;
    }

    return nombreDefile;
}

int peek(Queue* queue)
{
    if (queue == NULL) //verifier l'existance de la file
    {
        return -1;
    }

    return queue->m_pHeadEntry ? queue->m_pHeadEntry->m_nData : -1;
}


// Fonction pour vérifier si la file est vide
int Queue_IsEmpty(Queue* queue) {
    if (queue->m_pHeadEntry == NULL) {
        return 1;
    }
    else {
        return 0;
    }
}


void Queue_Display(Queue* queue)
{
    if (queue == NULL) //verifier l'existance de la file
    {
        return -1;
    }

    // Ma façon de presenter la file dans la console
    printf("\n      ------------------------------------------------------------\n");

    if (Queue_IsEmpty(queue) == 1) {
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
        QueueEntry* element = queue->m_pHeadEntry;
        printf("\n       ");
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 113);
        printf(" head ");
        while (element != NULL)
        {
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
            printf("-> (@=%x) %d (@suiv=%x)",(UINT)element, element->m_nData, (UINT)element->m_pNextEntry); // representer le contenu de l'element avec son adresse et l adresse du suivant
            element = element->m_pNextEntry;
        }
        printf(" <- ");
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 116);
        printf(" tail ");
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
        printf("\n");
    }

    printf("\n      ------------------------------------------------------------\n");
}

void DisplayHeadTail(Queue* queue)
{
    if (queue == NULL)  //verifier l'existance de la file
    {
        return -1;
    }


    QueueEntry* FirstElt = queue->m_pHeadEntry;
    QueueEntry* LastElt = queue->m_pTailEntry;

    //affichage
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 113);
    printf("\n Head: %d \n\n", FirstElt->m_nData);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 116);
    printf(" TaiL: %d \n\n", LastElt->m_nData);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
}

// Fonction pour vider la file
void Queue_EmptyFree(Queue* queue) {
    QueueEntry* current = queue->m_pHeadEntry;
    QueueEntry* m_pNextEntry;

    while (current != NULL) {
        m_pNextEntry = current->m_pNextEntry;
        free(current);
        current = m_pNextEntry;
    }

    queue->m_nItemsCount = 0;

    queue->m_pHeadEntry = NULL;
}

// Fonction pour compter le nombre d'éléments dans la file
int Queue_items_count(Queue* queue) {
    return queue->m_nItemsCount;
}

// Fonction pour ajouter des valeurs aléatoires à la file
void Queue_RandFill(Queue* queue, int nItemsCount) {
    Queue_EmptyFree(queue);
    srand(time(NULL));
    for (int i = 0; i < nItemsCount; i++) {
        int RandomValue = rand() % 100 ; // générer une valeur aléatoire entre 1 et 200
        Enqueue(queue, RandomValue);     // ajouter la valeur aléatoire à la file
    }
}