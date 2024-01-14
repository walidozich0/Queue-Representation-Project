#ifndef H_FILE
#define H_FILE

#ifdef __cplusplus  //Pour pouvoir utiliser ce fichier .h dans un fichier cpp 
extern "C" {
#endif

typedef struct QueueEntry QueueEntry;
struct QueueEntry
{
    int m_nData;
    QueueEntry* m_pNextEntry;
};

typedef struct Queue Queue;
struct Queue
{
    QueueEntry* m_pHeadEntry;
    QueueEntry* m_pTailEntry;
    int m_nItemsCount;
};

Queue* QueueInit();
void Enqueue(Queue* queue, int nvNombre);
int Dequeue(Queue* queue);
int peek(Queue* queue);
void Queue_Display(Queue* queue);
void DisplayHeadTail(Queue* queue);
int Queue_IsEmpty(Queue* queue);
void Queue_EmptyFree(Queue* queue);
int Queue_items_count(Queue* queue);
void Queue_RandFill(Queue* queue, int nItemsCount);

#ifdef __cplusplus
}
#endif

#endif
