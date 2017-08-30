cbuffer cbPerFrame
{
	float gTexelWidth;
	float gTexelHeight;
};
cbuffer cbSettings
{
	float gWeights[11] =
	{
		0.05f, 0.05f, 0.1f, 0.1f, 0.1f, 0.2f, 0.1f, 0.1f, 0.1f, 0.05f, 0.05f
	};
};
cbuffer cbFixed
{
	static const int gBlurRadius = 5;
};
Texture2D gInputImage;
SamplerState samInputImage
{
	Filter = MIN_MAG_LINEAR_MIP_POINT;
	AddressU = CLAMP;
	AddressV = CLAMP;
};
struct VertexIn//��ͨ����
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
	vout.PosH = float4(vin.pos, 1.0f);
	vout.Tex = vin.tex1;
	return vout;
}
float4 PS(VertexOut pin, uniform bool gHorizontalBlur) : SV_Target
{
	float2 texOffset;
	if (gHorizontalBlur)
	{
		texOffset = float2(gTexelWidth, 0.0f);
	}
	else
	{
		texOffset = float2(0.0f, gTexelHeight);
	}
	//�������ĵ����ɫֵ��Ϊ�������ֵ
	float4 center_color = gInputImage.SampleLevel(samInputImage, pin.Tex, 0.0);
	float4 color = gWeights[5] * center_color;
	for (float i = -gBlurRadius; i <= gBlurRadius; ++i)
	{
		if (i == 0)
			continue;
		float2 tex = pin.Tex + i*texOffset;
		//�ɼ��߽�����ɫ
		float4 neighborcolor = gInputImage.SampleLevel(samInputImage, tex, 0.0);
		//���㵱ǰ�����ɫ���
		float weight = gWeights[i + gBlurRadius];
		color += weight*neighborcolor;
	}
	return color;
}
technique11 HorzBlur
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS(true)));
	}
}

technique11 VertBlur
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS(false)));
	}
}