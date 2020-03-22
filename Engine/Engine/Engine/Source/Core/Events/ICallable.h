#pragma once

#include "Templates.h"

namespace Funky
{
	namespace Core
	{
		template <typename T>
		class ICallable;

		template <typename Ret, typename... Args>
		class ICallable<Ret(Args...)>
		{
		public:
			virtual Ret operator()(Args...args) = 0;
		};

		template <typename T>
		class Callable;

		template <typename Ret, typename... Args>
		class Callable<Ret(Args...)> : public ICallable<Ret(Args...)>
		{

		public:
			using FunctionType = Ret(*)(Args...);
			Callable(FunctionType&& InFunc) : Func(InFunc) {}

			Ret operator()(Args ...args)
			{
				if (Func) return Func(Forward<Args>(args)...);
				return Ret(0);
			}
		private:
			FunctionType Func = nullptr;
		};

		template <typename Class, typename Ret, typename... Args>
		class Callable<Ret(Class::*)(Args...)> : public ICallable<Ret(Args...)>
		{
		public:
			using MethodType = Ret(Class::*)(Args...);

			Callable(Class* InObject, MethodType&& InMethod) : Object(InObject), Func(InMethod) {}
			Callable(Class&& InObject) : Object(&InObject), Func(&decltype(InObject)::operator()) {}
			Callable(Class& InObject) : Object(&InObject), Func(&decltype(InObject)::operator()) {}

			Ret operator()(Args... args)
			{
				if (Object && Func) return (Object->*Func)(Forward<Args>(args)...);
				return Ret(0);
			}

		private:
			Class* Object = nullptr;
			MethodType Func = nullptr;
		};


		template <typename Class, typename Ret, typename... Args>
		class Callable<Ret(Class::*)(Args...) const> : public ICallable<Ret(Args...)>
		{

			Core::Memory::UniquePtr<Class> Object;
			Ret(Class::* Func)(Args...)const = nullptr;
		public:
			//Callable(Class&& InObject) : Object(&InObject), Func(decltype(InObject)::operator()) {}
			Callable(Class InObject) : Object(new Class(InObject)), Func(&decltype(InObject)::operator()) {}

			Ret operator()(Args... args)
			{
				if (Object && Func) return (Object->*Func)(Forward<Args>(args)...);
				return Ret(0);
			}
		};
	}
}