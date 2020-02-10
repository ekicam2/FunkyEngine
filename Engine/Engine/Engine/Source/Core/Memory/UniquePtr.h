#pragma once

namespace Funky
{
	namespace Core
	{
		namespace Memory
		{
			template <typename PtrType, bool ThreadSafe = false>
			class UniquePtr
			{
			public:
				UniquePtr(PtrType* NewObject = nullptr);
				~UniquePtr();
				
				UniquePtr(UniquePtr&&) = delete;
				UniquePtr(UniquePtr const&) = delete;
				UniquePtr& operator=(UniquePtr const&) = delete;
				UniquePtr& operator=(UniquePtr&&) = delete;

				void Reset(PtrType* NewObject = nullptr);
				PtrType* const Get() const;
				operator PtrType* ();
				PtrType* operator->();

			protected:
				void Free();
			private:
				PtrType* Object;
			};

			template <typename PtrType, bool ThreadSafe /*= false*/>
			Funky::Core::Memory::UniquePtr<PtrType, ThreadSafe>::UniquePtr(PtrType* NewObject /*= nullptr*/)
				: Object(NewObject)
			{

			}

			template <typename PtrType, bool ThreadSafe /*= false*/>
			Funky::Core::Memory::UniquePtr<PtrType, ThreadSafe>::~UniquePtr()
			{
				Free();
			}

			template <typename PtrType, bool ThreadSafe /*= false*/>
			Funky::Core::Memory::UniquePtr<PtrType, ThreadSafe>::operator PtrType* ()
			{
				return Get();
			}

			template <typename PtrType, bool ThreadSafe /*= false*/>
			PtrType* Funky::Core::Memory::UniquePtr<PtrType, ThreadSafe>::operator->()
			{
				return Object;
			}

			template <typename PtrType, bool ThreadSafe /*= false*/>
			void Funky::Core::Memory::UniquePtr<PtrType, ThreadSafe>::Reset(PtrType* NewObject /*= nullptr*/)
			{
				Free();
				Object = NewObject;
			}

			template <typename PtrType, bool ThreadSafe /*= false*/>
			PtrType * const Funky::Core::Memory::UniquePtr<PtrType, ThreadSafe>::Get() const
			{
				return Object;
			}

			template <typename PtrType, bool ThreadSafe /*= false*/>
			void Funky::Core::Memory::UniquePtr<PtrType, ThreadSafe>::Free()
			{
				delete Object;
			}
		}
	}
}