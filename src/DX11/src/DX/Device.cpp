#include"../stdafx.h"
#include"Device.h"

#include"program.h"


// RAII�Ή����悤

namespace Puer {
	namespace DX11 {

		//==================================================================================================================
		// Construct & Destruct
		//==================================================================================================================
		Device::Device(void)
		{
		}

		Device::~Device(void)
		{
			Release();
		}

		void Device::Release()
		{
			// return to WindowMode
			if (m_pDXGISwpChain != nullptr)
			{
				BOOL IsFullscreen;
				m_pDXGISwpChain->GetFullscreenState(&IsFullscreen, nullptr);
				if (IsFullscreen)
				{
					m_pDXGISwpChain->SetFullscreenState(FALSE, nullptr);
				}
			}

		}


		//==================================================================================================================
		// 
		//==================================================================================================================
		bool Device::Create(HWND hWnd, UINT nAdapter)
		{
			IDXGIDevice1* pDXGIDev = NULL;
			IDXGIOutput* pOutput = NULL;
			ID3D11Texture2D* pTexDepth = NULL;
			D3D11_DEPTH_STENCIL_VIEW_DESC DepthViewDesc;
			D3D11_TEXTURE2D_DESC TexDesc;
			DXGI_SAMPLE_DESC SampleDesc = { 1, 0 };
			bool IsSucceeded = false;

			HRESULT hr;

			GetClientRect(hWnd, &m_Rect);

			D3D_FEATURE_LEVEL FeatureLevels[] = {
				D3D_FEATURE_LEVEL_11_0,		// DirectX11.0
				D3D_FEATURE_LEVEL_10_1,		// DirectX10.1
				D3D_FEATURE_LEVEL_10_0,		// DirectX10.0
				D3D_FEATURE_LEVEL_9_3,		// DirectX9/ShaderModel3
				D3D_FEATURE_LEVEL_9_2,		// DirectX9/ShaderModel2
				D3D_FEATURE_LEVEL_9_1,		// DirectX9/ShaderModel2
			};


			//--------------------------------------------------------------------
			// Factory Adapter
			//--------------------------------------------------------------------
			hr = CreateDXGIFactory1(__uuidof(IDXGIFactory), (void**)&m_pDXGIFactory);
			if FAILED(hr)
			{
				MessageBox(hWnd, _T("IDXGIFactory�̎擾�Ɏ��s���܂���"), _T("IDXGIAdapter::GetParent"), MB_ICONSTOP);
				goto EXIT;
			}

			hr = m_pDXGIFactory->EnumAdapters1(nAdapter, &m_pDXGIAdapter);
			if FAILED(hr)
			{
				MessageBox(hWnd, _T("IDXGIAdapter�̎擾�Ɏ��s���܂���"), _T("IDXGIDevice1::GetAdapter"), MB_ICONSTOP);
				goto EXIT;
			}

			{
				IDXGIAdapter1* pAdapter;
				int i = 0;
				while (m_pDXGIFactory->EnumAdapters1(i, &pAdapter) != DXGI_ERROR_NOT_FOUND) { m_vAdapters.push_back(pAdapter);  	++i; pAdapter->Release(); }
			}
			m_pDXGIAdapter = m_vAdapters[nAdapter];

			//--------------------------------------------------------------------
			// �f�o�C�X�̐���
			//--------------------------------------------------------------------
			hr = D3D11CreateDevice(
				m_pDXGIAdapter,												// ��������A�_�v�^�[
				D3D_DRIVER_TYPE_UNKNOWN,							// �n�[�h�E�F�A�f�o�C�X
				NULL,												// �\�t�g�E�F�A�f�o�C�X�̃��W���[��
																	//				D3D11_CREATE_DEVICE_SINGLETHREADED,					// �t���O
				0,					// �t���O
				FeatureLevels,										// �f�o�C�X�̏��������n�[�h�E�F�A�e�[�u��
				sizeof(FeatureLevels) / sizeof(D3D_FEATURE_LEVEL),	// ���̃e�[�u���̐�
				D3D11_SDK_VERSION,									// SDK�̃o�[�W����
				&m_pDevice,											// �f�o�C�X�i�[��
				&m_FeatureLevel,									// �������ł����f�o�C�X�i�[��
				&m_pDeviceContext);								// �f�o�C�X�R���e�L�X�g�i�[��
			if FAILED(hr) {
				MessageBox(hWnd, _T("�f�o�C�X�̐����Ɏ��s���܂���"), _T("D3D11CreateDevice"), MB_ICONSTOP);
				goto EXIT;
			}

			//--------------------------------------------------------------------
			// �f�o�C�X����IDXGIDevice1���擾����
			// ��������A�_�v�^�[�ƃt�@�N�g���[���擾
			//--------------------------------------------------------------------
			/*
			hr = m_pDevice->QueryInterface( __uuidof(IDXGIDevice1), (void**)&pDXGIDev );
			if FAILED( hr ) {
			MessageBox( hWnd, "IDXGIDevice1�̎擾�Ɏ��s���܂���", "QueryInterface", MB_ICONSTOP );
			goto EXIT;
			}

			// �A�_�v�^�[�擾
			hr = pDXGIDev->GetAdapter( &m_pDXGIAdapter );
			if FAILED( hr ) {
			MessageBox( hWnd, "IDXGIAdapter�̎擾�Ɏ��s���܂���", "IDXGIDevice1::GetAdapter", MB_ICONSTOP );
			goto EXIT;
			}

			// �t�@�N�g���[�擾
			hr = m_pDXGIAdapter->GetParent( __uuidof(IDXGIFactory), (void**)&m_pDXGIFactory );
			if FAILED( hr ) {
			MessageBox( hWnd, "IDXGIFactory�̎擾�Ɏ��s���܂���", "IDXGIAdapter::GetParent", MB_ICONSTOP );
			goto EXIT;
			}
			*/
			//--------------------------------------------------------------------
			// �t�@�N�g���[��ALT+ENTER�Ŏ����I�ɉ�ʃ��[�h�ؑւƂ��̎w��
			//--------------------------------------------------------------------
			hr = m_pDXGIFactory->MakeWindowAssociation(hWnd, 0);
			if FAILED(hr) {
				MessageBox(hWnd, _T("�t�@�N�g���[�ւ̐ݒ�Ɏ��s���܂���"), _T("MakeWindowAssociation"), MB_ICONSTOP);
				goto EXIT;
			}

			//--------------------------------------------------------------------------
			// MSAA�̎�ގ擾
			//--------------------------------------------------------------------------
#ifdef IS_MULTISAMPLE
		// �g����}���`�T���v���̎�ނ��
			for (long i = 1; i <= 4; i++) {
				UINT Quality;
				if SUCCEEDED(m_pDevice->CheckMultisampleQualityLevels(BACK_BUFFER_FORMAT, i, &Quality)) {
					if (Quality > 0) {
						DXGI_SAMPLE_DESC Desc = { i, Quality - 1 };
						m_MultiSampleDescList.push_back(Desc);
						SampleDesc = Desc;
					}
				}
			}
#else
		//�}���`�T���v�����O�Ȃ�
			SampleDesc.Count = 1;
			SampleDesc.Quality = 0;
#endif

			//--------------------------------------------------------------------------
			// ��ʃ��[�h��
			//--------------------------------------------------------------------------
			/*
			hr = m_pDXGIAdapter->EnumOutputs( 0, &pOutput );
			if FAILED( hr ) {
			MessageBox( hWnd, "IDXGIOutput�̎擾�Ɏ��s���܂���", "EnumOutputs", MB_ICONSTOP );
			goto EXIT;
			}

			hr = pOutput->GetDisplayModeList( BACK_BUFFER_FORMAT, 0, &ModeCount, NULL );
			if ( FAILED( hr ) || (ModeCount == 0) ) {
			MessageBox( hWnd, "��ʃ��[�h�̎擾�Ɏ��s���܂���", "GetDisplayModeList", MB_ICONSTOP );
			goto EXIT;
			}

			m_DisplayModeList.resize( ModeCount );
			pOutput->GetDisplayModeList( BACK_BUFFER_FORMAT, 0, &ModeCount, &(m_DisplayModeList[0]) );
			if FAILED( hr ) {
			MessageBox( hWnd, "��ʃ��[�h�̎擾�Ɏ��s���܂���", "GetDisplayModeList", MB_ICONSTOP );
			goto EXIT
			}

			// ��ʃ��[�h�I��
			DisplayMode = m_DisplayModeList[0];
			for ( UINT i = 0; i < ModeCount; i++ ) {
			// ���t���b�V�����[�g60Hz�̂�D��I�Ɏg��
			if ( m_DisplayModeList[i].RefreshRate.Numerator / m_DisplayModeList[i].RefreshRate.Denominator == 60 ) {
			DisplayMode = m_DisplayModeList[i];
			break;
			}
			}
			//--------------------------------------------------------------------
			// �X���b�v�`�F�C���p�̃p�����[�^�[
			//--------------------------------------------------------------------
			m_SwapChainDesc.BufferDesc.Width			= m_Rect.right - m_Rect.left;				// �o�b�N�o�b�t�@�̉�
			m_SwapChainDesc.BufferDesc.Height			= m_Rect.bottom - m_Rect.top;				// �o�b�N�o�b�t�@�̏c
			m_SwapChainDesc.BufferDesc.Format			= DisplayMode.Format;						// �o�b�N�o�b�t�@�̃t�H�[�}�b�g
			m_SwapChainDesc.BufferDesc.RefreshRate		= DisplayMode.RefreshRate;					// ���t���b�V�����[�g
			m_SwapChainDesc.BufferDesc.Scaling			= DisplayMode.Scaling;						// �X�L�������C���̃p�����[�^�[
			m_SwapChainDesc.BufferDesc.ScanlineOrdering	= DisplayMode.ScanlineOrdering;				// �X�L�������C���̃p�����[�^�[
			m_SwapChainDesc.SampleDesc					= SampleDesc;								// �}���`�T���v��
			m_SwapChainDesc.BufferUsage					= DXGI_USAGE_RENDER_TARGET_OUTPUT;			// �o�b�N�o�b�t�@�̃t���O
			m_SwapChainDesc.BufferCount					= 2;										// �o�b�N�o�b�t�@��
			m_SwapChainDesc.OutputWindow				= hWnd;										// �֘A�t����E�B���h�E�̃n���h��
			m_SwapChainDesc.Windowed					= 1;										// �E�B���h�E���[�h�ŋN�����邩�ۂ�
			m_SwapChainDesc.SwapEffect					= DXGI_SWAP_EFFECT_DISCARD;					// ��ʍX�V�̕��@
			m_SwapChainDesc.Flags						= DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;	// �X���b�v�`�F�C���̃t���O
			*/
			/*
			RECT rc;
			GetClientRect(hWnd, &rc);
			UINT width = rc.right - rc.left;
			UINT height = rc.bottom - rc.top;
			*/
			DXGI_MODE_DESC sd;

			// �f�B�X�v���C���[�h���Œ�őI������ꍇ�̃T���v���R�[�h
			::ZeroMemory(&sd, sizeof(sd));
			sd.Width = m_Rect.right - m_Rect.left;
			sd.Height = m_Rect.bottom - m_Rect.top;
			sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			sd.RefreshRate.Numerator = 60;
			sd.RefreshRate.Denominator = 1;
			sd.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
			sd.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;

			//--------------------------------------------------------------------
			// �X���b�v�`�F�C���p�̃p�����[�^�[
			//--------------------------------------------------------------------
			m_SwapChainDesc.BufferDesc.Width = m_Rect.right - m_Rect.left;				// �o�b�N�o�b�t�@�̉�
			m_SwapChainDesc.BufferDesc.Height = m_Rect.bottom - m_Rect.top;				// �o�b�N�o�b�t�@�̏c
			m_SwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;						// �o�b�N�o�b�t�@�̃t�H�[�}�b�g
			m_SwapChainDesc.BufferDesc.RefreshRate = sd.RefreshRate;					// ���t���b�V�����[�g
			m_SwapChainDesc.BufferDesc.Scaling = sd.Scaling;						// �X�L�������C���̃p�����[�^�[
			m_SwapChainDesc.BufferDesc.ScanlineOrdering = sd.ScanlineOrdering;				// �X�L�������C���̃p�����[�^�[
			m_SwapChainDesc.SampleDesc = SampleDesc;								// �}���`�T���v��
			m_SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;			// �o�b�N�o�b�t�@�̃t���O
			m_SwapChainDesc.BufferCount = 2;										// �o�b�N�o�b�t�@��
			m_SwapChainDesc.OutputWindow = hWnd;										// �֘A�t����E�B���h�E�̃n���h��
			m_SwapChainDesc.Windowed = true;										// �E�B���h�E���[�h�ŋN�����邩�ۂ�
			m_SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;					// ��ʍX�V�̕��@
			m_SwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;	// �X���b�v�`�F�C���̃t���O

																			//--------------------------------------------------------------------
																			// �X���b�v�`�F�C�����f�o�C�X����쐬����
																			//--------------------------------------------------------------------
			hr = m_pDXGIFactory->CreateSwapChain(m_pDevice, &m_SwapChainDesc, &m_pDXGISwpChain);
			if FAILED(hr) {
				MessageBox(hWnd, _T("�X���b�v�`�F�C���̐����Ɏ��s���܂���"), _T("CreateSwapChain"), MB_ICONSTOP);
				goto EXIT;
			}

			//--------------------------------------------------------------------
			// �X���b�v�`�F�C���̃o�b�N�o�b�t�@���擾����
			//--------------------------------------------------------------------
			hr = m_pDXGISwpChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&m_pBackBuffer);
			if FAILED(hr) {
				MessageBox(hWnd, _T("�o�b�N�o�b�t�@�̎擾�Ɏ��s���܂���"), _T("GetBuffer"), MB_ICONSTOP);
				goto EXIT;
			}

