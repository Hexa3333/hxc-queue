#include "hxc_queue.h"
#include <stdlib.h>
#include <string.h>

#define HXC_QUEUE_SIZE_SIZE 4
#define HXC_QUEUE_ELEMENTSIZE_SIZE 2
#define HXC_QUEUE_INDEX_SIZE 2

#define HXC_QUEUE_SIZE_OFFSET 0x00
#define HXC_QUEUE_ELEMENTSIZE_OFFSET 0x04 
#define HXC_QUEUE_INDEXB_OFFSET 0x06
#define HXC_QUEUE_INDEXF_OFFSET 0x08
#define HXC_QUEUE_DATA_OFFSET 0x0A

#ifdef HXC_DEBUG_QUEUE
#include <stdio.h>
#endif

void* HxcQueueCreate(uint16_t sizeOfElement, uint16_t nPreAllocate, int alignmentFlags)
{
    void* memory = malloc(HXC_QUEUE_DATA_OFFSET + (sizeOfElement*nPreAllocate));
    uint32_t size = HXC_QUEUE_DATA_OFFSET + (nPreAllocate * sizeOfElement);
    uint16_t indexBack, indexFront;

    alignmentSpec:
    if (alignmentFlags & HXC_QUEUE_ALIGN_BEG)
    {
        indexBack = 0;
        indexFront = 1;
    }
    else if (alignmentFlags & HXC_QUEUE_ALIGN_MID)
    {
        indexBack = nPreAllocate/2 - 1;
        indexFront = nPreAllocate/2;
    }
    else if (alignmentFlags & HXC_QUEUE_ALIGN_END)
    {
        indexBack = nPreAllocate-1;
        indexFront = nPreAllocate;
    }
    else
    {
        alignmentFlags = HXC_QUEUE_ALIGN_MID;
        goto alignmentSpec;
    }

    memcpy(memory, &size, HXC_QUEUE_SIZE_SIZE);
    memcpy(memory+HXC_QUEUE_ELEMENTSIZE_OFFSET, &sizeOfElement, HXC_QUEUE_ELEMENTSIZE_SIZE);
    memcpy(memory+HXC_QUEUE_INDEXB_OFFSET, &indexBack, HXC_QUEUE_INDEX_SIZE);
    memcpy(memory+HXC_QUEUE_INDEXF_OFFSET, &indexFront, HXC_QUEUE_INDEX_SIZE);

    return memory;
}

// STATIC FUNCTIONS START
uint32_t _GetSize(void* q)
{
    uint32_t size;
    memcpy(&size, q, HXC_QUEUE_SIZE_SIZE);
    return size;
}

uint16_t _GetElementSize(void* q)
{
    uint16_t elSize;
    memcpy(&elSize, q+HXC_QUEUE_ELEMENTSIZE_OFFSET, HXC_QUEUE_ELEMENTSIZE_SIZE);
    return elSize;
}

uint16_t _GetIndexBack(void* q)
{
    uint16_t indexBack;
    memcpy(&indexBack, q+HXC_QUEUE_INDEXB_OFFSET, HXC_QUEUE_INDEX_SIZE);
    return indexBack;
}

uint16_t _GetIndexFront(void* q)
{
    uint16_t indexFront;
    memcpy(&indexFront, q+HXC_QUEUE_INDEXF_OFFSET, HXC_QUEUE_INDEX_SIZE);
    return indexFront;
}

uint16_t _GetElementCount(void* q)
{
    uint16_t indexB = _GetIndexBack(q);
    uint16_t indexF = _GetIndexFront(q);

    // Why -1 ?
    // indexF is at least indexB + 1
    return indexF - indexB - 1;
}

uint32_t _GetCurrentFrontOffset(void* q)
{
    uint16_t indexFront = _GetIndexFront(q);
    uint16_t elSize = _GetElementSize(q);

    uint32_t front = HXC_QUEUE_DATA_OFFSET + (indexFront*elSize);

    return front;    
}

uint32_t _GetCurrentBackOffset(void* q)
{
    uint16_t indexBack = _GetIndexBack(q);
    uint16_t elSize = _GetElementSize(q);

    uint32_t back = HXC_QUEUE_DATA_OFFSET + (indexBack*elSize);

    return back;
}
// STATIC FUNCTIONS END

void HxcQueuePushFront(void* q, void* data)
{
    uint32_t frontOffset = _GetCurrentFrontOffset(q);
    uint16_t elSize = _GetElementSize(q);

    if (frontOffset > _GetSize(q))
    {
        // reallocation
        // recalculation
        #ifdef HXC_DEBUG_QUEUE
        fprintf(stderr, "Reallocating...\n");
        #endif
    }

    memcpy(q+frontOffset, data, elSize);

    // update front
    uint16_t newIndexFront = _GetIndexFront(q)+1;
    memcpy(q+HXC_QUEUE_INDEXF_OFFSET, &newIndexFront, HXC_QUEUE_INDEX_SIZE);
}

