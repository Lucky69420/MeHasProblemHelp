#include "gui.h"
#include "notHacks.h"
#include "globals.h"

#include <thread>

int __stdcall wWinMain(
	HINSTANCE instance,
	HINSTANCE previousInstance,
	PWSTR arguments,
	int commandShow)
{
	Memory mem{ "csgo.exe" };

	globals::clientAdress = mem.GetModuleAddress("client.dll");
	globals::engineAdress = mem.GetModuleAddress("engine.dll");

	std::thread(notHacks::VisualsThread, mem).detach();
	std::thread(notHacks::RCSThread, mem).detach();
	std::thread(notHacks::TrigerThread, mem).detach();
	std::thread(notHacks::AimThread, mem).detach();

	//loo gui
	gui::CreateHWindow("settings");
	gui::CreateDevice();
	gui::CreateImGui();

	while (gui::isRunning)
	{
		gui::BeginRender();
		gui::Render();
		gui::EndRender();

		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}
	//kustuta gui
	gui::DestroyImGui();
	gui::DestroyDevice();
	gui::DestroyHWindow();

	return EXIT_SUCCESS;
}