			//--------------------------------------------------------------------
			// �`��^�[�Q�b�g�r���[���o�b�N�o�b�t�@����쐬����
			//--------------------------------------------------------------------
			hr = m_pDevice->CreateRenderTargetView(m_pBackBuffer, NULL, &m_pBackBufferView);
			if FAILED(hr) {
				MessageBox(hWnd, _T("�����_�[�^�[�Q�b�g�r���[�̐����Ɏ��s���܂���"), _T("CreateRenderTargetView"), MB_ICONSTOP);
				goto EXIT;
			}

			ZeroMemory(&TexDesc, sizeof(TexDesc));
			TexDesc.Width = m_SwapChainDesc.BufferDesc.Width;
			TexDesc.Height = m_SwapChainDesc.BufferDesc.Height;
			TexDesc.MipLevels = 1;
			TexDesc.ArraySize = 1;
			TexDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; //DXGI_FORMAT_R24G8_TYPELESS;
			TexDesc.SampleDesc = m_SwapChainDesc.SampleDesc;
			TexDesc.Usage = D3D11_USAGE_DEFAULT;
			TexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
			TexDesc.CPUAccessFlags = 0;
			TexDesc.MiscFlags = 0;
			hr = m_pDevice->CreateTexture2D(&TexDesc, NULL, &pTexDepth);
			if FAILED(hr) {
				MessageBox(hWnd, _T("�[�x�X�e���V���r���[�p�e�N�X�`���̐����Ɏ��s���܂���"), _T("CreateDepthStencilView"), MB_ICONSTOP);
				goto EXIT;
			}





