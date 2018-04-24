#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include <stdint.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string>
#include <cstring>
//#include <iostream>

using namespace std;

#include "RingBufT.h"
extern "C"
{
}


TEST_GROUP(ringBufTests)
{
   uint8_t buf[10];
   RingBufT<uint8_t> *ringBuf;

   void fillBuffer() 
   {
      uint8_t i;

      for (i=0; i<sizeof(buf); i++) 
      {
         ringBuf->Write(&i);
      }
   }

   void setup()
   {
      ringBuf = new RingBufT<uint8_t>(buf, sizeof(buf)/sizeof(uint8_t));
   }

   void teardown()
   {
      delete ringBuf;
   }
};

TEST(ringBufTests, BufferIsNotInitializedIfDataIsNull)
{
   RingBufT<uint8_t> myBuf(NULL, 3);
   BYTES_EQUAL(RingBufT_Failure, myBuf.IsFull());
}

TEST(ringBufTests, BufferIsNotInitializedIfSizeIsZero)
{
   uint8_t buf[10];
   RingBufT<uint8_t> myBuf(buf, 0);
   BYTES_EQUAL(RingBufT_Failure, myBuf.IsFull());
}

TEST(ringBufTests, InitializesCorrectly)
{
   BYTES_EQUAL(RingBufT_IsNotFull, ringBuf->IsFull());
   BYTES_EQUAL(RingBufT_IsEmpty, ringBuf->IsEmpty());
   BYTES_EQUAL(0, ringBuf->ElementsUsed());
   BYTES_EQUAL(sizeof(buf)/sizeof(uint8_t), ringBuf->ElementsAvailable());
}

TEST(ringBufTests, WriteWithNullPointerFails)
{
   BYTES_EQUAL(RingBufT_Failure, ringBuf->Write(NULL));
}

TEST(ringBufTests, CanAddBytesToBuffer)
{
   uint8_t i;

   memset(&buf[0], 42, sizeof(buf));

   for (i=0; i<sizeof(buf); i++) {
      ringBuf->Write(&i);
      BYTES_EQUAL(i, buf[i]);
      BYTES_EQUAL(i+1, ringBuf->ElementsUsed());
   }

   BYTES_EQUAL(RingBufT_IsNotEmpty, ringBuf->IsEmpty());
   BYTES_EQUAL(RingBufT_IsFull, ringBuf->IsFull());
}

TEST(ringBufTests, AddingBytesToAFullBufferFails)
{
   uint8_t val = 42;
   fillBuffer();
   BYTES_EQUAL(RingBufT_Failure, ringBuf->Write(&val));
}

TEST (ringBufTests, ReadFailsIfDestinationBufferIsNull)
{
   BYTES_EQUAL(RingBufT_Failure, ringBuf->Read(NULL));
}

TEST (ringBufTests, ReadFromEmptyBufferFails)
{
   uint8_t dest;

   BYTES_EQUAL(RingBufT_Failure, ringBuf->Read(&dest));
}

TEST (ringBufTests, ReadReturnsExpectedValue)
{
   uint8_t src = 42;
   uint8_t dest;
   ringBuf->Write(&src);
   BYTES_EQUAL(RingBufT_Success, ringBuf->Read(&dest));
   BYTES_EQUAL(42, dest);
}

TEST (ringBufTests, ReadingShouldDecrementElementsUsed)
{
   uint8_t dest;
   fillBuffer();
   ringBuf->Read(&dest);
   BYTES_EQUAL(9, ringBuf->ElementsUsed());
}

TEST (ringBufTests, DataReadShouldEqualDataWritten)
{
   uint8_t i;
   uint8_t dest;

   fillBuffer();

   for(i=0; i<sizeof(buf); i++) {
      BYTES_EQUAL(RingBufT_Success, ringBuf->Read(&dest));
      BYTES_EQUAL(i, dest);
   }
}

TEST (ringBufTests, AFullBufferShouldBecomeNotFullAfterARead)
{
   uint8_t dest;
   fillBuffer();

   ringBuf->Read(&dest);
   BYTES_EQUAL(RingBufT_IsNotFull, ringBuf->IsFull());
}

TEST(ringBufTests, BufferShouldBeEmptyAfterReadingAllElements)
{
   uint8_t dest;
   uint8_t i;

   fillBuffer();

   for(i=0; i<sizeof(buf); i++) 
   {
      ringBuf->Read(&dest);
   }

   BYTES_EQUAL(RingBufT_IsEmpty, ringBuf->IsEmpty());
}

TEST(ringBufTests, NotEmptyRingBufferReturnsNotEmpty)
{
   uint8_t src = 42;
   ringBuf->Write(&src);

   BYTES_EQUAL(RingBufT_IsNotEmpty, ringBuf->IsEmpty());
}

TEST(ringBufTests, IsFullReturnsFullWhenFull) {
   fillBuffer();
   BYTES_EQUAL(RingBufT_IsFull, ringBuf->IsFull());
}

TEST(ringBufTests, IsFullReturnsNotFullWhenNotFull) {
   uint8_t src = 42;
   ringBuf->Write(&src);
   BYTES_EQUAL(RingBufT_IsNotFull, ringBuf->IsFull());
}

