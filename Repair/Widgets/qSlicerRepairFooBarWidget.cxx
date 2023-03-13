#include "qSlicerRepairFooBarWidget.h"
#include "qSlicerRepairFooBarWidget.h"
/*==============================================================================

  Program: 3D Slicer

  Copyright (c) Kitware Inc.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  This file was originally developed by Jean-Christophe Fillion-Robin, Kitware Inc.
  and was partially funded by NIH grant 3P41RR013218-12S1

==============================================================================*/

// FooBar Widgets includes
#include "qSlicerRepairFooBarWidget.h"
#include "ui_qSlicerRepairFooBarWidget.h"

//Slicer includes
#include "qSlicerCLIModule.h"
#include "qSlicerApplication.h"
#include "qSlicerCoreIOManager.h"
#include "vtkSlicerModelsLogic.h"
#include "vtkMRMLModelNode.h"
#include "vtkMRMLScene.h"
#include "qSlicerLayoutManager.h"
#include "vtkMRMLDisplayNode.h"
#include "qMRMLThreeDWidget.h"
#include "vtkMRMLViewNode.h"
#include "qMRMLThreeDView.h"
#include "vtkMRMLLinearTransformNode.h"
#include "qSlicerModuleManager.h"
#include "qSlicerAbstractCoreModule.h"
#include "vtkSlicerCLIModuleLogic.h"
#include "vtkMRMLCommandLineModuleNode.h"
#include "qSlicerAbstractModuleRepresentation.h"
#include<vtkMatrix4x4.h>
#include <vtkMRMLMarkupsNode.h>
#include <vtkMRMLSelectionNode.h>
#include <vtkMRMLInteractionNode.h>
#include <vtkMRMLMarkupsFiducialNode.h>
#include "vtkSlicerMarkupsLogic.h"
#include "qSlicerModulePanel.h"
#include <ctkVTKObject.h>
#include "qSlicerAbstractModuleWidget.h"
#include "qMRMLSliceWidget.h"
#include "vtkMRMLSliceNode.h"
#include "vtkSlicerRepairLogic.h"
#include "vtkMRMLAnnotationPointDisplayNode.h"
#include "vtkMRMLScalarVolumeNode.h"
#include "vtkMRMLSegmentationNode.h"
#include"vtkMRMLSegmentEditorNode.h"
#include "qMRMLSegmentEditorWidget.h"
#include <vtkTransform.h>
#include <vtkTransformFilter.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkIterativeClosestPointTransform.h>
#include <vtkVertexGlyphFilter.h>
#include <vtkPoints.h>
#include <vtkLandmarkTransform.h>
#include "vtkSlicerSegmentationsModuleLogic.h"
#include <QlineEdit>
#include "myPointMatcher.h"
#include <itkeigen/Eigen/Eigen>
#include <itkeigen/Eigen/Dense>
#include <itkeigen/Eigen/Core>
#include <QFileDialog>
#include <qmainwindow.h>
#include <qSlicerModuleSelectorToolBar.h>
#include <qmenu.h>
#include <ctkMessageBox.h>
#include <qtablewidget.h>
#include <qDebug>
#include <QHeaderView.h>
#include <QStatusBar.h>
#include <QDockWidget>
#include <qpushbutton.h>
#include <QJsonObject>
#include <ctkCollapsibleButton.h>
#include"vtkMRMLLabelMapVolumeNode.h"
#include <vtkImageThreshold.h>
#include <QTimer>
#include<stdio.h>
#include "yunsuan.h"
#include "myMultiObjectMassProperties.h"
#include <vtkDataArray.h>
#include <vtkFieldData.h>
#include <ostream>
#include <vtkOrientedImageData.h>
#include <PythonQt.h>
#include <QList>

//-----------------------------------------------------------------------------
class qSlicerRepairFooBarWidgetPrivate
  : public Ui_qSlicerRepairFooBarWidget
{
  Q_DECLARE_PUBLIC(qSlicerRepairFooBarWidget);
protected:
  qSlicerRepairFooBarWidget* const q_ptr;

public:
  qSlicerRepairFooBarWidgetPrivate(
    qSlicerRepairFooBarWidget& object);
  virtual void setupUi(qSlicerRepairFooBarWidget*);
};

// --------------------------------------------------------------------------
qSlicerRepairFooBarWidgetPrivate
::qSlicerRepairFooBarWidgetPrivate(
  qSlicerRepairFooBarWidget& object)
  : q_ptr(&object)
{
}

// --------------------------------------------------------------------------
void qSlicerRepairFooBarWidgetPrivate
::setupUi(qSlicerRepairFooBarWidget* widget)
{
  this->Ui_qSlicerRepairFooBarWidget::setupUi(widget);
}

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
qSlicerRepairFooBarWidget
::qSlicerRepairFooBarWidget(QWidget* parentWidget)
  : Superclass( parentWidget )
  , d_ptr( new qSlicerRepairFooBarWidgetPrivate(*this) )
{
  Q_D(qSlicerRepairFooBarWidget);
  d->setupUi(this);


  qSlicerApplication::application()->layoutManager()->setLayout(vtkMRMLLayoutNode::SlicerLayoutDual3DView);

  QObject::connect(d->importPatientDICOMButton, &QPushButton::clicked, this, &qSlicerRepairFooBarWidget::onImportPatientDICOM);
  QObject::connect(d->importDentureDICOMButton, &QPushButton::clicked, this, &qSlicerRepairFooBarWidget::onImportDentureDICOM);

  QObject::connect(d->imageAddButton, &QPushButton::clicked,
	  [this]() {onEditFiducialPoints(QString::fromStdString("Image")); });
  QObject::connect(d->modelAddButton, &QPushButton::clicked,
	  [this]() {onEditFiducialPoints(QString::fromStdString("Model")); });
  QObject::connect(d->applyButton, &QPushButton::clicked, this, &qSlicerRepairFooBarWidget::onDentureRegistration);
  QObject::connect(d->handlesVisibilityCheckBox, &QPushButton::clicked, this, &qSlicerRepairFooBarWidget::onHandlesVisibilityClicked);
  QObject::connect(d->dentureVisibilityCheckBox, &QPushButton::clicked, this, &qSlicerRepairFooBarWidget::onDentureVisibilityClicked);
  d->handlesVisibilityCheckBox->setEnabled(false);
  d->dentureVisibilityCheckBox->setEnabled(false);

}

//-----------------------------------------------------------------------------
qSlicerRepairFooBarWidget
::~qSlicerRepairFooBarWidget()
{
}

void qSlicerRepairFooBarWidget::onImportPatientDICOM()
{
	Q_D(qSlicerRepairFooBarWidget);
	qSlicerApplication::application()->layoutManager()->selectModule("DICOM");
	this->qvtkConnect(qSlicerApplication::application()->mrmlScene(), vtkMRMLScene::NodeAddedEvent, this, SLOT(onPatientVolumeNodeAdded(vtkObject*, vtkObject*)));
}

void qSlicerRepairFooBarWidget::onPatientVolumeNodeAdded(vtkObject* scene, vtkObject* node)
{
	Q_D(qSlicerRepairFooBarWidget);
	vtkMRMLVolumeNode* volumeNode = vtkMRMLVolumeNode::SafeDownCast(node);
	if (!volumeNode)
	{
		return;
	}
	QTimer::singleShot(0, this, SLOT(setDicomProperty()));
	this->qvtkDisconnect(qSlicerApplication::application()->mrmlScene(), vtkMRMLScene::NodeAddedEvent, this, SLOT(onPatientVolumeNodeAdded(vtkObject*, vtkObject*)));
	qSlicerApplication::application()->layoutManager()->selectModule("Repair");

}
void qSlicerRepairFooBarWidget::setDicomProperty()
{
	vtkMRMLVolumeNode* volumeNode = vtkMRMLVolumeNode::SafeDownCast(qSlicerApplication::application()->mrmlScene()->GetFirstNodeByClass("vtkMRMLScalarVolumeNode"));
	volumeNode->SetName("OriginalCT");
	this->volumeRender();
}

