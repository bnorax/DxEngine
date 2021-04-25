#pragma once
namespace DirectX {
	struct AudioEmitter;
	class SoundEffect;
	class SoundEffectInstance;
}
namespace DxEngine {
	namespace SceneNS::Components {
		struct AudioSource {
			std::string pathToSound;
			std::shared_ptr<DirectX::SoundEffect> sound;
			std::shared_ptr<DirectX::SoundEffectInstance> soundInstance;
			std::unique_ptr<DirectX::AudioEmitter> emitter;
		};
	}
}