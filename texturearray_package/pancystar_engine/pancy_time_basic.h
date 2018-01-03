#pragma once
#include<windows.h>
#include<iostream>
class time_count
{
	double   count_freq;      //ϵͳʱ��Ƶ��
	double   delta_time;      //��֮֡��ʱ���
	double   all_timeneed;    //������ʱ��
	double   all_pause_time;  //��ͣ����ʱ��
	__int64  start_time;      //��ʼʱ��
	__int64  stop_time;       //ֹͣʱ��
	__int64  now_time;        //��ǰʱ��
	__int64  last_time;       //��һ֡��ʱ��
	bool     if_stop;         //�Ƿ���ͣ
private:
	time_count();          //���캯��
public:
	static time_count* get_instance()
	{
		static time_count* this_instance;
		if (this_instance == NULL)
		{
			this_instance = new time_count();
		}
		return this_instance;
	}
	void reset();          //ʱ������
	void start();          //��ʼ��ʱ
	void stop();           //��ͣ��ʱ
	void refresh();        //ˢ�¼�ʱ��
	float get_pause();     //��ȡ�ܵ���ͣʱ��
	float get_delta();     //��ȡ֡��ʱ���
	float get_alltime();   //��ȡ��ʱ��
};