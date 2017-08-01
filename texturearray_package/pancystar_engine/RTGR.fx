cbuffer PerFrame
{
	float4x4 gViewToTexSpace;      //����3D�ؽ��������任
	float4x4 invview_matrix;       //ȡ���任����
	float4x4 view_matrix;          //ȡ���任
	float3   view_position;        //�ӵ�λ��
	float4   gFrustumCorners[4];   //3D�ؽ����ĸ��ǣ����ڽ�����դ����ֵ
	float4x4 view_matrix_cube[6];
	float3   center_position;
	float3   proj_desc;//ͶӰ���������ڻ�ԭ�����Ϣ
};
Texture2D gNormalDepthMap;
Texture2D gdepth_map;
Texture2D gcolorMap;
//Texture for pass 2
TextureCube texture_cube;
//TextureCube depth_cube;
TextureCube stencil_cube;
Texture2D ssrcolor_input;
Texture2D mask_input;
SamplerState samp
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = Clamp;
	AddressV = Clamp;
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
struct pixelOut
{
	float4 color_need;
	float4 mask_need;
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

float2 get_depth(float now_distance, float3 ray_dir, float3 position)
{
	float3 now_3D_point = position + ray_dir * now_distance;
	float3 cube_ray_now = normalize(now_3D_point - center_position);
	float2 rzAstencil = stencil_cube.Sample(samNormalDepth, cube_ray_now);
	float rz = rzAstencil.g;
	//rz = 1.0f / (9.996f - rz * 9.996f);
	rz = 1.0f / (rz * proj_desc.x + proj_desc.y);
	uint cube_stencil = round(rzAstencil.r);
	float depth_3D_point = mul(float4(now_3D_point, 1.0f), view_matrix_cube[cube_stencil]).z;
	return float2(rz, depth_3D_point);
}

void compute_light_range(
	float st_find,
	float ed_find,
	float3 ray_dir,
	float3 position,
	out float ray_st,
	out float ray_end)
{
	ray_st = st_find;
	ray_end = ed_find;
	[unroll]
	for (int i = 0; i < 10; ++i)
	{
		float length_final_rec = (ray_st + ray_end) / 2.0f;
		float2 check_depth = get_depth(length_final_rec, ray_dir, position);
		if (check_depth.x > 99999.0f || check_depth.x < check_depth.y)
		{
			//�߶���
			ray_end = length_final_rec;
		}
		else
		{
			//�߶���
			ray_st = length_final_rec;
		}
	}
}

void compute_light_step(
	float ray_st,
	float ray_end,
	float3 ray_dir,
	float3 position,
	out float end_find)
{
	float step = 1.0f / 10;
	end_find = 0;
	float now_distance;
	float2 check_depth;

	//�����ƽ�һλ
	now_distance = ray_end * step * 0.2;
	check_depth = get_depth(now_distance, ray_dir, position);
	if (check_depth.x < check_depth.y)
	{
		end_find = now_distance;
	}
	else
	{
		now_distance = ray_end * step * 0.4;
		check_depth = get_depth(now_distance, ray_dir, position);
		if (check_depth.x < check_depth.y)
		{
			end_find = now_distance;
		}
		else
		{
			now_distance = ray_end * step * 0.6;
			check_depth = get_depth(now_distance, ray_dir, position);
			if (check_depth.x < check_depth.y)
			{
				end_find = now_distance;
			}
			else
			{
				now_distance = ray_end * step * 0.8;
				check_depth = get_depth(now_distance, ray_dir, position);
				if (check_depth.x < check_depth.y)
				{
					end_find = now_distance;
				}
				else
				{
					now_distance = ray_end * step * 1.0;
					check_depth = get_depth(now_distance, ray_dir, position);
					if (check_depth.x < check_depth.y)
					{
						end_find = now_distance;
					}
					else 
					{
						now_distance = ray_end * step * 1.5;
						check_depth = get_depth(now_distance, ray_dir, position);
						if (check_depth.x < check_depth.y)
						{
							end_find = now_distance;
						}
						else
						{
							now_distance = ray_end * step * 3.0;
							check_depth = get_depth(now_distance, ray_dir, position);
							if (check_depth.x < check_depth.y)
							{
								end_find = now_distance;
							}
							else
							{
								now_distance = ray_end * step * 5.0;
								check_depth = get_depth(now_distance, ray_dir, position);
								if (check_depth.x < check_depth.y)
								{
									end_find = now_distance;
								}
								else
								{
									now_distance = ray_end * step * 7.5;
									check_depth = get_depth(now_distance, ray_dir, position);
									if (check_depth.x < check_depth.y)
									{
										end_find = now_distance;
									}
									else
									{
										now_distance = ray_end * step * 10.0;
										check_depth = get_depth(now_distance, ray_dir, position);
										if (check_depth.x < check_depth.y)
										{
											end_find = now_distance;
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

}

void compute_light_pos(
	float st_find,
	float end_find,
	float3 ray_dir,
	float3 position,
	out float now_distance,
	out float3 now_3D_point,
	out float distance_minus)
{
	[unroll]
	for (int i = 0; i < 10; ++i)
	{
		now_distance = (st_find + end_find) / 2.0f;
		float2 check_depth = get_depth(now_distance, ray_dir, position);
		if (check_depth.x < check_depth.y)
		{
			end_find = now_distance;
		}
		else
		{
			st_find = now_distance;
		}
		distance_minus = abs(check_depth.x - check_depth.y);
	}
	now_3D_point = position + ray_dir * now_distance;
}

pixelOut PS(VertexOut pin) : SV_Target
{
	pixelOut out_color;
	out_color.color_need = float4(0.0f, 0.0f, 0.0f, 0.0f);
	out_color.mask_need = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 mask_color = mask_input.Sample(samTex_liner, pin.Tex);
	clip(0.3f - mask_color.r);
	//��ԭ�����������
	float4 normalDepth = gNormalDepthMap.Sample(samNormalDepth, pin.Tex);
	float3 n = normalDepth.xyz;
	float pz = gdepth_map.Sample(samNormalDepth, pin.Tex).r;
	//pz = 0.1f / (1.0f - pz);
	float3 position = (pz / pin.ToFarPlane.z)*pin.ToFarPlane;
	float step = 1.0f / 15;
	float st_find = 0.0f, end_find = 1000.0f;
	float2 answer_sampleloc;
	//���䷴�����
	float3 ray_dir = reflect(normalize(position), n);
	float ray_st = 0.0f, ray_end = 1000.0f;
	//����̽�����߶εľ�ȷ����
	[unroll]
	for (int i = 0; i < 15; ++i)
	{
		float length_final_rec = (ray_st + ray_end) / 2.0f;
		float3 now_3D_point = position + ray_dir * length_final_rec;
		float4 now_2D_position = mul(float4(now_3D_point, 1.0f), gViewToTexSpace);
		now_2D_position /= now_2D_position.w;
		float rz = gdepth_map.SampleLevel(samNormalDepth, now_2D_position.xy, 0.0f).r;
		if (rz > 99999.0f || rz < now_3D_point.z)
		{
			//�߶���
			ray_end = length_final_rec;
		}
		else
		{
			//�߶���
			ray_st = length_final_rec;
		}
	}
	//������Ѱ�ҵ�һ���������ڵ�����

	float delta_save;
	[unroll]
	for (int i = 1; i <= 15; i++)
	{
		//�����ƽ�һλ
		float now_distance = ray_end * step * i;
		float3 now_3D_point = position + ray_dir * now_distance;
		//float3 now_3D_point = mul(float4(position + ray_dir * now_distance,1.0f), view_matrix).xyz;
		float4 now_2D_position = mul(float4(now_3D_point, 1.0f), gViewToTexSpace);
		now_2D_position /= now_2D_position.w;
		float rz = gdepth_map.SampleLevel(samNormalDepth, now_2D_position.xy, 0.0f).r;
		if (rz < now_3D_point.z)
		{
			delta_save = abs(rz - now_3D_point.z);
			end_find = now_distance;
			break;
		}
		st_find = now_distance;
	}
	//���־�ȷѰ�ҵ�һ���������ϸλ��
	float now_distance;
	float3 now_3D_point;
	[unroll]
	for (int i = 0; i < 15; ++i)
	{
		now_distance = (st_find + end_find) / 2.0f;
		//float3 now_3D_point = mul(float4(position + ray_dir * now_distance, 1.0f), view_matrix).xyz;
		now_3D_point = position + ray_dir * now_distance;
		float4 now_2D_position = mul(float4(now_3D_point, 1.0f), gViewToTexSpace);
		now_2D_position /= now_2D_position.w;
		float rz = gdepth_map.SampleLevel(samNormalDepth, now_2D_position.xy, 0.0f).r;
		answer_sampleloc = now_2D_position.xy;
		if (rz < now_3D_point.z)
		{
			end_find = now_distance;
		}
		else
		{
			st_find = now_distance;
		}
		delta_save = abs(rz - now_3D_point.z);
		if (delta_save < 0.00005f)
		{
			break;
		}
	}
	float alpha_fade = pow(saturate(((20.0f - now_distance) / 20.0f)), 4);
	float3 normal_test_sample = gNormalDepthMap.Sample(samNormalDepth, answer_sampleloc).xyz;
	float test_dot = dot(normalize(normal_test_sample), ray_dir);
	if (delta_save < 0.00005f)
	{
		float4 outputColor = gcolorMap.Sample(samTex_liner, answer_sampleloc);
		out_color.color_need = outputColor;
		out_color.color_need.a = alpha_fade;
		out_color.mask_need = float4(1.0f, 0.0f, 0.0f, 1.0f);
	}
	return out_color;
	//return gcolorMap.Sample(samTex_liner, pin.Tex);
}
float4 PS_cube(VertexOut pin) : SV_Target
{
	float4 mask_color = mask_input.Sample(samNormalDepth, pin.Tex);
	float rec_ifuse = mask_color.r;
	clip(0.8f - rec_ifuse);
	//��ԭ�����������
	float4 normalDepth = gNormalDepthMap.Sample(samNormalDepth, pin.Tex);
	float3 n = normalDepth.xyz;
	float pz = gdepth_map.Sample(samNormalDepth, pin.Tex).r;
	float3 position = (pz / pin.ToFarPlane.z)*pin.ToFarPlane;
	float st_find = 0.0f, end_find = 1000.0f;
	float2 answer_sampleloc;
	position = mul(float4(position, 1.0f), invview_matrix).xyz;
	n = mul(float4(n, 0.0f), invview_matrix).xyz;
	float3 ray_dir = reflect(normalize(position - view_position), n);
	float ray_st = 0.0f, ray_end = 1000.0f;

	compute_light_range(st_find, end_find, ray_dir, position, ray_st, ray_end);
	//������Ѱ�ҵ�һ���������ڵ�����
	compute_light_step(ray_st, ray_end, ray_dir, position, end_find);
	float now_distance;
	float3 now_3D_point;
	float distance_minus;
	compute_light_pos(st_find, end_find, ray_dir, position, now_distance, now_3D_point, distance_minus);
	float3 cube_ray2 = normalize(now_3D_point - center_position);
	//float4 cube_color = texture_cube.SampleLevel(samTex_liner, cube_ray2,0);
	float alpha_fade = pow(saturate(((20.0f - now_distance) / 20.0f)), 4);

	float4 final_color = texture_cube.SampleLevel(samTex_liner, cube_ray2, 0);
	final_color.a = alpha_fade;

	//final_color = (1.0f - rec_ifuse) * final_color + rec_ifuse * color_ssr_map;


	return final_color;
}
technique11 draw_ssrmap
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS()));
	}
	pass P1
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_cube()));
	}
}
