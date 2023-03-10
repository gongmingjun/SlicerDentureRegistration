#ifndef __myPointDistanceMatrix_h
#define __myPointDistanceMatrix_h

#include <vtkDoubleArray.h>
#include <vtkObject.h>
#include <vtkPoints.h>
#include <vtkTimeStamp.h>

// export
//#include "vtkSlicerFiducialRegistrationWizardModuleLogicExport.h"

// At its most basic level, this is a Wrapper class for storing a matrix of
// point to point distances in a vtkDoubleArray.
// The purpose is to improve the abstraction of storing distances, instead of
// memorizing how distances are accessed etc... One can use this class to
// encapsulate that functionality.
// The contents of the matrix are automatically re-generated when either input
// point list is changed.
class /*VTK_SLICER_FIDUCIALREGISTRATIONWIZARD_MODULE_LOGIC_EXPORT*/ myPointDistanceMatrix : public vtkObject //vtkAlgorithm?
{
  public:
    vtkTypeMacro( myPointDistanceMatrix, vtkObject );
    void PrintSelf( ostream &os, vtkIndent indent ) override;
    
    static myPointDistanceMatrix* New();
    
    vtkPoints* GetPointList1();
    vtkPoints* GetPointList2();
    double GetDistance( int list1Index, int list2Index );
    void GetDistances( vtkDoubleArray* outputArray );
    vtkGetMacro( MaximumDistance, int );
    vtkGetMacro( MinimumDistance, int );

    void SetPointList1( vtkPoints* points );
    void SetPointList2( vtkPoints* points );
    
    void Update();

  protected:
    myPointDistanceMatrix();
    ~myPointDistanceMatrix();
  private:
    // inputs
    vtkPoints* PointList1;
    vtkPoints* PointList2;

    // outputs
    vtkSmartPointer< vtkDoubleArray > DistanceMatrix;
    vtkTimeStamp MatrixUpdateTime;
    double MaximumDistance;
    double MinimumDistance;

    bool UpdateNeeded();
    bool InputsContainErrors( bool verbose=true );

    void ResetDistances();

		myPointDistanceMatrix(const myPointDistanceMatrix&); // Not implemented.
		void operator=(const myPointDistanceMatrix&); // Not implemented.
};

#endif
