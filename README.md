# DxEngine
## About
Personal project for learning DirectX 11 and C++ 17.

<img src="https://github.com/bnorax/DxEngine/blob/ecsmain/resources/screens/DxEngine.png" width="720">

## Current status/features
* Loading meshes/simple materials(diffuse map only)/animations.
* Simple audio listener/emitter system.
* Skeletal animation(not working in current iteraion/need rework as ECS system).
* Free floating camera
## Structure
Latest iteration of engine uses ECS pattern for game objects in scene.
Audio system uses [DirectX ToolKit](https://github.com/Microsoft/DirectXTK) to work with sound.
Mesh importing done with [assimp library](https://github.com/assimp/assimp). 
ECS framework - [entt](https://github.com/skypjack/entt)
