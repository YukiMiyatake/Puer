#include "../stdafx.h"

#include"util.h"

namespace Puer {
	namespace Util {

		// Ç‚ÇÕÇË Shaderä÷òAÇÕ ClassÇ…Ç‹Ç∆ÇﬂÇÊÇ§
		HRESULT CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DX11ThreadPump *pPump, ID3DBlob** ppBlobOut)
		{


			HRESULT hr = S_OK;

			DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;

#if defined( DEBUG ) || defined( _DEBUG )
			dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

			ID3DBlob* pErrorBlob;
			hr = D3DX11CompileFromFile(szFileName, NULL, NULL, szEntryPoint, szShaderModel,
				dwShaderFlags, 0, pPump, ppBlobOut, &pErrorBlob, NULL);

			if (FAILED(hr)) goto Err;

Err:

			if (pErrorBlob) {
				OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
				pErrorBlob->Release();
				pErrorBlob = nullptr;
			}

			return hr;
		}


		// IncludeÅAMacro îÒëŒâû  templateÇ≈Ç´Ç»Ç¢Ç©Ç»
		HRESULT CreateVertexShaderFromFile(
			ID3D11Device &device,
			WCHAR* szFileName,
			LPCSTR szEntryPoint,
			D3D11_INPUT_ELEMENT_DESC *layout,
			UINT size,
			LPCSTR szShaderModel,
			ID3DX11ThreadPump *pPump,
			ID3D11VertexShader** ppShader 
		) {

			HRESULT hr = S_OK;
			LPCSTR sm;
			ID3DBlob** ppBlobOut=nullptr;
			ID3DBlob* pErrorBlob = nullptr;
			CComPtr<ID3D11InputLayout>      vertex_layout;


			if (szShaderModel == nullptr) {
				szShaderModel = "vs_4_0";
			}


			hr = CompileShaderFromFile(szFileName, szEntryPoint, szShaderModel, pPump, ppBlobOut );
			if (FAILED(hr)) goto Err;

			hr = device.CreateVertexShader((*ppBlobOut)->GetBufferPointer(), (*ppBlobOut)->GetBufferSize(), NULL, ppShader);
			if (FAILED(hr)) goto Err;

			hr = device.CreateInputLayout(layout, size, (*ppBlobOut)->GetBufferPointer(),
				(*ppBlobOut)->GetBufferSize(), &vertex_layout);

			if (FAILED(hr)) goto Err;

			// Set the input layout
			//g_pImmediateContext->IASetInputLayout(vertex_layout);



		Err:
			if (pErrorBlob) {
				OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
				pErrorBlob->Release();
				pErrorBlob = nullptr;
			}

			if (ppBlobOut)if (*ppBlobOut)(*ppBlobOut)->Release();

			return(hr);
		}



	
		HRESULT CreatePixelShaderFromFile(
			ID3D11Device &device,
			WCHAR* szFileName,
			LPCSTR szEntryPoint,
			LPCSTR szShaderModel,
			ID3DX11ThreadPump *pPump,
			ID3D11PixelShader** ppShader
		) {

			HRESULT hr = S_OK;
			LPCSTR sm;
			ID3DBlob** ppBlobOut = nullptr;
			ID3DBlob* pErrorBlob = nullptr;

			if (szShaderModel == nullptr) {
				szShaderModel = "ps_4_0";
			}


			hr = CompileShaderFromFile(szFileName, szEntryPoint, szShaderModel, pPump, ppBlobOut);
			if (FAILED(hr)) goto Err;

			hr = device.CreatePixelShader((*ppBlobOut)->GetBufferPointer(), (*ppBlobOut)->GetBufferSize(), NULL, ppShader);
			if (FAILED(hr)) goto Err;


		Err:
			if (pErrorBlob) {
				OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
				pErrorBlob->Release();
				pErrorBlob = nullptr;
			}

			if (ppBlobOut)if (*ppBlobOut)(*ppBlobOut)->Release();

			return(hr);
		}



	}
}
