// --------------------------------------------
// | Main.cpp								  |
// --------------------------------------------

#include "Application.h"

/// <summary>
/// Performance Comparison of Single-threaded and Multi-threaded Computations in Game Development.
/// By Alan Bolger.
/// 
/// All code by Alan Bolger unless otherwise stated.
/// 
/// Student ID: C00232036
/// Date started: 20.07.2023
/// Date finished: TBC
/// </summary>
/// <returns>0 for successful exit.</returns>
int main()
{
	Application *app = new Application();
	app->start();

	delete app;

	return 0;
}