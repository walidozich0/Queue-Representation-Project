#include <stdio.h>
#include <stdlib.h>
#include "../DataStruct/queue.h"

void menu() {


    printf("\n\n\n\nMenu :\n");
    printf("1. enfiler une valeur\n");
    printf("2. defiler la tete\n");
    printf("3. afficher la tete et la queue\n");
    printf("4. Vider la file\n");
    printf("5. Remplir la file aleatoirement\n");
    printf("6. Quitter\n\n\n");
    printf("Veuillez saisir un chiffre correspondant a votre choix : ");


}

int main() {

    Queue* maFile = QueueInit();
    int nbrElem;

    printf("\nUne file est initialise !\n\n\n");
    system("pause");
    int quitter,choice,n,rand;

    do {
        nbrElem = Queue_items_count(maFile);
        system("cls");
        printf("\nEtat de la file :                                         Nombre d'elements : %d\n\n",nbrElem);
        Queue_Display(maFile);
        menu();
        scanf_s("%d", &choice);
        switch (choice) {
        case 1:
            printf("\nEntrez la valeur: \n\n\n");
            scanf_s("%d", &n);
            Enqueue(maFile, n);
            break;
        case 2:
            printf("\nJe defile %d\n\n\n", Dequeue(maFile));
            system("pause");
            break;
        case 3:
            DisplayHeadTail(maFile);
            system("pause");

            break;
        case 4:
            Queue_EmptyFree(maFile);
            break;
        case 5:

            printf("\nCombien d'elements vous voulez ajouter :\n");
            scanf_s("%d", &rand);
            Queue_RandFill(maFile, rand);

            break;
        case 6:
            system("cls");
            printf("\nVous voulez quitter le programme ?\n\n");
            printf("    1.Oui      2.Non\n\n");
            scanf_s("%d", &quitter);
            if (quitter == 2) {
                choice++;
                break;
            }
            else {
                system("cls");
                printf("\n\n\n\n  MERCI, A PLUS TARD !!\n\n\n\n");
                break;
            }
            
        default:
            printf("\n\nChoix invalide. Veuillez resaisir un chiffre entre 1 et 6.\n\n\n");
            system("pause");
            break;
        }
    } while (choice != 6);


    return 0;
}

