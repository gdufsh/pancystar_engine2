#include"pancy_time_basic.h"
time_count::time_count()
{
	__int64 rec_frequency;
	if(QueryPerformanceFrequency((LARGE_INTEGER*)&rec_frequency))//��ȡϵͳ��ʱ��Ƶ��
	{
		count_freq = 1.0/(double)rec_frequency;
	}
	if_stop        = true;
	start_time     = 0;
	all_pause_time = 0;
	stop_time      = 0;
	now_time       = 0;
	last_time      = 0;
	delta_time     = 0;
}
void time_count::reset()
{
	QueryPerformanceCounter((LARGE_INTEGER*)&start_time);//���ó�ʼʱ��
	if_stop = false;
	now_time = start_time;
	last_time = start_time;
}
void time_count::start()//��ʼ��ʱ��ȡ����ͣ״̬������ͣʱ�����
{
	if(if_stop)
	{
		if_stop = false;
		all_pause_time = 0;
	}
}
void time_count::stop()//��ͣ��ʱ��ˢ�¿�ʼ��ͣ��ʱ�̲���ʼ������ͣʱ��
{
	if(!if_stop)
	{
		refresh();
		stop_time = now_time;
		delta_time = 0;
		if_stop = true;
	}
}
void time_count::refresh()
{
	if(!if_stop)//ʱ��δ����ͣ������������ʱ
	{
		last_time = now_time;
		QueryPerformanceCounter((LARGE_INTEGER*)(&now_time));	
		delta_time = static_cast<double>(now_time - last_time) * count_freq;
	}
	else//ʱ�䱻��ͣ�������ܵ���ͣʱ��
	{
		QueryPerformanceCounter((LARGE_INTEGER*)(&now_time));
		all_pause_time = static_cast<double>(now_time - stop_time) * count_freq;
	}
	all_timeneed = static_cast<double>(now_time - start_time) * count_freq;
}
float time_count::get_pause()
{
	return static_cast<float>(all_pause_time);
}
float time_count::get_delta()
{
	return static_cast<float>(delta_time);
}
float time_count::get_alltime()
{
	return static_cast<float>(all_timeneed);
}