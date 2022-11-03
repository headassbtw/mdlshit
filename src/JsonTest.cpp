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
#include <JsonTest.hpp>
#include <MLUtil.hpp>
#include <rapidjson/error/error.h>

using namespace rapidjson;

void JsonTest::ReadExtraStructs(FileInfo info) //This is temp stuff tbh. -Liberty
{
	if (info.str.has_value())
	{
		Logger::Info("HasExtraStructs\n");
		std::string extraStructs = info.str.value();

		std::ifstream ifs(extraStructs.c_str());

		// begin json parsing
		IStreamWrapper isw{ ifs };

//		rapidjson::Document doc{};

		doc.ParseStream(isw);

		if (doc.HasParseError()) Logger::Info("JSON parse error at offset: (%u)\n", doc.GetErrorOffset());
		else
		HandleType();


	}
	else Logger::Info("No Exta Struct Data Detected\n");
}

void JsonTest::HandleType()
{
	if (doc.HasMember("ExtraData") && doc["ExtraData"].IsArray())
	{
		Logger::Info("HasExtraData\n");
		for (auto& data : doc["ExtraData"].GetArray())
		{
			if (data.HasMember("$type"))
			{
				Logger::Info("HasType\n");
				if (data["$type"].GetStdString() == types[0]) { HandleBone(data, true); };

				if (data["$type"].GetStdString() == types[1]) { HandleHitbox(data, true); };

				if (data["$type"].GetStdString() == types[2]) { HandleSeq(data, true); };

				if (data["$type"].GetStdString() == types[3]) { HandleModel(data, true); };

				if (data["$type"].GetStdString() == types[4]) { HandleIkChain(data, true); };

				if (data["$type"].GetStdString() == types[5]) { HandleMesh(data, true); };

				if (data["$type"].GetStdString() == types[6]) { HandleRuiMesh(data, true); };

			}
		}
	}
}

void JsonTest::HandleBone(Value& value, bool debug)
{
	if (debug) Logger::Info("HasBone\n");
	JsonTest::ExtraBoneData boneData{};

	if (value.HasMember("bones") && value["bones"].IsArray())
	{
		for (auto& bone : value["bones"].GetArray())
		{
			if (bone.IsInt())
			{
				if (debug) Logger::Info("Has_Bone: %d\n", bone.GetInt());
				boneData.bones.push_back(bone.GetInt());
			}
		}
		if (value["bones"].GetArray().Size() == 0)
		{
			if (debug) Logger::Info("Bones: Empty. Will use single bone if possible. \n");
		}
	}
}

void JsonTest::HandleHitbox(Value& value, bool debug)
{
	if (debug) Logger::Info("HasHitbox\n");
	JsonTest::ExtraHitboxData hitboxData{};

	if (value.HasMember("groups") && value["groups"].IsArray())
	{
		if (debug) Logger::Info("HasGroups\n");
		for (auto& group : value["groups"].GetArray())
		{ 
			if (group.HasMember("name"))
			{
				if (debug) Logger::Info("HasGroupName\n");
				if (group["name"].GetStdString() != "" && group["name"].IsString())
				{
					if (debug) Logger::Info("GroupName: %s\n", group["name"].GetStdString().c_str());
					hitboxData.groupName = group["name"].GetStdString();
				}
				else
				{
					if (debug) Logger::Info("GroupName: Empty. Will use hBoxes arry if possible.\n");
					hitboxData.groupName = group["name"].GetStdString();
				}
			}

			if (group.HasMember("hBoxes") && group["hBoxes"].IsArray() && !group.HasMember("name") || group.HasMember("hBoxes") && group["hBoxes"].IsArray() && group.HasMember("name") && group["name"].GetStdString() == "")
			{
				if (debug) Logger::Info("HasHitboxGroup\n");
				for (auto& hBox : group["hBoxes"].GetArray())
				{
					if ( hBox.IsInt() )
					{
						if (debug) Logger::Info("HasHbox: %d\n", hBox.GetInt());
						hitboxData.hboxArry.push_back(hBox.GetInt());
					}
				}
				if (group["hBoxes"].GetArray().Size() == 0)
				{
					if (debug) Logger::Info("HitboxGroup: Empty. Will use GroupName if possible. \n");
				}
			}

			if (group.HasMember("isCrit"))
			{
				if (debug) Logger::Info("HasIsCrit\n");
				if (group["isCrit"].IsBool())
				{
					if (debug) Logger::Info("IsCritSet: %s\n", group["isCrit"].GetBool() ? "true" : "false");
					hitboxData.isCrit = group["isCrit"].GetBool();
				}
				else
				{
					if (debug) Logger::Info("IsCritSet: Empty. Setting to false. \n");
					hitboxData.isCrit = false;
				}
			}

			if (group.HasMember("kvName"))
			{
				if (debug) Logger::Info("HasGroupKvName\n");
				if (group["kvName"].GetStdString() != "" && group["kvName"].IsString())
				{
					if (debug) Logger::Info("kvName: %s\n", group["kvName"].GetStdString().c_str());
					hitboxData.kvName = group["kvName"].GetStdString();
				}
				else
				{
					if (debug) Logger::Info("kvName: Empty\n");
					hitboxData.kvName = group["kvName"].GetStdString();
				}
			}
		}
		extraHitboxData.push_back(hitboxData);
	}
	else Logger::Info("ERROR: Try not to be a dumbass and actually finish the json struct.\n");
}

void JsonTest::HandleSeq(Value& value, bool debug)
{
	if (debug) Logger::Info("HasSeq\n");
}

void JsonTest::HandleModel(Value& value, bool debug)
{
	if (debug) Logger::Info("HasModel\n");
}

void JsonTest::HandleIkChain(Value& value, bool debug)
{
	if (debug) Logger::Info("HasIkChain\n");
}

void JsonTest::HandleMesh(Value& value, bool debug)
{
	if (debug) Logger::Info("HasMesh\n");
}

void JsonTest::HandleRuiMesh(Value& value, bool debug)
{
	if (debug) Logger::Info("HasRuiMesh\n");
}