#include "FunkyEngine.h"

#include "Rendering/RawMesh.h"
#include "Rendering/Camera.h"
#include "Rendering/Light.h"
#include "Rendering/Texture.h"

#include <fstream>
#include <iterator>
#include <vector>
#include <locale>

#define MEMORY_TEST

#ifdef MEMORY_TEST

#include "Core/Memory/MemoryPool.h"


struct Tester10MB
{
	byte __unused[10 * 1024 * 1024];
};
static_assert(sizeof(Tester10MB) == 10 * 1024 * 1024);

int main()
{
	BREAK();

	Funky::Core::Memory::MemoryPool MP;
	if (!MP.InitMemoryPool(512 * 1024 * 1024))
		BREAK();
	BREAK();


	Tester10MB* pTester = MP.Alloc<Tester10MB>();
	(void)pTester;
	BREAK();


	i32 allocations = std::rand() % 100;
	std::vector<void*> AllocatedDumbs;
	for (i32 i = 0; i < allocations; ++i)
	{
		AllocatedDumbs.push_back((void*)MP.Alloc<Tester10MB>());
	}

	do
	{
		i32 randomID = std::rand() % AllocatedDumbs.size();
		MP.Free(AllocatedDumbs[randomID]);
	} while (AllocatedDumbs.size() == 0);

	MP.Free(pTester);
	BREAK();

	return 0;
	
}
#else 
int main()
{
	Funky::FunkyEngine Engine;
	if (Engine.Init())
		Engine.Run();

	Engine.Shutdown();

	return 0;
}
#endif // 



	// Lighting
