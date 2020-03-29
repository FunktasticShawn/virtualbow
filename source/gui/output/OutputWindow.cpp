#include "OutputWindow.hpp"
#include "NumberGrid.hpp"
#include "ShapePlot.hpp"
#include "StressPlot.hpp"
#include "CurvaturePlot.hpp"
#include "EnergyPlot.hpp"
#include "ComboPlot.hpp"
#include "Slider.hpp"
#include "../Application.hpp"
#include <algorithm>

OutputWindow::OutputWindow(QWidget* parent, InputData input, OutputData output)
    : QMainWindow(parent),
      input(input),
      output(output)
{
    this->setCentralWidget(new OutputWidget(this->input, this->output));
    this->setWindowTitle("Simulation Results");
    this->setAttribute(Qt::WA_DeleteOnClose);    // Make sure destructor is called on close, not when parent window is

    // Load geometry and state
    restoreState(Application::settings.value("OutputWindow/state").toByteArray());
    restoreGeometry(Application::settings.value("OutputWindow/geometry").toByteArray());
}

OutputWindow::~OutputWindow()
{
    // Save state and geometry
    Application::settings.setValue("OutputWindow/state", saveState());
    Application::settings.setValue("OutputWindow/geometry", saveGeometry());
}

OutputWidget::OutputWidget(const InputData& input, const OutputData& output)
{
    bool enable_statics = !output.statics.states.time.empty();
    bool enable_dynamics = !output.dynamics.states.time.empty();

    auto stack = new QStackedLayout();
    if(enable_statics) {
        stack->addWidget(new StaticOutputWidget(input, output));
    }
    if(enable_dynamics) {
        stack->addWidget(new DynamicOutputWidget(input, output));
    }

    auto bt_statics = new QPushButton("Statics");
    // bt_statics->setStyleSheet("background-color: rgb(249, 217, 111);");
    bt_statics->setIcon(QIcon(":/icons/show-statics"));
    bt_statics->setCheckable(true);
    bt_statics->setEnabled(enable_statics);
    bt_statics->setAutoExclusive(true);
    QObject::connect(bt_statics, &QPushButton::toggled, [=](bool checked) {
        if(checked)
            stack->setCurrentIndex(0);
    });

    auto bt_dynamics = new QPushButton("Dynamics");
    // bt_dynamics->setStyleSheet("background-color: rgb(170, 243, 117);");
    bt_dynamics->setIcon(QIcon(":/icons/show-dynamics"));
    bt_dynamics->setCheckable(true);
    bt_dynamics->setEnabled(enable_dynamics);
    bt_dynamics->setAutoExclusive(true);
    QObject::connect(bt_dynamics, &QPushButton::toggled, [=](bool checked) {
        if(checked)
            stack->setCurrentIndex(1);
    });

    auto btbox = new QDialogButtonBox();
    btbox->addButton(bt_statics, QDialogButtonBox::ActionRole);
    btbox->addButton(bt_dynamics, QDialogButtonBox::ActionRole);
    QObject::connect(btbox, &QDialogButtonBox::rejected, this, &QDialog::close);

    auto vbox = new QVBoxLayout();
    vbox->addLayout(stack, 1);
    vbox->addWidget(btbox);
    this->setLayout(vbox);

    bt_statics->setChecked(!enable_dynamics);
    bt_dynamics->setChecked(enable_dynamics);
}

