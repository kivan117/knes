#include <SDL2/SDL.h>
#include <iostream>
#include <string>
#include "Stopwatch.h"
#include "Logger.h"
#include "Cpu.h"
#include "Mmu.h"
#include "MasterClock.h"

int main(int argc, char** argv)
{
	Logger::Init();

	stopwatch::Stopwatch watch;
	stopwatch::Stopwatch title_timer;
	uint64_t frame_mus, average_frame_mus = 1;
	uint64_t running_frame_times[60] = { 0 };
	uint8_t frame_time_index = 0;

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
	SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");
	window = SDL_CreateWindow("knes", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 512, 480, SDL_WINDOW_OPENGL);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	SDL_GL_SetSwapInterval(1);
	SDL_SetRenderDrawColor(renderer, 0xA0, 0xA0, 0xA0, 0xFF);
	SDL_RenderClear(renderer);
	SDL_RenderPresent(renderer);
	SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 256, 240);

	SDL_RendererInfo info;
	SDL_GetRendererInfo(renderer, &info);

	Mmu* mmu = new Mmu();
	std::ifstream romFile;
	romFile.open(argv[1], std::ios::binary | std::ios::in | std::ios::beg);
	if (!romFile.good())
	{
		LOG_ERROR("Error opening file.");
		exit(-1);
	}
	mmu->LoadCart(romFile);

	Cpu* cpu = new Cpu(mmu);

	MasterClock* clock = new MasterClock(cpu);

	while (!userQuit)
	{
		while (!clock->Tick()) {}
		
		SDL_RenderClear(renderer);
		SDL_RenderPresent(renderer); //just using vsync to limit run speed for now.

		frame_mus = watch.elapsed<stopwatch::mus>();

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

		running_frame_times[frame_time_index] = frame_mus;
		frame_time_index = (frame_time_index + 1) % 60;
		if (title_timer.elapsed<stopwatch::ms>() > 200)
		{

			title_timer.start();
			average_frame_mus = 0;
			for (int i = 0; i < 60; i++)
				average_frame_mus += running_frame_times[i];
			average_frame_mus = average_frame_mus / 60;
			static double fps;
			fps = (1000000.0 / (double)average_frame_mus);
			//titlestream = "";
			//titlestream.str(std::string());
			//titlestream.precision(4);
			//titlestream << "NES    FPS: ";
			//titlestream << fps;
			static std::string title;
			title = "NES    FPS: " + std::to_string(fps);
			SDL_SetWindowTitle(window, title.c_str());

		}
		watch.start();
	}

	return 0;
}