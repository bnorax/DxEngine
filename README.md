# DxEngine
## About
Personal project for learning DirectX 11 and C++ 17.

<img src="https://github.com/bnorax/DxEngine/blob/ecsmain/resources/screens/DxEngine.png" width="720">
<img src="https://github.com/bnorax/DxEngine/blob/ecsmain/resources/screens/RCo2xIZ6Zow.jpg" width="720">
<img src="https://github.com/bnorax/DxEngine/blob/ecsmain/resources/screens/o8kYkFsQx9U.jpg" width="720">


## Current status/features
* Loading meshes/simple materials(diffuse map only)/animations.
* Simple audio listener/emitter system.
* Skeletal animation(not working in current iteraion/need rework as ECS system). When it worked, it looked like this (example): https://youtu.be/iSZX21vZ240
* Free floating camera
## Structure
Latest iteration of engine uses ECS pattern for game objects in scene.
Audio system uses [DirectX ToolKit](https://github.com/Microsoft/DirectXTK) to work with sound.
Mesh importing done with [assimp library](https://github.com/assimp/assimp). 
ECS framework - [entt](https://github.com/skypjack/entt)
