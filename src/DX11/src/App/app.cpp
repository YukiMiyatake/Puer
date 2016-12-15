#include "../stdafx.h"
#include<atlbase.h>


#include "app.h"
#include "../DX/program.h"


App::App(HWND hwnd)	
	: hwnd_(hwnd)
{

	device_ = std::make_unique<Puer::DX11::Device>();
	device_->Create( hwnd, 0 );

}


const Puer::DX11::Device& App::getDevice() {
	return( *device_ );
}



void App::Init() {
	RECT rc;
	GetClientRect(hwnd_, &rc);
	UINT width = rc.right - rc.left;
	UINT height = rc.bottom - rc.top;
	HRESULT hr;

	// Define the input layout
	UINT numElements = Puer::Util::simpleInputLayoutCount();


	program_forward_ = device_->CreateProgram();
	program_forward_->CreateVertexShaderFromFile(L"src/shader/forward.fx", "VS", Puer::Util::simpleInputLayout, numElements, nullptr, nullptr);
	program_forward_->CreatePixelShaderFromFile(L"src/shader/forward.fx", "PS", nullptr, nullptr);

	hr = device_->CreateBuffer(Puer::Util::simpleBoxVertex, sizeof(Puer::Util::SimpleVertex) * 24, D3D11_BIND_VERTEX_BUFFER, vertexBuffer_);
	hr = device_->CreateBuffer(Puer::Util::simpleBoxIndex, sizeof(WORD) * 36, D3D11_BIND_INDEX_BUFFER, indexBuffer_);
	hr = device_->CreateBuffer( nullptr, sizeof(ConstantBuffer), D3D11_BIND_CONSTANT_BUFFER, constantBuffer_);


	// ---- DeferredShading
	// MRT RenderTargetView
	
	D3D11_TEXTURE2D_DESC tex_desc;
	tex_desc.Width = width;
	tex_desc.Height = height;
	tex_desc.MipLevels = 1;
	tex_desc.ArraySize = 1;
	tex_desc.SampleDesc.Count = 1;
	tex_desc.SampleDesc.Quality = 0;
	tex_desc.Usage = D3D11_USAGE_DEFAULT;
	tex_desc.CPUAccessFlags = 0;
	tex_desc.MiscFlags = 0;
	tex_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	tex_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	hr = device_->GetDevice()->CreateTexture2D(&tex_desc, nullptr, &gbuf_pos_);
	hr = device_->GetDevice()->CreateTexture2D(&tex_desc, nullptr, &gbuf_normaldepth_);
	hr = device_->GetDevice()->CreateTexture2D(&tex_desc, nullptr, &gbuf_diffuse_);

	hr = device_->GetDevice()->CreateRenderTargetView(&device_->GetBackBuffer(), NULL, &rtv_[0]);
	hr = device_->GetDevice()->CreateRenderTargetView(gbuf_pos_, NULL, &rtv_[1]);
	hr = device_->GetDevice()->CreateRenderTargetView(gbuf_normaldepth_, NULL, &rtv_[2]);
	hr = device_->GetDevice()->CreateRenderTargetView(gbuf_diffuse_, NULL, &rtv_[3]);

	// テクスチャ情報を取得する
	D3D11_TEXTURE2D_DESC texDesc;
	gbuf_pos_->GetDesc(&texDesc);

	// ShaderResourceViewの情報を作成する
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = texDesc.MipLevels;

	// ShaderResourceViewを作成する
	hr = device_->GetDevice()->CreateShaderResourceView(gbuf_pos_, &srvDesc, &srv_gbuf_pos_);
	hr = device_->GetDevice()->CreateShaderResourceView(gbuf_normaldepth_, &srvDesc, &srv_gbuf_normaldepth_);
	hr = device_->GetDevice()->CreateShaderResourceView(gbuf_diffuse_, &srvDesc, &srv_gbuf_diffuse_);


	program_deferred_ = device_->CreateProgram();
	program_deferred_->CreateVertexShaderFromFile(L"src/shader/deferred.fx", "VS", Puer::Util::simpleInputLayout, numElements, nullptr, nullptr);
	program_deferred_->CreatePixelShaderFromFile(L"src/shader/deferred.fx", "PS", nullptr, nullptr);
	


	{

		hr = device_->CreateBuffer(Puer::Util::itaVertex, sizeof(Puer::Util::ItaVertex) * 20, D3D11_BIND_VERTEX_BUFFER, vertexBuffer_Deferred_light_);


		program_deferred_light_ = device_->CreateProgram();
		program_deferred_light_->CreateVertexShaderFromFile(L"src/shader/deferred_light.fx", "VS", Puer::Util::ItaInputLayout, Puer::Util::itaInputLayoutCount(), nullptr, nullptr);
		program_deferred_light_->CreatePixelShaderFromFile(L"src/shader/deferred_light.fx", "PS", nullptr, nullptr);

		program_debug_ = device_->CreateProgram();
		program_debug_->CreateVertexShaderFromFile(L"src/DX/shader/ita.hlsl", "VS", Puer::Util::ItaTexInputLayout, Puer::Util::itaTexInputLayoutCount(), nullptr, nullptr);
		program_debug_->CreateGeometryShaderFromFile(L"src/DX/shader/ita.hlsl", "GS9", nullptr, nullptr);
		program_debug_->CreatePixelShaderFromFile(L"src/DX/shader/ita.hlsl", "PS", nullptr, nullptr);
//		program_debug_->CreateVertexShaderFromFile(L"src/shader/deferred_light.fx", "VS", Puer::Util::ItaTexInputLayout, Puer::Util::itaTexInputLayoutCount(), nullptr, nullptr);
//		program_debug_->CreatePixelShaderFromFile(L"src/shader/deferred_light.fx", "PSTex1", nullptr, nullptr);

	}

	// ----


	// Initialize the world matrices
	world_ = XMMatrixIdentity();

	// Initialize the view matrix
	XMVECTOR Eye = XMVectorSet(0.0f, 4.0f, -10.0f, 0.0f);
	XMVECTOR At = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	view_ = XMMatrixLookAtLH(Eye, At, Up);

	// Initialize the projection matrix
	projection_ = XMMatrixPerspectiveFovLH(XM_PIDIV4, width / (FLOAT)height, 0.01f, 100.0f);

}

