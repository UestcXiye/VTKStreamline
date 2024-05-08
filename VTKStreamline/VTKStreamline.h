#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_VTKStreamline.h"

#include <QVTKOpenGLNativeWidget.h>

class VTKStreamline : public QMainWindow
{
    Q_OBJECT

public:
    VTKStreamline(QWidget *parent = nullptr);
    ~VTKStreamline();

private:
    Ui::VTKStreamlineClass ui;

    QVTKOpenGLNativeWidget* _pVTKWidget = Q_NULLPTR;
};
