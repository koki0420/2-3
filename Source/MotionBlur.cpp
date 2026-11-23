#include "MotionBlur.h"
#include "GpuResourceUtils.h"

HRESULT MotionBlur::Initialize(ID3D11Device* device, UINT width, UINT height)
{
    if (device == nullptr)return E_INVALIDARG;

    screenWidth = width;
	screenHeight = height;

    HRESULT hr = CreateRenderTargets(device, width, height);

    if (FAILED(hr))return hr;

    //全画面描画 VS
    hr = GpuResourceUtils::LoadVertexShader(
        device, "Data/Shader/MotionBlur_vs.cso",
        nullptr, 0, nullptr ,fullscreenVS.GetAddressOf());

    if (FAILED(hr))return hr;

    //モーションブラーPS
    hr = GpuResourceUtils::LoadPixelShader(
        device, "Data/Shader/MotionBlur_ps.cso",
        motionBlurPS.GetAddressOf());

    if (FAILED(hr))return hr;


    hr = GpuResourceUtils::LoadPixelShader(
        device,
        "Data/Shader/MotionBlurComposite_ps.cso",
        compositePS.GetAddressOf());

    if (FAILED(hr))
        return hr;

    //定数バッファ
    D3D11_BUFFER_DESC bufferDesc{};
    bufferDesc.ByteWidth = sizeof(CbBlur);
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

    hr = device->CreateBuffer(&bufferDesc, nullptr, blurConstantBuffer.GetAddressOf());

    if (FAILED(hr)) return hr;

    //サンプラー
    D3D11_SAMPLER_DESC samplerDesc{};

    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    hr = device->CreateSamplerState(
        &samplerDesc, linearSampler.GetAddressOf());

    if (FAILED(hr))return hr;

    return S_OK;
}

HRESULT MotionBlur::Resize(ID3D11Device* device, UINT width, UINT height)
{
    if (device == nullptr || width == 0 || height == 0)
    {
        return E_INVALIDARG;
    }

  

    playerColorRTV.Reset();
    playerColorSRV.Reset();
    playerColorTexture.Reset();

    velocityRTV.Reset();
    velocitySRV.Reset();
    velocityTexture.Reset();

    depthDSV.Reset();
    depthTexture.Reset();

    blurRTV.Reset();
    blurSRV.Reset();
    blurTexture.Reset();


    screenWidth = width;
    screenHeight = height;

    return CreateRenderTargets(device,width,height);
}

void MotionBlur::BeginPlayerColorPass(ID3D11DeviceContext* dc)
{
    ID3D11RenderTargetView* rtv = playerColorRTV.Get();

    dc->OMSetRenderTargets(1, &rtv, depthDSV.Get());

    float clearColor[4] = {};

    dc->ClearRenderTargetView(rtv, clearColor);

    dc->ClearDepthStencilView(
        depthDSV.Get(),
        D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f,0);

}

void MotionBlur::BeginVelocityPass(ID3D11DeviceContext* dc)
{
    ID3D11RenderTargetView* rtv = velocityRTV.Get();

    dc->OMSetRenderTargets(1, &rtv, depthDSV.Get());

    const float clearColor[4] = {};

    dc->ClearRenderTargetView(rtv, clearColor);

}

void MotionBlur::Execute(ID3D11DeviceContext* dc)
{

    if (dc == nullptr)return;

    //ブラー結果RTを設定
    ID3D11RenderTargetView* rtv = blurRTV.Get();


    //RTVとして使用していたリソースを解除
    dc->OMSetRenderTargets(1, &rtv, nullptr);

    //クリア
    const float clearColor[4] = {};

    dc->ClearRenderTargetView(rtv, clearColor);

    // ビューポート
    D3D11_VIEWPORT viewport{};

    viewport.Width = static_cast<float>(screenWidth);
    viewport.Height = static_cast<float>(screenHeight);
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;

    dc->RSSetViewports(1, &viewport);

    //定数バッファ
    CbBlur cbBlur{};

    cbBlur.blurStrength = 1.0f;

    cbBlur.texelSize =
    {
        1.0f / screenWidth,
        1.0f / screenHeight
    };

    cbBlur.sampleCount = 8;

    dc->UpdateSubresource(
        blurConstantBuffer.Get(),
        0, nullptr, &cbBlur, 0, 0);

    //シェーダー設定
    dc->VSSetShader(fullscreenVS.Get(), nullptr, 0);

    dc->PSSetShader(motionBlurPS.Get(), nullptr, 0);

    //入力レイアウトなし
    dc->IASetInputLayout(nullptr);

    //全画面三角形
    dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    //テクスチャ設定
    ID3D11ShaderResourceView* srvs[] =
    {
        playerColorSRV.Get(),
        velocitySRV.Get()
    };

    dc->PSSetShaderResources(0, 2, srvs);

    //サンプラー
    ID3D11SamplerState* sampler = linearSampler.Get();

    dc->PSSetSamplers(0, 1, &sampler);

    //定数バッファ
    ID3D11Buffer* cb = blurConstantBuffer.Get();

    dc->PSSetConstantBuffers(0, 1, &cb);

    //ブラー実行
    dc->Draw(3, 0);

    //SRV解除
    ID3D11ShaderResourceView* nullSRVs[2]{};

    dc->PSSetShaderResources(0, 2, nullSRVs);
}



