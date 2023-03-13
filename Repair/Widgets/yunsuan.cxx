//#include <vtkAutoInit.h>
#include<algorithm>
#include<stdio.h>
#include<vector>
#include<iostream>
#include <math.h>
#include <cmath>
#include <numeric>
#include "yunsuan.h"
#include "QList.h"
#include "vtkVector.h"
using namespace std;

clock_t start2, finish2;
double  duration2;

yunsuan::yunsuan()
{
	on_target = false;
	min_value = 0;
	p = NULL;
}

yunsuan::~yunsuan()
{

}

void yunsuan::Point_Registration(QList<vtkVector3d> DentureFiducialList_p,
	QList<vtkVector3d> DentureFiducialList_t,
	QList<vtkVector3d>& DentureFiducialList_1,
	QList<vtkVector3d>& DentureFiducialList_2)
{
	this->point_input(DentureFiducialList_p, DentureFiducialList_t, DentureFiducialList_p.size(), DentureFiducialList_t.size());
	this->onTooth_dis();//计算标准数据集距离
	this->onPatient_dis();//计算患者距离数据
	this->Point_Screening(DentureFiducialList_1, DentureFiducialList_2);//预测相似点法
}

void yunsuan::point_input(QList<vtkVector3d> DentureFiducialList_p,
	QList<vtkVector3d> DentureFiducialList_t, int p_size, int t_size)
{
	for (int i = 0; i < p_size; i++)
	{
		patient_x.push_back(DentureFiducialList_p[i][0]);
		patient_y.push_back(DentureFiducialList_p[i][1]);
		patient_z.push_back(DentureFiducialList_p[i][2]);
	}
	for (int i = 0; i < t_size; i++)
	{
		tooth_x.push_back(DentureFiducialList_t[i][0]);
		tooth_y.push_back(DentureFiducialList_t[i][1]);
		tooth_z.push_back(DentureFiducialList_t[i][2]);
	}
}

void yunsuan::onTooth_dis()
{
	teeth = tooth_x.size();
	//计算一维牙齿距离向量用于穷举法比对
	for (int i = 0; i < teeth - 1; i++)
	{
		for (int j = 1; j < teeth - i; j++)
		{
			double dis = sqrt((double)pow((tooth_x[i] - tooth_x[i + j]), 2)
				+ (double)pow((tooth_y[i] - tooth_y[i + j]), 2)
				+ (double)pow((tooth_z[i] - tooth_z[i + j]), 2));
			tooth_dis2.push_back(dis);
		}
	}
	sort(tooth_dis2.begin(), tooth_dis2.end());
	//用于计算二维距离矩阵
	for (int i = 0; i < teeth - 1; i++)
	{
		std::vector<double> group;
		for (int j = 0; j < i + 1; j++)
		{
			group.push_back(0);
		}

		for (int j = 1; j < teeth - i; j++)
		{
			double dis = sqrt((double)pow((tooth_x[i] - tooth_x[i + j]), 2)
				+ (double)pow((tooth_y[i] - tooth_y[i + j]), 2)
				+ (double)pow((tooth_z[i] - tooth_z[i + j]), 2));
			group.push_back(dis);
		}
		tooth_dis.push_back(group);
	}

	for (int i = 0; i < teeth; i++)
	{
		std::vector<double> group_src;
		//获取每个点到其余点的距离序列
		for (int j = 0; j < teeth; j++)
		{
			if (i > j) {
				group_src.push_back(tooth_dis[j][i]);
			}
			else if (i < j) {
				group_src.push_back(tooth_dis[i][j]);
			}
		}
		ontooth_src.push_back(group_src);
	}
}

void yunsuan::onPatient_dis()
{
	patient = patient_x.size();
	for (int i = 0; i < patient - 1; i++)
	{
		std::vector<double> group;
		for (int j = 0; j < i + 1; j++)
		{
			group.push_back(0);
		}
		
		for (int j = 1; j < patient - i; j++)
		{
			double dis = sqrt((double)pow((patient_x[i] - patient_x[i + j]), 2)
				+ (double)pow((patient_y[i] - patient_y[i + j]), 2)
				+ (double)pow((patient_z[i] - patient_z[i + j]), 2));
			group.push_back(dis);
		}
		patient_dis.push_back(group);
	}
}

