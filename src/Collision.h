#pragma once

class Object;

class QuadTree {
public:
	static unsigned smallestSize;

	float xPos, yPos;
	unsigned xSize, ySize;
	std::set<Object*> elements{};
	QuadTree* ne{};
	QuadTree* nw{};
	QuadTree* se{};
	QuadTree* sw{};
	Square* s{};
	bool hasBeenSplit = false; // if the tree had been split
	bool disabled = false;	// if the tree had been reinserted -> cannot hold elements
	bool cannotBeSplit = false;	// if the size has reached the limit
	uint8_t cap;

	QuadTree(float x, float y, unsigned xs, unsigned ys, uint8_t c);
	~QuadTree();

	void reinsert(Object* e);
	void insert(Object* e);
	bool containsCaseOverlap(const sf::Vector2f& pos, const sf::Vector2f& bound) const; // true if either x or y axis is in the target object 
	bool containsCaseOverlap(float xPos, float yPos, float xBound, float yBound) const; // true if either x or y axis is in the target object 
	bool contains(const sf::Vector2f& pos) const;
	bool contains(float xPos, float yPos) const;
	bool split();
	void draw(Layer* l);
	void tryChild(const sf::Vector2f& ePos, const sf::Vector2f& eBound, Object* e);
	void info();
};

class CollisionSystem {
public:
	static QuadTree* root;
	static bool expandHappend;
	static bool splitHappend;

	CollisionSystem(unsigned xs, unsigned ys);
	~CollisionSystem();

	static void insert(Object* e);
	static void insert(std::vector<Object*> e);
	static void draw(Layer* l);
	static QuadTree* getNode(sf::Vector2f& pos, QuadTree* node = nullptr);
	static void expand(sf::Vector2f& pos, sf::Vector2f& bound);
	static void info();
};