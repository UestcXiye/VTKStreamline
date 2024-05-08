#include "VTKStreamline.h"

#include <vtkConeSource.h>
#include <vtkLineSource.h>
#include <vtkMultiBlockPLOT3DReader.h>
#include <vtkDataSet.h>
#include <vtkMultiBlockDataSet.h>
#include <vtkRungeKutta4.h>
#include <vtkStreamTracer.h>
#include <vtkTubeFilter.h>
#include <vtkDataArray.h>
#include <vtkPointData.h>
#include <vtkShrinkPolyData.h>
#include <vtkStructuredGridGeometryFilter.h>
#include <vtkStructuredGridOutlineFilter.h>
#include <vtkContourFilter.h>
#include <vtkRuledSurfaceFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>

VTKStreamline::VTKStreamline(QWidget* parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	_pVTKWidget = new QVTKOpenGLNativeWidget();
	this->setCentralWidget(_pVTKWidget);
	// this->showMaximized();

	// 1. generate data
	vtkSmartPointer<vtkLineSource> seeds = vtkSmartPointer<vtkLineSource>::New();
	// �����߶εĶ˵�
	seeds->SetPoint1(15, -5, 32);
	seeds->SetPoint2(15, 5, 32);
	seeds->SetResolution(21);

	// or, read data
	// vtkMultiBlockPLOT3DReader ��һ����ȡ���������ڶ�ȡ PLOT3D ��ʽ���ļ��������ʱ���ɽṹ������
	vtkSmartPointer<vtkMultiBlockPLOT3DReader> plot3dReader = vtkSmartPointer<vtkMultiBlockPLOT3DReader>::New();
	plot3dReader->SetXYZFileName("combxyz.bin");
	plot3dReader->SetQFileName("combq.bin");
	plot3dReader->SetScalarFunctionNumber(100);
	plot3dReader->SetVectorFunctionNumber(202);
	qDebug() << plot3dReader->GetOutput()->GetNumberOfBlocks(); // 0
	// ������¹���
	plot3dReader->Update();
	qDebug() << plot3dReader->GetOutput()->GetNumberOfBlocks(); // 1
	vtkDataSet* plot3dOutput = (vtkDataSet*)(plot3dReader->GetOutput()->GetBlock(0));
	// �Ľ�������� (Runge-Kutta) ���΢��
	vtkSmartPointer<vtkRungeKutta4> integ = vtkSmartPointer<vtkRungeKutta4>::New();
	// ͨ������ʸ������������
	vtkSmartPointer<vtkStreamTracer> streamer = vtkSmartPointer<vtkStreamTracer>::New();
	streamer->SetIntegrator(integ);
	streamer->SetInputData(plot3dOutput);
	streamer->SetStartPosition(15, 5, 32);
	streamer->SetMaximumPropagation(100);
	streamer->SetInitialIntegreationStep(0.1);
	streamer->SetIntegreationDirectionToBackward();
	vtkSmartPointer<vtkStreamTracer> streamer2 = vtkSmartPointer<vtkStreamTracer>::New();
	streamer2->SetIntegrator(integ);
	streamer2->SetInputData(plot3dOutput);
	// streamer2->SetStartPosition(15, 5, 32);
	streamer2->SetMaximumPropagation(100);
	streamer2->SetInitialIntegreationStep(0.1);
	streamer2->SetIntegreationDirectionToBackward();
	streamer2->SetSourceConnection(seeds->GetOutputPort());

	// 2. filter
	// �����ṹ��դ��߽��һ��������
	vtkSmartPointer<vtkStructuredGridOutlineFilter> outline = vtkSmartPointer<vtkStructuredGridOutlineFilter>::New();
	outline->SetInputData(plot3dOutput);
	vtkSmartPointer<vtkTubeFilter> streamTube = vtkSmartPointer<vtkTubeFilter>::New();
	streamTube->SetInputConnection(streamer->GetOutputPort());
	streamTube->SetRadius(0.06);
	streamTube->SetNumberOfSides(12);
	vtkSmartPointer<vtkRuledSurfaceFilter> scalarSurface = vtkSmartPointer<vtkRuledSurfaceFilter>::New();
	scalarSurface->SetInputConnection(streamer2->GetOutputPort());
	// �������ɷ���
	scalarSurface->SetRuledModeToPointWalk();
	scalarSurface->SetOnRatio(2);

	// 3. mapper
	vtkSmartPointer<vtkPolyDataMapper> outlineMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	vtkSmartPointer<vtkPolyDataMapper> singleMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	singleMapper->SetScalarRange(plot3dOutput->GetPointData()->GetScalars()->GetRange());
	vtkSmartPointer<vtkPolyDataMapper> multipleMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	multipleMapper->SetScalarRange(plot3dOutput->GetPointData()->GetScalars()->GetRange());

	// 4. actor
	vtkSmartPointer<vtkActor> outlineActor = vtkSmartPointer<vtkActor>::New();
	vtkSmartPointer<vtkActor> singleActor = vtkSmartPointer<vtkActor>::New();
	vtkSmartPointer<vtkActor> multipleActor = vtkSmartPointer<vtkActor>::New();

	// 5. renderer
	vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
	renderer->SetBackground(0.3, 0.6, 0.3); // Background Color: Green

	// 6. connect
	outlineMapper->SetInputConnection(outline->GetOutputPort());
	singleMapper->SetInputConnection(streamTube->GetOutputPort());
	// multipleMapper->SetInputConnection(streamer2->GetOutputPort());
	multipleMapper->SetInputConnection(scalarSurface->GetOutputPort());
	outlineActor->SetMapper(outlineMapper);
	singleActor->SetMapper(singleMapper);
	multipleActor->SetMapper(multipleMapper);
	renderer->AddActor(outlineActor);
	renderer->AddActor(singleActor);
	renderer->AddActor(multipleActor);

	this->_pVTKWidget->renderWindow()->AddRenderer(renderer);
	this->_pVTKWidget->renderWindow()->Render();
}

VTKStreamline::~VTKStreamline()
{}
