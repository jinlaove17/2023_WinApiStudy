#pragma once

class CTexture;
class CSound;

class CAssetManager : public CSingleton<CAssetManager>
{
	friend class CSingleton;

private:
	wchar_t							  m_assetPath[255];

	unordered_map<wstring, CTexture*> m_textures;
	unordered_map<wstring, CSound*>   m_sounds;

private:
	CAssetManager();
	~CAssetManager();

public:
	void Init();

	wstring GetAssetPath();

	CTexture* CreateTexture(const wstring& key, int width, int height);
	CTexture* LoadTexture(const wstring& fileName, const wstring& key);
	CTexture* FindTexture(const wstring& key);

	CSound* LoadSound(const wstring& fileName, const wstring& key);
	CSound* FindSound(const wstring& key);
};
