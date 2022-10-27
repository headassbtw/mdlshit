#include <structs.hpp>
#include <logger.hpp>
#include <binarystream.hpp>
#include <argtools.hpp>
#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <rapidjson/istreamwrapper.h>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <JsonTest.h>
#include <MLUtil.h>


void JsonTest::ReadExtraStructs(FileInfo info) //This is temp stuff tbh. -Liberty
{
	if (info.str.has_value())
	{
		std::string extraStructs = info.str.value();

		std::ifstream ifs(extraStructs.c_str());

		// begin json parsing
		rapidjson::IStreamWrapper isw{ ifs };

//		rapidjson::Document doc{};

		doc.ParseStream(isw);

		HandleType();


	}
	else Logger::Info("No Exta Struct Data Detected\n");
}

void JsonTest::HandleType()
{
	if (doc.HasMember("hitbox"))	HandleHitbox();
	
	if (doc.HasMember("ikChain"))	HandleIkChain();
	
	if (doc.HasMember("ruiMesh"))	HandleRuiMesh();
}

void JsonTest::HandleHitbox()
{
	Logger::Info("HasHitbox\n");
}

void JsonTest::HandleIkChain()
{
	Logger::Info("HasIkChain\n");
}

void JsonTest::HandleRuiMesh()
{
	Logger::Info("HasRuiMesh\n");
}