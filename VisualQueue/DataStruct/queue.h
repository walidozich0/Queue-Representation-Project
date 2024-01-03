#ifndef H_FILE
#define H_FILE

#ifdef __cplusplus  //Pour pouvoir utiliser ce fichier .h dans un fichier cpp 
extern "C" {
#endif

typedef struct Element Element;
struct Element
{
    int nombre;
    Element* suivant;
};

typedef struct File File;
struct File
{
    Element* premier;
};

File* initialiser();
void enfiler(File* file, int nvNombre);
int defiler(File* file);
void afficherFile(File* file);
void afficherTeteEtQueue(File* file);
int file_est_vide(File* file);
void file_vider(File* file);
int file_nombre_elements(File* file);
void file_ajouter_aleatoires(File* file, int nb_elements);

#ifdef __cplusplus
}
#endif

#endif
