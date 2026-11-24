#include "MotionBlur.h"
#include "GpuResourceUtils.h"
#include "Graphics.h"

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



    hr = GpuResourceUtils::LoadPixelShader(
        device,
        "Data/Shader/MotionBlurHistory_ps.cso",
        historyPS.GetAddressOf());

    if (FAILED(hr))
        return hr;

    D3D11_BUFFER_DESC historyBufferDesc{};
    historyBufferDesc.ByteWidth = sizeof(CbHistory);
    historyBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    historyBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

    hr = device->CreateBuffer(
        &historyBufferDesc,
        nullptr,
        historyConstantBuffer.GetAddressOf());

    if (FAILED(hr))
        return hr;

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

    for (int i = 0; i < 2; ++i)
    {
        historyRTV[i].Reset();
        historySRV[i].Reset();
        historyTexture[i].Reset();
    }

    historyReadIndex = 0;
    historyValid = false;


    screenWidth = width;
    screenHeight = height;

    return CreateRenderTargets(device,width,height);
}

void MotionBlur::BeginPlayerColorPass(ID3D11DeviceContext* dc)
{
    if (dc == nullptr) return;

    ID3D11RenderTargetView* rtv = playerColorRTV.Get();

    float clearColor[4] = {};

    

    dc->OMSetRenderTargets(1, &rtv, Graphics::Instance().GetDepthStencilView());

    dc->ClearRenderTargetView(rtv, clearColor);


    // 深度テストあり・書き込みなし
    dc->OMSetDepthStencilState(
        Graphics::Instance().GetRenderState()
        ->GetDepthStencilState(DepthState::TestOnly),0);

    dc->OMSetBlendState(
        Graphics::Instance().GetRenderState()
        ->GetBlendState(BlendState::Opaque),
        nullptr,0xFFFFFFFF);

}

void MotionBlur::BeginVelocityPass(ID3D11DeviceContext* dc)
{
    if (dc == nullptr) return;

    ID3D11RenderTargetView* rtv = velocityRTV.Get();

    dc->OMSetRenderTargets(
        1, &rtv,Graphics::Instance().GetDepthStencilView());

    const float clearColor[4] = {};

    dc->ClearRenderTargetView(
        rtv,
        clearColor);

    dc->OMSetDepthStencilState(
        Graphics::Instance().GetRenderState()
        ->GetDepthStencilState(DepthState::TestOnly),
        0);

    dc->OMSetBlendState(
        Graphics::Instance().GetRenderState()
        ->GetBlendState(BlendState::Opaque),
        nullptr,
        0xFFFFFFFF);

}

void MotionBlur::Execute(ID3D11DeviceContext* dc, float elapsedTime)
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



    // 現在のブラー描画で使ったSRVを解除
    ID3D11ShaderResourceView* nullSrvs[2]{};
    dc->PSSetShaderResources(0, 2, nullSrvs);

    // 最初のフレームは、前履歴を透明にしてから読む
    if (!historyValid)
    {
        const float transparent[4]{};
        dc->ClearRenderTargetView(
            historyRTV[historyReadIndex].Get(),
            transparent);
    }

    const UINT writeIndex = 1 - historyReadIndex;

    // 読み込み中の履歴とは別のRTVに書く
    ID3D11RenderTargetView* historyTarget =
        historyRTV[writeIndex].Get();

    dc->OMSetRenderTargets(1, &historyTarget, nullptr);

    dc->OMSetBlendState(
        Graphics::Instance().GetRenderState()
        ->GetBlendState(BlendState::Opaque),
        nullptr,
        0xFFFFFFFF);

    dc->VSSetShader(fullscreenVS.Get(), nullptr, 0);
    dc->PSSetShader(historyPS.Get(), nullptr, 0);
    dc->IASetInputLayout(nullptr);
    dc->IASetPrimitiveTopology(
        D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    CbHistory cbHistory{};
    cbHistory.decay = expf(-elapsedTime / 0.18f);

    dc->UpdateSubresource(
        historyConstantBuffer.Get(),
        0,
        nullptr,
        &cbHistory,
        0,
        0);

    ID3D11Buffer* historyCB =
        historyConstantBuffer.Get();

    dc->PSSetConstantBuffers(0, 1, &historyCB);

    ID3D11ShaderResourceView* historyInputs[] =
    {
        blurSRV.Get(),
        historySRV[historyReadIndex].Get()
    };

    dc->PSSetShaderResources(0, 2, historyInputs);
    dc->PSSetSamplers(0, 1, &sampler);
    dc->Draw(3, 0);

    // 読み込み解除してから次フレームの読み取り先を切り替える
    dc->PSSetShaderResources(0, 2, nullSrvs);

    historyReadIndex = writeIndex;
    historyValid = true;
}



void MotionBlur::Composite(ID3D11DeviceContext* dc, ID3D11RenderTargetView* destinationRTV, const D3D11_VIEWPORT& viewport)
{
    if (dc == nullptr || destinationRTV == nullptr)
        return;

    // 出力先
    dc->OMSetRenderTargets(
        1,&destinationRTV,nullptr);

    dc->RSSetViewports(1, &viewport);


    // 深度テストを無効化
    dc->OMSetDepthStencilState(
        Graphics::Instance().GetRenderState()
        ->GetDepthStencilState(DepthState::NoTestNoWrite), 0);

    // アルファブレンド
    dc->OMSetBlendState(
        Graphics::Instance().GetRenderState()
        ->GetBlendState(BlendState::Transparency),
        nullptr, 0xFFFFFFFF);

    // SRV設定
    ID3D11ShaderResourceView* srv =
        historySRV[historyReadIndex].Get();

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


    if (FAILED(hr))
        return hr;

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

    //履歴用テクスチャ
    for (int i = 0; i < 2; ++i)
    {
        hr = device->CreateTexture2D(
            &desc, nullptr, historyTexture[i].GetAddressOf());
        if (FAILED(hr)) return hr;

        hr = device->CreateRenderTargetView(
            historyTexture[i].Get(), nullptr, historyRTV[i].GetAddressOf());
        if (FAILED(hr)) return hr;

        hr = device->CreateShaderResourceView(
            historyTexture[i].Get(), nullptr, historySRV[i].GetAddressOf());
        if (FAILED(hr)) return hr;
    }


    return S_OK;




}