			//--------------------------------------------------------------------
			// �[�x�X�e���V���r���[�̐���
			//--------------------------------------------------------------------
			ZeroMemory(&DepthViewDesc, sizeof(DepthViewDesc));
			DepthViewDesc.Format = TexDesc.Format;//DXGI_FORMAT_D24_UNORM_S8_UINT;
			DepthViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
//			DepthViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
			DepthViewDesc.Texture2D.MipSlice = 0;

			DepthViewDesc.Flags = 0;
			hr = m_pDevice->CreateDepthStencilView(
				pTexDepth,
				&DepthViewDesc,
				&m_DepthStencilView);
			if FAILED(hr) {
				MessageBox(hWnd, _T("�[�x�X�e���V���r���[�̐����Ɏ��s���܂���"), _T("CreateDepthStencilView"), MB_ICONSTOP);
				goto EXIT;
			}

			//--------------------------------------------------------------------
			// �����_�����O�^�[�Q�b�g�̐ݒ�
			//--------------------------------------------------------------------
			ID3D11RenderTargetView* pTbl[] = { m_pBackBufferView };//NULL, m_pRenderTarget
			m_pDeviceContext->OMSetRenderTargets(1, pTbl, m_DepthStencilView);

			//--------------------------------------------------------------------
			// �r���[�|�[�g�̐ݒ�
			//--------------------------------------------------------------------
			m_Viewport.TopLeftX = 0.0f;
			m_Viewport.TopLeftY = 0.0f;
			m_Viewport.Width = (FLOAT)m_SwapChainDesc.BufferDesc.Width;
			m_Viewport.Height = (FLOAT)m_SwapChainDesc.BufferDesc.Height;
			m_Viewport.MinDepth = 0.0f;
			m_Viewport.MaxDepth = 1.0f;
			m_pDeviceContext->RSSetViewports(1, &m_Viewport);

