#pragma once
#include"pancy_model_control.h"
#include"shader_pancy.h"
#include"pancy_pretreatment.h"
struct postRTGR_render_target
{
	D3D11_VIEWPORT           render_viewport;             //�ӿ���Ϣ
	D3D11_VIEWPORT           half_viewport;               //��ֱ����ӿ���Ϣ
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~����׶�����~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	ID3D11RenderTargetView   *rtgr_input_target;          //ȫ�ַ���������ȾĿ��
	ID3D11ShaderResourceView *rtgr_input_tex;             //ȫ�ַ���������Ⱦ����
	ID3D11RenderTargetView   *rtgr_InputMask_target;      //ȫ�ַ���������ȾĿ��
	ID3D11ShaderResourceView *rtgr_InputMask_tex;         //ȫ�ַ���������Ⱦ����

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~���������м���Ϣ�洢~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	ID3D11RenderTargetView   *reflect_target;             //�洢��̬��Ļ�ռ䷴�����ȾĿ��
	ID3D11ShaderResourceView *reflect_tex;                //�洢��̬��Ļ�ռ䷴���������Դ

	ID3D11RenderTargetView   *mask_target;                //�洢��̬��Ļ�ռ䷴���������ȾĿ��
	ID3D11ShaderResourceView *mask_tex;                   //�洢��̬��Ļ�ռ䷴�������������Դ

	ID3D11RenderTargetView   *final_reflect_target;       //�洢ȫ�ַ������������ȾĿ��
	ID3D11ShaderResourceView *final_reflect_tex;          //�洢ȫ�ַ����������������Դ

	ID3D11RenderTargetView   *blur_reflect1_target;       //�洢ģ���м�������ȾĿ��
	ID3D11ShaderResourceView *blur_reflect1_tex;          //�洢ģ���м�����������Դ

	ID3D11RenderTargetView   *blur_reflect2_target;       //�洢ģ���м�������ȾĿ��
	ID3D11ShaderResourceView *blur_reflect2_tex;          //�洢ģ���м�����������Դ

	ID3D11RenderTargetView   *reflect_out_target;         //�洢���պϳɽ������ȾĿ��
	ID3D11ShaderResourceView *reflect_out_tex;            //�洢���պϳɽ����������Դ
};
class postRTGR_out_message
{
	postRTGR_render_target buffer_data;
public:
	postRTGR_out_message(int width_in, int height_in);
	postRTGR_render_target* get_gbuffer() { return &buffer_data; };
	engine_basic::engine_fail_reason create();
	void release();
private:
	engine_basic::engine_fail_reason init_texture();
	engine_basic::engine_fail_reason init_texture_same_resource(DXGI_FORMAT tex_format, ID3D11ShaderResourceView **SRV_in, ID3D11RenderTargetView **RTV_in, string texture_name);
	engine_basic::engine_fail_reason init_texture_diffrent_resource(DXGI_FORMAT tex_format, ID3D11ShaderResourceView **SRV_in, ID3D11RenderTargetView **RTV_in, string texture_name);
};




class render_posttreatment_RTGR
{
	Geometry_basic           *fullscreen_buffer;         //ȫ��Ļƽ��
	XMFLOAT4X4               static_cube_view_matrix[6];  //������ͼ�����������ȡ���任
	float  width_static_cube;
	/*
	//postRTGR_render_target   rtgr_texture_input;         //����������Ϣ
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~������������~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	ID3D11ShaderResourceView *color_tex;                  //�洢��Ⱦ�����������Դ
	ID3D11ShaderResourceView *input_mask_tex;             //�洢��Ⱦ�����������Դ
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~���������м���Ϣ�洢~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	ID3D11RenderTargetView   *reflect_target;             //�洢��̬��Ļ�ռ䷴�����ȾĿ��
	ID3D11ShaderResourceView *reflect_tex;                //�洢��̬��Ļ�ռ䷴���������Դ

	ID3D11RenderTargetView   *mask_target;                //�洢��̬��Ļ�ռ䷴���������ȾĿ��
	ID3D11ShaderResourceView *mask_tex;                   //�洢��̬��Ļ�ռ䷴�������������Դ

	ID3D11RenderTargetView   *blur_reflect_target;        //�洢ģ���м�������ȾĿ��
	ID3D11ShaderResourceView *blur_reflect_tex;           //�洢ģ���м�����������Դ

	ID3D11RenderTargetView   *blur_reflect_target2[5];    //�洢����ģ���������ȾĿ��
	ID3D11ShaderResourceView *blur_reflect_texarray;      //�洢����ģ�����������������Դ

	//ID3D11RenderTargetView   *blur_reflect_target2;        //�洢��̬��Ļ�ռ䷴�����ȾĿ��
	//ID3D11ShaderResourceView *blur_reflect_tex2;           //�洢��̬��Ļ�ռ䷴���������Դ

	ID3D11RenderTargetView   *final_reflect_target;       //�洢��̬��Ļ�ռ䷴�����ȾĿ��
	ID3D11ShaderResourceView *final_reflect_tex;          //�洢��̬��Ļ�ռ䷴���������Դ
	//ID3D11RenderTargetView   *final_reflectmip_target[5]; //�洢����GI��mipmap��ȾĿ��

	ID3D11RenderTargetView   *reflectpass_outRTV;            //�洢����pass���������ȾĿ��
	ID3D11ShaderResourceView *reflectpass_outSRV;            //�洢����pass�������������Դ


	D3D11_VIEWPORT           render_viewport;             //�ӿ���Ϣ
	D3D11_VIEWPORT           half_render_viewport;        //�ӿ���Ϣ
	*/
private:
	render_posttreatment_RTGR();
public:
	static render_posttreatment_RTGR* get_instance()
	{
		static render_posttreatment_RTGR* this_instance;
		if (this_instance == NULL)
		{
			this_instance = new render_posttreatment_RTGR();
		}
		return this_instance;
	}
	engine_basic::engine_fail_reason create();
	void draw_reflect(
		gbuffer_render_target *gbuffer_texture_in,
		postRTGR_render_target *render_texture_in,
		XMFLOAT3 center_position,
		ID3D11ShaderResourceView *reflect_cube_SRV,
		ID3D11ShaderResourceView *reflect_cubestencil_SRV
		);
	
