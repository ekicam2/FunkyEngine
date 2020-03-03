#pragma once

#include "DebugMacros.h"
#include "LogMacros.h"
#include "Templates.h"

#include "IRenderingResource.h"

#include "Core/Memory/UniquePtr.h"

namespace Funky
{
	namespace Rendering
	{
		class RenderingResourcesManager final
		{
		public:
			RenderingResourcesManager()
			{

			}

			~RenderingResourcesManager()
			{
				FreeAll();
			}

			template <typename ResourceType>
			ResourceType* RegisterResource()
			{
				Resources.push_back(new ResourceType());
				return static_cast<ResourceType*>(Resources[Resources.size() - 1].Get());
			}

			template <typename ResourceType, typename... Args>
			ResourceType* RegisterResource(Args... args)
			{
				Resources.push_back(new ResourceType(Forward<Args>(args)...));
				return static_cast<ResourceType*>(Resources[Resources.size() - 1].Get());
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

			/** Not used at the moment. */
			void Free([[maybe_unused]] IRenderingResource* ResourceToDelete)
			{
			}

			void FreeAll()
			{
				Resources.clear();
				CHECK(Resources.size() == 0u);
			}
			

		private:
			Core::Memory::UniquePtr<RRenderTarget> SwapchainRT;
			Core::Memory::UniquePtr<RDepthStencil> SwapchainDS;

			struct ResourceList
			{
				ResourceList* Prev = nullptr;
				ResourceList* Next = nullptr;

				Core::Memory::UniquePtr<IRenderingResource> Resource;
			};

			size DebugAllocCounter = 0;
			//Core::Memory::UniquePtr<ResourceList> Resources;

			darray<Core::Memory::UniquePtr<IRenderingResource>> Resources;
		};

		//void RenderingResourcesManager::FreeAll()
		//{
		//	Resources.clear();
		//	CHECK(Resources.size() == 0u);

		//	/*ResourceList* CurrentNode = Resources.Get();
		//	while (CurrentNode)
		//	{
		//		if (CurrentNode->Next)
		//		{
		//			CurrentNode = CurrentNode->Next;
		//			delete CurrentNode->Prev;
		//			continue;
		//		}

		//		delete CurrentNode;
		//		CurrentNode = nullptr;
		//	}*/
		//}

		//RenderingResourcesManager::RenderingResourcesManager()
		////	: Resources(new ResourceList())
		//{

		//}

	}
}