cbuffer PerFrame
{
	float4x4 world_matrix;      //����任
	float4x4 normal_matrix;     //���߱任
	float4x4 final_matrix;      //�ܱ任
	float4x4 world_matrix_array[300];
	float4x4 view_proj_matrix;
};
Texture2DArray   texture_pack_array;  //��ͼ��
SamplerState samTex_liner
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};
struct VertexIn//��ͨ����
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
	float4 PosH       : SV_POSITION;
	float3 PosV       : POSITION;
	float3 NormalV    : NORMAL;
	float3 tangent    : TANGENT;
	uint4   texid     : TEXINDICES;   //��������
	float4  tex1      : TEXDIFFNORM;  //������������(�����估��������)
	float4  tex2      : TEXOTHER;     //������������(��������)
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

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	vout.PosV = mul(float4(vin.pos, 1.0f), world_matrix).xyz;
	vout.NormalV = mul(float4(vin.normal, 0.0f), normal_matrix).xyz;
	vout.tangent = mul(float4(vin.tangent, 0.0f), normal_matrix).xyz;
	vout.PosH = mul(float4(vin.pos, 1.0f), final_matrix);
	vout.texid = vin.texid;
	vout.tex1 = vin.tex1;
	vout.tex2 = vin.tex2;
	return vout;
}
VertexOut VS_instance(Vertex_IN_instance vin)
{
	VertexOut vout;
	vout.PosV = mul(float4(vin.pos, 1.0f), world_matrix_array[vin.InstanceId]).xyz;
	vout.NormalV = mul(float4(vin.normal, 0.0f), world_matrix_array[vin.InstanceId]).xyz;
	vout.tangent = mul(float4(vin.tangent, 0.0f), world_matrix_array[vin.InstanceId]).xyz;
	vout.PosH = mul(float4(vout.PosV, 1.0f), view_proj_matrix);
	vout.texid = vin.texid;
	vout.tex1 = vin.tex1;
	vout.tex2 = vin.tex2;
	return vout;
}
float4 PS(VertexOut pin) : SV_Target
{
	float texID_data_diffuse = pin.texid.x;
	float4 tex_color = texture_pack_array.Sample(samTex_liner, float3(pin.tex1.xy, texID_data_diffuse));
	clip(tex_color.a - 0.5f);
	pin.NormalV = normalize(pin.NormalV);
	return float4(pin.NormalV, 10.0f);
}
float4 PS_withnormal(VertexOut pin) : SV_Target
{
	float texID_data_diffuse = pin.texid.x;
	float texID_data_normal = pin.texid.y;
	float4 tex_color = texture_pack_array.Sample(samTex_liner, float3(pin.tex1.xy, texID_data_diffuse));
	clip(tex_color.a - 0.5f);
	pin.NormalV = normalize(pin.NormalV);
	pin.tangent = normalize(pin.tangent);
	//���ͼƬ�����Կռ�->ģ������ͳһ����ռ�ı任����
	float3 N = pin.NormalV;
	float3 T = normalize(pin.tangent - N * pin.tangent * N);
	float3 B = cross(N, T);
	float3x3 T2W = float3x3(T, B, N);
	float3 normal_map = texture_pack_array.Sample(samTex_liner, float3(pin.tex1.zw, texID_data_normal)).rgb;//�ӷ�����ͼ�л�÷��߲���
	//float3 normal_map = texture_normal.Sample(samTex_liner, pin.tex).rgb;
	normal_map = 2 * normal_map - 1;                               //��������ͼƬ����[0,1]ת������ʵ����[-1,1]  
	normal_map = normalize(mul(normal_map, T2W));                  //���߿ռ�������ռ�
	pin.NormalV = normal_map;
	return float4(normal_map, 10.0);
}
technique11 NormalDepth
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS()));
	}
}
technique11 NormalDepth_withinstance
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS_instance()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS()));
	}
}
technique11 NormalDepth_withinstance_normal
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS_instance()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_withnormal()));
	}
}
technique11 NormalDepth_withnormal
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_withnormal()));
	}
}