	void release();
	//void update_windowsize(int wind_width_need, int wind_height_need);
	//ID3D11ShaderResourceView* get_output_tex() { return reflectpass_outSRV; };
	//void draw_reflect(XMFLOAT3 center_position, ID3D11RenderTargetView *rendertarget_input, ID3D11RenderTargetView *mask_target_input, ID3D11ShaderResourceView *normaldepth_tex, ID3D11ShaderResourceView *depth_tex, ID3D11ShaderResourceView *reflect_cube_SRV, ID3D11ShaderResourceView *reflect_cubestencil_SRV);
	//void draw_to_posttarget(ID3D11ShaderResourceView *ao_in, ID3D11ShaderResourceView *metallic_in, ID3D11ShaderResourceView *specrough_in, ID3D11ShaderResourceView *brdf_in);

private:
	void build_reflect_map(
		gbuffer_render_target *gbuffer_texture_in,
		postRTGR_render_target *rtgr_texture_in,
		XMFLOAT3 center_position,
		ID3D11ShaderResourceView *reflect_cube_SRV,
		ID3D11ShaderResourceView *reflect_cubestencil_SRV
		);
	void blur_map(gbuffer_render_target *gbuffer_texture_in, postRTGR_render_target *render_texture_in);
	void basic_blur(
		gbuffer_render_target *gbuffer_texture_in,
		postRTGR_render_target *render_texture_in,
		ID3D11ShaderResourceView *input,
		ID3D11RenderTargetView *output,
		bool if_horz
		);
	void draw_to_posttarget(postRTGR_render_target *render_texture_in);
	//void basic_blur(ID3D11ShaderResourceView *normaldepth_tex, ID3D11ShaderResourceView *depth_tex, ID3D11ShaderResourceView *mask, ID3D11ShaderResourceView *input, ID3D11RenderTargetView *output, bool if_horz);
	//engine_basic::engine_fail_reason build_texture();	
	//void build_reflect_map(XMFLOAT3 center_position, ID3D11RenderTargetView *rendertarget_input, ID3D11RenderTargetView *mask_target_input, ID3D11ShaderResourceView *normaldepth_tex, ID3D11ShaderResourceView *depth_tex, ID3D11ShaderResourceView *reflect_cube_SRV, ID3D11ShaderResourceView *reflect_cubestencil_SRV);
	//void release_texture();
	//void blur_map(ID3D11ShaderResourceView *normaldepth_tex, ID3D11ShaderResourceView *depth_tex);
	//void basic_blur_mipmap(ID3D11ShaderResourceView *normaldepth_tex, ID3D11ShaderResourceView *depth_tex, ID3D11ShaderResourceView *mask, ID3D11ShaderResourceView *input, ID3D11RenderTargetView *output, bool if_horz, int mip_level);
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



struct HDR_render_target
{
	D3D11_VIEWPORT           render_viewport;             //�ӿ���Ϣ
	//D3D11_VIEWPORT           half_viewport;               //��ֱ����ӿ���Ϣ
	//��������
	//ID3D11RenderTargetView   *HDR_input_target;     //ȫ�ַ���������ȾĿ��
	//ID3D11ShaderResourceView *HDR_input_tex;        //ȫ�ַ���������Ⱦ����
	//�м�����
	ID3D11UnorderedAccessView   *UAV_HDR_mid;        //HDR�Ļ������������м����
	ID3D11UnorderedAccessView   *UAV_HDR_final;      //HDR�Ļ����������ڴ洢���
	ID3D11ShaderResourceView    *SRV_HDR_map;        //HDR�Ļ����������ڴ洢map���

	ID3D11ShaderResourceView    *SRV_HDR_save;       //HDR�߹�洢������Ⱦ��Դ�����߹���д洢��
	ID3D11RenderTargetView      *RTV_HDR_save;       //HDR�߹�洢������ȾĿ�꣬���߹���д洢��

