#pragma once
#include <dxpch.h>
#include "DxEngine.h"

namespace DxEngine {
	class Scene {
	public:
		std::string name;
		std::vector<Model> models;
	};
}