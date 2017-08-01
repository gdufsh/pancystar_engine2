cbuffer PerFrame
{
	uint4 part_ID;
	float4x4 final_matrix;      //�ܱ任;
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
struct Vertex_IN_bone//��������ͼ����
{
	float3	pos 	    : POSITION;     //����λ��
	float3	normal 	    : NORMAL;       //���㷨����
	float3	tangent     : TANGENT;      //����������
	uint4   bone_id     : BONEINDICES;  //����ID��
	float4  bone_weight : WEIGHTS;      //����Ȩ��
	uint4   texid       : TEXINDICES;   //��������
	float2  tex1        : TEXCOORD;     //������������
};
struct Vertex_out
{
	float4 PosH       : SV_POSITION;
	uint4  texid       : TEXINDICES;
};
Vertex_out VS(Vertex_IN vin)
{
	Vertex_out vout;
	vout.PosH = mul(float4(vin.pos, 1.0f), final_matrix);
	vout.texid = vin.texid;
	return vout;
}
uint4 PS(Vertex_out pin) : SV_Target
{
	//uint4 texcolor = pin.texid;
	//texcolor = part_ID;
	return part_ID;
}
technique11 draw_clipmap
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS()));
	}
}