	ID3D11ShaderResourceView    *SRV_HDR_blur1;       //HDR�߹�ģ����Ⱦ��Դ��
	ID3D11RenderTargetView      *RTV_HDR_blur1;       //HDR�߹�ģ����ȾĿ�ꡣ
	ID3D11ShaderResourceView    *SRV_HDR_blur2;       //HDR�߹�ģ����Ⱦ��Դ��
	ID3D11RenderTargetView      *RTV_HDR_blur2;       //HDR�߹�ģ����ȾĿ�ꡣ
};
class postHDR_out_message
{
	HDR_render_target buffer_data;
public:
	postHDR_out_message(int width_in, int height_in);
	HDR_render_target get_gbuffer() { return buffer_data; };
	engine_basic::engine_fail_reason create();
	void release();
private:
	engine_basic::engine_fail_reason init_texture();
	engine_basic::engine_fail_reason init_buffer();
	engine_basic::engine_fail_reason init_texture_same_resource(DXGI_FORMAT tex_format, ID3D11ShaderResourceView **SRV_in, ID3D11RenderTargetView **RTV_in, string texture_name);
};


class render_posttreatment_HDR
{
	//ȫ���ı���
	mesh_square                 *HDR_fullscreen;

	/*
	//postHDR_out_message         *texture_render;
	int                         width, height;       //��Ļ���
	//�ڲ�����
	ID3D11UnorderedAccessView   *UAV_HDR_mid;        //HDR�Ļ������������м����
	ID3D11UnorderedAccessView   *UAV_HDR_final;      //HDR�Ļ����������ڴ洢���
	ID3D11ShaderResourceView    *SRV_HDR_map;        //HDR�Ļ����������ڴ洢map���
	ID3D11ShaderResourceView    *SRV_HDR_use;        //HDR���벿�֣�Ҫ����Ļ����ת���ɷǿ���ݵ�����

	ID3D11ShaderResourceView    *SRV_HDR_save;       //HDR�߹�洢������Ⱦ��Դ�����߹���д洢��
	ID3D11RenderTargetView      *RTV_HDR_save;       //HDR�߹�洢������ȾĿ�꣬���߹���д洢��

	ID3D11ShaderResourceView    *SRV_HDR_blur1;       //HDR�߹�ģ����Ⱦ��Դ��
	ID3D11RenderTargetView      *RTV_HDR_blur1;       //HDR�߹�ģ����ȾĿ�ꡣ
	ID3D11ShaderResourceView    *SRV_HDR_blur2;       //HDR�߹�ģ����Ⱦ��Դ��
	ID3D11RenderTargetView      *RTV_HDR_blur2;       //HDR�߹�ģ����ȾĿ�ꡣ

	D3D11_VIEWPORT              render_viewport;      //�ӿ���Ϣ

	ID3D11Buffer*               CPU_read_buffer;
	*/
	float                       average_light;
	float                       average_light_last;
	int width_rec, height_rec, buffer_num, map_num;
private:
	render_posttreatment_HDR();
public:
	static render_posttreatment_HDR* get_instance()
	{
		static render_posttreatment_HDR* this_instance;
		if (this_instance == NULL)
		{
			this_instance = new render_posttreatment_HDR();
		}
		return this_instance;
	}
	engine_basic::engine_fail_reason create();
	void release();
	engine_basic::engine_fail_reason display(ID3D11ShaderResourceView *input, HDR_render_target render_target_need, ID3D11RenderTargetView *output);

	//render_posttreatment_HDR(int width_need, int height_need);
	//void update_windowsize(int wind_width_need, int wind_height_need);
	//engine_basic::engine_fail_reason display(ID3D11ShaderResourceView *SRV_HDR_use_in);
private:

	engine_basic::engine_fail_reason count_average_light(ID3D11ShaderResourceView *SRV_HDR_use_in, HDR_render_target render_target_need);
	engine_basic::engine_fail_reason build_preblur_map(ID3D11ShaderResourceView *SRV_HDR_use_in, HDR_render_target render_target_need);
	void render_posttreatment_HDR::basic_blur(HDR_render_target render_target_need, ID3D11ShaderResourceView *input, ID3D11RenderTargetView *output, bool if_horz);
	engine_basic::engine_fail_reason blur_map(HDR_render_target render_target_need);
	engine_basic::engine_fail_reason HDR_map(ID3D11ShaderResourceView *input, HDR_render_target render_target_need, ID3D11RenderTargetView *output);
	/*
	//engine_basic::engine_fail_reason init_buffer();
	//engine_basic::engine_fail_reason init_texture();
	//engine_basic::engine_fail_reason CreateCPUaccessBuf(int size_need);
	void basic_blur(ID3D11ShaderResourceView *input, ID3D11RenderTargetView *output, bool if_horz);
	engine_basic::engine_fail_reason count_average_light(ID3D11ShaderResourceView *SRV_HDR_use_in);
	engine_basic::engine_fail_reason build_preblur_map();
	engine_basic::engine_fail_reason blur_map();
	engine_basic::engine_fail_reason HDR_map();
	void release_basic();
	*/
};
