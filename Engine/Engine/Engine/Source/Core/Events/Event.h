//#pragma once
//
//// This file is a "Hello, world!" in C++ language by GCC for wandbox.
//#include <iostream>
//#include <cstdlib>
//#include <vector>
//
//template<typename T, typename... Args>
//struct Callable
//{
//	virtual ~Callable() = default;
//	virtual T operator()(Args... args) = 0;
//};
//
//
//template<typename T>
//struct FunctionWrapper;
//
//// methods, and capture lambdas
//template<typename T, typename J, typename... Args>
//struct FunctionWrapper<T(J::*)(Args...)> final : Callable<T, Args...>
//{
//	// you should use one group at the time
//
//	// group 1 for methods
//	using MethodRet = T;
//	using Class = J;
//	using MethodPointer = MethodRet(Class::*)(Args...);
//
//	// group 2 for capturing lambdas
//	using Lambda = T;
//	using LambdaRet = J;
//
//	MethodPointer Method;
//	Class* Instance;
//
//	FunctionWrapper(MethodPointer const& func, J* const& inst)
//		: Method(func), Instance(inst)
//	{
//	}
//
//	FunctionWrapper(FunctionWrapper const& Other) = delete;
//	//todo delete copy assignment as well
//
//	FunctionWrapper(FunctionWrapper&& Other)
//	{
//		this->Method = Other.Method;
//		this->Instance = Other.Instance;
//
//		Other.Method = nullptr;
//		Other.Instance = nullptr;
//	}
//
//	template <typename Lambda>
//	FunctionWrapper(Lambda CapturingLambda)
//	{
//		Instance = CapturingLambda;
//
//	}
//
//	T operator()(Args... args) { return Call(args...); }
//
//	T Call(Args... args)
//	{
//		return (Instance->*Method)(args...);
//	}
//};
//
//// (free) function / specific lambdas
//template<typename T, typename... Args>
//struct FunctionWrapper<T(*)(Args...)> final : Callable<T, Args...>
//{
//	using MethodPointer = T(*)(Args...);
//
//	MethodPointer Method;
//
//	FunctionWrapper(MethodPointer const& func)
//		: Method(func)
//	{
//	}
//
//	FunctionWrapper(FunctionWrapper const& Other)
//	{
//		Method = Other.Func;
//	}
//
//	FunctionWrapper(FunctionWrapper&& Other)
//	{
//		this->Method = Other.Func;
//
//		Other.Func = nullptr;
//	}
//
//	T operator()(Args... args) { return Call(args...); }
//
//	T Call(Args... args)
//	{
//		return (*Method)(args...);
//	}
//};
//
//
//template<typename T>
//struct Delegate;
//
//template<typename T, typename... Args>
//struct Delegate<T(Args...)> final
//{
//	using RawFuncType = T(*)(Args...);
//	template<typename J>
//	using MethodType = T(J::*)(Args...);
//
//	std::vector<Core::Memory::UniquePtr<Callable<T, Args...>>> InvocationList;
//
//	void Bind(RawFuncType func)
//	{
//		InvocationList.push_back(new FunctionWrapper<RawFuncType>(func));
//	}
//
//	void Bind(FunctionWrapper<RawFuncType>&& func)
//	{
//		InvocationList.push_back(new FunctionWrapper<RawFuncType>(func));
//	}
//
//	template<typename J>
//	void Bind(FunctionWrapper<MethodType<J>(Args...)>&& func)
//	{
//		InvocationList.push_back(new FunctionWrapper<MethodType>(func));
//	}
//
//	void SafeCall(Args... args)
//	{
//		for (auto& func : InvocationList)
//		{
//			if (func != nullptr)
//			{
//				(*func)(args...);
//			}
//		}
//	}
//};
//
//// struct Dog {
//// 	void Bark() { std::cout << "hau hau" << "\n"; }
//// 	void Func(bool ib, int ii, char ic) { std::cout << "hau, hau hau hau" << "\n" << (ib ? "true" : "false") << " " << ii << " " << ic << "\n"; }
//// };
//// 
//// 
//// void func(bool ib, int ii, char ic) { std::cout << "ból, int i czar" << "\n" << (ib ? "true" : "false") << " " << ii << " " << ic << "\n"; }
//// 
//// int main()
//// {
//// 	Dog doggy;
//// 
//// 	Delegate<void(void)> delegates;
//// 	delegates.Bind<Dog>(FunctionWrapper<void(Dog::*)()>(&Dog::Bark, &doggy));
//// 	delegates.SafeCall();
//// 
//// 	Delegate<void(bool, int, char)> delegates2;
//// 	delegates2.Bind(&func);
//// 	delegates2.Bind([](bool ib, int ii, char ic) {std::cout << "labdababda \n ból, int i czar" << "\n" << (ib ? "true" : "false") << " " << ii << " " << ic << "\n"; });
//// 	delegates2.Bind<Dog>(FunctionWrapper<void(Dog::*)(bool, int, char)>(&Dog::Func, &doggy));
//// 	delegates2.SafeCall(true, 213, 'h');
//// 
//// 	std::cout << "\n\tit works" << std::endl;
//// 
//// 	asm("int $3");
//// }