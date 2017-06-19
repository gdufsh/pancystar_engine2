#pragma once
#include"pancy_model_control.h"
#include"shader_pancy.h"
class render_posttreatment_RTGR : public engine_basic::window_size_observer
{
	Geometry_basic           *fullscreen_buffer;         //全屏幕平面
														 //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~反射计算的输入~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	ID3D11ShaderResourceView *color_tex;                  //存储渲染结果的纹理资源
	ID3D11ShaderResourceView *input_mask_tex;             //存储渲染结果的纹理资源
														  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~反射计算的中间信息存储~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	ID3D11RenderTargetView   *reflect_target;             //存储动态屏幕空间反射的渲染目标
	ID3D11ShaderResourceView *reflect_tex;                //存储动态屏幕空间反射的纹理资源

	ID3D11RenderTargetView   *mask_target;                //存储动态屏幕空间反射掩码的渲染目标
	ID3D11ShaderResourceView *mask_tex;                   //存储动态屏幕空间反射掩码的纹理资源

	ID3D11RenderTargetView   *blur_reflect_target;        //存储动态屏幕空间反射的渲染目标
	ID3D11ShaderResourceView *blur_reflect_tex;           //存储动态屏幕空间反射的纹理资源

	ID3D11RenderTargetView   *blur_reflect_target2;        //存储动态屏幕空间反射的渲染目标
	ID3D11ShaderResourceView *blur_reflect_tex2;           //存储动态屏幕空间反射的纹理资源

	ID3D11RenderTargetView   *final_reflect_target;       //存储动态屏幕空间反射的渲染目标
	ID3D11ShaderResourceView *final_reflect_tex;          //存储动态屏幕空间反射的纹理资源

	D3D11_VIEWPORT           render_viewport;             //视口信息
	D3D11_VIEWPORT           half_render_viewport;        //视口信息

	XMFLOAT4X4               static_cube_view_matrix[6];  //立方贴图的六个方向的取景变换

	float  width_static_cube;
public:
	render_posttreatment_RTGR();
	void update_windowsize(int wind_width_need, int wind_height_need);
	engine_basic::engine_fail_reason create();
	void draw_reflect(XMFLOAT3 center_position, ID3D11RenderTargetView *rendertarget_input, ID3D11RenderTargetView *mask_target_input, ID3D11ShaderResourceView *normaldepth_tex, ID3D11ShaderResourceView *depth_tex, ID3D11ShaderResourceView *reflect_cube_SRV, ID3D11ShaderResourceView *reflect_cubestencil_SRV);
	void release();
private:
	engine_basic::engine_fail_reason build_texture();
	void release_texture();
	void build_reflect_map(XMFLOAT3 center_position, ID3D11RenderTargetView *rendertarget_input, ID3D11RenderTargetView *mask_target_input, ID3D11ShaderResourceView *normaldepth_tex, ID3D11ShaderResourceView *depth_tex, ID3D11ShaderResourceView *reflect_cube_SRV, ID3D11ShaderResourceView *reflect_cubestencil_SRV);
	void blur_map(ID3D11ShaderResourceView *normaldepth_tex, ID3D11ShaderResourceView *depth_tex);
	void basic_blur(ID3D11ShaderResourceView *normaldepth_tex, ID3D11ShaderResourceView *depth_tex, ID3D11ShaderResourceView *mask, ID3D11ShaderResourceView *input, ID3D11RenderTargetView *output, bool if_horz);
	void draw_to_posttarget();
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
/*
class render_posttreatment_HDR
{
	//全屏四边形
	int                         width, height;       //屏幕宽高
													 //内部变量
	ID3D11UnorderedAccessView   *UAV_HDR_mid;        //HDR的缓冲区，用于中间计算
	ID3D11UnorderedAccessView   *UAV_HDR_final;      //HDR的缓冲区，用于存储结果
	ID3D11ShaderResourceView    *SRV_HDR_map;        //HDR的缓冲区，用于存储map结果
	ID3D11ShaderResourceView    *SRV_HDR_use;        //HDR输入部分，要把屏幕像素转换成非抗锯齿的纹理

	ID3D11ShaderResourceView    *SRV_HDR_save;       //HDR高光存储部分渲染资源，将高光进行存储。
	ID3D11RenderTargetView      *RTV_HDR_save;       //HDR高光存储部分渲染目标，将高光进行存储。

	ID3D11ShaderResourceView    *SRV_HDR_blur1;       //HDR高光模糊渲染资源。
	ID3D11RenderTargetView      *RTV_HDR_blur1;       //HDR高光模糊渲染目标。
	ID3D11ShaderResourceView    *SRV_HDR_blur2;       //HDR高光模糊渲染资源。
	ID3D11RenderTargetView      *RTV_HDR_blur2;       //HDR高光模糊渲染目标。

	D3D11_VIEWPORT              render_viewport;      //视口信息
	ID3D11Buffer*               CPU_read_buffer;
	float                       average_light;
	float                       average_light_last;
	int width_rec, height_rec, buffer_num, map_num;
public:
	render_posttreatment_HDR(int width_need, int height_need);
	HRESULT create();
	void release();
	HRESULT display();
private:
	HRESULT init_buffer();
	HRESULT init_texture();
	HRESULT CreateCPUaccessBuf(int size_need);
	HRESULT build_fullscreen_picturebuff();
	void basic_blur(ID3D11ShaderResourceView *input, ID3D11RenderTargetView *output, bool if_horz);
	HRESULT count_average_light();
	HRESULT build_preblur_map();
	HRESULT blur_map();
	HRESULT HDR_map();

};
*/