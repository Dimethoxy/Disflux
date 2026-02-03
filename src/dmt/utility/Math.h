//==============================================================================
/*
 * ██████  ██ ███    ███ ███████ ████████ ██   ██  ██████  ██   ██ ██    ██
 * ██   ██ ██ ████  ████ ██         ██    ██   ██ ██    ██  ██ ██   ██  ██
 * ██   ██ ██ ██ ████ ██ █████      ██    ██   ██ ██    ██   ███     ████
 * ██   ██ ██ ██  ██  ██ ██         ██    ██   ██ ██    ██  ██ ██     ██
 * ██████  ██ ██      ██ ███████    ██    ██   ██  ██████  ██   ██    ██
 *
 * Copyright (C) 2024 Dimethoxy Audio (https://dimethoxy.com)
 *
 * This file is part of the Dimethoxy Library, a collection of essential
 * classes used across various Dimethoxy projects.
 * These files are primarily designed for internal use within our repositories.
 *
 * License:
 * This code is licensed under the GPLv3 license. You are permitted to use and
 * modify this code under the terms of this license.
 * You must adhere GPLv3 license for any project using this code or parts of it.
 * Your are not allowed to use this code in any closed-source project.
 *
 * Description:
 * Math utility functions for geometry, trigonometry, and coordinate
 * transformations, optimized for real-time audio and graphics applications.
 *
 * Authors:
 * Lunix-420 (Primary Author)
 */
//==============================================================================

#pragma once

#include <JuceHeader.h>

//==============================================================================