void yunsuan::onPatientgroup_dis()//原始方法
{
	std::vector<double> onpatient;
	
	double error = 0;
	int min_error = 0;
	
	start2 = clock();
	for (int i = 0; i < patient_point.size() - 1; i++)
	{
		for (int j = 1; j < patient_point.size() - i; j++)
		{
			//cout << "i:" << i << "j:" << j << endl;
			onpatient.push_back(patient_dis[patient_point[i]][patient_point[i + j]]);
		}
	}
	finish2 = clock();
	sort(onpatient.begin(), onpatient.end());
	
	for (int i = 0; i < tooth_dis2.size(); i++)
	{
		double err = pow((onpatient[i] - tooth_dis2[i]), 2);
		error = err + error;
	}
	error_group.emplace_back(error);
	//计算误差最小值
	
	if (error <= min_value || min_value == 0)
	{
		min_value = error;
		on_target = true;
		cout << "min:" << error << endl;
		for (int i = 0; i < patient_point.size(); i++)
		{
			cout << patient_point[i] << " ";
		}
		cout << endl;
	}
	
	duration2 += (double)(finish2 - start2) / CLOCKS_PER_SEC;
}

void yunsuan::select_point(int pos, int process, int totla, int key, std::vector<double>& a, std::vector<int> b, std::vector<int>& p)
{
	if (process > totla)
		return;
	if (pos == key)//该次组合完毕
	{
		for (int i = 0; i < key; i++)
		{
			this->patient_point.push_back(b[i]);
		}
		this->onPatientgroup_dis();
		if (on_target == true)
		{
			for (int i = 0; i < key; i++)
			{
				p[i] = patient_point[i];
			}
			cout << "ti:" << duration2 << endl;
			on_target = false;
		}
		patient_point.clear();
		return;
	}
	else
	{
		select_point(pos, process + 1, totla, key, a, b, p);
		if (process >= this->patient)
		{
			select_point(pos + 1, process + 1, totla, key, a, b, p);
		}
		else
		{
			b[pos] = a[process];
			select_point(pos + 1, process + 1, totla, key, a, b, p);
		}
	}
}

void yunsuan::Point_Screening(QList<vtkVector3d>& DentureFiducialList_1,
	QList<vtkVector3d>& DentureFiducialList_2)
{
	for (int i = 0; i < patient; i++)
	{
		std::vector<double> onpatient_src;
		//获取每个点到其余点的距离序列
		for (int j = 0; j < patient; j++)
		{
			if (i > j) {
				onpatient_src.push_back(patient_dis[j][i]);
			}
			else if(i < j){
				onpatient_src.push_back(patient_dis[i][j]);
			}
		}
		//寻找当前序列与标准序列最接近的点的数量以及误差
		double sum = 0;
		double min_error = 0;
		double min_num = 0;
		sort(onpatient_src.begin(), onpatient_src.end());
		for (int k = 0; k < ontooth_src.size(); k++)
		{
			for (int m = 0; m < ontooth_src[k].size(); m++)
			{
				double num = 0;
				num = Find_Closest(0, onpatient_src.size() - 1, ontooth_src[k][m], onpatient_src);
				sum += abs(num - ontooth_src[k][m]);
			}
			if (sum < min_error||min_error == 0)
			{
				min_error = sum;
				min_num = k;
			}
			sum = 0;
		}
		std::vector<double> group;
		group.push_back(i);//当前病人牙齿点序号
		group.push_back(min_num);//预测最可能的对应标准牙齿点集的序号
		group.push_back(min_error);//当前牙距离序列与预测标准点集的差值，即误差
		patient_closest_point.push_back(group);
	}

	//如果有牙齿点没有预测点，说明阈值分割有问题，要进行处理
	int number = 0;
	bool err = false;
	for (int n = 0; n < teeth; n++)
	{
		for (int m = 0; m < patient_closest_point.size(); m++)
		{
			if (patient_closest_point[m][1] == n)
			{
				err = true;
			}
		}
		if (err == true)
		{
			number++;
		}
		else {
			wrong_teeth_num.push_back(n);
		}
		err = false;
	}
	if (number != teeth) {
		this->Processing_special_point(DentureFiducialList_1, DentureFiducialList_2);
		return;
	}

	for (int i = 0; i < teeth; i++)
	{
		std::vector<double> group;
		optional_number.push_back(group);
	}

	for (int i = 0; i < patient; i++)
	{
		for (int j = 0; j < teeth; j++)
		{
			if (patient_closest_point[i][1] == j) {
				optional_number[j].push_back(i);
			}
		}
	}
	std::vector<double> ans;
	for (int i = 0; i < teeth; i++)
	{
		tar.push_back(0);
		ans.push_back(0);
	}

	select_point_2(0, 0, optional_number.size(), optional_number, tar, ans);
	for (int i = 0; i < ans.size(); i++)
	{
		vtkVector4d p0 = { ans[i], patient_x[ans[i]], patient_y[ans[i]], patient_z[ans[i]] };
		list_ans.append(p0);
	}
	this->Point_Sorting(DentureFiducialList_1, DentureFiducialList_2);//排序，和牙齿标准序列一一对应
}

