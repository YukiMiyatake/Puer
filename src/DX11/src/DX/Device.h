#pragma once

#include <d3d11.h>
#include <d3dx11.h>
#include <d3dcompiler.h>
#include <atlbase.h>

#include<memory>



// バックバッファフォーマットは可変にしたいな・・
static const DXGI_FORMAT BACK_BUFFER_FORMAT = DXGI_FORMAT_R8G8B8A8_UNORM;		// バックバッファのフォーマット





namespace Puer {
	namespace DX11 {
		class Program;

		// Device初期化関連。バッファやらレジスタの初期化
		class Device {
		public:
			CComPtr<IDXGIFactory1>					m_pDXGIFactory;
			std::vector <CComPtr<IDXGIAdapter1>>	m_vAdapters;
			CComPtr<IDXGIAdapter1>					m_pDXGIAdapter;
			CComPtr<IDXGISwapChain>					m_pDXGISwpChain;

			CComPtr<ID3D11Device>					m_pDevice;
			CComPtr<ID3D11DeviceContext>			m_pDeviceContext;

			CComPtr<ID3D11Texture2D>				m_pBackBuffer;
			CComPtr<ID3D11RenderTargetView>			m_pBackBufferView;

			CComPtr<ID3D11DepthStencilView>			m_DepthStencilView;

			// Sampler以下は外に出したいなーーと
			CComPtr<ID3D11RasterizerState>			m_pRSCullBack;
			CComPtr<ID3D11RasterizerState>			m_pRSCullNone;
			CComPtr<ID3D11BlendState>				m_pBlendState;
			CComPtr<ID3D11BlendState>				m_pBlendState_bg;
			CComPtr<ID3D11SamplerState>				m_pSamlerState;
			CComPtr<ID3D11DepthStencilState>		m_pDepthStencilState;
			CComPtr<ID3D11DepthStencilState>		m_pDepthStencilState_NoWrite;
			D3D_FEATURE_LEVEL						m_FeatureLevel;
			DXGI_SWAP_CHAIN_DESC					m_SwapChainDesc;
			std::vector<DXGI_SAMPLE_DESC>			m_MultiSampleDescList;
			//	std::vector<DXGI_MODE_DESC>		m_DisplayModeList;

			D3D11_VIEWPORT					m_Viewport;

			RECT						m_Rect;



		public:
			Device(void);
			~Device(void);

			bool Create(HWND hWnd, UINT nAdapter = 0);
			std::unique_ptr<Program> CreateProgram();

				/*
				WCHAR* szFileName,
				LPCSTR szEntryPoint,
				D3D11_INPUT_ELEMENT_DESC *layout,
				UINT size,
				ID3DX11ThreadPump *pPump
			); // (IA) VS HS (TS) DS GS (RS) PS (OM) CS
			*/

			void Release();
			void Present();
			void Clear( ID3D11DeviceContext *pContext, const float ClearColor[]);
			void ClearDepth(ID3D11DeviceContext *pContext);

			_inline HWND GetDeviceWindow();
			_inline ID3D11DeviceContext* GetImmediateContext();
			_inline ID3D11Device* GetDevice();
			_inline IDXGISwapChain* GetSwapChain();
			_inline D3D_FEATURE_LEVEL GetDeviceFeatureLevel();

			_inline ID3D11Texture2D& GetBackBuffer(void) { return *m_pBackBuffer; }
			_inline ID3D11DepthStencilView& GetDepthStencilView(void) { return *m_DepthStencilView; }

			HRESULT SetCullMode(ID3D11DeviceContext *pContext, D3D11_CULL_MODE cullmode);
			
			ID3D11SamplerState& GetLinerSampler() { return(*m_pSamlerState); };


			void RestoreContext(ID3D11DeviceContext *pContext);


		};
	}
}

