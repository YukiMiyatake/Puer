#include"../stdafx.h"
#include"program.h"

#include"Device.h"


namespace Puer {
	namespace DX11 {


		Program::Program(Device &device) 
			: device_(device) {
		}


		ID3D11VertexShader& Program::getVertexShader() {
			return( *vertexShader_ );
		}

		ID3D11HullShader& Program::getHullShader() {
			return( *hullShader_ );
		}

		ID3D11DomainShader& Program::getDomainShader() {
			return(*domainShader_);
		}

		ID3D11GeometryShader& Program::getGeometryShader() {
			return(*geometryShader_);
		}

		ID3D11PixelShader& Program::getPixelShader() {
			return(*pixelShader_);
		}




		// Ç‚ÇÕÇË Shaderä÷òAÇÕ ClassÇ…Ç‹Ç∆ÇﬂÇÊÇ§
		HRESULT Program::CompileShaderFromFile(
			WCHAR* szFileName, 
			LPCSTR szEntryPoint, 
			LPCSTR szShaderModel, 
			ID3DX11ThreadPump *pPump, 
			ID3DBlob** ppBlobOut
		)
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



		// IncludeÅAMacro îÒëŒâû 
		HRESULT Program::CreateVertexShaderFromFile(
			WCHAR* szFileName,
			LPCSTR szEntryPoint,
			D3D11_INPUT_ELEMENT_DESC *layout,
			UINT size,
			LPCSTR szShaderModel,
			ID3DX11ThreadPump *pPump
		) {

			HRESULT hr = S_OK;
			ID3DBlob* ppBlobOut = nullptr;
			ID3DBlob* pErrorBlob = nullptr;
			CComPtr<ID3D11InputLayout>      vertex_layout;


			if (szShaderModel == nullptr) {
				szShaderModel = "vs_4_0";
			}


			hr = CompileShaderFromFile(szFileName, szEntryPoint, szShaderModel, pPump, &ppBlobOut);
			if (FAILED(hr)) goto Err;

			hr = device_.GetDevice()->CreateVertexShader((ppBlobOut)->GetBufferPointer(), (ppBlobOut)->GetBufferSize(), NULL, &vertexShader_);
			if (FAILED(hr)) goto Err;

			hr = device_.GetDevice()->CreateInputLayout(layout, size, (ppBlobOut)->GetBufferPointer(),
				(ppBlobOut)->GetBufferSize(), &vertex_layout);

			if (FAILED(hr)) goto Err;

			// Set the input layout
			device_.GetImmediateContext()->IASetInputLayout(vertex_layout);



		Err:
			if (pErrorBlob) {
				OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
				pErrorBlob->Release();
				pErrorBlob = nullptr;
			}

			if (&ppBlobOut)if (ppBlobOut)(ppBlobOut)->Release();

			return(hr);
		}




		HRESULT Program::CreatePixelShaderFromFile(
			WCHAR* szFileName,
			LPCSTR szEntryPoint,
			LPCSTR szShaderModel,
			ID3DX11ThreadPump *pPump
		) {

			HRESULT hr = S_OK;
			ID3DBlob* ppBlobOut = nullptr;
			ID3DBlob* pErrorBlob = nullptr;

			if (szShaderModel == nullptr) {
				szShaderModel = "ps_4_0";
			}


			hr = CompileShaderFromFile(szFileName, szEntryPoint, szShaderModel, pPump, &ppBlobOut);
			if (FAILED(hr)) goto Err;

			hr = device_.GetDevice()->CreatePixelShader((ppBlobOut)->GetBufferPointer(), (ppBlobOut)->GetBufferSize(), NULL, &pixelShader_);
			if (FAILED(hr)) goto Err;


		Err:
			if (pErrorBlob) {
				OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
				pErrorBlob->Release();
				pErrorBlob = nullptr;
			}

			if (&ppBlobOut)if (ppBlobOut)(ppBlobOut)->Release();

			return(hr);
		}

	}
}

