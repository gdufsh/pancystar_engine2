cbuffer PerFrame
{
	float4x4 gViewToTexSpace;      //����3D�ؽ��������任
	float4   gOffsetVectors[14];   //ƫ�������������ռ�AO
	float4   gFrustumCorners[4];   //3D�ؽ����ĸ��ǣ����ڽ�����դ����ֵ
								   //���������ߵķ���뾶
	float    gOcclusionRadius = 0.5f;
	//�ڵ����������ڸ����ڵ���������ڵ�Ȩֵ
	float    gOcclusionFadeStart = 0.2f;
	float    gOcclusionFadeEnd = 2.0f;
	float    gSurfaceEpsilon = 0.05f;
};
// Nonnumeric values cannot be added to a cbuffer.
Texture2D gNormalDepthMap;
Texture2D gdepth_map;
Texture2D gRandomVecMap;
float OcclusionFunction(float distZ)
{
	float occlusion = 0.0f;
	if (distZ > gSurfaceEpsilon)
	{
		float fadeLength = gOcclusionFadeEnd - gOcclusionFadeStart;
		occlusion = saturate((gOcclusionFadeEnd - distZ) / fadeLength);
	}
	return occlusion;
}
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
SamplerState samRandomVec
{
	Filter = MIN_MAG_LINEAR_MIP_POINT;
	AddressU = WRAP;
	AddressV = WRAP;
};
SamplerState samNormalDepth
{
	Filter = MIN_MAG_LINEAR_MIP_POINT;

	// Set a very far depth value if sampling outside of the NormalDepth map
	// so we do not get false occlusions.
	AddressU = BORDER;
	AddressV = BORDER;
	BorderColor = float4(1e5f, 0.0f, 0.0f, 1e5f);
};
struct VertexIn//��ͨ����
{
	float3	pos 	: POSITION;     //����λ��
	float3	normal 	: NORMAL;       //���㷨����
	float2  tex1    : TEXCOORD;     //������������
};
struct VertexOut
{
	float4 PosH       : SV_POSITION; //��Ⱦ���߱�Ҫ����
	float3 ToFarPlane : TEXCOORD0;   //����3D�ؽ�
	float2 Tex        : TEXCOORD1;   //��������
};
VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	//��Ϊ��ǰһ��shader��դ����ϵ����ص㣬����Ҫ���κα仯
	vout.PosH = float4(vin.pos, 1.0f);
	//���ĸ�����������ϣ��ĸ��ǵ�Ĵ���洢�ڷ��ߵ�x�������棩
	vout.ToFarPlane = gFrustumCorners[vin.normal.x].xyz;
	//��¼����������
	vout.Tex = vin.tex1;
	return vout;
}
//���������ľ�����ڵ��������Ȩֵ
float4 PS(VertexOut pin) : SV_Target
{
	//��ԭ�����������
	float4 normalDepth = gNormalDepthMap.Sample(samNormalDepth, pin.Tex);
	float3 n = normalDepth.xyz;
	float pz = gdepth_map.Sample(samNormalDepth, pin.Tex).r;
	//pz = 0.1f / (1.0f - pz);
	float3 p = (pz / pin.ToFarPlane.z)*pin.ToFarPlane;
	//��ȡ�������
	float3 randVec[3];
	randVec[0] = 2.0f*gRandomVecMap.SampleLevel(samRandomVec, 4.0f*pin.Tex,0.0f).rgb - 1.0f;
	randVec[1] = 2.0f*gRandomVecMap.SampleLevel(samRandomVec, 2.3f*pin.Tex, 0.0f).rgb - 1.0f;
	randVec[2] = 2.0f*gRandomVecMap.SampleLevel(samRandomVec, 5.2f*pin.Tex, 0.0f).rgb - 1.0f;
	randVec[0] = normalize(randVec[0]);
	randVec[1] = normalize(randVec[1]);
	randVec[2] = normalize(randVec[2]);
	float occlusionSum = 0.0f;
	[unroll]
	for (int i = 0; i < 42; ++i)
	{
		int now_rand = i / 14;
		int now_check = i % 14;
		//����ƽ������
		float3 offset = reflect(gOffsetVectors[now_check].xyz, randVec[now_rand]);
		float flip = sign(dot(offset, n));//����������
										  //���ݰ뾶��������
		float3 q = p + flip * gOcclusionRadius * offset;

		//�õ�����������normaldepthmap�ϵ�����
		float4 projQ = mul(float4(q, 1.0f), gViewToTexSpace);
		projQ /= projQ.w;
		//����������normalmap���ҵ���ǰ�ӽ��ܿ�����һ�������
		float rz = gdepth_map.SampleLevel(samNormalDepth, projQ.xy,0.0f).r;
		//��ԭ��ǰ�ӽ��ܿ�������������������
		float3 r = (rz / q.z) * q;
		//���ݾ���d = |p.z - r.z|���ڵ���ָ����Ե����������Ե㷨�����ļн�n*(r-p)��ͬ�����r���p����ڵ�����
		float distZ = p.z - r.z;
		//����AO����
		//float delta = min(max((30.0f - p.z), 1.0f) / (30.0f - 5.0f),1.0f);
		float dp = max(dot(n, normalize(r - p)), 0.0f);
		float occlusion = dp * OcclusionFunction(distZ);

		occlusionSum += occlusion*0.3f;
	}

	occlusionSum /= 42;
	float access = 1.0f - occlusionSum;
	float4 outputColor;
	//�ߴ�ƽ��
	outputColor.r = saturate(pow(access, 4.0f));
	outputColor.g = 0.0f;
	outputColor.b = 0.0f;
	outputColor.a = 0.0f;
	return outputColor;
}
technique11 draw_ssaomap
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS()));
	}
}