void qSlicerRepairFooBarWidget::volumeRender()
{
	vtkMRMLMarkupsROINode* roiNode =
		vtkMRMLMarkupsROINode::SafeDownCast(qSlicerApplication::application()->mrmlScene()->AddNewNodeByClassWithID("vtkMRMLMarkupsROINode", "GlobalROI", "vtkMRMLMarkupsROINode1"));
	vtkMRMLScalarVolumeNode* volumeNode = vtkMRMLScalarVolumeNode::SafeDownCast(qSlicerApplication::application()->mrmlScene()->GetFirstNodeByName("OriginalCT"));

	double bounds[6];
	volumeNode->GetRASBounds(bounds);

	if (roiNode == nullptr)
	{
		roiNode = vtkMRMLMarkupsROINode::SafeDownCast(qSlicerApplication::application()->mrmlScene()->GetNodeByID("vtkMRMLMarkupsROINode1"));
	}
	else
	{
		roiNode->SetXYZ((bounds[1] + bounds[0]) / 2,
			(bounds[3] + bounds[2]) / 2,
			(bounds[5] + bounds[4]) / 2);
		roiNode->SetRadiusXYZ(
			(bounds[1] - bounds[0]) / 2 * 0.75,
			(bounds[3] - bounds[2]) / 2 * 0.75,
			(bounds[5] - bounds[4]) / 2);
		roiNode->SetDisplayVisibility(false);
	}

	qSlicerAbstractCoreModule* volumeRenderingModule =
		qSlicerCoreApplication::application()->moduleManager()->module("VolumeRendering");
	vtkSlicerVolumeRenderingLogic* volumeRenderingLogic =
		volumeRenderingModule ? vtkSlicerVolumeRenderingLogic::SafeDownCast(volumeRenderingModule->logic()) : 0;
	if (volumeRenderingLogic)
	{
		vtkSmartPointer<vtkMRMLVolumeRenderingDisplayNode> displayNode = volumeRenderingLogic->GetFirstVolumeRenderingDisplayNode(volumeNode);
		if (displayNode == nullptr)
		{
			displayNode = vtkSmartPointer<vtkMRMLVolumeRenderingDisplayNode>::Take(volumeRenderingLogic->CreateVolumeRenderingDisplayNode());
		}
		qSlicerApplication::application()->mrmlScene()->AddNode(displayNode);
		volumeNode->AddAndObserveDisplayNodeID(displayNode->GetID());
		volumeRenderingLogic->UpdateDisplayNodeFromVolumeNode(displayNode, volumeNode);
		displayNode->CroppingEnabledOn();
		displayNode->SetAndObserveROINodeID(roiNode->GetID());
		displayNode->SetVisibility(1);
		displayNode->GetVolumePropertyNode()->Copy(volumeRenderingLogic->GetPresetByName("CT-AAA2"));

		std::vector<std::string> viewNodeIDs{ qSlicerApplication::application()->mrmlScene()->GetFirstNodeByName("View1")->GetID()};
		displayNode->SetViewNodeIDs(viewNodeIDs);
	}
	qSlicerApplication::application()->layoutManager()->threeDWidget("View1")->threeDView()->resetFocalPoint();
	
}


void qSlicerRepairFooBarWidget::onImportDenture()
{
	QString filePath = QFileDialog::getOpenFileName(this, tr("Open a stl File"), 0, "*.stl");
	if (filePath.isEmpty())
		return;

	if (qSlicerApplication::application()->mrmlScene()->GetFirstNodeByName("Denture"))
		qSlicerApplication::application()->mrmlScene()->RemoveNode(qSlicerApplication::application()->mrmlScene()->GetFirstNodeByName("Denture"));
	vtkSlicerModelsLogic* modelsLogic = vtkSlicerModelsLogic::SafeDownCast(qSlicerApplication::application()->moduleLogic("Models"));
	vtkMRMLModelNode* dentureModelNode = modelsLogic->AddModel(filePath.toStdString().c_str());
	dentureModelNode->SetName("Denture");
	dentureModelNode->GetDisplayNode()->SetDisplayableOnlyInView(
		qSlicerApplication::application()->layoutManager()->threeDWidget("View2")->mrmlViewNode()->GetID());

	vtkSmartPointer<vtkMatrix4x4> centerMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
	centerMatrix->SetElement(0, 3, -dentureModelNode->GetPolyData()->GetCenter()[0]);
	centerMatrix->SetElement(1, 3, -dentureModelNode->GetPolyData()->GetCenter()[1]);
	centerMatrix->SetElement(2, 3, -dentureModelNode->GetPolyData()->GetCenter()[2]);
	dentureModelNode->ApplyTransformMatrix(centerMatrix);
	qSlicerApplication::application()->layoutManager()->threeDWidget("View2")->threeDView()->resetCamera();
}
void qSlicerRepairFooBarWidget::onImportDentureDICOM()
{
	Q_D(qSlicerRepairFooBarWidget);
	qSlicerApplication::application()->layoutManager()->selectModule("DICOM");
	this->qvtkConnect(qSlicerApplication::application()->mrmlScene(), vtkMRMLScene::NodeAddedEvent, this, SLOT(onDentureVolumeNodeAdded(vtkObject*, vtkObject *)));
}

void qSlicerRepairFooBarWidget::onDentureVolumeNodeAdded(vtkObject* scene, vtkObject* node)
{
	Q_D(qSlicerRepairFooBarWidget);

	vtkMRMLVolumeNode* volumeNode = vtkMRMLVolumeNode::SafeDownCast(node);
	if (!volumeNode)
	{
		return;
	}
	QTimer::singleShot(0, this, SLOT(AutoDentureRegistration()));
	this->qvtkDisconnect(qSlicerApplication::application()->mrmlScene(), vtkMRMLScene::NodeAddedEvent, this, SLOT(onDentureVolumeNodeAdded(vtkObject*, vtkObject*)));
	qSlicerApplication::application()->layoutManager()->selectModule("Repair");

}

void qSlicerRepairFooBarWidget::AutoDentureRegistration()
{
	Q_D(qSlicerRepairFooBarWidget);

		int NumofVolume = qSlicerApplication::application()->mrmlScene()->GetNumberOfNodesByClass("vtkMRMLScalarVolumeNode");
		vtkMRMLScalarVolumeNode* DentureVolumeNode = vtkMRMLScalarVolumeNode::SafeDownCast(qSlicerApplication::application()->mrmlScene()->GetNthNodeByClass(NumofVolume - 1, "vtkMRMLScalarVolumeNode"));
		DentureVolumeNode->SetName("DentureCT");
		vtkMRMLScalarVolumeNode* PatientVolumeNode = vtkMRMLScalarVolumeNode::SafeDownCast(qSlicerApplication::application()->mrmlScene()->GetFirstNodeByName("OriginalCT"));

		if (!PatientVolumeNode)
		{
			int ret = ctkMessageBox::warning(this, tr("Dental"),
				tr("Please import Dicom images of a new patient first. \n"),
				ctkMessageBox::Ok);

			return;
		}

		QList<vtkVector3d> DentureOriginalList = ExtractFiducialCentroidFromCT("Denture");
		CreateDentureModel();

		QList<vtkVector3d> PatientOriginalList = ExtractFiducialCentroidFromCT("Patient");

		QList<vtkVector3d> PatientFiducialList;
		QList<vtkVector3d> DentureFiducialList;

		yunsuan yun;
		yun.Point_Registration(PatientOriginalList, DentureOriginalList, PatientFiducialList, DentureFiducialList);


		onEditFiducialPoints("Model", DentureFiducialList);
		onEditFiducialPoints("Image", PatientFiducialList);

		onDentureRegistration();

}

