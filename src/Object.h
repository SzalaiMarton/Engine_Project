#pragma once

#include <typeindex>

class QuadTree;
class Object;
class Layer;
class QuadTree;

constexpr float gravity = 9.81f;

class Component {
public:
	virtual ~Component() = default;
	virtual Component* copy() const = 0;
};

struct HealthComponent : public Component {
	uint32_t maxHp;
	uint32_t currentHp;

	HealthComponent(uint32_t maxHp) : maxHp(maxHp), currentHp(maxHp) {}
	HealthComponent(const HealthComponent& original);

	HealthComponent* copy() const override;
	HealthComponent& takeDmg(uint32_t dmg);
	HealthComponent& heal(uint32_t heal);
};

struct TextureComponent : public Component {
	sf::Sprite* sprite;
	bool isVisible;

	TextureComponent(std::string_view textureName, bool isVisible);
	TextureComponent(const TextureComponent& original);
	~TextureComponent() {
		delete sprite;
		sprite = nullptr;
	}

	TextureComponent* copy() const override;
	TextureComponent& display(sf::RenderWindow& window);
	TextureComponent& setTexture(std::string_view name);
	TextureComponent& setSize(unsigned int width, unsigned int height);
	TextureComponent& setPos(float x, float y);
	TextureComponent& setPos(const sf::Vector2f& pos);
	TextureComponent& setRot(float angle);
	bool contains(const sf::Vector2f& pos, const sf::Vector2f& bound) const;
	bool containsCaseOverlap(const sf::Vector2f& pos, const sf::Vector2f& bound) const;
};

struct PhysicsComponent : public Component {
	sf::Vector2f pos{};
	float mass{};
	float gravityTransformer = 1.f;
	sf::Vector2f velocity{};
	sf::Vector2f acceleration{};
	sf::Vector2f angularVelocity{};
	sf::Vector2f angularAcceleration{};

	PhysicsComponent() = default;
	PhysicsComponent(const PhysicsComponent& original);
	~PhysicsComponent() = default;

	PhysicsComponent* copy() const override;
	PhysicsComponent& setGravity(float value);
	PhysicsComponent& setPos(const sf::Vector2f& value);
	PhysicsComponent& setMass(float value);
	void update();
	void applyForce(const sf::Vector2f& force);
	void clampsToObject(Object* parent, Object* other);
};

struct CollideComponent : public Component {
	bool canCollide = true;
	bool continuousCollisionChecks = false; // if false it uses discrete checks
	std::set<QuadTree*> treeNodes{};
	sf::Vector2f prevPos{};

	CollideComponent(const sf::Vector2f& currentPos);
	CollideComponent(const CollideComponent& original);
	~CollideComponent() = default;

	CollideComponent* copy() const override;
	CollideComponent& setCollision(bool value);
	CollideComponent& removeTreeNode(QuadTree* node);
	bool isMoving(const sf::Vector2f& currentPos) const;
	std::vector<Object*> discreteChecks(Object* original);
	std::vector<Object*> continuousChecks(Object* original);
};


class Object {
public:
	std::string name;
	uint8_t zIndex;
	std::unordered_map<std::type_index, Component*> components;
	bool isOnScreen = false;
	Layer* parent = nullptr;

	Object(std::string_view name, Layer* parent, uint8_t zIndex);
	Object(const Object& obj);
	~Object();

	void operator=(const Object& obj) {
		this->name = obj.name;
		for (auto& pair : this->components) {
			delete pair.second;
		}
		this->components.clear();
		for (auto& pair : obj.components) {
			this->components[pair.first] = pair.second->copy();
		}
	}

	template<typename T>
	void addComponent(T* comp) {
		this->components[std::type_index(typeid(T))] = comp;
	}

	template<typename T>
	bool hasComponent() {
		return this->components.find(std::type_index(typeid(T))) != this->components.end();
	}

	template<typename T>
	T* getComponent() {
		auto it = components.find(std::type_index(typeid(T)));
		return it != components.end() ? dynamic_cast<T*>(it->second) : nullptr;
	}

	const std::string& getName() const;
	Object& setName(std::string_view name);
	Object& setZIndex(uint8_t newZIndex);
	Object& setOnScreen(bool value);
	Object& setPos(const sf::Vector2f& pos);

	void info();
	void onUpdate();
	
};

class Builder {
private:
	Object* object;
public:
	Builder(std::string_view name, Layer* parent, uint8_t zIndex) : object(new Object(name, parent, zIndex)) {}
	~Builder();

	Builder& withHealth(uint32_t maxHealth);
	Builder& withTexture(std::string_view textureName);
	Builder& withPhysics();
	Builder& withCollision(bool canCollide);
	Builder& withPos(const sf::Vector2f& pos);
	Object* build() const;
};

class Square {
public:
	sf::Vector2f pos;
	sf::Vector2f size;
	Layer* l = nullptr;

	Square(sf::Vector2f pos, sf::Vector2f size, Layer* l);
};