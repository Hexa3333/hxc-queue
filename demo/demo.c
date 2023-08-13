#include "hxc_queue.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(void)
{
    void* q = HxcQueueCreate(sizeof(int), 10, HXC_QUEUE_ALIGN_MID);

    int five = 5;
    int ten = 10;
    
    if (HxcQueueIsEmpty(q)) printf("Empty\n");

    HxcQueuePushBack(q, &five);
    HxcQueuePushBack(q, &ten);
    HxcQueuePushBack(q, &five);
    HxcQueuePushBack(q, &ten);

    int len = HxcQueueUsedNElements(q);
    printf("Amount: %i\n", len);

    for (int i = 0; i < HxcQueueUsedNElements(q); i++)
    {
        int n;
        HxcQueueAt(q, i, &n);
        printf("[%i]: %i\n", i, n);
    }

    free(q);
}