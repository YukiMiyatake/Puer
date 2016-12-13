#pragma once
#include "../DX/Device.h"
#include<memory>

#include <xnamath.h>

#include "../DX/program.h"


// forward declaration
namespace Puer {
	namespace DX11 {
//		class Program;
	}
}


// DeferredContextとImmediateContextにわける

class App {
public:
	struct SimpleVertex
	{
		XMFLOAT3 Pos;
		XMFLOAT3 Normal;
	};
	struct ItaVertex
	{
		XMFLOAT3 Pos;
		XMFLOAT2 uv;
	};
	struct ConstantBuffer
	{
		XMMATRIX mWorld;
		XMMATRIX mView;
		XMMATRIX mProjection;
		XMFLOAT4 vLightDir[2];
		XMFLOAT4 vLightColor[2];
		XMFLOAT4 vOutputColor;
	};

protected:
	HWND										hwnd_;
	std::unique_ptr<Puer::DX11::Device>			device_;

	std::unique_ptr<Puer::DX11::Program>		program_forward_;
	CComPtr<ID3D11Buffer>						vertexBuffer_;
	CComPtr<ID3D11Buffer>						indexBuffer_;
	CComPtr<ID3D11Buffer>						constantBuffer_;

	std::unique_ptr<Puer::DX11::Program>		program_deferred_;
	std::unique_ptr<Puer::DX11::Program>		program_deferred_light_;
	CComPtr<ID3D11Buffer>						vertexBuffer_Deferred_light_;

	// バッファ関連 classまとめたい
	CComPtr<ID3D11Texture2D>					gbuf_pos_;
	CComPtr<ID3D11ShaderResourceView>			srv_gbuf_pos_;
	CComPtr<ID3D11Texture2D>					gbuf_normaldepth_;
	CComPtr<ID3D11ShaderResourceView>			srv_gbuf_normaldepth_;
	CComPtr<ID3D11Texture2D>					gbuf_diffuse_;
	CComPtr<ID3D11ShaderResourceView>			srv_gbuf_diffuse_;

	CComPtr<ID3D11RenderTargetView>				rtv_[4];

	std::unique_ptr<Puer::DX11::Program>		program_debug_;

public:
	XMMATRIX                world_;
	XMMATRIX                view_;
	XMMATRIX                projection_;


public:
	App( HWND hwnd );


	const Puer::DX11::Device& getDevice();

	virtual void Init();
	virtual void Update();		// delta time
	virtual void Render();


};



