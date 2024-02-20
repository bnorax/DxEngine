# DxEngine
## About
Personal project for learning DirectX 11 and C++ 17.

<img src="https://github.com/bnorax/DxEngine/blob/ecsmain/resources/screens/DxEngine.png" width="720">
<p float="left">
  <img src="https://github.com/bnorax/DxEngine/blob/ecsmain/resources/screens/RCo2xIZ6Zow.jpg" width="360" height="360">
  <img src="https://github.com/bnorax/DxEngine/blob/ecsmain/resources/screens/o8kYkFsQx9U.jpg" width="360" height="360">
</p>

## Current status/features
* Loading meshes/simple materials(diffuse map only)/animations.
* Simple audio listener/emitter system.
* Skeletal animation(broken in current iteraion/need rework as ECS system).
* Free floating camera
## Structure
Latest iteration of engine uses ECS pattern for game objects in scene.
Audio system uses [DirectX ToolKit](https://github.com/Microsoft/DirectXTK) to work with sound.
Mesh importing done with [assimp library](https://github.com/assimp/assimp). 
ECS framework - [entt](https://github.com/skypjack/entt)