void qSlicerRepairFooBarWidget::CreateDentureModel()
{

	vtkMRMLScalarVolumeNode* PatientVolumeNode = vtkMRMLScalarVolumeNode::SafeDownCast(qSlicerApplication::application()->mrmlScene()->GetFirstNodeByName("DentureCT"));
	PythonQt::init();
	PythonQtObjectPtr openBrowserContext = PythonQt::self()->getMainModule();
	QString strQuery;
	strQuery = "\
from slicer.ScriptedLoadableModule import *                                                                                         \n\
from slicer.util import VTKObservationMixin                                                                                         \n\
import math                                                                                                                         \n\
import numpy as np                                                                                                                 \n\
import logging                                                                                                                      \n\
import os		                                                                                                                    \n\
import unittest                                                                                                                     \n\
ScalarVolumeNode = slicer.mrmlScene.GetFirstNodeByName('DentureCT') \n\
segmentationNode = slicer.mrmlScene.AddNewNodeByClass('vtkMRMLSegmentationNode')         \n\
segmentationNode.SetName('DentureModelSegmentationNode') \n\
segmentationNode.CreateDefaultDisplayNodes() # only needed for display         \n\
segmentationNode.SetReferenceImageGeometryParameterFromVolumeNode(ScalarVolumeNode)         \n\
addedSegmentID = segmentationNode.GetSegmentation().AddEmptySegment('DentureWhole')         \n\
segmentEditorWidget = slicer.qMRMLSegmentEditorWidget()         \n\
segmentEditorWidget.setMRMLScene(slicer.mrmlScene)         \n\
segmentEditorNode = slicer.mrmlScene.AddNewNodeByClass('vtkMRMLSegmentEditorNode')         \n\
segmentEditorWidget.setMRMLSegmentEditorNode(segmentEditorNode)         \n\
segmentEditorWidget.setSegmentationNode(segmentationNode)         \n\
segmentEditorWidget.setMasterVolumeNode(ScalarVolumeNode)         \n\
segmentEditorWidget.setActiveEffectByName('Threshold')         \n\
effect = segmentEditorWidget.activeEffect()         \n\
effect.setParameter('MinimumThreshold', '-600')         \n\
effect.setParameter('MaximumThreshold', '5000')         \n\
effect.self().onApply()         \n\
#segmentEditorWidget.setActiveEffectByName('Islands')  \n\
#effect = segmentEditorWidget.activeEffect()  \n\
#effect.setParameter('Operation','KEEP_LARGEST_ISLAND')  \n\
#effect.self().onApply()  \n\
segmentEditorWidget = None         \n\
slicer.mrmlScene.RemoveNode(segmentEditorNode)         \n\
segmentationNode.CreateClosedSurfaceRepresentation()";
	;
	openBrowserContext.evalScript(strQuery);

	vtkMRMLSegmentationNode* segmentationNode = vtkMRMLSegmentationNode::SafeDownCast(qSlicerApplication::application()->mrmlScene()->GetFirstNodeByName("DentureModelSegmentationNode"));
	segmentationNode->GetDisplayNode()->VisibilityOff();
	vtkSmartPointer<vtkPolyData> outputPolyData = vtkSmartPointer<vtkPolyData>::New();
	auto segmentId = segmentationNode->GetSegmentation()->GetNthSegmentID(0);
	segmentationNode->GetClosedSurfaceRepresentation(segmentId, outputPolyData);
	segmentationNode->RemoveClosedSurfaceRepresentation();

	if (qSlicerApplication::application()->mrmlScene()->GetFirstNodeByName("Denture"))
		qSlicerApplication::application()->mrmlScene()->RemoveNode(qSlicerApplication::application()->mrmlScene()->GetFirstNodeByName("Denture"));
	
	vtkSlicerModelsLogic* modelsLogic = vtkSlicerModelsLogic::SafeDownCast(qSlicerApplication::application()->moduleLogic("Models"));
	vtkMRMLModelNode* dentureModelNode = modelsLogic->AddModel(outputPolyData);
	dentureModelNode->SetName("Denture");
	dentureModelNode->GetDisplayNode()->SetDisplayableOnlyInView(
		qSlicerApplication::application()->layoutManager()->threeDWidget("View2")->mrmlViewNode()->GetID());
}

vtkSmartPointer<vtkMatrix4x4> qSlicerRepairFooBarWidget::TransformMatrix(vtkImageData*reader, float* ori) {
	int read_dims[3];
	double read_spacings[3];
	double read_distance[3];
	double read_origin[3];

	reader->GetDimensions(read_dims);
	reader->GetSpacing(read_spacings);

	for (int i = 0; i < 3; ++i) {
		read_distance[i] = read_dims[i] * read_spacings[i];
		read_origin[i] = ori[i];
	}

	double matrix_slicer[4][4] = { 1, 0, 0, (read_origin[0] + read_distance[0]),
		0, 1, 0, read_origin[1] + read_distance[1],
		0, 0, -1, read_origin[2] + read_distance[2],
		0, 0, 0, 1 };

	auto Matrix_slicer = vtkSmartPointer<vtkMatrix4x4>::New();
	Matrix_slicer->Zero();
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			Matrix_slicer->SetElement(i, j, matrix_slicer[i][j]);

	return Matrix_slicer;
}

QList<vtkVector3d> qSlicerRepairFooBarWidget::ExtractFiducialCentroidFromCT(QString mode)
{
	Q_D(qSlicerRepairFooBarWidget);

	vtkMRMLSegmentationNode* segmentationNode;
	if (mode == "Patient")
	{	
		PythonQt::init();
		PythonQtObjectPtr openBrowserContext = PythonQt::self()->getMainModule();
		
		QString str1 = "\
from slicer.ScriptedLoadableModule import *                                                                                         \n\
from slicer.util import VTKObservationMixin                                                                                         \n\
import slicer.util \n\
import math                                                                                                                         \n\
import numpy as np                                                                                                                  \n\
import logging                                                                                                                      \n\
import os		                                                                                                                    \n\
import unittest                                                                                                                    \n\
import numpy as np \n\
ScalarVolumeNode = slicer.mrmlScene.GetFirstNodeByName('OriginalCT') \n\
segmentationNode = slicer.mrmlScene.AddNewNodeByClass('vtkMRMLSegmentationNode')         \n\
segmentationNode.SetName('PatientSegmentationNode') \n\
segmentationNode.CreateDefaultDisplayNodes() # only needed for display         \n\
segmentationNode.SetReferenceImageGeometryParameterFromVolumeNode(ScalarVolumeNode)         \n\
addedSegmentID = segmentationNode.GetSegmentation().AddEmptySegment('PatientFiducial')         \n\
segmentEditorWidget = slicer.qMRMLSegmentEditorWidget()         \n\
segmentEditorWidget.setMRMLScene(slicer.mrmlScene)         \n\
segmentEditorNode = slicer.mrmlScene.AddNewNodeByClass('vtkMRMLSegmentEditorNode')         \n\
segmentEditorWidget.setMRMLSegmentEditorNode(segmentEditorNode)         \n\
segmentEditorWidget.setSegmentationNode(segmentationNode)         \n\
segmentEditorWidget.setMasterVolumeNode(ScalarVolumeNode)         \n\
segmentEditorWidget.setActiveEffectByName('Threshold')         \n\
effect = segmentEditorWidget.activeEffect()         \n\
effect.setParameter('MinimumThreshold', '";
		
			QString str3 = "')         \n\
effect.setParameter('MaximumThreshold', '5000')         \n\
effect.self().onApply()         \n\
segmentEditorWidget = None         \n\
slicer.mrmlScene.RemoveNode(segmentEditorNode)         \n\
segmentationNode.CreateClosedSurfaceRepresentation()";
		QString str2= QString("%1").arg(d->PatientSlider->value());

		QString strQuery = str1 + str2 + str3;
		openBrowserContext.evalScript(strQuery);
		segmentationNode = vtkMRMLSegmentationNode::SafeDownCast(qSlicerApplication::application()->mrmlScene()->GetFirstNodeByName("PatientSegmentationNode"));
	}

	else if (mode == "Denture")
	{	
		PythonQt::init();
		PythonQtObjectPtr openBrowserContext = PythonQt::self()->getMainModule();
		QString str1 = "\
from slicer.ScriptedLoadableModule import *                                                                                         \n\
from slicer.util import VTKObservationMixin                                                                                         \n\
import math                                                                                                                         \n\
import numpy as np                                                                                                                 \n\
import logging                                                                                                                      \n\
import os		                                                                                                                    \n\
import unittest                                                                                                                     \n\
ScalarVolumeNode = slicer.mrmlScene.GetFirstNodeByName('DentureCT') \n\
segmentationNode = slicer.mrmlScene.AddNewNodeByClass('vtkMRMLSegmentationNode')         \n\
segmentationNode.SetName('DentureSegmentationNode') \n\
segmentationNode.CreateDefaultDisplayNodes() # only needed for display         \n\
segmentationNode.SetReferenceImageGeometryParameterFromVolumeNode(ScalarVolumeNode)         \n\
addedSegmentID = segmentationNode.GetSegmentation().AddEmptySegment('DentureFiducial')         \n\
segmentEditorWidget = slicer.qMRMLSegmentEditorWidget()         \n\
segmentEditorWidget.setMRMLScene(slicer.mrmlScene)         \n\
segmentEditorNode = slicer.mrmlScene.AddNewNodeByClass('vtkMRMLSegmentEditorNode')         \n\
segmentEditorWidget.setMRMLSegmentEditorNode(segmentEditorNode)         \n\
segmentEditorWidget.setSegmentationNode(segmentationNode)         \n\
segmentEditorWidget.setMasterVolumeNode(ScalarVolumeNode)         \n\
segmentEditorWidget.setActiveEffectByName('Threshold')         \n\
effect = segmentEditorWidget.activeEffect()         \n\
effect.setParameter('MinimumThreshold', '";
		QString str3 = "')         \n\
effect.setParameter('MaximumThreshold', '5000')         \n\
effect.self().onApply()         \n\
segmentEditorWidget = None         \n\
slicer.mrmlScene.RemoveNode(segmentEditorNode)         \n\
segmentationNode.CreateClosedSurfaceRepresentation()";
		QString str2 = QString("%1").arg(d->DentureSlider->value());
		QString strQuery = str1 + str2 + str3;
		openBrowserContext.evalScript(strQuery);
		segmentationNode = vtkMRMLSegmentationNode::SafeDownCast(qSlicerApplication::application()->mrmlScene()->GetFirstNodeByName("DentureSegmentationNode"));
		segmentationNode->GetDisplayNode()->VisibilityOff();
	}

	vtkSmartPointer<vtkPolyData> outputPolyData = vtkSmartPointer<vtkPolyData>::New();
	auto segmentId = segmentationNode->GetSegmentation()->GetNthSegmentID(0);
	segmentationNode->GetClosedSurfaceRepresentation(segmentId, outputPolyData);
	segmentationNode->RemoveClosedSurfaceRepresentation();

	vtkSmartPointer<myMultiObjectMassProperties> getMass = vtkSmartPointer<myMultiObjectMassProperties>::New();
	if (mode == "Denture")
	{
		getMass->SetInputData(outputPolyData);

		vtkSlicerModelsLogic* modelsLogic = vtkSlicerModelsLogic::SafeDownCast(qSlicerApplication::application()->moduleLogic("Models"));
		vtkMRMLModelNode* dentureFiducialModelNode = modelsLogic->AddModel(outputPolyData);
		dentureFiducialModelNode->SetName("DentureFiducialPoint");


		dentureFiducialModelNode->GetDisplayNode()->SetDisplayableOnlyInView(
			qSlicerApplication::application()->layoutManager()->threeDWidget("View2")->mrmlViewNode()->GetID());
	}
	else if (mode == "Patient")
	{
		getMass->SetInputData(outputPolyData);

	}


	getMass->Update();
	auto numObjects = getMass->GetNumberOfObjects();
	auto centroidArray = getMass->GetOutput()->GetFieldData()->GetArray("ObjectCentroids");
	auto volArray = getMass->GetOutput()->GetFieldData()->GetArray("ObjectVolumes");
	auto areaArray = getMass->GetOutput()->GetFieldData()->GetArray("ObjectAreas");


	QList<vtkVector3d> FiducialList;
	for (vtkIdType i = 0; i < numObjects; ++i)
	{
		double volume = volArray->GetTuple1(i);
		double area = areaArray->GetTuple1(i);
		double* centroid = centroidArray->GetTuple3(i);
		if (mode == "Denture")
		{
			vtkVector3d p0{ centroid[0] ,centroid[1] ,centroid[2] };
			FiducialList.append(p0);
		}
		else if (mode == "Patient")
		{
			if (volume < 10 && volume>1e-5 && pow(area, 1.5) / volume < 60)
			{
				vtkVector3d p0{ centroid[0] ,centroid[1] ,centroid[2] };
				FiducialList.append(p0);
			}
		}
	}

	return FiducialList;
}



