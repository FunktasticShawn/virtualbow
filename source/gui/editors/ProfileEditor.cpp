#include "ProfileEditor.hpp"
#include "ProfileTableModel.hpp"
#include "solver/model/ProfileCurve.hpp"
#include <algorithm>
#include "gui/widgets/DoubleSpinBox.hpp"

SpinBoxDelegate::SpinBoxDelegate(QObject *parent)
    : QStyledItemDelegate(parent) {

}

QWidget* SpinBoxDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    auto editor = new DoubleSpinBox(ValueRange::UNRESTRICTED, parent);
    editor->setFrame(false);
    return editor;
}

void SpinBoxDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
    int value = index.model()->data(index, Qt::EditRole).toDouble();
    QDoubleSpinBox *spinBox = static_cast<QDoubleSpinBox*>(editor);
    spinBox->setValue(value);
}

void SpinBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {
    QDoubleSpinBox *spinBox = static_cast<QDoubleSpinBox*>(editor);
    spinBox->interpretText();
    double value = spinBox->value();
    model->setData(index, value, Qt::EditRole);
}

void SpinBoxDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    editor->setGeometry(option.rect);
}

ComboBoxDelegate::ComboBoxDelegate(QObject *parent)
    : QStyledItemDelegate(parent) {

}

QWidget* ComboBoxDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    QComboBox* editor = new QComboBox(parent);
    editor->addItems({"Radius", "Length", "Angle"});
    editor->setFrame(false);
    return editor;
}

void ComboBoxDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
    QString value = index.model()->data(index, Qt::EditRole).toString();
    QComboBox *comboBox = static_cast<QComboBox*>(editor);
    comboBox->setCurrentIndex(comboBox->findData(value));
}

void ComboBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {
    QComboBox *comboBox = static_cast<QComboBox*>(editor);
    QString value = comboBox->currentText();
    model->setData(index, value, Qt::EditRole);
}

void ComboBoxDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    editor->setGeometry(option.rect);
}

ProfileTreeItem::ProfileTreeItem(const SegmentInput& segment)
    : segment(segment),
      editor(nullptr)
{
    this->setText(0, segmentText(segment.type));
    this->setIcon(0, segmentIcon(segment.type));
    this->setFlags(flags() & ~Qt::ItemIsDropEnabled);
}

QWidget* ProfileTreeItem::getEditor() {
    if(editor == nullptr) {
        editor = segmentEditor(segment.type);
    }
    return editor;
}

QIcon ProfileTreeItem::segmentIcon(SegmentType type) const {
    switch(type) {
        case SegmentType::Line: return QIcon(":/icons/segment-line.svg");
        case SegmentType::Arc: return QIcon(":/icons/segment-arc.svg");
        case SegmentType::Spiral: return QIcon(":/icons/segment-spiral.svg");
        case SegmentType::Spline: return QIcon(":/icons/segment-spline.svg");
    }
    return QIcon();
}

QString ProfileTreeItem::segmentText(SegmentType type) const {
    switch(type) {
        case SegmentType::Line: return "Line";
        case SegmentType::Arc: return "Arc";
        case SegmentType::Spiral: return "Spiral";
        case SegmentType::Spline: return "Spline";
    }
    return QString();
}

QWidget* ProfileTreeItem::segmentEditor(SegmentType type) const {
    /*
    switch(type) {
        case SegmentType::Line: return new QLabel("Line");
        case SegmentType::Arc: return new QLabel("Arc");
        case SegmentType::Spiral: return new QLabel("Spiral");
        case SegmentType::Spline: return new QLabel("Spline");
    }
    return new QLabel("Unknown");
    */

    /*
    auto table_model = new ProfileTableModel();
    auto table_view = new QTableView();
    table_view->setModel(table_model);
    table_view->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table_view->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    table_view->verticalHeader()->setVisible(false);

    //table_view->setItemDelegateForColumn(0, new ComboBoxDelegate());
    table_view->setItemDelegateForColumn(1, new SpinBoxDelegate());
    */

    auto table_view = new QTableWidget(1, 2);
    table_view->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table_view->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    table_view->verticalHeader()->setVisible(false);

    auto box_1 = new QComboBox();
    box_1->setFrame(false);
    box_1->addItems({"Length", "Radius", "Angle"});

    auto spinner_1 = new DoubleSpinBox(ValueRange::UNRESTRICTED);
    spinner_1->setFrame(false);
    spinner_1->setSuffix(" mm");

    table_view->setCellWidget(0, 0, box_1);
    table_view->setCellWidget(0, 1, spinner_1);

    return table_view;
}

ProfileTreeHeader::ProfileTreeHeader(QWidget* parent, const QList<QToolButton*>& buttons)
    : QHeaderView(Qt::Horizontal, parent)
{
    auto hbox = new QHBoxLayout();
    hbox->setMargin(2);
    hbox->setSpacing(2);
    hbox->addStretch();
    for(auto button: buttons) {
        hbox->addWidget(button);
    }

    this->setLayout(hbox);
};