			//----------------------------------------------------------------------
			// �u�����h�X�e�[�g����
			//----------------------------------------------------------------------
			{
#if 0

#if 0
				D3D11_BLEND_DESC BlendDesc = {
					FALSE,			// BOOL AlphaToCoverageEnable;
					FALSE,			// BOOL IndependentBlendEnable;
				};
				for (int i = 0; i < 8; i++)
				{
					BlendDesc.RenderTarget[i].BlendEnable = FALSE;						// BOOL BlendEnable;
					BlendDesc.RenderTarget[i].SrcBlend = D3D11_BLEND_ONE;				// D3D11_BLEND SrcBlend;
					BlendDesc.RenderTarget[i].DestBlend = D3D11_BLEND_ZERO;				// D3D11_BLEND DestBlend;
					BlendDesc.RenderTarget[i].BlendOp = D3D11_BLEND_OP_ADD;			// D3D11_BLEND_OP BlendOp;
					BlendDesc.RenderTarget[i].SrcBlendAlpha = D3D11_BLEND_ONE;				// D3D11_BLEND SrcBlendAlpha;
					BlendDesc.RenderTarget[i].DestBlendAlpha = D3D11_BLEND_ZERO;				// D3D11_BLEND DestBlendAlpha;
					BlendDesc.RenderTarget[i].BlendOpAlpha = D3D11_BLEND_OP_ADD;			// D3D11_BLEND_OP BlendOpAlpha;
					BlendDesc.RenderTarget[i].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;	// UINT8 RenderTargetWriteMask;
				}
#else
				D3D11_BLEND_DESC BlendDesc = {
					FALSE,			// BOOL AlphaToCoverageEnable;
					FALSE,			// BOOL IndependentBlendEnable;
				};
				for (int i = 0; i < 8; i++)
				{
					BlendDesc.RenderTarget[i].BlendEnable = TRUE;						// BOOL BlendEnable;
					BlendDesc.RenderTarget[i].SrcBlend = D3D11_BLEND_SRC_ALPHA;				// D3D11_BLEND SrcBlend;
					BlendDesc.RenderTarget[i].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;				// D3D11_BLEND DestBlend;
					BlendDesc.RenderTarget[i].BlendOp = D3D11_BLEND_OP_ADD;			// D3D11_BLEND_OP BlendOp;
					BlendDesc.RenderTarget[i].SrcBlendAlpha = D3D11_BLEND_ONE;				// D3D11_BLEND SrcBlendAlpha;
					BlendDesc.RenderTarget[i].DestBlendAlpha = D3D11_BLEND_ONE;				// D3D11_BLEND DestBlendAlpha;
					BlendDesc.RenderTarget[i].BlendOpAlpha = D3D11_BLEND_OP_MAX;			// D3D11_BLEND_OP BlendOpAlpha;
					BlendDesc.RenderTarget[i].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;	// UINT8 RenderTargetWriteMask;
					BlendDesc.RenderTarget[i].BlendEnable = TRUE;						// BOOL BlendEnable;
					BlendDesc.RenderTarget[i].SrcBlend = D3D11_BLEND_SRC_ALPHA;				// D3D11_BLEND SrcBlend;
					BlendDesc.RenderTarget[i].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;				// D3D11_BLEND DestBlend;
					BlendDesc.RenderTarget[i].BlendOp = D3D11_BLEND_OP_ADD;			// D3D11_BLEND_OP BlendOp;
					BlendDesc.RenderTarget[i].SrcBlendAlpha = D3D11_BLEND_ONE;				// D3D11_BLEND SrcBlendAlpha;
					BlendDesc.RenderTarget[i].DestBlendAlpha = D3D11_BLEND_ONE;				// D3D11_BLEND DestBlendAlpha;
					BlendDesc.RenderTarget[i].BlendOpAlpha = D3D11_BLEND_OP_ADD;			// D3D11_BLEND_OP BlendOpAlpha;
					BlendDesc.RenderTarget[i].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;	// UINT8 RenderTargetWriteMask;
				}
#endif
				hr = GetDevice()->CreateBlendState(&BlendDesc, &m_pBlendState);
				if FAILED(hr)
				{
					MessageBox(hWnd, _T("�u�����h�X�e�[�g�̐����Ɏ��s"), _T("ERROR"), MB_ICONSTOP);
					goto EXIT;
				}





				for (int i = 0; i < 8; i++)
				{
					BlendDesc.RenderTarget[i].BlendEnable = FALSE;						// BOOL BlendEnable;
					BlendDesc.RenderTarget[i].SrcBlend = D3D11_BLEND_ONE;				// D3D11_BLEND SrcBlend;
					BlendDesc.RenderTarget[i].DestBlend = D3D11_BLEND_ZERO;				// D3D11_BLEND DestBlend;
					BlendDesc.RenderTarget[i].BlendOp = D3D11_BLEND_OP_ADD;			// D3D11_BLEND_OP BlendOp;
					BlendDesc.RenderTarget[i].SrcBlendAlpha = D3D11_BLEND_ZERO;				// D3D11_BLEND SrcBlendAlpha;
					BlendDesc.RenderTarget[i].DestBlendAlpha = D3D11_BLEND_ZERO;				// D3D11_BLEND DestBlendAlpha;
					BlendDesc.RenderTarget[i].BlendOpAlpha = D3D11_BLEND_OP_MIN;			// D3D11_BLEND_OP BlendOpAlpha;
					BlendDesc.RenderTarget[i].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;	// UINT8 RenderTargetWriteMask;
				}
				hr = GetDevice()->CreateBlendState(&BlendDesc, &m_pBlendState_bg);

#endif
			}

