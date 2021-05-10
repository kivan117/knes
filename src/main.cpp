#include <SDL2/SDL.h>
#include <iostream>
#include "Logger.h"
#include "Cpu.h"
#include "Mmu.h"

int main(int argc, char** argv)
{
	Logger::Init();

	if (argc < 2)
	{
		LOG_ERROR("No rom file argument given.");
		exit(-1);
	}

	//bool enabledGamepad = true;
	//SDL_GameController* controller = NULL;

	//bool enabledAudio = true;

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		LOG_ERROR("SDL could not initialize! SDL_Error: {}", SDL_GetError());
		return -1;
	}
	//if (SDL_Init(SDL_INIT_AUDIO) < 0)
	//{
	//	std::cout << "SDL could not initialize audio! SDL_Error: " << SDL_GetError() << std::endl;
	//	enabledAudio = false;
	//}
	//if (SDL_Init(SDL_INIT_GAMECONTROLLER) < 0)
	//{
	//	std::cout << "SDL could not initialize gamepad subsystem. SDL_Error: " << SDL_GetError() << std::endl;
	//	enabledGamepad = false;
	//}

	//if (enabledGamepad)
	//{
	//	for (int i = 0; i < SDL_NumJoysticks(); ++i) {
	//		if (SDL_IsGameController(i)) {
	//			controller = SDL_GameControllerOpen(i);
	//			if (controller) {
	//				break;
	//			}
	//			else {
	//				std::cout << "Could not open gamecontroller " << i << ". " << SDL_GetError() << std::endl;
	//			}
	//		}
	//	}
	//}

	//if (controller)
	//{
	//	SDL_GameControllerAddMappingsFromFile("gamecontrollerdb.txt");
	//}


	SDL_Event e;

	bool userQuit = false;


	SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");
	SDL_SetHint(SDL_HINT_RENDER_BATCHING, "1");

	SDL_Window* window;

	SDL_Renderer* renderer;

	SDL_SetHint(SDL_HINT_RENDER_VSYNC, "0");
	window = SDL_CreateWindow("knes", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 512, 480, SDL_WINDOW_OPENGL);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	SDL_GL_SetSwapInterval(0);
	SDL_SetRenderDrawColor(renderer, 0xA0, 0xA0, 0xA0, 0xFF);
	SDL_RenderClear(renderer);
	SDL_RenderPresent(renderer);
	SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 256, 240);

	Mmu* mmu = new Mmu();
	std::ifstream romFile;
	romFile.open(argv[1], std::ios::binary | std::ios::in | std::ios::beg);
	if (!romFile.good())
	{
		LOG_ERROR("Error opening file.");
		exit(-1);
	}
	mmu->LoadCart(romFile);

	Cpu cpu(mmu);

	while (!userQuit)
	{

		cpu.Tick();

		while (SDL_PollEvent(&e))
		{
			switch (e.type)
			{
			case(SDL_QUIT):
			{
				userQuit = true;
				break;
			}
			default:
				break;
			}
		}
	}

	return 0;
}