StaticOutputWidget::StaticOutputWidget(const InputData& input, const OutputData& output)
{
    auto numbers = new NumberGrid();
    numbers->addColumn();
    numbers->addGroup("Performance");
    numbers->addValue("Final draw force [N]", output.statics.final_draw_force);
    numbers->addValue("Drawing work [J]", output.statics.drawing_work);
    numbers->addValue("Storage ratio", output.statics.storage_ratio);
    numbers->addGroup("Properties");
    numbers->addValue("Limb mass [kg]", output.setup.limb_mass);
    numbers->addValue("String mass [kg]", output.setup.string_mass);
    numbers->addValue("String length [m]", output.setup.string_length);
    numbers->addColumn();
    numbers->addGroup("Maximum absolute stresses");
    for(int i = 0; i < output.statics.max_stress_index.size(); ++i) {
        numbers->addValue(QString::fromStdString(input.layers[i].name + " [Pa]"), output.statics.max_stress_value[i]);
    }
    numbers->addGroup("Maximum absolute forces");
    numbers->addValue("Draw force [N]", output.statics.states.draw_force[output.statics.max_draw_force_index]);
    numbers->addValue("Grip force [N]", output.statics.states.grip_force[output.statics.max_grip_force_index]);
    numbers->addValue("String force (total) [N]", output.statics.states.string_force[output.statics.max_string_force_index]);
    numbers->addValue("String force (strand) [N]", output.statics.states.strand_force[output.statics.max_string_force_index]);

    auto plot_shapes = new ShapePlot(output.setup.limb_properties, output.statics.states, true);
    auto plot_stress = new StressPlot(input, output.setup.limb_properties, output.statics.states);
    auto plot_curvature = new CurvaturePlot(output.setup.limb_properties, output.statics.states);
    auto plot_energy = new EnergyPlot(output.statics.states, output.statics.states.draw_length, "Draw length [m]");
    auto plot_combo = new ComboPlot();
    plot_combo->addData("Draw length [m]", output.statics.states.draw_length);
    plot_combo->addData("Draw force [N]", output.statics.states.draw_force);
    plot_combo->addData("String force (total) [N]", output.statics.states.string_force);
    plot_combo->addData("String force (strand) [N]", output.statics.states.strand_force);
    plot_combo->addData("Grip force [N]", output.statics.states.grip_force);
    plot_combo->addData("Pot. energy limbs [J]", output.statics.states.e_pot_limbs);
    plot_combo->addData("Pot. energy string [J]", output.statics.states.e_pot_string);
    plot_combo->setCombination(0, 1);

    auto tabs = new QTabWidget();
    tabs->addTab(numbers, "Characteristics");
    tabs->addTab(plot_shapes, "Shape");
    tabs->addTab(plot_stress, "Stress");
    tabs->addTab(plot_curvature, "Curvature");
    tabs->addTab(plot_energy, "Energy");
    tabs->addTab(plot_combo, "Other Plots");

    auto slider = new Slider(output.statics.states.draw_length, "Draw length [m]");
    slider->addJumpAction("Max. draw force", output.statics.max_draw_force_index);
    slider->addJumpAction("Max. grip force", output.statics.max_grip_force_index);
    slider->addJumpAction("Max. string force", output.statics.max_string_force_index);
    for (int i = 0; i < output.statics.max_stress_index.size(); ++i) {
        slider->addJumpAction(QString::fromStdString("Max. stress for " + input.layers[i].name), output.statics.max_stress_index[i].first);
    }

    QObject::connect(slider, &Slider::valueChanged, plot_shapes, &ShapePlot::setStateIndex);
    QObject::connect(slider, &Slider::valueChanged, plot_stress, &StressPlot::setStateIndex);
    QObject::connect(slider, &Slider::valueChanged, plot_curvature, &CurvaturePlot::setStateIndex);
    QObject::connect(slider, &Slider::valueChanged, plot_energy, &EnergyPlot::setStateIndex);
    emit slider->valueChanged(0);

    auto vbox = new QVBoxLayout();
    this->setLayout(vbox);
    vbox->setMargin(0);
    vbox->addWidget(tabs);
    vbox->addWidget(slider);
}

