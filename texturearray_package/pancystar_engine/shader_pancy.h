#pragma once
#include<windows.h>
#include<iostream>
#include<D3D11.h>
#include<assert.h>
#include<d3dx11effect.h>
//#include<d3dx11dbg.h>
#include<directxmath.h>
#include <sstream>
#include <fstream>
#include <vector>
#include<d3dcompiler.h>
#include<unordered_map>
#include<typeinfo>
#include<typeindex>
#include"pancystar_engine_basic.h"
#include"pancy_d3d11_basic.h"
using namespace DirectX;
enum light_type
{
	direction_light = 0,
	point_light = 1,
	spot_light = 2
};
enum shadow_type
{
	shadow_none = 0,
	shadow_map = 1,
	shadow_volume = 2
};
struct pancy_light_basic
{
	XMFLOAT4    ambient;
	XMFLOAT4    diffuse;
	XMFLOAT4    specular;

	XMFLOAT3    dir;
	float       spot;

	XMFLOAT3    position;
	float       theta;

	XMFLOAT3    decay;
	float       range;

	XMUINT4    light_type;
};
struct material_handle//Ϊshader�в�����ص�ȫ�ֱ�����ֵ�ľ������
{
	ID3DX11EffectVariable *ambient;
	ID3DX11EffectVariable *diffuse;
	ID3DX11EffectVariable *specular;
};
struct pancy_material//���ʽṹ
{
	XMFLOAT4 ambient;
	XMFLOAT4 diffuse;
	XMFLOAT4 specular;
	XMFLOAT4 reflect;
};

class shader_basic
{
protected:
	ID3D11InputLayout                     *input_need;          //������shader�����������ĸ�ʽ
	ID3DX11Effect                         *fx_need;             //shader�ӿ�
	LPCWSTR                               shader_filename;      //shader�ļ���
	std::string                           shader_file_string;
public:
	shader_basic(LPCWSTR filename);				 //���캯��������shader�ļ����ļ���
	engine_basic::engine_fail_reason shder_create();
	engine_basic::engine_fail_reason get_technique(ID3DX11EffectTechnique** tech_need, LPCSTR tech_name); //��ȡ��Ⱦ·��
	engine_basic::engine_fail_reason get_technique(D3D11_INPUT_ELEMENT_DESC member_point[], UINT num_member, ID3DX11EffectTechnique** tech_need, LPCSTR tech_name); //��ȡ������Ⱦ·��
	virtual void release() = 0;
protected:
	engine_basic::engine_fail_reason combile_shader(LPCWSTR filename);		//shader����ӿ�
	virtual void init_handle() = 0;                 //ע��ȫ�ֱ������
	virtual void set_inputpoint_desc(D3D11_INPUT_ELEMENT_DESC *member_point, UINT *num_member) = 0;
	engine_basic::engine_fail_reason set_matrix(ID3DX11EffectMatrixVariable *mat_handle, XMFLOAT4X4 *mat_need);
	bool WCharToMByte(LPCWSTR lpcwszStr, std::string &str);
	void release_basic();
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
class color_shader : public shader_basic 
{
	ID3DX11EffectMatrixVariable           *project_matrix_handle;      //ȫ�׼��α任���
public:
	color_shader(LPCWSTR filename);
	engine_basic::engine_fail_reason set_trans_all(XMFLOAT4X4 *mat_need);                            //�����ܱ任
	void release();
private:
	void init_handle();                 //ע��ȫ�ֱ������
	void set_inputpoint_desc(D3D11_INPUT_ELEMENT_DESC *member_point, UINT *num_member);
};

class picture_show_shader : public shader_basic
{
	ID3DX11EffectVariable                    *UI_scal_handle;
	ID3DX11EffectVariable                    *UI_position_handle;
	ID3DX11EffectShaderResourceVariable      *tex_color_input;      //shader�е�������Դ���
public:
	picture_show_shader(LPCWSTR filename);
	engine_basic::engine_fail_reason set_tex_color_resource(ID3D11ShaderResourceView *buffer_input);
	engine_basic::engine_fail_reason set_UI_scal(XMFLOAT4 scal_range);
	engine_basic::engine_fail_reason set_UI_position(XMFLOAT4 position_range);
	void release();
private:
	void init_handle();                 //ע��ȫ�ֱ������
	void set_inputpoint_desc(D3D11_INPUT_ELEMENT_DESC *member_point, UINT *num_member);
};
class virtual_light_shader : public shader_basic
{
	ID3DX11EffectMatrixVariable           *world_matrix_handle;      //����任���
	ID3DX11EffectMatrixVariable           *normal_matrix_handle;      //���߱任���
	ID3DX11EffectMatrixVariable           *project_matrix_handle;      //ȫ�׼��α任���
	ID3DX11EffectMatrixVariable           *ssao_matrix_handle;       //ssao����任���

