#pragma once
namespace DirectX {
	struct AudioListener;
}
namespace DxEngine::SceneNS::Components {
	struct AudioListener {
		std::unique_ptr<DirectX::AudioListener> listener;
	};

}