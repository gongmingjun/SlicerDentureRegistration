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

#ifndef __qSlicerRepairFooBarWidget_h
#define __qSlicerRepairFooBarWidget_h

// Qt includes
#include <QWidget>
#include <QTableWidget>
// FooBar Widgets includes
#include "qSlicerRepairModuleWidgetsExport.h"

#include <ctkVTKObject.h>
//Eigen includes
#include <itkeigen/Eigen/Eigen>
#include <itkeigen/Eigen/Dense>
#include <itkeigen/Eigen/Core>
#include <vtkPolyData.h>
#include <vtkImageData.h>
#include "vtkMRMLSegmentationNode.h"
#include "vtkMRMLScalarVolumeNode.h"
#include "vtkMRMLMarkupsROINode.h"
#include "vtkMRMLVolumeRenderingDisplayNode.h"
#include "vtkSlicerVolumeRenderingLogic.h"
#include "vtkMRMLVolumePropertyNode.h"
#include "vtkMRMLLayoutNode.h"
//#include <iostream>
#include<stdio.h>
#include <vtkMatrix4x4.h>
#include <QList>
#include <vtkVector.h>
#include <math.h>
class qSlicerRepairFooBarWidgetPrivate;
class QTableWidgetItem;
class QString;
class QJsonObject;
/// \ingroup Slicer_QtModules_Repair
class Q_SLICER_MODULE_REPAIR_WIDGETS_EXPORT qSlicerRepairFooBarWidget
  : public QWidget
{
  Q_OBJECT
  QVTK_OBJECT
public:
  typedef QWidget Superclass;
  qSlicerRepairFooBarWidget(QWidget *parent=0);
  virtual ~qSlicerRepairFooBarWidget();
  void CreateDentureModel();
  QList<vtkVector3d> ExtractFiducialCentroidFromCT(QString mode);

public slots:
	void setDicomProperty();
	void onImportDenture();
	void onImportDentureDICOM();
	void onImportPatientDICOM();

	void onEditFiducialPoints(QString mode);

	void onEditFiducialPoints(QString mode, QList<vtkVector3d> FiducialList);


	void onPatientVolumeNodeAdded(vtkObject* scene, vtkObject* node);
	void onDentureVolumeNodeAdded(vtkObject* scene, vtkObject* node);

	vtkSmartPointer<vtkMatrix4x4> TransformMatrix(vtkImageData* reader, float* ori);

protected slots:

protected:
  QScopedPointer<qSlicerRepairFooBarWidgetPrivate> d_ptr;

private:

  Q_DECLARE_PRIVATE(qSlicerRepairFooBarWidget);
  Q_DISABLE_COPY(qSlicerRepairFooBarWidget);
  void fiducialRegistration();
  void volumeRender();

private slots:
	void updateDentrueTransform();
	void onIsFiducialsEqualBetweenModelAndImage();
	void onHandlesVisibilityClicked(bool visibility);
	void onDentureVisibilityClicked(bool visibility);
	void onDentureRegistration();
	void AutoDentureRegistration();


signals:
	void moduleStateChanged();
};

class FiducialTableWidget :public QTableWidget 
{
	Q_OBJECT
	QVTK_OBJECT
public:
	FiducialTableWidget(QWidget* parent = 0);
	~FiducialTableWidget();
	void setActiveMarkupNodeID(std::string newID);

private:
	QStringList columnLabels;
	std::string activeMarkupsNodeID;
private slots:
	void onActiveMarkupTableCurrentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn);
	void onRightClickActiveMarkupTableWidget(QPoint pos);
	void onDeleteMarkupPushButtonClicked(bool confirm=true);
	void onJumpSlicesActionTriggered();
	void updateRow(int m);
	void onUpdateFiducialTable();
	void onActiveMarkupsNodePointModifiedEvent(vtkObject* caller, void* callData);
	void onActiveMarkupsNodePointAddedEvent();
	void onActiveMarkupsNodePointRemovedEvent(vtkObject* caller, void* callData);
	void onActiveMarkupTableCellClicked(QTableWidgetItem* item);
	void onActiveMarkupTableCellChanged(int row, int column);


};
#endif
