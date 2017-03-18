#pragma once
#include "gui/Plot.hpp"
#include "model/OutputData.hpp"

class ComboPlot: public QWidget
{
public:
    ComboPlot();

    void addData(const QString& name, const std::vector<double>& data);
    void setCombination(int index_x, int index_y);

private:
    QComboBox* combo_x;
    QComboBox* combo_y;
    Plot* plot;

    void setPlotData(int index);
};
