#include "Game.h"
#include "TextureManager.h"
#include "Map.h"
#include "EntityComponent\Components.h"

Map* map1;
SDL_Renderer* Game::renderer = nullptr;
Manager manager;
auto& newPlayer(manager.addEntity());

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
	newPlayer.addComponents<TransformComponent>(50,30);
	newPlayer.addComponents<SpriteComponent>("assets/Player.png");
}

void Game::handleEvents() {
	SDL_Event event;
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
	newPlayer.getComponent<TransformComponent>().position.add(Vector2D(5, 0));
	if (newPlayer.getComponent<TransformComponent>().position.x > 200) {
		newPlayer.getComponent<SpriteComponent>().setTexture("assets/Player2.png");
	}
	std::cout << newPlayer.getComponent<TransformComponent>().position;
}

void Game::render(){
	SDL_RenderClear(renderer);
	map1->drawMap();
	manager.draw();
	SDL_RenderPresent(renderer);
}

void Game::clean(){
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
	std::cout << "Game cleaned" << std::endl;
}

