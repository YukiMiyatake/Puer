/* �����ɓ���邩�s���Ȃ���  �S�~���� */

#pragma once
#include"stdafx.h"

#pragma pack(push, 1)

#define UI8 unsigned char

#ifndef _ALIGN_16
#define _ALIGN_16 __declspec(align(16))
#endif







#pragma pack(push, 1)
// DDS�w�b�_�\����
struct DDS_HEADER {
public:
	DWORD	dwMagic;	// == 0x20534444  ' SDD'
	DWORD	dwSize;		// == 124
	DWORD	dwFlags;	// �w�b�_���̗L���ȏ�� DDSD_* �̑g�ݍ��킹
	DWORD	dwHeight;	// �摜�̍��� x size
	DWORD	dwWidth;	// �摜�̕�   y size
	DWORD	dwPitchOrLinearSize;	// ��1 line �� byte �� (pitch)
									// �܂��� 1�ʕ��� byte �� (linearsize)
	DWORD	dwDepth;	// �摜�̉��s�� z size (Volume Texture �p)
	DWORD	dwMipMapCount;	// �܂܂�Ă��� mipmap ���x����
	DWORD	dwReserved1[11];
	DWORD	dwPfSize;	// == 32
	DWORD	dwPfFlags;	// pixel �t�H�[�}�b�g��\�� DDPF_* �̑g�ݍ��킹
	DWORD	dwFourCC;	// �t�H�[�}�b�g�� FourCC �ł���킳���ꍇ�̂�
	DWORD	dwRGBBitCount;	// 1 pixel �� bit ��
	DWORD	dwRBitMask;	// RGB format ���� mask
	DWORD	dwGBitMask;	// RGB format ���� mask
	DWORD	dwBBitMask;	// RGB format ���� mask
	DWORD	dwRGBAlphaBitMask;	// RGB format ���� mask
	DWORD	dwCaps;		// mipmap ���̃t���O�w��p
	DWORD	dwCaps2;	// cube/volume texture ���̃t���O�w��p
	DWORD	dwReservedCaps[2];
	DWORD	dwReserved2;
};

// Util�I��
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


// LXM�p�B�����I�ɂ̓��f����ނ��Ƃɂ������悤�ȁE�E
_ALIGN_16
struct POSTEFFECT_VERTEX
{
	D3DXVECTOR4 pos;
	D3DXVECTOR2 uv;
};

// �s�N�Z���V�F�[�_�[�萔
_ALIGN_16
struct CB_PS_VIEWPORT
{
	float			fViewport[2];
};

// ���_�V�F�[�_�[�p�̒萔
_ALIGN_16
struct CB_VS_GLOBAL
{
	D3DXMATRIXA16		matWVP;
};
// ���_�V�F�[�_�[�p�̒萔(Null�p)
_ALIGN_16
struct CB_VS_GLOBAL_NULL
{
	//		D3DXMATRIXA16		matWVP;
	D3DXMATRIXA16		matNULL;
	DWORD				boneIndex;
};

// �{�[���}�g���b�N�X�萔
_ALIGN_16
struct CB_VS_MATRIXPALETTE
{
	//		D3DXMATRIXA16	m_BoneMatrix[256];
	MATRIX4x3		m_BoneMatrix_VRAM[256];
};


#ifdef MODEL_HEAD_SCALE_TEST
// SCALE�p�̒萔
_ALIGN_16
struct CB_VS_GLOBAL_SCALE
{
	D3DXVECTOR4		m_Scale;
	D3DXVECTOR4		m_Offset;
};
#endif

// OIT������
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

// ���̂�����SharedPtr�ň͂��ׂ���
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


// fourCC   �S������BYTE�R�[�h���G���f�B�A���ϊ�����DWORD�ɂ��� �悭������
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

// VC�f�o�b�O�E�B���h�E�\��
#ifdef _DEBUG
#define PRINTF( str, ... ) \
      { \
        TCHAR c[10000]; \
        _stprintf( c, str, __VA_ARGS__ ); \
        OutputDebugString( c ); \
      }
#else
#define PRINTF( str, ... ) // �����
#endif



#ifdef _DEBUG

bool _DX_Trace(HRESULT hr, LPCTSTR strMsg);
#define FAILED_TRACE(x,y)		 _DX_Trace((x),(y))
#else
#define FAILED_TRACE(x,y)		FAILED(x)
#endif



// �p�t�H�[�}���X����p �f�o�b�O�E�C���h�E�ɏo��
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


// �r�b�g�����߂� for 32bit
#define BITCOUNT32(a,b){																							\
  (a) = (b & 0x55555555) + (b >> 1 & 0x55555555);																	\
  (a) = (a & 0x33333333) + (a >> 2 & 0x33333333);																	\
  (a) = (a & 0x0f0f0f0f) + (a >> 4 & 0x0f0f0f0f);																	\
  (a) = (a & 0x00ff00ff) + (a >> 8 & 0x00ff00ff);																	\
  (a) = (a & 0x0000ffff) + (a >>16 & 0x0000ffff);																	\
}


// �L���r�b�g�����߂� for 8Bit
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

