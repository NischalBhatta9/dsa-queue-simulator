#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <mutex>
#include <vector>

std::mutex lightMutex;

enum class Side { NONE, LEFT, RIGHT, TOP, BOTTOM };

class TrafficLight {
public:
    sf::RectangleShape shape;
    sf::Color colors[2] = { sf::Color::Red, sf::Color::Green };
    int state = 0; // 0 = Red, 1 = Green

    TrafficLight(float x, float y, float width, float height) {
        shape.setSize(sf::Vector2f(width, height));
        shape.setPosition(x, y);
        shape.setFillColor(colors[state]);
    }

};
class Road {
public:
    sf::RectangleShape shape;

    Road(float x, float y, float width, float height) {
        shape.setSize(sf::Vector2f(width, height));
        shape.setPosition(x, y);
        shape.setFillColor(sf::Color(50, 50, 50));
    }
};
class Car {
public:
    sf::RectangleShape shape;
    float speedX, speedY;
    bool isStraight;
    bool isRight;
    bool hasTurned;
    bool stopped;

    Car(float x, float y, float width, float height, float speedX, float speedY,
        bool isStraight, bool isRight, bool hasTurned = false)
        : speedX(speedX), speedY(speedY), isStraight(isStraight),
        isRight(isRight), hasTurned(hasTurned), stopped(false) {
        shape.setSize(sf::Vector2f(width, height));
        shape.setPosition(x, y);
        shape.setFillColor(sf::Color::Blue);
    }

    void move() { shape.move(speedX, speedY); }