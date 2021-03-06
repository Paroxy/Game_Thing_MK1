#pragma once
#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>
#include <bitset>
#include <array>

class Component;
class Entity;
class Manager;

using ComponentID = std::size_t;
using Group = std::size_t;

inline ComponentID getNewComponentTypeID() {
	static ComponentID lastID = 0u;
	return lastID++;
}

template <typename T> inline ComponentID getComponentTypeID() noexcept {
	static ComponentID typeID = getNewComponentTypeID();
	return typeID;
}

constexpr std::size_t maxComponents = 32;
constexpr std::size_t maxGroups = 32;

using ComponentBitSet = std::bitset<maxComponents>;
using GroupBitSet = std::bitset<maxGroups>;
using ComponentArray = std::array<Component*, maxComponents>;

class Component {
public:
	Entity* entity;
	virtual void init() {}
	virtual void update() {}
	virtual void draw() {}
	virtual ~Component() {}
};

class Entity {
public:
	Entity(Manager& mManager) : manager(mManager) {}
	void update() {
		for (auto& component : components) component->update();		
	}
	void draw() {
		for (auto& component : components) component->draw();
	}
	bool isActive() const { return active; }
	void destroy() { active = false; }

	bool hasGroup(Group mGroup) {
		return groupBitSet[mGroup];
	}

	void addGroup(Group mGroup);

	void removeGroup(Group mGroup) {
		groupBitSet[mGroup] = false;
	};

	template<typename T> bool hasComponent() const {
		return componentBitSet[getComponentTypeID<T>()];
	}

	template <typename T, typename... TArgs>
	T& addComponents(TArgs&&... nArgs) {
		T* comp(new T(std::forward<TArgs>(nArgs)...));
		comp->entity = this;
		std::unique_ptr<Component> uPtr{ comp };
		components.emplace_back(std::move(uPtr));

		componentArray[getComponentTypeID<T>()] = comp;
		componentBitSet[getComponentTypeID<T>()] = true;
		comp->init();
		return *comp;
	}

	template<typename T> T& getComponent() const {
		auto ptr(componentArray[getComponentTypeID<T>()]);
		return *static_cast<T*>(ptr);
	}

	

private:
	Manager & manager;
	bool active = true;
	std::vector<std::unique_ptr<Component>> components;
	ComponentArray componentArray;
	ComponentBitSet componentBitSet;
	GroupBitSet groupBitSet;
};

class Manager {
private:
	std::vector<std::unique_ptr<Entity>> entities;
	std::array<std::vector<Entity*>, maxGroups> entityGroups;
public:
	void update() {
		for (auto& entity : entities) entity->update();
	}
	void draw() {
		for (auto& entity : entities) entity->draw();
	}
	void refresh() {
		for (auto group = 0; group < maxGroups; group++) {
			auto& vec(entityGroups[group]);
			vec.erase(std::remove_if(std::begin(vec), std::end(vec),
				[group](Entity* mEntity) 
			{
				return !mEntity->isActive() || !mEntity->hasGroup(group);
			}), std::end(vec));
		}

		entities.erase(std::remove_if(std::begin(entities), std::end(entities),
			[](const std::unique_ptr<Entity> &mEntity) {
			return !mEntity->isActive();
		}),
			std::end(entities));
	}

	void addToGroup(Entity* mEntity, Group mGroup) {
		entityGroups[mGroup].emplace_back(mEntity);
	}

	std::vector<Entity*>& getGroup(Group mGroup) {
		return entityGroups[mGroup];
	}

	Entity& addEntity() {
		Entity* entity = new Entity(*this);
		std::unique_ptr<Entity> uPtr{entity};
		entities.emplace_back(std::move(uPtr));
		return *entity;
	}
};