void HxcQueuePushBack(void* q, void* data)
{
    uint32_t backOffset = _GetCurrentBackOffset(q);
    uint16_t elSize = _GetElementSize(q);

    if (backOffset == HXC_QUEUE_DATA_OFFSET)
    {
        // reallocation
        // recalculation
        #ifdef HXC_DEBUG_QUEUE
        fprintf(stderr, "Reallocating...\n");
        #endif
    }

    memcpy(q+backOffset, data, elSize);

    // update back
    uint16_t newIndexBack = _GetIndexBack(q)-1;
    memcpy(q+HXC_QUEUE_INDEXB_OFFSET, &newIndexBack, HXC_QUEUE_INDEX_SIZE);
}

uint16_t HxcQueueUsedNElements(void* q)
{
    uint16_t indexF = _GetIndexFront(q);
    uint16_t indexB = _GetIndexBack(q);

    // Why -1 ?
    // front index is at least back index + 1
    return indexF - indexB - 1;
}

uint32_t HxcQueueUsedSize(void* q)
{
    uint16_t usedElements = HxcQueueUsedNElements(q);
    uint16_t elSize = _GetElementSize(q);

    return HXC_QUEUE_DATA_OFFSET + (usedElements*elSize);
}

void HxcQueuePopFront(void* q)
{
    // update the queue
    uint16_t newIndexFront = _GetIndexFront(q)-1;
    memcpy(q+HXC_QUEUE_INDEXF_OFFSET, &newIndexFront, HXC_QUEUE_INDEX_SIZE);
}

void HxcQueuePopBack(void* q)
{
    // update the queue
    uint16_t newIndexBack = _GetIndexBack(q)+1;
    memcpy(q+HXC_QUEUE_INDEXB_OFFSET, &newIndexBack, HXC_QUEUE_INDEX_SIZE);
}

void HxcQueuePopFrontR(void* q, void* out)
{
    uint32_t frontOffset = _GetCurrentFrontOffset(q);
    uint16_t elSize = _GetElementSize(q);
    memcpy(out, q+frontOffset-elSize, elSize);

    // update the queue
    uint16_t newIndexFront = _GetIndexFront(q)-1;
    memcpy(q+HXC_QUEUE_INDEXF_OFFSET, &newIndexFront, HXC_QUEUE_INDEX_SIZE);
}

void HxcQueuePopBackR(void* q, void* out)
{
    uint32_t backOffset = _GetCurrentBackOffset(q);
    uint16_t elSize = _GetElementSize(q);
    memcpy(out, q+backOffset+elSize, elSize);

    // update the queue
    uint16_t newIndexBack = _GetIndexBack(q)+1;
    memcpy(q+HXC_QUEUE_INDEXB_OFFSET, &newIndexBack, HXC_QUEUE_INDEX_SIZE);
}

bool HxcQueueIsEmpty(void* q)
{
    return HxcQueueUsedNElements(q) == 0;
}

void HxcQueueAt(void* q, int index, void* out)
{
    uint32_t elSize = _GetElementSize(q);

    memcpy(out, q + HXC_QUEUE_DATA_OFFSET + (index*elSize), elSize);
}

#ifdef HXC_DEBUG_QUEUE
void HxcDBGQueuePrintInfo(void* q)
{
    uint32_t size = _GetSize(q);
    uint16_t elSize = _GetElementSize(q);
    uint16_t indexB = _GetIndexBack(q);
    uint16_t indexF = _GetIndexFront(q);

    uint32_t elCount = _GetElementCount(q);

    printf("Size: %u\nElsize:%u\nIndexes: %u,%u\tOffsets: (%u, %u)\n\n", size, elSize, indexB, indexF, _GetCurrentBackOffset(q), _GetCurrentFrontOffset(q));
}
#endif

#undef HXC_QUEUE_SIZE_SIZE
#undef HXC_QUEUE_ELEMENTSIZE_SIZE
#undef HXC_QUEUE_INDEXF_SIZE

#undef HXC_QUEUE_SIZE_OFFSET
#undef HXC_QUEUE_ELEMENTSIZE_OFFSET
#undef HXC_QUEUE_INDEXB_OFFSET
#undef HXC_QUEUE_INDEXF_OFFSET
#undef HXC_QUEUE_DATA_OFFSET