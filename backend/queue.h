#ifndef QUEUE_H
#define QUEUE_H

typedef struct QNode {
    int r;
    int c;
    struct QNode *next;
} QNode;

typedef struct Queue {
    QNode *front;
    QNode *rear;
} Queue;

Queue *queue_create(void);
void queue_destroy(Queue *q);
int queue_is_empty(Queue *q);
void queue_push(Queue *q, int r, int c);
int queue_pop(Queue *q, int *r, int *c);

#endif
