#include "stdafx.h"

unsigned QuadTree::smallestSize = 1500;
QuadTree* CollisionSystem::root{};
bool CollisionSystem::expandHappend{false};
bool CollisionSystem::splitHappend{false};

QuadTree::QuadTree(float x, float y, unsigned xs, unsigned ys, uint8_t c = 5) : xPos(x), yPos(y), xSize(xs), ySize(ys), cap(c) { }

QuadTree::~QuadTree() {
	this->elements.clear();
	delete this->ne;
	delete this->nw;
	delete this->se;
	delete this->sw;
	this->se = nullptr;
	this->ne = nullptr;
	this->sw = nullptr;
	this->nw = nullptr;
}

void QuadTree::reinsert(Object* obj = nullptr) {
	auto holder = this->elements;
	this->disabled = true;
	this->elements.clear();

	for (auto& e : holder) {
		e->getComponent<CollideComponent>()->removeTreeNode(this);
		this->insert(e);
	}
	if (obj) {
		obj->getComponent<CollideComponent>()->removeTreeNode(this);
		this->insert(obj);
	}
}

void QuadTree::insert(Object* e) {
	auto ePos = e->getComponent<PhysicsComponent>()->pos;
	auto eBounds = ePos + e->getComponent<TextureComponent>()->sprite->getGlobalBounds().size;

	if (this->cap <= this->elements.size() && !this->hasBeenSplit) {
		if (this->split()) {
			this->reinsert(e);
		}
	}
	else if ((this->hasBeenSplit || this->disabled) && !this->cannotBeSplit) {
		this->tryChild(ePos, eBounds, e);
	}
	else if (!this->disabled) {
		e->getComponent<CollideComponent>()->treeNodes.insert(this);
		this->elements.insert(e);
	}
}

bool QuadTree::containsCaseOverlap(const sf::Vector2f& pos, const sf::Vector2f& bound) const {
	return ((this->xPos >= pos.x && this->xPos + this->xSize <= bound.x && this->yPos < pos.y && this->yPos + this->ySize >= pos.y)
		|| (this->yPos >= pos.y && this->yPos + this->ySize <= bound.y && this->xPos < pos.x && this->xPos + this->xSize >= pos.x)
		|| (this->xPos >= pos.x && this->xPos + this->xSize <= bound.x && this->yPos >= pos.y && this->yPos + this->ySize <= bound.y));
}

bool QuadTree::containsCaseOverlap(float xPos, float yPos, float xBound, float yBound) const {
	return ((this->xPos > xPos || this->yPos > yPos) && (this->xPos + this->xSize < xBound || this->yPos + this->ySize < yBound));
}

bool QuadTree::contains(const sf::Vector2f& pos) const {
	return ((this->xPos < pos.x && this->yPos < pos.y) && (this->xPos + this->xSize >= pos.x && this->yPos + this->ySize >= pos.y));
}

bool QuadTree::contains(float xPos, float yPos) const {
	return ((this->xPos < xPos && this->yPos < yPos) && (this->xPos + this->xSize >= xPos && this->yPos + this->ySize >= yPos));
}

bool QuadTree::split() {
	CollisionSystem::splitHappend = true;
	this->hasBeenSplit = true;
	if (this->xSize <= smallestSize) {
		this->cannotBeSplit = true;
		return false;
	}

	if (!this->nw) {
		this->nw = new QuadTree(this->xPos, this->yPos, this->xSize / 2, this->ySize / 2);
	}
	if (!this->ne) {
		this->ne = new QuadTree(this->xPos + this->xSize / 2, this->yPos, this->xSize / 2, this->ySize / 2);
	}
	if (!this->sw) {
		this->sw = new QuadTree(this->xPos, this->yPos + this->ySize / 2, this->xSize / 2, this->ySize / 2);
	}
	if (!this->se) {
		this->se = new QuadTree(this->xPos + this->xSize / 2, this->yPos + this->ySize / 2, this->xSize / 2, this->ySize / 2);
	}

	return true;
}

CollisionSystem::CollisionSystem(unsigned xs, unsigned ys) {
	this->root = new QuadTree(0, 0, xs, ys);
}

CollisionSystem::~CollisionSystem() {
	delete this->root;
	this->root = nullptr;
}

void CollisionSystem::insert(std::vector<Object*> objs) {
	for (auto& e : objs) {
		insert(e);
	}
}

