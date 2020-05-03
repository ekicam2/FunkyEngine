#pragma once

#include "Templates.h"

namespace Funky
{
	namespace Core
	{
		namespace Memory
		{
			template <typename ObjectType, bool ThreadSafe = false>
			class UniquePtr
			{
			public:
				using PtrType = typename TypeTraits::RemoveArray<ObjectType>::Value*;

				UniquePtr(PtrType NewObject = nullptr);
				~UniquePtr();
				
				UniquePtr(UniquePtr&& Other);
				UniquePtr(UniquePtr const&) = delete;
				UniquePtr& operator=(UniquePtr const&) = delete;
				UniquePtr& operator=(UniquePtr&& Other);
				template <typename Derived, typename... Args>
				void Reset(Args... args);
				void Reset(PtrType NewObject = nullptr);
				PtrType const Get() const;
				operator PtrType ();
				operator PtrType () const;
				PtrType operator->();

			protected:
				void Free();
			private:
				PtrType Object;
			};

			template <typename ObjectType, bool ThreadSafe /*= false*/>
			UniquePtr<ObjectType, ThreadSafe>::UniquePtr(PtrType NewObject /*= nullptr*/)
				: Object(NewObject)
			{

			}

			template <typename ObjectType, bool ThreadSafe /*= false*/>
			UniquePtr<ObjectType, ThreadSafe>::UniquePtr(UniquePtr&& Other)
				: Object(Other.Object)
			{
				Other.Object = nullptr;
			}

			template <typename ObjectType, bool ThreadSafe /*= false*/>
			UniquePtr<ObjectType, ThreadSafe>::~UniquePtr()
			{
				Free();
			}

			template <typename ObjectType, bool ThreadSafe /*= false*/>
			UniquePtr<ObjectType, ThreadSafe>::operator PtrType()
			{
				return Get();
			}

			template <typename ObjectType, bool ThreadSafe /*= false*/>
			UniquePtr<ObjectType, ThreadSafe>::operator PtrType() const
			{
				return Get();
			}

			template <typename ObjectType, bool ThreadSafe /*= false*/>
			typename UniquePtr<ObjectType, ThreadSafe>::PtrType UniquePtr<ObjectType, ThreadSafe>::operator->()
			{
				return Object;
			}


			template <typename ObjectType, bool ThreadSafe /*= false*/>
			Funky::Core::Memory::UniquePtr<ObjectType, ThreadSafe>& Funky::Core::Memory::UniquePtr<ObjectType, ThreadSafe>::operator=(UniquePtr&& Other)
			{
				Object = Other.Object;
				Other.Object = nullptr;

				return *this;
			}


			template <typename ObjectType, bool ThreadSafe /*= false*/>
			template<typename Derived, typename... Args>
			void UniquePtr<ObjectType, ThreadSafe>::Reset(Args... args)
			{
				Free();
				Object = new Derived(Forward<Args>(args)...);
			}


			template <typename ObjectType, bool ThreadSafe /*= false*/>
			void Funky::Core::Memory::UniquePtr<ObjectType, ThreadSafe>::Reset(PtrType NewObject /*= nullptr*/)
			{
				Free();
				Object = NewObject;
			}

			template <typename ObjectType, bool ThreadSafe /*= false*/>
			typename UniquePtr<ObjectType, ThreadSafe>::PtrType const UniquePtr<ObjectType, ThreadSafe>::Get() const
			{
				return Object;
			}

			template <typename ObjectType, bool ThreadSafe /*= false*/>
			void UniquePtr<ObjectType, ThreadSafe>::Free()
			{
				using namespace TypeTraits;

				if (IsArray<PtrType>::Value)
					delete[] Object;
				else
					delete Object;
			}
		}
	}
}