void qSlicerRepairFooBarWidget::fiducialRegistration()
{
	Q_D(qSlicerRepairFooBarWidget);
	vtkMRMLLinearTransformNode* transformNode = vtkMRMLLinearTransformNode::SafeDownCast(
		qSlicerApplication::application()->mrmlScene()->AddNewNodeByClassWithID(
			"vtkMRMLLinearTransformNode", "Repair Transform", "vtkMRMLLinearTransformNodeRepair"));

	if (transformNode == nullptr)
	{
		transformNode = vtkMRMLLinearTransformNode::SafeDownCast(qSlicerApplication::application()->mrmlScene()->GetNodeByID("vtkMRMLLinearTransformNodeRepair"));
	}

	qSlicerCLIModule* regModule = static_cast<qSlicerCLIModule*> (qSlicerCoreApplication::application()->moduleManager()->module("FiducialRegistration"));//qSlicerCoreApplication
	vtkSlicerCLIModuleLogic* moduleLogic = vtkSlicerCLIModuleLogic::SafeDownCast(regModule->logic());

	vtkMRMLCommandLineModuleNode* cliNode = moduleLogic->CreateNodeInScene();
	cliNode->SetParameterAsString("saveTransform", transformNode->GetID());
	cliNode->SetParameterAsString("movingLandmarks", qSlicerApplication::application()->mrmlScene()
		->GetFirstNodeByName("ModelFiducial")->GetID());
	cliNode->SetParameterAsString("fixedLandmarks", qSlicerApplication::application()->mrmlScene()
		->GetFirstNodeByName("ImageFiducial")->GetID());
	moduleLogic->ApplyAndWait(cliNode);

	qSlicerApplication::application()->mainWindow()->statusBar()->showMessage(
		QString::fromStdString("rms = ") + QString::fromStdString(cliNode->GetParameterAsString("rms")) + "mm");
	QString bb = QString::fromStdString("rms = ") + QString::fromStdString(cliNode->GetParameterAsString("rms")) + "mm";
    d->textEdit->insertPlainText(bb );


}

void qSlicerRepairFooBarWidget::onDentureRegistration()
{
	vtkMRMLModelNode* modelNode = vtkMRMLModelNode::SafeDownCast(
		qSlicerApplication::application()->mrmlScene()->GetFirstNodeByName("Denture"));
	if (modelNode)
	{
		this->fiducialRegistration();

		vtkMRMLLinearTransformNode* registrationTransformNode = vtkMRMLLinearTransformNode::SafeDownCast(
			qSlicerApplication::application()->mrmlScene()->GetNodeByID("vtkMRMLLinearTransformNodeRepair"));

		if (qSlicerApplication::application()->mrmlScene()->GetFirstNodeByName("DentureTransformed"))
			qSlicerApplication::application()->mrmlScene()->RemoveNode(
				qSlicerApplication::application()->mrmlScene()->GetFirstNodeByName("DentureTransformed"));
		vtkSlicerModelsLogic* modelsLogic = vtkSlicerModelsLogic::SafeDownCast(
			qSlicerApplication::application()->moduleLogic("Models"));
		vtkMRMLModelNode* dentureTransformedModelNode = modelsLogic->AddModel(modelNode->GetPolyData());
		dentureTransformedModelNode->SetName("DentureTransformed");
		std::vector<std::string> viewNodeIDs{
			qSlicerApplication::application()->layoutManager()->sliceWidget("Red")->mrmlSliceNode()->GetID(),
			qSlicerApplication::application()->layoutManager()->sliceWidget("Green")->mrmlSliceNode()->GetID(),
			qSlicerApplication::application()->layoutManager()->sliceWidget("Yellow")->mrmlSliceNode()->GetID(),
			qSlicerApplication::application()->layoutManager()->threeDWidget("View1")->mrmlViewNode()->GetID() };
		dentureTransformedModelNode->GetDisplayNode()->SetViewNodeIDs(viewNodeIDs);
		dentureTransformedModelNode->GetDisplayNode()->SliceIntersectionVisibilityOn();
		
		dentureTransformedModelNode->SetAndObserveTransformNodeID(registrationTransformNode->GetID());

		double dentureBounds[6];
		dentureTransformedModelNode->GetPolyData()->GetBounds(dentureBounds);
		double dentureCenter[3] = { (dentureBounds[1] + dentureBounds[0]) / 2.0,
		(dentureBounds[2] + dentureBounds[3]) / 2.0,
		(dentureBounds[4] + dentureBounds[5]) / 2.0 };

		if (qSlicerApplication::application()->mrmlScene()->GetNodeByID("dentureInteractionFid"))
			qSlicerApplication::application()->mrmlScene()->RemoveNode(
				qSlicerApplication::application()->mrmlScene()->GetNodeByID("dentureInteractionFid"));
		vtkMRMLMarkupsFiducialNode* dentureInteractionFiducialNode = vtkMRMLMarkupsFiducialNode::SafeDownCast(
			qSlicerApplication::application()->mrmlScene()->AddNewNodeByClassWithID(
				"vtkMRMLMarkupsFiducialNode", "dentureInteractionFid", "dentureInteractionFid"));
		dentureInteractionFiducialNode->GetMarkupsDisplayNode()->SetViewNodeIDs(viewNodeIDs);

		dentureInteractionFiducialNode->GetMarkupsDisplayNode()->UseGlyphScaleOff();
		dentureInteractionFiducialNode->GetMarkupsDisplayNode()->SetGlyphSize(3);
		dentureInteractionFiducialNode->AddFiducial(0, 0, 0);

		dentureInteractionFiducialNode->SetAndObserveTransformNodeID(registrationTransformNode->GetID());
		dentureInteractionFiducialNode->HardenTransform();
		dentureInteractionFiducialNode->GetMarkupsDisplayNode()->HandlesInteractiveOn();
		dentureInteractionFiducialNode->GetInteractionHandleToWorldMatrix()->DeepCopy(
			registrationTransformNode->GetMatrixTransformToParent());
		dentureInteractionFiducialNode->Modified();

		Q_D(qSlicerRepairFooBarWidget);
		d->handlesVisibilityCheckBox->setEnabled(true);
		d->dentureVisibilityCheckBox->setEnabled(true);
		qvtkConnect(dentureInteractionFiducialNode, vtkMRMLMarkupsFiducialNode::PointModifiedEvent, 
			this, SLOT(updateDentrueTransform()));
	}
	else
	{
		ctkMessageBox::warning(this, tr("Dental"), tr("Please import denture before applying registration."), QMessageBox::Yes);
	}
}

