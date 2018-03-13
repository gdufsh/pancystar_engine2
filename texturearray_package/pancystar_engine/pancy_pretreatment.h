#include"pancy_d3d11_basic.h"
#include"geometry.h"
#include"pancy_model_control.h"
#include"shader_pancy.h"
#include"pancy_lighting.h"
#pragma once
struct gbuffer_render_target
{
	bool IF_MSAA;
	D3D11_VIEWPORT           render_viewport;             //�ӿ���Ϣ
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~gbuffer�׶�����~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	ID3D11ShaderResourceView *depthmap_tex;               //���������Ϣ��������Դ
	ID3D11DepthStencilView   *depthmap_target;            //������ȾĿ��Ļ�������Դ

	ID3D11RenderTargetView   *normalspec_target;          //�洢���ߺ;��淴��ϵ������ȾĿ��
	ID3D11ShaderResourceView *normalspec_tex;             //�洢���ߺ;��淴��ϵ����������Դ

	ID3D11RenderTargetView   *AtmosphereMask_target;      //�洢���������������ȾĿ��
	ID3D11ShaderResourceView *AtmosphereMask_tex;         //�洢�������������������Դ

	ID3D11RenderTargetView   *specroughness_target;       //�洢���ߺ;��淴��ϵ������ȾĿ��
	ID3D11ShaderResourceView *specroughness_tex;          //�洢���ߺ;��淴��ϵ����������Դ
	//MSAA�������
	ID3D11RenderTargetView   *depthmap_single_target;     //�洢���msaa��������Ϣ����ȾĿ��
	ID3D11ShaderResourceView *depthmap_single_tex;        //�洢���msaa��������Ϣ��������Դ
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~lbuffer�׶�����~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	ID3D11RenderTargetView   *gbuffer_diffuse_target;     //�洢���������Ч������ȾĿ��
	ID3D11ShaderResourceView   *gbuffer_diffuse_tex;      //�洢���������Ч������Ⱦ����

	ID3D11RenderTargetView   *gbuffer_specular_target;    //�洢�����淴�����Ч������ȾĿ��
	ID3D11ShaderResourceView   *gbuffer_specular_tex;     //�洢�����淴�����Ч������Ⱦ����

	ID3D11RenderTargetView   *gbuffer_atmosphere_target;  //�洢��������Ч������ȾĿ��
	ID3D11ShaderResourceView   *gbuffer_atmosphere_tex;   //�洢��������Ч������Ⱦ����
};
class gbuffer_out_message 
{
	gbuffer_render_target buffer_data;
public:
	gbuffer_out_message(int width_in, int height_in, bool if_MSAA);
	gbuffer_render_target *get_gbuffer() { return &buffer_data; };
	engine_basic::engine_fail_reason create();
	void release();
private:
	engine_basic::engine_fail_reason init_texture();
	engine_basic::engine_fail_reason init_texture_same_resource(DXGI_FORMAT tex_format, ID3D11ShaderResourceView **SRV_in, ID3D11RenderTargetView **RTV_in, string texture_name);
	engine_basic::engine_fail_reason init_texture_diffrent_resource(DXGI_FORMAT tex_format, ID3D11ShaderResourceView **SRV_in, ID3D11RenderTargetView **RTV_in, string texture_name);

};

class Pretreatment_gbuffer
{
	Geometry_basic           *fullscreen_buffer;          //ȫ��Ļƽ��
	Geometry_basic           *fullscreen_Lbuffer;         //ȫ��Ļ����ƽ��								 
private:
	Pretreatment_gbuffer();
public:
	static Pretreatment_gbuffer* get_instance()
	{
		static Pretreatment_gbuffer* this_instance;
		if (this_instance == NULL)
		{
			this_instance = new Pretreatment_gbuffer();
		}
		return this_instance;
	}
	engine_basic::engine_fail_reason create();
	void render_gbuffer(
		pancy_geometry_control *geometry_list, 
		gbuffer_render_target *render_target_out, 
		XMFLOAT4X4 view_matrix, 
		engine_basic::extra_perspective_message *perspective_message, 
		bool if_static,
		bool if_post
		);
	void render_lbuffer(
		gbuffer_render_target *render_target_out,
		XMFLOAT3 view_position, 
		XMFLOAT4X4 view_matrix, 
		XMFLOAT4X4 invview_matrix, 
		engine_basic::extra_perspective_message *perspective_message, 
		bool if_shadow
		);
	void release();
private:
	void set_normalspecdepth_target(gbuffer_render_target render_target_out,bool if_clear);
	void set_multirender_target(gbuffer_render_target render_target_out);
	void set_resolvdepth_target(gbuffer_render_target render_target_out);
	void resolve_depth_render(ID3DX11EffectTechnique* tech);
	void light_buffer_render(ID3DX11EffectTechnique* tech);


	
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