namespace dmt {
namespace math {

//==============================================================================

/**
 * @brief Mathematical constant π (pi) as a float.
 */
constexpr float pi = juce::MathConstants<float>::pi;

/**
 * @brief Mathematical constant 2π (two pi) as a float.
 */
constexpr float twoPi = juce::MathConstants<float>::twoPi;

/**
 * Calculates the angle in radians from a given slope.
 *
 * This function computes the angle in radians from a given slope using the
 * atan function. The slope is expected to be a floating-point type.
 *
 * @param slope The slope value. It is expected to be a floating-point type
 *              (e.g., `float` or `double`).
 * @return The angle in radians corresponding to the slope.
 *
 * @tparam SlopeType The desired type of the slope. Must be a floating-point
 * type (e.g., `float`, `double`).
 *
 * @note This function uses `atan` to calculate the angle in radians.
 *
 * @warning Ensure that the SlopeType is a floating-point type. This function
 *          may result in undefined behavior if passed a non-floating-point
 * type.
 *
 * @example
 * float angle = slopeToAngleInRadians<float>(slope);
 */
template<typename SlopeType>
[[nodiscard]] static inline SlopeType
slopeToAngleInRadians(SlopeType slope) noexcept
{
  // Ensure SlopeType is a floating-point type at compile-time
  static_assert(std::is_floating_point_v<SlopeType>,
                "SlopeType must be a floating-point type.");

  // Calculate and return the angle in radians using atan
  return std::atan(slope);
}

/**
 * Calculates the slope from a given angle in radians.
 *
 * This function computes the slope from a given angle in radians using the
 * tan function. The angle is expected to be a floating-point type representing
 * the angle in radians.
 *
 * @param angle The angle in radians. It is expected to be a floating-point type
 *              (e.g., `float` or `double`).
 * @return The slope corresponding to the angle in radians.
 *
 * @tparam AngleType The desired type of the angle. Must be a floating-point
 * type (e.g., `float`, `double`).
 *
 * @note This function uses `tan` to calculate the slope from the angle.
 *
 * @warning Ensure that the AngleType is a floating-point type. This function
 *          may result in undefined behavior if passed a non-floating-point
 *          type.
 *
 * @example
 * float slope = angleInRadiansToSlope<float>(angle);
 */
template<typename AngleType>
[[nodiscard]] static inline AngleType
angleInRadiansToSlope(AngleType angle) noexcept
{
  // Ensure AngleType is a floating-point type at compile-time
  static_assert(std::is_floating_point_v<AngleType>,
                "AngleType must be a floating-point type.");

  // Calculate and return the slope using tan
  return std::tan(angle);
}

/**
 * Calculates the slope between two points.
 *
 * This function calculates the slope of the line connecting two points (a and
 * b). It returns an optional value, which will be nullopt if the slope is
 * undefined (i.e., if the line is vertical).
 *
 * @param a The first point.
 * @param b The second point.
 * @return An optional value containing the slope if defined, or nullopt if
 * undefined.
 *
 * @tparam SlopeType The desired type of the slope result. Must be a floating
 * point type.
 * @tparam PointType The type used in the point coordinates.
 *
 * @note This function uses JUCE's approximatelyEqual() to safely check for
 * division by zero.
 *
 * @warning Make sure SlopeType is a floating point type.
 *
 * @example float slope = calculateSlope<float>(pointA, pointB).value_or(0.0f);
 */
template<typename SlopeType, typename PointType>
[[nodiscard]] static inline std::optional<SlopeType>
calculateSlope(const juce::Point<PointType>& a,
               const juce::Point<PointType>& b) noexcept
{
  // Ensure SlopeType is a floating point type at compile time
  static_assert(std::is_floating_point<SlopeType>::value,
                "SlopeType must be a floating point type.");

  // Convert point coordinates to the desired slope type
  const SlopeType x1 = a.x;
  const SlopeType y1 = a.y;
  const SlopeType x2 = b.x;
  const SlopeType y2 = b.y;

  // Compute differences in X and Y
  const SlopeType deltaX = x2 - x1;
  const SlopeType deltaY = y2 - y1;

  // Avoid division by zero by checking if deltaX is approximately zero
  if (juce::approximatelyEqual(deltaX, SlopeType{ 0 })) {
    jassertfalse; // Vertical line: slope is undefined
    return std::nullopt;
  }

  // Return the calculated slope
  return deltaY / deltaX;
}

/**
 * Moves a point in a given direction by a specified distance.
 *
 * @param origin   The starting point.
 * @param angleRad The direction to move in, in radians.
 * @param distance The distance to move.
 * @return A new point representing the result of moving from the origin
 *         in the specified direction by the given distance.
 *
 * @tparam CoordType The floating-point type used for point coordinates.
 *
 * @note The direction is interpreted as a standard angle in radians,
 *       where 0 is to the right (positive X axis), and positive angles
 *       rotate counter-clockwise.
 *
 * @example
 * juce::Point<float> result = marchPoint<float>(start, angle, 10.0f);
 */
template<typename CoordType>
[[nodiscard]] static inline juce::Point<CoordType>
marchPoint(const juce::Point<CoordType>& origin,
           CoordType angleRad,
           CoordType distance) noexcept
{
  static_assert(std::is_floating_point_v<CoordType>,
                "CoordType must be a floating-point type.");

  const CoordType dx = std::cos(angleRad) * distance;
  const CoordType dy = std::sin(angleRad) * distance;

  return { origin.x + dx, origin.y + dy };
}

/**
 * Projects a point along a line defined by an origin and angle in radians
 * until it reaches a specific Y-coordinate.
 *
 * @param origin    The starting point of the line.
 * @param angleRad  The direction of the line in radians.
 * @param yTarget   The Y-coordinate to project the point to.
 * @return A new point lying on the line at the specified Y-coordinate,
 *         or std::nullopt if the angle is horizontal (no intersection).
 *
 * @tparam CoordType The floating-point type used for point coordinates.
 *
 * @note Assumes a standard coordinate system where positive Y is down.
 *       If the angle represents a horizontal line and yTarget differs from
 *       origin.y, the result is undefined (returns std::nullopt).
 *
 * @example
 * auto projected = projectPointToY<float>(origin, angle,
 * targetY).value_or(origin);
 */
template<typename CoordType>
[[nodiscard]] static inline std::optional<juce::Point<CoordType>>
projectPointToY(const juce::Point<CoordType>& origin,
                CoordType angleRad,
                CoordType yTarget) noexcept
{
  static_assert(std::is_floating_point_v<CoordType>,
                "CoordType must be a floating-point type.");

  const CoordType deltaY = yTarget - origin.y;
  const CoordType tanAngle = std::tan(angleRad);

  // Avoid division by zero or near-zero slope (horizontal line)
  if (juce::approximatelyEqual(tanAngle, CoordType{ 0 })) {
    jassertfalse; // No X change possible — can't project to different Y
    return std::nullopt;
  }

  const CoordType deltaX = deltaY / tanAngle;
  const CoordType xTarget = origin.x + deltaX;

  return juce::Point<CoordType>{ xTarget, yTarget };
}

/**
 * Calculates the intersection point of two infinite lines defined by two points
 * each.
 *
 * This function finds where two infinitely extended lines intersect,
 * regardless of whether the points define overlapping segments.
 *
 * @param a1 The first point of the first line.
 * @param a2 The second point of the first line.
 * @param b1 The first point of the second line.
 * @param b2 The second point of the second line.
 * @return An optional point representing the intersection. Returns std::nullopt
 * if lines are parallel or coincident.
 *
 * @tparam CoordType The floating-point type used for point coordinates.
 *
 * @note This operates on *infinite* lines, not bounded segments.
 *
 * @example
 * auto intersection = intersectInfiniteLines<float>(a1, a2, b1, b2);
 */
template<typename CoordType>
[[nodiscard]] static inline std::optional<juce::Point<CoordType>>
intersectInfiniteLines(const juce::Point<CoordType>& a1,
                       const juce::Point<CoordType>& a2,
                       const juce::Point<CoordType>& b1,
                       const juce::Point<CoordType>& b2) noexcept
{
  static_assert(std::is_floating_point_v<CoordType>,
                "CoordType must be a floating-point type.");

  const CoordType x1 = a1.x, y1 = a1.y;
  const CoordType x2 = a2.x, y2 = a2.y;
  const CoordType x3 = b1.x, y3 = b1.y;
  const CoordType x4 = b2.x, y4 = b2.y;

  const CoordType denominator = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);

