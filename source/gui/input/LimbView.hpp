#pragma once
#include "gui/Plot.hpp"
#include "gui/input/LimbSource.hpp"
#include "model/Document.hpp"
#include "model/LimbProperties.hpp"

#include <QVTKWidget.h>
#include <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkCamera.h>
#include <vtkAxesActor.h>
#include <vtkOrientationMarkerWidget.h>

// Todo
// - Anti-aliasing
// - Edge tracing
// - Coordinate axes
// - ...

class InputData;

class LimbView: public QVTKWidget
{
public:
    LimbView(InputData& data)
        : data(data)
    {
        source = vtkSmartPointer<LimbSource>::New();

        auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
        mapper->SetInputConnection(source->GetOutputPort());
        
        auto actor = vtkSmartPointer<vtkActor>::New();
        actor->SetMapper(mapper);
        // Todo: Why is the face orientation wrong inside QVTKWidget?! (http://stackoverflow.com/questions/24131430/vtk-6-1-and-qt-5-3-3d-objects-in-qvtkwidget-with-bad-transparencies)
        actor->GetProperty()->SetFrontfaceCulling(false);
        actor->GetProperty()->SetBackfaceCulling(true);
        actor->GetProperty()->SetColor(1.0, 0.8, 0.4);

        renderer = vtkSmartPointer<vtkRenderer>::New();
        this->GetRenderWindow()->AddRenderer(renderer);
        renderer->AddActor(actor);
        renderer->SetBackground(0.2, 0.3, 0.4);
        renderer->SetUseFXAA(true);    // Todo: What if this is not supported (or can that be reasonably assumed?)

        // Integration of vtkOrientationMarkerWidget and QVTKWidget
        // http://vtk.markmail.org/message/cgkqlbz3jgmn6h3z?q=vtkOrientationMarkerWidget+qvtkwidget
        widget = vtkSmartPointer<vtkOrientationMarkerWidget>::New();
        widget->SetInteractor(this->GetInteractor());
        widget->SetDefaultRenderer(renderer);
        widget->SetOrientationMarker(vtkSmartPointer<vtkAxesActor>::New());
        widget->SetEnabled(true);
        widget->SetInteractive(false);

        auto camera = renderer->GetActiveCamera();
        camera->SetParallelProjection(true);

        // Buttons (Profile, width, 3D, fit)
        auto button0 = new QPushButton("Profile");
        // button0->setIcon(QIcon(":/icons/document-new"));
        QObject::connect(button0, &QPushButton::clicked, this, &LimbView::viewProfile);

        auto button1 = new QPushButton("Top");
        // button1->setIcon(QIcon(":/icons/document-new"));
        QObject::connect(button1, &QPushButton::clicked, this, &LimbView::viewTop);

        auto button2 = new QPushButton("3D View");
        // button2->setIcon(QIcon(":/icons/document-new"));
        QObject::connect(button2, &QPushButton::clicked, this, &LimbView::view3D);

        auto button3 = new QPushButton("Fit");
        // button3->setIcon(QIcon(":/icons/document-new"));
        QObject::connect(button3, &QPushButton::clicked, this, &LimbView::viewFit);

        // Todo: Magic numbers
        button0->setIconSize({32, 32});
        button1->setIconSize({32, 32});
        button2->setIconSize({32, 32});
        button3->setIconSize({32, 32});

        auto hbox = new QHBoxLayout();
        hbox->setAlignment(Qt::AlignBottom);
        hbox->addStretch();
        hbox->addWidget(button0);
        hbox->addSpacing(10);       // Magic number
        hbox->addWidget(button1);
        hbox->addSpacing(10);       // Magic number
        hbox->addWidget(button2);
        hbox->addSpacing(10);       // Magic number
        hbox->addWidget(button3);
        this->setLayout(hbox);

        // Event handling
        // Todo: Use std::bind or something...
        connections.push_back(data.profile_segments.connect([&](const Series&){ update(); }));
        connections.push_back(      data.profile_x0.connect([&](const double&){ update(); }));
        connections.push_back(      data.profile_y0.connect([&](const double&){ update(); }));
        connections.push_back(    data.profile_phi0.connect([&](const double&){ update(); }));
        connections.push_back(  data.sections_width.connect([&](const Series&){ update(); }));
        connections.push_back( data.sections_height.connect([&](const Series&){ update(); }));

        // Default view
        viewProfile();
    }

private:
    vtkSmartPointer<LimbSource> source;
    vtkSmartPointer<vtkRenderer> renderer;
    vtkSmartPointer<vtkOrientationMarkerWidget> widget;

    InputData& data;
    std::vector<Connection> connections;

    void update()
    {
        source->SetLimbData(LimbProperties(data, 150));    // Magic number
        this->GetInteractor()->Render();
    }

    virtual void resizeEvent(QResizeEvent* event)
    {
        const int size = 100;   // Magic number
        widget->SetViewport(0.0, 0.0, double(size)/event->size().width(), double(size)/event->size().height());

        /*
        widget->SetEnabled(true);
        widget->SetInteractive(false);

        widget->Modified();
        this->GetInteractor()->Render();
        renderer->Render();
        */

        qInfo() << double(size)/event->size().width() << ", " << double(size)/event->size().height();
    }

    virtual QSize sizeHint() const
    {
        return {900, 300};    // Magic numbers
    }

    void setCameraPosition(double rot_y, double rot_x)
    {
        auto center = source->GetOutput()->GetCenter();
        auto length = source->GetOutput()->GetLength();

        auto camera = renderer->GetActiveCamera();
        camera->SetFocalPoint(center);

        // Subsequently rotate camera around y- and x-axis
        camera->SetPosition(center[0] + length*std::cos(rot_y),
                            center[1] + length*std::sin(rot_x)*std::sin(rot_y),
                            center[2] - length*std::cos(rot_x)*std::sin(rot_y));

        camera->SetViewUp(0.0, -std::cos(rot_x), -std::sin(rot_x));

        this->GetInteractor()->Render();    // http://vtk.markmail.org/message/nyq3dwlyfrivrqac
    }

    void viewProfile()
    {
        setCameraPosition(M_PI_2, 0.0);
        viewFit();
    }

    void viewTop()
    {
        setCameraPosition(M_PI_2, -M_PI_2);
        viewFit();
    }

    void view3D()
    {
        setCameraPosition(1.2, -0.4);
        viewFit();
    }

    void viewFit()
    {
        auto length = source->GetOutput()->GetLength();
        const double factor = 1.05;    // Magic number
        QSize size = this->size();

        auto camera = renderer->GetActiveCamera();
        camera->SetParallelScale(0.5*factor*length*double(size.height())/double(size.width()));

        this->GetInteractor()->Render();    // http://vtk.markmail.org/message/nyq3dwlyfrivrqac
    }
};
