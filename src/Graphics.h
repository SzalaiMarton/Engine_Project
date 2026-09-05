#pragma once

#include <map>

class Layer;
class Scene;
class Object;
class Object;
class Camera;

class Layer {
private:
	std::string name;
	bool isVisible;
	Scene* parentScene;
	uint8_t zIndex;
	std::unordered_map<uint8_t, std::vector<Object*>> objects;
public:
	Layer(std::string_view name, Scene* parent, uint8_t zIndex);
	~Layer();

	void setZIndex(std::string_view name, uint8_t zIndex);
	void setZIndex(Object* obj, uint8_t zIndex);
	void setZIndex(uint8_t newZIndex);
	void setName(std::string_view name);
	void setParent(Scene* parent);

	void addObject(Object* obj, uint8_t zIndex);
	void removeObject(Object* obj, uint8_t zIndex);
	void removeObject(std::string_view objName, uint8_t zIndex);
	void reveal();
	void hide();
	void diplayLayer();
	void onUpdate();

	Object* getObject(std::string_view name);
	uint8_t getObjectZIndex(Object* obj);
	const std::string& getName() const;
	void info();
};


class Scene {
private:
	std::unordered_map<uint8_t, Layer*> layers;
public:
	Scene() = default;
	~Scene();
	
	void addLayer(Layer* layer, uint8_t zIndex);
	void removeLayer(std::string_view name);
	void revealLayer(std::string_view name);
	void hideLayer(std::string_view name);
	std::pair<uint8_t, Layer*> getLayer(std::string_view name);
	void setZIndex(Layer* layer, uint8_t newZIndex);
	uint8_t getLayerZIndex(Layer* layer);

	void onUpdate();
	void displayScene();
};

class Renderer {
private:
	static Renderer* instance;
	static Camera* currentCamera;
	static sf::RenderWindow* window;
	static std::string windowName;
	static unsigned int windowWidth;
	static unsigned int windowHeight;
	Scene* currentScene = nullptr;

	Renderer();
	~Renderer();

public:
	Renderer(const Renderer&) = delete;
	Renderer& operator=(const Renderer&) = delete;

	static Renderer* getInstance();
	static void deleteInstance();
	static void setWindowName(std::string_view name);
	static void setWindowSize(unsigned int windowWidth, unsigned int windowHeight);
	static void setCamera(Camera* camera);

	static sf::RenderWindow* getWindow();
	static Camera* getCamera();
	void changeScene(Scene* newScene);
	void refreshScreen();
	Scene* getScene();
};

class Camera {
private:
	sf::View* view;
public:
	sf::Vector2i lastMousePos{};

	Camera(float xSize, float ySize, float xPos = 0.0f, float yPos = 0.0f);
	~Camera();

	sf::View& getView();
	sf::Vector2f getTopLeft();
	sf::Vector2f getBotRight();
	bool isOnScreen(Object* e);
	void moveByMouse(bool isDragging, Renderer* r);
	void changeZoom(float value);
};
