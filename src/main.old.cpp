#include "stdafx.h"
#include <chrono>

// testing ground
int main() {
	auto mainView = Camera(1600, 900, 0, 0);
	auto collSys = CollisionSystem(5000,5000);
	Renderer::setCamera(&mainView);
	Renderer::setWindowSize(1280, 720);
	auto renderer = Renderer::getInstance();
	Textures::loadTextures();
	
	auto scene = new Scene();
	auto layer = new Layer("main_layer", scene, 1);

	auto obj1 = Builder("obj1", layer, 2).withTexture("sad").withHealth(100).withPhysics().withPos({ -100, -100 }).withCollision(true).build();
	obj1->getComponent<TextureComponent>()->setSize(100, 100);
	obj1->getComponent<PhysicsComponent>()->setMass(100);

	auto obj2 = new Object(*obj1);
	obj2->setName("obj2");
	obj2->getComponent<PhysicsComponent>()->setGravity(0);
	obj2->setPos({ -1000, 200 });
	obj2->getComponent<CollideComponent>()->setCollision(true);
	
	auto obj3 = new Object(*obj1); 
	obj3->setName("obj3");
	obj3->setPos({-1400, -800});
	obj3->getComponent<PhysicsComponent>()->setGravity(1);
	obj3->getComponent<CollideComponent>()->continuousCollisionChecks = true;

	auto obj4 = new Object(*obj1);
	obj4->setName("obj4");
	obj4->setPos({ 100, 800 });
	obj4->getComponent<PhysicsComponent>()->setGravity(1);

	auto obj5 = new Object(*obj1);
	obj5->setName("obj5");
	obj5->setPos({ 250, 800 });
	obj5->getComponent<PhysicsComponent>()->setGravity(1);

	auto obj6 = new Object(*obj1);
	obj6->setName("obj6");
	obj6->setPos({ -16000, 4000 });
	obj6->getComponent<TextureComponent>()->setSize(170, 60);
	obj6->getComponent<PhysicsComponent>()->setGravity(0);

	auto obj7 = new Object(*obj1);
	obj7->setName("obj7");
	obj7->setPos({ -16000, 4100 });
	obj7->getComponent<TextureComponent>()->setSize(17000, 60);
	obj7->getComponent<PhysicsComponent>()->setGravity(0);

	std::vector<Object*> allObjs = { obj1, obj2, obj3, obj4, obj5, obj6, obj7 };

	CollisionSystem::insert(allObjs);
	CollisionSystem::draw(layer);
	
	auto lastChecked = std::chrono::high_resolution_clock::now();

	renderer->changeScene(scene);
	renderer->getWindow()->setFramerateLimit(60);

	bool isDragging = false;

	while (renderer->getWindow()->isOpen()) {
		CollisionSystem::expandHappend = false;
		CollisionSystem::splitHappend = false;

		while (const std::optional event = renderer->getWindow()->pollEvent()) {
			if (event->is<sf::Event::Closed>()) {
				renderer->getWindow()->close();
			}

			if (event->is<sf::Event::MouseButtonPressed>()) {
				isDragging = true;
				mainView.lastMousePos = sf::Mouse::getPosition(*Renderer::getWindow());

				auto mouseP = Renderer::getWindow()->mapPixelToCoords(sf::Mouse::getPosition(*Renderer::getWindow()));
				for (auto& e : allObjs) {
					if (e->hasComponent<TextureComponent>()) {
						if (e->getComponent<TextureComponent>()->sprite->getGlobalBounds().contains(mouseP)) {
							e->info();
						}
					}
				}
				auto retrievedNode = CollisionSystem::getNode(mouseP, nullptr);

				if (retrievedNode) {
					LOG(retrievedNode);
				}
			}

			if (event->is<sf::Event::MouseButtonReleased>()) {
				isDragging = false;
			}

			if (event->is<sf::Event::MouseWheelScrolled>()) {
				const auto& mouseWheelEvent = event->getIf<sf::Event::MouseWheelScrolled>();
				mainView.changeZoom(mouseWheelEvent->delta);
			}
		}

		auto now = std::chrono::high_resolution_clock::now();
		auto deltaTime = std::chrono::duration<double>(now - lastChecked);
		lastChecked = now;

		renderer->getScene()->onUpdate();

		if (CollisionSystem::expandHappend || CollisionSystem::splitHappend) {
			CollisionSystem::draw(layer);
		}

		mainView.moveByMouse(isDragging, renderer);

		renderer->refreshScreen();
	}

	renderer->deleteInstance();

	return 0;
}