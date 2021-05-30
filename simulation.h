#pragma once

#include <vector>
#include <unordered_set>
#include <map>
#include <unordered_map>

#include "macro.h"

#include "vector2int.h"
#include "transform.h"
#include "notimplemented.h"
#include "event.h"

namespace Simulation {

	class Simulation;
	class SimulationObject;
	class TickGroup;
	class SceneLayer;
	class Scene;
	class SceneComponent;

	typedef int IdType;

	//=============================================================================================

	class Ticker {

	};

	//=============================================================================================

	typedef class SimulationObject {
	public:
		virtual void tick(float deltaSeconds) = 0;
	}*SimulationObjectPtr;

	//=============================================================================================

	typedef class TickGroup : public SimulationObject {
	private:
		std::unordered_set<SimulationObjectPtr> items;

	public:
		void tick(float deltaSeconds) OVERRIDE {
			for (auto i = items.begin(); i != items.end(); i++)
			{
				(**i).tick(deltaSeconds);
			}
		}
	}*TickGroupPtr;

	//=============================================================================================

	typedef class SceneComponent : public SimulationObject {
		GET(Transform, Transform);
		GET(SceneComponentPtr, Parent);
		GET_CONST(std::unordered_set<SceneComponentPtr>, Children);

		GET_CONST(TickGroupPtr, TickGroup);
	public:

		void addChild(SceneComponentPtr child) {
			_Children.insert(child);
			child->_Parent = this;
		}

		void removeChild(SceneComponentPtr child) {
			_Children.erase(child);
			child->_Parent = nullptr;
		}
	}*SceneComponentPtr;

	//=============================================================================================

	class SceneLayer {
		GET(const int, Width);
		GET(const int, Height);
	private:
		std::unordered_set<SceneComponentPtr>* layerSpace;
	public:
		SceneLayer(int width, int height, int depth) :
			_Width(width),
			_Height(height)
		{
			//Okaaaaay, I suppose compiler to strip out excess constructor calls
			layerSpace = new std::unordered_set<SceneComponentPtr>[width, height];
			for (auto x = 0; x < width; x++)
				for (auto y = 0; y < height; y++)
					layerSpace[x, y] = std::unordered_set<SceneComponentPtr>(depth);
		}

		~SceneLayer() {
			delete[] layerSpace;
		}

		inline const std::unordered_set<SceneComponentPtr> at(int x, int y) const {
			return layerSpace[x, y];
		}
	};

	//=============================================================================================

	class Scene : SimulationObject {
		friend Simulation;
		
		GET(const int, Width);
		GET(const int, Height);
		GET_CONST(std::unordered_set<SceneComponentPtr>, Objects);
	private:
		std::unordered_map<IdType, SceneLayer> layers = std::unordered_map<IdType, SceneLayer>();

		///
		///@param 
		///depth - Number of layers to resrve space for
		Scene(int width, int height, int depth) :
			_Width(width),
			_Height(height)
		{
			layers.reserve(depth);
			_Objects = std::unordered_set<SceneComponentPtr>();
		}
	public:

		~Scene() {
			for (auto o = _Objects.begin(); o != _Objects.end(); o++) {
				delete& (*o);
			}
		}

		inline const std::vector<const SceneLayer>& getLayers() const noexcept {
			return reinterpret_cast<const std::vector<const SceneLayer>&>(layers);
		}

		SceneLayer& addLayer(IdType id) {
			throw new NotImplementedException("Scene.addLayer()");
		}

		SceneLayer& removeLayer(IdType id) {
			throw new NotImplementedException("Scene.removeLayer()");
		}

		void addObject(SimulationObject obj) {
			throw new NotImplementedException("Scene.addObject()");
		}

		void removeObject(SimulationObject obj) {
			throw new NotImplementedException("Scene.removeObject()");
		}

		void tick(float deltaSeconds) OVERRIDE {
			throw new NotImplementedException("Scene.tick()");
		}
	};

	//=============================================================================================

	class Simulation : SimulationObject {
	private: typedef std::unordered_map<int, Scene*> SceneContainer;
		//Менеджер сцен
		//Менеджер общих ресурсов
		//Шина событий верхнего уровня
		GET_CONST(SceneContainer, Scenes);
	private:
		std::map<int, TickGroup> tickGroups;
	public:
		const EventBus<void, const SimulationObject&, int, const void*> eventBus = EventBus<void, const SimulationObject&, int, const void*>();

	public:
		Scene& addScene(IdType id, int width, int height, int depth) {
			auto scene = new Scene(width, height, depth);
			_Scenes[id] = scene;
			return *scene;
		}

		Scene& removeScene(IdType id) {
			auto scene = _Scenes.at(id);
			_Scenes.erase(id);
			return *scene;
		}

		void tick(float deltaSeconds) OVERRIDE {
			for (auto i = tickGroups.begin(); i != tickGroups.end(); i++) {
				i->second.tick(deltaSeconds);
			}
		}
	};





	class Block : SimulationObject {};

	class Ball : SimulationObject {};

	class Board : SimulationObject {};

	class Bonus : SimulationObject {};

	class Decoration : SimulationObject {};

}