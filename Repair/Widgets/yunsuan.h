#include<algorithm>
#include<vector>
#include<iostream>
#include "QList.h"
#include "vtkVector.h"
using namespace std;

class yunsuan
{
public:
	yunsuan();
	~yunsuan();

	void Point_Registration(QList<vtkVector3d> DentureFiducialList_p,
		QList<vtkVector3d> DentureFiducialList_t,
		QList<vtkVector3d>& DentureFiducialList_1,
		QList<vtkVector3d>& DentureFiducialList_2);

	std::vector<double> patient_x;
	std::vector<double> patient_y;
	std::vector<double> patient_z;
	std::vector<double> tooth_x;
	std::vector<double> tooth_y;
	std::vector<double> tooth_z;
	std::vector<double> error_group;
	std::vector<int> wrong_teeth_num;
	std::vector<std::vector<double>> tooth_dis;
	std::vector<std::vector<double>> ontooth_src;
	std::vector<double> tooth_dis2; //用于匹配计算
	std::vector<double> onpatient_dis;
	std::vector<std::vector<double>> patient_dis;
	std::vector<double> patient_point;
	std::vector<double> tar;
	std::vector<std::vector<double>> optional_number;
	std::vector<std::vector<double>> patient_closest_point;
	QList<vtkVector4d> list_ans;
	int teeth;
	int patient;
	double teeth_cen[3];
	double patient_cen[3];
	int* p;//储存当前最合适的一组点
	double min_value;
	bool on_target;

	void point_input(QList<vtkVector3d> DentureFiducialList_p,
		QList<vtkVector3d> DentureFiducialList_t, int p_size, int t_size);
	void onTooth_dis();
	void onPatientgroup_dis();
	void onPatient_dis();
	void select_point(int pos, int process, int totla, int key, std::vector<double>& a, std::vector<int> b, std::vector<int>& p);
	void select_point_2(int pos, int process, int totla, std::vector<std::vector<double>> aa, std::vector<double>& tar, std::vector<double>& ans);
	void Point_Screening(QList<vtkVector3d>& DentureFiducialList_1,
		QList<vtkVector3d>& DentureFiducialList_2);
	void Processing_special_point(QList<vtkVector3d>& DentureFiducialList_1, QList<vtkVector3d>& DentureFiducialList_2);
	void Point_Sorting(QList<vtkVector3d>& DentureFiducialList_1, QList<vtkVector3d>& DentureFiducialList_2);
	double Find_Closest(int mn, int mx, double a, std::vector<double> p);
};

