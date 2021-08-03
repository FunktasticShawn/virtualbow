#include "LayerEditor.hpp"

LayerEditor::LayerEditor(EditableTabBar* tabs, const UnitSystem& units)
    : tabs(tabs),
      table(new TableEditor({ "Position", "Height" }, { &units.ratio, &units.length })),
      view(new SplineView("Position", "Height", units.ratio, units.length)),
      edit_rho(new DoubleEditor("rho", units.density)),
      edit_E(new DoubleEditor("E", units.elastic_modulus))
{
    auto hbox = new QHBoxLayout();
    hbox->setContentsMargins(10, 5, 10, 5);
    hbox->addWidget(new QLabel("Material:"));
    hbox->addWidget(edit_rho);
    hbox->addWidget(edit_E);
    hbox->addStretch();

    auto group = new QGroupBox();
    group->setLayout(hbox);

    auto vbox = new QVBoxLayout();
    vbox->addWidget(group, 0);
    vbox->addWidget(view, 1);

    auto widget = new QWidget();
    widget->setLayout(vbox);

    this->addWidget(table);
    this->addWidget(widget);
    this->setStretchFactor(0, 0);
    this->setStretchFactor(1, 1);
    this->setChildrenCollapsible(false);

    // Event handling

    QObject::connect(this, &LayerEditor::modified, this, &LayerEditor::updateTabIcon);

    QObject::connect(edit_E, &DoubleEditor::modified, this, &LayerEditor::modified);
    QObject::connect(edit_rho, &DoubleEditor::modified, this, &LayerEditor::modified);

    QObject::connect(table, &TableEditor::rowSelectionChanged, view, &SplineView::setSelection);
    QObject::connect(table, &TableEditor::modified, [&]{
        view->setData(table->getData());
        emit modified();
    });
}

Layer LayerEditor::getData() const
{
    Layer layer;
    LayerEditor* wtf = const_cast<LayerEditor*>(this);
    layer.name = tabs->tabText(tabs->indexOf(wtf)).toStdString();
    layer.height = table->getData();
    layer.rho = edit_rho->getData();
    layer.E = edit_E->getData();

    return layer;
}

void LayerEditor::setData(const Layer& layer)
{
    tabs->setTabText(tabs->indexOf(this), QString::fromStdString(layer.name));
    #ifndef __APPLE__
    tabs->setTabIcon(tabs->indexOf(this), QIcon(getLayerPixmap(layer)));
    #endif
    table->setData(layer.height);
    view->setData(layer.height);
    edit_E->setData(layer.E);
    edit_rho->setData(layer.rho);
}

void LayerEditor::updateTabIcon()
{
    #ifndef __APPLE__
    tabs->setTabIcon(tabs->indexOf(this), QIcon(getLayerPixmap(getData())));
    #endif
}
