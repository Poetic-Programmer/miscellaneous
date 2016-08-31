#include "Lighthouse.h"

Lighthouse::Lighthouse()
: 	mBodyHeight(0), mBodyBaseHeight(0), mBodyHeadHeight(0), mTopHeight(0), mTopBaseHeight(0), 
    mTopHeadHeight(0), md3dDevice(0), mVB(0), mIB(0), mStoneMapRV(0), mGlassMapRV(0)
{
	D3DXMatrixIdentity(&mWorldMatrix);
	D3DXMatrixIdentity(&mWVP);
}

Lighthouse::~Lighthouse()
{
	if( md3dDevice )
		md3dDevice->ClearState();

	ReleaseCOM(mIB);
	ReleaseCOM(mVB);
	//ReleaseCOM(mStoneMapRV);
	//ReleaseCOM(mGlassMapRV);
}

void Lighthouse::Init(ID3D10Device* device)
{
	md3dDevice = device;   

	mStoneMapRV = GetTextureMgr().createTex(L"Textures/stone.dds");
	mGlassMapRV = GetTextureMgr().createTex(L"Textures/WireFence.dds");

	BuildGeometryBuffers();

	D3DXMatrixTranslation(&mWorldMatrix, 0, 10, 50);
}

void Lighthouse::SetWorldMatrix(D3DXMATRIX world)
{
	//Set the world matrix
	mWorldMatrix = world;
}

void Lighthouse::Draw()
{	
	//md3dDevice->IASetInputLayout(InputLayout::PosNormalTex);
	float blendFactor[] = {0.0f, 0.0f, 0.0f, 0.0f};
	// set constants	
	D3DXMATRIX view = GetCamera().view();
	D3DXMATRIX proj = GetCamera().proj();
	mWVP = mWorldMatrix * view * proj;
	TextureAlpha::fxWVPVar->SetMatrix((float*)&mWVP);
	TextureAlpha::fxWorldVar->SetMatrix((float*)&mWorldMatrix);

	// Scale texture coordinates by 5 units to map [0,1]-->[0,5]
	// so that the texture repeats five times in each direction.
	D3DXMATRIX texScaleMatrix;
    D3D10_TECHNIQUE_DESC techDesc;
	TextureAlpha::Tech->GetDesc( &techDesc );
    for(UINT p = 0; p < techDesc.Passes; ++p)
    {
		ID3D10EffectPass* pass = TextureAlpha::Tech->GetPassByIndex(p);
		TextureAlpha::Tech->GetPassByIndex( p )->Apply(0);
		UINT stride = sizeof(VertexType4);
		UINT offset = 0;
		md3dDevice->OMSetBlendState(states::TransparentBS, blendFactor, 0xffffffff);
		//md3dDevice->RSSetState(states::NoCullRS);

		//DRAW THE BODY
		D3DXMatrixIdentity(&texScaleMatrix);
		D3DXMatrixScaling(&texScaleMatrix, 1.0f, 1.0f, 1.0f);
		TextureAlpha::fxDiffuseMapVar->SetResource(mStoneMapRV);
		TextureAlpha::fxTexMtxVar->SetMatrix((float*)&texScaleMatrix);
		pass->Apply(0);
		md3dDevice->IASetVertexBuffers(0, 1, &mVB, &stride, &offset);
		md3dDevice->IASetIndexBuffer(mIB, DXGI_FORMAT_R32_UINT, 0);
		md3dDevice->DrawIndexed(mBodyIndexCount, mBodyIndexOffset, mBodyVertexOffset);
	
		//DRAW BODY BASE
		D3DXMatrixIdentity(&texScaleMatrix);
		D3DXMatrixScaling(&texScaleMatrix, 0.08f, 0.08f, 1.0f);
		TextureAlpha::fxDiffuseMapVar->SetResource(mStoneMapRV);
		TextureAlpha::fxTexMtxVar->SetMatrix((float*)&texScaleMatrix);
		pass->Apply(0);

		md3dDevice->IASetVertexBuffers(0, 1, &mVB, &stride, &offset);
		md3dDevice->IASetIndexBuffer(mIB, DXGI_FORMAT_R32_UINT, 0);
		md3dDevice->DrawIndexed(mBodyBaseIndexCount, mBodyBaseIndexOffset, mBodyBaseVertexOffset);

		//DRAW TOP BASE
		D3DXMatrixIdentity(&texScaleMatrix);
		D3DXMatrixScaling(&texScaleMatrix, 5.0f, 5.0f, 1.0f);
		TextureAlpha::fxDiffuseMapVar->SetResource(mStoneMapRV);
		TextureAlpha::fxTexMtxVar->SetMatrix((float*)&texScaleMatrix);
		pass->Apply(0);

		md3dDevice->IASetVertexBuffers(0, 1, &mVB, &stride, &offset);
		md3dDevice->IASetIndexBuffer(mIB, DXGI_FORMAT_R32_UINT, 0);
		md3dDevice->DrawIndexed(mTopBaseIndexCount, mTopBaseIndexOffset, mTopBaseVertexOffset);

		//DRAW TOP HEAD
		D3DXMatrixIdentity(&texScaleMatrix);
		D3DXMatrixScaling(&texScaleMatrix, 5.0f, 5.0f, 1.0f);
		TextureAlpha::fxDiffuseMapVar->SetResource(mStoneMapRV);	
		TextureAlpha::fxTexMtxVar->SetMatrix((float*)&texScaleMatrix);
		pass->Apply(0);
		md3dDevice->IASetVertexBuffers(0, 1, &mVB, &stride, &offset);
		md3dDevice->IASetIndexBuffer(mIB, DXGI_FORMAT_R32_UINT, 0);
		md3dDevice->DrawIndexed(mTopHeadIndexCount, mTopHeadIndexOffset, mTopHeadVertexOffset);

		//DRAW THE TOP
		D3DXMatrixIdentity(&texScaleMatrix);
		D3DXMatrixScaling(&texScaleMatrix, 5.0f, 5.0f, 1.0f);
		TextureAlpha::fxDiffuseMapVar->SetResource(mGlassMapRV);
		TextureAlpha::fxTexMtxVar->SetMatrix((float*)&texScaleMatrix);		
		md3dDevice->IASetVertexBuffers(0, 1, &mVB, &stride, &offset);
		md3dDevice->IASetIndexBuffer(mIB, DXGI_FORMAT_R32_UINT, 0);	
		pass->Apply(0);
		md3dDevice->DrawIndexed(mTopIndexCount, mTopIndexOffset, mTopVertexOffset);
		md3dDevice->OMSetBlendState(0, blendFactor, 0xffffffff);
		//md3dDevice->RSSetState(0);
	}
}

