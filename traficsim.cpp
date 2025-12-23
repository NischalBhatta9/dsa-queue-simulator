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

        bool isRed() { return state == 0; }
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
    bool isOutOfBounds(float windowWidth, float windowHeight) {
        sf::Vector2f pos = shape.getPosition();
        return (pos.x < 0 || pos.x > windowWidth || pos.y < 0 ||
            pos.y > windowHeight);
    }

    sf::FloatRect getCollisionBounds() const {
        // Get the car's bounding box
        return shape.getGlobalBounds();
    }

    bool isColliding(const Car& other) const {
        return getCollisionBounds().intersects(other.getCollisionBounds());
    }
};

class Lane {
public:
    sf::RectangleShape shape;
    sf::Color color;
    sf::Color carColor;
    TrafficLight* trafficLight;
    std::vector<Car> cars;
    bool ignoreTrafficLight;
    bool isPriority;
    int waitingVehicles;

    Lane(float x, float y, float width, float height, sf::Color color,
        sf::Color carColor, TrafficLight* trafficLight,
        bool ignoreTrafficLight = false, bool isPriority = false)
        : ignoreTrafficLight(ignoreTrafficLight), isPriority(isPriority),
        waitingVehicles(0) {
        shape.setSize(sf::Vector2f(width, height));
        shape.setPosition(x, y);
        shape.setFillColor(color);
        this->carColor = carColor;
        this->trafficLight = trafficLight;
    }