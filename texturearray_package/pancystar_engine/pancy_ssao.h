#pragma once
#include"pancy_d3d11_basic.h"
#include"geometry.h"
#include"pancy_model_control.h"
#include"shader_pancy.h"
class ssao_pancy : public engine_basic::window_size_observer
{
	int                      map_width;
	int                      map_height;
	Geometry_basic           *fullscreen_buffer;          //ȫ��Ļƽ��
	ID3D11ShaderResourceView *randomtex;           //���������Դ

	ID3D11ShaderResourceView *normaldepth_tex;     //�洢���ߺ���ȵ�������Դ
	ID3D11ShaderResourceView *depth_tex;     //�洢���ߺ���ȵ�������Դ

	ID3D11RenderTargetView   *ambient_target;     //�洢ssao����ȾĿ��
	ID3D11ShaderResourceView *ambient_tex;        //�洢ssao��������Դ

	ID3D11RenderTargetView   *ambient_target_blur1;     //�洢���ڽ�����ssao����ȾĿ��
	ID3D11ShaderResourceView *ambient_tex_blur1;        //�洢���ڽ�����ssao��������Դ
	ID3D11RenderTargetView   *ambient_target_blur2;     //�洢���ڽ�����ssao����ȾĿ��
	ID3D11ShaderResourceView *ambient_tex_blur2;        //�洢���ڽ�����ssao��������Դ

	XMFLOAT4                 random_Offsets[14];   //ʮ�ĸ����ڼ���ao�����������
public:
	ssao_pancy(int width, int height);
	void update_windowsize(int wind_width_need, int wind_height_need);
	engine_basic::engine_fail_reason basic_create();
	void compute_ssaomap();
	void get_normaldepthmap(ID3D11ShaderResourceView *normalspec_need, ID3D11ShaderResourceView *depth_need);
	ID3D11ShaderResourceView* get_aomap();
	void blur_ssaomap();
	void check_ssaomap();
	void release();
private:
	engine_basic::engine_fail_reason build_texture();
	void build_offset_vector();
	engine_basic::engine_fail_reason build_randomtex();
	void basic_blur(ID3D11ShaderResourceView *texin, ID3D11RenderTargetView *texout, bool if_row);
	void release_texture();
	template<class T>
	void safe_release(T t)
	{
		if (t != NULL)
		{
			t->Release();
			t = 0;
		}
	}
};