void qSlicerRepairFooBarWidget::updateDentrueTransform()
{
	vtkMRMLMarkupsFiducialNode* dentureInteractionFiducialNode = vtkMRMLMarkupsFiducialNode::SafeDownCast(
		qSlicerApplication::application()->mrmlScene()->GetNodeByID("dentureInteractionFid"));
	vtkMRMLLinearTransformNode* transformNode = vtkMRMLLinearTransformNode::SafeDownCast(
		qSlicerApplication::application()->mrmlScene()->GetNodeByID("vtkMRMLLinearTransformNodeRepair"));
	transformNode->SetAndObserveMatrixTransformToParent(dentureInteractionFiducialNode->GetInteractionHandleToWorldMatrix());
}


void qSlicerRepairFooBarWidget::onEditFiducialPoints(QString mode)
{
	Q_D(qSlicerRepairFooBarWidget);
	vtkMRMLScene* scene = qSlicerApplication::application()->mrmlScene();

	QDockWidget* dockWidget = qSlicerApplication::application()->mainWindow()->findChild<QDockWidget*>("PanelDockWidget");
	dockWidget->show();

	if (mode=="Image")
	{
		qSlicerApplication::application()->mainWindow()->statusBar()->showMessage(
			"Left click: add image fiducials, Right click: finish");

		vtkMRMLMarkupsFiducialNode* imageFiducialNode = vtkMRMLMarkupsFiducialNode::SafeDownCast(
			scene->AddNewNodeByClassWithID("vtkMRMLMarkupsFiducialNode", "ImageFiducial", "vtkMRMLMarkupsFiducialNodeImage"));
		if (imageFiducialNode == nullptr)
			imageFiducialNode = vtkMRMLMarkupsFiducialNode::SafeDownCast(scene->GetNodeByID("vtkMRMLMarkupsFiducialNodeImage"));
		else
		{
			std::vector<std::string> viewNodeIDs{
				qSlicerApplication::application()->layoutManager()->sliceWidget("Axial")->mrmlSliceNode()->GetID(),
				qSlicerApplication::application()->layoutManager()->sliceWidget("Coronal")->mrmlSliceNode()->GetID(),
				qSlicerApplication::application()->layoutManager()->sliceWidget("Sagittal")->mrmlSliceNode()->GetID(),
				qSlicerApplication::application()->layoutManager()->threeDWidget("View1")->mrmlViewNode()->GetID() };
			imageFiducialNode->GetMarkupsDisplayNode()->SetViewNodeIDs(viewNodeIDs);
			imageFiducialNode->GetMarkupsDisplayNode()->SetSelectedColor(1, 0, 0);
			imageFiducialNode->GetMarkupsDisplayNode()->SetUseGlyphScale(false);
			imageFiducialNode->GetMarkupsDisplayNode()->SetGlyphSize(1);
			imageFiducialNode->SetMarkupLabelFormat("I-%d");
			imageFiducialNode->GetMarkupsDisplayNode()->SetTextScale(4);
			imageFiducialNode->GetMarkupsDisplayNode()->SetGlyphType(6);
			imageFiducialNode->GetMarkupsDisplayNode()->PointLabelsVisibilityOn();
			MRMLNodeModifyBlocker blocker(imageFiducialNode);
			vtkSmartPointer<vtkTextProperty> textProperty = imageFiducialNode->GetMarkupsDisplayNode()->GetTextProperty(); 
			textProperty->SetFontFamily(2);
			textProperty->SetBold(false);
			textProperty->SetItalic(false);
			textProperty->SetShadow(false);
			textProperty->SetBackgroundOpacity(0);
			d->imageFiducialTableWidget->setActiveMarkupNodeID("vtkMRMLMarkupsFiducialNodeImage");
			qvtkConnect(scene->GetNodeByID("vtkMRMLMarkupsFiducialNodeImage"), vtkMRMLMarkupsNode::PointPositionDefinedEvent,
				this, SLOT(onIsFiducialsEqualBetweenModelAndImage()));
			qvtkConnect(scene->GetNodeByID("vtkMRMLMarkupsFiducialNodeImage"), vtkMRMLMarkupsNode::PointRemovedEvent,
				this, SLOT(onIsFiducialsEqualBetweenModelAndImage()));
		}
		vtkMRMLSelectionNode* selectionNode = vtkMRMLSelectionNode::SafeDownCast(scene->GetNodeByID("vtkMRMLSelectionNodeSingleton"));
		selectionNode->SetReferenceActivePlaceNodeClassName("vtkMRMLMarkupsFiducialNode");
		selectionNode->SetReferenceActivePlaceNodeID("vtkMRMLMarkupsFiducialNodeImage");
		vtkMRMLInteractionNode* interactionNode = vtkMRMLInteractionNode::SafeDownCast(
			scene->GetNodeByID("vtkMRMLInteractionNodeSingleton"));
		int placeModePersistence = 1;
		interactionNode->SetPlaceModePersistence(placeModePersistence);
		interactionNode->SetCurrentInteractionMode(1);
		interactionNode->SwitchToPersistentPlaceMode();
	}
	else if (mode=="Model")
	{
		qSlicerApplication::application()->mainWindow()->statusBar()->showMessage(
			"Left click: add model fiducials, Right click: finish");
		vtkMRMLMarkupsFiducialNode* modelFiducialNode = vtkMRMLMarkupsFiducialNode::SafeDownCast(
			scene->AddNewNodeByClassWithID("vtkMRMLMarkupsFiducialNode", "ModelFiducial", "vtkMRMLMarkupsFiducialNodeModel"));
		if (modelFiducialNode == nullptr)
			modelFiducialNode = vtkMRMLMarkupsFiducialNode::SafeDownCast(scene->GetNodeByID("vtkMRMLMarkupsFiducialNodeModel"));
		else
		{
			modelFiducialNode->GetMarkupsDisplayNode()->SetSelectedColor(0, 0, 1);
			modelFiducialNode->GetMarkupsDisplayNode()->SetDisplayableOnlyInView(
				qSlicerApplication::application()->layoutManager()->threeDWidget("View2")->mrmlViewNode()->GetID());
			modelFiducialNode->GetMarkupsDisplayNode()->SetUseGlyphScale(false);
			modelFiducialNode->GetMarkupsDisplayNode()->SetGlyphSize(1);
			modelFiducialNode->SetMarkupLabelFormat("M-%d");
			modelFiducialNode->GetMarkupsDisplayNode()->SetTextScale(4);
			modelFiducialNode->GetMarkupsDisplayNode()->SetGlyphType(6);
			modelFiducialNode->GetMarkupsDisplayNode()->PointLabelsVisibilityOn();
			MRMLNodeModifyBlocker blocker(modelFiducialNode);
			vtkSmartPointer<vtkTextProperty> textProperty = modelFiducialNode->GetMarkupsDisplayNode()->GetTextProperty(); 
			textProperty->SetFontFamily(2);
			textProperty->SetBold(false);
			textProperty->SetItalic(false);
			textProperty->SetShadow(false);
			textProperty->SetBackgroundOpacity(0);
			d->modelFiducialTableWidget->setActiveMarkupNodeID("vtkMRMLMarkupsFiducialNodeModel");
			qvtkConnect(scene->GetNodeByID("vtkMRMLMarkupsFiducialNodeModel"), vtkMRMLMarkupsNode::PointPositionDefinedEvent,
				this, SLOT(onIsFiducialsEqualBetweenModelAndImage()));
			qvtkConnect(scene->GetNodeByID("vtkMRMLMarkupsFiducialNodeModel"), vtkMRMLMarkupsNode::PointRemovedEvent,
				this, SLOT(onIsFiducialsEqualBetweenModelAndImage()));
		}

		vtkMRMLSelectionNode* selectionNode = vtkMRMLSelectionNode::SafeDownCast(scene->GetNodeByID("vtkMRMLSelectionNodeSingleton"));
		selectionNode->SetReferenceActivePlaceNodeClassName("vtkMRMLMarkupsFiducialNode");
		selectionNode->SetReferenceActivePlaceNodeID("vtkMRMLMarkupsFiducialNodeModel");
	
		vtkMRMLInteractionNode* interactionNode = vtkMRMLInteractionNode::SafeDownCast(scene->GetNodeByID("vtkMRMLInteractionNodeSingleton"));
		int placeModePersistence = 1;
		interactionNode->SetPlaceModePersistence(placeModePersistence);
		interactionNode->SetCurrentInteractionMode(1);
		interactionNode->SwitchToPersistentPlaceMode();
	}


}
void qSlicerRepairFooBarWidget::onEditFiducialPoints(QString mode, QList<vtkVector3d> FiducialList)
{
	Q_D(qSlicerRepairFooBarWidget);
	vtkMRMLScene* scene = qSlicerApplication::application()->mrmlScene();
	int p_size = FiducialList.size();


	if (mode == "Image")
	{
		vtkMRMLMarkupsFiducialNode* imageFiducialNode = vtkMRMLMarkupsFiducialNode::SafeDownCast(
			scene->AddNewNodeByClassWithID("vtkMRMLMarkupsFiducialNode", "ImageFiducial", "vtkMRMLMarkupsFiducialNodeImage"));
		if (imageFiducialNode == nullptr)
			imageFiducialNode = vtkMRMLMarkupsFiducialNode::SafeDownCast(scene->GetNodeByID("vtkMRMLMarkupsFiducialNodeImage"));
		else
		{
			std::vector<std::string> viewNodeIDs{
				qSlicerApplication::application()->layoutManager()->threeDWidget("View1")->mrmlViewNode()->GetID() };
			imageFiducialNode->GetMarkupsDisplayNode()->SetViewNodeIDs(viewNodeIDs);
			imageFiducialNode->GetMarkupsDisplayNode()->SetSelectedColor(1, 0, 0);
			imageFiducialNode->GetMarkupsDisplayNode()->SetUseGlyphScale(false);
			imageFiducialNode->GetMarkupsDisplayNode()->SetGlyphSize(1);
			imageFiducialNode->SetMarkupLabelFormat("I-%d");
			imageFiducialNode->GetMarkupsDisplayNode()->SetTextScale(4);
			imageFiducialNode->GetMarkupsDisplayNode()->SetGlyphType(6);
			imageFiducialNode->GetMarkupsDisplayNode()->PointLabelsVisibilityOn();
			for (int i = 0; i < p_size; i++)
			{
				vtkVector3d p0 = FiducialList.at(i);
				imageFiducialNode->AddControlPointWorld(p0);
			}
			MRMLNodeModifyBlocker blocker(imageFiducialNode);
			vtkSmartPointer<vtkTextProperty> textProperty = imageFiducialNode->GetMarkupsDisplayNode()->GetTextProperty();
			textProperty->SetFontFamily(2);
			textProperty->SetBold(false);
			textProperty->SetItalic(false);
			textProperty->SetShadow(false);
			textProperty->SetBackgroundOpacity(0);
			d->imageFiducialTableWidget->setActiveMarkupNodeID("vtkMRMLMarkupsFiducialNodeImage");
			qvtkConnect(scene->GetNodeByID("vtkMRMLMarkupsFiducialNodeImage"), vtkMRMLMarkupsNode::PointPositionDefinedEvent,
				this, SLOT(onIsFiducialsEqualBetweenModelAndImage()));
			qvtkConnect(scene->GetNodeByID("vtkMRMLMarkupsFiducialNodeImage"), vtkMRMLMarkupsNode::PointRemovedEvent,
				this, SLOT(onIsFiducialsEqualBetweenModelAndImage()));
		}
	}
	else if (mode == "Model")
	{
		vtkMRMLMarkupsFiducialNode* modelFiducialNode = vtkMRMLMarkupsFiducialNode::SafeDownCast(
			scene->AddNewNodeByClassWithID("vtkMRMLMarkupsFiducialNode", "ModelFiducial", "vtkMRMLMarkupsFiducialNodeModel"));
		if (modelFiducialNode == nullptr)
			modelFiducialNode = vtkMRMLMarkupsFiducialNode::SafeDownCast(scene->GetNodeByID("vtkMRMLMarkupsFiducialNodeModel"));
		else
		{
			modelFiducialNode->GetMarkupsDisplayNode()->SetSelectedColor(0, 0, 1);
			modelFiducialNode->GetMarkupsDisplayNode()->SetDisplayableOnlyInView(
				qSlicerApplication::application()->layoutManager()->threeDWidget("View2")->mrmlViewNode()->GetID());
			modelFiducialNode->GetMarkupsDisplayNode()->SetUseGlyphScale(false);
			modelFiducialNode->GetMarkupsDisplayNode()->SetGlyphSize(1);
			modelFiducialNode->SetMarkupLabelFormat("M-%d");
			modelFiducialNode->GetMarkupsDisplayNode()->SetTextScale(4);
			modelFiducialNode->GetMarkupsDisplayNode()->SetGlyphType(6);
			modelFiducialNode->GetMarkupsDisplayNode()->PointLabelsVisibilityOn();

			for (int i = 0; i < p_size; i++)
			{
				vtkVector3d p0 = FiducialList.at(i);
				modelFiducialNode->AddControlPointWorld(p0);

			}

			MRMLNodeModifyBlocker blocker(modelFiducialNode);
			vtkSmartPointer<vtkTextProperty> textProperty = modelFiducialNode->GetMarkupsDisplayNode()->GetTextProperty();
			textProperty->SetFontFamily(2);
			textProperty->SetBold(false);
			textProperty->SetItalic(false);
			textProperty->SetShadow(false);
			textProperty->SetBackgroundOpacity(0);
			d->modelFiducialTableWidget->setActiveMarkupNodeID("vtkMRMLMarkupsFiducialNodeModel");
			qvtkConnect(scene->GetNodeByID("vtkMRMLMarkupsFiducialNodeModel"), vtkMRMLMarkupsNode::PointPositionDefinedEvent,
				this, SLOT(onIsFiducialsEqualBetweenModelAndImage()));
			qvtkConnect(scene->GetNodeByID("vtkMRMLMarkupsFiducialNodeModel"), vtkMRMLMarkupsNode::PointRemovedEvent,
				this, SLOT(onIsFiducialsEqualBetweenModelAndImage()));
		}


	}


}

