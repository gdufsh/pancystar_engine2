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
#include"pancy_atmosphere.h"
#include"pancy_FFT_ocean.h"
#include<map>
#include <Shlobj.h>  
#include <tchar.h>  
#include <Commctrl.h> 
#pragma comment(lib, "comctl32.lib")  
class scene_root
{
protected:
	XMFLOAT3                  scene_center_pos;
	float                     time_game;
	pancy_geometry_control    *geometry_buffer;     //������洢
	gbuffer_out_message       *gbuffer_texture_data;//Ԥ��������洢
	postRTGR_out_message      *post_buffer_target;  //�������ȾĿ��
	postHDR_out_message       *HDR_buffer_target;   //HDR��ȾĿ��
public:
	scene_root();
	virtual engine_basic::engine_fail_reason create() = 0;
	virtual void display() = 0;
	virtual void display_nopost() = 0;
	virtual void display_environment(XMFLOAT4X4 view_matrix, XMFLOAT4X4 proj_matrix) = 0;
	virtual void update(float delta_time) = 0;
	virtual void release() = 0;
	gbuffer_render_target*  get_gbuffer_renderdata() { return gbuffer_texture_data->get_gbuffer(); };
	postRTGR_render_target *get_postbuffer_data() { return post_buffer_target->get_gbuffer(); };
	HDR_render_target      get_HDRbuffer_data() { return HDR_buffer_target->get_gbuffer(); };
	pancy_geometry_control   *get_geometry_buffer() { return geometry_buffer; };
	XMFLOAT3               get_scene_center() { return scene_center_pos; };
protected:
	engine_basic::engine_fail_reason create_basic();
	engine_basic::engine_fail_reason release_basic();
};


class scene_test_square : public scene_root
{
	float all_time_need = 0;
	pancy_model_ID ID_model_castel;
	int model_ID_castel;
	pancy_model_ID ID_model_floor;
	int model_ID_floor;
	pancy_model_ID ID_model_ball[30];
	int model_ID_ball;
	pancy_model_ID ID_model_sky;
	int model_ID_sky;
	pancy_model_ID ID_model_pbrtest;
	int model_ID_pbrtest;
	ID3D11ShaderResourceView *tex_cubesky;
	OceanSimulator *simulate_ocean;
	FFT_ocean      *render_ocean;
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
	void show_sky_single();
	void show_sky_single(XMFLOAT4X4 view_matrix, XMFLOAT4X4 *proj_matrix);
	void show_pbr_test(string tech_name, XMFLOAT4X4 *view_matrix = NULL, XMFLOAT4X4 *proj_matrix = NULL);
};
class scene_test_environment : public scene_root
{
	gbuffer_out_message       *environment_texture_data;
	pancy_model_ID ID_model_skin;
	pancy_model_ID ID_model_skin2;
	int model_ID_skin;
	int animation_id;
	model_reader_pancymesh   *test_model;
	Geometry_basic           *fullscreen_buffer;
	XMFLOAT3                 up_cube_reflect[6];
	XMFLOAT3                 look_cube_reflect[6];
	float quality_reflect;
	pancy_model_ID ID_model_sky;
	int model_ID_sky;
	ID3D11ShaderResourceView *tex_cubesky;
	ID3D11ShaderResourceView *SRV_cube;
	ID3D11RenderTargetView *RTV_cube[7*6];

	ID3D11ShaderResourceView *SRV_diffusecube;
	ID3D11RenderTargetView *RTV_diffusecube[6];

	ID3D11ShaderResourceView *SRV_singlecube;
	ID3D11RenderTargetView *RTV_singlecube[6];

	ID3D11DepthStencilView   *reflect_cube_DSV;

public:
	scene_test_environment();
	engine_basic::engine_fail_reason create();
	void display();
	void display_nopost() {};
	void display_environment(XMFLOAT4X4 view_matrix, XMFLOAT4X4 proj_matrix);
	void update(float delta_time);
	void release();
private:
	void show_sky_single();
	void show_sky_cube();
	void show_animation_test();
	engine_basic::engine_fail_reason create_cubemap();
};

class real_time_environment 
{
	D3D11_VIEWPORT           environment_VP;
	Geometry_basic           *fullscreen_buffer;          //ȫ��Ļƽ��
	engine_basic::extra_perspective_message *scene_perspective;
	//��Ⱦ����
	XMFLOAT3                 up_cube_reflect[6];
	XMFLOAT3                 look_cube_reflect[6];
	XMFLOAT3                 center_position;
	XMFLOAT3                 front_scene_center;
	int now_render_face;     //��ǰ��Ⱦ��
	bool gbuffer_render_turn;//�����Ƿ�Ϊgbuffer��Ⱦ����
	float quality_environment;
	//gbuffer��������
	gbuffer_out_message       *gbuffer_texture_data;
	//���&�����¼����
	ID3D11ShaderResourceView *cube_depthstencil_SRV;
	ID3D11RenderTargetView   *cube_depthstencil_RTV[6];

	ID3D11ShaderResourceView *cube_depthstencil_backSRV;
	ID3D11RenderTargetView   *cube_depthstencil_backRTV[6];
	//��ɫ��¼����
	ID3D11ShaderResourceView *cube_rendercolor_SRV;
	ID3D11RenderTargetView   *cube_rendercolor_RTV[6];

	ID3D11ShaderResourceView *cube_rendercolor_backSRV;
	ID3D11RenderTargetView   *cube_rendercolor_backRTV[6];
	ID3D11DepthStencilView   *reflect_cube_DSV;
public:
	//������������(0-1)
	real_time_environment(float quality_environment_in);
	engine_basic::engine_fail_reason create();
	void display_a_turn(scene_root *environment_scene);
	void release();
	ID3D11ShaderResourceView * get_env_depth_texture() { return cube_depthstencil_SRV; };
	ID3D11ShaderResourceView * get_env_color_texture() { return cube_rendercolor_SRV; };
	XMFLOAT3 get_scene_center() { return front_scene_center; };
private:
	engine_basic::engine_fail_reason init_texture();
	engine_basic::engine_fail_reason init_cube_texture(DXGI_FORMAT tex_format, ID3D11ShaderResourceView **SRV_in, ID3D11RenderTargetView *RTV_in[6], string texture_name);
	void display_backbuffer(scene_root *environment_scene);
	void display_environment(scene_root *environment_scene);
	void get_ViewMatrix(XMFLOAT4X4 *view_matrix, XMFLOAT4X4 *invview_matrix);
};


class pancy_scene_control
{
	int scene_now_show;
	XMFLOAT3 sundir;
	ID3D11ShaderResourceView *brdf_pic;
	ID3D11RenderTargetView *brdf_target;
	//Pretreatment_gbuffer *pretreat_render;
	ssao_pancy *ssao_render;
	//render_posttreatment_RTGR *globel_reflect;
	//render_posttreatment_HDR  *HDR_tonemapping;
	std::vector<scene_root*> scene_list;
	atmosphere_pretreatment *atmosphere_texture;
	real_time_environment *environment_map_list;




	mesh_square *picture_buf;
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

	//Pretreatment_gbuffer* get_pretreat() { return pretreat_render; };
	//render_posttreatment_HDR* get_post_hdr(){ return HDR_tonemapping; };
	void release();
private:
	void render_environment();
	void render_brdf_texture();
	engine_basic::engine_fail_reason build_brdf_texture();
};