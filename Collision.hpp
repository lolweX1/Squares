#ifndef COLLISION_HPP
#define COLLISION_HPP

#include "Rectangle.hpp"
#include "Vec2.hpp"
#include <vector>

std::vector<Vec2> getRectangleCorners(const Rectangle* rect);

struct Projection {
    float min;
    float max;
};

/**
 * @brief Holds full contact data required for physics resolution.
 */
struct Contact {
    bool colliding = false;
    Vec2 normal{0.0f, 0.0f};  // Points from A to B
    float depth = 0.0f;       // Penetration depth
    Vec2 point{0.0f, 0.0f};   // Contact point in world space
};

/**
 * @brief Evaluates SAT collision and populates detailed contact info.
 */
Contact checkSATContact(const Rectangle* rectA, const Rectangle* rectB);

/**
 * @brief Resolves linear & angular impulses and positional overlap between two bodies.
 */
void resolveCollision(Rectangle* a, Rectangle* b, const Contact& contact, float restitution = 0.8f);

#endif // COLLISION_HPP