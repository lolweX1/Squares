#include "Collision.hpp"
#include <cmath>
#include <algorithm>
#include <limits>

constexpr float DEG_TO_RAD = 3.14159265358979323846f / 180.0f;

std::vector<Vec2> getRectangleCorners(const Rectangle* rect) {
    float halfW = rect->width / 2.0f;
    float halfH = rect->height / 2.0f;

    Vec2 localCorners[4] = {
        {-halfW, -halfH},
        { halfW, -halfH},
        { halfW,  halfH},
        {-halfW,  halfH}
    };

    float rad = rect->angle * DEG_TO_RAD;
    float cosA = std::cos(rad);
    float sinA = std::sin(rad);

    std::vector<Vec2> worldCorners(4);
    for (int i = 0; i < 4; ++i) {
        worldCorners[i].x = localCorners[i].x * cosA - localCorners[i].y * sinA + rect->position.x;
        worldCorners[i].y = localCorners[i].x * sinA + localCorners[i].y * cosA + rect->position.y;
    }

    return worldCorners;
}

static Projection projectOntoAxis(const std::vector<Vec2>& corners, Vec2 axis) {
    float min = dot(corners[0], axis);
    float max = min;

    for (size_t i = 1; i < corners.size(); ++i) {
        float p = dot(corners[i], axis);
        if (p < min) min = p;
        if (p > max) max = p;
    }

    return {min, max};
}

static Vec2 getNormal(Vec2 p1, Vec2 p2) {
    Vec2 edge = p2 - p1;
    Vec2 normal = {-edge.y, edge.x};
    return normal.normalized();
}

// Calculates 2D cross product scalar: a x b
static float cross2D(Vec2 a, Vec2 b) {
    return a.x * b.y - a.y * b.x;
}

// Calculates 2D cross product of scalar and vector: s x v
static Vec2 cross2D(float s, Vec2 v) {
    return {-s * v.y, s * v.x};
}

Contact checkSATContact(const Rectangle* rectA, const Rectangle* rectB) {
    Contact contact;
    std::vector<Vec2> cornersA = getRectangleCorners(rectA);
    std::vector<Vec2> cornersB = getRectangleCorners(rectB);

    std::vector<Vec2> axes = {
        getNormal(cornersA[0], cornersA[1]),
        getNormal(cornersA[1], cornersA[2]),
        getNormal(cornersB[0], cornersB[1]),
        getNormal(cornersB[1], cornersB[2])
    };

    float minPenetration = std::numeric_limits<float>::max();
    Vec2 bestAxis{0.0f, 0.0f};

    for (Vec2 axis : axes) {
        Projection projA = projectOntoAxis(cornersA, axis);
        Projection projB = projectOntoAxis(cornersB, axis);

        if (projA.max < projB.min || projB.max < projA.min) {
            return contact; // Gap found -> No collision
        }

        // Calculate overlap depth along this axis
        float overlap = std::min(projA.max, projB.max) - std::max(projA.min, projB.min);
        if (overlap < minPenetration) {
            minPenetration = overlap;
            bestAxis = axis;
        }
    }

    contact.colliding = true;
    contact.depth = minPenetration;

    // Ensure normal points from A to B
    Vec2 dir = rectB->position - rectA->position;
    if (dot(dir, bestAxis) < 0) {
        bestAxis = bestAxis * -1.0f;
    }
    contact.normal = bestAxis;

    // Estimate contact point using support point on A deepest into B
    float maxDist = -std::numeric_limits<float>::max();
    for (const Vec2& corner : cornersA) {
        float d = dot(corner, contact.normal);
        if (d > maxDist) {
            maxDist = d;
            contact.point = corner;
        }
    }

    return contact;
}

void resolveCollision(Rectangle* a, Rectangle* b, const Contact& contact, float restitution) {
    if (!contact.colliding) return;

    // 1. Separate overlapping shapes to prevent sticking
    float invMassA = 1.0f / a->mass;
    float invMassB = 1.0f / b->mass;
    float totalInvMass = invMassA + invMassB;

    if (totalInvMass > 0.0f) {
        Vec2 correction = contact.normal * (contact.depth / totalInvMass);
        a->position = a->position - correction * invMassA;
        b->position = b->position + correction * invMassB;
        a->setTransform(a->position.x, a->position.y, a->angle);
        b->setTransform(b->position.x, b->position.y, b->angle);
    }

    // 2. Calculate Moment of Inertia for Rectangles: I = 1/12 * m * (w^2 + h^2)
    float invIA = 12.0f / (a->mass * (a->width * a->width + a->height * a->height));
    float invIB = 12.0f / (b->mass * (b->width * b->width + b->height * b->height));

    // Convert angular velocity to radians/sec for physics math
    float angVelA_rad = a->angularVelocity * DEG_TO_RAD;
    float angVelB_rad = b->angularVelocity * DEG_TO_RAD;

    // Relative position vectors from COM to contact point
    Vec2 rA = contact.point - a->position;
    Vec2 rB = contact.point - b->position;

    // Point velocities: v_p = v + w x r
    Vec2 velA = a->linearVelocity + cross2D(angVelA_rad, rA);
    Vec2 velB = b->linearVelocity + cross2D(angVelB_rad, rB);

    Vec2 relativeVel = velB - velA;
    float velAlongNormal = dot(relativeVel, contact.normal);

    // Do not resolve if velocities are separating
    if (velAlongNormal > 0) return;

    float rA_cross_N = cross2D(rA, contact.normal);
    float rB_cross_N = cross2D(rB, contact.normal);

    // 3. Compute Impulse Scalar J
    float j = -(1.0f + restitution) * velAlongNormal;
    j /= (invMassA + invMassB + (rA_cross_N * rA_cross_N * invIA) + (rB_cross_N * rB_cross_N * invIB));

    Vec2 impulse = contact.normal * j;

    // 4. Apply Linear Velocity Updates
    a->linearVelocity = a->linearVelocity - impulse * invMassA;
    b->linearVelocity = b->linearVelocity + impulse * invMassB;

    // 5. Apply Angular Velocity Updates (Convert back to deg/s)
    float deltaAngA_rad = -cross2D(rA, impulse) * invIA;
    float deltaAngB_rad =  cross2D(rB, impulse) * invIB;

    constexpr float RAD_TO_DEG = 180.0f / 3.14159265358979323846f;
    a->angularVelocity += deltaAngA_rad * RAD_TO_DEG;
    b->angularVelocity += deltaAngB_rad * RAD_TO_DEG;
}