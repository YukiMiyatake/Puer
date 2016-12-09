#pragma once
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dcompiler.h>
#include <atlbase.h>


/*
 * シェーダーをまとめたやつ。今のところ VertexShaderとPixelShader
 *
 */



namespace Puer {
	namespace DX11 {
		class Device;


		class Program {
		protected:
			Device &device_;

			// このリソース templateれないか？
			CComPtr<ID3D11VertexShader>				vertexShader_;
			CComPtr<ID3D11HullShader>				hullShader_;
			CComPtr<ID3D11DomainShader>				domainShader_;
			CComPtr<ID3D11GeometryShader>			geometryShader_;
			CComPtr<ID3D11PixelShader>				pixelShader_;

			CComPtr<ID3D11InputLayout>				vertex_layout_;

		public:

			Program(Device &device);

			ID3D11VertexShader& getVertexShader();
			ID3D11HullShader& getHullShader();
			ID3D11DomainShader& getDomainShader();
			ID3D11GeometryShader& getGeometryShader();
			ID3D11PixelShader& getPixelShader();



			HRESULT CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DX11ThreadPump *pPump, ID3DBlob** ppBlobOut);

			HRESULT CreateVertexShaderFromFile(
				WCHAR* szFileName,
				LPCSTR szEntryPoint,
				D3D11_INPUT_ELEMENT_DESC *layout,
				UINT size,
				LPCSTR szShaderModel,
				ID3DX11ThreadPump *pPump
			);

			HRESULT CreatePixelShaderFromFile(
				WCHAR* szFileName,
				LPCSTR szEntryPoint,
				LPCSTR szShaderModel,
				ID3DX11ThreadPump *pPump
			);

			void SetLayout();



		};


	}
}
