/*
 * Yet another ring buffer implementation, templated.
 *
 * Author: Rob Bultman
 * email : rob@firstbuild.com
 *
 * Copyright (c) 2014 FirstBuild
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

enum {
   RingBufT_Failure = -1,
   RingBufT_Success,
   RingBufT_IsFull,
   RingBufT_IsNotFull,
   RingBufT_IsEmpty,
   RingBufT_IsNotEmpty
};

template <typename Type>
class RingBufT {
   public:
      RingBufT(Type *pBuf, uint8_t size);
      int8_t Write(Type * pData);
      int8_t Read(Type * pData);
      int8_t IsEmpty();
      int8_t IsFull();
      int8_t Peek(Type * pData, uint8_t pos);
      int8_t ElementsUsed();
      int8_t ElementsAvailable();
   private:
      int8_t initialized;
      uint8_t head;
      uint8_t tail;
      uint8_t size;
      uint8_t elementsUsed;
      int8_t empty;
      int8_t full;
      Type *pBuf;
};

template <typename Type>
RingBufT<Type>::RingBufT(Type *_pBuf, uint8_t _size) {
   initialized = RingBufT_Failure;

   if ((_pBuf != NULL) && (_size != 0)) {
      head = 0;
      tail = 0;
      size = _size;
      pBuf = _pBuf;
      full = RingBufT_IsNotFull;
      empty = RingBufT_IsEmpty;
      elementsUsed = 0;
      initialized = RingBufT_Success;
   }
}

template <typename Type>
int8_t RingBufT<Type>::Write(Type * val) {
   int8_t retVal = RingBufT_Failure;

   if ((initialized == RingBufT_Success) && (full == RingBufT_IsNotFull) && (val != NULL)) {
      memcpy(&pBuf[tail++], val, sizeof(Type));
      elementsUsed++;
      empty = RingBufT_IsNotEmpty;
      if (tail >= size) {
         tail = 0;
      }
      if (head == tail) {
         full = RingBufT_IsFull;
      }
      retVal = RingBufT_Success;
   }

   return retVal;
}

template <typename Type>
int8_t RingBufT<Type>::Read(Type *pDest) {
   int8_t retVal = RingBufT_Failure;

   if ((initialized == RingBufT_Success) && (empty == RingBufT_IsNotEmpty) && (pDest != NULL)) {
      memcpy(pDest, &pBuf[head++], sizeof(Type));
      elementsUsed--;
      full = RingBufT_IsNotFull;
      if (head >= size) {
         head = 0;
      }
      if (head == tail) {
         empty = RingBufT_IsEmpty;
      }
      retVal = RingBufT_Success;
   }

   return retVal;
}

template <typename Type>
int8_t RingBufT<Type>::IsEmpty() {
   int8_t retVal = RingBufT_Failure;
   
   if (initialized == RingBufT_Success) {
      retVal = empty;
   }

   return retVal;
}

template <typename Type>
int8_t RingBufT<Type>::IsFull() {
   int8_t retVal = RingBufT_Failure;
   
   if (initialized == RingBufT_Success) {
      retVal = full;
   }

   return retVal;
}

template <typename Type>
int8_t RingBufT<Type>::Peek(Type *pDest, uint8_t pos) {
   int8_t retVal = RingBufT_Failure;

   if ((initialized == RingBufT_Success) && (pos < size) && (empty == RingBufT_IsNotEmpty) && (pos < elementsUsed) && (pDest != NULL)) {
      pos = (uint8_t)(pos + head);
      if (pos >= size) {
         pos = (uint8_t)(pos - (size - 1));
      }
      memcpy(pDest, &pBuf[pos], sizeof(Type));
      retVal = RingBufT_Success;
   }

   return retVal;
}

template <typename Type>
int8_t RingBufT<Type>::ElementsUsed() {
   int8_t retVal = RingBufT_Failure;

   if (initialized == RingBufT_Success) {
      retVal = (int8_t)elementsUsed;
   }

   return retVal;
}

template <typename Type>
int8_t RingBufT<Type>::ElementsAvailable() {
   int8_t retVal = RingBufT_Failure;

   if (initialized == RingBufT_Success) {
      retVal = (int8_t)(size - elementsUsed);
   } else {
      puts("Buffer not initialized.");
   }

   return retVal;
}