	ID3DX11EffectShaderResourceVariable   *texture_diffuse_handle;      //��������ͼ���
	ID3DX11EffectShaderResourceVariable   *texture_normal_handle;       //������ͼ����
	ID3DX11EffectShaderResourceVariable   *texture_specular_handle;     //�߹���ͼ���

	ID3DX11EffectShaderResourceVariable   *texture_diffusearray_handle;     //��������ͼ���

	ID3DX11EffectShaderResourceVariable   *texture_ssao_handle;      //������������Դ���
public:
	virtual_light_shader(LPCWSTR filename);
	engine_basic::engine_fail_reason set_trans_world(XMFLOAT4X4 *mat_world);//�����ܱ任
	engine_basic::engine_fail_reason set_trans_all(XMFLOAT4X4 *mat_need);//�����ܱ任
	engine_basic::engine_fail_reason set_trans_ssao(XMFLOAT4X4 *mat_need);                   //���û�����任

	engine_basic::engine_fail_reason set_tex_diffuse(ID3D11ShaderResourceView *tex_in);//��������������
	engine_basic::engine_fail_reason set_tex_normal(ID3D11ShaderResourceView *tex_in);//���÷�������
	engine_basic::engine_fail_reason set_tex_specular(ID3D11ShaderResourceView *tex_in);//���ø߹�����
	engine_basic::engine_fail_reason set_ssaotex(ID3D11ShaderResourceView *tex_in);			//����ssaomap

	engine_basic::engine_fail_reason set_tex_diffuse_array(ID3D11ShaderResourceView *tex_in);//������������������
	void release();
private:
	void init_handle();                 //ע��ȫ�ֱ������
	void set_inputpoint_desc(D3D11_INPUT_ELEMENT_DESC *member_point, UINT *num_member);
};
class shader_pretreat_gbuffer : public shader_basic
{
	ID3DX11EffectMatrixVariable           *project_matrix_handle;      //ȫ�׼��α任���
	ID3DX11EffectMatrixVariable           *world_matrix_handle;        //����任���
	ID3DX11EffectMatrixVariable           *normal_matrix_handle;       //���߱任���
	ID3DX11EffectMatrixVariable           *world_matrix_array_handle;  //����任�����
	ID3DX11EffectMatrixVariable           *viewproj_matrix_handle;     //ȡ��*ͶӰ�任����

	ID3DX11EffectShaderResourceVariable   *texture_packarray_handle;     //��ͼ����
public:
	shader_pretreat_gbuffer(LPCWSTR filename);
	engine_basic::engine_fail_reason set_trans_world(XMFLOAT4X4 *mat_world, XMFLOAT4X4 *mat_view);
	engine_basic::engine_fail_reason set_trans_all(XMFLOAT4X4 *mat_final);
	engine_basic::engine_fail_reason set_trans_viewproj(XMFLOAT4X4 *mat_need);               //����ȡ��*ͶӰ�任
	engine_basic::engine_fail_reason set_texturepack_array(ID3D11ShaderResourceView *tex_in);
	engine_basic::engine_fail_reason set_world_matrix_array(const XMFLOAT4X4* M, int cnt);	 //��������任�����
	void release();
private:
	void init_handle();//ע��shader������ȫ�ֱ����ľ��
	void set_inputpoint_desc(D3D11_INPUT_ELEMENT_DESC *member_point, UINT *num_member);
};
class shader_resolvedepth : public shader_basic
{
	ID3DX11EffectShaderResourceVariable   *texture_MSAA;
	ID3DX11EffectVariable   *window_size;            //���ڴ�С
	ID3DX11EffectVariable   *projmessage_handle;            //�ӵ�λ��
public:
	shader_resolvedepth(LPCWSTR filename);
	engine_basic::engine_fail_reason set_texture_MSAA(ID3D11ShaderResourceView *tex_in);
	engine_basic::engine_fail_reason set_window_size(float width,float height);
	engine_basic::engine_fail_reason set_projmessage(XMFLOAT3 proj_message);
	void release();
private:
	void init_handle();                 //ע��ȫ�ֱ������
	void set_inputpoint_desc(D3D11_INPUT_ELEMENT_DESC *member_point, UINT *num_member);
};
class shader_ssaomap : public shader_basic
{
	ID3DX11EffectMatrixVariable* ViewToTexSpace;
	ID3DX11EffectVectorVariable* OffsetVectors;
	ID3DX11EffectVectorVariable* FrustumCorners;
	ID3DX11EffectShaderResourceVariable* NormalDepthMap;
	ID3DX11EffectShaderResourceVariable* DepthMap;
	ID3DX11EffectShaderResourceVariable* RandomVecMap;
public:
	shader_ssaomap(LPCWSTR filename);

