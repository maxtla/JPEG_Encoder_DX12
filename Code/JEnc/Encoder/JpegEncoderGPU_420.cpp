//--------------------------------------------------------------------------------------
// Real-Time JPEG Compression using DirectCompute - Demo
//
// Copyright (c) Stefan Petersson 2012. All rights reserved.
//--------------------------------------------------------------------------------------
#include "JpegEncoderGPU_420.h"

#include <tchar.h>

JpegEncoderGPU_420::JpegEncoderGPU_420(ID3D11Device* d3dDevice,	ID3D11DeviceContext* d3dContext)
	: JpegEncoderGPU(d3dDevice, d3dContext)
{
	mSubsampleType = JENC_CHROMA_SUBSAMPLE_4_2_0;
}

JpegEncoderGPU_420::~JpegEncoderGPU_420()
{

}

bool JpegEncoderGPU_420::Init()
{
	D3D10_SHADER_MACRO shaderDefines_Y[] = {
		{ "C_4_2_0", "1" },
		{ "COMPONENT_Y", "1" },
		{ NULL, NULL}
	};

	mShader_Y_Component = mComputeSys->CreateComputeShader(mComputeShaderFile, "420_Y", "ComputeJPEG", shaderDefines_Y);
	if(!mShader_Y_Component)
	{
		return false;
	}

	D3D10_SHADER_MACRO shaderDefines_Cb[] = {
		{ "C_4_2_0", "1" },
		{ "COMPONENT_CB", "1" },
		{ NULL, NULL}
	};

	mShader_Cb_Component = mComputeSys->CreateComputeShader(mComputeShaderFile, "420_Cb", "ComputeJPEG", shaderDefines_Cb);
	if(!mShader_Cb_Component)
	{
		return false;
	}

	D3D10_SHADER_MACRO shaderDefines_Cr[] = {
		{ "C_4_2_0", "1" },
		{ "COMPONENT_CR", "1" },
		{ NULL, NULL}
	};

	mShader_Cr_Component = mComputeSys->CreateComputeShader(mComputeShaderFile, "420_Cr", "ComputeJPEG", shaderDefines_Cr);
	if(!mShader_Cr_Component)
	{
		return false;
	}

	return true;
}

void JpegEncoderGPU_420::DoEntropyEncode()
{
	short prev_DC_Y = 0;
	short prev_DC_Cb = 0;
	short prev_DC_Cr = 0;

	int Width = mComputationWidthY;
	int Height = mComputationHeightY;

	mCB_EntropyResult->CopyToStaging();
	int* pEntropyData = mCB_EntropyResult->Map<int>();

	int iterations = mComputationWidthY / 16 * mComputationHeightY / 16;
	while(iterations-- > 0)
	{
		DoHuffmanEncoding(pEntropyData, prev_DC_Y, Y_DC_Huffman_Table);
		DoHuffmanEncoding(pEntropyData+mEntropyBlockSize, prev_DC_Y, Y_DC_Huffman_Table);
		DoHuffmanEncoding(pEntropyData+mEntropyBlockSize*2, prev_DC_Y, Y_DC_Huffman_Table);
		DoHuffmanEncoding(pEntropyData+mEntropyBlockSize*3, prev_DC_Y, Y_DC_Huffman_Table);

		DoHuffmanEncoding(pEntropyData+mEntropyBlockSize*4, prev_DC_Cb, Cb_DC_Huffman_Table);
		DoHuffmanEncoding(pEntropyData+mEntropyBlockSize*5, prev_DC_Cr, Cb_DC_Huffman_Table);

		pEntropyData += mEntropyBlockSize*6;
	}

	mCB_EntropyResult->Unmap();
}

DX12_JpegEncoderGPU_420::DX12_JpegEncoderGPU_420(D3D12Wrap* d3dWrap)
	: DX12_JpegEncoderGPU(d3dWrap)
{
	mSubsampleType = JENC_CHROMA_SUBSAMPLE_4_2_0;
}

DX12_JpegEncoderGPU_420::~DX12_JpegEncoderGPU_420()
{
}

bool DX12_JpegEncoderGPU_420::Init()
{
	D3D10_SHADER_MACRO shaderDefines_Y[] = {
		{ "C_4_2_0", "1" },
	{ "COMPONENT_Y", "1" },
	{ NULL, NULL }
	};

	mShader_Y_Component = mComputeSys->CreateComputeShader(mComputeShaderFile, "ComputeJPEG", shaderDefines_Y);
	if (!mShader_Y_Component)
	{
		return false;
	}

	D3D10_SHADER_MACRO shaderDefines_Cb[] = {
		{ "C_4_2_0", "1" },
	{ "COMPONENT_CB", "1" },
	{ NULL, NULL }
	};

	mShader_Cb_Component = mComputeSys->CreateComputeShader(mComputeShaderFile, "ComputeJPEG", shaderDefines_Cb);
	if (!mShader_Cb_Component)
	{
		return false;
	}

	D3D10_SHADER_MACRO shaderDefines_Cr[] = {
		{ "C_4_2_0", "1" },
	{ "COMPONENT_CR", "1" },
	{ NULL, NULL }
	};

	mShader_Cr_Component = mComputeSys->CreateComputeShader(mComputeShaderFile, "ComputeJPEG", shaderDefines_Cr);
	if (!mShader_Cr_Component)
	{
		return false;
	}

	// Create the PSOs
	if (FAILED(createPiplineStateObjects()))
	{
		//PostMessageBoxOnError(0, L"Failed to create PSOs: ", L"Fatal error", MB_ICONERROR, wHnd);
		exit(-1);
	}

	return true;
}

void DX12_JpegEncoderGPU_420::DoEntropyEncode()
{
	short prev_DC_Y = 0;
	short prev_DC_Cb = 0;
	short prev_DC_Cr = 0;

	int Width = mComputationWidthY;
	int Height = mComputationHeightY;

	mCB_EntropyResult->CopyToStaging();
	int* pEntropyData = mCB_EntropyResult->Map<int>();

	//for (int i = 0; i < 1800000 / 8; i++)
	//{
	//	auto lol = pEntropyData[i];
	//	auto exDE = lol;

	//	//pEntropyData[i] = 40;
	//}

	int iterations = mComputationWidthY / 16 * mComputationHeightY / 16;
	while (iterations-- > 0)
	{
		DoHuffmanEncoding(pEntropyData, prev_DC_Y, Y_DC_Huffman_Table);
		DoHuffmanEncoding(pEntropyData + mEntropyBlockSize, prev_DC_Y, Y_DC_Huffman_Table);
		DoHuffmanEncoding(pEntropyData + mEntropyBlockSize * 2, prev_DC_Y, Y_DC_Huffman_Table);
		DoHuffmanEncoding(pEntropyData + mEntropyBlockSize * 3, prev_DC_Y, Y_DC_Huffman_Table);

		DoHuffmanEncoding(pEntropyData + mEntropyBlockSize * 4, prev_DC_Cb, Cb_DC_Huffman_Table);
		DoHuffmanEncoding(pEntropyData + mEntropyBlockSize * 5, prev_DC_Cr, Cb_DC_Huffman_Table);

		pEntropyData += mEntropyBlockSize * 6;
	}

	mCB_EntropyResult->Unmap();
}
