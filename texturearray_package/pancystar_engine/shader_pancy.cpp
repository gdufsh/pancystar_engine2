#include"shader_pancy.h"
bool shader_basic::WCharToMByte(LPCWSTR lpcwszStr, std::string &str)
{
	DWORD dwMinSize = 0;
	LPSTR lpszStr = NULL;
	dwMinSize = WideCharToMultiByte(CP_OEMCP, NULL, lpcwszStr, -1, NULL, 0, NULL, FALSE);
	if (0 == dwMinSize)
	{
		return FALSE;
	}
	lpszStr = new char[dwMinSize];
	WideCharToMultiByte(CP_OEMCP, NULL, lpcwszStr, -1, lpszStr, dwMinSize, NULL, FALSE);
	str = lpszStr;
	delete[] lpszStr;
	lpszStr = NULL;
	return TRUE;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~��������ɫ�����벿��~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
shader_basic::shader_basic(LPCWSTR filename)
{
	fx_need = NULL;
	shader_filename = filename;
	WCharToMByte(filename, shader_file_string);
}
engine_basic::engine_fail_reason shader_basic::shder_create()
{
	engine_basic::engine_fail_reason check_error = combile_shader(shader_filename);
	if (!check_error.check_if_failed())
	{
		return check_error;
	}
	init_handle();
	engine_basic::engine_fail_reason succeed;
	return succeed;
}
void shader_basic::release_basic()
{
	safe_release(fx_need);
}
engine_basic::engine_fail_reason shader_basic::get_technique(ID3DX11EffectTechnique** tech_need, LPCSTR tech_name)
{
	D3D11_INPUT_ELEMENT_DESC member_point[30];
	UINT num_member;
	set_inputpoint_desc(member_point, &num_member);
	*tech_need = fx_need->GetTechniqueByName(tech_name);
	D3DX11_PASS_DESC pass_shade;
	HRESULT hr;
	hr = (*tech_need)->GetPassByIndex(0)->GetDesc(&pass_shade);
	if (FAILED(hr))
	{
		engine_basic::engine_fail_reason failed_message("get technique" + std::string(tech_name) + "error in" + shader_file_string);
		return failed_message;
	}
	hr = d3d_pancy_basic_singleton::GetInstance()->get_d3d11_device()->CreateInputLayout(member_point, num_member, pass_shade.pIAInputSignature, pass_shade.IAInputSignatureSize, &input_need);
	d3d_pancy_basic_singleton::GetInstance()->get_d3d11_contex()->IASetInputLayout(input_need);
	input_need->Release();
	input_need = NULL;
	engine_basic::engine_fail_reason succeed;
	return succeed;
}
engine_basic::engine_fail_reason shader_basic::get_technique(D3D11_INPUT_ELEMENT_DESC member_point[], UINT num_member, ID3DX11EffectTechnique** tech_need, LPCSTR tech_name)
{
	*tech_need = fx_need->GetTechniqueByName(tech_name);
	D3DX11_PASS_DESC pass_shade;
	HRESULT hr2;
	hr2 = (*tech_need)->GetPassByIndex(0)->GetDesc(&pass_shade);
	HRESULT hr = d3d_pancy_basic_singleton::GetInstance()->get_d3d11_device()->CreateInputLayout(member_point, num_member, pass_shade.pIAInputSignature, pass_shade.IAInputSignatureSize, &input_need);
	if (FAILED(hr))
	{
		engine_basic::engine_fail_reason failed_message( "get technique" + std::string(tech_name) + "error in"+ shader_file_string);
		return failed_message;
	}
	d3d_pancy_basic_singleton::GetInstance()->get_d3d11_contex()->IASetInputLayout(input_need);
	input_need->Release();
	input_need = NULL;
	engine_basic::engine_fail_reason succeed;
	return succeed;
}
engine_basic::engine_fail_reason shader_basic::combile_shader(LPCWSTR filename)
{
	//����shader
	UINT flag_need(0);
	flag_need |= D3D10_SHADER_SKIP_OPTIMIZATION;
#if defined(DEBUG) || defined(_DEBUG)
	flag_need |= D3D10_SHADER_DEBUG;
#endif
	//����ID3D10Blob������ű���õ�shader��������Ϣ
	ID3D10Blob	*shader(NULL);
	ID3D10Blob	*errMsg(NULL);
	//����effect
	std::ifstream fin(filename, std::ios::binary);
	if (fin.fail())
	{
		engine_basic::engine_fail_reason failed_message("open shader file"+ shader_file_string + "error");
		return failed_message;
	}
	fin.seekg(0, std::ios_base::end);
	int size = (int)fin.tellg();
	fin.seekg(0, std::ios_base::beg);
	std::vector<char> compiledShader(size);
	fin.read(&compiledShader[0], size);
	fin.close();
	HRESULT hr = D3DX11CreateEffectFromMemory(&compiledShader[0], size, 0, d3d_pancy_basic_singleton::GetInstance()->get_d3d11_device(), &fx_need);
	if (FAILED(hr))
	{
		engine_basic::engine_fail_reason failed_message("Create Effect From" + shader_file_string + "error");
		return failed_message;
	}
	safe_release(shader);
	//�������붥���ʽ
	engine_basic::engine_fail_reason succeed;
	return succeed;
}
engine_basic::engine_fail_reason shader_basic::set_matrix(ID3DX11EffectMatrixVariable *mat_handle, XMFLOAT4X4 *mat_need)
{
	XMMATRIX rec_mat = XMLoadFloat4x4(mat_need);
	HRESULT hr;
	hr = mat_handle->SetMatrix(reinterpret_cast<float*>(&rec_mat));
	if (FAILED(hr)) 
	{
		engine_basic::engine_fail_reason failed_message("set matrix error in" + shader_file_string);
		return failed_message;
	}
	engine_basic::engine_fail_reason succeed;
	return succeed;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~��ɫ����~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
color_shader::color_shader(LPCWSTR filename) : shader_basic(filename)
{
}
void color_shader::init_handle()
{
	project_matrix_handle = fx_need->GetVariableByName("final_matrix")->AsMatrix();
}
engine_basic::engine_fail_reason color_shader::set_trans_all(XMFLOAT4X4 *mat_final)
{
	engine_basic::engine_fail_reason check_fail = set_matrix(project_matrix_handle, mat_final);
	if (!check_fail.check_if_failed())
	{
		engine_basic::engine_fail_reason failed_message("set final matrix error in" + shader_file_string);
		return failed_message;
	}
	engine_basic::engine_fail_reason succeed;
	return succeed;
}
void color_shader::set_inputpoint_desc(D3D11_INPUT_ELEMENT_DESC *member_point, UINT *num_member)
{
	//���ö�������
	D3D11_INPUT_ELEMENT_DESC rec[] =
	{
		//������    ��������      ���ݸ�ʽ          ����� ��ʼ��ַ     ����۵ĸ�ʽ 
		{ "POSITION",0  ,DXGI_FORMAT_R32G32B32_FLOAT   ,0    ,0  ,D3D11_INPUT_PER_VERTEX_DATA  ,0 },
		{ "NORMAL"  ,0  ,DXGI_FORMAT_R32G32B32_FLOAT   ,0    ,12 ,D3D11_INPUT_PER_VERTEX_DATA  ,0 },
		{ "TANGENT" ,0  ,DXGI_FORMAT_R32G32B32_FLOAT   ,0    ,24 ,D3D11_INPUT_PER_VERTEX_DATA  ,0 },
		{ "TEXINDICES" ,0  ,DXGI_FORMAT_R32G32B32A32_UINT  ,0    ,36 ,D3D11_INPUT_PER_VERTEX_DATA  ,0 },
		{ "TEXCOORD",0  ,DXGI_FORMAT_R32G32_FLOAT      ,0    ,52 ,D3D11_INPUT_PER_VERTEX_DATA  ,0 }
	};
	*num_member = sizeof(rec) / sizeof(D3D11_INPUT_ELEMENT_DESC);
	for (UINT i = 0; i < *num_member; ++i)
	{
		member_point[i] = rec[i];
	}
}
void color_shader::release()
{
	release_basic();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~��α����~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
virtual_light_shader::virtual_light_shader(LPCWSTR filename) : shader_basic(filename)
{
}
void virtual_light_shader::init_handle()
{
	world_matrix_handle = fx_need->GetVariableByName("world_matrix")->AsMatrix();
	normal_matrix_handle = fx_need->GetVariableByName("normal_matrix")->AsMatrix();
	project_matrix_handle = fx_need->GetVariableByName("final_matrix")->AsMatrix();
	ssao_matrix_handle = fx_need->GetVariableByName("ssao_matrix")->AsMatrix();             //ssao����任���

	texture_diffuse_handle = fx_need->GetVariableByName("texture_diffuse")->AsShaderResource();
	texture_normal_handle = fx_need->GetVariableByName("texture_specular")->AsShaderResource();
	texture_specular_handle = fx_need->GetVariableByName("texturet_normal")->AsShaderResource();
	texture_ssao_handle = fx_need->GetVariableByName("texture_ssao")->AsShaderResource();        //��������ͼ���

	texture_diffusearray_handle = fx_need->GetVariableByName("texture_pack_diffuse")->AsShaderResource();
}
engine_basic::engine_fail_reason virtual_light_shader::set_tex_diffuse_array(ID3D11ShaderResourceView *tex_in)
{
	HRESULT hr = texture_diffusearray_handle->SetResource(tex_in);
	if (hr != S_OK)
	{
		engine_basic::engine_fail_reason failed_message(hr, "set diffuse texarray error in" + shader_file_string);
		return failed_message;
	}
	engine_basic::engine_fail_reason succeed;
	return succeed;
}
engine_basic::engine_fail_reason virtual_light_shader::set_trans_world(XMFLOAT4X4 *mat_world)
{
	//����任
	engine_basic::engine_fail_reason check_fail = set_matrix(world_matrix_handle, mat_world);
	if (!check_fail.check_if_failed())
	{
		engine_basic::engine_fail_reason failed_message("set world matrix error in" + shader_file_string);
		return failed_message;
	}
	//���߱任
	XMMATRIX rec_mat = XMLoadFloat4x4(mat_world);
	XMVECTOR x_delta;
	XMMATRIX check = rec_mat;
	//���߱任
	XMMATRIX normal_need = DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(&x_delta, check));
	XMFLOAT4X4 mat_normal;
	XMStoreFloat4x4(&mat_normal, normal_need);
	mat_normal._41 = 0.0f;
	mat_normal._42 = 0.0f;
	mat_normal._43 = 0.0f;
	mat_normal._44 = 1.0f;
	check_fail = set_matrix(normal_matrix_handle, &mat_normal);
	if (!check_fail.check_if_failed())
	{
		engine_basic::engine_fail_reason failed_message("set normal matrix error in" + shader_file_string);
		return failed_message;
	}
	engine_basic::engine_fail_reason succeed;
	return succeed;
}
engine_basic::engine_fail_reason virtual_light_shader::set_trans_all(XMFLOAT4X4 *mat_final)
{
	engine_basic::engine_fail_reason check_fail = set_matrix(project_matrix_handle, mat_final);
	if (!check_fail.check_if_failed())
	{
		engine_basic::engine_fail_reason failed_message("set final matrix error in" + shader_file_string);
		return failed_message;
	}
	engine_basic::engine_fail_reason succeed;
	return succeed;
}
engine_basic::engine_fail_reason virtual_light_shader::set_trans_ssao(XMFLOAT4X4 *mat_need)
{
	engine_basic::engine_fail_reason check_error = set_matrix(ssao_matrix_handle, mat_need);;
	if (!check_error.check_if_failed())
	{
		return check_error;
	}
	engine_basic::engine_fail_reason succeed;
	return succeed;
}
engine_basic::engine_fail_reason virtual_light_shader::set_ssaotex(ID3D11ShaderResourceView *tex_in)
{
	HRESULT hr = texture_ssao_handle->SetResource(tex_in);
	if (hr != S_OK)
	{
		engine_basic::engine_fail_reason failed_message("set ssao texture error in" + shader_file_string);
		return failed_message;
	}
	engine_basic::engine_fail_reason succeed;
	return succeed;
}
engine_basic::engine_fail_reason virtual_light_shader::set_tex_diffuse(ID3D11ShaderResourceView *tex_in)
{
	HRESULT hr = texture_diffuse_handle->SetResource(tex_in);
	if (hr != S_OK)
	{
		engine_basic::engine_fail_reason failed_message(hr,"set diffuse tex error in" + shader_file_string);
		return failed_message;
	}
	engine_basic::engine_fail_reason succeed;
	return succeed;
}
engine_basic::engine_fail_reason virtual_light_shader::set_tex_normal(ID3D11ShaderResourceView *tex_in)
{
	HRESULT hr = texture_normal_handle->SetResource(tex_in);
	if (hr != S_OK)
	{
		engine_basic::engine_fail_reason failed_message(hr, "set diffuse tex error in" + shader_file_string);
		return failed_message;
	}
	engine_basic::engine_fail_reason succeed;
	return succeed;
}
engine_basic::engine_fail_reason virtual_light_shader::set_tex_specular(ID3D11ShaderResourceView *tex_in)
{
	HRESULT hr = texture_specular_handle->SetResource(tex_in);
	if (hr != S_OK)
	{
		engine_basic::engine_fail_reason failed_message(hr, "set diffuse tex error in" + shader_file_string);
		return failed_message;
	}
	engine_basic::engine_fail_reason succeed;
	return succeed;
}
void virtual_light_shader::set_inputpoint_desc(D3D11_INPUT_ELEMENT_DESC *member_point, UINT *num_member)
{
	//���ö�������
	D3D11_INPUT_ELEMENT_DESC rec[] =
	{
		//������    ��������      ���ݸ�ʽ          ����� ��ʼ��ַ     ����۵ĸ�ʽ 
		{ "POSITION",0  ,DXGI_FORMAT_R32G32B32_FLOAT   ,0    ,0  ,D3D11_INPUT_PER_VERTEX_DATA  ,0 },
		{ "NORMAL"  ,0  ,DXGI_FORMAT_R32G32B32_FLOAT   ,0    ,12 ,D3D11_INPUT_PER_VERTEX_DATA  ,0 },
		{ "TANGENT" ,0  ,DXGI_FORMAT_R32G32B32_FLOAT   ,0    ,24 ,D3D11_INPUT_PER_VERTEX_DATA  ,0 },
		{ "TEXINDICES" ,0  ,DXGI_FORMAT_R32G32B32A32_UINT  ,0    ,36 ,D3D11_INPUT_PER_VERTEX_DATA  ,0 },
		{ "TEXDIFFNORM",0  ,DXGI_FORMAT_R32G32B32A32_FLOAT      ,0    ,52 ,D3D11_INPUT_PER_VERTEX_DATA  ,0 },
		{ "TEXOTHER",0  ,DXGI_FORMAT_R32G32B32A32_FLOAT      ,0    ,64 ,D3D11_INPUT_PER_VERTEX_DATA  ,0 }
	};
	*num_member = sizeof(rec) / sizeof(D3D11_INPUT_ELEMENT_DESC);
	for (UINT i = 0; i < *num_member; ++i)
	{
		member_point[i] = rec[i];
	}
}
void virtual_light_shader::release()
{
	release_basic();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~��ʾͼ��~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
picture_show_shader::picture_show_shader(LPCWSTR filename) : shader_basic(filename)
{
}
void picture_show_shader::release()
{
	release_basic();
}
void picture_show_shader::init_handle()
{
	UI_scal_handle = fx_need->GetVariableByName("UI_scal");
	UI_position_handle = fx_need->GetVariableByName("UI_pos");
	tex_color_input = fx_need->GetVariableByName("texture_need")->AsShaderResource();
}
void picture_show_shader::set_inputpoint_desc(D3D11_INPUT_ELEMENT_DESC *member_point, UINT *num_member)
{
	//���ö�������
	D3D11_INPUT_ELEMENT_DESC rec[] =
	{
		//������    ��������      ���ݸ�ʽ          ����� ��ʼ��ַ     ����۵ĸ�ʽ 
		{ "POSITION",0  ,DXGI_FORMAT_R32G32B32_FLOAT   ,0    ,0  ,D3D11_INPUT_PER_VERTEX_DATA  ,0 },
		{ "TEXCOORD",0  ,DXGI_FORMAT_R32G32_FLOAT      ,0    ,12 ,D3D11_INPUT_PER_VERTEX_DATA  ,0 }
	};
	*num_member = sizeof(rec) / sizeof(D3D11_INPUT_ELEMENT_DESC);
	for (UINT i = 0; i < *num_member; ++i)
	{
		member_point[i] = rec[i];
	}
}
engine_basic::engine_fail_reason picture_show_shader::set_tex_color_resource(ID3D11ShaderResourceView *buffer_input)
{
	HRESULT hr;
	hr = tex_color_input->SetResource(buffer_input);
	if (FAILED(hr))
	{
		engine_basic::engine_fail_reason failed_message(hr, "set UI_comman color texture error" + shader_file_string);
		return failed_message;
	}
	engine_basic::engine_fail_reason succeed;
	return succeed;
}
engine_basic::engine_fail_reason picture_show_shader::set_UI_scal(XMFLOAT4 range)
{
	HRESULT hr;
	hr = UI_scal_handle->SetRawValue((void*)&range, 0, sizeof(range));
	if (FAILED(hr))
	{
		engine_basic::engine_fail_reason failed_message(hr, "set UI scal handle error" + shader_file_string);
		return failed_message;
	}
	engine_basic::engine_fail_reason succeed;
	return succeed;
}
engine_basic::engine_fail_reason picture_show_shader::set_UI_position(XMFLOAT4 range)
{
	HRESULT hr;
	hr = UI_position_handle->SetRawValue((void*)&range, 0, sizeof(range));
	if (FAILED(hr))
	{
		engine_basic::engine_fail_reason failed_message(hr, "set UI position handle error" + shader_file_string);
		return failed_message;
	}
	engine_basic::engine_fail_reason succeed;
	return succeed;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~gbuffer��¼~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
shader_pretreat_gbuffer::shader_pretreat_gbuffer(LPCWSTR filename) :shader_basic(filename)
{
}
void shader_pretreat_gbuffer::init_handle()
{
	world_matrix_handle = fx_need->GetVariableByName("world_matrix")->AsMatrix();
	normal_matrix_handle = fx_need->GetVariableByName("normal_matrix")->AsMatrix();
	project_matrix_handle = fx_need->GetVariableByName("final_matrix")->AsMatrix();
	texture_packarray_handle = fx_need->GetVariableByName("texture_pack_array")->AsShaderResource();
	world_matrix_array_handle = fx_need->GetVariableByName("world_matrix_array")->AsMatrix();//����任�����
	normal_matrix_array_handle = fx_need->GetVariableByName("normal_matrix_array")->AsMatrix();//���߱任�����
	proj_matrix_handle = fx_need->GetVariableByName("proj_matrix")->AsMatrix();   //ȡ��*ͶӰ�任����
}
engine_basic::engine_fail_reason shader_pretreat_gbuffer::set_trans_world(XMFLOAT4X4 *mat_world, XMFLOAT4X4 *mat_view)
{
	XMVECTOR x_delta;
	XMMATRIX world_need = XMLoadFloat4x4(mat_world);
	XMMATRIX view_need = XMLoadFloat4x4(mat_view);
	XMMATRIX normal_need = DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(&x_delta, world_need));
	normal_need.r[0].m128_f32[3] = 0;
	normal_need.r[1].m128_f32[3] = 0;
	normal_need.r[2].m128_f32[3] = 0;
	normal_need.r[3].m128_f32[3] = 1;
	HRESULT hr;
	hr = world_matrix_handle->SetMatrix(reinterpret_cast<float*>(&(world_need*view_need)));
	if (FAILED(hr))
	{
		engine_basic::engine_fail_reason error_message(hr, "set world matrix error in gbuffer depthnormal part");
		return error_message;
	}
	hr = normal_matrix_handle->SetMatrix(reinterpret_cast<float*>(&(normal_need*view_need)));
	if (FAILED(hr))
	{
		engine_basic::engine_fail_reason error_message(hr, "set normal matrix error in gbuffer depthnormal part");
		return error_message;
	}
	engine_basic::engine_fail_reason succeed;
	return succeed;
}
engine_basic::engine_fail_reason shader_pretreat_gbuffer::set_trans_all(XMFLOAT4X4 *mat_final)
{
	engine_basic::engine_fail_reason check_error = set_matrix(project_matrix_handle, mat_final);
	if (!check_error.check_if_failed())
	{
		return check_error;
	}
	engine_basic::engine_fail_reason succeed;
	return succeed;
}
engine_basic::engine_fail_reason shader_pretreat_gbuffer::set_trans_proj(XMFLOAT4X4 *mat_need)
{
	engine_basic::engine_fail_reason check_error = set_matrix(proj_matrix_handle, mat_need);;
	if (!check_error.check_if_failed())
	{
		return check_error;
	}
	engine_basic::engine_fail_reason succeed;
	return succeed;
}
engine_basic::engine_fail_reason shader_pretreat_gbuffer::set_texturepack_array(ID3D11ShaderResourceView *tex_in)
{
	HRESULT hr;
	hr = texture_packarray_handle->SetResource(tex_in);
	if (FAILED(hr))
	{
		engine_basic::engine_fail_reason error_message(hr, "set tex_pack error in gbuffer depthnormal part");
		return error_message;
	}
	engine_basic::engine_fail_reason succeed;
	return succeed;
}
engine_basic::engine_fail_reason shader_pretreat_gbuffer::set_world_matrix_array(const XMFLOAT4X4* M, XMFLOAT4X4 mat_view,int cnt)
{
	XMFLOAT4X4 *world_mat = new XMFLOAT4X4[cnt];
	XMFLOAT4X4 *normal_mat = new XMFLOAT4X4[cnt];
	for (int i = 0; i < cnt; ++i) 
	{
		XMVECTOR x_delta;
		XMMATRIX world_need = XMLoadFloat4x4(&M[i]);
		XMMATRIX view_need = XMLoadFloat4x4(&mat_view);
		XMMATRIX normal_need = DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(&x_delta, world_need));
		normal_need.r[0].m128_f32[3] = 0;
		normal_need.r[1].m128_f32[3] = 0;
		normal_need.r[2].m128_f32[3] = 0;
		normal_need.r[3].m128_f32[3] = 1;

		XMStoreFloat4x4(&world_mat[i], world_need * view_need);
		XMStoreFloat4x4(&normal_mat[i], normal_need * view_need);
	}
	
	HRESULT hr = world_matrix_array_handle->SetMatrixArray(reinterpret_cast<const float*>(world_mat), 0, cnt);
	if (FAILED(hr))
	{
		engine_basic::engine_fail_reason error_message(hr, "set world_matrix_array error in gbuffer depthnormal part");
		return error_message;
	}
	hr = normal_matrix_array_handle->SetMatrixArray(reinterpret_cast<const float*>(normal_mat), 0, cnt);
	if (FAILED(hr))
	{
		engine_basic::engine_fail_reason error_message(hr, "set normal_matrix_array error in gbuffer depthnormal part");
		return error_message;
	}
	engine_basic::engine_fail_reason succeed;
	return succeed;
}
void shader_pretreat_gbuffer::release()
{
	release_basic();
}
void shader_pretreat_gbuffer::set_inputpoint_desc(D3D11_INPUT_ELEMENT_DESC *member_point, UINT *num_member)
{
	//���ö�������
	D3D11_INPUT_ELEMENT_DESC rec[] =
	{
		//������    ��������      ���ݸ�ʽ          ����� ��ʼ��ַ     ����۵ĸ�ʽ 
		{ "POSITION"   ,0  ,DXGI_FORMAT_R32G32B32_FLOAT     ,0    ,0  ,D3D11_INPUT_PER_VERTEX_DATA  ,0 },
		{ "NORMAL"     ,0  ,DXGI_FORMAT_R32G32B32_FLOAT     ,0    ,12 ,D3D11_INPUT_PER_VERTEX_DATA  ,0 },
		{ "TANGENT"    ,0  ,DXGI_FORMAT_R32G32B32_FLOAT     ,0    ,24 ,D3D11_INPUT_PER_VERTEX_DATA  ,0 },
		{ "TEXINDICES" ,0  ,DXGI_FORMAT_R32G32B32A32_UINT   ,0    ,36 ,D3D11_INPUT_PER_VERTEX_DATA  ,0 },
		{ "TEXDIFFNORM",0  ,DXGI_FORMAT_R32G32B32A32_FLOAT  ,0    ,52 ,D3D11_INPUT_PER_VERTEX_DATA  ,0 },
		{ "TEXOTHER"   ,0  ,DXGI_FORMAT_R32G32B32A32_FLOAT  ,0    ,64 ,D3D11_INPUT_PER_VERTEX_DATA  ,0 }
	};
	*num_member = sizeof(rec) / sizeof(D3D11_INPUT_ELEMENT_DESC);
	for (UINT i = 0; i < *num_member; ++i)
	{
		member_point[i] = rec[i];
	}
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~����ز���MSAA->����~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
shader_resolvedepth::shader_resolvedepth(LPCWSTR filename) :shader_basic(filename)
{
}
engine_basic::engine_fail_reason shader_resolvedepth::set_texture_MSAA(ID3D11ShaderResourceView *tex_in)
{
	HRESULT hr = texture_MSAA->SetResource(tex_in);
	if (FAILED(hr))
	{
		engine_basic::engine_fail_reason error_message(hr, "set texture_MSAA error in resolve MSAA depth texture");
		return error_message;
	}
	engine_basic::engine_fail_reason succeed;
	return succeed;
}
engine_basic::engine_fail_reason shader_resolvedepth::set_projmessage(XMFLOAT3 proj_message)
{
	HRESULT hr = projmessage_handle->SetRawValue((void*)&proj_message, 0, sizeof(proj_message));
	if (hr != S_OK)
	{
		engine_basic::engine_fail_reason error_message(hr, "set set_projmessage error in resolve MSAA depth texture");
		return error_message;
	}
	engine_basic::engine_fail_reason succeed;
	return succeed;
}
engine_basic::engine_fail_reason shader_resolvedepth::set_window_size(float width, float height) 
{
	XMFLOAT4 window_size_data = XMFLOAT4(width, height,0.0f,0.0f);
	HRESULT hr = window_size->SetRawValue((void*)&window_size_data, 0, sizeof(window_size_data));
	if (hr != S_OK)
	{
		engine_basic::engine_fail_reason error_message(hr, "set window size error in resolve MSAA depth texture");
		return error_message;
	}
	engine_basic::engine_fail_reason succeed;
	return succeed;
}
void shader_resolvedepth::release()
{
	release_basic();
}
void shader_resolvedepth::init_handle()
{
	//������Ϣ���
	texture_MSAA = fx_need->GetVariableByName("gdepth_map")->AsShaderResource();
	//���α任��Ϣ���
	projmessage_handle = fx_need->GetVariableByName("proj_desc");
	//�������ڴ�С
	window_size = fx_need->GetVariableByName("window_size");
}
void shader_resolvedepth::set_inputpoint_desc(D3D11_INPUT_ELEMENT_DESC *member_point, UINT *num_member)
{
	//���ö�������
	D3D11_INPUT_ELEMENT_DESC rec[] =
	{
		//������    ��������      ���ݸ�ʽ          ����� ��ʼ��ַ     ����۵ĸ�ʽ 
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",     0, DXGI_FORMAT_R32G32_FLOAT,    0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	*num_member = sizeof(rec) / sizeof(D3D11_INPUT_ELEMENT_DESC);
	for (UINT i = 0; i < *num_member; ++i)
	{
		member_point[i] = rec[i];
	}
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ssao �ڱ���Ⱦ����~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
shader_ssaomap::shader_ssaomap(LPCWSTR filename) :shader_basic(filename)
{
}
void shader_ssaomap::init_handle()
{
	ViewToTexSpace = fx_need->GetVariableByName("gViewToTexSpace")->AsMatrix();
	OffsetVectors = fx_need->GetVariableByName("gOffsetVectors")->AsVector();
	FrustumCorners = fx_need->GetVariableByName("gFrustumCorners")->AsVector();

	NormalDepthMap = fx_need->GetVariableByName("gNormalDepthMap")->AsShaderResource();
	DepthMap = fx_need->GetVariableByName("gdepth_map")->AsShaderResource();
	RandomVecMap = fx_need->GetVariableByName("gRandomVecMap")->AsShaderResource();
}
void shader_ssaomap::release()
{
	release_basic();
}
engine_basic::engine_fail_reason shader_ssaomap::set_ViewToTexSpace(XMFLOAT4X4 *mat)
{
	engine_basic::engine_fail_reason check_error = set_matrix(ViewToTexSpace, mat);
	if (!check_error.check_if_failed())
	{
		return check_error;
	}
	engine_basic::engine_fail_reason succeed;
	return succeed;
}
engine_basic::engine_fail_reason shader_ssaomap::set_FrustumCorners(const XMFLOAT4 v[4])
{
	HRESULT hr = FrustumCorners->SetFloatVectorArray(reinterpret_cast<const float*>(v), 0, 4);
	if (FAILED(hr))
	{
		engine_basic::engine_fail_reason error_message(hr, "set FrustumCorners error in ssao draw part");
		return error_message;
	}
	engine_basic::engine_fail_reason succeed;
	return succeed;
}
engine_basic::engine_fail_reason shader_ssaomap::set_OffsetVectors(const XMFLOAT4 v[14])
{
	HRESULT hr = OffsetVectors->SetFloatVectorArray(reinterpret_cast<const float*>(v), 0, 14);
	if (FAILED(hr))
	{
		engine_basic::engine_fail_reason error_message(hr, "set OffsetVectors error in ssao draw part");
		return error_message;
	}
	engine_basic::engine_fail_reason succeed;
	return succeed;
}
engine_basic::engine_fail_reason shader_ssaomap::set_NormalDepthtex(ID3D11ShaderResourceView* srv)
{
	HRESULT hr = NormalDepthMap->SetResource(srv);
	if (FAILED(hr))
	{
		engine_basic::engine_fail_reason error_message(hr, "set Normalspectex error in ssao draw part");
		return error_message;
	}
	engine_basic::engine_fail_reason succeed;
	return succeed;
}
engine_basic::engine_fail_reason shader_ssaomap::set_Depthtex(ID3D11ShaderResourceView* srv)
{
	HRESULT hr = DepthMap->SetResource(srv);
	if (FAILED(hr))
	{
		engine_basic::engine_fail_reason error_message(hr, "set depthmap error in ssao draw part");
		return error_message;
	}
	engine_basic::engine_fail_reason succeed;
	return succeed;
}
engine_basic::engine_fail_reason shader_ssaomap::set_randomtex(ID3D11ShaderResourceView* srv)
{
	HRESULT hr = RandomVecMap->SetResource(srv);
	if (FAILED(hr))
	{
		engine_basic::engine_fail_reason error_message(hr, "set randomtex error in ssao draw part");
		return error_message;
	}
	engine_basic::engine_fail_reason succeed;
	return succeed;
}
void shader_ssaomap::set_inputpoint_desc(D3D11_INPUT_ELEMENT_DESC *member_point, UINT *num_member)
{
	//���ö�������
	D3D11_INPUT_ELEMENT_DESC rec[] =
	{
		//������    ��������      ���ݸ�ʽ          ����� ��ʼ��ַ     ����۵ĸ�ʽ 
		{ "POSITION",0  ,DXGI_FORMAT_R32G32B32_FLOAT   ,0    ,0  ,D3D11_INPUT_PER_VERTEX_DATA  ,0 },
		{ "NORMAL"  ,0  ,DXGI_FORMAT_R32G32B32_FLOAT   ,0    ,12 ,D3D11_INPUT_PER_VERTEX_DATA  ,0 },
		{ "TEXCOORD",0  ,DXGI_FORMAT_R32G32_FLOAT      ,0    ,24 ,D3D11_INPUT_PER_VERTEX_DATA  ,0 }
	};
	*num_member = sizeof(rec) / sizeof(D3D11_INPUT_ELEMENT_DESC);
	for (UINT i = 0; i < *num_member; ++i)
	{
		member_point[i] = rec[i];
	}
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ssao aoͼģ��������~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
shader_ssaoblur::shader_ssaoblur(LPCWSTR filename) :shader_basic(filename)
{
}
void shader_ssaoblur::release()
{
	release_basic();
}
void shader_ssaoblur::init_handle()
{
	TexelWidth = fx_need->GetVariableByName("gTexelWidth")->AsScalar();
	TexelHeight = fx_need->GetVariableByName("gTexelHeight")->AsScalar();

	NormalDepthMap = fx_need->GetVariableByName("gNormalDepthMap")->AsShaderResource();
	DepthMap = fx_need->GetVariableByName("gdepth_map")->AsShaderResource();
	InputImage = fx_need->GetVariableByName("gInputImage")->AsShaderResource();
}
engine_basic::engine_fail_reason shader_ssaoblur::set_image_size(float width, float height)
{
	HRESULT hr;
	hr = TexelWidth->SetFloat(width);
	if (FAILED(hr))
	{
		engine_basic::engine_fail_reason error_message(hr, "set image_size error in ssao blur part");
		return error_message;
	}
	hr = TexelHeight->SetFloat(height);
	if (FAILED(hr))
	{
		engine_basic::engine_fail_reason error_message(hr, "set image_size error in ssao blur part");
		return error_message;
	}
	engine_basic::engine_fail_reason succeed;
	return succeed;
}
engine_basic::engine_fail_reason shader_ssaoblur::set_tex_resource(ID3D11ShaderResourceView* tex_normaldepth, ID3D11ShaderResourceView* tex_aomap)
{
	HRESULT hr;
	hr = NormalDepthMap->SetResource(tex_normaldepth);
	if (FAILED(hr))
	{
		engine_basic::engine_fail_reason error_message(hr, "set tex_normaldepth error in ssao blur part");
		return error_message;
	}
	hr = InputImage->SetResource(tex_aomap);
	if (FAILED(hr))
	{
		engine_basic::engine_fail_reason error_message(hr, "set tex_aomap error in ssao blur part");
		return error_message;
	}
	engine_basic::engine_fail_reason succeed;
	return succeed;
}
engine_basic::engine_fail_reason shader_ssaoblur::set_Depthtex(ID3D11ShaderResourceView* srv)
{
	HRESULT hr = DepthMap->SetResource(srv);
	if (FAILED(hr))
	{
		engine_basic::engine_fail_reason error_message(hr, "set Depthtex error in ssao draw part");
		return error_message;
	}
	engine_basic::engine_fail_reason succeed;
	return succeed;
}
void shader_ssaoblur::set_inputpoint_desc(D3D11_INPUT_ELEMENT_DESC *member_point, UINT *num_member)
{
	//���ö�������
	D3D11_INPUT_ELEMENT_DESC rec[] =
	{
		//������    ��������      ���ݸ�ʽ          ����� ��ʼ��ַ     ����۵ĸ�ʽ 
		{ "POSITION",0  ,DXGI_FORMAT_R32G32B32_FLOAT   ,0    ,0  ,D3D11_INPUT_PER_VERTEX_DATA  ,0 },
		{ "NORMAL"  ,0  ,DXGI_FORMAT_R32G32B32_FLOAT   ,0    ,12 ,D3D11_INPUT_PER_VERTEX_DATA  ,0 },
		{ "TEXCOORD",0  ,DXGI_FORMAT_R32G32_FLOAT      ,0    ,24 ,D3D11_INPUT_PER_VERTEX_DATA  ,0 }
	};
	*num_member = sizeof(rec) / sizeof(D3D11_INPUT_ELEMENT_DESC);
	for (UINT i = 0; i < *num_member; ++i)
	{
		member_point[i] = rec[i];
	}
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~shadow map����~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
light_shadow::light_shadow(LPCWSTR filename) : shader_basic(filename)
{
}
engine_basic::engine_fail_reason light_shadow::set_trans_all(XMFLOAT4X4 *mat_need)
{
	auto check_error = set_matrix(project_matrix_handle, mat_need);;
	if (!check_error.check_if_failed())
	{
		return check_error;
	}
	engine_basic::engine_fail_reason succeed;
	return succeed;
}
engine_basic::engine_fail_reason light_shadow::set_trans_viewproj(XMFLOAT4X4 *mat_need)
{
	auto check_error = set_matrix(viewproj_matrix_handle, mat_need);;
	if (!check_error.check_if_failed())
	{
		return check_error;
	}
	engine_basic::engine_fail_reason succeed;
	return succeed;
}
engine_basic::engine_fail_reason light_shadow::set_world_matrix_array(const XMFLOAT4X4* M, int cnt)
{
	HRESULT hr = world_matrix_array_handle->SetMatrixArray(reinterpret_cast<const float*>(M), 0, cnt);
	if (FAILED(hr))
	{
		engine_basic::engine_fail_reason error_message(hr, "set instance world matrix array error in shadow map part");
		return error_message;
	}
	engine_basic::engine_fail_reason succeed;
	return succeed;
}
engine_basic::engine_fail_reason light_shadow::set_texturepack_array(ID3D11ShaderResourceView *tex_in)
{
	HRESULT hr;
	hr = texture_need->SetResource(tex_in);
	if (FAILED(hr))
	{
		engine_basic::engine_fail_reason error_message(hr, "set tex_normaldepth error in shadow map part");
		return error_message;
	}
	engine_basic::engine_fail_reason succeed;
	return succeed;
}
void light_shadow::init_handle()
{
	project_matrix_handle = fx_need->GetVariableByName("final_matrix")->AsMatrix();         //ȫ�׼��α任���
	texture_need = fx_need->GetVariableByName("texture_pack_array")->AsShaderResource();
	world_matrix_array_handle = fx_need->GetVariableByName("world_matrix_array")->AsMatrix();//����任�����
	viewproj_matrix_handle = fx_need->GetVariableByName("view_proj_matrix")->AsMatrix();   //ȡ��*ͶӰ�任����
}
void light_shadow::set_inputpoint_desc(D3D11_INPUT_ELEMENT_DESC *member_point, UINT *num_member)
{
	//���ö�������
	D3D11_INPUT_ELEMENT_DESC rec[] =
	{
		//������    ��������      ���ݸ�ʽ          ����� ��ʼ��ַ     ����۵ĸ�ʽ 
		{ "POSITION",0  ,DXGI_FORMAT_R32G32B32_FLOAT   ,0    ,0  ,D3D11_INPUT_PER_VERTEX_DATA  ,0 },
		{ "NORMAL"  ,0  ,DXGI_FORMAT_R32G32B32_FLOAT   ,0    ,12 ,D3D11_INPUT_PER_VERTEX_DATA  ,0 },
		{ "TANGENT" ,0  ,DXGI_FORMAT_R32G32B32_FLOAT   ,0    ,24 ,D3D11_INPUT_PER_VERTEX_DATA  ,0 },
		{ "TEXINDICES" ,0  ,DXGI_FORMAT_R32G32B32A32_UINT  ,0    ,36 ,D3D11_INPUT_PER_VERTEX_DATA  ,0 },
		{ "TEXDIFFNORM",0  ,DXGI_FORMAT_R32G32B32A32_FLOAT      ,0    ,52 ,D3D11_INPUT_PER_VERTEX_DATA  ,0 },
		{ "TEXOTHER",0  ,DXGI_FORMAT_R32G32B32A32_FLOAT      ,0    ,64 ,D3D11_INPUT_PER_VERTEX_DATA  ,0 }
	};
	*num_member = sizeof(rec) / sizeof(D3D11_INPUT_ELEMENT_DESC);
	for (UINT i = 0; i < *num_member; ++i)
	{
		member_point[i] = rec[i];
	}
	*num_member = sizeof(rec) / sizeof(D3D11_INPUT_ELEMENT_DESC);
	for (UINT i = 0; i < *num_member; ++i)
	{
		member_point[i] = rec[i];
	}
}
void light_shadow::release()
{
	release_basic();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~�ӳٹ����㷨���ջ�����~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
light_defered_lightbuffer::light_defered_lightbuffer(LPCWSTR filename) :shader_basic(filename)
{
}

engine_basic::engine_fail_reason light_defered_lightbuffer::set_sunlight(pancy_light_basic light_need)
{
	HRESULT hr = light_sun->SetRawValue(&light_need, 0, sizeof(light_need));
	if (hr != S_OK)
	{
		engine_basic::engine_fail_reason error_message(hr, "lightbuffer shader error when setting sunlight");
		return error_message;
	}
	engine_basic::engine_fail_reason succeed;
	return succeed;
}
engine_basic::engine_fail_reason light_defered_lightbuffer::set_sunshadow_matrix(const XMFLOAT4X4* M, int cnt)
{
	HRESULT hr = sunshadow_matrix_handle->SetMatrixArray(reinterpret_cast<const float*>(M), 0, cnt);
	if (FAILED(hr))
	{
		engine_basic::engine_fail_reason error_message(hr, "lightbuffer shader error when sunshadow_matrix");
		return error_message;
	}
	engine_basic::engine_fail_reason succeed;
	return succeed;
}
engine_basic::engine_fail_reason light_defered_lightbuffer::set_sunlight_num(XMUINT3 all_light_num)
{
	HRESULT hr = sunlight_num->SetRawValue((void*)&all_light_num, 0, sizeof(all_light_num));
	if (hr != S_OK)
	{
		engine_basic::engine_fail_reason error_message(hr, "lightbuffer shader error when setting light num");
		return error_message;
	}
	engine_basic::engine_fail_reason succeed;
	return succeed;
}
engine_basic::engine_fail_reason light_defered_lightbuffer::set_sunshadow_tex(ID3D11ShaderResourceView *tex_in)
{
	HRESULT hr = suntexture_shadow->SetResource(tex_in);
	if (hr != S_OK)
	{
		engine_basic::engine_fail_reason error_message(hr, "lightbuffer shader error when setting sunshadow tex");
		return error_message;
	}
	engine_basic::engine_fail_reason succeed;
	return succeed;
}
engine_basic::engine_fail_reason light_defered_lightbuffer::set_depth_devide(XMFLOAT4 v)
{
	HRESULT hr = depth_devide->SetRawValue((void*)&v, 0, sizeof(v));
	if (hr != S_OK)
	{
		engine_basic::engine_fail_reason error_message(hr, "lightbuffer shader error when setting depth divide");
		return error_message;
	}
	engine_basic::engine_fail_reason succeed;
	return succeed;
}

engine_basic::engine_fail_reason light_defered_lightbuffer::set_light(pancy_light_basic light_need, int light_num)
{
	HRESULT hr = light_list->SetRawValue(&light_need, light_num * sizeof(light_need), sizeof(light_need));
	if (hr != S_OK)
	{
		engine_basic::engine_fail_reason error_message(hr, "lightbuffer shader error when setting light");
		return error_message;
	}
	engine_basic::engine_fail_reason succeed;
	return succeed;
}
engine_basic::engine_fail_reason light_defered_lightbuffer::set_FrustumCorners(const XMFLOAT4 v[4])
{
	HRESULT hr = FrustumCorners->SetFloatVectorArray(reinterpret_cast<const float*>(v), 0, 4);
	if (FAILED(hr))
	{
		engine_basic::engine_fail_reason error_message(hr, "lightbuffer shader error when setting FrustumCorners");
		return error_message;
	}
	engine_basic::engine_fail_reason succeed;
	return succeed;
}
engine_basic::engine_fail_reason light_defered_lightbuffer::set_projmessage(XMFLOAT3 proj_message)
{
	HRESULT hr = projmessage_handle->SetRawValue((void*)&proj_message, 0, sizeof(proj_message));
	if (hr != S_OK)
	{
		engine_basic::engine_fail_reason error_message(hr, "set set_projmessage error in lightbuffer shadere");
		return error_message;
	}
	engine_basic::engine_fail_reason succeed;
	return succeed;
}
engine_basic::engine_fail_reason light_defered_lightbuffer::set_shadow_matrix(const XMFLOAT4X4* M, int cnt)
{
	HRESULT hr = shadow_matrix_handle->SetMatrixArray(reinterpret_cast<const float*>(M), 0, cnt);
	if (FAILED(hr))
	{
		engine_basic::engine_fail_reason error_message(hr, "lightbuffer shader error when setting shadow_matrix");
		return error_message;
	}
	engine_basic::engine_fail_reason succeed;
	return succeed;
}
engine_basic::engine_fail_reason light_defered_lightbuffer::set_view_matrix(XMFLOAT4X4 *mat_need)
{
	auto check_error = set_matrix(view_matrix_handle, mat_need);;
	if (!check_error.check_if_failed())
	{
		return check_error;
	}
	engine_basic::engine_fail_reason succeed;
	return succeed;
}
engine_basic::engine_fail_reason light_defered_lightbuffer::set_invview_matrix(XMFLOAT4X4 *mat_need)
{
	auto check_error = set_matrix(invview_matrix_handle, mat_need);;
	if (!check_error.check_if_failed())
	{
		return check_error;
	}
	engine_basic::engine_fail_reason succeed;
	return succeed;
}

engine_basic::engine_fail_reason light_defered_lightbuffer::set_light_num(XMUINT3 all_light_num)
{
	HRESULT hr = light_num_handle->SetRawValue((void*)&all_light_num, 0, sizeof(all_light_num));
	if (hr != S_OK)
	{
		engine_basic::engine_fail_reason error_message(hr, "lightbuffer shader error when setting light num");
		return error_message;
	}
	engine_basic::engine_fail_reason succeed;
	return succeed;
}
engine_basic::engine_fail_reason light_defered_lightbuffer::set_shadow_num(XMUINT3 all_light_num)
{
	HRESULT hr = shadow_num_handle->SetRawValue((void*)&all_light_num, 0, sizeof(all_light_num));
	if (hr != S_OK)
	{
		engine_basic::engine_fail_reason error_message(hr, "lightbuffer shader error when setting shadow num");
		return error_message;
	}
	engine_basic::engine_fail_reason succeed;
	return succeed;
}
engine_basic::engine_fail_reason light_defered_lightbuffer::set_Normalspec_tex(ID3D11ShaderResourceView *tex_in)
{
	HRESULT hr = NormalspecMap->SetResource(tex_in);
	if (hr != S_OK)
	{
		engine_basic::engine_fail_reason error_message(hr, "lightbuffer shader error when setting Normalspec_tex");
		return error_message;
	}
	engine_basic::engine_fail_reason succeed;
	return succeed;
}
engine_basic::engine_fail_reason light_defered_lightbuffer::set_DepthMap_tex(ID3D11ShaderResourceView *tex_in)
{
	HRESULT hr = DepthMap->SetResource(tex_in);
	if (hr != S_OK)
	{
		engine_basic::engine_fail_reason error_message(hr, "lightbuffer shader error when setting DepthMap_tex");
		return error_message;
	}
	engine_basic::engine_fail_reason succeed;
	return succeed;
}
engine_basic::engine_fail_reason light_defered_lightbuffer::set_shadow_tex(ID3D11ShaderResourceView *tex_in)
{
	HRESULT hr = texture_shadow->SetResource(tex_in);
	if (hr != S_OK)
	{
		engine_basic::engine_fail_reason error_message(hr, "lightbuffer shader error when setting shadow_tex");
		return error_message;
	}
	engine_basic::engine_fail_reason succeed;
	return succeed;
}
void light_defered_lightbuffer::release()
{
	release_basic();
}
void light_defered_lightbuffer::init_handle()
{
	//̫��������
	light_sun = fx_need->GetVariableByName("sun_light");                  //̫����
	sunlight_num = fx_need->GetVariableByName("sun_light_num");               //̫����ּ�����
	depth_devide = fx_need->GetVariableByName("depth_devide");               //ÿһ�������
	suntexture_shadow = fx_need->GetVariableByName("texture_sunshadow")->AsShaderResource();          //̫������Ӱ������Դ���
	sunshadow_matrix_handle = fx_need->GetVariableByName("sunlight_shadowmat")->AsMatrix();    //̫������Ӱͼ�任
																							   //��ͨ��Դ����
	shadow_matrix_handle = fx_need->GetVariableByName("shadowmap_matrix")->AsMatrix();//��Ӱ�任���
	view_matrix_handle = fx_need->GetVariableByName("view_matrix")->AsMatrix();       //ȡ���任���	
	invview_matrix_handle = fx_need->GetVariableByName("invview_matrix")->AsMatrix(); //ȡ���任��任���
	light_list = fx_need->GetVariableByName("light_need");                            //���վ��
	light_num_handle = fx_need->GetVariableByName("light_num");                       //��Դ�������
	shadow_num_handle = fx_need->GetVariableByName("shadow_num");                     //��Ӱ�������
	FrustumCorners = fx_need->GetVariableByName("gFrustumCorners")->AsVector();       //3D��ԭ�ǵ���
																					  //���α任��Ϣ���
	projmessage_handle = fx_need->GetVariableByName("proj_desc");
	NormalspecMap = fx_need->GetVariableByName("gNormalspecMap")->AsShaderResource();  //shader�е�������Դ���
	DepthMap = fx_need->GetVariableByName("gdepth_map")->AsShaderResource();  //shader�е�������Դ���
	texture_shadow = fx_need->GetVariableByName("texture_shadow")->AsShaderResource();  //shader�е�������Դ���
}
void light_defered_lightbuffer::set_inputpoint_desc(D3D11_INPUT_ELEMENT_DESC *member_point, UINT *num_member)
{
	//���ö�������
	D3D11_INPUT_ELEMENT_DESC rec[] =
	{
		//������    ��������      ���ݸ�ʽ          ����� ��ʼ��ַ     ����۵ĸ�ʽ 
		{ "POSITION",0  ,DXGI_FORMAT_R32G32B32_FLOAT   ,0    ,0  ,D3D11_INPUT_PER_VERTEX_DATA  ,0 },
		{ "NORMAL"  ,0  ,DXGI_FORMAT_R32G32B32_FLOAT   ,0    ,12 ,D3D11_INPUT_PER_VERTEX_DATA  ,0 },
		{ "TEXCOORD",0  ,DXGI_FORMAT_R32G32_FLOAT      ,0    ,24 ,D3D11_INPUT_PER_VERTEX_DATA  ,0 }
	};
	*num_member = sizeof(rec) / sizeof(D3D11_INPUT_ELEMENT_DESC);
	for (UINT i = 0; i < *num_member; ++i)
	{
		member_point[i] = rec[i];
	}
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~�ӳٹ����㷨������Ⱦ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
light_defered_draw::light_defered_draw(LPCWSTR filename) : shader_basic(filename)
{
}
engine_basic::engine_fail_reason light_defered_draw::set_view_pos(XMFLOAT3 eye_pos)
{
	HRESULT hr = view_pos_handle->SetRawValue((void*)&eye_pos, 0, sizeof(eye_pos));
	if (hr != S_OK)
	{
		engine_basic::engine_fail_reason error_message(hr, "deffered light draw error when setting view position");
		return error_message;
	}
	engine_basic::engine_fail_reason succeed;
	return succeed;
}
engine_basic::engine_fail_reason light_defered_draw::set_trans_world(XMFLOAT4X4 *mat_need)
{
	auto check_error = set_matrix(world_matrix_handle, mat_need);
	if (!check_error.check_if_failed())
	{
		return check_error;
	}
	engine_basic::engine_fail_reason succeed;
	return succeed;
}
engine_basic::engine_fail_reason light_defered_draw::set_trans_ssao(XMFLOAT4X4 *mat_need)
{
	auto check_error = set_matrix(ssao_matrix_handle, mat_need);;
	if (!check_error.check_if_failed())
	{
		return check_error;
	}
	engine_basic::engine_fail_reason succeed;
	return succeed;
}
engine_basic::engine_fail_reason light_defered_draw::set_trans_all(XMFLOAT4X4 *mat_need)
{
	auto check_error = set_matrix(final_matrix_handle, mat_need);;
	if (!check_error.check_if_failed())
	{
		return check_error;
	}
	engine_basic::engine_fail_reason succeed;
	return succeed;
}
engine_basic::engine_fail_reason light_defered_draw::set_trans_viewproj(XMFLOAT4X4 *mat_need)
{
	auto check_error = set_matrix(viewproj_matrix_handle, mat_need);;
	if (!check_error.check_if_failed())
	{
		return check_error;
	}
	engine_basic::engine_fail_reason succeed;
	return succeed;
}
engine_basic::engine_fail_reason light_defered_draw::set_world_matrix_array(const XMFLOAT4X4* M, int cnt)
{
	HRESULT hr = world_matrix_array_handle->SetMatrixArray(reinterpret_cast<const float*>(M), 0, cnt);
	if (FAILED(hr))
	{
		engine_basic::engine_fail_reason error_message(hr, "deffered light draw error when setting world_matrix");
		return error_message;
	}
	engine_basic::engine_fail_reason succeed;
	return succeed;
}
engine_basic::engine_fail_reason light_defered_draw::set_material(pancy_material material_in)
{
	HRESULT hr = material_need->SetRawValue(&material_in, 0, sizeof(material_in));
	if (FAILED(hr))
	{
		engine_basic::engine_fail_reason error_message(hr, "deffered light draw error when set material");
		return error_message;
	}
	engine_basic::engine_fail_reason succeed;
	return succeed;
}
engine_basic::engine_fail_reason light_defered_draw::set_ssaotex(ID3D11ShaderResourceView *tex_in)
{
	HRESULT hr = texture_ssao_handle->SetResource(tex_in);
	if (FAILED(hr))
	{
		engine_basic::engine_fail_reason error_message(hr, "deffered light draw error when set ssaotex");
		return error_message;
	}
	engine_basic::engine_fail_reason succeed;
	return succeed;
}
engine_basic::engine_fail_reason light_defered_draw::set_tex_diffuse_array(ID3D11ShaderResourceView *tex_in)
{
	HRESULT hr = texture_diffuse_handle->SetResource(tex_in);
	if (FAILED(hr))
	{
		engine_basic::engine_fail_reason error_message(hr, "deffered light draw error when set diffusetex");
		return error_message;
	}
	engine_basic::engine_fail_reason succeed;
	return succeed;
}
engine_basic::engine_fail_reason light_defered_draw::set_diffuse_light_tex(ID3D11ShaderResourceView *tex_in)
{
	HRESULT hr = tex_light_diffuse_handle->SetResource(tex_in);
	if (FAILED(hr))
	{
		engine_basic::engine_fail_reason error_message(hr, "deffered light draw error when set diffuse light tex");
		return error_message;
	}
	engine_basic::engine_fail_reason succeed;
	return succeed;
}
engine_basic::engine_fail_reason light_defered_draw::set_specular_light_tex(ID3D11ShaderResourceView *tex_in)
{
	HRESULT hr = tex_light_specular_handle->SetResource(tex_in);
	if (FAILED(hr))
	{
		engine_basic::engine_fail_reason error_message(hr, "deffered light draw error when set specular light tex");
		return error_message;
	}
	engine_basic::engine_fail_reason succeed;
	return succeed;
}
void light_defered_draw::release()
{
	release_basic();
}
void light_defered_draw::set_inputpoint_desc(D3D11_INPUT_ELEMENT_DESC *member_point, UINT *num_member)
{
	//���ö�������
	D3D11_INPUT_ELEMENT_DESC rec[] =
	{
		//������    ��������      ���ݸ�ʽ          ����� ��ʼ��ַ     ����۵ĸ�ʽ 
		{ "POSITION",0  ,DXGI_FORMAT_R32G32B32_FLOAT   ,0    ,0  ,D3D11_INPUT_PER_VERTEX_DATA  ,0 },
		{ "NORMAL"  ,0  ,DXGI_FORMAT_R32G32B32_FLOAT   ,0    ,12 ,D3D11_INPUT_PER_VERTEX_DATA  ,0 },
		{ "TANGENT" ,0  ,DXGI_FORMAT_R32G32B32_FLOAT   ,0    ,24 ,D3D11_INPUT_PER_VERTEX_DATA  ,0 },
		{ "TEXINDICES" ,0  ,DXGI_FORMAT_R32G32B32A32_UINT  ,0    ,36 ,D3D11_INPUT_PER_VERTEX_DATA  ,0 },
		{ "TEXDIFFNORM",0  ,DXGI_FORMAT_R32G32B32A32_FLOAT      ,0    ,52 ,D3D11_INPUT_PER_VERTEX_DATA  ,0 },
		{ "TEXOTHER",0  ,DXGI_FORMAT_R32G32B32A32_FLOAT      ,0    ,64 ,D3D11_INPUT_PER_VERTEX_DATA  ,0 }
	};
	*num_member = sizeof(rec) / sizeof(D3D11_INPUT_ELEMENT_DESC);
	for (UINT i = 0; i < *num_member; ++i)
	{
		member_point[i] = rec[i];
	}
}
void light_defered_draw::init_handle()
{
	texture_diffuse_handle = fx_need->GetVariableByName("texture_pack_array")->AsShaderResource();  //shader�е�������Դ���
	tex_light_diffuse_handle = fx_need->GetVariableByName("texture_light_diffuse")->AsShaderResource();    //������ͼ����
	tex_light_specular_handle = fx_need->GetVariableByName("texture_light_specular")->AsShaderResource();    //��Ӱ��ͼ���
	texture_ssao_handle = fx_need->GetVariableByName("texture_ssao")->AsShaderResource();        //��������ͼ���
	//���α任���
	view_pos_handle = fx_need->GetVariableByName("position_view");
	world_matrix_handle = fx_need->GetVariableByName("world_matrix")->AsMatrix();           //����任���
	final_matrix_handle = fx_need->GetVariableByName("final_matrix")->AsMatrix();         //ȫ�׼��α任���
	ssao_matrix_handle = fx_need->GetVariableByName("ssao_matrix")->AsMatrix();             //ssao����任���
	world_matrix_array_handle = fx_need->GetVariableByName("world_matrix_array")->AsMatrix();//����任�����
	viewproj_matrix_handle = fx_need->GetVariableByName("view_proj_matrix")->AsMatrix();   //ȡ��*ͶӰ�任����
	material_need = fx_need->GetVariableByName("material_need");
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~��¼cubemap����alpha����~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
shader_reflect_save_depth::shader_reflect_save_depth(LPCWSTR filename) :shader_basic(filename)
{
}
engine_basic::engine_fail_reason shader_reflect_save_depth::set_depthtex_input(ID3D11ShaderResourceView *tex_in)
{
	HRESULT hr = depth_input->SetResource(tex_in);
	if (FAILED(hr))
	{
		engine_basic::engine_fail_reason error_message(hr, "set cube depth error in reflect depth cube save shader");
		return error_message;
	}
	engine_basic::engine_fail_reason succeed;
	return succeed;
}
void shader_reflect_save_depth::release()
{
	release_basic();
}
void shader_reflect_save_depth::init_handle()
{
	//������Ϣ���
	depth_input = fx_need->GetVariableByName("depth_input")->AsShaderResource();
	cube_count_handle = fx_need->GetVariableByName("cube_count");
}
engine_basic::engine_fail_reason shader_reflect_save_depth::set_cube_count(XMFLOAT3 cube_count)
{
	HRESULT hr = cube_count_handle->SetRawValue((void*)&cube_count, 0, sizeof(cube_count));
	if (hr != S_OK)
	{
		engine_basic::engine_fail_reason error_message(hr, "set cube count error in reflect depth cube save shader");
		return error_message;
	}
	engine_basic::engine_fail_reason succeed;
	return succeed;
}
void shader_reflect_save_depth::set_inputpoint_desc(D3D11_INPUT_ELEMENT_DESC *member_point, UINT *num_member)
{
	//���ö�������
	D3D11_INPUT_ELEMENT_DESC rec[] =
	{
		//������    ��������      ���ݸ�ʽ          ����� ��ʼ��ַ     ����۵ĸ�ʽ 
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	*num_member = sizeof(rec) / sizeof(D3D11_INPUT_ELEMENT_DESC);
	for (UINT i = 0; i < *num_member; ++i)
	{
		member_point[i] = rec[i];
	}
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ʵʱȫ�ַ���~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
rtgr_reflect::rtgr_reflect(LPCWSTR filename) :shader_basic(filename)
{
}
void rtgr_reflect::init_handle()
{
	view_pos_handle = fx_need->GetVariableByName("view_position");
	view_matrix_handle = fx_need->GetVariableByName("view_matrix")->AsMatrix();       //ȡ���任���	
	ViewToTexSpace = fx_need->GetVariableByName("gViewToTexSpace")->AsMatrix();
	FrustumCorners = fx_need->GetVariableByName("gFrustumCorners")->AsVector();
	invview_matrix_handle = fx_need->GetVariableByName("invview_matrix")->AsMatrix(); //ȡ���任��任���
	cubeview_matrix_handle = fx_need->GetVariableByName("view_matrix_cube")->AsMatrix();
	NormalDepthMap = fx_need->GetVariableByName("gNormalDepthMap")->AsShaderResource();
	DepthMap = fx_need->GetVariableByName("gdepth_map")->AsShaderResource();
	texture_diffuse_handle = fx_need->GetVariableByName("gcolorMap")->AsShaderResource();
	texture_cube_handle = fx_need->GetVariableByName("texture_cube")->AsShaderResource();
	//texture_depthcube_handle = fx_need->GetVariableByName("depth_cube")->AsShaderResource();
	texture_stencilcube_handle = fx_need->GetVariableByName("stencil_cube")->AsShaderResource();
	texture_color_mask = fx_need->GetVariableByName("mask_input")->AsShaderResource();;
	texture_color_ssr = fx_need->GetVariableByName("ssrcolor_input")->AsShaderResource();;
	camera_positions = fx_need->GetVariableByName("center_position")->AsVector();
}
void rtgr_reflect::release()
{
	release_basic();
}
engine_basic::engine_fail_reason rtgr_reflect::set_ViewToTexSpace(XMFLOAT4X4 *mat)
{
	auto check_error = set_matrix(ViewToTexSpace, mat);
	if (!check_error.check_if_failed())
	{
		return check_error;
	}
	engine_basic::engine_fail_reason succeed;
	return succeed;
}
engine_basic::engine_fail_reason rtgr_reflect::set_view_matrix(XMFLOAT4X4 *mat_need)
{
	auto check_error = set_matrix(view_matrix_handle, mat_need);;
	if (!check_error.check_if_failed())
	{
		return check_error;
	}
	engine_basic::engine_fail_reason succeed;
	return succeed;
}
engine_basic::engine_fail_reason rtgr_reflect::set_view_pos(XMFLOAT3 eye_pos)
{
	HRESULT hr = view_pos_handle->SetRawValue((void*)&eye_pos, 0, sizeof(eye_pos));
	if (hr != S_OK)
	{
		engine_basic::engine_fail_reason error_message(hr, "rtgr reflect error when setting view position");
		return error_message;
	}
	engine_basic::engine_fail_reason succeed;
	return succeed;
}
engine_basic::engine_fail_reason rtgr_reflect::set_FrustumCorners(const XMFLOAT4 v[4])
{
	HRESULT hr = FrustumCorners->SetFloatVectorArray(reinterpret_cast<const float*>(v), 0, 4);
	if (FAILED(hr))
	{
		engine_basic::engine_fail_reason error_message(hr, "rtgr reflect error when setting FrustumCorners");
		return error_message;
	}
	engine_basic::engine_fail_reason succeed;
	return succeed;
}
engine_basic::engine_fail_reason rtgr_reflect::set_camera_positions(XMFLOAT3 v)
{
	HRESULT hr = camera_positions->SetRawValue((void*)&v, 0, sizeof(v));
	if (hr != S_OK)
	{
		engine_basic::engine_fail_reason error_message(hr, "rtgr reflect error when setting view position");
		return error_message;
	}
	engine_basic::engine_fail_reason succeed;
	return succeed;
}
engine_basic::engine_fail_reason rtgr_reflect::set_NormalDepthtex(ID3D11ShaderResourceView* srv)
{
	HRESULT hr = NormalDepthMap->SetResource(srv);
	if (FAILED(hr))
	{
		engine_basic::engine_fail_reason error_message(hr, "rtgr reflect error when setting NormalDepthtex");
		return error_message;
	}
	engine_basic::engine_fail_reason succeed;
	return succeed;
}
engine_basic::engine_fail_reason rtgr_reflect::set_Depthtex(ID3D11ShaderResourceView* srv)
{
	HRESULT hr = DepthMap->SetResource(srv);
	if (FAILED(hr))
	{
		engine_basic::engine_fail_reason error_message(hr, "rtgr reflect error when setting Depthtex");
		return error_message;
	}
	engine_basic::engine_fail_reason succeed;
	return succeed;
}
engine_basic::engine_fail_reason rtgr_reflect::set_diffusetex(ID3D11ShaderResourceView *tex_in)
{
	HRESULT hr = texture_diffuse_handle->SetResource(tex_in);
	if (hr != S_OK)
	{
		engine_basic::engine_fail_reason error_message(hr, "rtgr reflect error when setting diffuse texture");
		return error_message;
	}
	engine_basic::engine_fail_reason succeed;
	return succeed;
}
engine_basic::engine_fail_reason rtgr_reflect::set_enviroment_tex(ID3D11ShaderResourceView* srv)
{
	HRESULT hr = texture_cube_handle->SetResource(srv);
	if (hr != S_OK)
	{
		engine_basic::engine_fail_reason error_message(hr, "rtgr reflect error when setting cube texture");
		return error_message;
	}
	engine_basic::engine_fail_reason succeed;
	return succeed;
}
engine_basic::engine_fail_reason rtgr_reflect::set_color_mask_tex(ID3D11ShaderResourceView* srv)
{
	HRESULT hr = texture_color_mask->SetResource(srv);
	if (hr != S_OK)
	{
		engine_basic::engine_fail_reason error_message(hr, "rtgr reflect error when setting color_mask_tex");
		return error_message;
	}
	engine_basic::engine_fail_reason succeed;
	return succeed;
}
engine_basic::engine_fail_reason rtgr_reflect::set_color_ssr_tex(ID3D11ShaderResourceView* srv)
{
	HRESULT hr = texture_color_ssr->SetResource(srv);
	if (hr != S_OK)
	{
		engine_basic::engine_fail_reason error_message(hr, "rtgr reflect error when setting color_ssr_tex");
		return error_message;
	}
	engine_basic::engine_fail_reason succeed;
	return succeed;
}
engine_basic::engine_fail_reason rtgr_reflect::set_enviroment_stencil(ID3D11ShaderResourceView* srv)
{
	HRESULT hr = texture_stencilcube_handle->SetResource(srv);
	if (hr != S_OK)
	{
		engine_basic::engine_fail_reason error_message(hr, "rtgr reflect error when setting enviroment_stencil tex");
		return error_message;
	}
	engine_basic::engine_fail_reason succeed;
	return succeed;
}
engine_basic::engine_fail_reason rtgr_reflect::set_invview_matrix(XMFLOAT4X4 *mat_need)
{
	auto check_error = set_matrix(invview_matrix_handle, mat_need);;
	if (!check_error.check_if_failed())
	{
		return check_error;
	}
	engine_basic::engine_fail_reason succeed;
	return succeed;
}
engine_basic::engine_fail_reason rtgr_reflect::set_cubeview_matrix(const XMFLOAT4X4* M, int cnt)
{
	HRESULT hr = cubeview_matrix_handle->SetMatrixArray(reinterpret_cast<const float*>(M), 0, cnt);
	if (FAILED(hr))
	{
		engine_basic::engine_fail_reason error_message(hr, "rtgr reflect error when setting cubeview_matrix");
		return error_message;
	}
	engine_basic::engine_fail_reason succeed;
	return succeed;
}
void rtgr_reflect::set_inputpoint_desc(D3D11_INPUT_ELEMENT_DESC *member_point, UINT *num_member)
{
	//���ö�������
	D3D11_INPUT_ELEMENT_DESC rec[] =
	{
		//������    ��������      ���ݸ�ʽ          ����� ��ʼ��ַ     ����۵ĸ�ʽ 
		{ "POSITION",0  ,DXGI_FORMAT_R32G32B32_FLOAT   ,0    ,0  ,D3D11_INPUT_PER_VERTEX_DATA  ,0 },
		{ "NORMAL"  ,0  ,DXGI_FORMAT_R32G32B32_FLOAT   ,0    ,12 ,D3D11_INPUT_PER_VERTEX_DATA  ,0 },
		{ "TEXCOORD",0  ,DXGI_FORMAT_R32G32_FLOAT      ,0    ,24 ,D3D11_INPUT_PER_VERTEX_DATA  ,0 }
	};
	*num_member = sizeof(rec) / sizeof(D3D11_INPUT_ELEMENT_DESC);
	for (UINT i = 0; i < *num_member; ++i)
	{
		member_point[i] = rec[i];
	}
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~����Ч��ģ��~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
rtgr_reflect_blur::rtgr_reflect_blur(LPCWSTR filename) : shader_basic(filename)
{
}
engine_basic::engine_fail_reason rtgr_reflect_blur::set_tex_resource(ID3D11ShaderResourceView *buffer_input)
{
	HRESULT hr;
	hr = tex_input->SetResource(buffer_input);
	if (FAILED(hr))
	{
		engine_basic::engine_fail_reason error_message(hr, "set reflect blur texture error in globel reflect blur shader");
		return error_message;
	}
	engine_basic::engine_fail_reason succeed;
	return succeed;
}
engine_basic::engine_fail_reason rtgr_reflect_blur::set_image_size(XMFLOAT4 range)
{
	HRESULT hr;
	hr = Texelrange->SetRawValue((void*)&range, 0, sizeof(range));
	if (FAILED(hr))
	{
		engine_basic::engine_fail_reason error_message(hr, "set image_size error in globel reflect blur shader");
		return error_message;
	}
	engine_basic::engine_fail_reason succeed;
	return succeed;
}
void rtgr_reflect_blur::release()
{
	release_basic();
}
void rtgr_reflect_blur::init_handle()
{
	Texelrange = fx_need->GetVariableByName("tex_range_color_normal");
	tex_input = fx_need->GetVariableByName("gInputImage")->AsShaderResource();
	tex_normal_input = fx_need->GetVariableByName("normal_tex")->AsShaderResource();
	tex_depth_input = fx_need->GetVariableByName("depth_tex")->AsShaderResource();
	tex_mask_input = fx_need->GetVariableByName("gInputMask")->AsShaderResource();
}
void rtgr_reflect_blur::set_inputpoint_desc(D3D11_INPUT_ELEMENT_DESC *member_point, UINT *num_member)
{
	//���ö�������
	D3D11_INPUT_ELEMENT_DESC rec[] =
	{
		//������    ��������      ���ݸ�ʽ          ����� ��ʼ��ַ     ����۵ĸ�ʽ 
		{ "POSITION",0  ,DXGI_FORMAT_R32G32B32_FLOAT   ,0    ,0  ,D3D11_INPUT_PER_VERTEX_DATA  ,0 },
		{ "NORMAL"  ,0  ,DXGI_FORMAT_R32G32B32_FLOAT   ,0    ,12 ,D3D11_INPUT_PER_VERTEX_DATA  ,0 },
		{ "TEXCOORD",0  ,DXGI_FORMAT_R32G32_FLOAT      ,0    ,24 ,D3D11_INPUT_PER_VERTEX_DATA  ,0 }
	};
	*num_member = sizeof(rec) / sizeof(D3D11_INPUT_ELEMENT_DESC);
	for (UINT i = 0; i < *num_member; ++i)
	{
		member_point[i] = rec[i];
	}
}
engine_basic::engine_fail_reason rtgr_reflect_blur::set_tex_normal_resource(ID3D11ShaderResourceView *buffer_input)
{
	HRESULT hr;
	hr = tex_normal_input->SetResource(buffer_input);
	if (FAILED(hr))
	{
		engine_basic::engine_fail_reason error_message(hr, "set tex_normal_resource error in globel reflect blur shader");
		return error_message;
	}
	engine_basic::engine_fail_reason succeed;
	return succeed;
}
engine_basic::engine_fail_reason rtgr_reflect_blur::set_tex_depth_resource(ID3D11ShaderResourceView *buffer_input)
{
	HRESULT hr;
	hr = tex_depth_input->SetResource(buffer_input);
	if (FAILED(hr))
	{
		engine_basic::engine_fail_reason error_message(hr, "set tex_depth_resource error in globel reflect blur shader");
		return error_message;
	}
	engine_basic::engine_fail_reason succeed;
	return succeed;
}
engine_basic::engine_fail_reason rtgr_reflect_blur::set_tex_mask_resource(ID3D11ShaderResourceView *buffer_input)
{
	HRESULT hr;
	hr = tex_mask_input->SetResource(buffer_input);
	if (FAILED(hr))
	{
		engine_basic::engine_fail_reason error_message(hr, "set tex_mask error in globel reflect blur shader");
		return error_message;
	}
	engine_basic::engine_fail_reason succeed;
	return succeed;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~���շ������~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
rtgr_reflect_final::rtgr_reflect_final(LPCWSTR filename) : shader_basic(filename)
{
}
void rtgr_reflect_final::release()
{
	release_basic();
}
void rtgr_reflect_final::init_handle()
{
	Texelrange = fx_need->GetVariableByName("tex_range_color_normal");
	tex_color_input = fx_need->GetVariableByName("gInputImage")->AsShaderResource();
	tex_reflect_input = fx_need->GetVariableByName("gInputReflect")->AsShaderResource();
}
void rtgr_reflect_final::set_inputpoint_desc(D3D11_INPUT_ELEMENT_DESC *member_point, UINT *num_member)
{
	//���ö�������
	D3D11_INPUT_ELEMENT_DESC rec[] =
	{
		//������    ��������      ���ݸ�ʽ          ����� ��ʼ��ַ     ����۵ĸ�ʽ 
		{ "POSITION",0  ,DXGI_FORMAT_R32G32B32_FLOAT   ,0    ,0  ,D3D11_INPUT_PER_VERTEX_DATA  ,0 },
		{ "NORMAL"  ,0  ,DXGI_FORMAT_R32G32B32_FLOAT   ,0    ,12 ,D3D11_INPUT_PER_VERTEX_DATA  ,0 },
		{ "TEXCOORD",0  ,DXGI_FORMAT_R32G32_FLOAT      ,0    ,24 ,D3D11_INPUT_PER_VERTEX_DATA  ,0 }
	};
	*num_member = sizeof(rec) / sizeof(D3D11_INPUT_ELEMENT_DESC);
	for (UINT i = 0; i < *num_member; ++i)
	{
		member_point[i] = rec[i];
	}
}
engine_basic::engine_fail_reason rtgr_reflect_final::set_tex_color_resource(ID3D11ShaderResourceView *buffer_input)
{
	HRESULT hr;
	hr = tex_color_input->SetResource(buffer_input);
	if (FAILED(hr))
	{
		engine_basic::engine_fail_reason error_message(hr, "set color input error in globel reflect final shader");
		return error_message;
	}
	engine_basic::engine_fail_reason succeed;
	return succeed;
}
engine_basic::engine_fail_reason rtgr_reflect_final::set_tex_reflect_resource(ID3D11ShaderResourceView *buffer_input)
{
	HRESULT hr;
	hr = tex_reflect_input->SetResource(buffer_input);
	if (FAILED(hr))
	{
		engine_basic::engine_fail_reason error_message(hr, "set tex_reflect_resource input error in globel reflect final shader");
		return error_message;
	}
	engine_basic::engine_fail_reason succeed;
	return succeed;
}
engine_basic::engine_fail_reason rtgr_reflect_final::set_image_size(XMFLOAT4 range)
{
	HRESULT hr;
	hr = Texelrange->SetRawValue((void*)&range, 0, sizeof(range));
	if (FAILED(hr))
	{
		engine_basic::engine_fail_reason error_message(hr, "set image_size input error in globel reflect final shader");
		return error_message;
	}
	engine_basic::engine_fail_reason succeed;
	return succeed;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~���������ӳ��~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
shader_skycube::shader_skycube(LPCWSTR filename) : shader_basic(filename)
{
}
engine_basic::engine_fail_reason shader_skycube::set_view_pos(XMFLOAT3 eye_pos)
{
	HRESULT hr = view_pos_handle->SetRawValue((void*)&eye_pos, 0, sizeof(eye_pos));
	if (hr != S_OK)
	{
		engine_basic::engine_fail_reason error_message(hr, "cubemap shader error when setting view position");
		return error_message;
	}
	engine_basic::engine_fail_reason succeed;
	return succeed;
}
engine_basic::engine_fail_reason shader_skycube::set_trans_world(XMFLOAT4X4 *mat_need)
{
	XMMATRIX rec_mat = XMLoadFloat4x4(mat_need);
	XMVECTOR x_delta;
	XMMATRIX check = rec_mat;
	//���߱任
	XMMATRIX normal_need = DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(&x_delta, check));
	normal_need.r[0].m128_f32[3] = 0.0f;
	normal_need.r[1].m128_f32[3] = 0.0f;
	normal_need.r[2].m128_f32[3] = 0.0f;
	normal_need.r[3].m128_f32[3] = 1.0f;
	auto check_error = set_matrix(world_matrix_handle, mat_need);
	if (!check_error.check_if_failed())
	{
		return check_error;
	}
	HRESULT hr = normal_matrix_handle->SetMatrix(reinterpret_cast<float*>(&normal_need));
	if (hr != S_OK)
	{
		engine_basic::engine_fail_reason error_message(hr, "cubemap shader error when setting normal matrix");
		return error_message;
	}
	engine_basic::engine_fail_reason succeed;
	return succeed;
}
engine_basic::engine_fail_reason shader_skycube::set_trans_all(XMFLOAT4X4 *mat_need)
{
	auto check_error = set_matrix(project_matrix_handle, mat_need);;
	if (!check_error.check_if_failed())
	{
		return check_error;
	}
	engine_basic::engine_fail_reason succeed;
	return succeed;
}
engine_basic::engine_fail_reason shader_skycube::set_tex_resource(ID3D11ShaderResourceView* tex_cube)
{
	HRESULT hr;
	hr = cubemap_texture->SetResource(tex_cube);
	if (FAILED(hr))
	{
		engine_basic::engine_fail_reason error_message(hr, "cubemap shader error when setting cube texture");
		return error_message;
	}
	engine_basic::engine_fail_reason succeed;
	return succeed;
}
void shader_skycube::init_handle()
{
	project_matrix_handle = fx_need->GetVariableByName("final_matrix")->AsMatrix();         //ȫ�׼��α任���
	world_matrix_handle = fx_need->GetVariableByName("world_matrix")->AsMatrix();           //����任���
	normal_matrix_handle = fx_need->GetVariableByName("normal_matrix")->AsMatrix();         //���߱任���
	view_pos_handle = fx_need->GetVariableByName("position_view");
	cubemap_texture = fx_need->GetVariableByName("texture_cube")->AsShaderResource();  //shader�е�������Դ���
}
void shader_skycube::release()
{
	release_basic();
}
void shader_skycube::set_inputpoint_desc(D3D11_INPUT_ELEMENT_DESC *member_point, UINT *num_member)
{
	//���ö�������
	D3D11_INPUT_ELEMENT_DESC rec[] =
	{
		//������    ��������      ���ݸ�ʽ          ����� ��ʼ��ַ     ����۵ĸ�ʽ 
		{ "POSITION"   ,0  ,DXGI_FORMAT_R32G32B32_FLOAT     ,0    ,0  ,D3D11_INPUT_PER_VERTEX_DATA  ,0 },
		{ "NORMAL"     ,0  ,DXGI_FORMAT_R32G32B32_FLOAT     ,0    ,12 ,D3D11_INPUT_PER_VERTEX_DATA  ,0 },
		{ "TANGENT"    ,0  ,DXGI_FORMAT_R32G32B32_FLOAT     ,0    ,24 ,D3D11_INPUT_PER_VERTEX_DATA  ,0 },
		{ "TEXINDICES" ,0  ,DXGI_FORMAT_R32G32B32A32_UINT   ,0    ,36 ,D3D11_INPUT_PER_VERTEX_DATA  ,0 },
		{ "TEXDIFFNORM",0  ,DXGI_FORMAT_R32G32B32A32_FLOAT  ,0    ,52 ,D3D11_INPUT_PER_VERTEX_DATA  ,0 },
		{ "TEXOTHER"   ,0  ,DXGI_FORMAT_R32G32B32A32_FLOAT  ,0    ,64 ,D3D11_INPUT_PER_VERTEX_DATA  ,0 }
	};
	*num_member = sizeof(rec) / sizeof(D3D11_INPUT_ELEMENT_DESC);
	for (UINT i = 0; i < *num_member; ++i)
	{
		member_point[i] = rec[i];
	}
}
//shader������
shader_control *shader_control::shadercontrol_pInstance = NULL;
shader_control::shader_control() 
{
}
engine_basic::engine_fail_reason shader_control::add_a_new_shader(std::type_index class_type, std::shared_ptr<shader_basic> shader_in)
{
	if (!shader_in)
	{
		engine_basic::engine_fail_reason failed_message(std::string("the shader") + class_type.name() + "instance is NULL");
		return failed_message;
	}
	std::pair<std::string, std::shared_ptr<shader_basic>> data_needB(class_type.name(), shader_in);
	auto check_iferror = shader_list.insert(data_needB);
	if (!check_iferror.second)
	{
		engine_basic::engine_fail_reason failed_message(std::string("a repeat shader") + class_type.name() + "is already init");
		return failed_message;
	}
	engine_basic::engine_fail_reason succeed;
	return succeed;
}
engine_basic::engine_fail_reason shader_control::init()
{
	engine_basic::engine_fail_reason check_error = init_basic();
	if (check_error.check_if_failed())
	{
		return check_error;
	}
	engine_basic::engine_fail_reason succeed;
	return succeed;
}
engine_basic::engine_fail_reason shader_control::init_basic()
{
	/*
	std::shared_ptr<color_shader> shader_color_test = std::make_shared<color_shader>(L"F:\\Microsoft Visual Studio\\pancystar_engine2.0\\pancystar_engine2\\texturearray_package\\Release\\color_test.cso");
	engine_basic::engine_fail_reason check_error = shader_color_test->shder_create();
	if (!check_error.check_if_failed())
	{
		return check_error;
	}
	check_error = add_a_new_shader(std::type_index(typeid(color_shader)), shader_color_test);
	if (!check_error.check_if_failed())
	{
		return check_error;
	}


	std::shared_ptr<virtual_light_shader> shader_vlight_test = std::make_shared<virtual_light_shader>(L"F:\\Microsoft Visual Studio\\pancystar_engine2.0\\pancystar_engine2\\texturearray_package\\Release\\virtual_light.cso");
	check_error = shader_vlight_test->shder_create();
	if (!check_error.check_if_failed())
	{
		return check_error;
	}
	check_error = add_a_new_shader(std::type_index(typeid(virtual_light_shader)), shader_vlight_test);
	if (!check_error.check_if_failed())
	{
		return check_error;
	}

	std::shared_ptr<picture_show_shader> shader_picture_test = std::make_shared<picture_show_shader>(L"F:\\Microsoft Visual Studio\\pancystar_engine2.0\\pancystar_engine2\\texturearray_package\\Release\\show_pic.cso");
	check_error = shader_picture_test->shder_create();
	if (!check_error.check_if_failed())
	{
		return check_error;
	}
	check_error = add_a_new_shader(std::type_index(typeid(picture_show_shader)), shader_picture_test);
	if (!check_error.check_if_failed())
	{
		return check_error;
	}

	std::shared_ptr<shader_pretreat_gbuffer> shader_gbuffer = std::make_shared<shader_pretreat_gbuffer>(L"F:\\Microsoft Visual Studio\\pancystar_engine2.0\\pancystar_engine2\\texturearray_package\\Release\\gbuffer_pretreat.cso");
	check_error = shader_gbuffer->shder_create();
	if (!check_error.check_if_failed())
	{
		return check_error;
	}
	check_error = add_a_new_shader(std::type_index(typeid(shader_pretreat_gbuffer)), shader_gbuffer);
	if (!check_error.check_if_failed())
	{
		return check_error;
	}

	std::shared_ptr<shader_resolvedepth> shader_resolvemassdepth = std::make_shared<shader_resolvedepth>(L"F:\\Microsoft Visual Studio\\pancystar_engine2.0\\pancystar_engine2\\texturearray_package\\Release\\ResolveMSAAdepthstencil.cso");
	check_error = shader_resolvemassdepth->shder_create();
	if (!check_error.check_if_failed())
	{
		return check_error;
	}
	check_error = add_a_new_shader(std::type_index(typeid(shader_resolvedepth)), shader_resolvemassdepth);
	if (!check_error.check_if_failed())
	{
		return check_error;
	}

	std::shared_ptr<shader_ssaomap> shader_ssaomap_rec = std::make_shared<shader_ssaomap>(L"F:\\Microsoft Visual Studio\\pancystar_engine2.0\\pancystar_engine2\\texturearray_package\\Release\\ssao_draw_aomap.cso");
	check_error = shader_ssaomap_rec->shder_create();
	if (!check_error.check_if_failed())
	{
		return check_error;
	}
	check_error = add_a_new_shader(std::type_index(typeid(shader_ssaomap)), shader_ssaomap_rec);
	if (!check_error.check_if_failed())
	{
		return check_error;
	}

	std::shared_ptr<shader_ssaoblur> shader_ssaoblur_rec = std::make_shared<shader_ssaoblur>(L"F:\\Microsoft Visual Studio\\pancystar_engine2.0\\pancystar_engine2\\texturearray_package\\Release\\ssao_blur_map.cso");
	check_error = shader_ssaoblur_rec->shder_create();
	if (!check_error.check_if_failed())
	{
		return check_error;
	}
	check_error = add_a_new_shader(std::type_index(typeid(shader_ssaoblur)), shader_ssaoblur_rec);
	if (!check_error.check_if_failed())
	{
		return check_error;
	}

	std::shared_ptr<light_shadow> shader_shadowmap = std::make_shared<light_shadow>(L"F:\\Microsoft Visual Studio\\pancystar_engine2.0\\pancystar_engine2\\texturearray_package\\Release\\shadowmap.cso");
	check_error = shader_shadowmap->shder_create();
	if (!check_error.check_if_failed())
	{
		return check_error;
	}
	check_error = add_a_new_shader(std::type_index(typeid(light_shadow)), shader_shadowmap);
	if (!check_error.check_if_failed())
	{
		return check_error;
	}

	std::shared_ptr<light_defered_lightbuffer> shader_defered_lightbuffer = std::make_shared<light_defered_lightbuffer>(L"F:\\Microsoft Visual Studio\\pancystar_engine2.0\\pancystar_engine2\\texturearray_package\\Release\\light_buffer_pretreat.cso");
	check_error = shader_defered_lightbuffer->shder_create();
	if (!check_error.check_if_failed())
	{
		return check_error;
	}
	check_error = add_a_new_shader(std::type_index(typeid(light_defered_lightbuffer)), shader_defered_lightbuffer);
	if (!check_error.check_if_failed())
	{
		return check_error;
	}
	*/
	std::shared_ptr<color_shader> shader_color_test = std::make_shared<color_shader>(L"F:\\Microsoft Visual Studio\\pancystar_engine2.0\\pancystar_engine2\\texturearray_package\\Debug\\color_test.cso");
	engine_basic::engine_fail_reason check_error = shader_color_test->shder_create();
	if (!check_error.check_if_failed())
	{
		return check_error;
	}
	check_error = add_a_new_shader(std::type_index(typeid(color_shader)), shader_color_test);
	if (!check_error.check_if_failed())
	{
		return check_error;
	}


	std::shared_ptr<virtual_light_shader> shader_vlight_test = std::make_shared<virtual_light_shader>(L"F:\\Microsoft Visual Studio\\pancystar_engine2.0\\pancystar_engine2\\texturearray_package\\Debug\\virtual_light.cso");
	check_error = shader_vlight_test->shder_create();
	if (!check_error.check_if_failed())
	{
		return check_error;
	}
	check_error = add_a_new_shader(std::type_index(typeid(virtual_light_shader)), shader_vlight_test);
	if (!check_error.check_if_failed())
	{
		return check_error;
	}

	std::shared_ptr<picture_show_shader> shader_picture_test = std::make_shared<picture_show_shader>(L"F:\\Microsoft Visual Studio\\pancystar_engine2.0\\pancystar_engine2\\texturearray_package\\Debug\\show_pic.cso");
	check_error = shader_picture_test->shder_create();
	if (!check_error.check_if_failed())
	{
		return check_error;
	}
	check_error = add_a_new_shader(std::type_index(typeid(picture_show_shader)), shader_picture_test);
	if (!check_error.check_if_failed())
	{
		return check_error;
	}

	std::shared_ptr<shader_pretreat_gbuffer> shader_gbuffer = std::make_shared<shader_pretreat_gbuffer>(L"F:\\Microsoft Visual Studio\\pancystar_engine2.0\\pancystar_engine2\\texturearray_package\\Debug\\gbuffer_pretreat.cso");
	check_error = shader_gbuffer->shder_create();
	if (!check_error.check_if_failed())
	{
		return check_error;
	}
	check_error = add_a_new_shader(std::type_index(typeid(shader_pretreat_gbuffer)), shader_gbuffer);
	if (!check_error.check_if_failed())
	{
		return check_error;
	}

	std::shared_ptr<shader_resolvedepth> shader_resolvemassdepth = std::make_shared<shader_resolvedepth>(L"F:\\Microsoft Visual Studio\\pancystar_engine2.0\\pancystar_engine2\\texturearray_package\\Debug\\ResolveMSAAdepthstencil.cso");
	check_error = shader_resolvemassdepth->shder_create();
	if (!check_error.check_if_failed())
	{
		return check_error;
	}
	check_error = add_a_new_shader(std::type_index(typeid(shader_resolvedepth)), shader_resolvemassdepth);
	if (!check_error.check_if_failed())
	{
		return check_error;
	}

	std::shared_ptr<shader_ssaomap> shader_ssaomap_rec= std::make_shared<shader_ssaomap>(L"F:\\Microsoft Visual Studio\\pancystar_engine2.0\\pancystar_engine2\\texturearray_package\\Debug\\ssao_draw_aomap.cso");
	check_error = shader_ssaomap_rec->shder_create();
	if (!check_error.check_if_failed())
	{
		return check_error;
	}
	check_error = add_a_new_shader(std::type_index(typeid(shader_ssaomap)), shader_ssaomap_rec);
	if (!check_error.check_if_failed())
	{
		return check_error;
	}

	std::shared_ptr<shader_ssaoblur> shader_ssaoblur_rec = std::make_shared<shader_ssaoblur>(L"F:\\Microsoft Visual Studio\\pancystar_engine2.0\\pancystar_engine2\\texturearray_package\\Debug\\ssao_blur_map.cso");
	check_error = shader_ssaoblur_rec->shder_create();
	if (!check_error.check_if_failed())
	{
		return check_error;
	}
	check_error = add_a_new_shader(std::type_index(typeid(shader_ssaoblur)), shader_ssaoblur_rec);
	if (!check_error.check_if_failed())
	{
		return check_error;
	}

	std::shared_ptr<light_shadow> shader_shadowmap = std::make_shared<light_shadow>(L"F:\\Microsoft Visual Studio\\pancystar_engine2.0\\pancystar_engine2\\texturearray_package\\Debug\\shadowmap.cso");
	check_error = shader_shadowmap->shder_create();
	if (!check_error.check_if_failed())
	{
		return check_error;
	}
	check_error = add_a_new_shader(std::type_index(typeid(light_shadow)), shader_shadowmap);
	if (!check_error.check_if_failed())
	{
		return check_error;
	}

	std::shared_ptr<light_defered_lightbuffer> shader_defered_lightbuffer = std::make_shared<light_defered_lightbuffer>(L"F:\\Microsoft Visual Studio\\pancystar_engine2.0\\pancystar_engine2\\texturearray_package\\Debug\\light_buffer_pretreat.cso");
	check_error = shader_defered_lightbuffer->shder_create();
	if (!check_error.check_if_failed())
	{
	return check_error;
	}
	check_error = add_a_new_shader(std::type_index(typeid(light_defered_lightbuffer)), shader_defered_lightbuffer);
	if (!check_error.check_if_failed())
	{
	return check_error;
	}

	std::shared_ptr<light_defered_draw> shader_defered_draw = std::make_shared<light_defered_draw>(L"F:\\Microsoft Visual Studio\\pancystar_engine2.0\\pancystar_engine2\\texturearray_package\\Debug\\light_deffered.cso");
	check_error = shader_defered_draw->shder_create();
	if (!check_error.check_if_failed())
	{
		return check_error;
	}
	check_error = add_a_new_shader(std::type_index(typeid(light_defered_draw)), shader_defered_draw);
	if (!check_error.check_if_failed())
	{
		return check_error;
	}

	std::shared_ptr<shader_reflect_save_depth> shader_reflect_depthsave = std::make_shared<shader_reflect_save_depth>(L"F:\\Microsoft Visual Studio\\pancystar_engine2.0\\pancystar_engine2\\texturearray_package\\Debug\\save_cube_depthstencil.cso");
	check_error = shader_reflect_depthsave->shder_create();
	if (!check_error.check_if_failed())
	{
		return check_error;
	}
	check_error = add_a_new_shader(std::type_index(typeid(shader_reflect_save_depth)), shader_reflect_depthsave);
	if (!check_error.check_if_failed())
	{
		return check_error;
	}

	std::shared_ptr<rtgr_reflect> shader_rtgr_reflect = std::make_shared<rtgr_reflect>(L"F:\\Microsoft Visual Studio\\pancystar_engine2.0\\pancystar_engine2\\texturearray_package\\Debug\\RTGR.cso");
	check_error = shader_rtgr_reflect->shder_create();
	if (!check_error.check_if_failed())
	{
		return check_error;
	}
	check_error = add_a_new_shader(std::type_index(typeid(rtgr_reflect)), shader_rtgr_reflect);
	if (!check_error.check_if_failed())
	{
		return check_error;
	}

	std::shared_ptr<rtgr_reflect_blur> shader_rtgr_reflect_blur = std::make_shared<rtgr_reflect_blur>(L"F:\\Microsoft Visual Studio\\pancystar_engine2.0\\pancystar_engine2\\texturearray_package\\Debug\\reflect_blur.cso");
	check_error = shader_rtgr_reflect_blur->shder_create();
	if (!check_error.check_if_failed())
	{
		return check_error;
	}
	check_error = add_a_new_shader(std::type_index(typeid(rtgr_reflect_blur)), shader_rtgr_reflect_blur);
	if (!check_error.check_if_failed())
	{
		return check_error;
	}

	std::shared_ptr<rtgr_reflect_final> shader_rtgr_reflect_final = std::make_shared<rtgr_reflect_final>(L"F:\\Microsoft Visual Studio\\pancystar_engine2.0\\pancystar_engine2\\texturearray_package\\Debug\\reflect_final.cso");
	check_error = shader_rtgr_reflect_final->shder_create();
	if (!check_error.check_if_failed())
	{
		return check_error;
	}
	check_error = add_a_new_shader(std::type_index(typeid(rtgr_reflect_final)), shader_rtgr_reflect_final);
	if (!check_error.check_if_failed())
	{
		return check_error;
	}

	std::shared_ptr<shader_skycube> shader_sky_draw = std::make_shared<shader_skycube>(L"F:\\Microsoft Visual Studio\\pancystar_engine2.0\\pancystar_engine2\\texturearray_package\\Debug\\skycube.cso");
	check_error = shader_sky_draw->shder_create();
	if (!check_error.check_if_failed())
	{
		return check_error;
	}
	check_error = add_a_new_shader(std::type_index(typeid(shader_skycube)), shader_sky_draw);
	if (!check_error.check_if_failed())
	{
		return check_error;
	}
	engine_basic::engine_fail_reason succeed;
	return succeed;
}
std::shared_ptr<color_shader> shader_control::get_shader_color(engine_basic::engine_fail_reason &if_succeed)
{
	std::string name_need = std::type_index(typeid(color_shader)).name();
	auto shader_color = get_shader_by_type(std::type_index(typeid(color_shader)).name(), if_succeed);
	if (!if_succeed.check_if_failed())
	{
		return std::shared_ptr<color_shader>();
	}
	auto out_pointer = std::dynamic_pointer_cast<color_shader>(shader_color);
	return out_pointer;
}
std::shared_ptr<virtual_light_shader> shader_control::get_shader_virtual_light(engine_basic::engine_fail_reason &if_succeed)
{
	std::string name_need = std::type_index(typeid(virtual_light_shader)).name();
	auto shader_vlight = get_shader_by_type(std::type_index(typeid(virtual_light_shader)).name(), if_succeed);
	if (!if_succeed.check_if_failed())
	{
		return std::shared_ptr<virtual_light_shader>();
	}
	auto out_pointer = std::dynamic_pointer_cast<virtual_light_shader>(shader_vlight);
	return out_pointer;
}
std::shared_ptr<picture_show_shader> shader_control::get_shader_picture(engine_basic::engine_fail_reason &if_succeed)
{
	std::string name_need = std::type_index(typeid(picture_show_shader)).name();
	auto shader_vlight = get_shader_by_type(std::type_index(typeid(picture_show_shader)).name(), if_succeed);
	if (!if_succeed.check_if_failed())
	{
		return std::shared_ptr<picture_show_shader>();
	}
	auto out_pointer = std::dynamic_pointer_cast<picture_show_shader>(shader_vlight);
	return out_pointer;
}
std::shared_ptr<shader_basic> shader_control::get_shader_by_type(std::string type_name, engine_basic::engine_fail_reason &if_succeed)
{
	auto shader_out = shader_list.find(type_name)->second;
	if (!shader_out)
	{
		engine_basic::engine_fail_reason failed_message(std::string("could not find shader") + type_name);
		if_succeed = failed_message;
		return 0;
	}
	engine_basic::engine_fail_reason succeed;
	if_succeed = succeed;
	return shader_out;
}
std::shared_ptr<shader_pretreat_gbuffer> shader_control::get_shader_gbuffer(engine_basic::engine_fail_reason &if_succeed)
{
	std::string name_need = std::type_index(typeid(shader_pretreat_gbuffer)).name();
	auto shader_vlight = get_shader_by_type(std::type_index(typeid(shader_pretreat_gbuffer)).name(), if_succeed);
	if (!if_succeed.check_if_failed())
	{
		return std::shared_ptr<shader_pretreat_gbuffer>();
	}
	auto out_pointer = std::dynamic_pointer_cast<shader_pretreat_gbuffer>(shader_vlight);
	return out_pointer;
}
std::shared_ptr<shader_resolvedepth> shader_control::get_shader_resolvedepth(engine_basic::engine_fail_reason &if_succeed)
{
	std::string name_need = std::type_index(typeid(shader_resolvedepth)).name();
	auto shader_vlight = get_shader_by_type(std::type_index(typeid(shader_resolvedepth)).name(), if_succeed);
	if (!if_succeed.check_if_failed())
	{
		return std::shared_ptr<shader_resolvedepth>();
	}
	auto out_pointer = std::dynamic_pointer_cast<shader_resolvedepth>(shader_vlight);
	return out_pointer;
}
std::shared_ptr<shader_ssaomap> shader_control::get_shader_ssaodraw(engine_basic::engine_fail_reason &if_succeed) 
{
	std::string name_need = std::type_index(typeid(shader_ssaomap)).name();
	auto shader_vlight = get_shader_by_type(std::type_index(typeid(shader_ssaomap)).name(), if_succeed);
	if (!if_succeed.check_if_failed())
	{
		return std::shared_ptr<shader_ssaomap>();
	}
	auto out_pointer = std::dynamic_pointer_cast<shader_ssaomap>(shader_vlight);
	return out_pointer;
}
std::shared_ptr<shader_ssaoblur> shader_control::get_shader_ssaoblur(engine_basic::engine_fail_reason &if_succeed) 
{
	std::string name_need = std::type_index(typeid(shader_ssaoblur)).name();
	auto shader_vlight = get_shader_by_type(std::type_index(typeid(shader_ssaoblur)).name(), if_succeed);
	if (!if_succeed.check_if_failed())
	{
		return std::shared_ptr<shader_ssaoblur>();
	}
	auto out_pointer = std::dynamic_pointer_cast<shader_ssaoblur>(shader_vlight);
	return out_pointer;
}
std::shared_ptr<light_shadow> shader_control::get_shader_shadowmap(engine_basic::engine_fail_reason &if_succeed)
{
	std::string name_need = std::type_index(typeid(light_shadow)).name();
	auto shader_vlight = get_shader_by_type(std::type_index(typeid(light_shadow)).name(), if_succeed);
	if (!if_succeed.check_if_failed())
	{
		return std::shared_ptr<light_shadow>();
	}
	auto out_pointer = std::dynamic_pointer_cast<light_shadow>(shader_vlight);
	return out_pointer;
}
std::shared_ptr<light_defered_lightbuffer> shader_control::get_shader_lightbuffer(engine_basic::engine_fail_reason &if_succeed)
{
	std::string name_need = std::type_index(typeid(light_defered_lightbuffer)).name();
	auto shader_vlight = get_shader_by_type(std::type_index(typeid(light_defered_lightbuffer)).name(), if_succeed);
	if (!if_succeed.check_if_failed())
	{
		return std::shared_ptr<light_defered_lightbuffer>();
	}
	auto out_pointer = std::dynamic_pointer_cast<light_defered_lightbuffer>(shader_vlight);
	return out_pointer;
}
std::shared_ptr<light_defered_draw> shader_control::get_shader_lightdeffered(engine_basic::engine_fail_reason &if_succeed)
{
	std::string name_need = std::type_index(typeid(light_defered_draw)).name();
	auto shader_vlight = get_shader_by_type(std::type_index(typeid(light_defered_draw)).name(), if_succeed);
	if (!if_succeed.check_if_failed())
	{
		return std::shared_ptr<light_defered_draw>();
	}
	auto out_pointer = std::dynamic_pointer_cast<light_defered_draw>(shader_vlight);
	return out_pointer;
}
std::shared_ptr<shader_reflect_save_depth> shader_control::get_shader_reflect_savedepth(engine_basic::engine_fail_reason &if_succeed) 
{
	std::string name_need = std::type_index(typeid(shader_reflect_save_depth)).name();
	auto shader_vlight = get_shader_by_type(std::type_index(typeid(shader_reflect_save_depth)).name(), if_succeed);
	if (!if_succeed.check_if_failed())
	{
		return std::shared_ptr<shader_reflect_save_depth>();
	}
	auto out_pointer = std::dynamic_pointer_cast<shader_reflect_save_depth>(shader_vlight);
	return out_pointer;
}
std::shared_ptr<rtgr_reflect> shader_control::get_shader_reflect_draw(engine_basic::engine_fail_reason &if_succeed) 
{
	std::string name_need = std::type_index(typeid(rtgr_reflect)).name();
	auto shader_vlight = get_shader_by_type(std::type_index(typeid(rtgr_reflect)).name(), if_succeed);
	if (!if_succeed.check_if_failed())
	{
		return std::shared_ptr<rtgr_reflect>();
	}
	auto out_pointer = std::dynamic_pointer_cast<rtgr_reflect>(shader_vlight);
	return out_pointer;
}
std::shared_ptr<rtgr_reflect_blur> shader_control::get_shader_reflect_blur(engine_basic::engine_fail_reason &if_succeed)
{
	std::string name_need = std::type_index(typeid(rtgr_reflect_blur)).name();
	auto shader_vlight = get_shader_by_type(std::type_index(typeid(rtgr_reflect_blur)).name(), if_succeed);
	if (!if_succeed.check_if_failed())
	{
		return std::shared_ptr<rtgr_reflect_blur>();
	}
	auto out_pointer = std::dynamic_pointer_cast<rtgr_reflect_blur>(shader_vlight);
	return out_pointer;
}
std::shared_ptr<rtgr_reflect_final> shader_control::get_shader_reflect_final(engine_basic::engine_fail_reason &if_succeed)
{
	std::string name_need = std::type_index(typeid(rtgr_reflect_final)).name();
	auto shader_vlight = get_shader_by_type(std::type_index(typeid(rtgr_reflect_final)).name(), if_succeed);
	if (!if_succeed.check_if_failed())
	{
		return std::shared_ptr<rtgr_reflect_final>();
	}
	auto out_pointer = std::dynamic_pointer_cast<rtgr_reflect_final>(shader_vlight);
	return out_pointer;
}
std::shared_ptr<shader_skycube> shader_control::get_shader_sky_draw(engine_basic::engine_fail_reason &if_succeed)
{
	std::string name_need = std::type_index(typeid(shader_skycube)).name();
	auto shader_vlight = get_shader_by_type(std::type_index(typeid(shader_skycube)).name(), if_succeed);
	if (!if_succeed.check_if_failed())
	{
		return std::shared_ptr<shader_skycube>();
	}
	auto out_pointer = std::dynamic_pointer_cast<shader_skycube>(shader_vlight);
	return out_pointer;
}
void shader_control::release() 
{
	for (auto shader_data = shader_list.begin(); shader_data != shader_list.end(); ++shader_data) 
	{
		shader_data->second->release();
	}
	shader_list.clear();
}