#pragma once
#include"geometry.h"
#include"pancystar_engine_basic.h"
#include"pancy_d3d11_basic.h"
#include"shader_pancy.h"
#include"PancyCamera.h"
#include"PancyInput.h"
#include"pancy_model_control.h"
#include"pancy_pretreatment.h"
#include"pancy_ssao.h"
#include"pancy_lighting.h"
#include"pancy_posttreatment.h"
#include<map>
#include <Shlobj.h>  
#include <tchar.h>  
#include <Commctrl.h> 
#pragma comment(lib, "comctl32.lib")  
class scene_root
{
protected:
	float                     time_game;
public:
	scene_root();
	virtual engine_basic::engine_fail_reason create() = 0;
	virtual void display() = 0;
	virtual void display_nopost() = 0;
	virtual void display_environment(XMFLOAT4X4 view_matrix, XMFLOAT4X4 proj_matrix) = 0;
	virtual void update(float delta_time) = 0;
	virtual void release() = 0;
protected:
};
class scene_test_square : public scene_root
{
	pancy_model_ID ID_model_castel;
	int model_ID_castel;
	pancy_model_ID ID_model_floor;
	int model_ID_floor;
	pancy_model_ID ID_model_ball[30];
	int model_ID_ball;
	pancy_model_ID ID_model_sky;
	int model_ID_sky;
	ID3D11ShaderResourceView *tex_cubesky;
public:
	scene_test_square();
	engine_basic::engine_fail_reason create();
	void display();
	void display_nopost() {};
	void display_environment(XMFLOAT4X4 view_matrix, XMFLOAT4X4 proj_matrix);
	void update(float delta_time);
	void release();
private:
	void show_model_single(string tech_name, XMFLOAT4X4 *view_matrix = NULL, XMFLOAT4X4 *proj_matrix = NULL);
	void show_floor_single();
	void show_ball_single();
	void show_sky_single();
};

class pancy_scene_control
{
	int scene_now_show;
	Pretreatment_gbuffer *pretreat_render;
	ssao_pancy *ssao_render;
	render_posttreatment_RTGR *globel_reflect;
	std::vector<scene_root*> scene_list;
	float quality_reflect;
	float delta_time_now;
	float update_time;

	ID3D11ShaderResourceView *reflect_cube_SRV;           //�洢��̬cubemapping��������Դ
	ID3D11RenderTargetView   *reflect_cube_RTV[6];        //�洢��̬cubemapping����ȾĿ��
	ID3D11DepthStencilView   *reflect_cube_DSV;

	ID3D11RenderTargetView   *reflect_cube_RTV_backbuffer[6];        //�洢��̬cubemapping����ȾĿ��
	ID3D11ShaderResourceView   *reflect_cube_SRV_backbuffer;

	

	XMFLOAT3                 up_cube_reflect[6];
	XMFLOAT3                 look_cube_reflect[6];
	XMFLOAT3                 center_position;
public:
	pancy_scene_control();
	void update(float delta_time);
	void display();
	engine_basic::engine_fail_reason init_cube_map();
	engine_basic::engine_fail_reason create();
	engine_basic::engine_fail_reason add_a_scene(scene_root* scene_in);
	engine_basic::engine_fail_reason change_now_scene(int scene_ID);
	void release();
private:
	void render_environment();
};