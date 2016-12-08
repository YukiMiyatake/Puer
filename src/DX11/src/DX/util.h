#pragma once
#include <atlbase.h>

namespace Puer {
	namespace Util {


		HRESULT CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DX11ThreadPump *pPump, ID3DBlob** ppBlobOut);

		HRESULT CreateVertexShaderFromFile(
			ID3D11Device &device,
			WCHAR* szFileName,
			LPCSTR szEntryPoint,
			D3D11_INPUT_ELEMENT_DESC *layout,
			UINT size,
			LPCSTR szShaderModel,
			ID3DX11ThreadPump *pPump,
			ID3D11VertexShader** ppShader
		);
		
		HRESULT CreatePixelShaderFromFile(
			ID3D11Device &device,
			WCHAR* szFileName,
			LPCSTR szEntryPoint,
			LPCSTR szShaderModel,
			ID3DX11ThreadPump *pPump,
			ID3D11PixelShader** ppShader
		);



	}
}


