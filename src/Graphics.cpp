#include "stdafx.h"

Renderer* Renderer::instance = nullptr;
sf::RenderWindow* Renderer::window = nullptr;
Camera* Renderer::currentCamera = nullptr;
std::string Renderer::windowName = "Window";
unsigned int Renderer::windowWidth = 700;
unsigned int Renderer::windowHeight = 1000;

Renderer::Renderer() {
	this->window = new sf::RenderWindow(sf::VideoMode({ this->windowWidth, this->windowHeight }), this->windowName);
	if (this->currentCamera) {
		this->window->setView(this->currentCamera->getView());
	}
}

Renderer::~Renderer() {
	LOG("deleting renderer.");

	delete this->window;
	this->window = nullptr;

	delete this->currentScene;
	this->currentScene = nullptr;
	
	LOG("finished deleting renderer.");
}

Renderer* Renderer::getInstance() {
	if (!instance) {
		instance = new Renderer();
	}
	return instance;
}

void Renderer::deleteInstance() {
	delete instance;
	instance = nullptr;
}

sf::RenderWindow* Renderer::getWindow() {
	if (window) {
		return window;
	}
	else {
		throw WindowError("Window hasn't been created yet.");
	}
}

void Renderer::setWindowName(std::string_view name) {
	if (!window) {
		windowName = name;
	}
	else {
		throw WindowError("Window has been already created!");
	}
}

void Renderer::setWindowSize(unsigned int width, unsigned int height) {
	if (!window) {
		windowWidth = width;
		windowHeight = height;
	}
	else {
		throw WindowError("Window has been already created!");
	}
}

void Renderer::setCamera(Camera* camera) {
	currentCamera = camera;
	if (window) {
		window->setView(camera->getView());
	}
	else {
		LOG("Window hasn't been created;");
	}
}

void Renderer::changeScene(Scene* newScene) {
	this->currentScene = newScene;
}

void Renderer::refreshScreen() {
	this->window->clear();
	if (this->currentScene) {
		this->currentScene->displayScene();
	}
	this->window->display();
}

Scene* Renderer::getScene() {
	return this->currentScene;
}

Scene::~Scene() {
	LOG("deleting scene.");

	for (auto& zInd : layers) {
		delete zInd.second;
	}
	this->layers.clear();

	LOG("finished deleting scene.");
}

void Layer::addObject(Object* obj, uint8_t zIndex) {
	if (obj) {
		this->objects[zIndex].emplace_back(obj);
	}
}

void Layer::removeObject(Object* obj, uint8_t zIndex) {
	for (auto i = objects[zIndex].begin(); i != objects[zIndex].end(); i++) {
		if (*i == obj) {
			objects[zIndex].erase(i);
		}
	}
}

void Layer::removeObject(std::string_view objName, uint8_t zIndex) {
	for (auto i = objects[zIndex].begin(); i != objects[zIndex].end(); i++) {
		if ((*i)->getName() == objName) {
			objects[zIndex].erase(i);
		}
	}
}

void Layer::reveal() {
	this->isVisible = true;
}

void Layer::hide() {
	this->isVisible = false;
}

void Layer::diplayLayer() {
	if (!this->isVisible) {
		return;
	}

	auto window = Renderer::getInstance()->getWindow();
	for (auto& zInd : objects) {
		if (!zInd.second.empty()) {
			for (auto& obj : zInd.second) {
				if (obj) {
					if (obj->hasComponent<TextureComponent>()) {
						obj->getComponent<TextureComponent>()->display(*window);
					}
				}
			}
		}
	}
}

Object* Layer::getObject(std::string_view name) {
	for (auto& zInd : this->objects) {
		if (!zInd.second.empty()) {
			for (auto& obj : zInd.second) {
				if (obj) {
					if (obj->getName() == name) {
						return obj;
					}
				}
			}
		}
	}
	throw ObjectError("Object wasn't found.");
}

uint8_t Layer::getObjectZIndex(Object* obj) {
	for (auto& zInd : this->objects) {
		if (!zInd.second.empty()) {
			if (find(zInd.second.begin(), zInd.second.end(), obj) != zInd.second.end()) {
				return zInd.first;
			}
		}
	}
	throw ObjectError("zIndex wasn't found.");
}

void Scene::addLayer(Layer* layer, uint8_t zIndex) {
	delete this->layers[zIndex];
	this->layers[zIndex] = layer;
}

void Scene::removeLayer(std::string_view name) {
	auto l = this->getLayer(name);
	if (l.second) {
		delete this->layers[l.first];
		this->layers[l.first] = nullptr;
	}
}

void Scene::revealLayer(std::string_view name) {
	auto l = this->getLayer(name);
	if (l.second) {
		this->layers[l.first]->reveal();
	}
}

void Scene::hideLayer(std::string_view name) {
	auto l = this->getLayer(name);
	if (l.second) {
		this->layers[l.first]->hide();
	}
}

std::pair<uint8_t, Layer*> Scene::getLayer(std::string_view name) {
	for (auto& layer : layers) {
		if (layer.second->getName() == name) {
			return {layer.first, layer.second};
		}
	}
	return {0, nullptr};
}

