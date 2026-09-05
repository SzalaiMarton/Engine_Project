#include "stdafx.h"
#include "Object.h"
#include <cmath>

void Object::onUpdate() {
	if (Renderer::getCamera()->isOnScreen(this)) {
		this->isOnScreen = true;
	}
	else {
		this->isOnScreen = false;
	}

	auto collComp = this->getComponent<CollideComponent>();
	auto texComp = this->getComponent<TextureComponent>();
	auto phyComp = this->getComponent<PhysicsComponent>();

	if (phyComp) {
		auto prevPos = phyComp->pos;

		phyComp->applyForce({ 0, (gravity * phyComp->gravityTransformer)});
		phyComp->update();

		if (collComp && texComp && phyComp && collComp->continuousCollisionChecks) {
			std::vector<Object*> collidingObjs = collComp->continuousChecks(this);

			if (!collidingObjs.empty()) {
				if (this->getName() == "obj3") {
					LOG(collidingObjs.size());
				}
				
				phyComp->gravityTransformer = 0;
				phyComp->velocity = { 0, 0 };
				phyComp->clampsToObject(this, collidingObjs[0]);
			}
		}

		if (texComp) {
			texComp->setPos(phyComp->pos);
		}
	}

	if (collComp && texComp) {
		if (collComp->isMoving(texComp->sprite->getPosition())) {
			collComp->treeNodes.clear();
			CollisionSystem::insert(this);
		}
	}

	if (collComp && texComp && phyComp && !collComp->continuousCollisionChecks) {
		std::vector<Object*> collidingObjs = collComp->discreteChecks(this);

		if (!collidingObjs.empty()) {
			phyComp->gravityTransformer = 0;
			phyComp->velocity = {0, 0};
		}
	}
}

Object::Object(std::string_view name, Layer* parent, uint8_t zIndex) : name(name), parent(parent), zIndex(zIndex) {
	this->parent->addObject(this, this->zIndex);
}

Object::Object(const Object& obj) : name(obj.name), zIndex(obj.zIndex), parent(obj.parent) {
	for (auto& pair : obj.components) {
		this->components[pair.first] = pair.second->copy();

		if (pair.first == std::type_index(typeid(CollideComponent))) {
			for (auto& node : this->getComponent<CollideComponent>()->treeNodes) {
				node->insert(this);
			}
		}
	}

	this->parent->addObject(this, this->zIndex);
}

Object::~Object() {
	LOG("deleting " << this->name);
	for (auto& comp : this->components) {
		if (comp.second) {
			delete comp.second;
		}
		comp.second = nullptr;
	}
	LOG("finished deleting " << this->name);
}

const std::string& Object::getName() const {
	return this->name;
}
Object& Object::setName(std::string_view name) {
	this->name = name;
	return *this;
}

Object& Object::setOnScreen(bool value) {
	this->isOnScreen = value;
	return *this;
}


Builder::~Builder() {
	//LOG("deleting builder.");
	this->object = nullptr;
	//LOG("finished deleting builder.");
}

Builder& Builder::withHealth(uint32_t maxHealth) {
	object->addComponent(new HealthComponent(maxHealth));
	return *this;
}

Builder& Builder::withTexture(std::string_view textureName) {
	object->addComponent(new TextureComponent(textureName, true));
	return *this;
}

Builder& Builder::withPhysics() {
	if (this->object->hasComponent<TextureComponent>()) {
		this->object->addComponent(new PhysicsComponent());
		return *this;
	}
	else {
		throw ObjectError("Cannot add physics without texture component. Add texture component first then add physics component.");
		return *this;
	}
}

Builder& Builder::withCollision(bool canCollide) {
	if (this->object->hasComponent<TextureComponent>()) {
		this->object->addComponent(new CollideComponent(this->object->getComponent<TextureComponent>()->sprite->getPosition()));
		return *this;
	}
	else {
		throw ObjectError("Cannot add collision without texture component. Add texture component first then add collide component.");
		return *this;
	}
}

Builder& Builder::withPos(const sf::Vector2f& pos) {
	if (this->object->hasComponent<PhysicsComponent>()) {
		this->object->getComponent<PhysicsComponent>()->setPos(pos);
	}
	if (this->object->hasComponent<TextureComponent>()) {
		this->object->getComponent<TextureComponent>()->setPos(pos);
	}
	return *this;
}

Object* Builder::build() const {
	return object;
}


HealthComponent& HealthComponent::takeDmg(uint32_t dmg) {
	this->currentHp -= dmg;
	return *this;
}

HealthComponent& HealthComponent::heal(uint32_t heal) {
	this->currentHp += heal;
	return *this;
}

TextureComponent::TextureComponent(std::string_view textureName, bool isVisible) {
	try {
		this->sprite = new sf::Sprite(*Textures::getTexture(textureName));
	}
	catch (TextureError e) {
		ERROR(e.what());
		this->sprite = nullptr;
	}
	this->isVisible = true;
}

