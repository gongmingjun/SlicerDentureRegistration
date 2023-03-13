/*==============================================================================

  Copyright (c) Laboratory for Percutaneous Surgery (PerkLab)
  Queen's University, Kingston, ON, Canada. All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  This file was originally developed by Csaba Pinter, PerkLab, Queen's University
  and was supported through CANARIE's Research Software Program, and Cancer
  Care Ontario.

==============================================================================*/

#ifndef __qSlicerRepairTabWidget_h
#define __qSlicerRepairTabWidget_h

// Qt includes
#include <QWidget>
#include <QSlider>

// FooBar Widgets includes
#include "qSlicerRepairModuleWidgetsExport.h"
#include <qdialogbuttonbox.h>
class qSlicerRepairTabWidgetPrivate;
class QString;
/// \ingroup Slicer_QtModules_Util
class Q_SLICER_MODULE_REPAIR_WIDGETS_EXPORT qSlicerRepairTabWidget
	: public QWidget
{
	Q_OBJECT
public:
	typedef QWidget Superclass;
	qSlicerRepairTabWidget(QWidget* parent = 0);
	virtual ~qSlicerRepairTabWidget();

	QDialogButtonBox* GetButtonBox();
	/*QSlider* GetPatientSlider();
	QSlider* GetDentureSlider();

	void onPatientChanged(int a);
	void onDentureChanged(int a);*/
protected slots:


protected:
	QScopedPointer<qSlicerRepairTabWidgetPrivate> d_ptr;

private:
	Q_DECLARE_PRIVATE(qSlicerRepairTabWidget);
	Q_DISABLE_COPY(qSlicerRepairTabWidget);
signals:
	void importDentureButtonClicked();
	void importDentureDICOMButtonClicked();
	void sigPatientThresholdChanged(int a);
	void sigDentureThresholdChanged(int a);

	//void denturePlacementButtonClicked();
	//void editImageFiducialButtonClicked(QString);
	//void editModelFiducialButtonClicked(QString);
	void moduleStateChanged();
};

#endif
