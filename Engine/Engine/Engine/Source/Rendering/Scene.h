#pragma once


#include "3rd/imgui/imgui.h"
#include "Core/String.h"
#include "Core/Containers.h"

#include "Rendering/MeshManager.h"
#include "Math/Math.h"

namespace Funky
{
	struct Scene
	{
		struct Node
		{
			std::string Name;

			virtual void DrawGUI() = 0;
		};

		struct Terrain : Node
		{
			bool bVisible;
			i32 GridCellsX = 0u;
			i32 GridCellsY = 0u;
			
			i32 ChunkResolution = 0u;
			f32 ChunkEdgeSize = 0.0f;


			std::vector<RawMesh*> TerrainNodes;


			Terrain()
			{
				Name = "Terrain";
			}

			virtual void DrawGUI() override
			{
				ImGui::Checkbox("Visible", &bVisible);

				ImGui::Spacing();
				ImGui::Spacing();
				ImGui::Spacing();

				ImGui::InputInt("GridCellsX", (i32*)(&GridCellsX));
				ImGui::InputInt("GridCellsY", (i32*)(&GridCellsY));

				ImGui::Spacing();
				ImGui::Spacing();

				ImGui::InputInt("ChunkResolution", (i32*)(&ChunkResolution));
				ImGui::InputFloat("ChunkEdgeSize", &ChunkEdgeSize);
				ImGui::Spacing();

				GridCellsX = GridCellsX < 0 ? 0 : GridCellsX;
				GridCellsY = GridCellsY < 0 ? 0 : GridCellsY;

				if (ImGui::Button("Recreate"))
				{
					if (GridCellsX == 0 || GridCellsY == 0)
					{
						for (auto& TerrainNode : TerrainNodes)
						{
							delete TerrainNode;
						}

						TerrainNodes.clear();
					}
					else
					{
						//TODO(ekicam2): 
						TerrainNodes.push_back(MeshUtils::CreateTerrainPlane(ChunkResolution, ChunkResolution, ChunkEdgeSize));
					}
				}

			}
		};

		//TODO(ekicam2): move to component
		struct Drawable : Node
		{
			Drawable()
				: Position(0.0f, 0.0f, 0.0f)
				, Rotation(0.0f, 0.0f, 0.0f)
				, Scale(1.0f, 1.0f, 1.0f)
				, bVisible(true)
			{
			}

			class RawMesh* Mesh = nullptr;

			bool bVisible;

			Math::Vector3f Position;
			Math::Vector3f Rotation;
			Math::Vector3f Scale;

			f32 PositionInput = 100.0f;
			f32 RotationInput = 180.0f;
			f32 ScaleInput = 100.0f;

			virtual void DrawGUI() override
			{
				if (ImGui::BeginPopupContextItem("position context menu"))
				{
					ImGui::InputFloat("Min/Max", &PositionInput);
					ImGui::EndPopup();
				}

				if (ImGui::BeginPopupContextItem("rotation context menu"))
				{
					ImGui::InputFloat("Min/Max", &RotationInput);
					ImGui::EndPopup();
				}

				if (ImGui::BeginPopupContextItem("scale context menu"))
				{
					ImGui::InputFloat("Min/Max", &ScaleInput);
					ImGui::EndPopup();
				}

				ImGui::Checkbox("Visible", &bVisible);

				ImGui::Spacing();
				ImGui::Spacing();
				ImGui::Spacing();

				ImGui::BeginGroup();
					ImGui::InputFloat3("Position##1", &Position.X);
					ImGui::SliderFloat3("Position##2", &Position.X, -PositionInput, PositionInput);
					ImGui::OpenPopupOnItemClick("position context menu", 1);
					ImGui::SameLine();
					ImGui::TextDisabled("(?)");
					if (ImGui::IsItemHovered())
					{
						ImGui::BeginTooltip();
						ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
						ImGui::TextUnformatted("Click RBM to change slider range.");
						ImGui::PopTextWrapPos();
						ImGui::EndTooltip();
					}
				ImGui::EndGroup();
				
				ImGui::Spacing();
				ImGui::Spacing();
				ImGui::Spacing();

				ImGui::BeginGroup();
					ImGui::InputFloat3("Rotation##1", &Rotation.X);
					ImGui::SliderFloat3("Rotation##2", &Rotation.X, -RotationInput, RotationInput);
					ImGui::OpenPopupOnItemClick("rotation context menu", 1);
					ImGui::SameLine();
					ImGui::TextDisabled("(?)");
					if (ImGui::IsItemHovered())
					{
						ImGui::BeginTooltip();
						ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
						ImGui::TextUnformatted("Click RBM to change slider range.");
						ImGui::PopTextWrapPos();
						ImGui::EndTooltip();
					}
				ImGui::EndGroup();

				ImGui::Spacing();
				ImGui::Spacing();
				ImGui::Spacing();

				ImGui::BeginGroup();
					ImGui::InputFloat3("Scale##1", &Scale.X);
					ImGui::SliderFloat3("Scale##2", &Scale.X, -ScaleInput, ScaleInput);
					ImGui::OpenPopupOnItemClick("scale context menu", 1);
					ImGui::SameLine();
					ImGui::TextDisabled("(?)");
					if (ImGui::IsItemHovered())
					{
						ImGui::BeginTooltip();
						ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
						ImGui::TextUnformatted("Click RBM to change slider range.");
						ImGui::PopTextWrapPos();
						ImGui::EndTooltip();
					}
				ImGui::EndGroup();

				if (Scale.X == 0.0f) Scale.X = 1.0f;
				if (Scale.Y == 0.0f) Scale.Y = 1.0f;
				if (Scale.Z == 0.0f) Scale.Z = 1.0f;
			}

		};
		
