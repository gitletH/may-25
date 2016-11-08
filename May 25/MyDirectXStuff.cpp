#include "stdafx.h"
#include "MyDirectXStuff.h"
#include "Geometry.h"

XMFLOAT3				mEyePosW;
float					LeftBound;
float					RightBound;
UINT					NCount;
SolidMethod				g_SolidMethod;
BoundToWhat				g_BoundToWhat;

void BuildGeometryBuffers(GeometryPointers mGeoPointers, ID3D11Buffer*& pVertexBuffer, ID3D11Buffer*& pIndexBuffer,
	UINT NumOfVertex, UINT NumOfIndice)
{
	// Release old geometry buffers
	ReleaseCom(pVertexBuffer);
	ReleaseCom(pIndexBuffer);

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SimpleVertex) * NumOfVertex;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = mGeoPointers.pVertexPointer;
	ThrowIfFailed(g_pd3dDevice->CreateBuffer(&bd, &InitData, &pVertexBuffer));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = NumOfIndice * sizeof(WORD);        // 36 vertices needed for 12 triangles in a triangle list
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	InitData.pSysMem = mGeoPointers.pIndexPointer;
	ThrowIfFailed(g_pd3dDevice->CreateBuffer(&bd, &InitData, &pIndexBuffer));

	delete[] mGeoPointers.pVertexPointer;
	mGeoPointers.pVertexPointer = nullptr;
	delete[] mGeoPointers.pIndexPointer;
	mGeoPointers.pIndexPointer = nullptr;
	return;
}

void BuildAxisGeometryBuffers(ID3D11Buffer*& pVertexBuffer, ID3D11Buffer*& pIndexBuffer, UINT& NumOfIndices_Cylinder, UINT& NumOfIndices_Cone)
{
	UINT NumOfVertices_Cone = 0;
	NumOfIndices_Cone = 0;
	UINT NumOfVertices_Cylinder = 0;
	NumOfIndices_Cylinder = 0;

	GeometryPointers CylinderGeometry = BuildDiskGeometryBuffers(&NumOfVertices_Cylinder, &NumOfIndices_Cylinder);
	GeometryPointers ConeGeometry = BuildConeGeometryBuffers(&NumOfVertices_Cone, &NumOfIndices_Cone);
	GeometryPointers AxisGeometry;
	AxisGeometry.pVertexPointer = new SimpleVertex[NumOfVertices_Cone + NumOfVertices_Cylinder];
	AxisGeometry.pIndexPointer = new WORD[NumOfIndices_Cone + NumOfIndices_Cylinder];

	// Fill out vertices array
	memcpy(AxisGeometry.pVertexPointer, CylinderGeometry.pVertexPointer, NumOfVertices_Cylinder * sizeof(SimpleVertex));
	memcpy(AxisGeometry.pVertexPointer + NumOfVertices_Cylinder, ConeGeometry.pVertexPointer, NumOfVertices_Cone * sizeof(SimpleVertex));
	// Fill out indices array
	memcpy(AxisGeometry.pIndexPointer, CylinderGeometry.pIndexPointer, NumOfIndices_Cylinder * sizeof(WORD));
	memcpy(AxisGeometry.pIndexPointer + NumOfIndices_Cylinder, ConeGeometry.pIndexPointer, NumOfIndices_Cone * sizeof(WORD));

	UINT NumOfVertices = NumOfVertices_Cone + NumOfVertices_Cylinder;
	UINT NumOfIndices = NumOfIndices_Cone + NumOfIndices_Cylinder;

	BuildGeometryBuffers(AxisGeometry, pVertexBuffer, pIndexBuffer, NumOfVertices, NumOfIndices);
}

