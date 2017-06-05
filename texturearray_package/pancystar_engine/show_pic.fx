cbuffer PerFrame
{
	float4 UI_scal;        //UI��ռ�Ĵ�С(��������ȣ�����)
	float4 UI_pos;         //UI���ڵ�λ��
};
Texture2D texture_need;
SamplerState samTex_liner
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};
struct VertexIn
{
	float3	pos 	: POSITION;     //����λ��
	float2  tex1    : TEXCOORD;     //������������
};
struct VertexOut
{
	float4 PosH       : SV_POSITION; //��Ⱦ���߱�Ҫ����
	float2 Tex        : TEXCOORD1;   //��������
};
VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	float3 now_pos;
	now_pos = vin.pos * UI_scal.xyz;
	now_pos = now_pos + UI_pos.xyz;
	vout.PosH = float4(now_pos, 1.0f);
	vout.Tex = vin.tex1;
	return vout;
}
float4 PS(VertexOut pin) : SV_Target
{
	float2 tex_uv = pin.Tex;
	float4 texcolor = texture_need.Sample(samTex_liner, tex_uv);
	return texcolor;
}
technique11 draw_ui
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS()));
	}
}