void qSlicerRepairFooBarWidget::onIsFiducialsEqualBetweenModelAndImage()
{
	Q_D(qSlicerRepairFooBarWidget);
	vtkMRMLMarkupsFiducialNode* imageFiducialNode = vtkMRMLMarkupsFiducialNode::SafeDownCast(
		qSlicerApplication::application()->mrmlScene()->GetNodeByID("vtkMRMLMarkupsFiducialNodeImage"));
	vtkMRMLMarkupsFiducialNode* modelFiducialNode = vtkMRMLMarkupsFiducialNode::SafeDownCast(
		qSlicerApplication::application()->mrmlScene()->GetNodeByID("vtkMRMLMarkupsFiducialNodeModel"));
	if (imageFiducialNode&& modelFiducialNode&&imageFiducialNode->GetNumberOfFiducials() 
		== modelFiducialNode->GetNumberOfFiducials()&& imageFiducialNode->GetNumberOfFiducials()>=3)
		d->applyButton->setEnabled(true);
	else
		d->applyButton->setEnabled(false);
}

void qSlicerRepairFooBarWidget::onHandlesVisibilityClicked(bool visibility)
{
	vtkMRMLMarkupsFiducialNode* dentureInteractionFiducialNode = vtkMRMLMarkupsFiducialNode::SafeDownCast(
		qSlicerApplication::application()->mrmlScene()->GetNodeByID("dentureInteractionFid"));
	dentureInteractionFiducialNode->GetMarkupsDisplayNode()->SetScaleHandleVisibility(visibility);
	dentureInteractionFiducialNode->GetMarkupsDisplayNode()->SetRotationHandleVisibility(visibility);
	dentureInteractionFiducialNode->GetMarkupsDisplayNode()->SetTranslationHandleVisibility(visibility);
}

void qSlicerRepairFooBarWidget::onDentureVisibilityClicked(bool visibility)
{
	Q_D(qSlicerRepairFooBarWidget);
	vtkMRMLModelNode* dentureModelNode = vtkMRMLModelNode::SafeDownCast(
		qSlicerApplication::application()->mrmlScene()->GetFirstNodeByName("DentureTransformed"));
	dentureModelNode->GetDisplayNode()->SetVisibility(visibility);
	if (visibility == false)
	{
		d->handlesVisibilityCheckBox->setChecked(false);
		onHandlesVisibilityClicked(false);
	}
	
}


