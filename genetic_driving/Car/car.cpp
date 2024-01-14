#include <algorithm>

#include "car.h"
//#include <myutils.h>

bool pointInRect(sf::Vector2f& p1, Checkpoint& c) {
    //check if p1 is in a checkpoint
    sf::Vector2f a, b, d; a = c.pointsPosition[0]; b = c.pointsPosition[1]; d = c.pointsPosition[3];
    float bax = b.x - a.x;
    float bay = b.y - a.y;
    float dax = d.x - a.x;
    float day = d.y - a.y;

    if ((p1.x - a.x) * bax + (p1.y - a.y) * bay < 0) return false;
    if ((p1.x - b.x) * bax + (p1.y - b.y) * bay > 0) return false;
    if ((p1.x - a.x) * dax + (p1.y - a.y) * day < 0) return false;
    if ((p1.x - d.x) * dax + (p1.y - d.y) * day > 0) return false;

    return true;
}

//const float PI = 3.141592f;
//const float MAX_PIXEL_X = 1920.0f;
//const float MAX_PIXEL_Y = 1080.0f;

#ifndef MAX_PIXEL_X
#define MAX_PIXEL_X 1920 // 1920.0f
#endif

#ifndef MAX_PIXEL_Y
#define MAX_PIXEL_Y 1080 // 1080.0f
#endif


// Q: Should this be `const`?
void Car::draw(sf::RenderWindow& window) {
	rect.setRotation(angleVect(orientation) * 57.29); // 1 rad
	rect.setPosition(position);

	window.draw(rect);
}

//float Car::distWall(float angle, const sf::Image& img) {
//    sf::Vector2f direction = normalizeVector2f(vectAngle(orientation + angle));
//    float distance = 0.0f;
//    const float increment = 5.0f;
//
//    while (distance < viewDist) {
//        distance += increment;
//        sf::Vector2f testPoint = position + distance * direction;
//
//        // Clamping the testPoint to the size of the image
//        testPoint.x = std::clamp(testPoint.x, 0.0f, static_cast<float>(img.getSize().x - 1));
//        testPoint.y = std::clamp(testPoint.y, 0.0f, static_cast<float>(img.getSize().y - 1));
//
//        if (img.getPixel(static_cast<unsigned int>(testPoint.x), static_cast<unsigned int>(testPoint.y)) == sf::Color::Black) {
//            break;
//        }
//    }
//
//    return distance;
//}

float Car::distWall(float angle, const sf::Image& img) {
	float a = angleVect(orientation) + angle;

	sf::Vector2f tp;
	sf::Vector2f vect = sf::Vector2f(cos(a), sin(a));
	float dist = 5;
	bool test = false;

	while (dist < viewDist and !test) {
		dist *= 1.5;
		tp.x = vect.x * dist + position.x;
		tp.y = vect.y * dist + position.y;
		test = img.getPixel(fmax(0, fmin(tp.x, MAX_PIXEL_X - 1)), fmax(0, fmin(tp.y, MAX_PIXEL_Y - 1))) == sf::Color(0, 0, 0);
	}

	return dist;
}

void Car::killCar(float addScore, std::vector<Car>& BestCars) {
    score += addScore / 3;
    dead = true;

    for (int i = 0; i < BestCars.size(); i++) {
        if (score > BestCars[i].score) {
            BestCars[i].nn = nn;
            BestCars[i].score = score;
            return;
        }
    }
}

/*
void Car::updateStateBasedOnNNOutput(const std::vector<float>& ans) {
    float speedC = 0.4 + abs(ans[0]) / 2;
    float angle = ans[1] / 10; // ANGLE_SCALE;
    orientation = vectAngle(angleVect(orientation) + angle);
    speedVect.x += speedC * orientation.x * speedFactor;
    speedVect.y += speedC * orientation.y * speedFactor;
    speedVect.x *= friction;
    speedVect.y *= friction;

    // not sure if this will work because these are vectors
    // position += speedVect;
    position = addVectors2f(position, speedVect);
}

void Car::handleCollision(const sf::Image& img, std::vector<Car>& BestCars) {
    sf::Vector2f v1 = orientation;
    sf::Vector2f v2 = sf::Vector2f(orientation.y * -1, orientation.x); //v1 + pi/2

    // Scale the vectors so they can reach the angles of the rectangle
    v1.x *= size.x / 2; v1.y *= size.x / 2;
    v2.x *= size.y / 2; v2.y *= size.y / 2;

    sf::Vector2f p1, p2;
    sf::Color col;
    
    // Front angles
    p1 = addVectors2f(addVectors2f(position, v1), v2);
    col = img.getPixel(fmax(0, fmin(p1.x, MAX_PIXEL_X)), fmax(0, fmin(p1.y, MAX_PIXEL_Y)));
    if (col == sf::Color(0, 0, 0))
        killCar((maxlife - life) / 2, BestCars);

    p2 = subVectors2f(addVectors2f(position, v1), v2);
    col = img.getPixel(fmax(0, fmin(p2.x, MAX_PIXEL_X)), fmax(0, fmin(p2.y, MAX_PIXEL_Y)));
    if (col == sf::Color(0, 0, 0))
        killCar((maxlife - life) / 2, BestCars);

    // Back angles, useful if the car can go backward or drift. Disable this part else
    sf::Vector2f p = addVectors2f(subVectors2f(position, v1), v2);
    col = img.getPixel(fmax(0, fmin(p.x, MAX_PIXEL_X)), fmax(0, fmin(p.y, MAX_PIXEL_Y)));
    if (col == sf::Color(0, 0, 0))
        killCar((maxlife - life) / 2, BestCars);

    p = subVectors2f(subVectors2f(position, v1), v2);
    col = img.getPixel(fmax(0, fmin(p.x, MAX_PIXEL_X)), fmax(0, fmin(p.y, MAX_PIXEL_Y)));
    if (col == sf::Color(0, 0, 0))
        killCar((maxlife - life) / 2, BestCars);

    // Handle checkpoint
    if (lastCheckpoint < checkpoints.size()) {
        if (pointInRect(p1, checkpoints[lastCheckpoint]) or pointInRect(p2, checkpoints[lastCheckpoint])) {
            score += life;
            life = maxlife;
            lastCheckpoint += 1;
            if (lastCheckpoint == checkpoints.size()) {
                score *= 1.2;
                killCar(0, BestCars);
            }
        }
    }

    life -= !dead * (speedFactor + speedC) / 6;
}*/

