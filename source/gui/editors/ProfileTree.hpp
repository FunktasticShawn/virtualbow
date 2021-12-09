#pragma once
#include "ProfileSegmentEditor.hpp"
#include "solver/model/profile/ProfileInput.hpp"
#include <QtWidgets>

class ProfileTree;

enum class SegmentType {
    Line, Arc, Spiral, Spline
};

class ProfileTreeItem: public QTreeWidgetItem {
public:
    ProfileTreeItem(ProfileTree* parent, const SegmentInput& input);
    ProfileSegmentEditor* getEditor();
    SegmentInput getData() const;

private:
    ProfileSegmentEditor* editor;

    QIcon segmentIcon(const SegmentInput& input) const;
    QString segmentText(const SegmentInput& input) const;
    ProfileSegmentEditor* segmentEditor(const SegmentInput& input) const;
};

class ProfileTreeHeader: public QHeaderView {
public:
    ProfileTreeHeader(QWidget* parent, const QList<QToolButton*>& buttons);
};

class ProfileTree: public QTreeWidget {
    Q_OBJECT

public:
    ProfileTree();

    ProfileInput getData() const;
    void setData(const ProfileInput& data);

protected:
    void dropEvent(QDropEvent *event) override;

signals:
    void modified();
};
