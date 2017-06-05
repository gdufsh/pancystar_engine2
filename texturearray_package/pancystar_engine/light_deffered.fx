/*�ӳٹ���Ч��*/
#include"light_count.hlsli"
cbuffer perobject
{
	pancy_material   material_need;    //����
	float4x4         world_matrix;     //����任
	float3           position_view;         //�ӵ�λ��
	float4x4         final_matrix;     //�ܱ任
	float4x4         ssao_matrix;      //ssao�任
	float4x4         world_matrix_array[300];
	float4x4         view_proj_matrix;
};
Texture2D        texture_light_diffuse;      //�����������ͼ
Texture2D        texture_light_specular;     //���淴�������ͼ
Texture2DArray        texture_pack_array;         //��������ͼ
Texture2D        texture_ssao;               //ssao��ͼ
SamplerState samTex
{
	Filter = ANISOTROPIC;
	MaxAnisotropy = 4;
	AddressU = WRAP;
	AddressV = WRAP;
};
SamplerState samTex_liner
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};
struct Vertex_IN//��������ͼ����
{
	float3	pos 	: POSITION;     //����λ��
	float3	normal 	: NORMAL;       //���㷨����
	float3	tangent : TANGENT;      //����������
	uint4   texid   : TEXINDICES;   //��������
	float4  tex1    : TEXDIFFNORM;  //������������(�����估��������)
	float4  tex2    : TEXOTHER;     //������������(��������)
};
struct Vertex_IN_instance
{
	float3	pos 	: POSITION;     //����λ��
	float3	normal 	: NORMAL;       //���㷨����
	float3	tangent : TANGENT;      //����������
	uint4   texid   : TEXINDICES;   //��������
	float4  tex1    : TEXDIFFNORM;  //������������(�����估��������)
	float4  tex2    : TEXOTHER;     //������������(��������)
	uint InstanceId : SV_InstanceID;//instace������
};

struct VertexOut
{
	float3 position_before : POSITION;
	float4 position        : SV_POSITION;    //�任��Ķ�������
	uint4   texid          : TEXINDICES;     //��������
	float4  tex1           : TEXDIFFNORM;    //������������(�����估��������)
	float4  tex2           : TEXOTHER;       //������������(��������)
	float4 pos_ssao        : POSITION1;      //��Ӱ��������
};
struct PixelOut
{
	float4 final_color;
	float4 reflect_message;
};

VertexOut VS(Vertex_IN vin)
{
	VertexOut vout;
	vout.position_before = mul(float4(vin.pos, 1.0f), world_matrix).xyz;
	vout.position = mul(float4(vin.pos, 1.0f), final_matrix);
	vout.texid = vin.texid;
	vout.tex1 = vin.tex1;
	vout.tex2 = vin.tex2;
	vout.pos_ssao = mul(float4(vout.position_before, 1.0f), ssao_matrix);
	return vout;
}
VertexOut VS_instance(Vertex_IN_instance vin)
{
	VertexOut vout;
	vout.position_before = mul(float4(vin.pos, 1.0f), world_matrix_array[vin.InstanceId]).xyz;
	vout.position = mul(float4(vout.position_before, 1.0f), view_proj_matrix);
	vout.texid = vin.texid;
	vout.tex1 = vin.tex1;
	vout.tex2 = vin.tex2;
	vout.pos_ssao = mul(float4(vout.position_before, 1.0f), ssao_matrix);
	return vout;
}
PixelOut PS(VertexOut pin) :SV_TARGET
{
	pin.pos_ssao /= pin.pos_ssao.w;
	float texID_data_diffuse = pin.texid.x;
	float4 tex_color = texture_pack_array.Sample(samTex_liner, float3(pin.tex1.xy, texID_data_diffuse));
	//float4 tex_color = texture_diffuse.Sample(samTex_liner, pin.tex);
	clip(tex_color.a - 0.6f);
	float4 ambient = 0.5f*float4(1.0f, 1.0f, 1.0f, 0.0f) * texture_ssao.Sample(samTex_liner, pin.pos_ssao.xy, 0.0f).r;
	float4 diffuse = material_need.diffuse * texture_light_diffuse.Sample(samTex_liner, pin.pos_ssao.xy, 0.0f);      //�������
	float4 spec = material_need.specular * texture_light_specular.Sample(samTex_liner, pin.pos_ssao.xy, 0.0f);       //���淴���
	float4 final_color = tex_color *(ambient + diffuse) + spec;
	final_color.a = tex_color.a;

	PixelOut ans_pix;
	ans_pix.final_color = final_color;
	ans_pix.reflect_message = material_need.reflect;
	return ans_pix;
}
technique11 LightTech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS()));
	}
}
technique11 LightTech_instance
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS_instance()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS()));
	}
}