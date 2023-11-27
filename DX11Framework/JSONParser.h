#pragma once
#include <string>
#include "JSON/json.hpp"
#include <fstream>
#include <vector>
#include "Structures.h"

using json = nlohmann::json;

class JSONParser
{
private:
public:
	LightBuffer Light;

	JSONParser()
	{
		//Json Parser
		json jFile;


		std::ifstream fileOpen("JSON/LightData.json");

		jFile = json::parse(fileOpen);

		std::string v = jFile["version"].get<std::string>();

		json& lights = jFile["LightData"]; //? gets an array
		int size = lights.size();
		for (unsigned int i = 0; i < size; i++)
		{
			LightBuffer LD;
			json& lightDesc = lights.at(i);
#pragma region Diffuse
			LD.DiffuseLight.x = lightDesc["DiffuseLightR"];
			LD.DiffuseLight.y = lightDesc["DiffuseLightG"];
			LD.DiffuseLight.z = lightDesc["DiffuseLightB"];
			LD.DiffuseLight.w = lightDesc["DiffuseLightA"];

			LD.DiffuseMaterial.x = lightDesc["DiffuseMaterialR"];
			LD.DiffuseMaterial.y = lightDesc["DiffuseMaterialG"];
			LD.DiffuseMaterial.z = lightDesc["DiffuseMaterialB"];
			LD.DiffuseMaterial.w = lightDesc["DiffuseMaterialA"];
#pragma endregion

#pragma region Ambiant
			LD.AmbiantLight.x = lightDesc["AmbiantLightR"];
			LD.AmbiantLight.y = lightDesc["AmbiantLightG"];
			LD.AmbiantLight.z = lightDesc["AmbiantLightB"];
			LD.AmbiantLight.w = lightDesc["AmbiantLightA"];

			LD.AmbiantMaterial.x = lightDesc["AmbiantMaterialR"];
			LD.AmbiantMaterial.y = lightDesc["AmbiantMaterialG"];
			LD.AmbiantMaterial.z = lightDesc["AmbiantMaterialB"];
			LD.AmbiantMaterial.w = lightDesc["AmbiantMaterialA"];
#pragma endregion

#pragma region Specular
			LD.SpecularLight.x = lightDesc["SpecularLightR"];
			LD.SpecularLight.y = lightDesc["SpecularLightG"];
			LD.SpecularLight.z = lightDesc["SpecularLightB"];
			LD.SpecularLight.w = lightDesc["SpecularLightA"];

			LD.SpecularMaterial.x = lightDesc["SpecularMaterialR"];
			LD.SpecularMaterial.y = lightDesc["SpecularMaterialR"];
			LD.SpecularMaterial.z = lightDesc["SpecularMaterialR"];
			LD.SpecularMaterial.w = lightDesc["SpecularMaterialR"];

			LD.SpecPower = lightDesc["SpecularPower"];
#pragma endregion

#pragma region LightDirection
			LD.LightDir.x = lightDesc["LightDirectionX"];
			LD.LightDir.y = lightDesc["LightDirectionY"];
			LD.LightDir.z = lightDesc["LightDirectionZ"];
#pragma endregion

			Light = LD;
		}
		fileOpen.close();
	}
};

