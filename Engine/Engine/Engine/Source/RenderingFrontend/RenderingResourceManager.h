#pragma once

#include "DebugMacros.h"
#include "LogMacros.h"
#include "Templates.h"

#include "IRenderingResource.h"

#include "Core/Memory/UniquePtr.h"
#include "Core/Containers.h"


#define RENDER_RESOURCE_TYPE(Name) R##Name
#define STORAGE_TYPE(Type) AssetStorage<RENDER_RESOURCE_TYPE(Type)>
#define RENDER_RESOURCE_MEMBER(Name) Name##s
#define RENDER_RESOURCE_FREE_TRACKER(Name) Free##Name##s

#define IMPLEMENT_RENDERING_RESOURCE(Name)									\
			STORAGE_TYPE(Name)	RENDER_RESOURCE_MEMBER(Name);				\
																			\
			template <>														\
			STORAGE_TYPE(Name)& GetBuffer<RENDER_RESOURCE_TYPE(Name)>()		\
			{																\
				return RENDER_RESOURCE_MEMBER(Name);						\
			}																\
																			\
			darray<size> RENDER_RESOURCE_FREE_TRACKER(Name);				\
																			\
			template <>														\
			darray<size>& GetFreeTracker<RENDER_RESOURCE_TYPE(Name)>()		\
			{																\
				return RENDER_RESOURCE_FREE_TRACKER(Name);					\
			}															

namespace Funky
{
	namespace Rendering
	{
		class RenderingResourcesManager final
		{
		public:
			template<typename R>
			using AssetStorage = darray<Core::Memory::UniquePtr<R>>;

			RenderingResourcesManager()
			{
			}

			~RenderingResourcesManager()
			{
				FreeAll();
			}

			RRenderTarget* GetSwapchainRenderTarget()
			{
				return SwapchainRT.Get();
			}

			RDepthStencil* GetSwapchainDepthStencil()
			{
				return SwapchainDS.Get();
			}

			void ReleaseSwapchain()
			{
				SwapchainRT.Reset();
				SwapchainDS.Reset();
			}

			void AssociateSwapchain(RRenderTarget* RT, RDepthStencil* DS)
			{
				SwapchainRT.Reset(RT);
				SwapchainDS.Reset(DS);
			}

			void FreeAll()
			{
				ReleaseSwapchain();

				RenderTargets.clear();
				DepthStencils.clear();
				Buffers.clear();
				Shaders.clear();
				Textures.clear();
			}

			//template<typename T, typename... Args>
			//Resource::ID CreateResource(Args... args)
			//{
			//	auto resourceBuffer = GetBuffer<T>();

			//	auto feeBuffer	= GetFreeTracker<T>();
			//	auto foundLast = feeBuffer.end();
			//	if (foundLast != feeBuffer.end())
			//	{
			//		size id = *foundLast;
			//		feeBuffer.pop_back();
			//		resourceBuffer[id].Reset(Core::Memory::UniquePtr<T>(new T(Forward<Args>(args)...)));

			//		// XD
			//		return id;
			//	}

			//	resourceBuffer.push_back(Core::Memory::UniquePtr<T>(new T(Forward<Args>(args)...)));
			//	return resourceBuffer.size();
			//}


			template<typename T>
			Resource::ID CreateResource(T* inResource)
			{
				auto& resourceBuffer = GetBuffer<T>();

				auto& feeBuffer = GetFreeTracker<T>();
				auto foundLast = feeBuffer.end();
				if (foundLast != feeBuffer.end())
				{
					size id = *foundLast;
					feeBuffer.pop_back();
					resourceBuffer[id].Reset(Core::Memory::UniquePtr<T>(inResource));

					// XD
					return id;
				}

				resourceBuffer.push_back(Core::Memory::UniquePtr<T>(inResource));
				return resourceBuffer.size() - 1;
			}
			
			template <typename T>
			void FreeResource(Resource::ID id)
			{
				GetBuffer<T>()[id].Reset();
				GetFreeTracker<T>().push_back(id);
			}

			template <typename T>
			T* GetResource(Resource::ID id)
			{
				return GetBuffer<T>()[id];
			}

		private:
			Core::Memory::UniquePtr<RRenderTarget> SwapchainRT;
			Core::Memory::UniquePtr<RDepthStencil> SwapchainDS;

			template <typename T>
			AssetStorage<T>& GetBuffer()
			{
				static_assert(false, "GetBuffer() not overloaded");
			}	

			template <typename T>
			darray<size>& GetFreeTracker()
			{
				static_assert(false, "GetFreeTracker() not overloaded");
			}

			IMPLEMENT_RENDERING_RESOURCE(RenderTarget)
			IMPLEMENT_RENDERING_RESOURCE(DepthStencil)
			IMPLEMENT_RENDERING_RESOURCE(Buffer)
			IMPLEMENT_RENDERING_RESOURCE(Shader)
			IMPLEMENT_RENDERING_RESOURCE(Texture)
		};

	}
}