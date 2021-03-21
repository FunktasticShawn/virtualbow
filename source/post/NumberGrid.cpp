#include "NumberGrid.hpp"

NumberGrid::NumberGrid()
    : columnLayout(new QHBoxLayout()),
    currentColumn(nullptr),
    currentGrid(nullptr)
{
    columnLayout->setSpacing(20);
    columnLayout->setMargin(20);
    columnLayout->addStretch();
    this->setLayout(columnLayout);
}

void NumberGrid::addColumn() {
    // Create new column
    currentColumn = new QVBoxLayout();
    currentColumn->addStretch();

    // Forget old grid
    currentGrid = nullptr;

    // Insert before horizontal stretch
    int i = columnLayout->count() - 1;
    columnLayout->insertLayout(i, currentColumn);
}

void NumberGrid::addGroup(const QString& name) {
    if(currentColumn == nullptr) {
        addColumn();
    }

    currentGrid = new QGridLayout();
    auto group = new QGroupBox(name);
    group->setLayout(currentGrid);

    // Insert before vertical stretch
    int i = currentColumn->count() - 1;
    currentColumn->insertWidget(i, group);
}

void NumberGrid::addValue(const QString& name, double value, const UnitGroup& unit) {
    if(currentColumn == nullptr) {
        addColumn();
    }

    if(currentGrid == nullptr) {
        addGroup("Default");
    }

    auto label = new QLabel();
    auto edit = new QLineEdit();
    edit->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    edit->setReadOnly(true);

    int i = currentGrid->rowCount();
    currentGrid->addWidget(label, i, 0, Qt::AlignRight);
    currentGrid->addWidget(edit, i, 1);

    auto update = [&, name, value, label, edit] {
        label->setText(name + " " + unit.getSelectedUnit().getLabel());
        edit->setText(QLocale().toString(unit.getSelectedUnit().fromBase(value)));
    };

    QObject::connect(&unit, &UnitGroup::selectionChanged, this, update);
    update();
}
