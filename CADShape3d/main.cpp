#include <QApplication>
#include <QVBoxLayout>
#include <QWidget>
#include <QMenuBar>
#include "CADShape3d.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    QWidget root;
    root.setWindowTitle("CAD Shape Editor");
    root.resize(1280, 700);

    QVBoxLayout* layout = new QVBoxLayout(&root);
    layout->setContentsMargins(0, 0, 0, 0);

    // Menu bar
    QMenuBar* menuBar = new QMenuBar;
    QMenu* viewMenu = menuBar->addMenu("View");

    QAction* view2D = viewMenu->addAction("2D View");
    QAction* view3D = viewMenu->addAction("3D View");

    // Canvas
    CADShape3d* canvas = new CADShape3d;

    layout->setMenuBar(menuBar);
    layout->addWidget(canvas);

    // Connections
    QObject::connect(view2D, &QAction::triggered, [&] {
        canvas->setViewMode(VIEW_2D);
        });

    QObject::connect(view3D, &QAction::triggered, [&] {
        canvas->setViewMode(VIEW_3D);
        });

    root.show();
    return app.exec();
}