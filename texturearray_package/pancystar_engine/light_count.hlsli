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
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~phong�����·�����brdf~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
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
	float3 light_dir_rec = normalize(light_dir.dir);
	float diffuse_angle = dot(-light_dir_rec, normal); //������н�
	if (diffuse_angle > 0.0f)
	{
		float3 v = normalize(reflect(light_dir_rec, normal));
		float spec_angle = pow(max(dot(v, direction_view), 0.0f), 10);

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
		float spec_angle = pow(max(dot(v, direction_view), 0.0f), 10.0f);
		diffuse = decay_spot*decay_final * diffuse_angle * mat.diffuse * light_spot.diffuse;//�������
		spec = decay_spot*decay_final * spec_angle * mat.specular * light_spot.specular;    //���淴���
	}
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~phong������΢����brdf~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//����΢����brdf
void count_brdf_reflect(
	float3 specular_F0,
	float  tex_matallic,
	float  tex_roughness,
	float3 light_dir_in,
	float3 normal,
	float3 direction_view,
	float diffuse_angle,
	out float  diffuse_out,
	out float3 specular_out
	)
{
	float3 h_vec = (light_dir_in + direction_view) / 2.0f;
	diffuse_out = (1 - tex_matallic);
	float pi = 3.141592653;
	float view_angle = dot(direction_view, normal);//���߼н�
	float cos_vh = dot(direction_view, h_vec);
	//��������
	float3 fresnel = specular_F0 + (float4(1.0f, 1.0f, 1.0f, 1.0f) - specular_F0)*(1.0f - pow(cos_vh, 5.0f));
	//NDF����������
	float alpha = tex_roughness * tex_roughness;
	float nh_mul = dot(normal, h_vec);
	float divide_ndf1 = nh_mul*nh_mul * (alpha * alpha - 1.0f) + 1.0f;
	float divide_ndf2 = pi * divide_ndf1 *divide_ndf1;
	float ndf = (alpha*alpha) / divide_ndf2;
	//GGX�ڵ���
	float ggx_k = (tex_roughness + 1.0f) * (tex_roughness + 1.0f) / 8.0f;
	float ggx_v = view_angle / (view_angle*(1 - ggx_k) + ggx_k);
	float ggx_l = diffuse_angle / (diffuse_angle*(1 - ggx_k) + ggx_k);
	float ggx = ggx_v * ggx_l;
	//���յľ��淴����
	specular_out = (fresnel * ndf * ggx) / (4 * view_angle * diffuse_angle);
}
//�����pbr
void compute_dirlight_pbr(
	float3 specular_F0,
	float  tex_matallic,
	float  tex_roughness,
	pancy_light_basic light_dir,
	float3 normal,
	float3 direction_view,
	out float4 diffuse,
	out float4 spec)
{
	float3 light_dir_rec = normalize(light_dir.dir);
	float diffuse_angle = dot(-light_dir_rec, normal); //������н�
	if (diffuse_angle > 0.0f)
	{
		//����brdf
		float mat_diffuse;
		float3 mat_specular;
		float diffuse_angle = dot(-light_dir.dir, normal); //������н�
		count_brdf_reflect(specular_F0, tex_matallic, tex_roughness, -light_dir_rec, normal, direction_view, diffuse_angle, mat_diffuse, mat_specular);
		//���㷴���
		diffuse = diffuse_angle * mat_diffuse * light_dir.diffuse;//�������
		spec = diffuse_angle * float4(mat_specular, 1.0f) * light_dir.specular;    //���淴���
	}
	else
	{
		diffuse = 0.0f;//�������
		spec = 0.0f;;    //���淴���
	}
}
//���Դpbr
void compute_pointlight_pbr(
	float3 specular_F0,
	float  tex_matallic,
	float  tex_roughness,
	pancy_light_basic light_point,
	float3 pos,
	float3 normal,
	float3 position_view,
	out float4 diffuse,
	out float4 spec)
{
	diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	spec = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float3 lightVec = light_point.position - pos;
	float d = length(lightVec);
	float3 eye_direct = normalize(position_view - pos);
	if (d > light_point.range)
	{
		return;
	}
	//���շ���          
	lightVec = lightVec /= d;
	//������н�
	float diffuse_angle = dot(lightVec, normal);
	//���淴��
	if (diffuse_angle > 0.0f)
	{
		//ֱ��˥��Ч��
		float4 distance_need;
		distance_need = float4(1.0f, d, d*d, 0.0f);
		float decay_final = 1.0 / dot(distance_need, float4(light_point.decay, 0.0f));
		//����brdf
		float mat_diffuse;
		float3 mat_specular;
		count_brdf_reflect(specular_F0, tex_matallic, tex_roughness, lightVec, normal, eye_direct, diffuse_angle, mat_diffuse, mat_specular);
		//���㷴���
		diffuse = decay_final * diffuse_angle * mat_diffuse * light_point.diffuse;//�������
		spec = decay_final * diffuse_angle * float4(mat_specular, 1.0f) * light_point.specular;    //���淴���
	}
}
//�۹��pbr
void compute_spotlight_pbr(
	float3 specular_F0,
	float  tex_matallic,
	float  tex_roughness,
	pancy_light_basic light_spot,
	float3 pos,
	float3 normal,
	float3 direction_view,
	out float4 diffuse,
	out float4 spec)
{
	diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	spec = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float3 lightVec = light_spot.position - pos;
	float d = length(lightVec);
	//���շ���
	lightVec /= d;
	light_spot.dir = normalize(light_spot.dir);
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
	//���淴��
	if (diffuse_angle > 0.0f)
	{
		//ֱ��˥��Ч��
		float4 distance_need;
		distance_need = float4(1.0f, d, d*d, 0.0f);
		float decay_final = 1.0 / dot(distance_need, float4(light_spot.decay, 0.0f));
		//����˥��Ч��
		float decay_spot = pow(tmp, light_spot.spot);
		//����brdf
		float mat_diffuse;
		float3 mat_specular;
		count_brdf_reflect(specular_F0, tex_matallic, tex_roughness, lightVec, normal, direction_view, diffuse_angle, mat_diffuse, mat_specular);
		//���㷴���
		diffuse = decay_spot * decay_final * diffuse_angle * mat_diffuse * light_spot.diffuse;//�������
		spec = decay_spot * decay_final * diffuse_angle * float4(mat_specular, 1.0f) * light_spot.specular;    //���淴���
	}
}