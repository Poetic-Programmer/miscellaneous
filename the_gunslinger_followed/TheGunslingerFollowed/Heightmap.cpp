#include "Heightmap.h"

Heightmap::Heightmap()
: 	
	m_width(0),
	m_height(0),
	m_scale(0),
	m_heightmapSRV(0), 
	m_texture(0)
{

}

Heightmap::Heightmap(UINT width, UINT height, float scale)
: 	
	m_width(width),
	m_height(height),
	m_scale(scale),
	m_heightmapSRV(0), 
	m_texture(0)
{

}

Heightmap::~Heightmap()
{
	ReleaseCOM(m_heightmapSRV);
	ReleaseCOM(m_texture);
}

Heightmap& Heightmap::operator=(const Heightmap& other)
{
	if(this == &other)
		return *this;

	this->m_heightValues = other.m_heightValues;
	this->m_noise        = other.m_noise;
	this->m_width        = other.m_width;
	this->m_height       = other.m_height;
	this->m_scale        = other.m_scale;
	this->m_heightmapSRV = other.m_heightmapSRV;
	this->m_texture      = other.m_texture;

	return *this;
}

UINT Heightmap::GetWidth()const
{
	return m_width;
}

UINT Heightmap::GetHeight()const
{
	return m_height;
}

float Heightmap::GetIndexValue(int index)const
{
	return m_heightValues[index];
}

std::vector<float> Heightmap::GetHeightValues()
{
	return m_heightValues;
}

float Heightmap::GetHeightValue(int row, int col)const
{
	return m_heightValues[row * m_width + col];
}

void Heightmap::CreateHeightmapFromFile(std::wstring filename)
{
	// A height for each vertex
	std::vector<unsigned char> in(m_width * m_height);

	// Open the file.
	std::ifstream inFile;
	inFile.open(filename.c_str(), std::ios_base::binary);

	if(inFile)
	{
		// Read the RAW bytes.
		inFile.read((char*)&in[0], (std::streamsize)in.size());

		// Done with file.
		inFile.close();
	}

	// Copy the array data into a float array and scale it.
	m_heightValues.resize(m_height * m_width, 0);
	for(UINT i = 0; i < m_height * m_width; ++i)
	{
		m_heightValues[i] = (in[i] / 255.0f) * m_scale;
	}

	// Create smooth height values
	Smooth();
}

void Heightmap::CreateHeightmapFromPerlinNoise()
{
	m_noise.Initialize();

	m_heightValues.resize(m_height * m_width);
	for(UINT i = 0; i < m_height; ++i)
	{
		for(UINT j = 0; j < m_width; ++j)
		{
			//m_heightValues[i * m_width + j] = m_noise.RawNoise2D((float)i, (float)j, 6) * m_scale;
			m_heightValues[i * m_width + j] = m_noise.PerlinNoise2D((float)i, (float)j, 6) * m_scale;
		}
	}
	Smooth();
}

void Heightmap::Initialize(ID3D11Device* device, ID3D11DeviceContext* dc)
{

}

void Heightmap::Update(float deltaTime)
{

}

void Heightmap::CreateShaderResourceView(ID3D11Device* device, ID3D11DeviceContext* dc)
{
	static int counter = 0;

	D3D11_TEXTURE2D_DESC fBmDesc;
	D3D11_SUBRESOURCE_DATA fBmInitData;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;

	fBmDesc.Width              = m_width;
	fBmDesc.Height             = m_height;
	fBmDesc.MipLevels          = 1;
	fBmDesc.ArraySize          = 1;
	fBmDesc.Format             = DXGI_FORMAT_R16_FLOAT;
	fBmDesc.SampleDesc.Count   = 1;
	fBmDesc.SampleDesc.Quality = 0;
	fBmDesc.Usage              = D3D11_USAGE_DEFAULT;
	fBmDesc.BindFlags          = D3D11_BIND_SHADER_RESOURCE;
    fBmDesc.CPUAccessFlags     = 0;
	fBmDesc.MiscFlags          = 0;

	std::vector<HALF> hmap(m_heightValues.size());
	std::transform(m_heightValues.begin(), m_heightValues.end(), hmap.begin(), XMConvertFloatToHalf);

	fBmInitData.pSysMem = &hmap[0];
	fBmInitData.SysMemPitch = m_width * sizeof(HALF);
	fBmInitData.SysMemSlicePitch = 0;

	HR(device->CreateTexture2D(&fBmDesc, &fBmInitData, &m_texture));

	if(counter == 0)
		D3DX11SaveTextureToFile(dc, m_texture, D3DX11_IFF_DDS, L"heightmap.dds");
	else
		D3DX11SaveTextureToFile(dc, m_texture, D3DX11_IFF_DDS, L"noise.dds");

	counter++;

	srvDesc.Format = fBmDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;
	HR(device->CreateShaderResourceView(m_texture, &srvDesc, &m_heightmapSRV));
	//fBm.clear();
}

ID3D11ShaderResourceView* Heightmap::GetShaderResourceView()
{
	return m_heightmapSRV;
}

void Heightmap::Smooth()
{
	std::vector<float> dest(m_heightValues.size());

	for(UINT i = 0; i < m_height; ++i)
	{
		for(UINT j = 0; j < m_width; ++j)
		{
			dest[i * m_width + j] = Average(i,j);
		}
	}

	// Replace the old heightmap with the filtered one.
	m_heightValues = dest;
}

bool Heightmap::InBounds(int i, int j)
{
	// True if ij are valid indices; false otherwise.
	return 
		i >= 0 && i < (int)m_height && 
		j >= 0 && j < (int)m_width;
}

float Heightmap::Average(int i, int j)
{
	float avg = 0.0f;
	float num = 0.0f;

	// Use int to allow negatives.  If we use UINT, @ i=0, m=i-1=UINT_MAX
	// and no iterations of the outer for loop occur.
	for(int m = i-1; m <= i+1; ++m)
	{
		for(int n = j - 1; n <= j + 1; ++n)
		{
			if(InBounds(m,n))
			{
				avg += m_heightValues[m * m_width + n];
				num += 1.0f;
			}
		}
	}

	return avg / num;
}