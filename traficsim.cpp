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
    void addCar(const Car& car) {
        // Check if there is enough space to add the car without collision
        Car newCar = car;
        newCar.shape.setFillColor(carColor);

        // Check for collisions with existing cars
        for (const auto& existingCar : cars) {
            if (newCar.isColliding(existingCar)) {
                // Don't add car if it would collide
                return;
            }
        }

        cars.push_back(newCar);
    }

    void updateCars() {
        const float stopThreshold = 10.0f;  // Distance threshold before stopping
        const float collisionBuffer = 8.0f; // Minimum distance between cars

        for (auto it = cars.begin(); it != cars.end();) {
            bool shouldMove = true;
            sf::Vector2f carPos = it->shape.getPosition();
            bool inRectangularArea = (carPos.x >= 350 && carPos.x <= 450 &&
                carPos.y >= 250 && carPos.y <= 350);

            // Check collision with other cars in the same lane
            for (auto& otherCar : cars) {
                // Skip self comparison
                if (&otherCar == &(*it)) {
                    continue;
                }

                // Calculate future position
                sf::Vector2f futurePos = carPos;
                futurePos.x += it->speedX;
                futurePos.y += it->speedY;

                // Temporary car at future position to check collision
                Car futureCar = *it;
                futureCar.shape.setPosition(futurePos);

                // Check if future position would cause collision
                if (futureCar.isColliding(otherCar)) {
                    shouldMove = false;
                    it->stopped = true;
                    break;
                }