	engine_basic::engine_fail_reason set_ViewToTexSpace(XMFLOAT4X4 *mat);
	engine_basic::engine_fail_reason set_OffsetVectors(const XMFLOAT4 v[14]);
	engine_basic::engine_fail_reason set_FrustumCorners(const XMFLOAT4 v[4]);
	engine_basic::engine_fail_reason set_NormalDepthtex(ID3D11ShaderResourceView* srv);
	engine_basic::engine_fail_reason set_Depthtex(ID3D11ShaderResourceView* srv);
	engine_basic::engine_fail_reason set_randomtex(ID3D11ShaderResourceView* srv);
	void release();
private:
	void init_handle();//ע��shader������ȫ�ֱ����ľ��
	void set_inputpoint_desc(D3D11_INPUT_ELEMENT_DESC *member_point, UINT *num_member);
};
class shader_ssaoblur : public shader_basic
{
	ID3DX11EffectScalarVariable* TexelWidth;
	ID3DX11EffectScalarVariable* TexelHeight;

	ID3DX11EffectShaderResourceVariable* NormalDepthMap;
	ID3DX11EffectShaderResourceVariable* DepthMap;
	ID3DX11EffectShaderResourceVariable* InputImage;
public:
	shader_ssaoblur(LPCWSTR filename);
	engine_basic::engine_fail_reason set_image_size(float width, float height);
	engine_basic::engine_fail_reason set_tex_resource(ID3D11ShaderResourceView* tex_normaldepth, ID3D11ShaderResourceView* tex_aomap);
	engine_basic::engine_fail_reason set_Depthtex(ID3D11ShaderResourceView* srv);
	void release();
private:
	void init_handle();//ע��shader������ȫ�ֱ����ľ��
	void set_inputpoint_desc(D3D11_INPUT_ELEMENT_DESC *member_point, UINT *num_member);
};
class light_shadow : public shader_basic
{
	ID3DX11EffectMatrixVariable           *project_matrix_handle; //ȫ�׼��α任���
	ID3DX11EffectShaderResourceVariable   *texture_need;
	ID3DX11EffectMatrixVariable           *world_matrix_array_handle;//����任�����
	ID3DX11EffectMatrixVariable           *viewproj_matrix_handle;   //ȡ��*ͶӰ�任����
public:
	light_shadow(LPCWSTR filename);
	engine_basic::engine_fail_reason set_trans_all(XMFLOAT4X4 *mat_need);        //�����ܱ任
	engine_basic::engine_fail_reason set_texturepack_array(ID3D11ShaderResourceView *tex_in);
	engine_basic::engine_fail_reason set_world_matrix_array(const XMFLOAT4X4* M, int cnt);	 //��������任�����
	engine_basic::engine_fail_reason set_trans_viewproj(XMFLOAT4X4 *mat_need);               //����ȡ��*ͶӰ�任
	void release();
private:
	void init_handle();                 //ע��ȫ�ֱ������
	void set_inputpoint_desc(D3D11_INPUT_ELEMENT_DESC *member_point, UINT *num_member);
};
class light_defered_lightbuffer : public shader_basic
{
	ID3DX11EffectVariable                 *light_sun;                  //̫����
	ID3DX11EffectVariable                 *sunlight_num;               //̫����ּ�����
	ID3DX11EffectVariable                 *depth_devide;               //ÿһ�������
	ID3DX11EffectShaderResourceVariable   *suntexture_shadow;          //̫������Ӱ������Դ���
	ID3DX11EffectMatrixVariable           *sunshadow_matrix_handle;    //̫������Ӱͼ�任

