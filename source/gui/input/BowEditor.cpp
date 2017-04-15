#include "BowEditor.hpp"
#include "ModelTree.hpp"
#include "LimbView.hpp"

BowEditor::BowEditor(InputData& data)
    : QSplitter(Qt::Horizontal)
{
    this->addWidget(new ModelTree());
    this->addWidget(new LimbView(data));
    this->setStretchFactor(0, 0);
    this->setStretchFactor(1, 1);
}
