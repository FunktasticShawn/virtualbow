#pragma once
#include "gui/PlotWidget.hpp"
#include "model/output/OutputData.hpp"

class ShapePlot: public PlotWidget
{
public:
    ShapePlot(const SetupData& setup, const BowStates& states, bool intermediate_steps);
    void setStateIndex(int index);

private:
    const SetupData& setup;
    const BowStates& states;

    QCPCurve* limb_right;
    QCPCurve* limb_left;
    QCPCurve* string_right;
    QCPCurve* string_left;
    QCPCurve* arrow;    // Todo: Replace with other QCustomPlot object?

    void plotIntermediateStates();
    void setAxesRanges();
};