void App::Update() {
	// Update our time
	static float t = 0.0f;
	{
		static DWORD dwTimeStart = 0;
		DWORD dwTimeCur = GetTickCount();
		if (dwTimeStart == 0)
			dwTimeStart = dwTimeCur;
		t = (dwTimeCur - dwTimeStart) / 1000.0f;
	}

	// Rotate cube around the origin
	world_ = XMMatrixRotationY(t);

	// Setup our lighting parameters
	XMFLOAT4 vLightDirs[2] =
	{
		XMFLOAT4(-0.577f, 0.577f, -0.577f, 1.0f),
		XMFLOAT4(0.0f, 0.0f, -1.0f, 1.0f),
	};
	XMFLOAT4 vLightColors[2] =
	{
		XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f),
		XMFLOAT4(0.5f, 0.0f, 0.0f, 1.0f)
	};

	// Rotate the second light around the origin
	XMMATRIX mRotate = XMMatrixRotationY(-2.0f * t);
	XMVECTOR vLightDir = XMLoadFloat4(&vLightDirs[1]);
	vLightDir = XMVector3Transform(vLightDir, mRotate);
	XMStoreFloat4(&vLightDirs[1], vLightDir);

	//
	// Update matrix variables and lighting variables
	//
	ConstantBuffer cb1;
	cb1.mWorld = XMMatrixTranspose(world_);
	cb1.mView = XMMatrixTranspose(view_);
	cb1.mProjection = XMMatrixTranspose(projection_);
	cb1.vLightDir[0] = vLightDirs[0];
	cb1.vLightDir[1] = vLightDirs[1];
	cb1.vLightColor[0] = vLightColors[0];
	cb1.vLightColor[1] = vLightColors[1];
	cb1.vOutputColor = XMFLOAT4(0, 0, 0, 0);
	device_->GetImmediateContext()->UpdateSubresource(constantBuffer_, 0, NULL, &cb1, 0, 0);
}


