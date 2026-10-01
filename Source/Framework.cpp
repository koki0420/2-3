#include <memory>
#include <sstream>
#include <imgui.h>

#include "Framework.h"
#include "Graphics.h"
#include "ImGuiRenderer.h"

#include"SceneGame.h"
#include"SceneResult.h"
#include"SceneTitle.h"
//#include"SceneAudioTest.h"
//#include"SceneAudioWASD.h"
#include"SceneManager.h"
#include"SceneLoading.h"

#include "shader.h"
#include "texture.h"
#include <static_mesh.h>

// ���������Ԋu�ݒ�
static const int syncInterval = 1;

//#define LIGHT 12

// �R���X�g���N�^
Framework::Framework(HWND hWnd)
	: hWnd(hWnd)
{
	Graphics::Instance().Initialize(hWnd);

	// IMGUI������
	ImGuiRenderer::Initialize(hWnd, Graphics::Instance().GetDevice(), Graphics::Instance().GetDeviceContext());

	SceneManager::Instance().ChangeScene(new SceneTitle);


	audio_device::initialize();

	
	
	{
		
		float floaLight = 13.5f;
		float floalightY = 4.75f;

		for (int i = 0; i < LIGHT_MAX; i++)
		{
			point_light[i].position = { 0,0,0,0 };
			point_light[i].color = { 0,0,0,0 };
			point_light[i].range = 0.0f;
		}

		//初期
		point_light[0].position = { 12,5,42,0 };
		point_light[0].color = { 1,1,1,1 };
		point_light[0].range = 18.0f;

		point_light[1].position = { 12,5,32,0 };
		point_light[1].color = { 1,1,1,1 };
		point_light[1].range = 18;
					
		//橋のギミック
		point_light[2].position = { 11,7,3,0 };
		point_light[2].color = { 1,1,1,1 };
		point_light[2].range = 30;

		point_light[13].position = { 11,7,-8,0 };
		point_light[13].color = { 1,1,1,1 };
		point_light[13].range = 30;

		point_light[14].position = { 11,7,15,0 };
		point_light[14].color = { 1,1,1,1 };
		point_light[14].range = 15;

		//コンテナ
		point_light[3].position = { 8,5,-18,0 };
		point_light[3].color = { 1,1,1,1 };
		point_light[3].range = 15;

		//広間
		point_light[4].position = { -2,floalightY,-15,0 };
		point_light[4].color = { 1,1,1,1 };
		point_light[4].range = floaLight;
					
		point_light[5].position = { -4,floalightY,12,0 };
		point_light[5].color = { 1,1,1,1 };
		point_light[5].range = floaLight;


		point_light[6].position = { -14,floalightY,10,0 };
		point_light[6].color = { 1,1,1,1 };
		point_light[6].range = floaLight;

		point_light[7].position = { -14,floalightY,20,0 };
		point_light[7].color = { 1,1,1,1 };
		point_light[7].range = floaLight;

		point_light[8].position = { -14,floalightY,0,0 };
		point_light[8].color = { 1,1,1,1 };
		point_light[8].range = floaLight;

		point_light[9].position = { -14,floalightY,-10,0 };
		point_light[9].color = { 1,1,1,1 };
		point_light[9].range = floaLight;
	   
		point_light[10].position = { -7,floalightY,-5,0 };
		point_light[10].color = { 1,1,1,1 };
		point_light[10].range =floaLight;

		point_light[11].position = { 0,floalightY,0,0 };
		point_light[11].color = { 1,1,1,1 };
		point_light[11].range = floaLight;

		point_light[12].position = { 2,floalightY,20,0 };
		point_light[12].color = { 1,1,1,1 };
		point_light[12].range = floaLight;


		//横穴
		point_light[15].position = { -23,5.5f,5.5f,0 };
		point_light[15].color = { 1,1,1,1 };
		point_light[15].range = 10.0f;

		point_light[16].position = { -12,5.0f,-18,0 };
		point_light[16].color = { 1,1,1,1 };
		point_light[16].range = 15.0f;


		point_light[17].position = { -9,6,38,0 };
		point_light[17].color = { 1,1,1,1 };
		point_light[17].range = 20.0f;

		//ゴールの光
		point_light[18].position = { -11,3,40,0 };
		point_light[18].color = { 1,1,1,1 };
		point_light[18].range = 35.0f;

	}

}