	ID3DX11EffectVariable                 *light_list;                 //�ƹ�
	ID3DX11EffectVariable                 *light_num_handle;           //��Դ����
	ID3DX11EffectVariable                 *shadow_num_handle;           //��Դ����
	ID3DX11EffectMatrixVariable           *shadow_matrix_handle;       //��Ӱͼ�任
	ID3DX11EffectMatrixVariable           *view_matrix_handle;         //ȡ���任���
	ID3DX11EffectMatrixVariable           *invview_matrix_handle;      //ȡ���任��任���
	ID3DX11EffectVectorVariable           *FrustumCorners;             //3D��ԭ�ǵ�
	ID3DX11EffectShaderResourceVariable   *NormalspecMap;             //���߾����������Դ���
	ID3DX11EffectShaderResourceVariable   *DepthMap;                   //���������Դ���
	ID3DX11EffectShaderResourceVariable   *texture_shadow;             //��Ӱ������Դ���
public:
	light_defered_lightbuffer(LPCWSTR filename);

	engine_basic::engine_fail_reason set_sunlight(pancy_light_basic light_need);             //����̫����Դ
	engine_basic::engine_fail_reason set_sunshadow_matrix(const XMFLOAT4X4* M, int cnt);     //����̫����Ӱͼ�任����
	engine_basic::engine_fail_reason set_sunlight_num(XMUINT3 all_light_num);                //����̫����Դ����
	engine_basic::engine_fail_reason set_sunshadow_tex(ID3D11ShaderResourceView *tex_in);	//������Ӱ����
	engine_basic::engine_fail_reason set_depth_devide(XMFLOAT4 v);                           //����̫����ּ�

	engine_basic::engine_fail_reason set_light(pancy_light_basic light_need, int light_num); //����һ����Դ
	engine_basic::engine_fail_reason set_light_num(XMUINT3 all_light_num);                   //���ù�Դ����
	engine_basic::engine_fail_reason set_shadow_num(XMUINT3 all_light_num);                  //���ù�Դ����
	engine_basic::engine_fail_reason set_FrustumCorners(const XMFLOAT4 v[4]);                //����3D��ԭ�ǵ�
	engine_basic::engine_fail_reason set_shadow_matrix(const XMFLOAT4X4* M, int cnt);		//������Ӱͼ�任����
	engine_basic::engine_fail_reason set_view_matrix(XMFLOAT4X4 *mat_need);                  //����ȡ���任
	engine_basic::engine_fail_reason set_invview_matrix(XMFLOAT4X4 *mat_need);                  //����ȡ���任

