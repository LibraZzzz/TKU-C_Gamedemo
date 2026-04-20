#include <stdlib.h>

#include "queue.h"

Queue *queue_create(void) {
    Queue *q = (Queue *)malloc(sizeof(Queue));
    if (!q) return NULL;
    q->front = NULL;
    q->rear = NULL;
    return q;
}

void queue_destroy(Queue *q) {
    if (!q) return;
    QNode *cur = q->front;
    while (cur) {
        QNode *next = cur->next;
        free(cur);
        cur = next;
    }
    free(q);
}

int queue_is_empty(Queue *q) {
    return !q || q->front == NULL;
}

void queue_push(Queue *q, int r, int c) {
    if (!q) return;
    QNode *node = (QNode *)malloc(sizeof(QNode));
    if (!node) return;
    node->r = r;
    node->c = c;
    node->next = NULL;

    if (q->rear) {
        q->rear->next = node;
    } else {
        q->front = node;
    }
    q->rear = node;
}

int queue_pop(Queue *q, int *r, int *c) {
    if (queue_is_empty(q)) return 0;
    QNode *node = q->front;
    q->front = node->next;
    if (!q->front) q->rear = NULL;

    if (r) *r = node->r;
    if (c) *c = node->c;
    free(node);
    return 1;
}