Framework::~Framework()
{
	// IMGUI�I����
	ImGuiRenderer::Finalize();

	SceneManager::Instance().Clear();
}

bool Framework::initialize()
{
	HRESULT hr{ S_OK };

	ID3D11Device* device = Graphics::Instance().GetDevice();

	ID3D11DeviceContext* immediate_context = Graphics::Instance().GetDeviceContext();

	

	// �f�o�C�X���X���b�v�`�F�[�����
	{
		UINT create_device_flags{ 0 };
#ifdef _DEBUG
		create_device_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		D3D_FEATURE_LEVEL feature_levels{ D3D_FEATURE_LEVEL_11_0 };

		DXGI_SWAP_CHAIN_DESC swap_chain_desc{};
		swap_chain_desc.BufferCount = 1;
		swap_chain_desc.BufferDesc.Width = SCREEN_WIDTH;
		swap_chain_desc.BufferDesc.Height = SCREEN_HEIGHT;
		swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swap_chain_desc.BufferDesc.RefreshRate.Numerator = 60;
		swap_chain_desc.BufferDesc.RefreshRate.Denominator = 1;
		swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swap_chain_desc.OutputWindow = hWnd;
		swap_chain_desc.SampleDesc.Count = 1;
		swap_chain_desc.SampleDesc.Quality = 0;
		swap_chain_desc.Windowed = !FULLSCREEN;
		/*hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, create_device_flags,
			&feature_levels, 1, D3D11_SDK_VERSION, &swap_chain_desc,
			swap_chain.GetAddressOf(), device.GetAddressOf(), NULL, immediate_context.GetAddressOf());

		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));*/





	}
	// �����_�[�^�[�Q�b�g�r���[�̐���
	{
		/*	Microsoft::WRL::ComPtr<ID3D11Texture2D> back_buffer{};
			hr = swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(back_buffer.GetAddressOf()));
			_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

			hr = device->CreateRenderTargetView(back_buffer.Get(), NULL, render_target_view.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));*/
	}
	// �f�v�X�X�e���V���r���[�̐���
	{
		Microsoft::WRL::ComPtr<ID3D11Texture2D> depth_stencil_buffer{};
		D3D11_TEXTURE2D_DESC texture2d_desc{};
		texture2d_desc.Width = SCREEN_WIDTH;
		texture2d_desc.Height = SCREEN_HEIGHT;
		texture2d_desc.MipLevels = 1;
		texture2d_desc.ArraySize = 1;
		texture2d_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		texture2d_desc.SampleDesc.Count = 1;
		texture2d_desc.SampleDesc.Quality = 0;
		texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
		texture2d_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		texture2d_desc.CPUAccessFlags = 0;
		texture2d_desc.MiscFlags = 0;
		hr = device->CreateTexture2D(&texture2d_desc, NULL, depth_stencil_buffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc{};
		depth_stencil_view_desc.Format = texture2d_desc.Format;
		depth_stencil_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depth_stencil_view_desc.Texture2D.MipSlice = 0;
		hr = device->CreateDepthStencilView(depth_stencil_buffer.Get(), &depth_stencil_view_desc, depth_stencil_view.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}
	// �T���v���X�e�[�g�̐���
	{
		D3D11_SAMPLER_DESC sampler_desc{};
		sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sampler_desc.MipLODBias = 0;
		sampler_desc.MaxAnisotropy = 16;
		sampler_desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		sampler_desc.BorderColor[0] = 0;
		sampler_desc.BorderColor[1] = 0;
		sampler_desc.BorderColor[2] = 0;
		sampler_desc.BorderColor[3] = 0;
		sampler_desc.MinLOD = 0;
		sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;
		hr = device->CreateSamplerState(&sampler_desc, sampler_state.GetAddressOf());
	}
	// �[�x�X�e���V���X�e�[�g�̐���
	{
		D3D11_DEPTH_STENCIL_DESC depth_stencil_desc{};
		depth_stencil_desc.DepthEnable = TRUE;
		depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		hr = device->CreateDepthStencilState(&depth_stencil_desc, depth_stencil_state.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}
	// �u�����h�X�e�[�g�̐���
	{
		// �A���t�@�u�����h
		D3D11_BLEND_DESC blend_desc{};
		blend_desc.AlphaToCoverageEnable = FALSE;
		blend_desc.IndependentBlendEnable = FALSE;
		blend_desc.RenderTarget[0].BlendEnable = TRUE;
		blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
		blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		hr = device->CreateBlendState(&blend_desc, blend_state.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}
	// ���X�^���C�U�[�X�e�[�g�̐���
	{
		D3D11_RASTERIZER_DESC rasterizer_desc{};
		rasterizer_desc.FillMode = D3D11_FILL_SOLID;
		rasterizer_desc.CullMode = D3D11_CULL_BACK;
		rasterizer_desc.FrontCounterClockwise = FALSE;
		rasterizer_desc.DepthBias = 0;
		rasterizer_desc.DepthBiasClamp = 0;
		rasterizer_desc.SlopeScaledDepthBias = 0;
		rasterizer_desc.DepthClipEnable = TRUE;
		rasterizer_desc.ScissorEnable = FALSE;
		rasterizer_desc.MultisampleEnable = FALSE;
		rasterizer_desc.AntialiasedLineEnable = FALSE;
		hr = device->CreateRasterizerState(&rasterizer_desc, rasterizer_state.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}
	// �萔�o�b�t�@�̐���
	{
		D3D11_BUFFER_DESC buffer_desc{};
		buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		buffer_desc.CPUAccessFlags = 0;
		buffer_desc.MiscFlags = 0;
		buffer_desc.StructureByteStride = 0;
		{
			buffer_desc.ByteWidth = sizeof(scene_constants);
			hr = device->CreateBuffer(&buffer_desc, nullptr, scene_constant_buffer.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		}

		{
			buffer_desc.ByteWidth = sizeof(light_constants);
			hr = device->CreateBuffer(&buffer_desc, nullptr, light_constant_buffer.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		}

		{
			buffer_desc.ByteWidth = sizeof(shadowmap_constants);
			hr = device->CreateBuffer(&buffer_desc, nullptr, shadowmap_constant_buffer.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		}

	}
	//�V�[���`��p�̃o�b�t�@����
	{
		Microsoft::WRL::ComPtr<ID3D11Texture2D>color_buffer{};
		D3D11_TEXTURE2D_DESC texture2d_desc{};
		texture2d_desc.Width = SCREEN_WIDTH;
		texture2d_desc.Height = SCREEN_HEIGHT;
		texture2d_desc.MipLevels = 1;
		texture2d_desc.ArraySize = 1;
		texture2d_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		texture2d_desc.SampleDesc.Count = 1;
		texture2d_desc.SampleDesc.Quality = 0;
		texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
		texture2d_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		texture2d_desc.CPUAccessFlags = 0;
		texture2d_desc.MiscFlags = 0;
		hr = device->CreateTexture2D(&texture2d_desc, NULL, color_buffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		//�����_�[�^�[�Q�b�g�r���[����
		hr = device->CreateRenderTargetView(color_buffer.Get(), NULL, scene_render_target_view.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		//�V�F�[�_�[���\�[�X�r���[����
		hr = device->CreateShaderResourceView(color_buffer.Get(), NULL, scene_shader_resource_view.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}
	//���C�g���猩���V�[���̐[�x�`��p�̃o�b�t�@����
	{
		Microsoft::WRL::ComPtr<ID3D11Texture2D> depth_buffer{};
		D3D11_TEXTURE2D_DESC texture2d_desc{};
		texture2d_desc.Width = SCREEN_WIDTH;
		texture2d_desc.Height = SCREEN_HEIGHT;
		texture2d_desc.MipLevels = 1;
		texture2d_desc.ArraySize = 1;
		texture2d_desc.Format = DXGI_FORMAT_R32_TYPELESS;
		texture2d_desc.SampleDesc.Count = 1;
		texture2d_desc.SampleDesc.Quality = 0;
		texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
		texture2d_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		texture2d_desc.CPUAccessFlags = 0;
		texture2d_desc.MiscFlags = 0;
		hr = device->CreateTexture2D(&texture2d_desc, NULL, depth_buffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		//�@�[�x�X�e���V���r���[
		D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc{};
		depth_stencil_view_desc.Format = DXGI_FORMAT_D32_FLOAT;
		depth_stencil_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depth_stencil_view_desc.Texture2D.MipSlice = 0;
		hr = device->CreateDepthStencilView(depth_buffer.Get(), &depth_stencil_view_desc,
			shadowmap_depth_stencil_view.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		//�V�F�[�_�[���\�[�X�r���[����
		D3D11_SHADER_RESOURCE_VIEW_DESC shader_resource_view_desc{};
		shader_resource_view_desc.Format = DXGI_FORMAT_R32_FLOAT;
		shader_resource_view_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		shader_resource_view_desc.Texture2D.MostDetailedMip = 0;
		shader_resource_view_desc.Texture2D.MipLevels = 1;
		hr = device->CreateShaderResourceView(depth_buffer.Get(), &shader_resource_view_desc,
			shadowmap_shader_resource_view.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		//�T���v���[�X�e�[�g�̐���
		{
			D3D11_SAMPLER_DESC sampler_desc{};
			sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
			sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
			sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
			sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
			sampler_desc.MipLODBias = 0;
			sampler_desc.MaxAnisotropy = 16;
			sampler_desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
			sampler_desc.BorderColor[0] = FLT_MAX;
			sampler_desc.BorderColor[1] = FLT_MAX;
			sampler_desc.BorderColor[2] = FLT_MAX;
			sampler_desc.BorderColor[3] = FLT_MAX;
			sampler_desc.MinLOD = 0;
			sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;
			hr = device->CreateSamplerState(&sampler_desc, shadowmap_sampler_state.GetAddressOf());

		}
	}


	// �`��I�u�W�F�N�g�̓ǂݍ���
	{
		//dummy_static_mesh = std::make_unique<static_mesh>(device.Get(), L".\\resources\\ball\\ball.obj", true);
		/*dummy_static_meshes.push_back(std::make_unique<static_mesh>(Graphics::Instance().GetDevice(), L".\\Data\\Model\\Cube\\ball.obj",
			true));
		dummy_static_meshes.push_back(std::make_unique<static_mesh>(Graphics::Instance().GetDevice(), L".\\Data\\Model\\Cube\\plane.obj",
			true));*/

		scaling.x = 0.01f;
		scaling.y = 0.01f;
		scaling.z = 0.01f;
		//dummy_sprite = std::make_unique<sprite>(device.Get(), L".\\resources\\chip_win.png");
		//dummy_sprite = std::make_unique<Sprite>(Graphics::Instance().GetDevice(), scene_shader_resource_view);



		//�T���v���X�e�[�g����
		D3D11_SAMPLER_DESC sampler_desc{};
		sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		sampler_desc.MaxAnisotropy = 16;
		sampler_desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		sampler_desc.MinLOD = 0;
		sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;

	}
	// �V�F�[�_�[�̓ǂݍ���
	{
		// static_mesh�p�f�t�H���g�`��V�F�[�_�[
		{
			D3D11_INPUT_ELEMENT_DESC input_element_desc[]

			{
				{ "POSITION",     0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "NORMAL",       0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TANGENT",      0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD",     0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "COLOR",        0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "BONE_WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "BONE_INDICES", 0, DXGI_FORMAT_R32G32B32A32_UINT,  0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			};



			//���C�e�B���O
			create_vs_from_cso(Graphics::Instance().GetDevice(),
				"Data/Shader/LambertVS.cso",
				mesh_vertex_shader.GetAddressOf(),
				mesh_input_layout.GetAddressOf(),
				input_element_desc,
				ARRAYSIZE(input_element_desc));
			create_ps_from_cso(Graphics::Instance().GetDevice(),
				"Data/Shader/phong_shader_ps.cso",
				mesh_pixel_shader.GetAddressOf());


			//		////�V���h�E�}�b�v�����p�V�F�[�_�[
			//create_vs_from_cso(device.Get(), "shadowmap_caster_vs.cso",
			//	shadowmap_caster_vertex_shader.GetAddressOf(),
			//	shadowmap_caster_input_layout.GetAddressOf(),
			//	input_element_desc, ARRAYSIZE(input_element_desc));


		}
		// sprite�p�f�t�H���g�`��V�F�[�_�[
		{
			D3D11_INPUT_ELEMENT_DESC input_element_desc[]
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			};
			create_vs_from_cso(Graphics::Instance().GetDevice(),
				"Data/Shader/SpriteVS.cso",
				sprite_vertex_shader.GetAddressOf(),
				sprite_input_layout.GetAddressOf(),
				input_element_desc,
				_countof(input_element_desc));
			create_ps_from_cso(Graphics::Instance().GetDevice(),
				"Data/Shader/SpritePS.cso",
				sprite_pixel_shader.GetAddressOf());

			//create_vs_from_cso(device.Get(),
			//	"UVScroll_vs.cso",
			//	sprite_vertex_shader.GetAddressOf(),
			//	sprite_input_layout.GetAddressOf(),
			//	input_element_desc,
			//	_countof(input_element_desc));
			//create_ps_from_cso(device.Get(),
			//	"UVScroll_ps.cso",
			//	sprite_pixel_shader.GetAddressOf());

			//create_vs_from_cso(device.Get(),
			//	"sprite_dissolve_vs.cso",
			//	sprite_vertex_shader.GetAddressOf(),
			//	sprite_input_layout.GetAddressOf(),
			//	input_element_desc,
			//	ARRAYSIZE(input_element_desc));
			//create_ps_from_cso(device.Get(),
			//	"sprite_dissolve_ps.cso",
			//	sprite_pixel_shader.GetAddressOf());


			////�J���[�t�B���^�[
			//create_vs_from_cso(device.Get(),
			//	"color_filter_vs.cso",
			//	sprite_vertex_shader.GetAddressOf(),
			//	sprite_input_layout.GetAddressOf(),
			//	input_element_desc,
			//	_countof(input_element_desc));
			//create_ps_from_cso(device.Get(),
			//	"color_filter_ps.cso",
			//	sprite_pixel_shader.GetAddressOf());
		}
	}
	EffectManager::Instance().Initialize();
	return true;
}
//�V���h�E�}�b�v����
void Framework::RenderShadowMap()
{
}

// �X�V����
void Framework::Update(float elapsedTime)
{
	// IMGUI�t���[���J�n����	
	ImGuiRenderer::NewFrame();

	// �V�[���X�V����
	SceneManager::Instance().Update(elapsedTime);

	EffectManager::Instance().Update(elapsedTime);
}

// �`�揈��
void Framework::Render(float elapsedTime)
{
	/*assert(
		Graphics::Instance().GetDevice()
		== device.Get());*/






	ID3D11DeviceContext* dc = Graphics::Instance().GetDeviceContext();

	//ID3D11DeviceContext* dc = Graphics::Instance().GetDeviceContext();


	auto immediate_context =
		Graphics::Instance().GetDeviceContext();



	// ��ʃN���A
		// ��ʃN���A
	Graphics::Instance().Clear(0.1f, 0.1f, 0.1f, 1);

	// �����_�[�^�[�Q�b�g�ݒ�
	Graphics::Instance().SetRenderTargets();

	// �V�[���`�揈��
	SceneManager::Instance().Render(elapsedTime);

	// �V�[��GUI�`�揈��
#ifndef DEBUG

	// �V�[��GUI�`�揈��
	SceneManager::Instance().DrawGUI();

	// �V�[���؂�ւ�GUI
	SceneSelectGUI();
#endif


	//�^�[�Q�b�g�ݒ�
	FLOAT color[]{ 0.2f, 0.2f, 0.2f, 1.0f };

	//immediate_context->ClearRenderTargetView(
	//	render_target_view.Get(),
	//	color);


	immediate_context->ClearDepthStencilView(
		depth_stencil_view.Get(),
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0);

	/*immediate_context->OMSetRenderTargets(
		1,
		render_target_view.GetAddressOf(),
		depth_stencil_view.Get());*/


		//D3D11_VIEWPORT viewport{};

		//viewport.TopLeftX = 0;
		//viewport.TopLeftY = 0;
		//viewport.Width = static_cast<float>(SCREEN_WIDTH);
		//viewport.Height = static_cast<float>(SCREEN_HEIGHT);
		//viewport.MinDepth = 0.0f;
		//viewport.MaxDepth = 1.0f;

		//immediate_context->RSSetViewports(1, &viewport);

		//�r���[�s��
	DirectX::XMMATRIX V;
	{
		DirectX::XMVECTOR up =
			DirectX::XMVectorSet(0, 1, 0, 0);

		float sx = sinf(rotateX);
		float cx = cosf(rotateX);
		float sy = sinf(rotateY);
		float cy = cosf(rotateY);

		DirectX::XMVECTOR Focus =
			DirectX::XMLoadFloat3(&camera_focus);

		DirectX::XMVECTOR Front =
			DirectX::XMVectorSet(-cx * sy, -sx, -cx * cy, 0);

		DirectX::XMVECTOR Distance =
			DirectX::XMVectorSet(distance, distance, distance,
				0);

		Front = DirectX::XMVectorMultiply(
			Front,
			Distance);

		DirectX::XMVECTOR Eye =
			DirectX::XMVectorSubtract(
				Focus,
				Front);

		DirectX::XMStoreFloat3(
			&camera_position,
			Eye);

		V = DirectX::XMMatrixLookAtLH(
			DirectX::XMLoadFloat3(&camera_position),
			DirectX::XMLoadFloat3(&camera_focus),
			up);
	}


	float aspect = Graphics::Instance().GetScreenWidth() / Graphics::Instance().GetScreenHeight();

	DirectX::XMMATRIX P =
		DirectX::XMMatrixPerspectiveFovLH(
			DirectX::XMConvertToRadians(30),
			aspect,
			0.1f,
			100.0f);

	//scene_constants�X�V
	scene_constants scene_data{};

	scene_data.camera_position.x = camera_position.x;
	scene_data.camera_position.y = camera_position.y;
	scene_data.camera_position.z = camera_position.z;

	DirectX::XMStoreFloat4x4(&scene_data.view_projection, V * P);

	immediate_context->UpdateSubresource(scene_constant_buffer.Get(), 0, 0, &scene_data, 0, 0);

	immediate_context->VSSetConstantBuffers(1, 1, scene_constant_buffer.GetAddressOf());

	immediate_context->PSSetConstantBuffers(1, 1, scene_constant_buffer.GetAddressOf());


	//���C�g
	light_constants lights{};

	lights.ambient_color =
		ambient_color;

	lights.directional_light_direction =
		directional_light_direction;

	lights.directional_light_color =
		directional_light_color;


	memcpy_s(
		lights.point_light,
		sizeof(lights.point_light),
		point_light,
		sizeof(point_light));


	


	immediate_context->UpdateSubresource(
		light_constant_buffer.Get(), 0, 0,
		&lights, 0, 0);

	immediate_context->VSSetConstantBuffers(2, 1,
		light_constant_buffer.GetAddressOf());

	immediate_context->PSSetConstantBuffers(2, 1,
		light_constant_buffer.GetAddressOf());

	//�V�F�[�_�[�ݒ�

	immediate_context->IASetInputLayout(
		mesh_input_layout.Get());

	immediate_context->VSSetShader(mesh_vertex_shader.Get(),
		nullptr, 0);

	immediate_context->PSSetShader(
		mesh_pixel_shader.Get(), nullptr, 0);

	immediate_context->PSSetSamplers(
		0, 1, sampler_state.GetAddressOf());

	DirectX::XMMATRIX S{ DirectX::XMMatrixScaling(scaling.x, scaling.y, scaling.z) };
	DirectX::XMMATRIX R{ DirectX::XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z) };
	DirectX::XMMATRIX T{ DirectX::XMMatrixTranslation(translation.x, translation.y, translation.z) };
	DirectX::XMFLOAT4X4 world;
	DirectX::XMStoreFloat4x4(&world, S * R * T);



	//dummy_static_meshes[0]->render(
	//	immediate_context,
	//	world,
	//	material_color);




#if 0
	// IMGUI�f���E�C���h�E�`��iIMGUI�@�\�e�X�g�p�j
	ImGui::ShowDemoWindow();
#endif



	immediate_context->IASetInputLayout(
		mesh_input_layout.Get());

	immediate_context->VSSetShader(
		mesh_vertex_shader.Get(),
		nullptr,
		0);

	immediate_context->PSSetShader(
		mesh_pixel_shader.Get(),
		nullptr,
		0);



	// �V�[���`�揈��
	//scene->Render(elapsedTime);

	// �V�[��GUI�`�揈��
	//scene->DrawGUI();

	// IMGUI�`��
	ImGuiRenderer::Render(immediate_context);

	// ��ʕ\��
	Graphics::Instance().Present(syncInterval);

}

template<class T>
void Framework::ChangeSceneButtonGUI(const char* name)
{
	if (ImGui::Button(name))
	{
		SceneManager::Instance().ChangeScene(new SceneLoading(new T));
	}
}

// �V�[���؂�ւ�GUI
void Framework::SceneSelectGUI()
{
	ImVec2 displaySize = ImGui::GetIO().DisplaySize;
	ImVec2 pos = ImGui::GetMainViewport()->GetWorkPos();
	float width = 210;
	float height = 460;
#ifndef DEBUG
	ImGui::SetNextWindowPos(ImVec2(pos.x + displaySize.x - width - 10, pos.y + 10), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(width, height), ImGuiCond_Once);

	if (ImGui::Begin("Scene"))
	{
		ChangeSceneButtonGUI<SceneTitle>("タイトルへ");
		ChangeSceneButtonGUI<SceneGame>("ゲームへ");
		ChangeSceneButtonGUI<SceneResult>("リザルトへ");

		//ChangeSceneButtonGUI<SceneAudioWASD>("音楽");
		if (ImGui::Button("フルスクリーン"))
		{
			ToggleFullscreen();
		}
		if (ImGui::Button("END"))
		{
			PostMessage(hWnd, WM_CLOSE, 0, 0);
		}


		/*ChangeSceneButtonGUI<RayCastScene>(u8"01.���C�L���X�g");
		ChangeSceneButtonGUI<LandWalkScene>(u8"02.�n����s");
		ChangeSceneButtonGUI<SlideMoveScene>(u8"03.�ǂ���ړ�");
		ChangeSceneButtonGUI<AnimationScene>(u8"04.�A�j���[�V����");
		ChangeSceneButtonGUI<ProjectScreenScene>(u8"05.�X�N���[�����W�ϊ�");
		ChangeSceneButtonGUI<AttachWeaponScene>(u8"06.�A�^�b�`�����g");
		ChangeSceneButtonGUI<HitStopScene>(u8"07.�q�b�g�X�g�b�v");
		ChangeSceneButtonGUI<UIAnimScene>(u8"08.UI���o");
		ChangeSceneButtonGUI<MoveFloorScene>(u8"09.�ړ���");
		ChangeSceneButtonGUI<TerrainAlignScene>(u8"10.�n�`�ɉ����p������");
		ChangeSceneButtonGUI<ResourceManagementScene>(u8"11.���\�[�X�Ǘ�");*/

	}
	ImGui::End();


#endif
}

// �t���[�����[�g�v�Z
void Framework::CalculateFrameStats()
{
	// Code computes the average frames per second, and also the 
	// average time it takes to render one frame.  These stats 
	// are appended to the window caption bar.
	static int frames = 0;
	static float time_tlapsed = 0.0f;

	frames++;

	// Compute averages over one second period.
	if ((timer.TimeStamp() - time_tlapsed) >= 1.0f)
	{
		float fps = static_cast<float>(frames); // fps = frameCnt / 1
		float mspf = 1000.0f / fps;
		std::ostringstream outs;
		outs.precision(6);
		outs << "FPS : " << fps << " / " << "Frame Time : " << mspf << " (ms)";
		SetWindowTextA(hWnd, outs.str().c_str());

		// Reset for next average.
		frames = 0;
		time_tlapsed += 1.0f;
	}
}

// �A�v���P�[�V�������[�v
int Framework::Run()
{
	if (!initialize())
	{
		return -1; // ���������s
	}
	MSG msg = {};
#if DEBUG
	ToggleFullscreen();
#endif

	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			timer.Tick();
#ifndef DEBUG
			CalculateFrameStats();

#endif
			float elapsedTime = timer.TimeInterval();
			Update(elapsedTime);
			Render(elapsedTime);
		}
	}

	return static_cast<int>(msg.wParam);
}

// ���b�Z�[�W�n���h��
LRESULT CALLBACK Framework::HandleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGuiRenderer::HandleMessage(hWnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc;
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_CREATE:
		break;
	//case WM_KEYDOWN:
	//	if (wParam == VK_ESCAPE) PostMessage(hWnd, WM_CLOSE, 0, 0);
	//	break;
	case WM_ENTERSIZEMOVE:
		// WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
		timer.Stop();
		break;
	case WM_EXITSIZEMOVE:
		// WM_EXITSIZEMOVE is sent when the user releases the resize bars.
		// Here we reset everything based on the new window dimensions.
		timer.Start();
		break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

void Framework::ToggleFullscreen()
{
	DWORD style = GetWindowLong(hWnd, GWL_STYLE);

	if (!isFullscreen)
	{
		GetWindowRect(hWnd, &windowRect);

		SetWindowLong(hWnd, GWL_STYLE, style & ~WS_OVERLAPPEDWINDOW);

		HMONITOR hMonitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
		MONITORINFO mi = { sizeof(mi) };
		GetMonitorInfo(hMonitor, &mi);

		SetWindowPos(
			hWnd, HWND_TOP,
			mi.rcMonitor.left, mi.rcMonitor.top,
			mi.rcMonitor.right - mi.rcMonitor.left,
			mi.rcMonitor.bottom - mi.rcMonitor.top,
			SWP_NOOWNERZORDER | SWP_FRAMECHANGED
		);

		isFullscreen = true;
	}
	else
	{
		SetWindowLong(hWnd, GWL_STYLE, style | WS_OVERLAPPEDWINDOW);

		SetWindowPos(
			hWnd, NULL,
			windowRect.left, windowRect.top,
			windowRect.right - windowRect.left,
			windowRect.bottom - windowRect.top,
			SWP_NOOWNERZORDER | SWP_FRAMECHANGED
		);

		isFullscreen = false;
	}

	// �� �t���X�N���[����Ƀo�b�N�o�b�t�@�����T�C�Y ��
	RECT rc;
	GetClientRect(hWnd, &rc);
	UINT w = rc.right - rc.left;
	UINT h = rc.bottom - rc.top;

	Graphics::Instance().Resize(w, h);
}

