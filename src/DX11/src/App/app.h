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




class App {
protected:
	HWND										hwnd_;
	std::unique_ptr<Puer::DX11::Device>			device_;

	std::unique_ptr<Puer::DX11::Program>		program_forward_;


	CComPtr<ID3D11Buffer>						vertexBuffer;
	CComPtr<ID3D11Buffer>						indexBuffer;
	CComPtr<ID3D11Buffer>						constantBuffer;

public:
	XMMATRIX                g_World;
	XMMATRIX                g_View;
	XMMATRIX                g_Projection;


public:
	App( HWND hwnd );


	const Puer::DX11::Device& getDevice();

	virtual void Init();
	virtual void Render();


};