			//----------------------------------------------------------------------
			// ���X�^���C�U�[�X�e�[�g����
			//----------------------------------------------------------------------
			{
#if 1
				D3D11_RASTERIZER_DESC RasterizerDesc = {
					D3D11_FILL_SOLID,	// D3D11_FILL_MODE FillMode;
					D3D11_CULL_BACK,	// D3D11_CULL_MODE CullMode;
										//			D3D11_CULL_NONE,	// D3D11_CULL_MODE CullMode;
										FALSE,				// BOOL FrontCounterClockwise;
										0,					// INT DepthBias;
										0.0f,				// FLOAT DepthBiasClamp;
										FALSE,				// FLOAT SlopeScaledDepthBias;
										TRUE,				// BOOL DepthClipEnable;
										FALSE,				// BOOL ScissorEnable;
										FALSE,				// BOOL MultisampleEnable;
										TRUE,				// BOOL AntialiasedLineEnable;
				};
				hr = GetDevice()->CreateRasterizerState(&RasterizerDesc, &m_pRSCullBack);
				if FAILED(hr)
				{
					MessageBox(hWnd, _T("���X�^���C�U�[�X�e�[�g�̐����Ɏ��s"), _T("ERROR"), MB_ICONSTOP);
					goto EXIT;
				}

				RasterizerDesc.CullMode = D3D11_CULL_NONE;
				hr = GetDevice()->CreateRasterizerState(&RasterizerDesc, &m_pRSCullNone);
				if FAILED(hr)
				{
					MessageBox(hWnd, _T("���X�^���C�U�[�X�e�[�g�̐����Ɏ��s"), _T("ERROR"), MB_ICONSTOP);
					goto EXIT;
				}
#endif
			}

