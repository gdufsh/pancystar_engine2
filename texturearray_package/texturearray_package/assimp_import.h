#pragma once
#include"pancystar_engine_basic.h"
#include"pancy_d3d11_basic.h"
#include"geometry.h"
#include"shader_pancy.h"
#include <assimp/Importer.hpp>      // �������ڸ�ͷ�ļ��ж���
#include <assimp/scene.h>           // ��ȡ����ģ�����ݶ�����scene��
#include <assimp/postprocess.h>     // ��ͷ�ļ��а�������ı�־λ����
#include <assimp/matrix4x4.h>
#include <assimp/matrix3x3.h>


struct material_list
{
	char                       texture_diffuse[512];       //�����������ַ
	char                       texture_normal[512];        //������ͼ�����ַ
	char                       texture_specular[512];      //������ͼ�����ַ
	ID3D11ShaderResourceView   *tex_diffuse_resource;      //����������
	ID3D11ShaderResourceView   *texture_normal_resource;   //������ͼ����
	ID3D11ShaderResourceView   *texture_specular_resource; //�߹���ͼ����
	material_list()
	{
		texture_diffuse[0] = '\0';
		texture_normal[0] = '\0';
		texture_specular[0] = '\0';
		tex_diffuse_resource = NULL;
		texture_normal_resource = NULL;
		texture_specular_resource = NULL;
	}
};
struct meshview_list
{
	Geometry_basic *point_buffer;
	int material_use;
	meshview_list()
	{
		point_buffer = NULL;
		material_use = 0;
	}
};
class assimp_basic
{
protected:
	meshview_list *mesh_need;        //�����
	ID3DX11EffectTechnique *teque_pancy;       //����·��
	std::string filename;        //ģ���ļ���
	char rec_texpath[128];       //����·��
	Assimp::Importer importer;   //ģ�͵�����
	const aiScene *model_need;   //ģ�ʹ洢��
	material_list *matlist_need; //���ʱ�
	int material_optimization;
	int mesh_optimization;
	std::vector<std::string> meshpart_name;
public:
	assimp_basic(const char* filename, const char* texture_path);
	engine_basic::engine_fail_reason model_create(bool if_adj,int alpha_partnum, int* alpha_part);
	int get_meshnum();
	int get_texnum() { return material_optimization; };
	virtual void get_texture(material_list *texture_need, int i);
	virtual void get_texture_byindex(material_list *texture_need, int index);
	bool check_if_anim();
	virtual void release();
	virtual void draw_part(int i);
	std::string get_mesh_name_bypart(int mesh_id);
	HRESULT get_technique(ID3DX11EffectTechnique *teque_need);
protected:
	virtual engine_basic::engine_fail_reason init_mesh(bool if_adj) = 0;
	engine_basic::engine_fail_reason init_texture();
	void remove_texture_path(char rec[]);
	void change_texturedesc_2dds(char rec[]);
};

