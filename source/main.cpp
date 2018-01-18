#include "gui/TableWidget.hpp"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    TableWidget table("x", "y", 25);
    table.show();

    Series s;
    s.push_back(0.0, 0.0);
    s.push_back(1.0, 1.0);
    s.push_back(2.0, 4.0);
    s.push_back(3.0, 9.0);
    s.push_back(4.0, 16.0);
    s.push_back(5.0, 25.0);

    table.setData(s);

    return app.exec();
}

/*
#include "gui/Application.hpp"

int main(int argc, char* argv[])
{
    return Application::run(argc, argv);
}
*/

/*
#include "gui/EditableTabBar.hpp"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    // QApplication::setStyle("windows");

    EditableTabBar tb;
    tb.addTab("Layer0");
    tb.addTab("Layer1");
    tb.addTab("Layer2");
    tb.show();

    QObject::connect(&tb, &EditableTabBar::addTabRequested, []()
    {
        qInfo() << "Add Tab!";
    });

    QObject::connect(&tb, &EditableTabBar::tabRenamed, [](int index, const QString& name)
    {
        qInfo() << index << " renamed to " << name;
    });

    return app.exec();
}
*/