			//----------------------------------------------------------------------
			// �e�N�X�`���T���v���[�X�e�[�g
			//----------------------------------------------------------------------
			{
				D3D11_SAMPLER_DESC SamlerDesc = {
					//			D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR,		// D3D11_FILTER Filter;
					D3D11_FILTER_MIN_MAG_MIP_LINEAR,		// D3D11_FILTER Filter;
					D3D11_TEXTURE_ADDRESS_WRAP,				// D3D11_TEXTURE_ADDRESS_MODE AddressU;
					D3D11_TEXTURE_ADDRESS_WRAP,				// D3D11_TEXTURE_ADDRESS_MODE AddressV;
					D3D11_TEXTURE_ADDRESS_WRAP,				// D3D11_TEXTURE_ADDRESS_MODE AddressW;
					0.0f,									// FLOAT MipLODBias;
					1,										// UINT MaxAnisotropy;
					D3D11_COMPARISON_ALWAYS,				// D3D11_COMPARISON_FUNC ComparisonFunc;
					{ 0.0f, 0.0f, 0.0f, 0.0f },				// FLOAT BorderColor[ 4 ];
					0.0f,									// FLOAT MinLOD;
					D3D11_FLOAT32_MAX,						// FLOAT MaxLOD;
				};
				// �T���v���[�X�e�[�g�X�e�[�g�ݒ�
				hr = GetDevice()->CreateSamplerState(&SamlerDesc, &m_pSamlerState);
				if FAILED(hr)
				{
					MessageBox(hWnd, _T("�e�N�X�`���T���v���[�X�e�[�g�̐����Ɏ��s"), _T("ERROR"), MB_ICONSTOP);
					goto EXIT;
				}
			}

