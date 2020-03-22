#pragma once

#include "Core/Containers.h"

#include "Core/Memory/UniquePtr.h"
#include "Core/Events/ICallable.h"

namespace Funky
{
	namespace Core
	{
		template <typename T>
		class Event;


		template <typename Ret, typename ...Args>
		class Event<Ret(Args...)>
		{
			darray<Core::Memory::UniquePtr<ICallable<Ret(Args...)>>> RegisteredList;
		public:

			template <typename Class>
			void RegisterMethod(Class* Object, typename Callable<Ret(Class::*)(Args...)>::MethodType&& Method)
			{
				RegisteredList.push_back(new Callable<Ret(Class::*)(Args...)>(Object, Method));
			}

			void RegisterFunction(typename Callable<Ret(Args...)>::FunctionType&& Function)
			{
				RegisteredList.push_back(new Callable<Ret(Args...)>(Function));
			}

			void RegisterLambda(typename Callable<Ret(Args...)>::FunctionType&& Function)
			{
				RegisteredList.push_back(new Callable<Ret(Args...)>(Function));
			}

			template<typename LAMBDA>
			void RegisterLambda(LAMBDA&& LambdaObj)
			{
				RegisteredList.push_back(new Callable<Ret(RemoveReference<LAMBDA>::Type::*)(Args...) const>(LambdaObj));
			}

			void Broadcast(Args ...args)
			{
				for (auto& Registered : RegisteredList)
				{
					Registered->operator ()(Forward<Args>(args)...);
				}
			}

		};
	}
}