// 
// 	DEFINE_CONSTANT_BUFFER_BEGIN(ConstantBufferLights)
// 		Light PointLight;
// 	DEFINE_CONSTANT_BUFFER_END(ConstantBufferLights)
// 
// 
// 	ConstantBufferLights LightingConstantBufferData;
// 	LightingConstantBufferData.PointLight.Color = Funky::Math::LinearColor(0.0f, 1.0f, 0.0f, 1.0f);
// 	LightingConstantBufferData.PointLight.Position = Funky::Math::Vector3f(2.0f, 25.0f, -10.0f);
// 
// 	Microsoft::WRL::ComPtr<ID3D11Buffer> LightingConstantBuffer;
// 
// 	D3D11_BUFFER_DESC LightBufferDesc;
// 	ZeroMemory(&LightBufferDesc, sizeof(D3D11_BUFFER_DESC));
// 	LightBufferDesc.ByteWidth = sizeof(ConstantBufferLights);
// 	LightBufferDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER;
// 	LightBufferDesc.Usage = D3D11_USAGE::D3D11_USAGE_IMMUTABLE;
// 
// 	D3D11_SUBRESOURCE_DATA LightBufferData;
// 	ZeroMemory(&LightBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
// 	LightBufferData.pSysMem = &LightingConstantBufferData;
// 
// 	hr = pDevice->CreateBuffer(&LightBufferDesc, &LightBufferData, LightingConstantBuffer.GetAddressOf());
// 	ASSERT(SUCCEEDED(hr), L"Couldn't create light constant buffer");
// 
// 	// Lighting END
// 
// 
// 	// Scene creation
// 
// 	struct RenderObject
// 	{
// 		Funky::VertexShaderInfo* m_pVertexShader = nullptr;
// 		ID3D11PixelShader* m_pPixelShader = nullptr;
// 
// 		std::vector<TextureBase*> textures;
// 		RawMesh* m_pMesh = nullptr;
// 
// 		Funky::Math::Vector3f m_pPosition = Funky::Math::Vector3f(0.0f, 0.0f, 0.0f);
// 		Funky::Math::Vector3f m_pScale = Funky::Math::Vector3f(1.0f, 1.0f, 1.0f);
// 		Funky::Math::Vector3f m_pRotation = Funky::Math::Vector3f(0.0f, 0.0f, 0.0f);
// 
// 		void BindTextures(ID3D11DeviceContext* pDeviceContext)
// 		{
// 			if (unsigned texCount = textures.size())
// 			{
// 				std::vector<ID3D11ShaderResourceView*> Resources;
// 				Resources.reserve(texCount);
// 
// 				for (auto& tex : textures)
// 				{
// 					Resources.push_back(tex->m_pTextureView.Get());
// 				}
// 
// 				pDeviceContext->PSSetShaderResources(0, texCount, &Resources[0]);
// 			}
// 		}
// 
// 		void BindShaders(ID3D11DeviceContext* pDeviceContext)
// 		{
// 			pDeviceContext->IASetInputLayout(m_pVertexShader->InputLayout.Get());
// 			pDeviceContext->VSSetShader(m_pVertexShader->VertexShader.Get(), nullptr, 0);
// 			pDeviceContext->PSSetShader(m_pPixelShader, nullptr, 0);
// 		}
// 
// 		void BindMesh(ID3D11DeviceContext* pDeviceContext)
// 		{
// 			m_pMesh->Bind(pDeviceContext);
// 		}
// 
// 		bool WindowOpen = false;
// 		float Position[3] = { m_pPosition.x, m_pPosition.y, m_pPosition.z };
// 		float Rotation[3] = { m_pRotation.x, m_pRotation.y, m_pRotation.z };
// 		float Scale[3] = { m_pScale.x, m_pScale.y, m_pScale.z };
// 
// 		void CreateDetailsWindow()
// 		{
// 			WindowOpen = true;
// 
// 			m_pPosition = Position;
// 			m_pRotation = Rotation;
// 			m_pScale = Scale;
// 
// 			ImGuiWindowFlags WindowFlags = 0;
//			WindowFlags |= ImGuiWindowFlags_NoCollapse;
// 			ImGui::Begin("RenderObject Details", &WindowOpen, WindowFlags);
// 			ImGui::SliderFloat3("Position", Position, -1000.0f, 1000.0f);
// 			ImGui::SliderFloat3("Rotation", Rotation, -1000.0f, 1000.0f);
// 			ImGui::SliderFloat3("Scale", Scale, 1.0f, 1000.0f);
// 			ImGui::End();
// 		}
// 	};
// 
// 	Texture2D* MainTexture = Texture2D::CreateFromFile("Resource/Models/robot/walker_color22.png", pDevice.Get());
// 	ASSERT(MainTexture, L"Couldn't create Main Texture");
// 
// 	char const* textures[] = {
// 		"Resource/Textures/mp_troubled/troubled-waters_rt.tga",
// 		"Resource/Textures/mp_troubled/troubled-waters_lf.tga",
// 		"Resource/Textures/mp_troubled/troubled-waters_up.tga",
// 		"Resource/Textures/mp_troubled/troubled-waters_dn.tga",
// 		"Resource/Textures/mp_troubled/troubled-waters_bk.tga",
// 		"Resource/Textures/mp_troubled/troubled-waters_ft.tga"
// 	};
// 
// 	CubemapTexture* SkyTexture = CubemapTexture::CreateFromFile(textures, pDevice.Get());
// 	ASSERT(SkyTexture, L"Couldn't create Main Texture");
// 
// 	RawMesh* OBJMesh = MeshUtils::LoadOBJFromFile(pDevice.Get(), "C:/Users/Maciej Dro¿d¿/Desktop/Engine/Engine/Engine/Engine/Build/x64/Debug/Resource/Models/robot/robot.obj");
// 	RawMesh* Terrain = MeshUtils::CreateTerrainPlane(pDevice.Get(), 5, 5, 5.0f);
// 	float whiteColor[] = { 1.0f, 1.0f, 1.0f };
// 	RawMesh* WhiteCube = MeshUtils::CreateCube(pDevice.Get(), whiteColor);
// 	RawMesh* SkySphere = MeshUtils::CreateSphere(pDevice.Get(), 49.9f, true);
// 
// 	RenderObject RobotObject;
// 	RobotObject.m_pMesh = OBJMesh;
// 	RobotObject.textures.push_back(MainTexture);
// 	RobotObject.textures.push_back(SkyTexture);
// 	RobotObject.m_pVertexShader = MaterialsManager.GetOrCreateVertexShader("AdvancedVertexShader", DirectUtils::InputLayout::PositionColorNormalUV, pDevice.Get());
// 	RobotObject.m_pPixelShader = MaterialsManager.GetOrCreatePixelShader("AdvancedPixelShader", pDevice.Get());
// 
// 	RenderObject TerrainObject;
// 	TerrainObject.m_pMesh = Terrain;
// 	TerrainObject.m_pVertexShader = MaterialsManager.GetOrCreateVertexShader("SampleVertexShader", DirectUtils::InputLayout::PositionColor, pDevice.Get());
// 	TerrainObject.m_pPixelShader = MaterialsManager.GetOrCreatePixelShader("TerrainPixelShader", pDevice.Get());
// 
// 	RenderObject LightDebug;
// 	LightDebug.m_pMesh = WhiteCube;
// 	LightDebug.m_pVertexShader = MaterialsManager.GetOrCreateVertexShader("SampleVertexShader", DirectUtils::InputLayout::PositionColor, pDevice.Get());
// 	LightDebug.m_pPixelShader = MaterialsManager.GetOrCreatePixelShader("SamplePixelShader", pDevice.Get());
// 	LightDebug.m_pPosition = LightingConstantBufferData.PointLight.Position;
// 
// 	RenderObject SkyBox;
// 	SkyBox.m_pScale = Funky::Math::Vector3f(200.0f, 200.0f, 200.0f);
// 	SkyBox.Scale[0] = 200.0f;
// 	SkyBox.Scale[1] = 200.0f;
// 	SkyBox.Scale[2] = 200.0f;
// 	SkyBox.m_pMesh = SkySphere;
// 	SkyBox.textures.push_back(SkyTexture);
// 	SkyBox.m_pVertexShader = MaterialsManager.GetOrCreateVertexShader("SkyVertexShader", DirectUtils::InputLayout::PositionColorUV, pDevice.Get());
// 	SkyBox.m_pPixelShader = MaterialsManager.GetOrCreatePixelShader("SkyPixelShader", pDevice.Get());
// 
// 	RenderObject* RenderScene[] = { &RobotObject, &TerrainObject, &LightDebug, &SkyBox };
// 	Camera SceneCamera(aspectRatio, 75.0f, 0.01f, 100000.0f);

	// Scene creation END
