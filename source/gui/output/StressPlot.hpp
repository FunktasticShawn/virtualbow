#pragma once
#include "gui/Plot.hpp"
#include "model/OutputData.hpp"

class StressPlot: public Plot
{
public:
    StressPlot(const BowSetup& setup, const BowStates& states);
    void setStateIndex(int index);

private:
    const BowSetup& setup;
    const BowStates& states;

    void setContentRanges();
};
