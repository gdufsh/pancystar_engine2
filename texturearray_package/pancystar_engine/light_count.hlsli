//�����ƹ�ṹ
struct pancy_light_basic
{
	//����ǿ��
	float4    ambient;
	float4    diffuse;
	float4    specular;
	//����λ�ã�����˥��
	float3    dir;
	float     spot;
	//�۹������
	float3    position;
	float     theta;
	//˥������Χ
	float3    decay;
	float     range;
	//��������
	uint4   type;
};
struct pancy_material
{
	float4   ambient;   //���ʵĻ����ⷴ��ϵ��
	float4   diffuse;   //���ʵ�������ϵ��
	float4   specular;  //���ʵľ��淴��ϵ��
	float4   reflect;   //���ʵķ���ϵ��
};
void compute_dirlight(
	pancy_material mat,
	pancy_light_basic light_dir,
	float3 normal,
	float3 direction_view,
	out float4 ambient,
	out float4 diffuse,
	out float4 spec)
{
	ambient = mat.ambient * light_dir.ambient;         //������
	float diffuse_angle = dot(-light_dir.dir, normal); //������н�
	if (diffuse_angle > 0.0f)
	{
		float3 v = reflect(light_dir.dir, normal);
		float spec_angle = pow(max(dot(v, direction_view), 0.0f), mat.specular.w);

		diffuse = diffuse_angle * mat.diffuse * light_dir.diffuse;//�������

		spec = spec_angle * mat.specular * light_dir.specular;    //���淴���
	}
	else
	{
		diffuse = 0.0f;//�������
		spec = 0.0f;;    //���淴���
	}
}

void compute_pointlight(
	pancy_material mat,
	pancy_light_basic light_point,
	float3 pos,
	float3 normal,
	float3 position_view,
	out float4 ambient,
	out float4 diffuse,
	out float4 spec)
{
	ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	spec = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float3 lightVec = light_point.position - pos;
	float d = length(lightVec);
	ambient = mat.ambient * light_point.ambient;         //������
	float3 eye_direct = normalize(position_view - pos);
	if (d > light_point.range)
	{
		return;
	}
	//���շ���          
	lightVec = lightVec /= d;
	//������н�
	float diffuse_angle = dot(lightVec, normal);
	//ֱ��˥��Ч��
	float4 distance_need;
	distance_need = float4(1.0f, d, d*d, 0.0f);
	float decay_final = 1.0 / dot(distance_need, float4(light_point.decay, 0.0f));
	//���淴��
	if (diffuse_angle > 0.0f)
	{
		float3 v = reflect(-lightVec, normal);
		float spec_angle = pow(max(dot(v, eye_direct), 0.0f), mat.specular.w);
		diffuse = decay_final * diffuse_angle * mat.diffuse * light_point.diffuse;//�������
		spec = decay_final * spec_angle * mat.specular * light_point.specular;    //���淴���
	}
}

void compute_spotlight(
	pancy_material mat,
	pancy_light_basic light_spot,
	float3 pos,
	float3 normal,
	float3 direction_view,
	out float4 ambient,
	out float4 diffuse,
	out float4 spec)
{
	ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	spec = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float3 lightVec = light_spot.position - pos;
	float d = length(lightVec);
	//���շ���
	lightVec /= d;
	light_spot.dir = normalize(light_spot.dir);
	ambient = mat.ambient * light_spot.ambient;//������
	float tmp = -dot(lightVec, light_spot.dir);//������������������ļн�
	if (tmp < cos(light_spot.theta))//�۹�Ʒ���֮��
	{
		return;
	}
	if (d > light_spot.range)//�۹�Ʒ�Χ֮��
	{
		return;
	}
	//������н�
	float diffuse_angle = dot(lightVec, normal);
	//ֱ��˥��Ч��
	float4 distance_need;
	distance_need = float4(1.0f, d, d*d, 0.0f);
	float decay_final = 1.0 / dot(distance_need, float4(light_spot.decay, 0.0f));
	//����˥��Ч��
	float decay_spot = pow(tmp, light_spot.spot);
	//���淴��
	if (diffuse_angle > 0.0f)
	{
		float3 v = reflect(-lightVec, normal);
		float spec_angle = pow(max(dot(v, direction_view), 0.0f), mat.specular.w);
		diffuse = decay_spot*decay_final * diffuse_angle * mat.diffuse * light_spot.diffuse;//�������
		spec = decay_spot*decay_final * spec_angle * mat.specular * light_spot.specular;    //���淴���
	}
}