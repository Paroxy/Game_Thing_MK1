#include "Game.h"
#include "TextureManager.h"
#include "Map.h"
#include "EntityComponent\Components.h"
#include "Collision.h"

Map* map1;
SDL_Renderer* Game::renderer = nullptr;
SDL_Event Game::event;
Manager manager;
std::vector<CollisionComponent*> Game::colliders;
auto& newPlayer(manager.addEntity());
auto& boundary(manager.addEntity());

enum groupIDs : std::size_t {
	mapGroup,
	playerGroup,
	enemyGroup,
	colliderGroup
};

Game::Game(){

}


Game::~Game(){

}

void Game::init(const char * title, int xPos, int yPos, int width, int height, bool fullScreen){

	int flags = 0;
	if (fullScreen) flags = SDL_WINDOW_FULLSCREEN;

	if (SDL_Init(SDL_INIT_EVERYTHING) == 0) {
		std::cout << "Subsystems Initialized" << std::endl;
		window = SDL_CreateWindow(title, xPos, yPos, width, height, flags);
		if (window) {
			std::cout << "Window created!" << std::endl;
		}
		renderer = SDL_CreateRenderer(window, -1, 0);
		if (renderer) {
			SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
			std::cout << "Renderer created!" << std::endl;
		}

		isRunning = true;
	}
	else {
		isRunning = false;
	}

	map1 = new Map();
	Map::loadMap("assets/map1_16x16.map",16,16);
	newPlayer.addComponents<TransformComponent>(2);
	newPlayer.addComponents<SpriteComponent>("assets/Player_idle.png", 4, 200);
	newPlayer.addComponents<KeyboardController>();
	newPlayer.addComponents<CollisionComponent>("Me");
	newPlayer.addGroup(playerGroup);

	boundary.addComponents<TransformComponent>(400.0f, 400.0f, 400, 30, 1);
	boundary.addComponents<SpriteComponent>("assets/dirt.png");
	boundary.addComponents<CollisionComponent>("Wall");
	boundary.addGroup(mapGroup);
}

void Game::handleEvents() {
	
	SDL_PollEvent(&event);
	switch (event.type) {
	case SDL_QUIT:
		isRunning = false;
		break;
	default:
		break;
	}
}


void Game::update(){
	count++;
	manager.refresh();
	manager.update();

	for (auto collider : colliders) {
		Collision::AABBCollision(newPlayer.getComponent<CollisionComponent>(), *collider);
	}
	//std::cout << "Tick: " << count << ", " << newPlayer.getComponent<TransformComponent>().position;
}

auto& tiles(manager.getGroup(mapGroup));
auto& players(manager.getGroup(playerGroup));
auto& enemies(manager.getGroup(enemyGroup));

void Game::render(){
	SDL_RenderClear(renderer);
	for (auto& tile : tiles) {
		tile->draw();
	}
	for (auto& player : players) {
		player->draw();
	}
	for (auto& enemy : enemies) {
		enemy->draw();
	}
	SDL_RenderPresent(renderer);
}

void Game::clean(){
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
	std::cout << "Game cleaned" << std::endl;
}

void Game::addTile(int ID, int xPos, int yPos){
	auto& tile(manager.addEntity());
	tile.addComponents<TileComponent>(xPos, yPos, 32, 32, ID);
	tile.addGroup(mapGroup);
}

