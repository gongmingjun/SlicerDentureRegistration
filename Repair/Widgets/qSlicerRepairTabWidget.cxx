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

// qSlicerVolumeRendering includes
#include "ui_qSlicerRepairTabWidget.h"

// Qt includes
#include <QDebug>
#include <QDockWidget>
#include <qmainwindow.h>
#include <QMessageBox>

// CTK includes
#include <ctkUtils.h>
#include <ctkMessageBox.h>
// MRMLWidgets includes
#include <qMRMLSceneModel.h>

// MRML includes
#include <vtkMRMLScalarVolumeNode.h>
#include <vtkMRMLVolumePropertyNode.h>
#include <vtkSlicerRepairLogic.h>
#include <vtkMRMLScene.h>
// VTK includes
#include <vtkWeakPointer.h>
#include <vtkImageData.h>
#include <vtkColorTransferFunction.h>
#include <vtkVolumeProperty.h>

//-----------------------------------------------------------------------------
// Tab Widgets includes
#include "qSlicerRepairTabWidget.h"
#include "PythonQt.h"
#include <qSlicerLayoutManager.h>
#include <qSlicerApplication.h>
//#include "qSlicerCustomLayout.h"
//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_Util
class qSlicerRepairTabWidgetPrivate
    : public Ui_qSlicerRepairTabWidget
{
    Q_DECLARE_PUBLIC(qSlicerRepairTabWidget);
protected:
    qSlicerRepairTabWidget* const q_ptr;

public:
    qSlicerRepairTabWidgetPrivate(
        qSlicerRepairTabWidget& object);
    virtual void setupUi(qSlicerRepairTabWidget*);
};

// --------------------------------------------------------------------------
qSlicerRepairTabWidgetPrivate
::qSlicerRepairTabWidgetPrivate(
    qSlicerRepairTabWidget& object)
    : q_ptr(&object)
{
}

// --------------------------------------------------------------------------
void qSlicerRepairTabWidgetPrivate
::setupUi(qSlicerRepairTabWidget* widget)
{
    this->Ui_qSlicerRepairTabWidget::setupUi(widget);
}

//-----------------------------------------------------------------------------
// qSlicerRepairTabWidget methods

//-----------------------------------------------------------------------------
qSlicerRepairTabWidget
::qSlicerRepairTabWidget(QWidget* parentWidget)
    : Superclass(parentWidget)
    , d_ptr(new qSlicerRepairTabWidgetPrivate(*this))
{
    Q_D(qSlicerRepairTabWidget);
    d->setupUi(this);
	QDockWidget* dockWidget = qSlicerApplication::application()->mainWindow()->findChild<QDockWidget*>("PanelDockWidget");
	dockWidget->show();

    QObject::connect(d->importDentureButton, &QPushButton::clicked,
        [this]() {emit importDentureButtonClicked(); });
	QObject::connect(d->importDentureDICOMButton, &QPushButton::clicked,
		[this]() {emit importDentureDICOMButtonClicked(); });

    QObject::connect(d->denturePlacementButton, &QPushButton::clicked,
        [this]() {
        if (qSlicerApplication::application()->mrmlScene()->GetFirstNodeByName("Denture"))
        {
            QDockWidget* dockWidget = qSlicerApplication::application()->mainWindow()->findChild<QDockWidget*>("PanelDockWidget");
            dockWidget->show();
            vtkSlicerRepairLogic::SafeDownCast(qSlicerApplication::application()->moduleLogic("Repair"))
                ->SetState(vtkSlicerRepairLogic::DentureRegistratingState);
            emit moduleStateChanged();
        }
        else
        {
			ctkMessageBox::warning(this, "warning", "Please place denture first.", QMessageBox::Ok, QMessageBox::Ok);
        }
    });
}

//-----------------------------------------------------------------------------
qSlicerRepairTabWidget
::~qSlicerRepairTabWidget()
{
}


QDialogButtonBox* qSlicerRepairTabWidget::GetButtonBox()
{
    Q_D(qSlicerRepairTabWidget);
    return d->buttonBox;
}

