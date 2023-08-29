// --------------------------------------------
// ParticleEffects.h
// ParticleEffects.cpp
// --------------------------------------------
// Test particle effects on multiple threads.
// --------------------------------------------

#ifndef PARTICLEEFFECTS_H
#define PARTICLEEFFECTS_H

#include <SFML/Graphics.hpp>

#include "imgui.h"
#include "imgui-SFML.h"
#include "ThreadPool.h"
#include "DefaultParticle.h"

struct Generator
{
	Generator(sf::Vector2f &startPosition, sf::Time &dt, int &numOfParticles, std::vector<uint8_t> *pixelsPtr) 
		: startPosition(startPosition), dt(dt), numOfParticles(numOfParticles), pixelsPtr(pixelsPtr) { }

	std::vector<Particle*> particlePool;
	sf::Vector2f &startPosition;
	sf::Time &dt;
	int &numOfParticles;
	std::vector<uint8_t> *pixelsPtr;

	void update(int scrW)
	{
		if (numOfParticles > particlePool.size())
		{
			numOfParticles = particlePool.size();
		}

		for (int i = 0; i < numOfParticles; ++i)
		{
			if (particlePool[i]->alive == false)
			{
				particlePool[i]->generate(startPosition);
			}
			else
			{
				particlePool[i]->update(dt, *pixelsPtr, scrW);
			}
		}
	}
};

class ParticleEffects
{
public:
	ParticleEffects(sf::Time &dt);
	~ParticleEffects();
	void update(const sf::Time &dt);
	void handleUI();

private:
	std::vector<uint8_t> pixels;
	std::vector<uint8_t> *pixelsPtr;
	std::unique_ptr<ThreadPool> threadPool;
	int numOfParticles = 1000;
	int maxNumOfParticles = 10000;
	sf::Vector2f startPosition;
	ImVec2 renderWindowSize{ 1280, 720 };
	std::vector<Generator*> threadGens;
	sf::Time &dt;
	std::unique_ptr<sf::Texture> renderTexture;
	int scrW;
	bool multiThreaded = false;
};

#endif // !PARTICLEEFFECTS_H