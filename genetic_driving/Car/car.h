#ifndef _CAR_H_
#define _CAR_H_

#include <SFML/Graphics.hpp>
#include <vector>
#include "../NeuralNetwork/nn.h"
#include "../NeuralNetwork/checkpoint.h"

class Car {
public:
    void init(sf::Vector2f pos, sf::Vector2f ori) {
        position = pos;
        orientation = ori;
        size = sf::Vector2f(50.0f, 30.0f);
        speedVect = sf::Vector2f(0.0f, 0.0f);
        nn = NN();

        speedFactor = 1;
        lastCheckpoint = 0;
        score = 0.0f;
        dead = false;
        life = maxlife;

        rect = sf::RectangleShape(size);
        rect.setFillColor(HSVtoRGB(rand() % 360, 1, 1));
        size = sf::Vector2f(30, 10);
        rect.setSize(size);
        rect.setOrigin(sf::Vector2f(15, 5));
        rect.setRotation(static_cast<float>(angleVect(orientation) * 57.29));
        rect.setRotation(M_PI / 2);
    }

    void update(sf::Image& img, std::vector<Checkpoint>& checkpoints, std::vector<Car>& bestCars);
    void draw(sf::RenderWindow& window);
    float distWall(float angle, const sf::Image& img); // Angle relative to orientation
    void killCar(float addScore, std::vector<Car>& bestCars);

    bool isDead() const { return dead; }
    float getScore() const { return score; }
    float score;
    NN nn;
    bool dead = false;

//private:
    sf::Vector2f position;
    sf::Vector2f speedVect;
    sf::Vector2f orientation;
    sf::Vector2f size;
    sf::RectangleShape rect;

    const float minSpeed = 0.4f;
    float speedFactor; // Speed factor for the NN: maxspeed = minSpeed + nn * speedFactor
    float friction = 0.9f;
    const float maxlife = 70.0f / (friction * friction);
    const float viewDist = 300.0f;

    int lastCheckpoint = 0;
    
    float life; // Decreases over time, reset by checkpoints
};

#endif
