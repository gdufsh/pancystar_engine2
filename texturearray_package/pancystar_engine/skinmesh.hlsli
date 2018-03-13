#define MAX_BONE_NUM 100
struct Vertex_IN_bone
{
	float3	pos 	: POSITION;     //����λ��
	float3	normal 	: NORMAL;       //���㷨����
	float3	tangent : TANGENT;      //����������
	uint4   texid   : TEXINDICES;   //��������
	float4  tex1    : TEXDIFFNORM;  //������������(�����估��������)
	float4  tex2    : TEXOTHER;     //������������(��������)
	uint4   bone_id     : BONEINDICES;  //����ID��
	float4  bone_weight : WEIGHTS;      //����Ȩ��
};
struct Vertex_IN_bone_instance
{
	float3	pos 	: POSITION;     //����λ��
	float3	normal 	: NORMAL;       //���㷨����
	float3	tangent : TANGENT;      //����������
	uint4   texid   : TEXINDICES;   //��������
	float4  tex1    : TEXDIFFNORM;  //������������(�����估��������)
	float4  tex2    : TEXOTHER;     //������������(��������)
	uint4   bone_id     : BONEINDICES;  //����ID��
	float4  bone_weight : WEIGHTS;      //����Ȩ��
	uint InstanceId : SV_InstanceID;//instace������
};
void count_skin_mesh(
	in Vertex_IN_bone vin,
	in float4x4 gBoneTransforms[100],
	out float3 posL,
	out float3 normalL,
	out float3 tangentL
	)
{
	posL = float3(0.0f, 0.0f, 0.0f);
	normalL = float3(0.0f, 0.0f, 0.0f);
	tangentL = float3(0.0f, 0.0f, 0.0f);
	float weights[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	weights[0] = vin.bone_weight.x;
	weights[1] = vin.bone_weight.y;
	weights[2] = vin.bone_weight.z;
	weights[3] = vin.bone_weight.w;
	for (int i = 0; i < 4; ++i)
	{
		// �����任һ�㲻���ڲ������ŵ���������Կ��Բ������ߵ���ת�ò���
		posL += weights[i] * mul(float4(vin.pos, 1.0f), gBoneTransforms[vin.bone_id[i]]).xyz;
		normalL += weights[i] * mul(vin.normal, (float3x3)gBoneTransforms[vin.bone_id[i]]);
		tangentL += weights[i] * mul(vin.tangent.xyz, (float3x3)gBoneTransforms[vin.bone_id[i]]);
	}
}
void count_skin_mesh(
	in Vertex_IN_bone_instance vin,
	in StructuredBuffer<float4x4> input_buffer,
	in uint bone_num,
	out float3 posL,
	out float3 normalL,
	out float3 tangentL
	) 
{
	posL = float3(0.0f, 0.0f, 0.0f);
	normalL = float3(0.0f, 0.0f, 0.0f);
	tangentL = float3(0.0f, 0.0f, 0.0f);
	float weights[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	weights[0] = vin.bone_weight.x;
	weights[1] = vin.bone_weight.y;
	weights[2] = vin.bone_weight.z;
	weights[3] = vin.bone_weight.w;
	for (int i = 0; i < 4; ++i)
	{
		// �����任һ�㲻���ڲ������ŵ���������Կ��Բ������ߵ���ת�ò���
		int instance_bone = vin.InstanceId * bone_num + vin.bone_id[i];
		posL += weights[i] * mul(float4(vin.pos, 1.0f), input_buffer[instance_bone]).xyz;
		normalL += weights[i] * mul(vin.normal, (float3x3)input_buffer[instance_bone]);
		tangentL += weights[i] * mul(vin.tangent.xyz, (float3x3)input_buffer[instance_bone]);
	}
}