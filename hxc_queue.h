#ifndef HXC_QUEUE_H
#define HXC_QUEUE_H

#include <stdint.h>
#include <stdbool.h>

/* Data Reference */
/*
*   u32 size            <0x00>
*   u16 elementSize     <0x04>
*   u16 indexB          <0x06>
*   u16 indexF          <0x08>
*/
// TODO maybe define allocation standards as flags? (may require change to the memory structure)

void* HxcQueueCreate(uint16_t sizeOfElement, uint16_t nPreAllocate, int alignmentFlags);
void HxcQueuePushFront(void* q, void* data);
void HxcQueuePushBack(void* q, void* data);

uint16_t HxcQueueUsedNElements(void* q);
uint32_t HxcQueueUsedSize(void* q);

void HxcQueuePopFront(void* q);
void HxcQueuePopBack(void* q);
void HxcQueuePopFrontR(void* q, void* out);
void HxcQueuePopBackR(void* q, void* out);

bool HxcQueueIsEmpty(void* q);

void HxcQueueAt(void* q, int index, void* out);

#ifdef HXC_DBG_QUEUE
void HxcDBGQueuePrintInfo(void* q);
#endif

#define HXC_QUEUE_ALIGN_BEG 0b0001
#define HXC_QUEUE_ALIGN_MID 0b0010
#define HXC_QUEUE_ALIGN_END 0b0100

#define HXC_QUEUE_STATIC  0b0001
#define HXC_QUEUE_ROLLDBL 0b0010

#endif // HXC_QUEUE_H