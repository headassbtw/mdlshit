#pragma once
#include <structs.hpp>

class JsonTest
{
	public:

		rapidjson::Document doc{};

		std::string types[3]{"hitbox", "ikChain", "ruiMesh"};

		void ReadExtraStructs(FileInfo info);

		void HandleType();

		void HandleHitbox();

		void HandleIkChain();

		void HandleRuiMesh();
};