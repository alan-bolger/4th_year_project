// --------------------------------------------
// ParticleEffect.h
// ParticleEffect.cpp
// --------------------------------------------
// Test particle effect on multiple threads.
// --------------------------------------------

#ifndef PARTICLEEFFECT_H
#define PARTICLEEFFECT_H

#include <SFML/Graphics.hpp>

#include "imgui.h"
#include "imgui-SFML.h"
#include "ThreadPool.h"
#include "DefaultParticle.h"

struct Generator
{
	Generator(sf::Vector2f &startPosition, sf::Time &dt, int &numOfParticles, float &speed, float &timeToLive, std::vector<uint8_t> *pixelsPtr) 
		: startPosition(startPosition), dt(dt), numOfParticles(numOfParticles), speed(speed), timeToLive(timeToLive), pixelsPtr(pixelsPtr) { }

	sf::Vector2f &startPosition;
	sf::Time &dt;
	int &numOfParticles;
	float &speed;
	float &timeToLive;
	std::vector<uint8_t> *pixelsPtr;
	std::vector<Particle*> particlePool;	

	void update(int scrW, int threadAmount)
	{
		if ((numOfParticles / threadAmount) > particlePool.size())
		{
			numOfParticles = particlePool.size();
		}

		for (int i = 0; i < (numOfParticles / threadAmount); ++i)
		{
			if (particlePool[i]->alive == false)
			{
				particlePool[i]->generate(startPosition, speed, timeToLive);
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
	int threadAmount = 0;
	int numOfParticles = 500000;
	int particlesPerThread = 0;
	int maxNumOfParticles = 1000000;
	sf::Vector2f startPosition;
	ImVec2 renderWindowSize{ 1280, 720 };
	std::vector<Generator*> threadGens;
	sf::Time &dt;
	std::unique_ptr<sf::Texture> renderTexture;
	int scrW;
	bool multiThreaded = false;
	float speed = 180.0f;
	float timeToLive = 20.0f;
};

#endif // !PARTICLEEFFECTS_H