TEST(ringBufTests, PeekFailsIfDestinationPointerIsNull)
{
   BYTES_EQUAL(RingBufT_Failure, ringBuf->Peek(NULL, 0));
}

TEST(ringBufTests, PeekFailsIfPositionIsBad)
{
   uint8_t dest;
   BYTES_EQUAL(RingBufT_Failure, ringBuf->Peek(&dest, sizeof(buf) + 1));
}

TEST(ringBufTests, PeekFailsIfBufferIsEmpty)
{
   uint8_t dest;
   BYTES_EQUAL(RingBufT_Failure, ringBuf->Peek(&dest, 1));
}

TEST(ringBufTests, PeekFailsIfPositionIsInvalid)
{
   uint8_t src;
   uint8_t dest;
   ringBuf->Write(&src);
   BYTES_EQUAL(RingBufT_Failure, ringBuf->Peek(&dest, 1));
}

TEST(ringBufTests, CanPeekEveryElement)
{
   uint8_t i;
   uint8_t dest;

   fillBuffer();

   for(i=0; i<sizeof(buf); i++) {
      BYTES_EQUAL(RingBufT_Success, ringBuf->Peek(&dest, i));
      BYTES_EQUAL(i, dest);
   }
}

TEST(ringBufTests, PeekWorksWithWrapAround)
{
   uint8_t i;
   int8_t retVal;
   uint8_t dest;

   fillBuffer();

   for (i=0; i<4; i++) {
      ringBuf->Read(&dest);
   }

   i = 10;
   do {
      retVal = ringBuf->Write(&i);
      i++;
   } while(retVal != RingBufT_Failure);

   BYTES_EQUAL(RingBufT_Success, ringBuf->Peek(&dest, 8));
   BYTES_EQUAL(13, dest);
}

TEST(ringBufTests, ElementsUsedIsZeroIfBufferIsEmpty)
{
   BYTES_EQUAL(0, ringBuf->ElementsUsed());
}

TEST(ringBufTests, ElementsUsedReturnsSizeOfWhenFull)
{
   fillBuffer();

   BYTES_EQUAL(sizeof(buf), ringBuf->ElementsUsed());
}

TEST(ringBufTests, ElementsAvailableReturnsBufSizeWhenEmpty)
{
   BYTES_EQUAL(sizeof(buf), ringBuf->ElementsAvailable());
}

TEST(ringBufTests, ElementsAvailableReturns0WhenFull)
{
   fillBuffer();
   BYTES_EQUAL(0, ringBuf->ElementsAvailable());
}

TEST(ringBufTests, WrapAroundIsHandled)
{
   uint8_t i;
   uint8_t dest;

   for (i=0; i<5; i++) ringBuf->Write(&i);
   BYTES_EQUAL(5, ringBuf->ElementsUsed());

   for (i=0; i<5; i++) ringBuf->Read(&dest);
   BYTES_EQUAL(0, ringBuf->ElementsUsed());

   fillBuffer();
   for (i=0; i<10; i++) {
      ringBuf->Read(&dest);
      BYTES_EQUAL(i, dest);
   }
}

TEST(ringBufTests, WorksWithUint32)
{
   uint8_t i;
   uint32_t buf[10];
   uint32_t val;
   uint32_t dest;
   RingBufT<uint32_t> ringBuf(buf, sizeof(buf)/sizeof(uint32_t));

   // add some stuff
   for(i=0; i<10; i++) {
      val = ((uint32_t)i<<24) + ((uint32_t)i<<16) + ((uint32_t)i<<8) + ((uint32_t)i);
      BYTES_EQUAL(RingBufT_Success, ringBuf.Write(&val)); 
      BYTES_EQUAL(RingBufT_Success, ringBuf.Peek(&dest, i)); 
      CHECK_EQUAL(val, dest);
   }

   for(i=0; i<10; i++) {
      val = ((uint32_t)i<<24) + ((uint32_t)i<<16) + ((uint32_t)i<<8) + ((uint32_t)i);
      BYTES_EQUAL(RingBufT_Success, ringBuf.Read(&dest)); 
      CHECK_EQUAL(val, dest);
   }

   BYTES_EQUAL(RingBufT_IsEmpty, ringBuf.IsEmpty()); 
}

typedef struct AStruct_t {
   int16_t data;
   uint8_t flags;
} AStruct_t;

TEST(ringBufTests, WorksWithStruct)
{
   uint8_t i;
   AStruct_t buf[10];
   AStruct_t val;
   AStruct_t dest;
   RingBufT<AStruct_t> ringBuf(buf, sizeof(buf)/sizeof(uint32_t));

   // add some stuff
   for(i=0; i<10; i++) {
      val.data = i;
      BYTES_EQUAL(RingBufT_Success, ringBuf.Write(&val)); 
      BYTES_EQUAL(RingBufT_Success, ringBuf.Peek(&dest, i)); 
      CHECK_EQUAL(val.data, dest.data);
      CHECK_EQUAL(val.flags, dest.flags);
   }

   for(i=0; i<10; i++) {
      val.data = i;
      BYTES_EQUAL(RingBufT_Success, ringBuf.Read(&dest)); 
      CHECK_EQUAL(val.data, dest.data);
      CHECK_EQUAL(val.flags, dest.flags);
   }

   BYTES_EQUAL(RingBufT_IsEmpty, ringBuf.IsEmpty()); 
}