void MotionBlur::Composite(ID3D11DeviceContext* dc, ID3D11RenderTargetView* destinationRTV, const D3D11_VIEWPORT& viewport)
{
    if (dc == nullptr || destinationRTV == nullptr)
        return;

    // 出力先
    dc->OMSetRenderTargets(
        1,&destinationRTV,nullptr);

    dc->RSSetViewports(1, &viewport);

    // SRV設定
    ID3D11ShaderResourceView* srv =
        blurSRV.Get();

    dc->PSSetShaderResources(
        0,1,&srv);

    // シェーダー設定
    dc->VSSetShader(
        fullscreenVS.Get(),
        nullptr,0);

    dc->PSSetShader(
        compositePS.Get(),
        nullptr,0);

    dc->IASetInputLayout(nullptr);

    dc->IASetPrimitiveTopology(
        D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // 描画
    dc->Draw(3, 0);

    // SRV解除
    ID3D11ShaderResourceView* nullSRV = nullptr;

    dc->PSSetShaderResources(
        0,1, &nullSRV);

}

HRESULT MotionBlur::CreateRenderTargets(ID3D11Device* device, UINT width, UINT height)
{
    D3D11_TEXTURE2D_DESC desc{};

    //プレイヤー用

    desc.Width = width;
    desc.Height = height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;

    desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

    HRESULT hr = device->CreateTexture2D(&desc, nullptr, playerColorTexture.GetAddressOf());

    if (FAILED(hr))return hr;

    hr = device->CreateRenderTargetView(playerColorTexture.Get(), nullptr, playerColorRTV.GetAddressOf());

    if (FAILED(hr))return hr;

    hr = device->CreateShaderResourceView(playerColorTexture.Get(), nullptr, playerColorSRV.GetAddressOf());

    if (FAILED(hr))return hr;

    //速度用
    desc.Format = DXGI_FORMAT_R16G16_FLOAT;

    hr = device->CreateTexture2D(&desc, nullptr, velocityTexture.GetAddressOf());

    if (FAILED(hr))return hr;

    hr = device->CreateRenderTargetView(velocityTexture.Get(), nullptr, velocityRTV.GetAddressOf());

    if (FAILED(hr))return hr;

    hr = device->CreateShaderResourceView(velocityTexture.Get(), nullptr, velocitySRV.GetAddressOf());

    if (FAILED(hr)) return hr;


    //深度バッファ
    D3D11_TEXTURE2D_DESC depthDesc{};

    depthDesc.Width = width;
    depthDesc.Height = height;
    depthDesc.MipLevels = 1;
    depthDesc.ArraySize = 1;
    depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthDesc.SampleDesc.Count = 1;
    depthDesc.Usage = D3D11_USAGE_DEFAULT;
    depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

    hr = device->CreateTexture2D(
        &depthDesc,
        nullptr, depthTexture.GetAddressOf());

    if (FAILED(hr))
        return hr;

    hr = device->CreateDepthStencilView(
        depthTexture.Get(),
        nullptr,depthDSV.GetAddressOf());

    desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;

    hr = device->CreateTexture2D(
        &desc,nullptr,
        blurTexture.GetAddressOf());

    if (FAILED(hr))
        return hr;

    hr = device->CreateRenderTargetView(
        blurTexture.Get(), nullptr,
        blurRTV.GetAddressOf());

    if (FAILED(hr))
        return hr;

    hr = device->CreateShaderResourceView(
        blurTexture.Get(),nullptr,
        blurSRV.GetAddressOf());

    if (FAILED(hr))
        return hr;


    return S_OK;




}


