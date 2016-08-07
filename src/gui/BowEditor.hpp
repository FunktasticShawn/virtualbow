#pragma once
#include "NumberView.hpp"
#include "SeriesEditor.hpp"
#include "Document.hpp"

#include "../numerics/StepFunction.hpp"
#include "../model/BowParameters.hpp"

class BowEditor: public QWidget
{
    Q_OBJECT

public:
    BowEditor()
    {
        /*
        auto bt_width = new QPushButton("Edit");
        auto bt_height = new QPushButton("Edit");

        auto bt_curvature = new QPushButton("Edit");
        connect(bt_curvature, &QPushButton::clicked, [&]()
        {
            auto edit = new SeriesEditor(this, param.curvature, [](const DataSeries& input)
            {
                try
                {
                    StepFunction f(input);
                    return f.sample();
                }
                catch(std::runtime_error e)
                {
                    return DataSeries();
                }
            });

            edit->setInputLabels("Width", "Curvature");
            edit->setOutputLabels("Arc length", "Curvature");
            edit->exec();
        });
        */

        auto tf_angle = new NumberView<double, Domain::All>(document, [](BowParameters& p)->double&{ return p.angle; });
        auto tf_offset = new NumberView<double, Domain::NonNeg>(document, [](BowParameters& p)->double&{ return p.offset; });

        auto form = new QFormLayout();
        //form->addRow("Width", bt_width);
        //form->addRow("Height", bt_height);
        //form->addRow("Curvature", bt_curvature);
        form->addRow("Angle:", tf_angle);
        form->addRow("Offset:", tf_offset);
        this->setLayout(form);

        auto box = new QGroupBox("Limb Geometry");
        box->setLayout(form);

        auto h = new QHBoxLayout();
        h->addWidget(box);
        this->setLayout(h);
    }

private:
    Document document;
};
