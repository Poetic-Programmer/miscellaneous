#include "AncientRuin.h"

AncientRuin::AncientRuin()
: md3dDevice(0), mVB(0), mIB(0), mFloorMapRV(0), mColumnMapRV(0), mSpecMapRV(0)
{
	D3DXMatrixIdentity(&mFloorWorldMatrix);

	for(int i = 0; i < NumColumns; ++i)
	{
		D3DXMatrixIdentity(&mColumnsWorldMatrix[i]);
	}
}

AncientRuin::~AncientRuin()
{
	if( md3dDevice )
		md3dDevice->ClearState();

	ReleaseCOM(mIB);
	ReleaseCOM(mVB);

	//ReleaseCOM(mFloorMapRV);
	//ReleaseCOM(mColumnMapRV);
	//ReleaseCOM(mSpecMapRV);
}

void AncientRuin::Initialize(ID3D10Device* device)
{
	md3dDevice = device;

	float angle = (float)360 / NumColumns;
	float radius = 10;

	for(int i = 0; i < NumColumns; ++i)
	{	
		D3DXMatrixTranslation(&mColumnsWorldMatrix[i], radius * cosf(angle * i), 2, radius * sinf(angle * i));
	}


	mFloorMapRV  = GetTextureMgr().createTex(L"Textures/bricks.dds");
	mColumnMapRV = GetTextureMgr().createTex(L"Textures/stone.dds");
	mSpecMapRV   = GetTextureMgr().createTex(L"Textures/default_spec.dds");

	BuildGeometryBuffers();
}

void AncientRuin::Draw()
{
	md3dDevice->IASetInputLayout(InputLayout::PosNormalTex);

	D3DXMATRIX view = GetCamera().view();
	D3DXMATRIX proj = GetCamera().proj();
	mWVP = mFloorWorldMatrix * view * proj;
	TextureAlpha::fxWVPVar->SetMatrix((float*)&mWVP);
	TextureAlpha::fxWorldVar->SetMatrix((float*)&mFloorWorldMatrix);

	D3DXMATRIX texScaleMatrix;
	D3DXMatrixIdentity(&texScaleMatrix);
	D3DXMatrixScaling(&texScaleMatrix, 1.0f, 1.0f, 1.0f);

    D3D10_TECHNIQUE_DESC techDesc;
	TextureAlpha::Tech->GetDesc( &techDesc );

	UINT stride = sizeof(VertexType4);
	UINT offset = 0;

    for(UINT i = 0; i < techDesc.Passes; ++i)
    {
		ID3D10EffectPass* pass = TextureAlpha::Tech->GetPassByIndex(i);
		//
		// draw floor
		//
		D3DXMATRIX floorWVP = mFloorWorldMatrix*view*proj;
		TextureAlpha::fxWVPVar->SetMatrix((float*)&floorWVP);
		TextureAlpha::fxWorldVar->SetMatrix((float*)&mFloorWorldMatrix);
		TextureAlpha::fxTexMtxVar->SetMatrix((float*)&texScaleMatrix);
		TextureAlpha::fxDiffuseMapVar->SetResource(mFloorMapRV);
		TextureAlpha::fxSpecMapVar->SetResource(mSpecMapRV);
		pass->Apply(0);

		md3dDevice->IASetVertexBuffers(0, 1, &mVB, &stride, &offset);
		md3dDevice->IASetIndexBuffer(mIB, DXGI_FORMAT_R32_UINT, 0);
		md3dDevice->DrawIndexed(mFloorIndexCount, mFloorIndexOffset, mFloorVertexOffset);
		//
		// draw columns
		//
		for(int i = 0; i < NumColumns; ++i)
		{
			D3DXMATRIX columnWVP = mColumnsWorldMatrix[i]*view*proj;	
			TextureAlpha::fxWorldVar->SetMatrix((float*)&mColumnsWorldMatrix[i]);
			TextureAlpha::fxWVPVar->SetMatrix((float*)&columnWVP);
			TextureAlpha::fxTexMtxVar->SetMatrix((float*)&texScaleMatrix);
			TextureAlpha::fxDiffuseMapVar->SetResource(mColumnMapRV);
			TextureAlpha::fxSpecMapVar->SetResource(mSpecMapRV);
			pass->Apply(0);
			md3dDevice->IASetVertexBuffers(0, 1, &mVB, &stride, &offset);
			md3dDevice->IASetIndexBuffer(mIB, DXGI_FORMAT_R32_UINT, 0);
			md3dDevice->DrawIndexed(mColumnsIndexCount, mColumnsIndexOffset, mColumnsVertexOffset);
		}
    }
}

void AncientRuin::BuildGeometryBuffers()
{
	Geometry geo;


	geo.CreateCylinder(1.0f, 1.0f, 20, 8, 8, mColumn);
	geo.CreateCylinder(15.0f, 15.0f, 5, 16, 8, mFloor);
	geo.CreateCylinder(4.0f, 4.0f, 1, 8,  4, mPath);

	// Cache the vertex offsets to each object in the concatenated vertex buffer.
	mColumnsVertexOffset        = 0;
	mFloorVertexOffset          = mColumn.Vertices.size();
	mPathVertexOffset           = mFloorVertexOffset + mFloor.Vertices.size();

	// Cache the index count of each object.
	mColumnsIndexCount          = mColumn.Indices.size();
	mFloorIndexCount            = mFloor.Indices.size();
	mPathIndexCount             = mPath.Indices.size();

	// Cache the vertex offsets to each object in the concatenated vertex buffer.
	mColumnsIndexOffset         = 0;
	mFloorIndexOffset           = mColumnsIndexCount;
	mPathIndexOffset            = mFloorIndexCount + mFloorIndexOffset;

	UINT totalVertexCount = 
		mColumn.Vertices.size()     + 
		mFloor.Vertices.size() + 
		mPath.Vertices.size();

	UINT totalIndexCount = 
		mColumnsIndexCount     + 
		mFloorIndexCount + 
		mPathIndexCount;
//
	// Extract the vertex elements we are interested in and pack the
	// vertices of all the meshes into one vertex buffer.
	//

	std::vector<VertexType4> vertices(totalVertexCount);

	UINT k = 0;
	for(size_t i = 0; i < mColumn.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos    = mColumn.Vertices[i].Position;
		vertices[k].Normal = mColumn.Vertices[i].Normal;
		vertices[k].TexC   = mColumn.Vertices[i].TexC;
	}
	for(size_t i = 0; i < mFloor.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos    = mFloor.Vertices[i].Position;
		vertices[k].Normal = mFloor.Vertices[i].Normal;
		vertices[k].TexC   = mFloor.Vertices[i].TexC;
	}
	for(size_t i = 0; i < mPath.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos    = mPath.Vertices[i].Position;
		vertices[k].Normal = mPath.Vertices[i].Normal;
		vertices[k].TexC   = mPath.Vertices[i].TexC;
	}

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
	indices.insert(indices.end(), mColumn.Indices.begin(), mColumn.Indices.end());
	indices.insert(indices.end(), mFloor.Indices.begin(),  mFloor.Indices.end());
	indices.insert(indices.end(), mPath.Indices.begin(),   mPath.Indices.end());

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