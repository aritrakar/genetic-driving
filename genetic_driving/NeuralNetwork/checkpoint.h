#ifndef _CHECKPOINT_H_
#define _CHECKPOINT_H_

#include <SFML/Graphics.hpp>
#include <array>
#include <myutils.h>

class Checkpoint {
public:
    Checkpoint(sf::Vector2f center, sf::Vector2f size, float orientation, int num)
        : checkpointNumber(num), rect(size), color(sf::Color::Red) {

        rect.setOrigin(size.x / 2, size.y / 2);
        rect.setRotation(static_cast<float>(orientation * (180.0f / M_PI)));
        rect.setPosition(center);
        rect.setFillColor(color);

        sf::Vector2f v1 = vectAngle(orientation);
        sf::Vector2f v2(-v1.y, v1.x); // Rotate v1 by 90 degrees

        v1 *= size.x / 2;
        v2 *= size.y / 2;

        pointsPosition[0] = center + v1 + v2;
        pointsPosition[1] = center + v1 - v2;
        pointsPosition[2] = center - v1 - v2;
        pointsPosition[3] = center - v1 + v2;
    }

    const sf::RectangleShape& getRect() const { return rect; }
    int getCheckpointNumber() const { return checkpointNumber; }
    const std::array<sf::Vector2f, 4>& getPointsPosition() const { return pointsPosition; }

//private:
    sf::RectangleShape rect;
    sf::Color color;
    int checkpointNumber;
    std::array<sf::Vector2f, 4> pointsPosition;
};

#endif
