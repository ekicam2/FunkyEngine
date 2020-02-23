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
			RenderingResourcesManager() = default;
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
				return reinterpret_cast<RRenderTarget*>(Resources[0].Get());
			}

			RDepthStencil* GetSwapchainDepthStencil()
			{
				return reinterpret_cast<RDepthStencil*>(Resources[1].Get());
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
			darray<Core::Memory::UniquePtr<IRenderingResource>> Resources;
		};
	}
}