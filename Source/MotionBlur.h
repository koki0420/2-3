#pragma once
#include <DirectXMath.h>
#include <wrl.h>
#include <d3d11.h>

class MotionBlur
{

public:
	static MotionBlur& Instance()
	{
		static MotionBlur instance;
		return instance;
	}

	MotionBlur(const MotionBlur&) = delete;
	MotionBlur& operator=(const MotionBlur&) = delete;

public:
	MotionBlur() = default;
	~MotionBlur() = default;

	//初期化
	HRESULT Initialize(ID3D11Device* device, UINT width, UINT height);

	//解像度変更
	HRESULT Resize(ID3D11Device* device, UINT width, UINT height);

	//プレイヤーカラー描画開始
	void BeginPlayerColorPass(ID3D11DeviceContext* dc);

	//速度バッファ描画開始
	void BeginVelocityPass(ID3D11DeviceContext* dc);

	//モーションブラー実行
	void Execute(ID3D11DeviceContext* dc, float elapsedTime);

	//前フレーム情報更新
	void UdatePreviousFrame();

	// ブラー結果を画面に合成
	void Composite(ID3D11DeviceContext* dc,ID3D11RenderTargetView* destinationRTV,const D3D11_VIEWPORT& viewport);

private:

	//レンダーターゲット生成
	HRESULT CreateRenderTargets(ID3D11Device* device, UINT width, UINT height);

private:

	struct CbBlur
	{
		float blurStrength;
		DirectX::XMFLOAT2 texelSize;
		int sampleCount;
	};

	struct CbHistory
	{
		float decay;
		float padding[3];
	};


	//プレイヤーカラー
	Microsoft::WRL::ComPtr<ID3D11Texture2D> playerColorTexture;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> playerColorRTV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> playerColorSRV;

	//速度バッファ
	Microsoft::WRL::ComPtr<ID3D11Texture2D> velocityTexture;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> velocityRTV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> velocitySRV;

	//深度
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depthTexture;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthDSV;

	//画面サイズ
	UINT screenWidth = 0;
	UINT screenHeight = 0;

	// ブラー結果
	Microsoft::WRL::ComPtr<ID3D11Texture2D> blurTexture;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> blurRTV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> blurSRV;

	// 全画面描画用シェーダー
	Microsoft::WRL::ComPtr<ID3D11VertexShader> fullscreenVS;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> motionBlurPS;



	// サンプラー
	Microsoft::WRL::ComPtr<ID3D11SamplerState> linearSampler;

	// ブラー定数バッファ
	Microsoft::WRL::ComPtr<ID3D11Buffer> blurConstantBuffer;

	Microsoft::WRL::ComPtr<ID3D11BlendState> compositeBlendState;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> compositePS;

	//履歴テクスチャ
	Microsoft::WRL::ComPtr<ID3D11Texture2D> historyTexture[2];
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> historyRTV[2];
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> historySRV[2];

	Microsoft::WRL::ComPtr<ID3D11PixelShader> historyPS;
	Microsoft::WRL::ComPtr<ID3D11Buffer> historyConstantBuffer;

	UINT historyReadIndex = 0;
	bool historyValid = false;
};