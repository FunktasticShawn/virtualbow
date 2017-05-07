#include "SplineView.hpp"
#include "model/InputData.hpp"
#include "numerics/CubicSpline.hpp"

SplineView::SplineView(const QString& x_label, const QString& y_label, DocItem<Series>& doc_item)
    : PlotWidget({750, 250})    // Todo: Magic number
{
    this->xAxis->setLabel(x_label);
    this->yAxis->setLabel(y_label);

    // Line
    this->addGraph();
    this->graph()->setPen({Qt::blue, 2});

    // Control points
    this->addGraph();
    this->graph()->setScatterStyle({QCPScatterStyle::ssSquare, Qt::red, 6});
    this->graph()->setLineStyle(QCPGraph::lsNone);

    // Selected points
    this->addGraph();
    this->graph()->setScatterStyle({QCPScatterStyle::ssDisc, Qt::red, 8});
    this->graph()->setLineStyle(QCPGraph::lsNone);


    connection = doc_item.connect([this](const Series& points)
    {
        try
        {
            input = points;
            output = CubicSpline::sample(input, 150);    // Todo: Magic number
        }
        catch(std::runtime_error& e)
        {
            output = Series();
        }

        updatePlot();
    });
}

void SplineView::setSelection(const std::vector<int>& indices)
{
    selection = indices;
    updatePlot();
}

void SplineView::updatePlot()
{
    std::vector<double> x_selected;
    std::vector<double> y_selected;
    for(int i: selection)
    {
        x_selected.push_back(input.arg(i));
        y_selected.push_back(input.val(i));
    }

    this->graph(0)->setData(output.args(), output.vals());
    this->graph(1)->setData(input.args(), input.vals());
    this->graph(2)->setData(x_selected, y_selected);

    this->rescaleAxes(false, true);
    this->replot();
}
