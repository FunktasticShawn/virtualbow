#pragma once
#include "UserSettings.hpp"
#include <QtGui>

class Application
{
public:
    static const std::string version;
    static UserSettings settings;

    static int run(int argc, char* argv[]);

private:
    static int run_gui(QApplication& app, QString path);
    static int run_cli(QString input_path, QString output_path, bool dynamic);
};