TextureComponent::TextureComponent(const TextureComponent& original) {
	this->isVisible = original.isVisible;
	this->sprite = new sf::Sprite(*original.sprite);
	//this->setSize(original.sprite->getGlobalBounds().size.x, original.sprite->getGlobalBounds().size.y); don't remember the reason why it's commented
}

HealthComponent::HealthComponent(const HealthComponent& original) {
	this->currentHp = original.currentHp;
	this->maxHp = original.maxHp;
}

TextureComponent* TextureComponent::copy() const {
	return new TextureComponent(*this);
}

HealthComponent* HealthComponent::copy() const {
	return new HealthComponent(*this);
}

TextureComponent& TextureComponent::display(sf::RenderWindow& window) {
	if (this->isVisible && this->sprite) {
		window.draw(*(this->sprite));
	}
	return *this;
}

TextureComponent& TextureComponent::setTexture(std::string_view name) {
	try {
		auto t = Textures::getTexture(name);
		this->sprite->setTexture(*t);
	}
	catch (TextureError e) {
		ERROR(e.what());
	}
	return *this;
}

TextureComponent& TextureComponent::setSize(unsigned int width, unsigned int height) {
	if (this->sprite) {
		auto gb = this->sprite->getLocalBounds().size;
		this->sprite->setScale({ width / gb.x, height / gb.y });
	}
	return *this;
}

TextureComponent& TextureComponent::setPos(float x, float y) {
	if (this->sprite) {
		this->sprite->setPosition({ x, y });
	}
	return *this;
}

TextureComponent& TextureComponent::setRot(float angle) {
	if (this->sprite) {
		this->sprite->setRotation(sf::degrees(angle));
	}
	return *this;
}

bool TextureComponent::contains(const sf::Vector2f& pos, const sf::Vector2f& bound) const {
	return (this->sprite->getGlobalBounds().contains(pos) || this->sprite->getGlobalBounds().contains(bound));
}

bool TextureComponent::containsCaseOverlap(const sf::Vector2f& pos, const sf::Vector2f& bound) const {
	auto p = this->sprite->getGlobalBounds().position;
	auto b = this->sprite->getGlobalBounds().size + p;

	auto xPos = p.x;
	auto yPos = p.y;

	auto xSize = b.x;
	auto ySize = b.y;

	return ((xPos >= pos.x && xPos + xSize <= bound.x && yPos < pos.y && yPos + ySize >= pos.y)
		|| (yPos >= pos.y && yPos + ySize <= bound.y && xPos < pos.x && xPos + xSize >= pos.x)
		|| (xPos >= pos.x && xPos + xSize <= bound.x && yPos >= pos.y && yPos + ySize <= bound.y));
}

TextureComponent& TextureComponent::setPos(const sf::Vector2f& pos) {
	if (this->sprite) {
		this->sprite->setPosition(pos);
	}
	return *this;
}

PhysicsComponent& PhysicsComponent::setPos(const sf::Vector2f& pos) {
	this->pos = pos;
	return *this;
}

PhysicsComponent& PhysicsComponent::setMass(float value) {
	this->mass = value;
	return *this;
}

Object& Object::setPos(const sf::Vector2f& pos) {
	if (this->hasComponent<PhysicsComponent>()) {
		this->getComponent<PhysicsComponent>()->setPos(pos);
	}
	if (this->hasComponent<TextureComponent>()) {
		this->getComponent<TextureComponent>()->setPos(pos);
	}
	return *this;
}

PhysicsComponent::PhysicsComponent(const PhysicsComponent& original) {
	this->mass = original.mass;
	this->velocity = original.velocity;
	this->acceleration = original.acceleration;
	this->angularVelocity = original.angularVelocity;
	this->angularAcceleration = original.angularAcceleration;
	this->gravityTransformer = original.gravityTransformer;
}

PhysicsComponent* PhysicsComponent::copy() const {
	return new PhysicsComponent(*this);
}

CollideComponent::CollideComponent(const sf::Vector2f& currentPos) : prevPos(currentPos) { }

CollideComponent::CollideComponent(const CollideComponent& original) {
	this->canCollide = original.canCollide;
	this->treeNodes = original.treeNodes;
}

CollideComponent* CollideComponent::copy() const {
	return new CollideComponent(*this);
}

CollideComponent& CollideComponent::setCollision(bool value) {
	this->canCollide = value;
	return *this;
}

CollideComponent& CollideComponent::removeTreeNode(QuadTree* node) {
	if (this->treeNodes.find(node) != this->treeNodes.end()) {
		this->treeNodes.erase(this->treeNodes.find(node));
	}
	return *this;
}

std::vector<Object*> CollideComponent::discreteChecks(Object* original) {
	// discrete check always have to be called after position refresh

	std::vector<Object*> res{};

	auto ogPos = original->getComponent<TextureComponent>()->sprite->getGlobalBounds().position;
	auto ogBound = original->getComponent<TextureComponent>()->sprite->getGlobalBounds().size + ogPos;

	for (auto& node : this->treeNodes) {
		for (auto& e : node->elements) {
			if (e == original) {
				continue;
			}

			if (e->getComponent<TextureComponent>()->contains(ogPos, ogBound)) {
				res.push_back(e);
			}
		}
	}

	return res;
}

