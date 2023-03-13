

#ifndef myMultiObjectMassProperties_h
#define myMultiObjectMassProperties_h

#include "vtkFiltersCoreModule.h" // For export macro
#include "vtkPolyDataAlgorithm.h"
//#include <vtkPolygon.h>
class vtkDoubleArray;
class vtkUnsignedCharArray;
class vtkIdTypeArray;
//vtkStandardNewMacro(myMultiObjectMassProperties);

class /*VTKFILTERSCORE_EXPORT*/ myMultiObjectMassProperties : public vtkPolyDataAlgorithm
{
public:
  ///@{
  /**
   * Standard methods for construction, type and printing.
   */
  static myMultiObjectMassProperties* New();
  //myMultiObjectMassProperties::myMultiObjectMassProperties();

  //vtkTypeMacro(myMultiObjectMassProperties, vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;
  ///@}

  ///@{
  /**
   * Indicate whether to skip the validity check (the first part of the
   * algorithm). By default this is off; however even if enabled validity
   * skipping will only occur if a vtkIdTypeArray named "ObjectIds" is also
   * provided on input to the filter.
   */
  //vtkSetMacro(SkipValidityCheck, vtkTypeBool);
  //vtkGetMacro(SkipValidityCheck, vtkTypeBool);
  //vtkBooleanMacro(SkipValidityCheck, vtkTypeBool);
  ///@}

  ///@{
  /**
   * Set/Get the name of the ObjectIds array. This array, which indicates in which object
   * a polygon belongs to, can be either provided by the user or computed.
   * The default name is "ObjectIds".
   */
  //vtkSetStdStringFromCharMacro(ObjectIdsArrayName);
  //vtkGetCharFromStdStringMacro(ObjectIdsArrayName);
  ///@}

  /**
   * Return the number of objects identified. This is valid only after the
   * filter executes. Check the ObjectValidity array which indicates which of
   * these identified objects are valid. Invalid objects may have incorrect
   * volume and area values.
   */
  vtkIdType GetNumberOfObjects(); 

  /**
   * Return whether all objects are valid or not. This is valid only after the
   * filter executes.
   */
  vtkTypeBool GetAllValid();

  /**
   * Return the summed volume of all objects. This is valid only after the
   * filter executes.
   */
  double GetTotalVolume();

  /**
   * Return the summed area of all objects. This is valid only after the
   * filter executes.
   */
  double GetTotalArea();

protected:
    myMultiObjectMassProperties();
  ~myMultiObjectMassProperties() override;

  int RequestData(vtkInformation* request, vtkInformationVector** inputVector,
    vtkInformationVector* outputVector) override;

  // Data members supporting API
  vtkTypeBool SkipValidityCheck;
  vtkTypeBool AllValid;
  double TotalVolume;
  double TotalArea;

  // Internal data members supporting algorithm execution
  vtkIdType NumberOfObjects;      // number of objects identified
  vtkIdTypeArray* ObjectIds;      // for each input polygon, the object id that the polygon is in
  std::string ObjectIdsArrayName; // the array name of ObjectIds.

  vtkUnsignedCharArray* ObjectValidity; // is it a valid object?
  vtkDoubleArray* ObjectVolumes;        // what is the object volume (if valid)?
  vtkDoubleArray* ObjectAreas;          // what is the total object area?
  vtkDoubleArray* ObjectCentroids;      // what is the object centroid

  vtkIdList* CellNeighbors; // avoid repetitive new/delete
  vtkIdList* Wave;          // processing wave
  vtkIdList* Wave2;

  // Connected traversal to identify objects
  void TraverseAndMark(
    vtkPolyData* output, vtkIdType* objectIds, vtkDataArray* valid, unsigned char* orient);

private:
    myMultiObjectMassProperties(const myMultiObjectMassProperties&) = delete;
  void operator=(const myMultiObjectMassProperties&) = delete;
};

#endif
