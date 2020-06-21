#pragma once

#include "BasicTypes.h"

namespace Funky
{
	template<typename ...Args>
	using void_t = void;

	struct False
	{
		static constexpr bool Value = false;
	};

	struct True
	{
		static constexpr bool Value = true;
	};

	namespace TypeTraits
	{
		template <typename T> struct RemoveReference { using Type = T; };
		template <typename T> struct RemoveReference<T&> { using Type = T; };
		template <typename T> struct RemoveReference<T&&> { using Type = T; };

		/**
		 * This table should explain everything below :)
		 *
		 *	Object &  &  = Object &
		 *	Object &  && = Object &
		 *	Object && &  = Object &
		 *	Object && && = Object &&
		 */

		template <typename T, typename = void> struct AddReference{ using Lvalue = T; using Rvalue = T;};
		template <typename T> struct AddReference<T, void_t<T&>> { using Lvalue = T & ; using Rvalue = T && ; };


		template <typename T>
		struct IsArray : False {};

		template <typename T>
		struct IsArray<T[]> : True {};

		template <typename T, size N>
		struct IsArray<T[N]> : True {};

		template <typename T>
		struct RemoveArray
		{
			using Value = T;
		};

		template <typename T>
		struct RemoveArray<T[]>
		{
			using Value = T;
		};

		template <typename T, size N>
		struct RemoveArray<T[N]>
		{
			using Value = T;
		};

	}

	template <typename T> T&& Forward(typename TypeTraits::RemoveReference<T>::Type& In) { return (T&&)In; }
	template <typename T> T&& Forward(typename TypeTraits::RemoveReference<T>::Type&& In) { return (T&&)In; }
	
	template<class T>
	typename TypeTraits::AddReference<T>::Rvalue Declval() noexcept;

	template <typename T> typename TypeTraits::RemoveReference<T>::Type&& Move(T&& In) { return (typename TypeTraits::RemoveReference<T>::Type&&)(In); }
}