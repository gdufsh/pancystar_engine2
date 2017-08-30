Texture2D<float4>   input_tex;
uint4                input_range;
RWStructuredBuffer<float> input_buffer;
RWStructuredBuffer<float> output_buffer;
#include"HDR_define.hlsli"
//��һ�鴦������δ�����ͼ�񣬻�ȡ���������ݣ����²�����1/16��buffer
[numthreads(16, 16, 1)]
void main_first(uint3 DTid : SV_DispatchThreadID)
{
	float delta_ln = 0.001f;
	float final_lum = 0.0f;
	uint2 basic_pos;
	basic_pos.x = DTid.x * 4;
	basic_pos.y = DTid.y * 4;
	[unroll]
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			uint2 now_position = basic_pos + uint2(i, j);
			if (now_position.x < input_range.x && now_position.y < input_range.y)
			{
				//�����õ���������
				float4 input_texcolor = input_tex.Load(uint3(now_position, 0));
				//�������ǰ��������Ȼ����
				float lum = saturate(input_texcolor.r) *0.299f + saturate(input_texcolor.g)*0.587f + saturate(input_texcolor.b)*0.114f;
				//�Ӻ���������
				final_lum += log(lum + delta_ln) / (input_range.x*input_range.y);
			}
		}
	}
	input_buffer[DTid.x * input_range.w + DTid.y] = final_lum;
}
//�ڶ��鴦�����²����õ������ݽ��н�һ����ѹ��
groupshared float accum[256];
[numthreads(256, 1, 1)]
void main_comman(uint3 Gid : SV_GroupID, uint3 DTid : SV_DispatchThreadID, uint3 GTid : SV_GroupThreadID, uint GI : SV_GroupIndex)
{
	if (DTid.x < input_range.z)
		accum[GI] = input_buffer[DTid.x];
	else
		accum[GI] = 0;
	GroupMemoryBarrierWithGroupSync();
	if (GI < 128)
		accum[GI] += accum[128 + GI];

	GroupMemoryBarrierWithGroupSync();
	if (GI < 64)
		accum[GI] += accum[64 + GI];

	GroupMemoryBarrierWithGroupSync();
	if (GI < 32)
		accum[GI] += accum[32 + GI];

	GroupMemoryBarrierWithGroupSync();
	if (GI < 16)
		accum[GI] += accum[16 + GI];

	GroupMemoryBarrierWithGroupSync();
	if (GI < 8)
		accum[GI] += accum[8 + GI];

	GroupMemoryBarrierWithGroupSync();
	if (GI < 4)
		accum[GI] += accum[4 + GI];

	GroupMemoryBarrierWithGroupSync();
	if (GI < 2)
		accum[GI] += accum[2 + GI];

	GroupMemoryBarrierWithGroupSync();
	if (GI < 1)
		accum[GI] += accum[1 + GI];

	if (GI == 0)
	{
		output_buffer[Gid.x] = accum[0];
	}
	//output_buffer[DTid.x] = final_lum;
}
//���һ�����������map����
[numthreads(256, 1, 1)]
void main_final(uint3 Gid : SV_GroupID, uint3 DTid : SV_DispatchThreadID, uint3 GTid : SV_GroupThreadID, uint GI : SV_GroupIndex)
{

	if (DTid.x < (input_range.z / 256))
		accum[GI] = output_buffer[DTid.x];
	else
		accum[GI] = 0;
	GroupMemoryBarrierWithGroupSync();
	if (GI < 128)
		accum[GI] += accum[128 + GI];

	GroupMemoryBarrierWithGroupSync();
	if (GI < 64)
		accum[GI] += accum[64 + GI];

	GroupMemoryBarrierWithGroupSync();
	if (GI < 32)
		accum[GI] += accum[32 + GI];

	GroupMemoryBarrierWithGroupSync();
	if (GI < 16)
		accum[GI] += accum[16 + GI];

	GroupMemoryBarrierWithGroupSync();
	if (GI < 8)
		accum[GI] += accum[8 + GI];

	GroupMemoryBarrierWithGroupSync();
	if (GI < 4)
		accum[GI] += accum[4 + GI];

	GroupMemoryBarrierWithGroupSync();
	if (GI < 2)
		accum[GI] += accum[2 + GI];

	GroupMemoryBarrierWithGroupSync();
	if (GI < 1)
		accum[GI] += accum[1 + GI];

	if (GI == 0)
	{
		input_buffer[Gid.x] = accum[0];
	}
}
technique11 HDR_average_pass
{
	pass P0
	{
		SetVertexShader(NULL);
		SetPixelShader(NULL);
		SetGeometryShader(NULL);
		SetComputeShader(CompileShader(cs_5_0, main_first()));
	}
	pass P1
	{
		SetVertexShader(NULL);
		SetPixelShader(NULL);
		SetGeometryShader(NULL);
		SetComputeShader(CompileShader(cs_5_0, main_comman()));
	}
	pass P2
	{
		SetVertexShader(NULL);
		SetPixelShader(NULL);
		SetGeometryShader(NULL);
		SetComputeShader(CompileShader(cs_5_0, main_final()));
	}
}