void Car::update(sf::Image& img, std::vector<Checkpoint>& checkpoints, std::vector<Car>& BestCars) {
    // std::cout << "Updating car. dead: " << dead << ", life: " << life << std::endl;

    // Check if the car is dead
    if (!dead && life <= 0) {
        killCar(maxlife / 3, BestCars);
    }

    if (!dead) {
        // Prepare Neural Network Inputs (example: distance to walls at various angles)
        std::vector<float> nnInputs = {
            distWall(-M_PI / 3, img),
            distWall(-M_PI / 4, img),
            distWall(-M_PI / 6, img),
            distWall(0, img),
            distWall(M_PI / 6, img),
            distWall(M_PI / 4, img),
            distWall(M_PI / 3, img),
            angleVect(orientation) / 10,
            speedVect.x / 5,
            speedVect.y / 5
        };

        // Forward Propagate through Neural Network
        std::vector<float> nnOutputs = nn.forward_propagate(nnInputs);

        //updateStateBasedOnNNOutput(nnOutputs);

        float speedC = 0.4 + abs(nnOutputs[0]) / 2;
        float angle = nnOutputs[1] / 10; // ANGLE_SCALE;
        orientation = vectAngle(angleVect(orientation) + angle);
        speedVect.x += speedC * orientation.x * speedFactor;
        speedVect.y += speedC * orientation.y * speedFactor;
        speedVect.x *= friction;
        speedVect.y *= friction;

        // not sure if this will work because these are vectors
        // position += speedVect;
        position = addVectors2f(position, speedVect);

        //handleCollision(img, BestCars);

        sf::Vector2f v1 = orientation;
        sf::Vector2f v2 = sf::Vector2f(orientation.y * -1, orientation.x); // v1 + pi/2

        // Scale the vectors so they can reach the angles of the rectangle
        v1.x *= size.x / 2; v1.y *= size.x / 2;
        v2.x *= size.y / 2; v2.y *= size.y / 2;

        sf::Vector2f p1, p2;
        sf::Color col;

        // Front angles
        p1 = addVectors2f(addVectors2f(position, v1), v2);
        col = img.getPixel(fmax(0, fmin(p1.x, MAX_PIXEL_X)), fmax(0, fmin(p1.y, MAX_PIXEL_Y)));
        if (col == sf::Color(0, 0, 0))
            killCar((maxlife - life) / 2, BestCars);

        p2 = subVectors2f(addVectors2f(position, v1), v2);
        col = img.getPixel(fmax(0, fmin(p2.x, MAX_PIXEL_X)), fmax(0, fmin(p2.y, MAX_PIXEL_Y)));
        if (col == sf::Color(0, 0, 0))
            killCar((maxlife - life) / 2, BestCars);

        // Back angles, useful if the car can go backward or drift. Disable this part else
        sf::Vector2f p = addVectors2f(subVectors2f(position, v1), v2);
        col = img.getPixel(fmax(0, fmin(p.x, MAX_PIXEL_X)), fmax(0, fmin(p.y, MAX_PIXEL_Y)));
        if (col == sf::Color(0, 0, 0))
            killCar((maxlife - life) / 2, BestCars);

        p = subVectors2f(subVectors2f(position, v1), v2);
        col = img.getPixel(fmax(0, fmin(p.x, MAX_PIXEL_X)), fmax(0, fmin(p.y, MAX_PIXEL_Y)));
        if (col == sf::Color(0, 0, 0))
            killCar((maxlife - life) / 2, BestCars);

        // Handle checkpoint
        if (lastCheckpoint < checkpoints.size()) {
            if (pointInRect(p1, checkpoints[lastCheckpoint]) or pointInRect(p2, checkpoints[lastCheckpoint])) {
                score += life;
                life = maxlife;
                lastCheckpoint += 1;
                if (lastCheckpoint == checkpoints.size()) {
                    score *= 1.2;
                    killCar(0, BestCars);
                }
            }
        }

        life -= !dead * (speedFactor + speedC) / 6;
    }
}
