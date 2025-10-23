#pragma once

#include "Core/Objects/Hitbox/HitboxShape.h"
#include "Util/GMath.h"

struct SweepResult {
    bool hit = false;
    double toi = 1.0;          // time of impact in [0,1] relative to dt
    Vector2d normal = {0, 0};  // contact normal pointing from A -> B
    bool isTrigger = false;    // whether collision involves a trigger
};

inline SweepResult SweptAABB(const Rect2d& a, const Vector2d& va,
                      const Rect2d& b, const Vector2d& vb, double dt) {
    Vector2d relVel = va - vb;

    // Compute entry/exit times for x/y
    double xInvEntry, xInvExit;
    double yInvEntry, yInvExit;

    if (relVel.x > 0.0) {
        xInvEntry = b.Left() - a.Right();
        xInvExit  = b.Right() - a.Left();
    } else {
        xInvEntry = b.Right() - a.Left();
        xInvExit  = b.Left() - a.Right();
    }

    if (relVel.y > 0.0) {
        yInvEntry = b.Top() - a.Bottom();
        yInvExit  = b.Bottom() - a.Top();
    } else {
        yInvEntry = b.Bottom() - a.Top();
        yInvExit  = b.Top() - a.Bottom();
    }

    double xEntry = (relVel.x == 0.0) ? -INFINITY : xInvEntry / relVel.x;
    double xExit  = (relVel.x == 0.0) ?  INFINITY : xInvExit  / relVel.x;
    double yEntry = (relVel.y == 0.0) ? -INFINITY : yInvEntry / relVel.y;
    double yExit  = (relVel.y == 0.0) ?  INFINITY : yInvExit  / relVel.y;

    double entryTime = std::max(xEntry, yEntry);
    double exitTime  = std::min(xExit, yExit);

    SweepResult result{ false, 1.0, {0,0} };

    if (entryTime > exitTime || (xEntry < 0.0 && yEntry < 0.0) || entryTime > 1.0)
        return result; // no collision

    result.hit = true;
    result.toi = std::max(entryTime, 0.0);

    if (xEntry > yEntry)
        result.normal = (xInvEntry < 0.0) ? Vector2d{1, 0} : Vector2d{-1, 0};
    else
        result.normal = (yInvEntry < 0.0) ? Vector2d{0, 1} : Vector2d{0, -1};

    return result;
}

inline SweepResult SweptRectCircle(
    const RectShape& rectShape, const Vector2d& rectPos, const Vector2d& rectDisp,
    const CircleShape& circleShape, const Vector2d& circlePos, const Vector2d& circleDisp,
    double dt
) {
    SweepResult result;
    Vector2d relDisp = circleDisp - rectDisp;

    Rect2d rect = rectShape.GetBounds().Translated(rectPos);
    Vector2d circleCenter = circlePos + circleShape.GetCenter();
    double r = circleShape.GetRadius();

    // Expand rectangle by radius (Minkowski sum)
    Rect2d expanded = rect.Expanded(r);

    // Treat circle as point
    result = SweptAABB(expanded, relDisp, Rect2d{circleCenter.x, circleCenter.y, 0, 0}, {0, 0}, dt);

    return result;
}
inline bool OverlapOnAxis(
    const std::vector<Vector2d>& vertsA,
    const std::vector<Vector2d>& vertsB,
    const Vector2d& axis
) {
    double minA = INFINITY, maxA = -INFINITY;
    double minB = INFINITY, maxB = -INFINITY;

    for (const auto& v : vertsA) {
        double proj = v.Dot(axis);
        minA = std::min(minA, proj);
        maxA = std::max(maxA, proj);
    }
    for (const auto& v : vertsB) {
        double proj = v.Dot(axis);
        minB = std::min(minB, proj);
        maxB = std::max(maxB, proj);
    }

    return !(maxA < minB || maxB < minA);
}

inline bool SATIntersects(
    const std::vector<Vector2d>& vertsA,
    const std::vector<Vector2d>& vertsB
) {
    auto testAxes = [&](const std::vector<Vector2d>& verts) {
        for (size_t i = 0; i < verts.size(); ++i) {
            Vector2d p1 = verts[i];
            Vector2d p2 = verts[(i + 1) % verts.size()];
            Vector2d edge = p2 - p1;
            Vector2d axis(-edge.y, edge.x);
            axis = axis.Normalized();
            if (!OverlapOnAxis(vertsA, vertsB, axis))
                return false;
        }
        return true;
    };

    return testAxes(vertsA) && testAxes(vertsB);
}

// Discrete polygon sweep fallback
inline SweepResult SweptPolygonSweep(
    const PolygonShape& polyA, const Vector2d& posA, const Vector2d& dispA,
    const PolygonShape& polyB, const Vector2d& posB, const Vector2d& dispB,
    double dt
) {
    SweepResult result;
    std::vector<Vector2d> vertsA = polyA.GetVertices();
    std::vector<Vector2d> vertsB = polyB.GetVertices();

    for (auto& v : vertsA) v += posA;
    for (auto& v : vertsB) v += posB;

    // Discrete test at t=0 and t=1
    if (SATIntersects(vertsA, vertsB)) {
        result.hit = true;
        result.toi = 0.0;
        result.normal = {0, 1}; // default normal
        return result;
    }

    for (auto& v : vertsA) v += dispA;
    for (auto& v : vertsB) v += dispB;
    if (SATIntersects(vertsA, vertsB)) {
        result.hit = true;
        result.toi = 1.0;
        result.normal = {0, 1};
    }

    return result;
}

