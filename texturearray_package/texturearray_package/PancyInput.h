#pragma once
#include <windows.h>
#include<D3D11.h>
#include<assert.h>
#include<d3dx11effect.h>
#include<directxmath.h>
#include<string.h>
#include<stdlib.h>
#include<Dinput.h>
#include<iostream>
#include"pancystar_engine_basic.h"
#pragma comment(lib, "Dinput8.lib") 
using namespace std;
class pancy_input
{
	LPDIRECTINPUT8             pancy_dinput;                                 //DirectInput���豸�ӿ�    
	LPDIRECTINPUTDEVICE8       dinput_keyboard;                              //�����豸�ӿ�
	LPDIRECTINPUTDEVICE8       dinput_mouse;                                 //����豸�ӿ�
	char                       key_buffer[256];                              //���̰�����Ϣ�Ļ���
	DIMOUSESTATE               mouse_buffer;                                 //��������Ϣ�Ļ���
private:
	pancy_input();         //���캯��
public:
	static pancy_input *pancy_input_pInstance;
	static engine_basic::engine_fail_reason single_create(HWND hwnd, HINSTANCE hinst)
	{
		if (pancy_input_pInstance != NULL)
		{
			engine_basic::engine_fail_reason failed_reason("the d3d input instance have been created before");
			return failed_reason;
		}
		else
		{
			pancy_input_pInstance = new pancy_input();
			engine_basic::engine_fail_reason check_failed = pancy_input_pInstance->init(hwnd, hinst);
			return check_failed;
		}
	}
	static pancy_input * GetInstance()
	{
		return pancy_input_pInstance;
	}
	~pancy_input();                                                          //��������
	void  get_input();                                                       //��ȡ��������
	bool  check_keyboard(int key_value);                                     //�������ϵ�ĳ�����������
	bool  check_mouseDown(int mouse_value);                                  //�������ϵ�ĳ�����������
	float MouseMove_X();                                                     //��ȡ�����x����ƶ���
	float MouseMove_Y();                                                     //��ȡ�����y����ƶ���
	float MouseMove_Z();                                                     //��ȡ�����z����ƶ���
	engine_basic::engine_fail_reason init(HWND hwnd, HINSTANCE hinst);
private:
	engine_basic::engine_fail_reason dinput_clear(HWND hwnd,DWORD keyboardCoopFlags, DWORD mouseCoopFlags);//��ʼ������
};