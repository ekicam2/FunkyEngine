#pragma once

#include <new.h>

#include "Memory.h"
#include "BasicTypes.h"

#include "Math/MathGlobals.h"

namespace Funky
{
	namespace Core
	{
		namespace Memory
		{
			/** 
			 * Memory Pool is an object within whole allocated memory resides.
			 * The main reason for introducing such a class is to provide an easy interface for memory management, in a controlled manner.
			 * 
			 * Memory Pool is not responsible for memory allocation at all. 
			 *
			 * @warning At the moment Allocation doesn't implement alignment at all
			 * 
			 * @sa
			 * Memory::MemoryPool::Bucket and Memory::MemoryPool::Bucket::ChunkDesc
			 */
			class MemoryPool
			{
			public:

				/** Try to allocate an object. Returns requested object if succeed, nullptr otherwise.*/
				template <typename T>
				T* Alloc();

				/** Try to allocate an object. And forward arguments to the constructor. Returns requested object if succeed, nullptr otherwise.*/
				template <typename T, typename ... Args>
				T* Alloc(Args...);

				/** Try to allocate an undefined block of memory. Returns pointer to the beginning of the block if succeed, nullptr otherwise. @param Size in bytes */
				void* Alloc(size Size);

				/** Try to initialize memory pool return true if succeed, false otherwise
				  * 
				  * @param BucketsSize size per Bucket in Bytes
				  */
				bool InitMemoryPool(u64 BucketsSize);

				void Free(void* DataToFree);


			private:
				/**
				 * Bucket structure is responsible for allocating memory from OS, and then creating and freeing objects inside already allocated memory buffer.
				 * \sa
				 * Memory::MemoryPool and Memory::MemoryPool::Bucket::ChunkDesc
				 */
				struct Bucket final
				{
					/**
					 * Header structure is being placed inside a Bucket just before real object.
					 * \sa 
					 * Memory::MemoryPool and Memory::MemoryPool::Bucket
					 */
					struct ChunkDesc
					{
						/** Size of the object allocated in bytes */
						size Size;
						/** Tells if Data pointer is pointing to a valid data segment */
						bool bUsed;
						/** Data segment associated with Header */
						byte* Data;
						/** Previous ChunkDescriptor if nullptr current is the first one */
						ChunkDesc* Prev;
						/** Following CunkDescriptor if nullptr current is the last one */
						ChunkDesc* Next;
					};

					static constexpr size ChunkDescSize = sizeof(ChunkDesc);

					/** Start address of Bucket memory, always points to a valid ChunkDesc object */
					byte* Begin;
					/** End address of Bucket memory */
					byte* End;
					/** Size of a total allocated memory size */
					size Size;
					/** Size of an actually used memory size inside the Bucket */
					size UsedSpace;

					explicit Bucket()
						: Begin(nullptr)
						, End(nullptr)
						, Size(0)
						, UsedSpace(0)
					{
					}

					Bucket(size InSize)
						: Bucket()
					{
						byte* Temp = new (std::nothrow) byte[InSize];
						if (Temp != nullptr)
						{
							Begin = Temp;
							Size = InSize;
							End = Begin + Size;

							UsedSpace = Bucket::ChunkDescSize;

							ChunkDesc* InitChunk = (ChunkDesc*)Begin;
							ZeroMem(InitChunk, Bucket::ChunkDescSize);
							InitChunk->bUsed = false;
							InitChunk->Data = (byte*)InitChunk + Bucket::ChunkDescSize;
							InitChunk->Size = Size - UsedSpace;
						}
					}

					~Bucket()
					{
						delete[] Begin;
					}



#ifdef GetFreeSpace
#undef GetFreeSpace
#endif
					/** Returns TOTAL free space, may be fragmented */
					FORCEINLINE size GetFreeSpace() const { return Size - UsedSpace; }

					/** Try to allocate an undefined block of memory. Returns pointer to the beginning of the block if succeed, nullptr otherwise.*/
					void* TryAlloc(size Size);
					/** Free used memory. @warning Doesn't turn memory back to the OS!*/
					void Free(byte* Data);
				};

				using BucketsPool = Bucket**;
				static constexpr size MaxBucketsAvailable= 512u;
				size BucketSize = 0;
				
				BucketsPool Buckets = nullptr;
			};
		}
	}
}

template <typename T>
T* Funky::Core::Memory::MemoryPool::Alloc()
{
	return (T*)Alloc(sizeof(T));
}


template <typename T, typename ... Args>
T* Funky::Core::Memory::MemoryPool::Alloc(Args... _Args)
{
	T* Temp = (T*)Alloc(sizeof(T));
	if (Temp) new (Temp) T(std::forward<Args>(Args)...);

	return Temp;
}

