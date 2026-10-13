#pragma once

#include <windows.h>
#include "HighResolutionTimer.h"
#include "Scene.h"
#include <static_mesh.h>
#include <Sprite.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include<sstream>




#define LIGHT_MAX 20

const LONG SCREEN_WIDTH = 1280;
const LONG SCREEN_HEIGHT = 720;

CONST BOOL FULLSCREEN{ FALSE };
CONST LPWSTR APPLICATION_NAME{ L"X3DGP" };

class Framework
{
public:
	Framework(HWND hWnd);
	~Framework();

private:
	void Update(float elapsedTime);
	void Render(float elapsedTime);

	template<class T>
	void ChangeSceneButtonGUI(const char* name);

	void SceneSelectGUI();

	void CalculateFrameStats();

	void ToggleFullscreen();

	void RenderShadowMap();

public:
	int Run();
	LRESULT CALLBACK HandleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:
	//const HWND				hWnd;
	//HighResolutionTimer		timer;
	//std::unique_ptr<Scene>	scene;
	bool isFullscreen = false;
	RECT windowRect = {};

	DirectX::XMFLOAT3 camera_position{ 0.0f, 0.0f, -10.0f };
	DirectX::XMFLOAT3 camera_focus{ 0.0f, 0.0f, 0.0f };
	float rotateX{ 0.0f };
	float rotateY{ DirectX::XMConvertToRadians(180) };
	POINT cursor_position;
	float wheel{ 0 };
	float distance{ 10.0f };

public:

	//Microsoft::WRL::ComPtr<ID3D11Device> device;
	//Microsoft::WRL::ComPtr<ID3D11DeviceContext> immediate_context;
	Microsoft::WRL::ComPtr<IDXGISwapChain> swap_chain;
	//Microsoft::WRL::ComPtr<ID3D11RenderTargetView> render_target_view;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depth_stencil_view;

	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler_state;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depth_stencil_state;
	Microsoft::WRL::ComPtr<ID3D11BlendState> blend_state;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizer_state;

	struct scene_constants
	{
		DirectX::XMFLOAT4X4 view_projection;
		DirectX::XMFLOAT4 options;	//	xy : マウスの座標値, z : タイマー, w : フラグ
		DirectX::XMFLOAT4 camera_position;
	};

	Microsoft::WRL::ComPtr<ID3D11Buffer> scene_constant_buffer;

	//点光源
	struct point_lights
	{
		DirectX::XMFLOAT4 position{ 0,0,0,0 };
		DirectX::XMFLOAT4 color{ 1,1,1,1 };
		float range{ 0 };
		DirectX::XMFLOAT3 dummy;
	};



	// 環境マップ
		struct  environment_constants
	{
		float environment_value;
		DirectX::XMFLOAT3 dummy;
	};
	Microsoft::WRL::ComPtr<ID3D11Buffer>environment_constant_buffer;
	D3D11_TEXTURE2D_DESC environment_texture2dDesc;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>environment_texture;
	float environment_value{ 0.0f };


	//ライティング
	struct light_constants
	{
		DirectX::XMFLOAT4 ambient_color;
		DirectX::XMFLOAT4 directional_light_direction;
		DirectX::XMFLOAT4 directional_light_color;
		point_lights point_light[LIGHT_MAX];
	};
	Microsoft::WRL::ComPtr<ID3D11Buffer>light_constant_buffer;
	DirectX::XMFLOAT4 ambient_color{ 0.2f,0.2f,0.2f,0.2f };
	DirectX::XMFLOAT4 directional_light_direction{ 0.0f,-1.0f,1.0f,1.0f };
	DirectX::XMFLOAT4 directional_light_color{ 1.0f,1.0f,1.0f,1.0f };
	point_lights point_light[LIGHT_MAX];
	//spot_lights spot_light[8];



	//シャドウマップ
	Microsoft::WRL::ComPtr<ID3D11Buffer> color_filter_constant_buffer;
	DirectX::XMFLOAT4 color_filter_parameter{ 0.0f,1.0f,1.0f,0.0f };

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> scene_render_target_view;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> scene_shader_resource_view;

