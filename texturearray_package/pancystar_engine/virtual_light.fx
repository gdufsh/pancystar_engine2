#include<light_count.hlsli>
float4x4         world_matrix;     //����任
float4x4         normal_matrix;    //���߱任
float4x4         final_matrix;     //�ܱ任
float4x4         ssao_matrix;      //ssao�任

Texture2D        texture_diffuse;      //��������ͼ
Texture2D        texture_specular;     //���淴����ͼ
Texture2D        texturet_normal;      //������ͼ

Texture2D        texture_ssao;      //ao��ͼ

Texture2DArray   texture_pack_diffuse;


SamplerState samTex_liner
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};
struct Vertex_IN
{
	float3	pos 	: POSITION;     //����λ��
	float3	normal 	: NORMAL;       //���㷨����
	float3	tangent : TANGENT;      //����������
	uint4   texid   : TEXINDICES;   //��������
	float4  tex1    : TEXDIFFNORM;  //������������(�����估��������)
	float4  tex2    : TEXOTHER;     //������������(��������)
};
struct VertexOut
{
	float4 position      : SV_POSITION;    //�任��Ķ�������
	float3 normal        : NORMAL;         //�任��ķ�����
	float3 tangent       : TANGENT;        //����������
	uint4  texid         : TEXINDICES;     //��������
	float4  tex1         : TEXDIFFNORM;  //������������(�����估��������)
	float4  tex2         : TEXOTHER;     //������������(��������)
	float3 position_bef  : POSITION;       //�任ǰ�Ķ�������
	float4 pos_ssao      : AOPOSITION;      //��Ӱ��������
};
VertexOut VS(Vertex_IN vin)
{
	VertexOut vout;
	vout.position = mul(float4(vin.pos, 1.0f), final_matrix);
	vout.normal = mul(float4(vin.normal, 0.0f), normal_matrix).xyz;
	vout.tangent = mul(float4(vin.tangent, 0.0f), normal_matrix).xyz;
	vout.texid = vin.texid;
	vout.tex1 = vin.tex1;
	vout.tex2 = vin.tex2;
	vout.position_bef = mul(float4(vin.pos, 1.0f), world_matrix).xyz;
	vout.pos_ssao = mul(float4(vout.position_bef, 1.0f), ssao_matrix);
	return vout;
}
float4 PS(VertexOut pin) :SV_TARGET
{
	float4 A,D,S;
	pancy_light_basic light_dir;
	light_dir.ambient = float4(0.5f, 0.5f, 0.5f,1.0f);
	light_dir.diffuse = float4(1.0f, 1.0f, 1.0f, 1.0f);
	light_dir.specular = float4(0.5f, 0.5f, 0.5f, 1.0f);
	light_dir.dir = float3(1.0f,0.0f, 0.0f);

	pancy_material material_need;
	material_need.ambient = float4(1.0f, 1.0f, 1.0f, 1.0f);
	material_need.diffuse = float4(1.0f, 1.0f, 1.0f, 1.0f);
	material_need.specular = float4(1.0f, 1.0f, 1.0f, 1.0f);
	material_need.reflect = float4(0.0f, 0.0f, 0.0f, 1.0f);

	float3 normal_need = normalize(pin.normal);
	float3 view_dir = float3(0.0f, 0.0f, 1.0f);

	compute_dirlight(material_need, light_dir, normal_need, view_dir,A,D,S);
	float4 tex_color = texture_diffuse.Sample(samTex_liner,pin.tex1.xy);
	float4 final_color = tex_color *(A + D) + S;
	return final_color;
}
float4 PS_array(VertexOut pin) :SV_TARGET
{
	pin.pos_ssao /= pin.pos_ssao.w;
	float4 A,D,S;
	pancy_light_basic light_dir;
	light_dir.ambient = float4(0.5f, 0.5f, 0.5f,1.0f);
	light_dir.diffuse = float4(1.0f, 1.0f, 1.0f, 1.0f);
	light_dir.specular = float4(0.5f, 0.5f, 0.5f, 1.0f);
	light_dir.dir = float3(1.0f,0.0f, 0.0f);

	pancy_material material_need;
	material_need.ambient = float4(1.0f, 1.0f, 1.0f, 1.0f);
	material_need.diffuse = float4(1.0f, 1.0f, 1.0f, 1.0f);
	material_need.specular = float4(1.0f, 1.0f, 1.0f, 1.0f);
	material_need.reflect = float4(0.0f, 0.0f, 0.0f, 1.0f);

	float3 normal_need = normalize(pin.normal);
	float3 view_dir = float3(0.0f, 0.0f, 1.0f);
	compute_dirlight(material_need, light_dir, normal_need, view_dir,A,D,S);
	float texID_data = pin.texid.x;
	if (texID_data > 1.0f) 
	{
		texID_data *= 1.001;
	}
	float4 tex_color = texture_pack_diffuse.Sample(samTex_liner, float3(pin.tex1.xy, texID_data));
	float4 ao_color = texture_ssao.Sample(samTex_liner, pin.pos_ssao.xy, 0.0f).r;
//	if (texID_data < 1.5f) 
//	{
//		tex_color = float4(0.0f, 0.0f, 0.0f, 0.0f);
//	}
	
	float4 final_color = ao_color *A + D + S;
	return final_color;
}
technique11 light_tech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS()));
	}
}
technique11 light_tech_array
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_array()));
	}
}