FiducialTableWidget::FiducialTableWidget(QWidget* parent):
	QTableWidget(parent)
{
	QObject::connect(this, SIGNAL(cellChanged(int, int)),
		this, SLOT(onActiveMarkupTableCellChanged(int, int)));

	QObject::connect(this, SIGNAL(itemClicked(QTableWidgetItem*)),
		this, SLOT(onActiveMarkupTableCellClicked(QTableWidgetItem*)));
	QObject::connect(this, SIGNAL(currentCellChanged(int, int, int, int)),
		this, SLOT(onActiveMarkupTableCurrentCellChanged(int, int, int, int)));
	this->setContextMenuPolicy(Qt::CustomContextMenu);
	QObject::connect(this, SIGNAL(customContextMenuRequested(QPoint)),
		this, SLOT(onRightClickActiveMarkupTableWidget(QPoint)));

	this->qvtkConnect(qSlicerApplication::application()->mrmlScene(), vtkMRMLScene::EndImportEvent,
		this, SLOT(onUpdateFiducialTable()));
	this->qvtkConnect(qSlicerApplication::application()->mrmlScene(), vtkMRMLScene::EndBatchProcessEvent,
		this, SLOT(onUpdateFiducialTable()));
	this->qvtkConnect(qSlicerApplication::application()->mrmlScene(), vtkMRMLScene::EndCloseEvent,
		this, SLOT(onUpdateFiducialTable()));
	this->qvtkConnect(qSlicerApplication::application()->mrmlScene(), vtkMRMLScene::EndRestoreEvent,
		this, SLOT(onUpdateFiducialTable()));
	this->setMinimumSize(QSize(0, 250));
	this->setAlternatingRowColors(true);
	this->setSelectionBehavior(QAbstractItemView::SelectRows);
	this->setSelectionMode(QAbstractItemView::ExtendedSelection);

	columnLabels << "Visible" << "Name" << "R" << "A" << "S";

	this->setColumnCount(columnLabels.size());
	this->setHorizontalHeaderLabels(columnLabels);
	this->horizontalHeader()->setFixedHeight(32);

	this->setColumnWidth(columnLabels.indexOf("Name"), 60);
	this->setColumnWidth(columnLabels.indexOf("R"), 65);
	this->setColumnWidth(columnLabels.indexOf("A"), 65);
	this->setColumnWidth(columnLabels.indexOf("S"), 65);

	QTableWidgetItem* visibleHeader = this->horizontalHeaderItem(columnLabels.indexOf("Visible"));
	visibleHeader->setText("");
	visibleHeader->setIcon(QIcon(":/Icons/Small/SlicerVisibleInvisible.png"));
	visibleHeader->setToolTip(QString("Click in this column to show/hide markups in 2D and 3D"));
	this->setColumnWidth(columnLabels.indexOf("Visible"), 30);

	this->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	this->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
}

FiducialTableWidget::~FiducialTableWidget()
{
}

void FiducialTableWidget::setActiveMarkupNodeID(std::string newID)
{
	if (activeMarkupsNodeID != newID)
	{
		vtkMRMLMarkupsFiducialNode* activeMarkupsNode = vtkMRMLMarkupsFiducialNode::SafeDownCast(
			qSlicerApplication::application()->mrmlScene()->GetNodeByID(activeMarkupsNodeID.c_str()));
		vtkMRMLMarkupsFiducialNode* newActiveMarkupsNode = vtkMRMLMarkupsFiducialNode::SafeDownCast(
			qSlicerApplication::application()->mrmlScene()->GetNodeByID(newID));
		qvtkReconnect(activeMarkupsNode, newActiveMarkupsNode, vtkCommand::ModifiedEvent,
			this, SLOT(onUpdateFiducialTable()));
		qvtkReconnect(activeMarkupsNode, newActiveMarkupsNode, vtkMRMLMarkupsNode::PointModifiedEvent,
			this, SLOT(onActiveMarkupsNodePointModifiedEvent(vtkObject*, void*)));
		qvtkReconnect(activeMarkupsNode, newActiveMarkupsNode, vtkMRMLMarkupsNode::PointAddedEvent,
			this, SLOT(onActiveMarkupsNodePointAddedEvent()));
		qvtkReconnect(activeMarkupsNode, newActiveMarkupsNode, vtkMRMLMarkupsNode::PointRemovedEvent,
			this, SLOT(onActiveMarkupsNodePointRemovedEvent(vtkObject*, void*)));
	}
	activeMarkupsNodeID = newID;
	this->onUpdateFiducialTable();
}

//-----------------------------------------------------------------------------
void FiducialTableWidget::onActiveMarkupTableCurrentCellChanged(
	int currentRow, int currentColumn, int previousRow, int previousColumn)
{
	Q_UNUSED(currentColumn);
	Q_UNUSED(previousRow);
	Q_UNUSED(previousColumn);
}
//-----------------------------------------------------------------------------
void FiducialTableWidget::onRightClickActiveMarkupTableWidget(QPoint pos)
{
	Q_UNUSED(pos);
	QMenu menu;

	QAction* deleteFiducialAction =
		new QAction(QString("Delete fiducials"), &menu);
	menu.addAction(deleteFiducialAction);
	QObject::connect(deleteFiducialAction, SIGNAL(triggered()),
		this, SLOT(onDeleteMarkupPushButtonClicked()));

	QAction* jumpSlicesAction =
		new QAction(QString("Jump slices"), &menu);
	menu.addAction(jumpSlicesAction);
	QObject::connect(jumpSlicesAction, SIGNAL(triggered()),
		this, SLOT(onJumpSlicesActionTriggered()));

	menu.exec(QCursor::pos());
}

void FiducialTableWidget::onDeleteMarkupPushButtonClicked(bool confirm /*=true*/)
{

	vtkMRMLMarkupsFiducialNode* MarkupsNode = vtkMRMLMarkupsFiducialNode::SafeDownCast(
		qSlicerApplication::application()->mrmlScene()->GetNodeByID(activeMarkupsNodeID.c_str()));
	if (!MarkupsNode)
	{
		return;
	}

	QList<QTableWidgetItem*> selectedItems =this->selectedItems();

	if (selectedItems.isEmpty())
	{
		return;
	}

	QList<int> rows;
	for (int i = 0; i < selectedItems.size(); i += columnLabels.size())
	{
		int row = selectedItems.at(i)->row();
		rows << row;
	}
	std::sort(rows.begin(), rows.end());

	if (confirm)
	{
		ctkMessageBox deleteAllMsgBox;
		deleteAllMsgBox.setWindowTitle("Delete Markups in this list?");
		QString labelText = QString("Delete ")
			+ QString::number(rows.size())
			+ QString(" Markups from this list?");
		deleteAllMsgBox.setText(labelText);

		QPushButton* deleteButton =
			deleteAllMsgBox.addButton(tr("Delete"), QMessageBox::AcceptRole);
		deleteAllMsgBox.addButton(QMessageBox::Cancel);
		deleteAllMsgBox.setDefaultButton(deleteButton);
		deleteAllMsgBox.setIcon(QMessageBox::Question);
		deleteAllMsgBox.setDontShowAgainVisible(true);
		deleteAllMsgBox.setDontShowAgainSettingsKey("Markups/AlwaysDeleteMarkups");
		deleteAllMsgBox.exec();
		if (deleteAllMsgBox.clickedButton() != deleteButton)
		{
			return;
		}
	}

	for (int i = rows.size() - 1; i >= 0; --i)
	{
		int index = rows.at(i);
		MarkupsNode->RemoveNthControlPoint(index);
	}

	this->clearSelection();
}

void FiducialTableWidget::onJumpSlicesActionTriggered()
{
	vtkMRMLMarkupsFiducialNode* MarkupsNode = vtkMRMLMarkupsFiducialNode::SafeDownCast(
		qSlicerApplication::application()->mrmlScene()->GetNodeByID(activeMarkupsNodeID.c_str()));
	if (!MarkupsNode)
	{
		return;
	}

	QList<QTableWidgetItem*> selectedItems = this->selectedItems();

	if (selectedItems.isEmpty())
	{
		return;
	}

	bool jumpCentered = false;
	jumpCentered = true;
	vtkSlicerMarkupsLogic* markupsLogic = vtkSlicerMarkupsLogic::SafeDownCast(
		qSlicerApplication::application()->moduleLogic("Markups"));

	if (markupsLogic)
	{
		markupsLogic->JumpSlicesToNthPointInMarkup(MarkupsNode->GetID(), selectedItems.at(0)->row(), jumpCentered);
	}
}