DynamicOutputWidget::DynamicOutputWidget(const InputData& input, const OutputData& output)
{
    auto numbers = new NumberGrid();
    numbers->addColumn();
    numbers->addGroup("Performance");
    numbers->addValue("Final arrow velocity [m/s]", output.dynamics.final_vel_arrow);
    numbers->addValue("Efficiency", output.dynamics.efficiency);
    numbers->addGroup("Energy at arrow departure");
    numbers->addValue("Kinetic energy arrow [J]", output.dynamics.final_e_kin_arrow);
    numbers->addValue("Kinetic energy limbs [J]", output.dynamics.final_e_kin_limbs);
    numbers->addValue("Kinetic energy string [J]", output.dynamics.final_e_kin_string);
    numbers->addColumn();
    numbers->addGroup("Maximum absolute stresses");
    for (int i = 0; i < output.dynamics.max_stress_value.size(); ++i) {
        numbers->addValue(QString::fromStdString(input.layers[i].name + " [Pa]"), output.dynamics.max_stress_value[i]);
    }
    numbers->addGroup("Maximum absolute forces");
    numbers->addValue("String force (total) [N]", output.dynamics.states.string_force[output.dynamics.max_string_force_index]);
    numbers->addValue("String force (strand) [N]", output.dynamics.states.strand_force[output.dynamics.max_string_force_index]);
    numbers->addValue("Grip force [N]", output.dynamics.states.grip_force[output.dynamics.max_grip_force_index]);

    auto plot_shapes = new ShapePlot(output.setup.limb_properties, output.dynamics.states, false);
    auto plot_stress = new StressPlot(input, output.setup.limb_properties, output.dynamics.states);
    auto plot_curvature = new CurvaturePlot(output.setup.limb_properties, output.dynamics.states);
    auto plot_energy = new EnergyPlot(output.dynamics.states, output.dynamics.states.time, "Time [s]");
    auto plot_combo = new ComboPlot();
    plot_combo->addData("Time [s]", output.dynamics.states.time);
    plot_combo->addData("Arrow position [m]", output.dynamics.states.pos_arrow);
    plot_combo->addData("Arrow velocity [m/s]", output.dynamics.states.vel_arrow);
    plot_combo->addData("Arrow acceleration [m/s²]", output.dynamics.states.acc_arrow);
    plot_combo->addData("String force (total) [N]", output.dynamics.states.string_force);
    plot_combo->addData("String force (strand) [N]", output.dynamics.states.strand_force);
    plot_combo->addData("Grip force [N]", output.dynamics.states.grip_force);
    plot_combo->addData("Pot. energy limbs [J]", output.dynamics.states.e_pot_limbs);
    plot_combo->addData("Kin. energy limbs [J]", output.dynamics.states.e_kin_limbs);
    plot_combo->addData("Pot. energy string [J]", output.dynamics.states.e_pot_string);
    plot_combo->addData("Kin. energy string [J]", output.dynamics.states.e_kin_string);
    plot_combo->addData("Kin. energy arrow [J]", output.dynamics.states.e_kin_arrow);
    plot_combo->setCombination(0, 1);

    auto tabs = new QTabWidget();
    tabs->addTab(numbers, "Characteristics");
    tabs->addTab(plot_shapes, "Shape");
    tabs->addTab(plot_stress, "Stress");
    tabs->addTab(plot_curvature, "Curvature");
    tabs->addTab(plot_energy, "Energy");
    tabs->addTab(plot_combo, "Other Plots");

    auto slider = new Slider(output.dynamics.states.time, "Time [s]");
    slider->addJumpAction("Arrow departure", output.dynamics.arrow_departure_index);
    slider->addJumpAction("Max. grip force", output.dynamics.max_grip_force_index);
    slider->addJumpAction("Max. string force", output.dynamics.max_string_force_index);
    for (int i = 0; i < output.dynamics.max_stress_index.size(); ++i) {
        slider->addJumpAction(QString::fromStdString("Max. stress for " + input.layers[i].name), output.dynamics.max_stress_index[i].first);
    }

    QObject::connect(slider, &Slider::valueChanged, plot_shapes, &ShapePlot::setStateIndex);
    QObject::connect(slider, &Slider::valueChanged, plot_stress, &StressPlot::setStateIndex);
    QObject::connect(slider, &Slider::valueChanged, plot_curvature, &CurvaturePlot::setStateIndex);
    QObject::connect(slider, &Slider::valueChanged, plot_energy, &EnergyPlot::setStateIndex);
    emit slider->valueChanged(0);

    auto vbox = new QVBoxLayout();
    this->setLayout(vbox);
    vbox->setMargin(0);
    vbox->addWidget(tabs);
    vbox->addWidget(slider);
}
