#pragma once
#include "gui/PlotWidget.hpp"
#include "numerics/CubicSpline.hpp"

class SplineView: public PlotWidget
{
public:
    SplineView(const QString& x_label, const QString& y_label);
    void setData(Series input);
};