void* Funky::Core::Memory::MemoryPool::Alloc(size Size)
{
	Bucket* CurrentBucket = Buckets[0];

	do
	{
		if (CurrentBucket->GetFreeSpace() >= Size)
		{
			// even if in the bucket seems to have enough free space,
			// there still may be too little inline free space for our object due to defragmentation
			void* Temp = CurrentBucket->TryAlloc(Size);
			if (Temp != nullptr) return Temp;
		}

		++CurrentBucket;
	} while (CurrentBucket);

	return nullptr;
}

bool Funky::Core::Memory::MemoryPool::InitMemoryPool(u64 InBucketsSize)
{
	Buckets = new (std::nothrow) Bucket*[MemoryPool::MaxBucketsAvailable];
	MemSet(Buckets, sizeof(Bucket*) * MemoryPool::MaxBucketsAvailable, NULL);

	if (!Buckets) return false;
	BucketSize = InBucketsSize;

	Buckets[0] = new (std::nothrow) Bucket(BucketSize);
	if (!Buckets[0]) return false;

	return true;
}

void Funky::Core::Memory::MemoryPool::Free(void* DataToFree)
{
	if (DataToFree == nullptr)
		return;

	for (int i =0; i < MaxBucketsAvailable; ++i)
	{
		Bucket* SingleBucket = Buckets[i];

		if (DataToFree > SingleBucket->Begin && DataToFree < SingleBucket->End)
		{
			SingleBucket->Free((byte*)DataToFree);
			break;
		}
	}
}

void* Funky::Core::Memory::MemoryPool::Bucket::TryAlloc(size InSize)
{
	ChunkDesc* CurrentBlob = (ChunkDesc*)Begin;

	do 
	{
		if (!CurrentBlob->bUsed && CurrentBlob->Size >= InSize)
		{
			CurrentBlob->bUsed = true;
			UsedSpace += InSize;
			
			const byte* NewDescPosition = (CurrentBlob->Data + InSize);

			const bool bHasEnoughSpaceBeforeEnd	 = NewDescPosition < End && NewDescPosition + Bucket::ChunkDescSize < End;
			const bool bHasEnoughSpaceBeforeNext = CurrentBlob->Next ? NewDescPosition < (byte*)CurrentBlob->Next && NewDescPosition + Bucket::ChunkDescSize < (byte*)CurrentBlob->Next : true;

			if (bHasEnoughSpaceBeforeEnd && bHasEnoughSpaceBeforeNext)
			{
				ZeroMem((void*)NewDescPosition, Bucket::ChunkDescSize);
				((ChunkDesc*)NewDescPosition)->bUsed = false;
				((ChunkDesc*)NewDescPosition)->Data = (byte*)(NewDescPosition + Bucket::ChunkDescSize);
				((ChunkDesc*)NewDescPosition)->Next = CurrentBlob->Next;
				((ChunkDesc*)NewDescPosition)->Prev = CurrentBlob;
				((ChunkDesc*)NewDescPosition)->Size = (CurrentBlob->Next ? (byte*)CurrentBlob->Next : End) - (((ChunkDesc*)NewDescPosition)->Data);


				// Update only if we could place CunkDesc after, otherwise we could lost track of some bytes
				CurrentBlob->Size = InSize;
				CurrentBlob->Next = ((ChunkDesc*)NewDescPosition);

				UsedSpace += Bucket::ChunkDescSize;
			}

			return CurrentBlob->Data;
		}

		CurrentBlob = CurrentBlob->Next;
	} while (CurrentBlob);

	return nullptr;
}

void Funky::Core::Memory::MemoryPool::Bucket::Free(byte* Data)
{
	bool bPrevUnused = false;
	bool bNextUnused = false;

	ChunkDesc* MyDesc = (ChunkDesc*)(Data - Bucket::ChunkDescSize);

	MyDesc->bUsed = false; // mark unused
	UsedSpace -= MyDesc->Size;

	bPrevUnused = MyDesc->Prev ? !MyDesc->Prev->bUsed : false;
	bNextUnused = MyDesc->Next ? !MyDesc->Next->bUsed : false;

	// merge with next and it's descriptor if it's unused
	if (bNextUnused)
	{
		MyDesc->Size += Bucket::ChunkDescSize + MyDesc->Next->Size;
		MyDesc->Next = MyDesc->Next->Next;
		UsedSpace -= Bucket::ChunkDescSize;
	}

	if (bPrevUnused)
	{
		MyDesc->Prev->Size += MyDesc->Size;
		// remember to mark my descriptor as garbare!
		MyDesc->Prev->Size += Bucket::ChunkDescSize;
		MyDesc->Prev->Next = MyDesc->Next;
		UsedSpace -= Bucket::ChunkDescSize;
	}
}