ProfileEditor::ProfileEditor(const UnitSystem& units) {
    auto button_add = new QToolButton();
    button_add->setIcon(QIcon(":/icons/list-add.svg"));
    button_add->setPopupMode(QToolButton::InstantPopup);
    button_add->setMenu(new QMenu());

    auto button_remove = new QToolButton();
    button_remove->setIcon(QIcon(":/icons/list-remove.svg"));

    auto action_remove = new QAction(this);
    action_remove->setShortcut(QKeySequence::Delete);
    this->addAction(action_remove);

    auto tree_view = new QTreeWidget();
    tree_view->setHeader(new ProfileTreeHeader(this, {button_add, button_remove}));
    tree_view->header()->setStretchLastSection(false);
    tree_view->header()->setSectionResizeMode(QHeaderView::Stretch);
    tree_view->header()->setDefaultAlignment(Qt::AlignLeft);
    tree_view->setHeaderLabel("Segments");
    tree_view->setRootIsDecorated(false);
    tree_view->setSelectionMode(QAbstractItemView::ExtendedSelection);
    tree_view->setDragDropMode(QAbstractItemView::InternalMove);
    tree_view->setDragEnabled(true);
    tree_view->setAcceptDrops(true);
    tree_view->setDropIndicatorShown(true);

    /*
    auto table_model = new ProfileTableModel(this);
    auto table_view = new QTableView();
    table_view->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table_view->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    table_view->verticalHeader()->setVisible(false);
    table_view->setModel(table_model);
    */

    auto placeholder = new QTableWidget();
    //placeholder->horizontalHeader()->setVisible(false);
    //placeholder->verticalHeader()->setVisible(false);

    auto splitter = new QSplitter(Qt::Vertical);
    splitter->setChildrenCollapsible(false);
    splitter->addWidget(tree_view);
    splitter->addWidget(placeholder);
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 0);

    auto vbox = new QVBoxLayout();
    vbox->setMargin(0);
    vbox->addWidget(splitter);

    this->setLayout(vbox);

    // Actions for adding and removing segments

    button_add->menu()->addAction(QIcon(":/icons/segment-line.svg"), "Line", this, [tree_view] {
        auto item = new ProfileTreeItem(SegmentInput{ .type=SegmentType::Line, .constraints={} });
        tree_view->addTopLevelItem(item);
    });

    button_add->menu()->addAction(QIcon(":/icons/segment-arc.svg"), "Arc", this, [tree_view] {
        auto item = new ProfileTreeItem(SegmentInput{ .type=SegmentType::Arc, .constraints={} });
        tree_view->addTopLevelItem(item);
    });

    button_add->menu()->addAction(QIcon(":/icons/segment-spiral.svg"), "Spiral", this, [tree_view] {
        auto item = new ProfileTreeItem(SegmentInput{ .type=SegmentType::Spiral, .constraints={} });
        tree_view->addTopLevelItem(item);
    });

    button_add->menu()->addAction(QIcon(":/icons/segment-spline.svg"), "Spline", this, [tree_view] {
        auto item = new ProfileTreeItem(SegmentInput{ .type=SegmentType::Spline, .constraints={} });
        tree_view->addTopLevelItem(item);
    });

    QObject::connect(button_remove, &QPushButton::clicked, this, [tree_view] {
        int count = tree_view->topLevelItemCount();
        if(count != 0) {
            QTreeWidgetItem* item = tree_view->topLevelItem(count - 1);
            delete item;    // https://stackoverflow.com/a/9399167
        }
    });

    QObject::connect(action_remove, &QAction::triggered, this, [tree_view] {
        auto selection = tree_view->selectedItems();
        for(auto item: selection) {
            delete item;    // https://stackoverflow.com/a/9399167
        }
    });

    QObject::connect(tree_view, &QTreeWidget::itemSelectionChanged, this, [tree_view, splitter, placeholder](){
        auto selection = tree_view->selectedItems();
        if(selection.isEmpty()) {
            splitter->replaceWidget(1, placeholder);
        }
        else if(selection.size() == 1) {
            auto item = dynamic_cast<ProfileTreeItem*>(selection[0]);
            splitter->replaceWidget(1, item->getEditor());
        }
    });

    /*
    QObject::connect(tree_view, &QTreeView::clicked, this, [](const QModelIndex &index) {
        qInfo() << index.row();
    });
    */

    /*
    QObject::connect(&model, &TableModel::modified, this, &ProfileEditor::modified);
    QObject::connect(this->selectionModel(), &QItemSelectionModel::selectionChanged, this, [&] {
        emit this->rowSelectionChanged(getSelectedIndices());
    });
    */
}

MatrixXd ProfileEditor::getData() const {
    return MatrixXd(); //model.getData();
}

void ProfileEditor::setData(const MatrixXd& data) {
    //model.setData(data);
}

QVector<int> ProfileEditor::getSelectedIndices() {
    //QItemSelectionRange range = selectionModel()->selection().first();
    QVector<int> selection;
    /*
    int index = 0;
    for(int i = 0; i < model.rowCount(); ++i) {
        QVariant arg = model.index(i, 0).data();
        QVariant val = model.index(i, 1).data();
        if(!arg.isNull() && !val.isNull()) {
            if(i >= range.top() && i <= range.bottom()) {
                selection.push_back(index);
            }
            ++index;
        }
    }
    */

    return selection;
}
