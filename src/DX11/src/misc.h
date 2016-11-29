/* 何処に入れるか不明なもの  ゴミ多し */

#pragma once
#include"stdafx.h"

#pragma pack(push, 1)

#define UI8 unsigned char

#ifndef _ALIGN_16
#define _ALIGN_16 __declspec(align(16))
#endif







#pragma pack(push, 1)
// DDSヘッダ構造体
struct DDS_HEADER {
public:
	DWORD	dwMagic;	// == 0x20534444  ' SDD'
	DWORD	dwSize;		// == 124
	DWORD	dwFlags;	// ヘッダ内の有効な情報 DDSD_* の組み合わせ
	DWORD	dwHeight;	// 画像の高さ x size
	DWORD	dwWidth;	// 画像の幅   y size
	DWORD	dwPitchOrLinearSize;	// 横1 line の byte 数 (pitch)
									// または 1面分の byte 数 (linearsize)
	DWORD	dwDepth;	// 画像の奥行き z size (Volume Texture 用)
	DWORD	dwMipMapCount;	// 含まれている mipmap レベル数
	DWORD	dwReserved1[11];
	DWORD	dwPfSize;	// == 32
	DWORD	dwPfFlags;	// pixel フォーマットを表す DDPF_* の組み合わせ
	DWORD	dwFourCC;	// フォーマットが FourCC であらわされる場合のみ
	DWORD	dwRGBBitCount;	// 1 pixel の bit 数
	DWORD	dwRBitMask;	// RGB format 時の mask
	DWORD	dwGBitMask;	// RGB format 時の mask
	DWORD	dwBBitMask;	// RGB format 時の mask
	DWORD	dwRGBAlphaBitMask;	// RGB format 時の mask
	DWORD	dwCaps;		// mipmap 等のフラグ指定用
	DWORD	dwCaps2;	// cube/volume texture 等のフラグ指定用
	DWORD	dwReservedCaps[2];
	DWORD	dwReserved2;
};

// Util的な
_ALIGN_16 struct MATRIX4x3
{
	//public:
	union {
		struct {
			float        _11, _12, _13, _14;
			float        _21, _22, _23, _24;
			float        _31, _32, _33, _34;
		};
		float m[3][4];
	};
};


// LXM用。将来的にはモデル種類ごとにかえれるような・・
_ALIGN_16
struct POSTEFFECT_VERTEX
{
	D3DXVECTOR4 pos;
	D3DXVECTOR2 uv;
};

// ピクセルシェーダー定数
_ALIGN_16
struct CB_PS_VIEWPORT
{
	float			fViewport[2];
};

// 頂点シェーダー用の定数
_ALIGN_16
struct CB_VS_GLOBAL
{
	D3DXMATRIXA16		matWVP;
};
// 頂点シェーダー用の定数(Null用)
_ALIGN_16
struct CB_VS_GLOBAL_NULL
{
	//		D3DXMATRIXA16		matWVP;
	D3DXMATRIXA16		matNULL;
	DWORD				boneIndex;
};

// ボーンマトリックス定数
_ALIGN_16
struct CB_VS_MATRIXPALETTE
{
	//		D3DXMATRIXA16	m_BoneMatrix[256];
	MATRIX4x3		m_BoneMatrix_VRAM[256];
};


#ifdef MODEL_HEAD_SCALE_TEST
// SCALE用の定数
_ALIGN_16
struct CB_VS_GLOBAL_SCALE
{
	D3DXVECTOR4		m_Scale;
	D3DXVECTOR4		m_Offset;
};
#endif

// OIT半透明
_ALIGN_16 struct FragmentData
{
	unsigned int nColor;
	float		 fDepth;
};

_ALIGN_16 struct FragmentLink
{
	FragmentData fragmentData;	// Fragment data
	unsigned int nNext;			// Link to next fragment
};
#pragma pack (pop)

// このあたりSharedPtrで囲うべきか
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(x)			if( (x) != NULL )												\
								{																\
									(x)->Release(); (x)=NULL;									\
								}
#endif


#ifndef SAFE_CLOSEHANDLE
#define SAFE_CLOSEHANDLE(x)		if( (x) != NULL )												\
								{																\
									CloseHandle(x);												\
									(x) = NULL;													\
								}
#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE(x)			if( (x) != NULL )												\
								{																\
									delete (x);													\
									(x) = NULL;													\
								}
#endif	// DELETE

