//--------------------------------------------------------------------------------------
// Real-Time JPEG Compression using DirectCompute - Demo
//
// Copyright (c) Stefan Petersson 2012. All rights reserved.
//--------------------------------------------------------------------------------------
#pragma once

#include "JpegEncoderGPU.h"

#include "../../Shared/ComputeShader.h"
#include "../../Demo/D3DWrap/D3DWrap.h"

class JpegEncoderGPU_444 : public JpegEncoderGPU
{
public:
	JpegEncoderGPU_444(ID3D11Device* d3dDevice,	ID3D11DeviceContext* d3dContext);
	virtual ~JpegEncoderGPU_444();

	virtual bool Init();

private:
	virtual void DoEntropyEncode();
};

class DX12_JpegEncoderGPU_444 : public DX12_JpegEncoderGPU
{
public:
	DX12_JpegEncoderGPU_444(D3D12Wrap* d3dWrap);
	virtual ~DX12_JpegEncoderGPU_444();

	virtual bool Init();

private:
	virtual void DoEntropyEncode();
};