void DrawSolids(RenderObject mRenderObject)
{
	// Draw solid
	switch (g_SolidMethod)
	{
	case Disk:
	case CrossSection_Semicircle:
	case CrossSection_EquilateralTriangle:
	case CrossSection_Square:
	{
		// Draw objects
		float dx = (RightBound - LeftBound) / (float)NCount;
		for (UINT i = 0; i < NCount; i++)
		{
			float x = LeftBound + i * dx;
			float y = evaluate(Expression_1, x);

			// Get world matrix
			//XMMATRIX mSpin = XMMatrixRotationZ(-t * 2.0f);
			//XMMATRIX mOrbit = XMMatrixRotationY(t);
			XMMATRIX mTranslate = XMMatrixTranslation(x, 0.0f, 0.0f);
			XMMATRIX mScale = XMMatrixScaling(dx, y, y);

			g_World = mScale * mTranslate;

			//
			// Update variables for the object
			//
			switch (mRenderObject)
			{
			case Shadowmap:
			{
				ConstantBuffer_Shadowmap cb;
				cb.mWorldViewProj = XMMatrixTranspose(g_World * g_LightView * g_Projection);

				g_pImmediateContext->UpdateSubresource(g_pConstantBuffer_Shadowmap, 0, nullptr, &cb, 0, 0);
				g_pImmediateContext->VSSetShader(g_pShadowmapVertexShader, nullptr, 0);
				g_pImmediateContext->VSSetConstantBuffers(0, 1, &g_pConstantBuffer_Shadowmap);
				g_pImmediateContext->PSSetShader(g_pShadowmapPixelShader, nullptr, 0);
				break;
			}

			case Scene:
			{
				ConstantBuffer cb;
				cb.mWorld = XMMatrixTranspose(g_World);
				cb.mWorldLightviewProj = XMMatrixTranspose(g_World * g_LightView * g_Projection);
				cb.mWorldViewProj = XMMatrixTranspose(g_World * g_View * g_Projection);
				cb.mWorldInvTranspose = InverseTranspose(g_World);
				cb.mEyePosW = mEyePosW;
				cb.ColorSwitch = i % 3;

				g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, nullptr, &cb, 0, 0);
				g_pImmediateContext->VSSetShader(g_pVertexShader, nullptr, 0);
				g_pImmediateContext->VSSetConstantBuffers(0, 1, &g_pConstantBuffer);
				g_pImmediateContext->PSSetShader(g_pPixelShader, nullptr, 0);
				break;
			}
			default:
				break;
			}

			g_pImmediateContext->DrawIndexed(NumOfIndices_Solid, 0, 0);

		}
	}
	break;
	case Washer:
	case Shell:
	{

		// Get world matrix
		//XMMATRIX mSpin = XMMatrixRotationZ(-t * 2.0f);
		//XMMATRIX mOrbit = XMMatrixRotationY(t);
		XMMATRIX mTranslate = XMMatrixTranslation(0.0f, 0.0f, 0.0f);
		XMMATRIX mScale = XMMatrixScaling(1.0f, 1.0f, 1.0f);

		g_World = mScale * mTranslate;

		//
		// Update variables for the object
		//
		ConstantBuffer cb;
		cb.mWorldLightviewProj = XMMatrixTranspose(g_World * g_LightView * g_Projection);
		cb.mWorldViewProj = XMMatrixTranspose(g_World * g_View * g_Projection);
		cb.mWorldInvTranspose = InverseTranspose(g_World);
		cb.mEyePosW = mEyePosW;
		cb.ColorSwitch = 999;
		g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, nullptr, &cb, 0, 0);

		//
		// Render the object
		//
		g_pImmediateContext->VSSetShader(g_pVertexShader, nullptr, 0);
		g_pImmediateContext->VSSetConstantBuffers(0, 1, &g_pConstantBuffer);
		g_pImmediateContext->PSSetShader(g_pPixelShader, nullptr, 0);
		g_pImmediateContext->DrawIndexed(NumOfIndices_Solid, 0, 0);
	}
	break;
	default:
		break;
	}
}

void DrawAxis()
{
	UINT offset = 0;
	g_pImmediateContext->VSSetShader(g_pVertexShader, nullptr, 0);
	g_pImmediateContext->PSSetShader(g_pPixelShader, nullptr, 0);
	g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pAxesVertexBuffer, &stride, &offset);
	g_pImmediateContext->IASetIndexBuffer(g_pAxesIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	const float SemiLengthOfStick = 10;
	const float r_stick = 0.005f * mRadius;
	// Draw negative part of x-axis
	XMMATRIX mTranslate = XMMatrixTranslation(-SemiLengthOfStick, 0.0f, 0.0f);
	XMMATRIX mScale = XMMatrixScaling(SemiLengthOfStick, r_stick, r_stick);
	g_World = mScale * mTranslate;

	ConstantBuffer cb;
	cb.mWorld = XMMatrixTranspose(g_World);
	cb.mWorldLightviewProj = XMMatrixTranspose(g_World * g_LightView * g_Projection);
	cb.mWorldViewProj = XMMatrixTranspose(g_World * g_View * g_Projection);
	cb.mWorldInvTranspose = InverseTranspose(g_World);
	cb.mEyePosW = mEyePosW;
	cb.ColorSwitch = 999;

	g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, nullptr, &cb, 0, 0);
	g_pImmediateContext->VSSetConstantBuffers(0, 1, &g_pConstantBuffer);
	g_pImmediateContext->DrawIndexed(NumOfIndices_Cylinder, 0, 0);

	// Draw positive part of x-axis
	mTranslate = XMMatrixTranslation(0.0f, 0.0f, 0.0f);
	mScale = XMMatrixScaling(SemiLengthOfStick, r_stick, r_stick);
	g_World = mScale * mTranslate;

	ZeroMemory(&cb, sizeof(cb));
	cb.mWorld = XMMatrixTranspose(g_World);
	cb.mWorldLightviewProj = XMMatrixTranspose(g_World * g_LightView * g_Projection);
	cb.mWorldViewProj = XMMatrixTranspose(g_World * g_View * g_Projection);
	cb.mWorldInvTranspose = InverseTranspose(g_World);
	cb.mEyePosW = mEyePosW;
	cb.ColorSwitch = 999;

	g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, nullptr, &cb, 0, 0);
	g_pImmediateContext->VSSetConstantBuffers(0, 1, &g_pConstantBuffer);
	g_pImmediateContext->DrawIndexed(NumOfIndices_Cylinder, 0, 0);
}
