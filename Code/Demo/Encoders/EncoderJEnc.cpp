//--------------------------------------------------------------------------------------
// Real-Time JPEG Compression using DirectCompute - Demo
//
// Copyright (c) Stefan Petersson 2012. All rights reserved.
//--------------------------------------------------------------------------------------
#include "EncoderJEnc.h"

#if defined( DEBUG ) || defined( _DEBUG )
#pragma comment(lib, "JEncD.lib")
#else
#pragma comment(lib, "JEnc.lib")
#endif

EncoderJEnc::EncoderJEnc(SurfacePreparation* surfacePrep)
	: Encoder(surfacePrep)	
{
	jEncoder444 = NULL;
	jEncoder422 = NULL;
	jEncoder420 = NULL;
}

EncoderJEnc::~EncoderJEnc()
{
	SAFE_DELETE(jEncoder444);
	SAFE_DELETE(jEncoder422);
	SAFE_DELETE(jEncoder420);
}

EncodeResult EncoderJEnc::Encode(	ID3D11Texture2D* textureResource,
									CHROMA_SUBSAMPLE subsampleType,
									float outputScale,
									int jpegQuality)
{
	EncodeResult result;
	PreparedSurface ps = surfacePreparation->GetValidSurface(textureResource, outputScale);
		
	JEncD3DDataDesc jD3D;
	jD3D.Width = ps.Width;
	jD3D.Height = ps.Height;
	jD3D.ResourceView = ps.SRV;

	VerifyDestinationBuffer(ps.Width, ps.Height);

	jD3D.TargetMemory = destinationBuffer;

	JEncResult jRes;
	
		 if(subsampleType == CHROMA_SUBSAMPLE_4_4_4)	jRes = jEncoder444->Encode(jD3D, jpegQuality);
	else if(subsampleType == CHROMA_SUBSAMPLE_4_2_2)	jRes = jEncoder422->Encode(jD3D, jpegQuality);
	else if(subsampleType == CHROMA_SUBSAMPLE_4_2_0)	jRes = jEncoder420->Encode(jD3D, jpegQuality);

	result.Bits = jRes.Bits;
	result.DataSize = jRes.DataSize;
	result.HeaderSize = jRes.HeaderSize;
	result.ImageWidth = ps.Width;
	result.ImageHeight = ps.Height;

	return result;
}

HRESULT EncoderJEnc::Init(D3D11Wrap* d3d)
{
	jEncoder444 = CreateJpegEncoderInstance(GPU_ENCODER, JENC_CHROMA_SUBSAMPLE_4_4_4, d3d->GetDevice(), d3d->GetDeviceContext());
	jEncoder422 = CreateJpegEncoderInstance(GPU_ENCODER, JENC_CHROMA_SUBSAMPLE_4_2_2, d3d->GetDevice(), d3d->GetDeviceContext());
	jEncoder420 = CreateJpegEncoderInstance(GPU_ENCODER, JENC_CHROMA_SUBSAMPLE_4_2_0, d3d->GetDevice(), d3d->GetDeviceContext());

	if(jEncoder444 && jEncoder422 && jEncoder420)
		return S_OK;

	return E_FAIL;
}

/*
	DX12 class
*/

DX12_EncoderJEnc::DX12_EncoderJEnc(SurfacePreperationDX12 * surfacePrep)
	: DX12_Encoder(surfacePrep)
{
	jEncoder444 = NULL;
	jEncoder422 = NULL;
	jEncoder420 = NULL;
}

DX12_EncoderJEnc::~DX12_EncoderJEnc()
{
	SAFE_DELETE(jEncoder444);
	SAFE_DELETE(jEncoder422);
	SAFE_DELETE(jEncoder420);
}

EncodeResult DX12_EncoderJEnc::DX12_Encode(ID3D12Resource * textureResource, CHROMA_SUBSAMPLE subsampleType, float outputScale, int jpegQuality)
{
	EncodeResult result;
	DX12_PreparedSurface ps = surfacePreparation->GetValidSurface(mD3D12Wrap->GetDevice(), textureResource, outputScale);

	DX12_JEncD3DDataDesc jD3D;
	jD3D.DescriptorHeap = ps.Heap;
	jD3D.Width = ps.Width;
	jD3D.Height = ps.Height;

	VerifyDestinationBuffer(jD3D.Width, jD3D.Height);

	jD3D.TargetMemory = destinationBuffer;

	JEncResult jRes;

	if (subsampleType == CHROMA_SUBSAMPLE_4_4_4)	jRes = jEncoder444->Encode(jD3D, jpegQuality);
	else if (subsampleType == CHROMA_SUBSAMPLE_4_2_2)	jRes = jEncoder422->Encode(jD3D, jpegQuality);
	else if (subsampleType == CHROMA_SUBSAMPLE_4_2_0)	jRes = jEncoder420->Encode(jD3D, jpegQuality);

	result.Bits = jRes.Bits;
	result.DataSize = jRes.DataSize;
	result.HeaderSize = jRes.HeaderSize;
	result.ImageWidth = jD3D.Width;
	result.ImageHeight = jD3D.Height;

	return result;
}

HRESULT DX12_EncoderJEnc::Init(D3D12Wrap * d3d)
{
	mD3D12Wrap = d3d;
	jEncoder444 = DX12_CreateJpegEncoderInstance(GPU_ENCODER, JENC_CHROMA_SUBSAMPLE_4_4_4, d3d);
	jEncoder422 = DX12_CreateJpegEncoderInstance(GPU_ENCODER, JENC_CHROMA_SUBSAMPLE_4_2_2, d3d);
	jEncoder420 = DX12_CreateJpegEncoderInstance(GPU_ENCODER, JENC_CHROMA_SUBSAMPLE_4_2_0, d3d);

	if (jEncoder444 && jEncoder422 && jEncoder420)
		return S_OK;

	return E_FAIL;
}