//-----------------------------------------------------------------------------
void FiducialTableWidget::updateRow(int m)
{
	this->blockSignals(true);

	vtkMRMLMarkupsFiducialNode* markupsNode = vtkMRMLMarkupsFiducialNode::SafeDownCast(
		qSlicerApplication::application()->mrmlScene()->GetNodeByID(activeMarkupsNodeID.c_str()));
	if (!markupsNode)
	{
		return;
	}

	QTableWidgetItem* visibleItem = new QTableWidgetItem();
	visibleItem->setData(Qt::CheckStateRole, QVariant());
	visibleItem->setFlags(visibleItem->flags() & ~Qt::ItemIsUserCheckable);
	visibleItem->setFlags(visibleItem->flags() & ~Qt::ItemIsEditable);
	if (markupsNode->GetNthControlPointVisibility(m))
	{
		visibleItem->setData(Qt::UserRole, QVariant(true));
		visibleItem->setData(Qt::DecorationRole, QPixmap(":/Icons/Small/SlicerVisible.png"));
	}
	else
	{
		visibleItem->setData(Qt::UserRole, QVariant(false));
		visibleItem->setData(Qt::DecorationRole, QPixmap(":/Icons/Small/SlicerInvisible.png"));
	}
	int visibleIndex = columnLabels.indexOf("Visible");
	if (this->item(m, visibleIndex) == nullptr ||
		this->item(m, visibleIndex)->data(Qt::UserRole) != visibleItem->data(Qt::UserRole))
	{
		this->setItem(m, visibleIndex, visibleItem);
	}

	int nameIndex = columnLabels.indexOf("Name");
	QString markupLabel = QString(markupsNode->GetNthControlPointLabel(m).c_str());
	if (this->item(m, nameIndex) == nullptr ||
		this->item(m, nameIndex)->text() != markupLabel)
	{
		this->setItem(m, nameIndex, new QTableWidgetItem(markupLabel));
	}

	double point[3] = { 0.0, 0.0, 0.0 };
	if (true)
	{
		double worldPoint[4] = { 0.0, 0.0, 0.0, 1.0 };
		markupsNode->GetNthControlPointPositionWorld(m, worldPoint);
		for (int p = 0; p < 3; ++p)
		{
			point[p] = worldPoint[p];
		}
	}
	else
	{
		markupsNode->GetNthControlPointPosition(m, point);
	}
	int rColumnIndex = columnLabels.indexOf("R");
	for (int p = 0; p < 3; p++)
	{
		QString coordinate = QString::number(point[p], 'f', 3);
		if (this->item(m, rColumnIndex + p) == nullptr ||
			this->item(m, rColumnIndex + p)->text() != coordinate)
		{
			this->setItem(m, rColumnIndex + p, new QTableWidgetItem(coordinate));
		}
	}

	this->blockSignals(false);
}




void FiducialTableWidget::onUpdateFiducialTable()
{
	vtkMRMLMarkupsFiducialNode* markupsNode = vtkMRMLMarkupsFiducialNode::SafeDownCast(
		qSlicerApplication::application()->mrmlScene()->GetNodeByID(activeMarkupsNodeID.c_str()));
	if (!markupsNode)
	{
		return;
	}
	int numberOfPoints = markupsNode->GetNumberOfControlPoints();
	if (this->rowCount() != numberOfPoints)
	{
		this->setRowCount(numberOfPoints);
	}
	for (int m = 0; m < numberOfPoints; m++)
	{
		this->updateRow(m);
	}
}

//-----------------------------------------------------------------------------
void FiducialTableWidget::onActiveMarkupsNodePointModifiedEvent(vtkObject* caller, void* callData)
{
	if (caller == nullptr)
	{
		return;
	}

	int* nPtr = reinterpret_cast<int*>(callData);
	int n = (nPtr ? *nPtr : -1);
	if (n >= 0)
	{
		this->updateRow(n);
	}
	else
	{
		this->onUpdateFiducialTable();
	}
}

//-----------------------------------------------------------------------------
void FiducialTableWidget::onActiveMarkupsNodePointAddedEvent()
{

	int newRow = this->rowCount();
	this->insertRow(newRow);

	this->updateRow(newRow);

	this->setCurrentCell(newRow, 0);

	this->onUpdateFiducialTable();
}

//-----------------------------------------------------------------------------
void FiducialTableWidget::onActiveMarkupsNodePointRemovedEvent(vtkObject* caller, void* callData)
{

	if (caller == nullptr)
	{
		return;
	}

	int* nPtr = reinterpret_cast<int*>(callData);
	int n = (nPtr ? *nPtr : -1);
	if (n >= 0)
	{
		this->removeRow(n);
	}
	else
	{
		this->onUpdateFiducialTable();
	}
}

//-----------------------------------------------------------------------------
void FiducialTableWidget::onActiveMarkupTableCellClicked(QTableWidgetItem* item)
{
	if (item == nullptr)
	{
		return;
	}
	int column = item->column();
	if (column == columnLabels.indexOf(QString("Visible")))
	{
		if (item->data(Qt::UserRole) == QVariant(false))
		{
			item->setData(Qt::UserRole, QVariant(true));
		}
		else
		{
			item->setData(Qt::UserRole, QVariant(false));
		}
	}
}

//-----------------------------------------------------------------------------
void FiducialTableWidget::onActiveMarkupTableCellChanged(int row, int column)
{
	vtkMRMLMarkupsFiducialNode* markupsNode = vtkMRMLMarkupsFiducialNode::SafeDownCast(
		qSlicerApplication::application()->mrmlScene()->GetNodeByID(activeMarkupsNodeID.c_str()));
	if (!markupsNode)
	{
		return;
	}
	if (!markupsNode)
	{
		return;
	}

	int n = row;

	QTableWidgetItem* item = this->item(row, column);
	if (!item)
	{
		qDebug() << QString("Unable to find item in table at ") + QString::number(row) + QString(", ") + QString::number(column);
		return;
	}
	if (column == columnLabels.indexOf("Visible"))
	{
		bool flag = item->data(Qt::UserRole) == QVariant(true) ? true : false;
		if (flag)
		{
			item->setData(Qt::DecorationRole, QPixmap(":/Icons/Small/SlicerVisible.png"));
		}
		else
		{
			item->setData(Qt::DecorationRole, QPixmap(":/Icons/Small/SlicerInvisible.png"));
		}
		markupsNode->SetNthControlPointVisibility(n, flag);
	}
	else if (column == columnLabels.indexOf("Name"))
	{
		std::string name = std::string(item->text().toUtf8());
		markupsNode->SetNthControlPointLabel(n, name);
	}
	else if (column == columnLabels.indexOf("R") ||
		column == columnLabels.indexOf("A") ||
		column == columnLabels.indexOf("S"))
	{
		double newPoint[3] = { 0.0, 0.0, 0.0 };
		if (this->item(row, columnLabels.indexOf("R")) == nullptr ||
			this->item(row, columnLabels.indexOf("A")) == nullptr ||
			this->item(row, columnLabels.indexOf("S")) == nullptr)
		{
			return;
		}
		newPoint[0] = this->item(row, columnLabels.indexOf("R"))->text().toDouble();
		newPoint[1] = this->item(row, columnLabels.indexOf("A"))->text().toDouble();
		newPoint[2] = this->item(row, columnLabels.indexOf("S"))->text().toDouble();

		double point[3] = { 0.0, 0.0, 0.0 };
		if (true)
		{
			double worldPoint[4] = { 0.0, 0.0, 0.0, 1.0 };
			markupsNode->GetNthControlPointPositionWorld(n, worldPoint);
			for (int p = 0; p < 3; ++p)
			{
				point[p] = worldPoint[p];
			}
		}
		else
		{
			markupsNode->GetNthControlPointPosition(n, point);
		}

		double minChange = 0.001;
		if (fabs(newPoint[0] - point[0]) > minChange ||
			fabs(newPoint[1] - point[1]) > minChange ||
			fabs(newPoint[2] - point[2]) > minChange)
		{
			if (true)
			{
				markupsNode->SetNthControlPointPositionWorld(n, newPoint[0], newPoint[1], newPoint[2]);
			}
			else
			{
				markupsNode->SetNthControlPointPositionFromArray(n, newPoint);
			}
		}
		else
		{
		}
	}
	else
	{
		qDebug() << QString("Cell Changed: unknown column: ") + QString::number(column);
	}
}

