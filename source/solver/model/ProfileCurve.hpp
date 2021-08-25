#pragma once
#include "solver/numerics/Eigen.hpp"

enum class SegmentType {
    Line,
    Arc,
    Spiral,
    Spline
};

enum class ConstraintType {
    X_End,
    Y_End,
    S_End
};

struct Constraint {
    ConstraintType type;
    double value;
};

struct SegmentInput {
    SegmentType type;
    std::vector<Constraint> constraints;
};



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
    ProfileCurve(const MatrixXd& input, double x0, double y0, double phi0);

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