SweepResult SweptCircleCircle(
    const CircleShape& aShape, const Vector2d& aPos, const Vector2d& aDisp,
    const CircleShape& bShape, const Vector2d& bPos, const Vector2d& bDisp,
    double dt
) {
    SweepResult out;
    // positions relative to time 0
    Vector2d cA0 = aPos + aShape.GetCenter();
    Vector2d cB0 = bPos + bShape.GetCenter();

    // relative motion (move A into B's space)
    Vector2d relDisp = aDisp - bDisp; // displacement over the sweep interval

    double r = static_cast<double>(aShape.GetRadius() + bShape.GetRadius());

    // Solve |(cA0 + t*relDisp) - cB0|^2 = r^2  for t in [0,1]
    Vector2d s = cA0 - cB0; // initial offset
    double a = relDisp.Dot(relDisp); // |v|^2
    double b = 2.0 * s.Dot(relDisp);
    double c = s.Dot(s) - r * r;

    // If already intersecting at t=0, treat as immediate hit
    if (c <= 0.0) {
        out.hit = true;
        out.toi = 0.0;
        // normal: from A to B (avoid zero-length)
        Vector2d n = s.LengthSquared() > 1e-12 ? s.Normalized() : Vector2d{1.0, 0.0};
        out.normal = n;
        return out;
    }

    // If no relative motion, no sweep hit
    if (a < 1e-12) {
        return out;
    }

    double disc = b * b - 4.0 * a * c;
    if (disc < 0.0) return out; // no real roots -> no hit

    double sqrtD = std::sqrt(disc);
    double t1 = (-b - sqrtD) / (2.0 * a);
    double t2 = (-b + sqrtD) / (2.0 * a);

    // we want the smallest t in [0,1]
    double t = 1.0;
    bool found = false;
    if (t1 >= 0.0 && t1 <= 1.0) { t = t1; found = true; }
    else if (t2 >= 0.0 && t2 <= 1.0) { t = t2; found = true; }

    if (!found) return out;

    out.hit = true;
    out.toi = t; // relative (0..1)
    // contact normal at impact: (posA(t) - posB(t)) normalized
    Vector2d pa = cA0 + relDisp * t;
    Vector2d pb = cB0;
    Vector2d n = (pa - pb);
    if (n.LengthSquared() > 1e-12) n = n.Normalized();
    else n = Vector2d{1.0, 0.0}; // fallback normal
    out.normal = n;
    return out;
}

inline SweepResult SweptShapeCollision(
    const HitboxShape& aShape, const Vector2d& aPos, const Vector2d& aDisp,
    const HitboxShape& bShape, const Vector2d& bPos, const Vector2d& bDisp,
    double dt
) {
    auto typeA = aShape.GetType();
    auto typeB = bShape.GetType();

    // Rect vs Rect
    if (typeA == HitboxShapeType::Rectangle && typeB == HitboxShapeType::Rectangle) {
        const auto& ra = static_cast<const RectShape&>(aShape);
        const auto& rb = static_cast<const RectShape&>(bShape);
        Rect2d rectA = ra.GetBounds().Translated(aPos);
        Rect2d rectB = rb.GetBounds().Translated(bPos);
        return SweptAABB(rectA, aDisp, rectB, bDisp, dt);
    }

    // Circle vs Circle
    if (typeA == HitboxShapeType::Circle && typeB == HitboxShapeType::Circle) {
        const auto& ca = static_cast<const CircleShape&>(aShape);
        const auto& cb = static_cast<const CircleShape&>(bShape);
        return SweptCircleCircle(ca, aPos, aDisp, cb, bPos, bDisp, dt);
    }

    // Rect vs Circle
    if (typeA == HitboxShapeType::Rectangle && typeB == HitboxShapeType::Circle) {
        const auto& ra = static_cast<const RectShape&>(aShape);
        const auto& cb = static_cast<const CircleShape&>(bShape);
        return SweptRectCircle(ra, aPos, aDisp, cb, bPos, bDisp, dt);
    }

    // Circle vs Rect (swap)
    if (typeA == HitboxShapeType::Circle && typeB == HitboxShapeType::Rectangle) {
        auto res = SweptRectCircle(
            static_cast<const RectShape&>(bShape), bPos, bDisp,
            static_cast<const CircleShape&>(aShape), aPos, aDisp, dt
        );
        res.normal = -res.normal; // flip direction since we swapped
        return res;
    }

    // Polygon vs Polygon (discrete fallback)
    if (typeA == HitboxShapeType::Polygon && typeB == HitboxShapeType::Polygon) {
        return SweptPolygonSweep(
            static_cast<const PolygonShape&>(aShape), aPos, aDisp,
            static_cast<const PolygonShape&>(bShape), bPos, bDisp, dt
        );
    }

    // Mixed polygon cases — discrete test fallback
    SweepResult result;
    result.hit = false;
    return result;
}