			//----------------------------------------------------------------------
			// �[�x�X�e���V���X�e�[�g
			//----------------------------------------------------------------------
			{
				// �ʏ�
				D3D11_DEPTH_STENCIL_DESC DepthDesc = {
					TRUE,							// BOOL DepthEnable;
					D3D11_DEPTH_WRITE_MASK_ALL,		// D3D11_DEPTH_WRITE_MASK DepthWriteMask;
					D3D11_COMPARISON_LESS_EQUAL,	// D3D11_COMPARISON_FUNC DepthFunc;
				};
				hr = GetDevice()->CreateDepthStencilState(&DepthDesc, &m_pDepthStencilState);
				if FAILED(hr)
				{
					MessageBox(hWnd, _T("�[�x�X�e���V���X�e�[�g�̐����Ɏ��s"), _T("ERROR"), MB_ICONSTOP);
					goto EXIT;
				}
			}
			{
				// ZWrite�Ȃ�
				D3D11_DEPTH_STENCIL_DESC DepthDesc = {
					TRUE,							// BOOL DepthEnable;
					D3D11_DEPTH_WRITE_MASK_ZERO,	// D3D11_DEPTH_WRITE_MASK DepthWriteMask;
					D3D11_COMPARISON_LESS_EQUAL,	// D3D11_COMPARISON_FUNC DepthFunc;
				};
				hr = GetDevice()->CreateDepthStencilState(&DepthDesc, &m_pDepthStencilState_NoWrite);
				if FAILED(hr)
				{
					MessageBox(hWnd, _T("�[�x�X�e���V���X�e�[�g�̐����Ɏ��s"), _T("ERROR"), MB_ICONSTOP);
					goto EXIT;
				}
			}


			RestoreContext(GetImmediateContext());

			//--------------------------------------------------------------------
			// �S������������˔j����
			//--------------------------------------------------------------------
			IsSucceeded = true;

		EXIT:
			SAFE_RELEASE(pTexDepth);
			SAFE_RELEASE(pOutput);
			SAFE_RELEASE(pDXGIDev);