#ifndef SAFE_DELETEA
#define SAFE_DELETEA(x)			if( (x) != NULL )												\
								{																\
									delete[] (x);												\
									(x) = NULL;													\
								}
#endif	// DELETEA


// fourCC   ４文字のBYTEコードをエンディアン変換してDWORDにする よくあるやつ
#ifdef FCC
#undef FCC
#endif
#define FCC(ch4) ((((DWORD)(ch4) & 0xFF) << 24) |     \
                  (((DWORD)(ch4) & 0xFF00) << 8) |    \
                  (((DWORD)(ch4) & 0xFF0000) >> 8) |  \
                  (((DWORD)(ch4) & 0xFF000000) >> 24))



#ifndef QI
#define QI		QueryInterface
#endif

#ifndef SWAP
#define SWAP(x, y)		((x)^=(y)^=(x)^=(y))
#endif

// VCデバッグウィンドウ表示
#ifdef _DEBUG
#define PRINTF( str, ... ) \
      { \
        TCHAR c[10000]; \
        _stprintf( c, str, __VA_ARGS__ ); \
        OutputDebugString( c ); \
      }
#else
#define PRINTF( str, ... ) // 空実装
#endif



#ifdef _DEBUG

bool _DX_Trace(HRESULT hr, LPCTSTR strMsg);
#define FAILED_TRACE(x,y)		 _DX_Trace((x),(y))
#else
#define FAILED_TRACE(x,y)		FAILED(x)
#endif



// パフォーマンス測定用 デバッグウインドウに出る
#ifdef _DEBUG
#define PERFORMANCE_COUNT( name, func, count ){														\
		DWORD _dwPERFORMANCE_COUNT = timeGetTime();																		\
		for( int i=0; i<(count); i++)																	\
		{																								\
				func;				\
		}																								\
		_dwPERFORMANCE_COUNT = timeGetTime() - _dwPERFORMANCE_COUNT;																		\
		char _chPERFORMANCE_COUNT[1024];																					\
		sprintf( _chPERFORMANCE_COUNT, "%s:: loop[%d]: Time[%dms]: FPS[%f]\n", (name), (count), _dwPERFORMANCE_COUNT, ((float)count)*1000.0 / (float)_dwPERFORMANCE_COUNT );	\
		OutputDebugString( _chPERFORMANCE_COUNT );}
#else
#define PERFORMANCE_COUNT( name, func, count ) func;

#endif


// ビット数求める for 32bit
#define BITCOUNT32(a,b){																							\
  (a) = (b & 0x55555555) + (b >> 1 & 0x55555555);																	\
  (a) = (a & 0x33333333) + (a >> 2 & 0x33333333);																	\
  (a) = (a & 0x0f0f0f0f) + (a >> 4 & 0x0f0f0f0f);																	\
  (a) = (a & 0x00ff00ff) + (a >> 8 & 0x00ff00ff);																	\
  (a) = (a & 0x0000ffff) + (a >>16 & 0x0000ffff);																	\
}


// 有効ビット数求める for 8Bit
#define BITSIZE8( y_, x_ ) {																						\
	int a_; int xx_; y_=0; xx_ = x_ -1;																				\
	a_ = xx_ >>4; if( a_ != 0 ){ y_=4; xx_ = a_; }																	\
	a_ = xx_ >>2; if( a_ != 0 ){ y_+=2; xx_ = a_; }																	\
	a_ = xx_ >>1; if( a_ != 0 ){ y_+=1; }																			\
}

//#define BITSIZE8( y_, x_ ) 



typedef struct
{
	float		x0;
	float		y0;
	float		x1;
	float		y1;
} FRECT;

struct TEXTURE_DATA
{
	DWORD				dwSize;
	DWORD				dwTime;
	DWORD				dwReserved[2];
	DDS_HEADER			dds_header;
	unsigned char		pData[];
};

enum SHADER_TYPE
{
	SHADER_TYPE_VERTEX = 0,
	SHADER_TYPE_GEOMETRY,
	SHADER_TYPE_HUL,
	SHADER_TYPE_DOMAIN,
	SHADER_TYPE_PIXEL,
	SHADER_TYPE_COMPUTE,
};

struct SHADER_LOAD_TABLE
{
	int						nShaderType;
	char					szID_STRING[256];
	char					szEntryPoint[256];
	char					szProfile[10];
	ID3D11DeviceChild**		ppShader;
};
#pragma pack(pop)