template<typename T>
class model_reader_assimp : public assimp_basic
{
protected:
	T *point_pack_list;
	UINT *index_pack_list;
	int vertex_final_num;
	int index_pack_num;
public:
	model_reader_assimp(const char* filename, const char* texture_path);
	void get_model_pack_num(int &vertex_num,int &index_num);
	void get_model_pack_data(T *point_data, UINT *index_data);
protected:
	virtual engine_basic::engine_fail_reason init_mesh(bool if_adj);
};
template<typename T>
model_reader_assimp<T>::model_reader_assimp(const char* pFile, const char *texture_path) : assimp_basic(pFile, texture_path)
{
	point_pack_list = NULL;
	index_pack_list = NULL;
	vertex_final_num = 0;
	index_pack_num = 0;
}
template<typename T>
engine_basic::engine_fail_reason model_reader_assimp<T>::init_mesh(bool if_adj)
{

	for (int i = 0; i < model_need->mNumMeshes; i++)
	{
		//��ȡģ�͵ĵ�i��ģ��
		const aiMesh* paiMesh = model_need->mMeshes[i];
		std::string data_name = paiMesh->mName.C_Str();
		int count = 0;
		for (int i = 0; i < data_name.size(); ++i) 
		{
			if (data_name[i] == ' ') 
			{
				count = i+1;
			}
		}
		std::string final_name;
		for (int i = count; i < data_name.size(); ++i)
		{
			final_name += data_name[i];
		}
		meshpart_name.push_back(final_name);
		vertex_final_num += paiMesh->mNumVertices;
		index_pack_num += paiMesh->mNumFaces * 3;
	}
	point_pack_list = (T*)malloc(vertex_final_num * sizeof(T));
	index_pack_list = (unsigned int*)malloc(index_pack_num * sizeof(unsigned int));
	int count_point_pack = 0;
	int count_index_pack = 0;
	T *point_need;
	unsigned int *index_need;
	//���������¼��
	mesh_need = new meshview_list[model_need->mNumMeshes];
	mesh_optimization = model_need->mNumMeshes;
	int now_index_start = count_point_pack;
	for (int i = 0; i < model_need->mNumMeshes; i++)
	{
		//��ȡģ�͵ĵ�i��ģ��
		const aiMesh* paiMesh = model_need->mMeshes[i];
		//��ȡģ�͵Ĳ��ʱ��
		mesh_need[i].material_use = paiMesh->mMaterialIndex;
		point_need = (T*)malloc(paiMesh->mNumVertices * sizeof(T));
		index_need = (unsigned int*)malloc(paiMesh->mNumFaces * 3 * sizeof(unsigned int));
		//���㻺��
		for (unsigned int j = 0; j < paiMesh->mNumVertices; j++)
		{
			point_need[j].position.x = paiMesh->mVertices[j].x;
			point_need[j].position.y = paiMesh->mVertices[j].y;
			point_need[j].position.z = paiMesh->mVertices[j].z;

			point_need[j].normal.x = paiMesh->mNormals[j].x;
			point_need[j].normal.y = paiMesh->mNormals[j].y;
			point_need[j].normal.z = paiMesh->mNormals[j].z;

			if (paiMesh->HasTextureCoords(0))
			{
				point_need[j].tex.x = paiMesh->mTextureCoords[0][j].x;
				point_need[j].tex.y = 1 - paiMesh->mTextureCoords[0][j].y;
			}
			else
			{
				point_need[j].tex.x = 0.0f;
				point_need[j].tex.y = 0.0f;
			}
			if (paiMesh->mTangents != NULL)
			{
				point_need[j].tangent.x = paiMesh->mTangents[j].x;
				point_need[j].tangent.y = paiMesh->mTangents[j].y;
				point_need[j].tangent.z = paiMesh->mTangents[j].z;
			}
			else
			{
				point_need[j].tangent.x = 0.0f;
				point_need[j].tangent.y = 0.0f;
				point_need[j].tangent.z = 0.0f;
			}
			point_pack_list[count_point_pack] = point_need[j];
			point_pack_list[count_point_pack].tex_id.x = i;
			count_point_pack += 1;
		}
		//����������
		int count_index = 0;
		for (unsigned int j = 0; j < paiMesh->mNumFaces; j++)
		{
			if (paiMesh->mFaces[j].mNumIndices == 3)
			{
				index_need[count_index++] = paiMesh->mFaces[j].mIndices[0];
				index_need[count_index++] = paiMesh->mFaces[j].mIndices[1];
				index_need[count_index++] = paiMesh->mFaces[j].mIndices[2];

				index_pack_list[count_index_pack++] = paiMesh->mFaces[j].mIndices[0] + now_index_start;
				index_pack_list[count_index_pack++] = paiMesh->mFaces[j].mIndices[1] + now_index_start;
				index_pack_list[count_index_pack++] = paiMesh->mFaces[j].mIndices[2] + now_index_start;
			}
			else
			{
				engine_basic::engine_fail_reason fail_message("model" + filename + "find no triangle face");
				return fail_message;
			}
		}
		//ģ�͵ĵ�i��ģ��Ķ��㼰������Ϣ
		mesh_need[i].point_buffer = new mesh_model<T>(point_need, index_need,paiMesh->mNumVertices, paiMesh->mNumFaces * 3,if_adj);
		//�����ڴ���Ϣ�����Դ���
		engine_basic::engine_fail_reason check_fail = mesh_need[i].point_buffer->create_object();
		if (!check_fail.check_if_failed())
		{
			return check_fail;
		}
		now_index_start = count_point_pack;
		//�ͷ��ڴ�
		free(point_need);
		point_need = NULL;
		free(index_need);
		index_need = NULL;
	}
	engine_basic::engine_fail_reason succeed;
	return succeed;
}

template<typename T>
void model_reader_assimp<T>::get_model_pack_num(int &vertex_num, int &index_num)
{
	vertex_num = vertex_final_num;
	index_num = index_pack_num;
}
template<typename T>
void model_reader_assimp<T>::get_model_pack_data(T *point_data, UINT *index_data)
{
	for (int i = 0; i < vertex_final_num; ++i) 
	{
		point_data[i] = point_pack_list[i];
	}
	for (int i = 0; i < index_pack_num; ++i)
	{
		index_data[i] = index_pack_list[i];
	}
}