std::vector<Object*> CollideComponent::continuousChecks(Object* original) {
	// continuous check always have to be called before position refresh
	
	std::vector<Object*> res{};
	
	auto ogPos = original->getComponent<TextureComponent>()->sprite->getGlobalBounds().position;
	auto ogSize = original->getComponent<TextureComponent>()->sprite->getGlobalBounds().size;
	auto distance = original->getComponent<TextureComponent>()->sprite->getGlobalBounds().position + original->getComponent<PhysicsComponent>()->velocity;

	uint8_t stepsX = (uint8_t)ceil(abs(ogPos.x - distance.x) / ogSize.x);
	uint8_t stepsY = (uint8_t)ceil(abs(ogPos.y - distance.y) / ogSize.y);

	auto testingPos = ogPos;

	while (stepsX > 0 || stepsY > 0) {
		if (stepsX <= 0) {
			testingPos.x = ogPos.x;
		}
		
		if (stepsY <= 0) {
			testingPos.y = ogPos.y;
		}
		auto testingBound = testingPos + ogSize;

		// test pos
		auto node = CollisionSystem::getNode(testingPos);
		for (auto& e : node->elements) {
			if (e == original) {
				continue;
			}

			if (e->getComponent<TextureComponent>()->contains(ogPos, testingBound)) {
				res.push_back(e);
			}
		}

		// test bound
		node = CollisionSystem::getNode(testingBound);
		for (auto& e : node->elements) {
			if (e == original) {
				continue;
			}

			if (e->getComponent<TextureComponent>()->contains(ogPos, testingBound)) {
				res.push_back(e);
			}
		}

		if (stepsX > 0) {
			stepsX--;
			testingPos.x -= ogSize.x;
		}

		if (stepsY > 0) {
			stepsY--;
			testingPos.y -= ogSize.y;
		}
	}

	return res;
}

bool CollideComponent::isMoving(const sf::Vector2f& currentPos) const {
	return (currentPos != prevPos);
}

PhysicsComponent& PhysicsComponent::setGravity(float value) {
	this->gravityTransformer = value;
	return *this;
}

void PhysicsComponent::update() {
	this->velocity.x += this->acceleration.x;
	this->velocity.y += this->acceleration.y;

	this->pos.x += this->velocity.x;
	this->pos.y += this->velocity.y;

	this->acceleration = {0, 0};
}

void PhysicsComponent::applyForce(const sf::Vector2f& force) {
	sf::Vector2f f = sf::Vector2f({0,0});

	if (this->mass <= 0) {
		this->mass = 1;
	}
	
	f.x += force.x / this->mass;
	f.y += force.y / this->mass;
	this->acceleration.x = f.x;
	this->acceleration.y = f.y;
}

void PhysicsComponent::clampsToObject(Object* parent, Object* other) {
	if (!parent->hasComponent<TextureComponent>() || !other->hasComponent<TextureComponent>()) {
		return;
	}

	auto pPos = parent->getComponent<TextureComponent>()->sprite->getGlobalBounds().position;
	auto oPos = other->getComponent<TextureComponent>()->sprite->getGlobalBounds().position;
	
	auto pSize = parent->getComponent<TextureComponent>()->sprite->getGlobalBounds().size;
	auto oSize = other->getComponent<TextureComponent>()->sprite->getGlobalBounds().size;

	sf::Vector2f rd_oPos(abs(oPos.x - (pPos.x + pSize.x)), abs(oPos.y - (pPos.y + pSize.y)));
	sf::Vector2f rd_oBound(abs((oPos.x + oSize.x) - pPos.x), abs((oPos.y + oSize.y) - pPos.y));

	if (rd_oPos.x < rd_oBound.x || rd_oPos.y < rd_oBound.y) {
		parent->getComponent<PhysicsComponent>()->pos = oPos + pSize;
	}
	else {
		parent->getComponent<PhysicsComponent>()->pos = oPos + oSize + pSize;
	}
}

Square::Square(sf::Vector2f pos, sf::Vector2f size, Layer* l) {
	float thickness = 10;

	auto line1 = Builder("line", l, 1).withTexture("bound").build();
	line1->setPos(pos);
	line1->getComponent<TextureComponent>()->setSize((unsigned)thickness, (unsigned)size.y);

	auto line2 = new Object(*line1);
	line2->setPos({ pos.x + size.x - thickness, pos.y });

	auto line3 = new Object(*line1);
	line3->getComponent<TextureComponent>()->setSize((unsigned)size.x, (unsigned)thickness);

	auto line4 = new Object(*line3);
	line4->setPos({ pos.x , pos.y + size.y - thickness });
}
