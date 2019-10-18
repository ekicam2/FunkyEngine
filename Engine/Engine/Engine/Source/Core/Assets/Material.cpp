#include "Material.h"
#include "FunkyEngine.h"

Funky::Asset::Material::Material(str const& Name)
	: IAsset(MaterialsPath, Asset::Type::Material)
{

	const str MaterialFullPath = MaterialsPath + Name;
	auto Shaders = Material::ParseMaterial(MaterialFullPath);
	if (!Shaders)
		LOG_ERROR_FUNKY("couldn't read material ", MaterialFullPath.c_str());

	VS = [&]() {
		std::ifstream Input(Shaders->first, std::ios::binary);
		darray<byte> ShaderBinaryBuffer(std::istreambuf_iterator<char>(Input), {});
		Input.close();
		return FunkyEngine::GetEngine()->GetRenderingBackend()->CreateVertexShader(ShaderBinaryBuffer.data(), ShaderBinaryBuffer.size());
	}();

	PS = [&]() {
		std::ifstream Input(Shaders->second, std::ios::binary);
		darray<BYTE> ShaderBinaryBuffer(std::istreambuf_iterator<char>(Input), {});
		Input.close();
		return FunkyEngine::GetEngine()->GetRenderingBackend()->CreatePixelShader(ShaderBinaryBuffer.data(), ShaderBinaryBuffer.size());
	}();
}

FORCEINLINE bool Funky::Asset::Material::IsValid()
{
	return VS != Rendering::RenderingBackend::INVALID_INDEX && PS != Rendering::RenderingBackend::INVALID_INDEX;
}

std::optional<std::pair<str, str>> Funky::Asset::Material::ParseMaterial(str const& Path)
{
	std::ifstream MaterialFile(Path, std::ios::binary);
	if (!MaterialFile.is_open()) return {};

	std::pair<str, str> Ret;

	std::getline(MaterialFile, Ret.first);
	std::getline(MaterialFile, Ret.second);

	MaterialFile.close();

	return Ret;
}