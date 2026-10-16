#ifndef RECTANGLE_HPP
#define RECTANGLE_HPP

#include "Vec2.hpp"
#include <QGraphicsRectItem>

/**
 * @brief Represents a physical 2D oriented rectangle body.
 */
struct Rectangle {
    Vec2 position;          // Center of mass (world space)
    float width = 50.0f;    // Full width
    float height = 50.0f;   // Full height
    float mass = 1.0f;      // Mass in kg
    float angle = 0.0f;     // Rotation angle in degrees

    Vec2 linearVelocity{0.0f, 0.0f}; // Linear velocity vector (px/s)
    float angularVelocity = 0.0f;    // Angular velocity (deg/s)

    QGraphicsRectItem* graphicsItem = nullptr;

    /**
     * @brief Updates position and rotation on both the struct and the Qt render item.
     */
    void setTransform(float newX, float newY, float newAngle) {
        position = {newX, newY};
        angle = newAngle;
        if (graphicsItem) {
            graphicsItem->setPos(position.x, position.y);
            graphicsItem->setRotation(angle);
        }
    }
};

#endif // RECTANGLE_HPP