// 
// void handleIO(HWND hwnd, bool& bPrevFrameRMB, Camera& SceneCamera)
// {
// 	constexpr unsigned char PressedMask = 1 << 7;
// 
// 	if (PressedMask & GetKeyState('W'))
// 		SceneCamera.Translate({ 0.0f, 0.0f, 0.5f });
// 	if (PressedMask & GetKeyState('S'))
// 		SceneCamera.Translate({ 0.0f, 0.0f, -0.5f });
// 	if (PressedMask & GetKeyState('A'))
// 		SceneCamera.Translate({ -0.5f, 0.0f, 0.0f });
// 	if (PressedMask & GetKeyState('D'))
// 		SceneCamera.Translate({ 0.5f, 0.0f, 0.0f });
// 
// 	if (PressedMask & GetKeyState(VK_RBUTTON))
// 	{
// 		RECT WindowRect = { 0 };
// 		GetWindowRect(hwnd, &WindowRect); //get window rect of control relative to screen
// 		//GetClientRect(hwnd, &WindowRect);
// 		POINT CursorPosition = { 0, 0 };
// 		GetCursorPos(&CursorPosition);
// 		POINT WindowCenter = {
// 			WindowRect.left + ((WindowRect.right - WindowRect.left) / 2),
// 			WindowRect.top + ((WindowRect.bottom - WindowRect.top) / 2)
// 		};
// 
// 		POINT MouseOffset = {
// 			CursorPosition.x - WindowCenter.x,
// 			CursorPosition.y - WindowCenter.y
// 		};
// 
// 		float CameraStepX = -1.0f;
// 		float CameraStepY = -2.7f;
// 
// 		if (bPrevFrameRMB)
// 		{
// 			//SetCursorPos(WindowCenter.x, WindowCenter.y);
// 			SceneCamera.Rotate(Funky::Math::Vector3f(MouseOffset.y * CameraStepY, MouseOffset.x * CameraStepX, 0.0f));
// 		}
// 
// 		SetCursorPos(WindowCenter.x, WindowCenter.y);
// 
// 		bPrevFrameRMB = true;
// 	}
// 	else
// 	{
// 		bPrevFrameRMB = false;
// 	}
// }
