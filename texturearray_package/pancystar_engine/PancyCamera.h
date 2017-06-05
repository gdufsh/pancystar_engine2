#pragma once
#include<D3D11.h>
#include<assert.h>
#include<d3dx11effect.h>
#include<directxmath.h>
#include<string.h>
#include<stdlib.h>
#include<Dinput.h>
#include<iostream>
using namespace DirectX;

#define CameraForFPS 0
#define CameraForFly 1
class pancy_camera
{
	//ID3D11Device *device_d3d; //ȷ�����任��d3d�豸�ӿ�
	XMFLOAT3 camera_right;    //����������ҷ�������
	XMFLOAT3 camera_look;     //������Ĺ۲췽������
	XMFLOAT3 camera_up;       //����������Ϸ�������
	XMFLOAT3 camera_position; //�����������λ������
private:
	pancy_camera();
public:
	static pancy_camera* get_instance()
	{
		static pancy_camera* this_instance;
		if (this_instance == NULL)
		{
			this_instance = new pancy_camera();
		}
		return this_instance;
	}
	void rotation_right(float angle);                    //��������������ת
	void rotation_up(float angle);                       //��������������ת
	void rotation_look(float angle);                     //���Ź۲�������ת

	void rotation_x(float angle);                    //����x��������ת
	void rotation_y(float angle);                    //����y��������ת
	void rotation_z(float angle);                    //����z��������ת

	void walk_front(float distance);                     //�������ǰƽ��
	void walk_right(float distance);                     //���������ƽ��
	void walk_up(float distance);                        //���������ƽ��
	void count_view_matrix(XMFLOAT4X4* view_matrix);     //����ȡ������
	void count_view_matrix(XMFLOAT3 rec_look, XMFLOAT3 rec_up, XMFLOAT3 rec_pos, XMFLOAT4X4 *matrix);
	void count_invview_matrix(XMFLOAT4X4* inv_view_matrix);  //����ȡ�����������
	void count_invview_matrix(XMFLOAT3 rec_look, XMFLOAT3 rec_up, XMFLOAT3 rec_pos, XMFLOAT4X4* inv_view_matrix);  //����ȡ�����������
	
	void get_view_position(XMFLOAT3 *view_pos);
	void get_view_direct(XMFLOAT3 *view_direct);
	void get_right_direct(XMFLOAT3 *right_direct);
	void set_camera(XMFLOAT3 rec_look, XMFLOAT3 rec_up, XMFLOAT3 rec_pos);
	void reset_camera();
private:
};