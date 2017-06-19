/*�ӳٹ���Ч��*/
#include"light_count.hlsli"
cbuffer perframe
{
	pancy_light_basic   sun_light;
	uint3               sun_light_num;
	float4              depth_devide;
	float4x4            sunlight_shadowmat[6];
	pancy_light_basic   light_need[100];      //��Դ
	float4x4            shadowmap_matrix[20]; //��Ӱ��ͼ�任
	float4x4            view_matrix;          //ȡ���任
	float4x4            invview_matrix;       //ȡ���任����
	uint3               light_num;            //��Դ����
	uint3               shadow_num;           //��Ӱ����
	float4              gFrustumCorners[4];   //3D�ؽ����ĸ��ǣ����ڽ�����դ����ֵ
	float3              proj_desc;//ͶӰ���������ڻ�ԭ�����Ϣ
};
Texture2DArray texture_sunshadow;//̫������Ӱ��ͼ
Texture2DArray texture_shadow;   //��Ӱ��ͼ
Texture2D gNormalspecMap;        //��Ļ�ռ䷨��&���淴��ǿ������
Texture2D gdepth_map;            //��Ļ�ռ��������
SamplerState samTex
{
	Filter = ANISOTROPIC;
	MaxAnisotropy = 4;
	AddressU = WRAP;
	AddressV = WRAP;
};
SamplerComparisonState samShadow
{
	Filter = COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
	AddressU = BORDER;
	AddressV = BORDER;
	AddressW = BORDER;
	BorderColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
	ComparisonFunc = LESS_EQUAL;
};
SamplerState samTex_liner
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};
SamplerState samNormalDepth
{
	Filter = MIN_MAG_LINEAR_MIP_POINT;

	AddressU = CLAMP;
	AddressV = CLAMP;
};
float CalcShadowFactor(SamplerComparisonState samShadow, Texture2DArray shadowMap, int shadowtex_num, float4 shadowPosH)
{
	//��һ��
	shadowPosH.xyz /= shadowPosH.w;

	//�ɼ���ԴͶӰ������
	float depth = shadowPosH.z-0.05f;

	//��Ӱ��ͼ�Ĳ���
	const float dx = 1.0f / 1024.0f;
	float percentLit = 0.0f;
	const float2 offsets[9] =
	{
		float2(-dx,  -dx), float2(0.0f,  -dx), float2(dx,  -dx),
		float2(-dx, 0.0f), float2(0.0f, 0.0f), float2(dx, 0.0f),
		float2(-dx,  +dx), float2(0.0f,  +dx), float2(dx,  +dx)
	};

	[unroll]
	for (int i = 0; i < 9; ++i)
	{
		float2 rec_pos = shadowPosH.xy + offsets[i];
		if (rec_pos.x > 1.0f || rec_pos.x < 0.0f || rec_pos.y > 1.0f || rec_pos.y < 0.0f)
		{
			percentLit += 1.0f;
		}
		else
		{
			/*
			float depth_rec = shadowMap.Sample(samNormalDepth, float3(rec_pos, shadowtex_num)).r; //Sample(samShadow, float3(rec_pos, shadowtex_num)).r;
			if (depth_rec < depth)
			{
			percentLit += 1;
			}
			*/
			percentLit += shadowMap.SampleCmpLevelZero(samShadow, float3(rec_pos, shadowtex_num), depth).r;
		}
	}
	return 1.0f;
	return percentLit /= 9.0f;
}
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
struct PixelOut_high
{
	float4 diffuse       : SV_TARGET0;
	float4 specular      : SV_TARGET1;
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
PixelOut_high PS(VertexOut pin)
{
	PixelOut_high pout;
	pout.diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	pout.specular = float4(0.0f, 0.0f, 0.0f, 0.0f);
	//��ԭ�����������
	float4 normalspec = gNormalspecMap.Sample(samNormalDepth, pin.Tex);
	float3 normal_need = normalspec.xyz;
	float pz = gdepth_map.Sample(samNormalDepth, pin.Tex).r;
	float3 position_need = (pz / pin.ToFarPlane.z)*pin.ToFarPlane;
	pancy_material material_need;
	material_need.ambient = float4(1.0f, 1.0f, 1.0f, 0.0f);
	material_need.diffuse = float4(1.0f, 1.0f, 1.0f, 0.0f);
	material_need.specular = float4(1.0f, 1.0f, 1.0f, normalspec.a);
	int count_shadow = 0;
	//~~~~~~~~~~~~~~~~~~~���߿ռ���ɫ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//[unroll]
	float4 A = 0.0f, D = 0.0f, S = 0.0f;
	//�ȼ���̫����
	pancy_light_basic rec_sunlight = sun_light;
	float3 position_view_sun = float3(0.0f, 0.0f, 0.0f);
	float3 eye_direct_sun = normalize(position_view_sun - position_need);
	compute_dirlight(material_need, rec_sunlight, normal_need, eye_direct_sun, A, D, S);

	int count_sunlight;
	if (position_need.z < depth_devide.x)
	{
		count_sunlight = 0;
	}
	else if (position_need.z < depth_devide.y)
	{
		count_sunlight = 1;
	}
	else if (position_need.z < depth_devide.z)
	{
		count_sunlight = 2;
	}
	else
	{
		count_sunlight = 3;
	}
	float4 pos_shadow_world = mul(float4(position_need, 1.0f), invview_matrix);
	float4 pos_shadow = mul(pos_shadow_world, sunlight_shadowmat[count_sunlight]);
	float rec_shadow = CalcShadowFactor(samShadow, texture_sunshadow, count_sunlight, pos_shadow);
	pout.diffuse += rec_shadow*D;
	pout.specular += rec_shadow*S;
	//�ټ�����ͨ��
	int count_all = 0;
	for (uint i = 0; i < shadow_num.x; ++i)
	{
		pancy_light_basic rec_light = light_need[count_all];
		rec_light.dir = mul(float4(light_need[count_all].dir, 0.0f), view_matrix).xyz;
		float3 position_view = float3(0.0f, 0.0f, 0.0f);
		float3 eye_direct = normalize(position_view - position_need);
		compute_dirlight(material_need, rec_light, normal_need, eye_direct, A, D, S);

		float4 pos_shadow_world = mul(float4(position_need, 1.0f), invview_matrix);
		float4 pos_shadow = mul(pos_shadow_world, shadowmap_matrix[count_all]);
		float rec_shadow = CalcShadowFactor(samShadow, texture_shadow, count_all, pos_shadow);
		pout.diffuse += (0.2f + 0.8f*rec_shadow)*D;
		pout.specular += (0.2f + 0.8f*rec_shadow)*S;
		count_all += 1;
	}
	for (uint i = 0; i < shadow_num.z; ++i)
	{
		pancy_light_basic rec_light = light_need[count_all];
		rec_light.position = mul(float4(light_need[count_all].position, 1.0f), view_matrix).xyz;
		rec_light.dir = mul(float4(light_need[count_all].dir, 0.0f), view_matrix).xyz;
		float3 position_view = float3(0.0f, 0.0f, 0.0f);
		float3 eye_direct = normalize(position_view - position_need);
		compute_spotlight(material_need, rec_light, position_need, normal_need, eye_direct, A, D, S);

		float4 pos_shadow_world = mul(float4(position_need, 1.0f), invview_matrix);
		float4 pos_shadow = mul(pos_shadow_world, shadowmap_matrix[count_all]);
		float rec_shadow = CalcShadowFactor(samShadow, texture_shadow, count_all, pos_shadow);
		pout.diffuse += (0.2f + 0.8f*rec_shadow)*D;
		pout.specular += (0.2f + 0.8f*rec_shadow)*S;
		count_all += 1;
	}
	for (uint i = 0; i < light_num.x; ++i)
	{
		pancy_light_basic rec_light = light_need[count_all];
		rec_light.position = mul(float4(light_need[count_all].position, 1.0f), view_matrix).xyz;
		rec_light.dir = mul(float4(light_need[count_all].dir, 0.0f), view_matrix).xyz;
		float3 position_view = float3(0.0f, 0.0f, 0.0f);
		float3 eye_direct = normalize(position_view - position_need);
		compute_dirlight(material_need, rec_light, normal_need, eye_direct, A, D, S);
		pout.diffuse += D;
		pout.specular += S;
		count_all += 1;
	}
	for (uint i = 0; i < light_num.y; ++i)
	{
		pancy_light_basic rec_light = light_need[count_all];
		rec_light.position = mul(float4(light_need[count_all].position, 1.0f), view_matrix).xyz;
		rec_light.dir = mul(float4(light_need[count_all].dir, 0.0f), view_matrix).xyz;
		float3 position_view = float3(0.0f, 0.0f, 0.0f);
		float3 eye_direct = normalize(position_view - position_need);
		compute_pointlight(material_need, rec_light, position_need, normal_need, position_view, A, D, S);
		pout.diffuse += D;
		pout.specular += S;
		count_all += 1;
	}
	for (uint i = 0; i < light_num.z; ++i)
	{
		pancy_light_basic rec_light = light_need[count_all];
		rec_light.position = mul(float4(light_need[count_all].position, 1.0f), view_matrix).xyz;
		rec_light.dir = mul(float4(light_need[count_all].dir, 0.0f), view_matrix).xyz;
		float3 position_view = float3(0.0f, 0.0f, 0.0f);
		float3 eye_direct = normalize(position_view - position_need);
		compute_spotlight(material_need, rec_light, position_need, normal_need, eye_direct, A, D, S);
		pout.diffuse += D;
		pout.specular += S;
		count_all += 1;
	}
	return pout;
}
PixelOut_high PS_withoutMSAA(VertexOut pin)
{
	PixelOut_high pout;
	pout.diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	pout.specular = float4(0.0f, 0.0f, 0.0f, 0.0f);
	//��ԭ�����������
	float4 normalspec = gNormalspecMap.Sample(samNormalDepth, pin.Tex);
	float3 normal_need = normalspec.xyz;
	float pz = gdepth_map.Sample(samNormalDepth, pin.Tex).r;
	pz = 1.0f / (pz * proj_desc.x + proj_desc.y);
	float3 position_need = (pz / pin.ToFarPlane.z)*pin.ToFarPlane;
	pancy_material material_need;
	material_need.ambient = float4(1.0f, 1.0f, 1.0f, 0.0f);
	material_need.diffuse = float4(1.0f, 1.0f, 1.0f, 0.0f);
	material_need.specular = float4(1.0f, 1.0f, 1.0f, normalspec.a);
	int count_shadow = 0;
	//~~~~~~~~~~~~~~~~~~~���߿ռ���ɫ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//[unroll]
	float4 A = 0.0f, D = 0.0f, S = 0.0f;
	//�ȼ���̫����
	pancy_light_basic rec_sunlight = sun_light;
	float3 position_view_sun = float3(0.0f, 0.0f, 0.0f);
	float3 eye_direct_sun = normalize(position_view_sun - position_need);
	compute_dirlight(material_need, rec_sunlight, normal_need, eye_direct_sun, A, D, S);

	int count_sunlight;
	if (position_need.z < depth_devide.x)
	{
		count_sunlight = 0;
	}
	else if (position_need.z < depth_devide.y)
	{
		count_sunlight = 1;
	}
	else if (position_need.z < depth_devide.z)
	{
		count_sunlight = 2;
	}
	else
	{
		count_sunlight = 3;
	}
	float4 pos_shadow_world = mul(float4(position_need, 1.0f), invview_matrix);
	float4 pos_shadow = mul(pos_shadow_world, sunlight_shadowmat[count_sunlight]);
	float rec_shadow = CalcShadowFactor(samShadow, texture_sunshadow, count_sunlight, pos_shadow);
	pout.diffuse += rec_shadow*D;
	pout.specular += rec_shadow*S;
	//�ټ�����ͨ��
	int count_all = 0;
	for (uint i = 0; i < shadow_num.x; ++i)
	{
		pancy_light_basic rec_light = light_need[count_all];
		rec_light.dir = mul(float4(light_need[count_all].dir, 0.0f), view_matrix).xyz;
		float3 position_view = float3(0.0f, 0.0f, 0.0f);
		float3 eye_direct = normalize(position_view - position_need);
		compute_dirlight(material_need, rec_light, normal_need, eye_direct, A, D, S);

		float4 pos_shadow_world = mul(float4(position_need, 1.0f), invview_matrix);
		float4 pos_shadow = mul(pos_shadow_world, shadowmap_matrix[count_all]);
		float rec_shadow = CalcShadowFactor(samShadow, texture_shadow, count_all, pos_shadow);
		pout.diffuse += (0.2f + 0.8f*rec_shadow)*D;
		pout.specular += (0.2f + 0.8f*rec_shadow)*S;
		count_all += 1;
	}
	for (uint i = 0; i < shadow_num.z; ++i)
	{
		pancy_light_basic rec_light = light_need[count_all];
		rec_light.position = mul(float4(light_need[count_all].position, 1.0f), view_matrix).xyz;
		rec_light.dir = mul(float4(light_need[count_all].dir, 0.0f), view_matrix).xyz;
		float3 position_view = float3(0.0f, 0.0f, 0.0f);
		float3 eye_direct = normalize(position_view - position_need);
		compute_spotlight(material_need, rec_light, position_need, normal_need, eye_direct, A, D, S);

		float4 pos_shadow_world = mul(float4(position_need, 1.0f), invview_matrix);
		float4 pos_shadow = mul(pos_shadow_world, shadowmap_matrix[count_all]);
		float rec_shadow = CalcShadowFactor(samShadow, texture_shadow, count_all, pos_shadow);
		pout.diffuse += (0.2f + 0.8f*rec_shadow)*D;
		pout.specular += (0.2f + 0.8f*rec_shadow)*S;
		count_all += 1;
	}
	for (uint i = 0; i < light_num.x; ++i)
	{
		pancy_light_basic rec_light = light_need[count_all];
		rec_light.position = mul(float4(light_need[count_all].position, 1.0f), view_matrix).xyz;
		rec_light.dir = mul(float4(light_need[count_all].dir, 0.0f), view_matrix).xyz;
		float3 position_view = float3(0.0f, 0.0f, 0.0f);
		float3 eye_direct = normalize(position_view - position_need);
		compute_dirlight(material_need, rec_light, normal_need, eye_direct, A, D, S);
		pout.diffuse += D;
		pout.specular += S;
		count_all += 1;
	}
	for (uint i = 0; i < light_num.y; ++i)
	{
		pancy_light_basic rec_light = light_need[count_all];
		rec_light.position = mul(float4(light_need[count_all].position, 1.0f), view_matrix).xyz;
		rec_light.dir = mul(float4(light_need[count_all].dir, 0.0f), view_matrix).xyz;
		float3 position_view = float3(0.0f, 0.0f, 0.0f);
		float3 eye_direct = normalize(position_view - position_need);
		compute_pointlight(material_need, rec_light, position_need, normal_need, position_view, A, D, S);
		pout.diffuse += D;
		pout.specular += S;
		count_all += 1;
	}
	for (uint i = 0; i < light_num.z; ++i)
	{
		pancy_light_basic rec_light = light_need[count_all];
		rec_light.position = mul(float4(light_need[count_all].position, 1.0f), view_matrix).xyz;
		rec_light.dir = mul(float4(light_need[count_all].dir, 0.0f), view_matrix).xyz;
		float3 position_view = float3(0.0f, 0.0f, 0.0f);
		float3 eye_direct = normalize(position_view - position_need);
		compute_spotlight(material_need, rec_light, position_need, normal_need, eye_direct, A, D, S);
		pout.diffuse += D;
		pout.specular += S;
		count_all += 1;
	}
	return pout;
}
PixelOut_high PS_withoutshadow(VertexOut pin)
{
	PixelOut_high pout;
	pout.diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	pout.specular = float4(0.0f, 0.0f, 0.0f, 0.0f);
	//��ԭ�����������
	float4 normalspec = gNormalspecMap.Sample(samNormalDepth, pin.Tex);
	float3 normal_need = normalspec.xyz;
	float pz = gdepth_map.Sample(samNormalDepth, pin.Tex).r;
	float3 position_need = (pz / pin.ToFarPlane.z)*pin.ToFarPlane;
	pancy_material material_need;
	material_need.ambient = float4(1.0f, 1.0f, 1.0f, 0.0f);
	material_need.diffuse = float4(1.0f, 1.0f, 1.0f, 0.0f);
	material_need.specular = float4(1.0f, 1.0f, 1.0f, normalspec.a);
	int count_shadow = 0;
	//~~~~~~~~~~~~~~~~~~~���߿ռ���ɫ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//[unroll]
	float4 A = 0.0f, D = 0.0f, S = 0.0f;
	//�ȼ���̫����
	pancy_light_basic rec_sunlight = sun_light;
	float3 position_view_sun = float3(0.0f, 0.0f, 0.0f);
	float3 eye_direct_sun = normalize(position_view_sun - position_need);
	compute_dirlight(material_need, rec_sunlight, normal_need, eye_direct_sun, A, D, S);

	int count_sunlight;
	if (position_need.z < depth_devide.x)
	{
		count_sunlight = 0;
	}
	else if (position_need.z < depth_devide.y)
	{
		count_sunlight = 1;
	}
	else if (position_need.z < depth_devide.z)
	{
		count_sunlight = 2;
	}
	else
	{
		count_sunlight = 3;
	}
	float4 pos_shadow_world = mul(float4(position_need, 1.0f), invview_matrix);
	float4 pos_shadow = mul(pos_shadow_world, sunlight_shadowmat[count_sunlight]);
	float rec_shadow = CalcShadowFactor(samShadow, texture_sunshadow, count_sunlight, pos_shadow);
	pout.diffuse += D;
	pout.specular += S;
	//�ټ�����ͨ��
	int count_all = 0;
	for (uint i = 0; i < shadow_num.x; ++i)
	{
		pancy_light_basic rec_light = light_need[count_all];
		rec_light.dir = mul(float4(light_need[count_all].dir, 0.0f), view_matrix).xyz;
		float3 position_view = float3(0.0f, 0.0f, 0.0f);
		float3 eye_direct = normalize(position_view - position_need);
		compute_dirlight(material_need, rec_light, normal_need, eye_direct, A, D, S);

		float4 pos_shadow_world = mul(float4(position_need, 1.0f), invview_matrix);
		float4 pos_shadow = mul(pos_shadow_world, shadowmap_matrix[count_all]);
		float rec_shadow = CalcShadowFactor(samShadow, texture_shadow, count_all, pos_shadow);
		pout.diffuse += D;
		pout.specular += S;
		count_all += 1;
	}
	for (uint i = 0; i < shadow_num.z; ++i)
	{
		pancy_light_basic rec_light = light_need[count_all];
		rec_light.position = mul(float4(light_need[count_all].position, 1.0f), view_matrix).xyz;
		rec_light.dir = mul(float4(light_need[count_all].dir, 0.0f), view_matrix).xyz;
		float3 position_view = float3(0.0f, 0.0f, 0.0f);
		float3 eye_direct = normalize(position_view - position_need);
		compute_spotlight(material_need, rec_light, position_need, normal_need, eye_direct, A, D, S);

		float4 pos_shadow_world = mul(float4(position_need, 1.0f), invview_matrix);
		float4 pos_shadow = mul(pos_shadow_world, shadowmap_matrix[count_all]);
		float rec_shadow = CalcShadowFactor(samShadow, texture_shadow, count_all, pos_shadow);
		pout.diffuse += D;
		pout.specular += S;
		count_all += 1;
	}
	for (uint i = 0; i < light_num.x; ++i)
	{
		pancy_light_basic rec_light = light_need[count_all];
		rec_light.position = mul(float4(light_need[count_all].position, 1.0f), view_matrix).xyz;
		rec_light.dir = mul(float4(light_need[count_all].dir, 0.0f), view_matrix).xyz;
		float3 position_view = float3(0.0f, 0.0f, 0.0f);
		float3 eye_direct = normalize(position_view - position_need);
		compute_dirlight(material_need, rec_light, normal_need, eye_direct, A, D, S);
		pout.diffuse += D;
		pout.specular += S;
		count_all += 1;
	}
	for (uint i = 0; i < light_num.y; ++i)
	{
		pancy_light_basic rec_light = light_need[count_all];
		rec_light.position = mul(float4(light_need[count_all].position, 1.0f), view_matrix).xyz;
		rec_light.dir = mul(float4(light_need[count_all].dir, 0.0f), view_matrix).xyz;
		float3 position_view = float3(0.0f, 0.0f, 0.0f);
		float3 eye_direct = normalize(position_view - position_need);
		compute_pointlight(material_need, rec_light, position_need, normal_need, position_view, A, D, S);
		pout.diffuse += D;
		pout.specular += S;
		count_all += 1;
	}
	for (uint i = 0; i < light_num.z; ++i)
	{
		pancy_light_basic rec_light = light_need[count_all];
		rec_light.position = mul(float4(light_need[count_all].position, 1.0f), view_matrix).xyz;
		rec_light.dir = mul(float4(light_need[count_all].dir, 0.0f), view_matrix).xyz;
		float3 position_view = float3(0.0f, 0.0f, 0.0f);
		float3 eye_direct = normalize(position_view - position_need);
		compute_spotlight(material_need, rec_light, position_need, normal_need, eye_direct, A, D, S);
		pout.diffuse += D;
		pout.specular += S;
		count_all += 1;
	}
	return pout;
}
PixelOut_high PS_withoutshadowMSAA(VertexOut pin)
{
	PixelOut_high pout;
	pout.diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	pout.specular = float4(0.0f, 0.0f, 0.0f, 0.0f);
	//��ԭ�����������
	float4 normalspec = gNormalspecMap.Sample(samNormalDepth, pin.Tex);
	float3 normal_need = normalspec.xyz;
	float pz = gdepth_map.Sample(samNormalDepth, pin.Tex).r;
	pz = 1.0f / (pz * proj_desc.x + proj_desc.y);
	float3 position_need = (pz / pin.ToFarPlane.z)*pin.ToFarPlane;
	pancy_material material_need;
	material_need.ambient = float4(1.0f, 1.0f, 1.0f, 0.0f);
	material_need.diffuse = float4(1.0f, 1.0f, 1.0f, 0.0f);
	material_need.specular = float4(1.0f, 1.0f, 1.0f, normalspec.a);
	int count_shadow = 0;
	//~~~~~~~~~~~~~~~~~~~���߿ռ���ɫ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//[unroll]
	float4 A = 0.0f, D = 0.0f, S = 0.0f;
	//�ȼ���̫����
	pancy_light_basic rec_sunlight = sun_light;
	float3 position_view_sun = float3(0.0f, 0.0f, 0.0f);
	float3 eye_direct_sun = normalize(position_view_sun - position_need);
	compute_dirlight(material_need, rec_sunlight, normal_need, eye_direct_sun, A, D, S);

	int count_sunlight;
	if (position_need.z < depth_devide.x)
	{
		count_sunlight = 0;
	}
	else if (position_need.z < depth_devide.y)
	{
		count_sunlight = 1;
	}
	else if (position_need.z < depth_devide.z)
	{
		count_sunlight = 2;
	}
	else
	{
		count_sunlight = 3;
	}
	float4 pos_shadow_world = mul(float4(position_need, 1.0f), invview_matrix);
	float4 pos_shadow = mul(pos_shadow_world, sunlight_shadowmat[count_sunlight]);
	float rec_shadow = CalcShadowFactor(samShadow, texture_sunshadow, count_sunlight, pos_shadow);
	pout.diffuse += D;
	pout.specular += S;
	//�ټ�����ͨ��
	int count_all = 0;
	for (uint i = 0; i < shadow_num.x; ++i)
	{
		pancy_light_basic rec_light = light_need[count_all];
		rec_light.dir = mul(float4(light_need[count_all].dir, 0.0f), view_matrix).xyz;
		float3 position_view = float3(0.0f, 0.0f, 0.0f);
		float3 eye_direct = normalize(position_view - position_need);
		compute_dirlight(material_need, rec_light, normal_need, eye_direct, A, D, S);

		float4 pos_shadow_world = mul(float4(position_need, 1.0f), invview_matrix);
		float4 pos_shadow = mul(pos_shadow_world, shadowmap_matrix[count_all]);
		float rec_shadow = CalcShadowFactor(samShadow, texture_shadow, count_all, pos_shadow);
		pout.diffuse += D;
		pout.specular += S;
		count_all += 1;
	}
	for (uint i = 0; i < shadow_num.z; ++i)
	{
		pancy_light_basic rec_light = light_need[count_all];
		rec_light.position = mul(float4(light_need[count_all].position, 1.0f), view_matrix).xyz;
		rec_light.dir = mul(float4(light_need[count_all].dir, 0.0f), view_matrix).xyz;
		float3 position_view = float3(0.0f, 0.0f, 0.0f);
		float3 eye_direct = normalize(position_view - position_need);
		compute_spotlight(material_need, rec_light, position_need, normal_need, eye_direct, A, D, S);

		float4 pos_shadow_world = mul(float4(position_need, 1.0f), invview_matrix);
		float4 pos_shadow = mul(pos_shadow_world, shadowmap_matrix[count_all]);
		float rec_shadow = CalcShadowFactor(samShadow, texture_shadow, count_all, pos_shadow);
		pout.diffuse += D;
		pout.specular += S;
		count_all += 1;
	}
	for (uint i = 0; i < light_num.x; ++i)
	{
		pancy_light_basic rec_light = light_need[count_all];
		rec_light.position = mul(float4(light_need[count_all].position, 1.0f), view_matrix).xyz;
		rec_light.dir = mul(float4(light_need[count_all].dir, 0.0f), view_matrix).xyz;
		float3 position_view = float3(0.0f, 0.0f, 0.0f);
		float3 eye_direct = normalize(position_view - position_need);
		compute_dirlight(material_need, rec_light, normal_need, eye_direct, A, D, S);
		pout.diffuse += D;
		pout.specular += S;
		count_all += 1;
	}
	for (uint i = 0; i < light_num.y; ++i)
	{
		pancy_light_basic rec_light = light_need[count_all];
		rec_light.position = mul(float4(light_need[count_all].position, 1.0f), view_matrix).xyz;
		rec_light.dir = mul(float4(light_need[count_all].dir, 0.0f), view_matrix).xyz;
		float3 position_view = float3(0.0f, 0.0f, 0.0f);
		float3 eye_direct = normalize(position_view - position_need);
		compute_pointlight(material_need, rec_light, position_need, normal_need, position_view, A, D, S);
		pout.diffuse += D;
		pout.specular += S;
		count_all += 1;
	}
	for (uint i = 0; i < light_num.z; ++i)
	{
		pancy_light_basic rec_light = light_need[count_all];
		rec_light.position = mul(float4(light_need[count_all].position, 1.0f), view_matrix).xyz;
		rec_light.dir = mul(float4(light_need[count_all].dir, 0.0f), view_matrix).xyz;
		float3 position_view = float3(0.0f, 0.0f, 0.0f);
		float3 eye_direct = normalize(position_view - position_need);
		compute_spotlight(material_need, rec_light, position_need, normal_need, eye_direct, A, D, S);
		pout.diffuse += D;
		pout.specular += S;
		count_all += 1;
	}
	return pout;
}
technique11 draw_common
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS()));
	}
}
technique11 draw_withoutshadow
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_withoutshadow()));
	}
}
technique11 draw_withoutMSAA
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_withoutMSAA()));
	}
}
technique11 draw_withoutshadowMSAA
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_withoutshadowMSAA()));
	}
}