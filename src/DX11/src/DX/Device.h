#pragma once

#include"../stdafx.h"

// バックバッファフォーマットは可変にする
static const DXGI_FORMAT BACK_BUFFER_FORMAT = DXGI_FORMAT_R8G8B8A8_UNORM;		// バックバッファのフォーマット


namespace Puer {

	class Device {
	private:
		IDXGIFactory1*					m_pDXGIFactory;
		IDXGIAdapter1*					m_pDXGIAdapter;
		IDXGISwapChain*					m_pDXGISwpChain;
		ID3D11Device*					m_pDevice;
		ID3D11DeviceContext*			m_pDeviceContext;

		ID3DInclude*					m_pInclude;
	public:
		ID3D11Texture2D*				m_pBackBuffer;
		ID3D11RenderTargetView*			m_pBackBufferView;
		ID3D11DepthStencilView*			m_DepthStencilView;
		ID3D11RasterizerState*			m_pRSCullBack;
		ID3D11RasterizerState*			m_pRSCullNone;
		ID3D11BlendState*				m_pBlendState;
		ID3D11BlendState*				m_pBlendState_bg;
		ID3D11SamplerState*				m_pSamlerState;
		ID3D11DepthStencilState*		m_pDepthStencilState;
		ID3D11DepthStencilState*		m_pDepthStencilState_NoWrite;
		D3D_FEATURE_LEVEL				m_FeatureLevel;
		DXGI_SWAP_CHAIN_DESC			m_SwapChainDesc;
		std::vector<DXGI_SAMPLE_DESC>	m_MultiSampleDescList;
		std::vector<DXGI_MODE_DESC>		m_DisplayModeList;

		D3D11_VIEWPORT					m_Viewport;

		RECT						m_Rect;

	public:
		Device(void);
		~Device(void);

		bool Create(HWND hWnd, UINT nAdapter);
		void Release();
		void Present();
		//	void Clear( const float ClearColor[] );
		void ClearDepth(ID3D11DeviceContext *pContext);

		_inline HWND GetDeviceWindow();
		_inline ID3D11DeviceContext* GetImmediateContext();
		_inline ID3D11Device* GetDevice();
		_inline IDXGISwapChain* GetSwapChain();
		_inline D3D_FEATURE_LEVEL GetDeviceFeatureLevel();

		_inline ID3D11Texture2D* GetBackBuffer(void) { return m_pBackBuffer; }

		HRESULT SetCullMode(ID3D11DeviceContext *pContext, D3D11_CULL_MODE cullmode);


		void RestoreContext(ID3D11DeviceContext *pContext);


	};

}

