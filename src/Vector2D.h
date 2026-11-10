#pragma once
#include <iostream>
#include <cmath>

class Vector2D {
public:
    // Constructors
    Vector2D();
    Vector2D(float x, float y);

    // Getters and Setters
    float getX() const;
    void setX(float x);
    float getY() const;
    void setY(float y);

    // Vector operations
    float magnitude() const;
    Vector2D normalized() const;
    Vector2D operator+(const Vector2D& other) const;
    Vector2D operator-(const Vector2D& other) const;
    Vector2D operator*(float scalar) const;
    Vector2D operator/(float scalar) const;
    bool operator==(const Vector2D& other) const;

    // L12: ordering (x first, then y). std::priority_queue<std::pair<int, Vector2D>, ..., std::greater<>>
    // compares the whole pair, so when two tiles cost the same it falls back to comparing the tiles.
    bool operator<(const Vector2D& other) const;
    bool operator>(const Vector2D& other) const;

    // Output stream operator
    friend std::ostream& operator<<(std::ostream& os, const Vector2D& vec);

private:
    float x;
    float y;
};