			return IsSucceeded;
		}


		std::unique_ptr<Program> Device::CreateProgram(){

			return std::make_unique<Program>(*this); 
		};



		void Device::RestoreContext(ID3D11DeviceContext *pContext)
		{
			ID3D11SamplerState* pSamplerTbl[] = { m_pSamlerState };
			pContext->PSSetSamplers(0, 1, pSamplerTbl);
			pContext->RSSetState(m_pRSCullBack);
			pContext->OMSetDepthStencilState(m_pDepthStencilState, 0);
//			pContext->OMSetBlendState(m_pBlendState, NULL, 0xFFFFFFFF);
		}


		// ���X�^���C�U�X�e�[�g�ݒ�
		HRESULT Device::SetCullMode(ID3D11DeviceContext *pContext, D3D11_CULL_MODE cullmode)
		{
			HRESULT hr = S_OK;

			if (cullmode == D3D11_CULL_BACK)
			{
				pContext->RSSetState(m_pRSCullBack);
			}
			else if (cullmode == D3D11_CULL_NONE)
			{
				pContext->RSSetState(m_pRSCullNone);
			}
			else
			{
				hr = E_FAIL;
			}

			return(hr);

		}

		//==================================================================================================================
		// ��ʃN���A
		//==================================================================================================================
		
		void Device::Clear(ID3D11DeviceContext *pContext, const float ClearColor[])
		{
			pContext->ClearRenderTargetView( m_pBackBufferView, ClearColor );
			//pContext->ClearDepthStencilView( m_DepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0 );
		}
		
		void Device::ClearDepth(ID3D11DeviceContext *pContext)
		{
			pContext->ClearDepthStencilView(m_DepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		}

		//==================================================================================================================
		// ��ʍX�V
		//==================================================================================================================
		void Device::Present()
		{
			HRESULT hr = S_OK;
			hr = m_pDXGISwpChain->Present(0, 0);
			if FAILED(hr)
			{
				MessageBox(m_SwapChainDesc.OutputWindow, _T("��ʂ̃X���b�v�Ɏ��s���܂���"), _T("Present"), MB_ICONSTOP);
				SendMessage(m_SwapChainDesc.OutputWindow, WM_CLOSE, 0, 0);
			}
		}

		//==================================================================================================================
		// �f�o�C�X�p�E�B���h�E�̎擾
		//==================================================================================================================
		HWND Device::GetDeviceWindow()
		{
			return m_SwapChainDesc.OutputWindow;
		}

		//==================================================================================================================
		// ID3D11DeviceContext�擾
		//==================================================================================================================
		ID3D11DeviceContext* Device::GetImmediateContext()
		{
			return m_pDeviceContext;
		}

		//==================================================================================================================
		// ID3D11Device�擾
		//==================================================================================================================
		ID3D11Device* Device::GetDevice()
		{
			return m_pDevice;
		}

		//==================================================================================================================
		// IDXGISwapChain�擾
		//==================================================================================================================
		IDXGISwapChain* Device::GetSwapChain()
		{
			return m_pDXGISwpChain;
		}

		//==================================================================================================================
		// D3D_FEATURE_LEVEL�擾
		//==================================================================================================================
		D3D_FEATURE_LEVEL Device::GetDeviceFeatureLevel()
		{
			return m_FeatureLevel;
		}


		HRESULT Device::CreateBuffer(void *buffer, int byteWidth, D3D11_BIND_FLAG bind, CComPtr<ID3D11Buffer> &vertexBuffer_){
			
			D3D11_BUFFER_DESC bd;
			ZeroMemory(&bd, sizeof(bd));
			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.ByteWidth = byteWidth;
			bd.BindFlags = bind;
			bd.CPUAccessFlags = 0;

			D3D11_SUBRESOURCE_DATA *pdata = nullptr;
			D3D11_SUBRESOURCE_DATA InitData;
			if (buffer) {
				ZeroMemory(&InitData, sizeof(InitData));
				InitData.pSysMem = buffer;
				pdata = &InitData;
			}

			return( m_pDevice->CreateBuffer(&bd, pdata, &vertexBuffer_));
		}

	}
}