	struct shadowmap_constants
	{
		DirectX::XMFLOAT4X4 light_view_projection; //ライトの位置から見た射影行列
		DirectX::XMFLOAT3   shadow_color;  //影色
		float               shadow_bias;    //深度バイアス
	};
	Microsoft::WRL::ComPtr<ID3D11Buffer> shadowmap_constant_buffer;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> shadowmap_depth_stencil_view;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shadowmap_shader_resource_view;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> shadowmap_sampler_state;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> shadowmap_caster_vertex_shader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> shadowmap_caster_input_layout;
	DirectX::XMFLOAT4X4 light_view_projection;
	float               shadow_bias{ 0.008f };
	DirectX::XMFLOAT3   shadow_color{ 0.3f,0.3f,0.3f };



	//スカイマッピング
	struct skymap_constants
	{
		DirectX::XMFLOAT4X4 inverse_view_ptojection;
	};
	Microsoft::WRL::ComPtr<ID3D11Buffer> skymap_constant_buffer;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> skymap_vertex_shader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> skymap_input_layout;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> skymap_pixel_shader;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> skymap_depth_stencil_state;
	D3D11_TEXTURE2D_DESC skymap_texture2d_desc;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> skymap_shader_resource_view;
	std::unique_ptr<Sprite> skymap_sprite;




	std::vector<std::unique_ptr<static_mesh>>dummy_static_meshes;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> mesh_vertex_shader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> mesh_input_layout;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> mesh_pixel_shader;

	std::unique_ptr<Sprite> dummy_sprite;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> sprite_vertex_shader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> sprite_input_layout;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> sprite_pixel_shader;

	Framework(const Framework&) = delete;
	Framework& operator=(const Framework&) = delete;
	Framework(Framework&&) noexcept = delete;
	Framework& operator=(Framework&&) noexcept = delete;

	DirectX::XMFLOAT3 translation{ 0, 0, 0 };
	DirectX::XMFLOAT3 scaling{ 1, 1, 1 };
	DirectX::XMFLOAT3 rotation{ 0, 0, 0 };
	DirectX::XMFLOAT4 material_color{ 1 ,1, 1, 1 };


	int run()
	{
		MSG msg{};

		if (!initialize())
		{
			return 0;
		}


		while (WM_QUIT != msg.message)
		{
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				tictoc.Tick();
				calculate_frame_stats();
				Update(tictoc.TimeInterval());
				Render(tictoc.TimeInterval());
			}
		}

#ifdef USE_IMGUI
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
#endif

		BOOL fullscreen{};
		swap_chain->GetFullscreenState(&fullscreen, 0);
		if (fullscreen)
		{
			swap_chain->SetFullscreenState(FALSE, 0);
		}

		return Framework::uninitialize() ? static_cast<int>(msg.wParam) : 0;
	}

	LRESULT CALLBACK handle_message(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
//#ifdef USE_IMGUI
//		if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) { return true; }
//#endif
		switch (msg)
		{
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			BeginPaint(hwnd, &ps);

			EndPaint(hwnd, &ps);
			break;
		}
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_CREATE:
			break;
		case WM_KEYDOWN:
			if (wparam == VK_ESCAPE)
			{
				PostMessage(hwnd, WM_CLOSE, 0, 0);
			}
			break;
		case WM_ENTERSIZEMOVE:
			tictoc.Stop();
			break;
		case WM_EXITSIZEMOVE:
			tictoc.Start();
			break;
		case WM_MOUSEWHEEL:
			wheel = GET_WHEEL_DELTA_WPARAM(wparam);
			break;
		default:
			return DefWindowProc(hwnd, msg, wparam, lparam);
		}
		return 0;
	}

private:
	bool initialize();
	//void update(float elapsed_time/*Elapsed seconds from last frame*/);
	//void render(float elapsed_time/*Elapsed seconds from last frame*/);
	bool uninitialize();

private:
	HighResolutionTimer tictoc;
	uint32_t frames{ 0 };
	float elapsed_time{ 0.0f };
	void calculate_frame_stats()
	{
		const float current = tictoc.TimeStamp();
		if (++frames, current >=elapsed_time+ 1.0f)
		{
			float fps = static_cast<float>(frames);
			std::wostringstream outs;
			outs.precision(6);
			outs << APPLICATION_NAME << L" : FPS : " << fps << L" / " << L"Frame Time : " << 1000.0f / fps << L" (ms)";
			SetWindowTextW(hWnd, outs.str().c_str());

			frames = 0;
			elapsed_time += 1.0f;
		}
	}


	const HWND				hWnd;
	HighResolutionTimer		timer;
	std::unique_ptr<Scene>	scene;
};