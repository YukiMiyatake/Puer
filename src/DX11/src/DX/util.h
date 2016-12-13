#pragma once

//#include <atlbase.h>
//#include <d3d11.h>
//#include <d3dx11.h>
#include <xnamath.h>


namespace Puer {
	namespace Util {

		struct SimpleVertex
		{
			XMFLOAT3 Pos;
			XMFLOAT3 Normal;
		};
		D3D11_INPUT_ELEMENT_DESC simpleInputLayout[];
		UINT simpleInputLayoutCount();
		
		// SimpleBox
		SimpleVertex simpleBoxVertex[];
		WORD simpleBoxIndex[];



		struct ItaVertex
		{
			XMFLOAT3 Pos;
			XMFLOAT2 uv;
		};
		ItaVertex itaVertex[];
		D3D11_INPUT_ELEMENT_DESC ItaInputLayout[];
		UINT itaInputLayoutCount();
		D3D11_INPUT_ELEMENT_DESC ItaTexInputLayout[];
		UINT itaTexInputLayoutCount();

	}
}


