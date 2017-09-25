cbuffer PerFrame
{
	float4x4 world_matrix;      //����任
	float4x4 normal_matrix;     //���߱任
	float4x4 final_matrix;      //�ܱ任
	float4x4 textureproj_matrix;//����ͶӰ�任
	float3   position_view;     //�ӵ�λ��
};
TextureCube texture_cube;
Texture2D atmosphere_mask;           //����ɢ������
SamplerState samTex
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
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
struct VertexOut
{
	float4 position      : SV_POSITION;    //�任��Ķ�������
	float3 normal        : TEXCOORD0;      //�任��ķ�����
	float3 position_bef	 : TEXCOORD1;      //�任ǰ�Ķ�������
	float4 pos_texproj   : TEXCOORD2;      //��Ӱ��������
};
RasterizerState DisableCulling
{
	CullMode = FRONT;
};
VertexOut VS(Vertex_IN vin)
{
	VertexOut vout;
	vout.position = mul(float4(vin.pos, 1.0f), final_matrix);
	vout.position_bef = mul(float4(vin.pos, 1.0f), world_matrix).xyz;
	vout.normal = normalize(mul(float4(vin.normal, 0.0f), normal_matrix)).xyz;
	vout.pos_texproj = mul(float4(vout.position_bef, 1.0f), textureproj_matrix);
	return vout;
}
float4 PS(VertexOut pin) :SV_TARGET
{
	float4 tex_color = float4(0.0f,0.0f,0.0f,0.0f);
	float4 color_fog = float4(0.75f, 0.75f, 0.75f, 1.0f);
	float3 view_direct = normalize(pin.position_bef - position_view);
	float3 map_direct = view_direct.xyz;//��������
	
	tex_color = texture_cube.Sample(samTex, map_direct);
	pin.pos_texproj /= pin.pos_texproj.w;
	float4 atomosphere_color = atmosphere_mask.Sample(samTex, pin.pos_texproj.xy);
	return tex_color;
}

technique11 draw_sky
{
	Pass p0
	{
		SetVertexShader(CompileShader(vs_5_0,VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS()));
		SetRasterizerState(DisableCulling);
	}
}