  if (juce::approximatelyEqual(denominator, CoordType{ 0 })) {
    jassertfalse; // Lines are parallel or coincident
    return std::nullopt;
  }

  const CoordType detA = x1 * y2 - y1 * x2;
  const CoordType detB = x3 * y4 - y3 * x4;

  const CoordType ix = (detA * (x3 - x4) - (x1 - x2) * detB) / denominator;
  const CoordType iy = (detA * (y3 - y4) - (y1 - y2) * detB) / denominator;

  return juce::Point<CoordType>{ ix, iy };
}

/**
 * Converts a linear value to its exponential equivalent.
 *
 * This function maps a linear value to an exponential scale. For non-negative
 * values, it returns (value + 1). For negative values, it returns the
 * reciprocal of -(value + 1). This is useful for mapping linear controls to
 * exponential responses, such as in audio parameter scaling.
 *
 * @param value The linear value to convert.
 * @return The exponential equivalent of the input value.
 *
 * @note For value >= 0, result is (value + 1). For value < 0, result is
 *       1 / -(value + 1).
 *
 * @example
 * float expValue = linearToExponent(0.5f); // returns 1.5f
 * float expValueNeg = linearToExponent(-0.5f); // returns 2.0f
 */
static inline float
linearToExponent(float value) noexcept
{
  return (value >= 0.0f) ? (value + 1.0f) : (1.0f / -(value + 1.0f));
}

/**
 * Calculates the coordinates of a point on a circle given a radius and angle.
 *
 * This function computes the (x, y) coordinates of a point on a circle centered
 * at the origin, for a given radius and angle in radians. The angle is measured
 * from the positive X axis, with 0 at the top (12 o'clock).
 *
 * @param radius           The radius of the circle.
 * @param angleInRadians   The angle in radians, where 0 is at the top.
 * @return A juce::Point<float> representing the coordinates on the circle.
 *
 * @note The angle is offset by -π/2 so that 0 radians is at the top.
 *
 * @example
 * juce::Point<float> p = pointOnCircle(1.0f,
 * juce::MathConstants<float>::halfPi);
 */
static inline const juce::Point<float>
pointOnCircle(const float radius, const float angleInRadians) noexcept
{
  return juce::Point<float>(radius * std::cos(angleInRadians - pi * 0.5f),
                            radius * std::sin(angleInRadians - pi * 0.5f));
}

/**
 * Calculates the coordinates of a point on a circle given a center, radius, and
 * angle.
 *
 * This function computes the (x, y) coordinates of a point on a circle centered
 * at a specified point, for a given radius and angle in radians. The angle is
 * measured from the positive X axis, with 0 at the top (12 o'clock).
 *
 * @param centre           The center of the circle.
 * @param radius           The radius of the circle.
 * @param angleInRadians   The angle in radians, where 0 is at the top.
 * @return A juce::Point<float> representing the coordinates on the circle.
 *
 * @note The angle is offset by -π/2 so that 0 radians is at the top.
 *
 * @example
 * juce::Point<float> p = pointOnCircle({ 10.0f, 10.0f }, 5.0f, 0.0f);
 */
static inline const juce::Point<float>
pointOnCircle(juce::Point<float> centre,
              const float radius,
              const float angleInRadians) noexcept
{
  juce::Point<float> raw(radius * std::cos(angleInRadians - pi * 0.5f),
                         radius * std::sin(angleInRadians - pi * 0.5f));
  return raw + centre;
}

/**
 * Converts an angle from degrees to radians.
 *
 * @param angleInDegree The angle in degrees.
 * @return The angle in radians.
 *
 * @example
 * float radians = degreeToRadians(180.0f); // returns π
 */
static inline const float
degreeToRadians(float angleInDegree) noexcept
{
  return angleInDegree * (juce::MathConstants<float>::pi / 180.0f);
}

/**
 * Normalizes an angle in radians to the range [0, 2π).
 *
 * If the input angle is negative, this function adds 2π to bring it into the
 * standard positive range. If the angle is already positive, it is returned
 * unchanged.
 *
 * @param angleInRadians The angle in radians.
 * @return The normalized angle in radians, in the range [0, 2π).
 *
 * @example
 * float norm = normalizeAngleInRadians(-1.0f); // returns value in [0, 2π)
 */
static inline const float
normalizeAngleInRadians(float angleInRadians) noexcept
{
  if (angleInRadians < 0.0f)
    angleInRadians += twoPi;
  return angleInRadians;
}

/**
 * Converts an angle from radians to degrees.
 *
 * @param angleInRadians The angle in radians.
 * @return The angle in degrees.
 *
 * @example
 * float degrees = radiansToDegree(juce::MathConstants<float>::pi); // returns
 * 180.0f
 */
static inline const float
radiansToDegree(float angleInRadians) noexcept
{
  return angleInRadians * (180.0f / juce::MathConstants<float>::pi);
}

//==============================================================================
} // namespace math
} // namespace dmt