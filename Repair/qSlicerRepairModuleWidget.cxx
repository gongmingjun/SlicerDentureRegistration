/*==============================================================================

  Program: 3D Slicer

  Portions (c) Copyright Brigham and Women's Hospital (BWH) All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

==============================================================================*/

// Qt includes
#include <QDebug>
#include <QDialogButtonBox>
#include <qstatusbar.h>
#include <qpushbutton.h>
#include <QDockWidget>
#include "qmainwindow.h"
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>
// Slicer includes
#include "qSlicerRepairModuleWidget.h"
#include "ui_qSlicerRepairModuleWidget.h"
#include "vtkSlicerRepairLogic.h"

#include "qSlicerRepairModule.h"
#include "qSlicerRepairModuleWidget.h"
#include "qSlicerRepairModule.h"
#include "qSlicerApplication.h"
#include "qSlicerRepairTabWidget.h"
#include "qSlicerModuleManager.h"
#include <qSlicerApplication.h>
#include <vtkMRMLScene.h>
#include <vtkMRMLNode.h>
#include <vtkMRMLModelNode.h>
#include <vtkMRMLStorageNode.h>
#include "vtkMRMLLinearTransformNode.h"
#include "qSlicerIOManager.h"
#include <vtkMRMLMarkupsFiducialNode.h>
//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerRepairModuleWidgetPrivate: public Ui_qSlicerRepairModuleWidget
{
public:
  qSlicerRepairModuleWidgetPrivate();
};

//-----------------------------------------------------------------------------
// qSlicerRepairModuleWidgetPrivate methods

//-----------------------------------------------------------------------------
qSlicerRepairModuleWidgetPrivate::qSlicerRepairModuleWidgetPrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerRepairModuleWidget methods

//-----------------------------------------------------------------------------
qSlicerRepairModuleWidget::qSlicerRepairModuleWidget(QWidget* _parent)
  : Superclass( _parent )
  , d_ptr( new qSlicerRepairModuleWidgetPrivate )
{
}

//-----------------------------------------------------------------------------
qSlicerRepairModuleWidget::~qSlicerRepairModuleWidget()
{
	//if (this->btnBox)
	//	delete this->btnBox;
	//if (this->repairTabWidget)
	//	delete this->repairTabWidget;
}

void qSlicerRepairModuleWidget::enter()
{
	vtkMRMLMarkupsFiducialNode* dentureInteractionFiducialNode = vtkMRMLMarkupsFiducialNode::SafeDownCast(
		qSlicerApplication::application()->mrmlScene()->GetFirstNodeByName("dentureInteractionFid"));
	if (dentureInteractionFiducialNode)
	{
		dentureInteractionFiducialNode->SetDisplayVisibility(true);
	}
	vtkMRMLMarkupsFiducialNode* imageFiducialNode = vtkMRMLMarkupsFiducialNode::SafeDownCast(
		qSlicerApplication::application()->mrmlScene()->GetNodeByID("vtkMRMLMarkupsFiducialNodeImage"));
	if (imageFiducialNode)
	{
		imageFiducialNode->SetDisplayVisibility(true);
	}
	vtkMRMLMarkupsFiducialNode* modelFiducialNode = vtkMRMLMarkupsFiducialNode::SafeDownCast(
		qSlicerApplication::application()->mrmlScene()->GetNodeByID("vtkMRMLMarkupsFiducialNodeModel"));

	if (modelFiducialNode)
	{
		modelFiducialNode->SetDisplayVisibility(true);
	}
}

void qSlicerRepairModuleWidget::exit()
{
	vtkMRMLMarkupsFiducialNode* dentureInteractionFiducialNode = vtkMRMLMarkupsFiducialNode::SafeDownCast(
		qSlicerApplication::application()->mrmlScene()->GetFirstNodeByName("dentureInteractionFid"));
	if (dentureInteractionFiducialNode)
	{
		dentureInteractionFiducialNode->SetDisplayVisibility(false);
		
	}
	vtkMRMLMarkupsFiducialNode* imageFiducialNode = vtkMRMLMarkupsFiducialNode::SafeDownCast(
		qSlicerApplication::application()->mrmlScene()->GetNodeByID("vtkMRMLMarkupsFiducialNodeImage"));
	if (imageFiducialNode)
	{
		imageFiducialNode->SetDisplayVisibility(false);
	}
	vtkMRMLMarkupsFiducialNode* modelFiducialNode = vtkMRMLMarkupsFiducialNode::SafeDownCast(
		qSlicerApplication::application()->mrmlScene()->GetNodeByID("vtkMRMLMarkupsFiducialNodeModel"));

	if (modelFiducialNode)
	{
		modelFiducialNode->SetDisplayVisibility(false);
	}
}