	engine_basic::engine_fail_reason set_Normalspec_tex(ID3D11ShaderResourceView *tex_in);	//���÷��߾��������
	engine_basic::engine_fail_reason set_DepthMap_tex(ID3D11ShaderResourceView *tex_in);		//�����������
	engine_basic::engine_fail_reason set_shadow_tex(ID3D11ShaderResourceView *tex_in);		//������Ӱ����
	void release();
private:
	void init_handle();                 //ע��ȫ�ֱ������
	void set_inputpoint_desc(D3D11_INPUT_ELEMENT_DESC *member_point, UINT *num_member);
};
class light_defered_draw : public shader_basic
{
	ID3DX11EffectVariable                 *material_need;            //����
	ID3DX11EffectVariable                 *view_pos_handle;          //�ӵ�λ��
	ID3DX11EffectMatrixVariable           *world_matrix_handle;      //����任���
	ID3DX11EffectMatrixVariable           *final_matrix_handle;      //ȫ�׼��α任���
	ID3DX11EffectMatrixVariable           *ssao_matrix_handle;       //ssao����任���
	ID3DX11EffectMatrixVariable           *world_matrix_array_handle;//����任�����
	ID3DX11EffectMatrixVariable           *viewproj_matrix_handle;   //ȡ��*ͶӰ�任����
	ID3DX11EffectShaderResourceVariable   *tex_light_diffuse_handle; //�������������Դ���
	ID3DX11EffectShaderResourceVariable   *tex_light_specular_handle;//�����������Դ���
	ID3DX11EffectShaderResourceVariable   *texture_ssao_handle;      //������������Դ���
	ID3DX11EffectShaderResourceVariable   *texture_diffuse_handle;   //������������Դ���

public:
	light_defered_draw(LPCWSTR filename);
	engine_basic::engine_fail_reason set_view_pos(XMFLOAT3 eye_pos);
	engine_basic::engine_fail_reason set_trans_ssao(XMFLOAT4X4 *mat_need);                   //���û�����任
	engine_basic::engine_fail_reason set_trans_world(XMFLOAT4X4 *mat_need);                  //��������任
	engine_basic::engine_fail_reason set_trans_all(XMFLOAT4X4 *mat_need);                    //�����ܱ任
	engine_basic::engine_fail_reason set_trans_viewproj(XMFLOAT4X4 *mat_need);               //����ȡ��*ͶӰ�任
	engine_basic::engine_fail_reason set_material(pancy_material material_in);				//���ò���
	engine_basic::engine_fail_reason set_ssaotex(ID3D11ShaderResourceView *tex_in);			//����ssaomap
	engine_basic::engine_fail_reason set_tex_diffuse_array(ID3D11ShaderResourceView *tex_in);		//��������������
	engine_basic::engine_fail_reason set_diffuse_light_tex(ID3D11ShaderResourceView *tex_in);//���������������
	engine_basic::engine_fail_reason set_specular_light_tex(ID3D11ShaderResourceView *tex_in);//���þ��淴�������
	engine_basic::engine_fail_reason set_world_matrix_array(const XMFLOAT4X4* M, int cnt);	 //��������任�����
	void release();
private:
	void init_handle();                 //ע��ȫ�ֱ������
	void set_inputpoint_desc(D3D11_INPUT_ELEMENT_DESC *member_point, UINT *num_member);
};


class shader_control
{
private:
	std::unordered_map<std::string, std::shared_ptr<shader_basic>> shader_list;
	shader_control();
public:
	static shader_control *shadercontrol_pInstance;
	static engine_basic::engine_fail_reason single_create()
	{
		if (shadercontrol_pInstance != NULL)
		{
			engine_basic::engine_fail_reason failed_reason("the shader contorller instance have been created before");
			return failed_reason;
		}
		else
		{
			shadercontrol_pInstance = new shader_control();
			engine_basic::engine_fail_reason check_failed = shadercontrol_pInstance->init();
			return check_failed;
		}
	}
	static shader_control * GetInstance()
	{
		return shadercontrol_pInstance;
	}
	engine_basic::engine_fail_reason init();
	std::shared_ptr<shader_basic> get_shader_by_type(std::string type_name, engine_basic::engine_fail_reason &if_succeed);
	std::shared_ptr<color_shader> get_shader_color(engine_basic::engine_fail_reason &if_succeed);
	std::shared_ptr<virtual_light_shader> get_shader_virtual_light(engine_basic::engine_fail_reason &if_succeed);
	std::shared_ptr<picture_show_shader> get_shader_picture(engine_basic::engine_fail_reason &if_succeed);
	std::shared_ptr<shader_pretreat_gbuffer> get_shader_gbuffer(engine_basic::engine_fail_reason &if_succeed);
	std::shared_ptr<shader_resolvedepth> get_shader_resolvedepth(engine_basic::engine_fail_reason &if_succeed);
	std::shared_ptr<shader_ssaomap> get_shader_ssaodraw(engine_basic::engine_fail_reason &if_succeed);
	std::shared_ptr<shader_ssaoblur> get_shader_ssaoblur(engine_basic::engine_fail_reason &if_succeed);
	std::shared_ptr<light_shadow> get_shader_shadowmap(engine_basic::engine_fail_reason &if_succeed);
	std::shared_ptr<light_defered_lightbuffer> get_shader_lightbuffer(engine_basic::engine_fail_reason &if_succeed);
	std::shared_ptr<light_defered_draw> get_shader_lightdeffered(engine_basic::engine_fail_reason &if_succeed);
	engine_basic::engine_fail_reason add_a_new_shader(std::type_index class_name, std::shared_ptr<shader_basic> shader_in);
	void release();
private:
	engine_basic::engine_fail_reason init_basic();
};