void App::Render(){

	bool deferred = true;
	if (deferred) {

		//*  Path 1 GBuffer
		// Clear the back buffer and GBuffer
		float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f }; // red, green, blue, alpha
//		ID3D11RenderTargetView *rtv[] = { rtv_[0].p, rtv_[1].p, rtv_[2].p, rtv_[3].p };

		device_->GetImmediateContext()->ClearRenderTargetView(rtv_[0], ClearColor);
		device_->GetImmediateContext()->ClearRenderTargetView(rtv_[1], ClearColor);
		device_->GetImmediateContext()->ClearRenderTargetView(rtv_[2], ClearColor);
		device_->GetImmediateContext()->ClearRenderTargetView(rtv_[3], ClearColor);

//		device_->ClearDepth(device_->GetImmediateContext());
		device_->GetImmediateContext()->ClearDepthStencilView(device_->m_DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);


		device_->GetImmediateContext()->OMSetRenderTargets(0, nullptr, nullptr);

//		device_->GetImmediateContext()->OMSetRenderTargets(3, &rtv[1], &device_->GetDepthStencilView());
		device_->GetImmediateContext()->OMSetRenderTargets(3, &rtv_[1].p, device_->m_DepthStencilView);

		program_deferred_->SetLayout();

		// Set vertex buffer
		UINT stride = sizeof(Puer::Util::SimpleVertex);
		UINT offset = 0;
		device_->GetImmediateContext()->IASetVertexBuffers(0, 1, &vertexBuffer_.p, &stride, &offset);

		// Set index buffer
		device_->GetImmediateContext()->IASetIndexBuffer(indexBuffer_, DXGI_FORMAT_R16_UINT, 0);
		// Set primitive topology
		device_->GetImmediateContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//
		// Render the cube
		//
		device_->GetImmediateContext()->VSSetShader(&program_deferred_->getVertexShader(), nullptr, 0);
		device_->GetImmediateContext()->VSSetConstantBuffers(0, 1, &constantBuffer_.p);
		device_->GetImmediateContext()->PSSetShader(&program_deferred_->getPixelShader(), nullptr, 0);
		device_->GetImmediateContext()->PSSetConstantBuffers(0, 1, &constantBuffer_.p);
		device_->GetImmediateContext()->DrawIndexed(36, 0, 0);


#if 1
		// ita
		// debug   Sampler、Layout、 RT、SRV、vertex、index等設定できるようにしよう・・
		ID3D11SamplerState *ss = &device_->GetLinerSampler();
		device_->GetImmediateContext()->PSSetSamplers(0, 1, &ss);
		device_->GetImmediateContext()->OMSetRenderTargets(1, &rtv_[0].p, nullptr);
		program_debug_->SetLayout();



		stride = sizeof(Puer::Util::ItaVertex);
		offset = 0;
		device_->GetImmediateContext()->IASetVertexBuffers(0, 1, &vertexBuffer_Deferred_light_.p, &stride, &offset);

		device_->GetImmediateContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

		device_->GetImmediateContext()->VSSetShader(&program_debug_->getVertexShader(), NULL, 0);
		device_->GetImmediateContext()->VSSetConstantBuffers(0, 1, &constantBuffer_.p);
		device_->GetImmediateContext()->GSSetShader(&program_debug_->getGeometryShader(), NULL, 0);
		device_->GetImmediateContext()->PSSetShader(&program_debug_->getPixelShader(), NULL, 0);
		device_->GetImmediateContext()->PSSetConstantBuffers(0, 1, &constantBuffer_.p);

		ID3D11ShaderResourceView* psrv[] = { srv_gbuf_pos_.p, srv_gbuf_normaldepth_.p, srv_gbuf_diffuse_.p, 0 };
		device_->GetImmediateContext()->PSSetShaderResources(0, 3, psrv);
		//	g_pImmediateContext->PSSetShaderResources(1, 1, &g_pSRV_GBuf_NormalDepth);
		//	g_pImmediateContext->PSSetShaderResources(2, 1, &g_pSRV_GBuf_Diffuse);
		device_->GetImmediateContext()->Draw(6, 0);

		device_->GetImmediateContext()->GSSetShader(nullptr, nullptr, 0);

#endif
	
#if 0
		// path 2  Light
		ID3D11SamplerState *ss = &device_->GetLinerSampler();
		device_->GetImmediateContext()->PSSetSamplers(0, 1, &ss );
		device_->GetImmediateContext()->OMSetRenderTargets(1, &rtv_[0].p, nullptr);
		//	g_pImmediateContext->OMSetRenderTargets(1, &g_pRenderTargetView[0], g_pDepthStencilView);
		program_deferred_light_->SetLayout();



		stride = sizeof(Puer::Util::ItaVertex);
		offset = 0;
		device_->GetImmediateContext()->IASetVertexBuffers(0, 1, &vertexBuffer_Deferred_light_.p, &stride, &offset);

		device_->GetImmediateContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

		device_->GetImmediateContext()->VSSetShader(&program_deferred_light_->getVertexShader(), NULL, 0);
		device_->GetImmediateContext()->VSSetConstantBuffers(0, 1, &constantBuffer_.p);
		device_->GetImmediateContext()->PSSetShader(&program_deferred_light_->getPixelShader(), NULL, 0);
		device_->GetImmediateContext()->PSSetConstantBuffers(0, 1, &constantBuffer_.p);

		ID3D11ShaderResourceView* psrv[] = { srv_gbuf_pos_.p, srv_gbuf_normaldepth_.p, srv_gbuf_diffuse_.p, 0 };
		device_->GetImmediateContext()->PSSetShaderResources(0, 3, psrv);
		//	g_pImmediateContext->PSSetShaderResources(1, 1, &g_pSRV_GBuf_NormalDepth);
		//	g_pImmediateContext->PSSetShaderResources(2, 1, &g_pSRV_GBuf_Diffuse);
		device_->GetImmediateContext()->Draw(6, 0);


		//*/


#endif

		ID3D11ShaderResourceView* hoge[3] = { NULL };

		device_->GetImmediateContext()->PSSetShader(NULL, NULL, 0);
		device_->GetImmediateContext()->PSSetShaderResources(0, 3, hoge);
//		device_->GetImmediateContext()->PSSetShaderResources(1, 1, hoge);
		//g_pImmediateContext->PSSetShaderResources(2, 1, hoge);






	}
	else {
		//
		// Clear the back buffer
		//
		float ClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f }; // red, green, blue, alpha

		device_->Clear(device_->GetImmediateContext(), ClearColor);
		device_->ClearDepth(device_->GetImmediateContext());




		// Set vertex buffer
		UINT stride = sizeof(Puer::Util::SimpleVertex);
		UINT offset = 0;
		device_->GetImmediateContext()->IASetVertexBuffers(0, 1, &vertexBuffer_.p, &stride, &offset);

		// Set index buffer
		device_->GetImmediateContext()->IASetIndexBuffer(indexBuffer_, DXGI_FORMAT_R16_UINT, 0);
		// Set primitive topology
		device_->GetImmediateContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//
		// Render the cube
		//
		device_->m_pDeviceContext->OMSetRenderTargets(1, &rtv_[0].p, device_->m_DepthStencilView);
//		device_->m_pDeviceContext->OMSetRenderTargets(1, &device_->m_pBackBufferView.p, device_->m_DepthStencilView);
		device_->GetImmediateContext()->VSSetShader(&program_forward_->getVertexShader(), nullptr, 0);
		program_forward_->SetLayout();
		device_->GetImmediateContext()->VSSetConstantBuffers(0, 1, &constantBuffer_.p);
		device_->GetImmediateContext()->PSSetShader(&program_forward_->getPixelShader(), nullptr, 0);
		device_->GetImmediateContext()->PSSetConstantBuffers(0, 1, &constantBuffer_.p);
		device_->GetImmediateContext()->DrawIndexed(36, 0, 0);


	}

	device_->Present();
}