void CollisionSystem::insert(Object* e) {
	if (!e->hasComponent<CollideComponent>() || !e->hasComponent<PhysicsComponent>() || !e->hasComponent<TextureComponent>()) return;

	auto pos = e->getComponent<PhysicsComponent>()->pos;
	auto bound = e->getComponent<TextureComponent>()->sprite->getGlobalBounds().size + pos;

	if (root->contains(pos) || root->contains(bound) || root->containsCaseOverlap(pos, bound)) {
		root->insert(e);
	}
	if (!root->contains(pos) || !root->contains(bound)) {
		expand(pos, bound);
		insert(e);
	}
}

void CollisionSystem::expand(sf::Vector2f& pos, sf::Vector2f& bound) {
	if (root->xPos > pos.x || root->yPos > pos.y || root->xPos > bound.x || root->yPos > bound.y) {
		// expand towards nw
		LOG("expand nw");
		CollisionSystem::expandHappend = true;
		auto newRoot = new QuadTree(root->xPos - root->xSize, root->yPos - root->ySize, root->xSize * 2, root->ySize * 2);
		newRoot->se = root;
		root = newRoot;
		root->split();
		root->disabled = true;
	}
	if (root->xSize + root->xPos < bound.x || root->ySize + root->yPos < bound.y || root->xSize + root->xPos < pos.x || root->ySize + root->yPos < pos.y) {
		// expand towards se
		LOG("expand se");
		CollisionSystem::expandHappend = true;
		auto newRoot = new QuadTree(root->xPos, root->yPos, root->xSize * 2, root->ySize * 2);
		newRoot->nw = root;
		root = newRoot;
		root->split();
		root->disabled = true;
	}
}

void CollisionSystem::draw(Layer* l) {
	root->draw(l);
}

QuadTree* CollisionSystem::getNode(sf::Vector2f& pos, QuadTree* node) {
	if (!node) {
		node = root;
	}

	if (node->contains(pos)) {
		if (node->ne && node->ne->contains(pos)) {
			return getNode(pos, node->ne);
		}
		else if (node->nw && node->nw->contains(pos)) {
			return getNode(pos, node->nw);
		}
		else if (node->sw && node->sw->contains(pos)) {
			return getNode(pos, node->sw);
		}
		else if (node->se && node->se->contains(pos)) {
			return getNode(pos, node->se);
		}
		else {
			return node;
		}
	}
	else {
		return nullptr;
	}
}

void QuadTree::draw(Layer* l) {
	if (!this->s) {
		this->s = new Square({ this->xPos, this->yPos }, { (float)this->xSize, (float)this->xSize }, l);
	}
	if (this->hasBeenSplit && !this->cannotBeSplit) {
		this->ne->draw(l);
		this->nw->draw(l);
		this->se->draw(l);
		this->sw->draw(l);
	}
}

void QuadTree::tryChild(const sf::Vector2f& ePos, const sf::Vector2f& eBounds, Object* e) {
	if (this->ne->contains(ePos)) {
		this->ne->insert(e);
	}
	if (this->nw->contains(ePos)) {
		this->nw->insert(e);
	}
	if (this->se->contains(ePos)) {
		this->se->insert(e);
	}
	if (this->sw->contains(ePos)) {
		this->sw->insert(e);
	}

	if (this->ne->contains(eBounds)) {
		this->ne->insert(e);
	}
	if (this->nw->contains(eBounds)) {
		this->nw->insert(e);
	}
	if (this->se->contains(eBounds)) {
		this->se->insert(e);
	}
	if (this->sw->contains(eBounds)) {
		this->sw->insert(e);
	}

	if (this->ne->containsCaseOverlap(ePos, eBounds)) {
		this->ne->insert(e);
	}
	if (this->nw->containsCaseOverlap(ePos, eBounds)) {
		this->nw->insert(e);
	}
	if (this->se->containsCaseOverlap(ePos, eBounds)) {
		this->se->insert(e);
	}
	if (this->sw->containsCaseOverlap(ePos, eBounds)) {
		this->sw->insert(e);
	}
}

void CollisionSystem::info() {
	if (root) {
		root->info();
	}
}

void QuadTree::info() {
	std::cout << "curr: " << this << std::endl;
	std::cout << "children: " << std::endl;
	if (this->ne) {
		std::cout << "ne: " << this->ne << " ";
	}
	if (this->nw) {
		std::cout << "nw: " << this->nw << " ";
	}
	if (this->se) {
		std::cout << "se: " << this->se << " ";
	}
	if (this->sw) {
		std::cout << "sw: " << this->sw << " ";
	}

	std::cout << std::endl << std::endl;

	if (this->ne) {
		this->ne->info();
	}
	if (this->nw) {
		this->nw->info();
	}
	if (this->se) {
		this->se->info();
	}
	if (this->sw) {
		this->sw->info();
	}
}