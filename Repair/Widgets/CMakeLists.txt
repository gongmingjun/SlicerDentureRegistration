project(qSlicer${MODULE_NAME}ModuleWidgets)

set(KIT ${PROJECT_NAME})

find_package(SlicerExecutionModel REQUIRED)
include(${SlicerExecutionModel_USE_FILE})

set(${KIT}_EXPORT_DIRECTIVE "Q_SLICER_MODULE_${MODULE_NAME_UPPER}_WIDGETS_EXPORT")

set(${KIT}_INCLUDE_DIRECTORIES
	${CMAKE_CURRENT_SOURCE_DIR}/../../Util/
	${CMAKE_CURRENT_SOURCE_DIR}/../../Repair/

  )

set(${KIT}_SRCS
  qSlicer${MODULE_NAME}FooBarWidget.cxx
  qSlicer${MODULE_NAME}FooBarWidget.h
  qSlicer${MODULE_NAME}TabWidget.cxx
  qSlicer${MODULE_NAME}TabWidget.h
  myMultiObjectMassProperties.cxx
  myMultiObjectMassProperties.h
  yunsuan.cxx
  yunsuan.h
 myPointMatcher.cxx
  myPointMatcher.h
myCombinatoricGenerator.cxx
myCombinatoricGenerator.h
myPointDistanceMatrix.cxx
myPointDistanceMatrix.h
  )

set(${KIT}_MOC_SRCS
  qSlicer${MODULE_NAME}FooBarWidget.h
  qSlicer${MODULE_NAME}TabWidget.h

  )

set(${KIT}_UI_SRCS
  ../Resources/UI/qSlicer${MODULE_NAME}FooBarWidget.ui
  ../Resources/UI/qSlicer${MODULE_NAME}TabWidget.ui
  )

set(${KIT}_RESOURCES
  ../Resources/qSlicer${MODULE_NAME}Module.qrc
  )

set(${KIT}_TARGET_LIBRARIES
  vtkSlicer${MODULE_NAME}ModuleLogic
  vtkSlicerModelsModuleLogic
  vtkSlicerMarkupsModuleLogic

  vtkSlicerSegmentationsModuleLogic
  qSlicerSegmentationsModuleWidgets

  vtkSlicerAnnotationsModuleLogic
  vtkSlicerCropVolumeModuleLogic
  vtkSlicerVolumeRenderingModuleLogic
  vtkSlicerCamerasModuleLogic
  qSlicerVolumesModuleWidgets
  qSlicerVolumeRenderingModuleWidgets
  )

#-----------------------------------------------------------------------------
SlicerMacroBuildModuleWidgets(
  NAME ${KIT}
  EXPORT_DIRECTIVE ${${KIT}_EXPORT_DIRECTIVE}
  INCLUDE_DIRECTORIES ${${KIT}_INCLUDE_DIRECTORIES}
  SRCS ${${KIT}_SRCS}
  MOC_SRCS ${${KIT}_MOC_SRCS}
  UI_SRCS ${${KIT}_UI_SRCS}
  TARGET_LIBRARIES ${${KIT}_TARGET_LIBRARIES}
  RESOURCES ${${KIT}_RESOURCES}
  WRAP_PYTHONQT
  )
