#pragma once
#include <QtWidgets>

class OutputGrid: public QWidget
{
public:
    OutputGrid()
        : grid(new QGridLayout())
    {
        auto hbox = new QHBoxLayout();
        this->setLayout(hbox);
        hbox->addLayout(grid);
        hbox->addStretch();

        grid->setHorizontalSpacing(15);     // Todo: Unify with NumberGroup. Todo: Spacing only after line edits.
    }

    void add(int i, int j, const QString& text, const QString& unit, double value)
    {
        auto edit = new QLineEdit(QLocale::c().toString(value));
        edit->setReadOnly(true);

        grid->addWidget(new QLabel(text), i, 3*j, 1, 1, Qt::AlignRight);
        grid->addWidget(edit, i, 3*j+1);
        grid->addWidget(new QLabel(unit.isEmpty() ? "" : "[" + unit + "]"), i, 3*j+2);
        grid->setColumnStretch(3*j, 1);
    }

private:
    QGridLayout* grid;
};