void yunsuan::select_point_2(int pos, int process, int total, 
	std::vector<std::vector<double>> aa, std::vector<double>& tar, std::vector<double>& ans)
{
	if (process > total)
		return;
	if (process != total && pos >= aa[process].size())
		return;
	if (process == total)//该次组合完毕
	{
		for (int i = 0; i < process; i++)
		{
			this->patient_point.push_back(tar[i]);
		}
		sort(patient_point.begin(), patient_point.end());
		this->onPatientgroup_dis();
		if (on_target == true)
		{
			for (int i = 0; i < process; i++)
			{
				ans[i] = patient_point[i];
			}
			cout << "ti:" << duration2 << endl;
			on_target = false;
		}
		patient_point.clear();
		return;
	}
	else
	{
		select_point_2(pos + 1, process, total, aa, tar, ans);
		if (process >= total)
		{
			select_point_2(pos, process + 1, total, aa, tar, ans);
		}
		else
		{
			tar[process] = aa[process][pos];
			select_point_2(0, process + 1, total, aa, tar, ans);
		}
	}
}

double yunsuan::Find_Closest(int mn, int mx, double a, std::vector<double> p)
{
	if (a >= p[mx])
		return p[mx];
	else if (a <= p[mn])
		return p[mn];

	if (mx - mn == 1)
	{
		int t1 = a - p[mn], t2 = p[mx] - a;
		if (t1 > t2)
			return p[mx];
		else        //(t1<=t2)
			return p[mn];
	}
	else
	{
		int mid = (mn + mx) / 2;
		if (a > p[mid])
			return Find_Closest(mid, mx, a, p);
		else if (a < p[mid])
			return Find_Closest(mn, mid, a, p);
		else
			return p[mid];
	}
}

void yunsuan::Point_Sorting(QList<vtkVector3d>& DentureFiducialList_1, QList<vtkVector3d>& DentureFiducialList_2)
{
	for (int i = 0; i < list_ans.size(); i++)
	{
		for (int j = 0; j < list_ans.size(); j++)
		{
			if (patient_closest_point[list_ans[j][0]][1] == i)
			{
				vtkVector3d ans0 = { list_ans[j][1], list_ans[j][2], list_ans[j][3] };
				DentureFiducialList_1.append(ans0);
			}
		}
	}

	for (double i = 0,k = 0; i < tooth_x.size()+ wrong_teeth_num.size(); i++,k++)
	{
		for (int j = 0; j < wrong_teeth_num.size(); j++)
		{
			if (wrong_teeth_num[j] == i)
			{
				i++;
			}
		}
		vtkVector3d ans1 = { tooth_x[k], tooth_y[k], tooth_z[k] };
		DentureFiducialList_2.append(ans1);
	}
}

void yunsuan::Processing_special_point(QList<vtkVector3d>& DentureFiducialList_1, QList<vtkVector3d>& DentureFiducialList_2)
{
	for (int i = 0; i < wrong_teeth_num.size(); i++)
	{
		std::vector<double>::iterator it_x = std::find(tooth_x.begin(), tooth_x.end(), tooth_x[wrong_teeth_num[i]]);
		tooth_x.erase(it_x);
		std::vector<double>::iterator it_y = std::find(tooth_y.begin(), tooth_y.end(), tooth_y[wrong_teeth_num[i]]);
		tooth_y.erase(it_y);
		std::vector<double>::iterator it_z = std::find(tooth_z.begin(), tooth_z.end(), tooth_z[wrong_teeth_num[i]]);
		tooth_z.erase(it_z);
	}
	tooth_dis2.clear();
	tooth_dis.clear();
	ontooth_src.clear();
	patient_closest_point.clear();
	this->onTooth_dis();//计算标准数据集距离
	this->Point_Screening(DentureFiducialList_1, DentureFiducialList_2);//预测相似点法
}