		Terrain MainTerrain;
		std::vector<Node*> SceneNodes;

		Scene()
		{
			SceneNodes.push_back(&MainTerrain);
		}

		~Scene()
		{
			i32 i = 0;

			for (auto& Obj : SceneNodes)
			{
				if(i == 0)
					continue;
				delete Obj;
				++i;
			}
		}

		void DrawGUI()
		{
			static i64 Selected = -1;


			ImGuiWindowFlags WindowFlags = 0;
			WindowFlags |= ImGuiWindowFlags_NoCollapse;
			ImGui::Begin("Scene Info", NULL, WindowFlags);

			ImGui::BeginChild("select", ImVec2(0, ImGui::GetWindowHeight() / 4.0f), true);
			for (i64 i = 0; i < (i64)SceneNodes.size(); ++i)
			{
				if (ImGui::Selectable(SceneNodes[i]->Name.c_str(), Selected == i))
					Selected = i;
			}
			ImGui::EndChild();

			ImGui::BeginChild("details", ImVec2(0, 0), true);
			if (Selected != -1)
			{
				auto DynamicCheck = dynamic_cast<Drawable*>(SceneNodes[Selected]);
				if (DynamicCheck)
				{
					if (ImGui::Button("Duplicate"))
					{
						//TODO(ekicam2): make it work, it has to be something other than virtual Node* Clone();
						auto NewDrawable = new Drawable();
						NewDrawable->Name = SceneNodes[Selected]->Name;
						Drawable* CurrDrawable = dynamic_cast<Drawable*>(SceneNodes[Selected]);
						if (CurrDrawable)
						{
							NewDrawable->Name = SceneNodes[Selected]->Name;
							for (auto const & SceneNode : SceneNodes)
							{
								if(std::string::npos != SceneNode->Name.find(NewDrawable->Name))
									NewDrawable->Name.append("_Clone");
							}

							NewDrawable->Position = CurrDrawable->Position;
							NewDrawable->PositionInput = CurrDrawable->PositionInput;

							NewDrawable->Scale = CurrDrawable->Scale;
							NewDrawable->ScaleInput = CurrDrawable->ScaleInput;

							NewDrawable->Rotation = CurrDrawable->Rotation;
							NewDrawable->RotationInput = CurrDrawable->RotationInput;

							NewDrawable->bVisible = CurrDrawable->bVisible;

							NewDrawable->Mesh = CurrDrawable->Mesh;
						}
						SceneNodes.push_back(NewDrawable);
					}

					ImGui::SameLine();

					if (ImGui::Button("Delete"))
					{
						delete SceneNodes[Selected];
						SceneNodes[Selected] = nullptr;
						SceneNodes.erase(SceneNodes.begin() + Selected);

						Selected = -1;
					}

					ImGui::Spacing();
					ImGui::Spacing();
					ImGui::Spacing();
					ImGui::Spacing();
				}

				if(Selected != -1)
					SceneNodes[Selected]->DrawGUI();
			}
			ImGui::EndChild();

			ImGui::End();
		}
	};

}
