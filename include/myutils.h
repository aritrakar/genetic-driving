#ifndef _MYUTILS_H_
#define _MYUTILS_H_

#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>

#ifndef M_PI
#define M_PI 3.141592 // 65358979323846
#endif

#ifndef MAX_PIXEL_X
#define MAX_PIXEL_X 1920 // 1920.0f
#endif

#ifndef MAX_PIXEL_Y
#define MAX_PIXEL_Y 1080 // 1080.0f
#endif

inline float dist(float x1, float y1, float x2, float y2) {
	return std::sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

inline sf::Vector2f addVectors2f(const sf::Vector2f& v1, const sf::Vector2f& v2) {
	return sf::Vector2f(v1.x + v2.x, v1.y + v2.y);
}

inline sf::Vector2f subVectors2f(const sf::Vector2f& v1, const sf::Vector2f& v2) {
	return sf::Vector2f(v1.x - v2.x, v1.y - v2.y);
}

inline sf::Vector2f multVectors2f(const sf::Vector2f& v1, const sf::Vector2f& v2) {
	return sf::Vector2f(v1.x * v2.x, v1.y * v2.y);
}

inline sf::Vector2f normalizeVector2f(const sf::Vector2f& v) {
	float d = dist(0, 0, v.x, v.y);
	return (d != 0.0f) ? sf::Vector2f(v.x / d, v.y / d) : sf::Vector2f(0, 0);
}

inline float dotProductVectors2f(const sf::Vector2f& v1, const sf::Vector2f& v2) {
	return v1.x * v2.x + v1.y * v2.y;
}

inline float angleVect(const sf::Vector2f& v) {
	return std::atan2(v.y, v.x);
}

inline sf::Vector2f vectAngle(float a) {
	return sf::Vector2f(std::cos(a), std::sin(a));
}

inline sf::Color HSVtoRGB(float H, float S, float V) {   // h:0-360.0, s:0.0-1.0, v:0.0-1.0
	S *= 100; V *= 100;
	if (H > 360 || H < 0 || S>100 || S < 0 || V>100 || V < 0) {
		std::cout << "The given HSV values are not in valid range" << "\n";
		return sf::Color(0, 0, 0);
	}
	float s = S / 100;
	float v = V / 100;
	float C = s * v;
	double X = C * (1 - fabs(fmod(H / 60.0, 2) - 1));
	float m = v - C;
	float r, g, b;
	if (H >= 0 && H < 60) {
		r = C, g = X, b = 0;
	}
	else if (H >= 60 && H < 120) {
		r = X, g = C, b = 0;
	}
	else if (H >= 120 && H < 180) {
		r = 0, g = C, b = X;
	}
	else if (H >= 180 && H < 240) {
		r = 0, g = X, b = C;
	}
	else if (H >= 240 && H < 300) {
		r = X, g = 0, b = C;
	}
	else {
		r = C, g = 0, b = X;
	}
	int R = static_cast<int>((r + m) * 255);
	int G = static_cast<int>((g + m) * 255);
	int B = static_cast<int>((b + m) * 255);

	return sf::Color(R, G, B);
}

#endif