void Scene::setZIndex(Layer* layer, uint8_t newZIndex) {
	try {
		auto zInd = this->getLayerZIndex(layer);
		this->layers[zInd] = nullptr;
		this->layers[newZIndex] = layer;
	}
	catch (ObjectError e) {
		ERROR(e.what());
	}
}

uint8_t Scene::getLayerZIndex(Layer* layer) {
	for (auto& zInd : this->layers) {
		if (zInd.second == layer) {
			return zInd.first;
		}
	}
	throw ObjectError("Layer wasn't found.");
}

void Layer::onUpdate() {
	for (auto& zInd : this->objects) {
		if (!zInd.second.empty()) {
			for (auto& obj : zInd.second) {
				if (obj) {
					obj->onUpdate();
				}
			}
		}
	}
}

void Scene::onUpdate() {
	for (auto& layer : this->layers) {
		if (layer.second) {
			layer.second->onUpdate();
		}
	}
}

void Scene::displayScene() {
	for (auto& zInd : layers) {
		if (zInd.second) {
			zInd.second->diplayLayer();
		}
	}
}

Layer::Layer(std::string_view name, Scene* parent, uint8_t zIndex) :
	name(name), isVisible(true), parentScene(parent), zIndex(zIndex) {
	this->parentScene->addLayer(this, this->zIndex);
}

Layer::~Layer() {
	LOG("deleting " << this->name);

	this->parentScene = nullptr;
	for (auto& zInd : objects) {
		if (!zInd.second.empty()) {
			for (auto obj : zInd.second) {
				if (obj) {
					delete obj;
					obj = nullptr;
				}
			}
		}
	}
	this->objects.clear();

	LOG("finished deleting " << this->name);
}

void Layer::setZIndex(std::string_view name, uint8_t zIndex) {
	try {
		auto obj = this->getObject(name);
		this->setZIndex(obj, zIndex);
	}
	catch (ObjectError e) {
		ERROR(e.what());
	}
}

void Layer::setZIndex(Object* obj, uint8_t zIndex) {
	try {
		auto zInd = this->getObjectZIndex(obj);
		this->objects[zInd].erase(find(this->objects[zInd].begin(), this->objects[zInd].end(), obj));
		this->objects[zIndex].emplace_back(obj);
	}
	catch (ObjectError e) {
		ERROR(e.what());
	}
}

void Layer::setZIndex(uint8_t newZIndex) {
	this->zIndex = newZIndex;
	this->parentScene->setZIndex(this, this->zIndex);
}

void Layer::info() {
	for (auto& zInd : objects) {
		if (!zInd.second.empty()) {
			for (auto& obj : zInd.second) {
				if (obj) {
					LOG(obj->getName());
				}
			}
		}
	}
}

void Object::info() {
	LOG(this->getName() << " info -----------");
	if (this->hasComponent<HealthComponent>()) {
		LOG("current hp: " << this->getComponent<HealthComponent>()->currentHp);
		LOG("max hp: " << this->getComponent<HealthComponent>()->maxHp);
	}
	if (this->hasComponent<TextureComponent>()) {
		LOG("sprite: " << this->getComponent<TextureComponent>()->sprite);
		LOG("isVisible: " << this->getComponent<TextureComponent>()->isVisible);
	}
	if (this->hasComponent<CollideComponent>()) {
		LOG("canCollide: " << this->getComponent<CollideComponent>()->canCollide);
		LOG("prevPos: " << this->getComponent<CollideComponent>()->prevPos.x << " " << this->getComponent<CollideComponent>()->prevPos.y);
		for (auto& e : this->getComponent<CollideComponent>()->treeNodes) {
			LOG("\tnode: " << e << " " << e->disabled);
		}
	}
	LOG("---------------------------------");
}

const std::string& Layer::getName() const {
	return this->name;
}

void Layer::setName(std::string_view name) {
	this->name = name;
}

void Layer::setParent(Scene* parent) {
	if (parent) {
		this->parentScene = parent;
	}
}

Camera::Camera(float xSize, float ySize, float xPos, float yPos) {
	this->view = new sf::View(sf::FloatRect({ xPos, yPos }, { xSize, ySize }));
}

Camera::~Camera() {
	delete this->view;
	this->view = nullptr;
}

sf::View& Camera::getView() {
	return *this->view;
}

void Camera::moveByMouse(bool isDragging, Renderer* r) {
	if (isDragging) {
		sf::Vector2i mousePos = sf::Mouse::getPosition(*Renderer::getWindow());
		sf::Vector2f delta = Renderer::getWindow()->mapPixelToCoords(this->lastMousePos) - Renderer::getWindow()->mapPixelToCoords(mousePos);
		this->view->move(delta);
		this->lastMousePos = mousePos;
		Renderer::getWindow()->setView(*this->view);
		r->refreshScreen();
	}
}

void Camera::changeZoom(float value) {
	if (value > 0) {
		this->view->zoom(0.8f);
	}
	else {
		this->view->zoom(1.2f);
	}
	Renderer::getWindow()->setView(*this->view);
}