//-----------------------------------------------------------------------------
void qSlicerRepairModuleWidget::setup()
{
  Q_D(qSlicerRepairModuleWidget);
  d->setupUi(this);
  this->Superclass::setup();

  ////设置主窗口的repairTab
  //QLayout* repairTab = qSlicerApplication::application()->mainWindow()->findChild < QLayout* >("repairGridLayout");
  //repairTabWidget = new qSlicerRepairTabWidget();
  //repairTab->addWidget(repairTabWidget);
  //repairTabWidget->setObjectName(QString::fromUtf8("repairTabWidget"));

  ////设置repair的btnbox
  //this->btnBox = repairTabWidget->GetButtonBox(); //new QDialogButtonBox(Qt::Horizontal);
  //this->btnBox->setObjectName("statusButtonBoxRepair");
  //this->btnBox->setStandardButtons(QDialogButtonBox::Ok);
  //QList<QAbstractButton*> buttons = btnBox->buttons();
  //while (buttons.size())
  //{
	 // buttons.takeFirst()->setMinimumSize(80, 60);
  //}
  //this->btnBox->hide();

  ////连接foobar 与 tab
  //QObject::connect(repairTabWidget, &qSlicerRepairTabWidget::importDentureButtonClicked, d->FooBar, &qSlicerRepairFooBarWidget::onImportDenture);
  //QObject::connect(repairTabWidget, &qSlicerRepairTabWidget::importDentureDICOMButtonClicked, d->FooBar, &qSlicerRepairFooBarWidget::onImportDentureDICOM);
  //QObject::connect(this->btnBox, &QDialogButtonBox::clicked, this, &qSlicerRepairModuleWidget::onBtnBoxClicked);
  //QObject::connect(d->FooBar, &qSlicerRepairFooBarWidget::moduleStateChanged, this, &qSlicerRepairModuleWidget::updateFromModuleState);
  //QObject::connect(repairTabWidget, &qSlicerRepairTabWidget::moduleStateChanged, this, &qSlicerRepairModuleWidget::updateFromModuleState);


  ////保存、打开模块
  //qvtkConnect(this->logic(), vtkSlicerRepairLogic::RepairModuleSaveEvent, this, SLOT(onSaveModule()));
  //qvtkConnect(this->logic(), vtkSlicerRepairLogic::RepairModuleLoadEvent, this, SLOT(onLoadModule()));
}