void Lighthouse::BuildGeometryBuffers()
{
	mBodyHeight = 20.0f;
	mBodyBaseHeight = 2.0f;
	mTopHeight = 4.0f;
	mTopBaseHeight = 2.0f;
	mTopHeadHeight = 2.0f;

	Geometry geo;

	geo.CreateCylinder(5.0f, 4.0f, mBodyHeight,     16, 16, mBody);
	geo.CreateCylinder(8.0f, 8.0f, mBodyBaseHeight, 8,  4, mBodyBase);

	geo.CreateCylinder(4.0f, 5.0f,   mTopHeight,     16,  1, mTop);
	geo.CreateCylinder(5.0f, 5.0f,   mTopBaseHeight,  8,  1, mTopBase);
	geo.CreateCylinder(6.0f, 6.0f,   mTopHeadHeight,  8,  1, mTopHead);

	// Cache the vertex offsets to each object in the concatenated vertex buffer.
	mBodyVertexOffset          = 0;
	mBodyBaseVertexOffset      = mBody.Vertices.size();
	mTopVertexOffset           = mBodyBaseVertexOffset + mBodyBase.Vertices.size();
	mTopBaseVertexOffset       = mTopVertexOffset      + mTop.Vertices.size();
	mTopHeadVertexOffset       = mTopBaseVertexOffset  + mTopBase.Vertices.size();

	// Cache the index count of each object.
	mBodyIndexCount         = mBody.Indices.size();
	mBodyBaseIndexCount     = mBodyBase.Indices.size();
	mTopIndexCount          = mTop.Indices.size();
	mTopBaseIndexCount      = mTopBase.Indices.size();
	mTopHeadIndexCount      = mTopHead.Indices.size();

	// Cache the vertex offsets to each object in the concatenated vertex buffer.
	mBodyIndexOffset         = 0;
	mBodyBaseIndexOffset     = mBodyIndexCount;
	mTopIndexOffset          = mBodyBaseIndexCount     + mBodyBaseIndexOffset;
	mTopBaseIndexOffset      = mTopIndexCount         + mTopIndexOffset;
	mTopHeadIndexOffset      = mTopBaseIndexCount     + mTopBaseIndexOffset;

	// Allign the components
	for(UINT i = 0; i < mBodyBase.Vertices.size(); ++i)
	{
		mBodyBase.Vertices[i].Position.y -= (mBodyHeight * 0.5f) + (mBodyBaseHeight * 0.5f);
	}

	for(UINT i = 0; i < mTopBase.Vertices.size(); ++i)
	{
		mTopBase.Vertices[i].Position.y += (mBodyBaseHeight * 0.5f) + (mBodyHeight * 0.5f) - 
			                            (mTopBaseHeight * 0.5f);
	}
	for(UINT i = 0; i < mTop.Vertices.size(); ++i)
	{
		mTop.Vertices[i].Position.y += (mBodyBaseHeight * 0.5f) + (mBodyHeight * 0.5f) + 
			(mTopHeight * 0.5f);
	}
	for(UINT i = 0; i < mTopHead.Vertices.size(); ++i)
	{
		mTopHead.Vertices[i].Position.y += (mBodyBaseHeight * 0.5f) + (mBodyHeight * 0.5f) + 
			(mTopHeight * 0.5f) +	(mTopHeadHeight) + (mTopBaseHeight * 0.5f);
	}

	UINT totalVertexCount = 
		mBody.Vertices.size()     + 
		mBodyBase.Vertices.size() + 
		mTop.Vertices.size()      + 
		mTopBase.Vertices.size()  + 
		mTopHead.Vertices.size();

	UINT totalIndexCount = 
		mBodyIndexCount     + 
		mBodyBaseIndexCount + 
		mTopIndexCount      + 
		mTopBaseIndexCount  + 
		mTopHeadIndexCount;
//
	// Extract the vertex elements we are interested in and pack the
	// vertices of all the meshes into one vertex buffer.
	//

	std::vector<VertexType4> vertices(totalVertexCount);
	//std::vector<VertexType3> vertices(totalVertexCount);

	D3DXVECTOR4 black(0.0f, 0.0f, 0.0f, 1.0f);
	D3DXVECTOR4 white(1.0f, 1.0f, 1.0f, 1.0f);

	UINT k = 0;
	for(size_t i = 0; i < mBody.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos    = mBody.Vertices[i].Position;
		vertices[k].Normal = mBody.Vertices[i].Normal;
		vertices[k].TexC   = mBody.Vertices[i].TexC;
	}
	for(size_t i = 0; i < mBodyBase.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos    = mBodyBase.Vertices[i].Position;
		vertices[k].Normal = mBodyBase.Vertices[i].Normal;
		vertices[k].TexC   = mBodyBase.Vertices[i].TexC;
	}
	for(size_t i = 0; i < mTop.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos    = mTop.Vertices[i].Position;
		vertices[k].Normal = mTop.Vertices[i].Normal;
		vertices[k].TexC   = mTop.Vertices[i].TexC;
	}
	for(size_t i = 0; i < mTopBase.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos    = mTopBase.Vertices[i].Position;
		vertices[k].Normal = mTopBase.Vertices[i].Normal;
		vertices[k].TexC   = mTopBase.Vertices[i].TexC;
	}
	for(size_t i = 0; i < mTopHead.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos    = mTopHead.Vertices[i].Position;
		vertices[k].Normal = mTopHead.Vertices[i].Normal;
		vertices[k].TexC   = mTopHead.Vertices[i].TexC;
	}

    //D3D10_BUFFER_DESC vbd;
    //vbd.Usage = D3D10_USAGE_DYNAMIC;
    //vbd.ByteWidth = sizeof(VertexType4) * mNumVertices;
    //vbd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
    //vbd.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
   // vbd.MiscFlags = 0;
   // HR(md3dDevice->CreateBuffer(&vbd, 0, &mVB));

    D3D10_BUFFER_DESC vbd;
    vbd.Usage = D3D10_USAGE_DYNAMIC;
    vbd.ByteWidth = sizeof(VertexType4) * totalVertexCount;
    vbd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
    vbd.MiscFlags = 0;
    D3D10_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = &vertices[0];
    HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mVB));

	//
	// Pack the indices of all the meshes into one index buffer.
	//

	std::vector<UINT> indices;
	indices.insert(indices.end(), mBody.Indices.begin(),     mBody.Indices.end());
	indices.insert(indices.end(), mBodyBase.Indices.begin(), mBodyBase.Indices.end());
	indices.insert(indices.end(), mTop.Indices.begin(),      mTop.Indices.end());
	indices.insert(indices.end(), mTopBase.Indices.begin(),  mTopBase.Indices.end());
	indices.insert(indices.end(), mTopHead.Indices.begin(),  mTopHead.Indices.end());

	D3D10_BUFFER_DESC ibd;
    ibd.Usage = D3D10_USAGE_IMMUTABLE;
    ibd.ByteWidth = sizeof(UINT) * totalIndexCount;
    ibd.BindFlags = D3D10_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    D3D10_SUBRESOURCE_DATA iinitData;
    iinitData.pSysMem = &indices[0];
    HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mIB));
}