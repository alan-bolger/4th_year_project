#include "Application.h"

/// <summary>
/// Performance Comparison of Single-threaded and Multi-threaded Computations in Game Development.
/// By Alan Bolger.
/// Date started: 20.07.2023
/// Data finished: TBC
/// </summary>
/// <returns>0 for successful exit.</returns>
int WinMain()
{
	Application *app = new Application();
	app->start();

	delete app;

	return 0;
}