//
//template<typename NODE_TYPE>
//bool qSlicerRepairModuleWidget::WriteNode(std::string nodeName, std::string extensionName)
//{
//	vtkSlicerRepairLogic* logic = vtkSlicerRepairLogic::SafeDownCast(this->logic());
//	NODE_TYPE* node = nullptr;
//	if (node = NODE_TYPE::SafeDownCast(qSlicerApplication::application()->mrmlScene()->GetFirstNodeByName(nodeName.c_str())))
//	{
//		node->AddDefaultStorageNode();
//		node->GetStorageNode()->SetFileName((logic->getModulePath() + "/" + nodeName + extensionName).c_str());
//		return node->GetStorageNode()->WriteData(node);
//	}
//	else
//	{
//		std::cout << nodeName << "export failed" << std::endl;
//		return false;
//	}
//}
//
//
///// <summary>
///// 保存模块
///// </summary>
//void qSlicerRepairModuleWidget::onSaveModule()
//{
//	Q_D(qSlicerRepairModuleWidget);
//	//vtkSlicerRepairLogic* logic = vtkSlicerRepairLogic::SafeDownCast(this->logic());
//
//	//this->WriteNode<vtkMRMLModelNode>("DentureTransformed", ".stl");
//	//this->WriteNode<vtkMRMLMarkupsFiducialNode>("ModelFiducial", ".mrk.moduleJson");
//	//this->WriteNode<vtkMRMLMarkupsFiducialNode>("ImageFiducial", ".mrk.moduleJson");
//	//this->WriteNode<vtkMRMLMarkupsFiducialNode>("dentureInteractionFid", ".mrk.moduleJson");
//	//this->WriteNode<vtkMRMLLinearTransformNode>("Repair Transform", ".h5");
//
//	std::cout << "qSlicerRepairModuleWidget::onSaveModule()" << std::endl;
//	vtkSlicerRepairLogic* logic = vtkSlicerRepairLogic::SafeDownCast(this->logic());
//
//	// 以读写方式打开主目录下的_module_settings.json文件，若该文件不存在则会自动创建
//	QFile file(QString::fromStdString(logic->getModulePath() + "/_module_settings.json"));
//	if (!file.open(QIODevice::ReadWrite)) {
//		qDebug() << "File open error";
//	}
//	else {
//		qDebug() << "File open!";
//	}
//
//	QJsonObject moduleJson;
//	
//	moduleJson.insert("Foobar",d->FooBar->getModuleJson());
//	QJsonParseError error;
//	QJsonDocument document = QJsonDocument::fromJson(QString(file.readAll()).toUtf8(), &error);
//	if (document.isEmpty())
//	{
//		document.setObject(moduleJson);
//	}
//	else
//	{
//		QJsonObject existJson = document.object();
//		existJson.insert("Repair", moduleJson);
//		document.setObject(existJson);
//	}
//	//if (!document.isEmpty() && (error.error == QJsonParseError::NoError)) {
//	//	QJsonObject moduleJson = document.object();
//	//	moduleJson.insert("id", "1");
//	//}
//	file.resize(0);
//	file.write(document.toJson());
//	file.close();
//
//}
//
///// <summary>
///// 打开模块
///// </summary>
//void qSlicerRepairModuleWidget::onLoadModule()
//{
//	std::cout << "qSlicerRepairModuleWidget::onLoadModule()" << std::endl;
//	Q_D(qSlicerRepairModuleWidget);
//	vtkSlicerRepairLogic* logic = vtkSlicerRepairLogic::SafeDownCast(this->logic());
//	// 以读写方式打开主目录下的_module_settings.json文件，若该文件不存在则会自动创建
//	QFile file(QString::fromStdString(logic->getModulePath() + "/_module_settings.json"));
//	if (!file.open(QIODevice::ReadOnly)) {
//		qDebug() << "File open error";
//	}
//	else {
//		qDebug() << "File open!";
//	}
//	QJsonParseError error;
//	QJsonDocument document = QJsonDocument::fromJson(QString(file.readAll()).toUtf8(), &error);
//	QJsonObject moduleJson = document.object().value("Repair").toObject();
//	
//	d->FooBar->loadModule(moduleJson.value("Foobar").toObject());
//
//	file.close();
//
//	
//}
//
///// <summary>
///// 根据vtkSlicerRepairLogic的state更新btnBox和repairTabWidget
///// </summary>
//void qSlicerRepairModuleWidget::updateFromModuleState()
//{
//	int state = vtkSlicerRepairLogic::SafeDownCast(qSlicerApplication::application()->moduleLogic("Repair"))->GetState();
//	if (state == vtkSlicerRepairLogic::DentureRegistratingState)
//	{
//		//配准中，tab enableoff ； btnbox显示 ;panel显示
//		QWidget* tabWidget = qSlicerApplication::application()->mainWindow()->findChild < QWidget* >("repairControlwidget");
//		tabWidget->setEnabled(false);
//		this->btnBox->show();
//		QDockWidget* dockWidget = qSlicerApplication::application()->mainWindow()->findChild<QDockWidget*>("PanelDockWidget");
//		dockWidget->show();
//	}
//	else if (state == vtkSlicerRepairLogic::IdleState)
//	{
//		//空闲，tab enableon ; btnbox不显示 ；panel 隐藏
//		QWidget* tabWidget = qSlicerApplication::application()->mainWindow()->findChild < QWidget* >("repairControlwidget");
//		tabWidget->setEnabled(true);
//		this->btnBox->hide();
//		QDockWidget* dockWidget = qSlicerApplication::application()->mainWindow()->findChild<QDockWidget*>("PanelDockWidget");
//		dockWidget->hide();
//	}
//}
//
///// <summary>
///// buttonbox槽函数
///// </summary>
//void qSlicerRepairModuleWidget::onBtnBoxClicked(QAbstractButton* button)
//{
//	int state = vtkSlicerRepairLogic::SafeDownCast(qSlicerApplication::application()->moduleLogic("Repair"))->GetState();
//	if (state == vtkSlicerRepairLogic::DentureRegistratingState)
//	{
//		if (this->btnBox->button(QDialogButtonBox::Ok) == button)//ok，返回界面
//		{
//			vtkSlicerRepairLogic::SafeDownCast(qSlicerApplication::application()->moduleLogic("Repair"))->SetState(vtkSlicerRepairLogic::IdleState);
//			this->updateFromModuleState();
//		}
//		else if (this->btnBox->button(QDialogButtonBox::Cancel) == button)
//		{
//			//Todo 撤销配准
//		}
//	}
//}