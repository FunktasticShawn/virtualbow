#pragma once
#include "solver/numerics/Eigen.hpp"
#include <nlohmann/json.hpp>

enum class SegmentType {
    Line,
    Arc,
    Spiral,
    Spline
};

enum class ConstraintType {
    X_END,
    Y_END,
    S_END,
    PHI_END,

    DELTA_X,
    DELTA_Y,
    DELTA_S,
    DELTA_PHI,

    R_START,
    R_END
};

struct Constraint {
    ConstraintType type;
    double value ;
};

static bool operator==(const Constraint& lhs, const Constraint& rhs) {
    return lhs.type == rhs.type && lhs.value == rhs.value;
}

struct SegmentInput {
    SegmentType type = SegmentType::Line;
    std::vector<Constraint> constraints = {};
};

static bool operator==(const SegmentInput& lhs, const SegmentInput& rhs) {
    return lhs.type == rhs.type && lhs.constraints == rhs.constraints;
}


NLOHMANN_JSON_SERIALIZE_ENUM(SegmentType, {
    { SegmentType::Line, "line" },
    { SegmentType::Arc, "arc" },
    { SegmentType::Spiral, "spiral" },
    { SegmentType::Spline, "spline" },
})
NLOHMANN_JSON_SERIALIZE_ENUM(ConstraintType, {
    { ConstraintType::X_END, "x_end" },
    { ConstraintType::Y_END, "y_end" },
    { ConstraintType::S_END, "s_end" },
    { ConstraintType::PHI_END, "phi_end" },
    { ConstraintType::DELTA_X, "delta_x" },
    { ConstraintType::DELTA_Y, "delta_y" },
    { ConstraintType::DELTA_S, "delta_s" },
    { ConstraintType::DELTA_PHI, "delta_phi" },
    { ConstraintType::R_START, "r_start" },
    { ConstraintType::R_END, "r_end" }
})

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Constraint, type, value)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(SegmentInput, type, constraints)

struct CurvePoint {
    double k;
    double phi;
    double x;
    double y;
};

class ProfileCurve
{
public:
    // Matrix withtwo solumnd, s and k.
    // s: Arc lengths of the profile curve in ascending order
    // k: Curvature of the profile curve at the specified arc lengths
    // Throws std::invalid_argument on invalid input
    ProfileCurve(const std::vector<SegmentInput>& input, double x0, double y0, double phi0);

    // Arc length at start and end of the curve
    double s_min() const;
    double s_max() const;

    // Evaluate curve at a specific arc length
    CurvePoint operator()(double arg) const;

private:
    using segment_fn= std::function<CurvePoint(double)>;

    mutable size_t index = 0;    // Interval index of last evaluation
    std::vector<double> s;       // Arc lengths of the intervals
    std::vector<segment_fn> f;   // Curve functions for each interval

    // Returns a function s -> {x, y, phi} that describes a curve with varying curvature k over arc length s.
    // s0, s1, k0, k1 : Arc length and curvature at start and end of the curve
    // x0, y0, phi0 : Starting point and angle
    segment_fn euler_spiral(double s0, double s1, double k0, double k1, double x0, double y0, double phi0);
};