struct skin_tree
{
	char bone_ID[128];
	int bone_number;
	XMFLOAT4X4 basic_matrix;
	XMFLOAT4X4 animation_matrix;
	XMFLOAT4X4 now_matrix;
	skin_tree *brother;
	skin_tree *son;
	skin_tree()
	{
		bone_ID[0] = '\0';
		bone_number = -1;
		brother = NULL;
		son = NULL;
		XMStoreFloat4x4(&basic_matrix, XMMatrixIdentity());
		XMStoreFloat4x4(&animation_matrix, XMMatrixIdentity());
		XMStoreFloat4x4(&now_matrix, XMMatrixIdentity());
	}
};
//�任����
struct vector_animation
{
	float time;               //֡ʱ��
	float main_key[3];        //֡����
};
//�任��Ԫ��
struct quaternion_animation
{
	float time;               //֡ʱ��
	float main_key[4];        //֡����
};
//�任����
struct matrix_animation
{
	float time;               //֡ʱ��
	float main_key[16];       //֡����
};
struct animation_data
{
	char bone_name[128];                                //���α任���ݶ�Ӧ�Ĺ�������
	skin_tree *bone_point;                              //���α任���ݶ�Ӧ�Ĺ�����ָ��

	DWORD number_translation;                           //ƽ�Ʊ任������
	vector_animation *translation_key;                  //����ƽ�Ʊ任����

	DWORD number_scaling;                               //�����任������
	vector_animation *scaling_key;                      //���������任����

	DWORD number_rotation;                              //��ת�任������
	quaternion_animation *rotation_key;                 //������ת�任������

	DWORD number_transform;                             //��ϱ任������
	matrix_animation *transform_key;                    //������ϱ任������	

	struct animation_data *next;                        //��һ���任����
};
struct animation_set
{
	char  animation_name[128];                          //�ö���������
	float animation_length;                             //�����ĳ���
	DWORD number_animation;                             //���������ı任����
	animation_data *head_animition;                     //�ö���������
	animation_set *next;                                //ָ����һ��������ָ��
};
class model_reader_skin :public model_reader_assimp<point_skincommon> 
{
	skin_tree *root_skin;
	animation_set *first_animation;
	float time_all;
	int bone_num;
	XMFLOAT4X4 bone_matrix_array[100];
	XMFLOAT4X4 offset_matrix_array[100];
	XMFLOAT4X4 final_matrix_array[100];
	int tree_node_num[100][100];
	int hand_matrix_number;
public:
	model_reader_skin(const char* filename, const char* texture_path);
	void update_root(skin_tree *root, XMFLOAT4X4 matrix_parent);
	void update_mesh_offset(int i);
	void update_mesh_offset();
	void update_animation(float delta_time);
	void specify_animation_time(float animation_time);
	XMFLOAT4X4* get_bone_matrix(int i, int &num_bone);
	XMFLOAT4X4* get_bone_matrix();
	XMFLOAT4X4 get_hand_matrix();
	XMFLOAT4X4* get_offset_mat() { return offset_matrix_array; };
	skin_tree* get_bone_tree() { return root_skin; };
	animation_set* get_animation_data() { return first_animation; };
	float get_animation_length() { return first_animation->animation_length; };
	int get_bone_num() { return bone_num; };
	void release_all();
private:
	engine_basic::engine_fail_reason init_mesh(bool if_adj);
	aiNode *find_skinroot(aiNode *now_node, char root_name[]);
	HRESULT build_skintree(aiNode *now_node, skin_tree *now_root);
	HRESULT build_animation_list();
	bool check_ifsame(char a[], char b[]);
	void set_matrix(XMFLOAT4X4 &out, aiMatrix4x4 *in);
	skin_tree* find_tree(skin_tree* p, char name[]);
	skin_tree* find_tree(skin_tree* p, int num);
	void free_tree(skin_tree *now);
	void update_anim_data(animation_data *now);
	void find_anim_sted(int &st, int &ed, quaternion_animation *input, int num_animation);
	void find_anim_sted(int &st, int &ed, vector_animation *input, int num_animation);
	void Interpolate(quaternion_animation& pOut, quaternion_animation pStart, quaternion_animation pEnd, float pFactor);
	void Interpolate(vector_animation& pOut, vector_animation pStart, vector_animation pEnd, float pFactor);
	void Get_quatMatrix(XMFLOAT4X4 &resMatrix, quaternion_animation& pOut);
	int find_min(float x1, float x2, float x3, float x4);
};