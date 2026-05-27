/*
 * c8080 stdlib
 * Copyright (c) 2026 Aleksey Morozov aleksey.f.morozov@gmail.com aleksey.f.morozov@yandex.ru
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdlib.h>
#include <c8080/heap.h>

// Very slow, but a small memory allocation algorithm.
// In the future, there will be an RB tree of free blocks here.

void *malloc(size_t size) {
    size += sizeof(struct __HeapBlock);
    if (size < sizeof(struct __HeapBlock))
        return NULL;

    struct __HeapBlock *i = &__heap;
    do {
        if (i->free) {
            while (i->next->free)  // The last block is always used
                i->next = i->next->next;
            const size_t block_size = (void *)i->next - (void *)i;
            if (block_size >= size) {
                if (block_size - size >= sizeof(struct __HeapBlock)) {
                    struct __HeapBlock *n = (void *)i + size;
                    n->free = 1;
                    n->next = i->next;
                    i->next = n;
                }
                i->free = 0;
                return (void *)i + sizeof(struct __HeapBlock);
            }
        }
        i = i->next;
    } while (i);
    return NULL;
}
