#pragma once

namespace YAML
{
	class Emitter;
	class Node;
}

namespace tomato
{
    class Scene;
    class Entity;
    class EntitySerializer
	{
	public:
		static void SerializeEntity(YAML::Emitter& out, Entity* entity);
		static UUID DeserializeEntity(const YAML::Node& node, Scene& scene, bool preserveUUID);
		static void SerializeEntityAsPrefab(const char* filepath, Entity* entity);
		static Entity* DeserializeEntityAsPrefab(const char* filepath, Scene& scene) { return nullptr;  };
	};
}
