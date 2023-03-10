#include "pch.h"
#include "Animation.h"

#include "TimeManager.h"
#include "AssetManager.h"
#include "Camera.h"

#include "Object.h"

#include "Animator.h"

#include "Texture.h"

CAnimation::CAnimation() :
	m_name(),
	m_isFinished(),
	m_frames(),
	m_frameIndex(),
	m_accTime(),
	m_animator(),
	m_texture()
{
}

CAnimation::CAnimation(const wstring& name, CAnimator* animator, CTexture* texture, const Vec2& count, int startIndex, int frameCount, float duration) :
	m_name(name),
	m_isFinished(),
	m_frames(),
	m_frameIndex(),
	m_accTime(),
	m_animator(animator),
	m_texture(texture)
{
	assert((m_animator != nullptr) && (m_texture != nullptr));

	Vec2 sliceSize(m_texture->GetWidth() / count.m_x, m_texture->GetHeight() / count.m_y);
	
	m_frames.reserve(frameCount);

	for (int i = 0; i < frameCount; ++i)
	{
		AnimationFrame frame = {};
		int frameIndex = startIndex + i;

		frame.m_leftTop.m_x = sliceSize.m_x * (frameIndex % (int)count.m_x);
		frame.m_leftTop.m_y = sliceSize.m_y * (frameIndex / (int)count.m_x);
		frame.m_sliceSize = sliceSize;
		frame.m_duration = duration;

		m_frames.push_back(frame);
	}
}

CAnimation::CAnimation(const CAnimation& rhs) :
	m_name(rhs.m_name),
	m_isFinished(rhs.m_isFinished),
	m_frames(rhs.m_frames),
	m_frameIndex(),
	m_accTime(),
	m_animator(),
	m_texture(rhs.m_texture)
{
}

CAnimation::~CAnimation()
{
}

void CAnimation::SetName(const wstring& name)
{
	m_name = name;
}

const wstring& CAnimation::GetName()
{
	return m_name;
}

bool CAnimation::IsFinished()
{
	return m_isFinished;
}

void CAnimation::SetFrame(int frameIndex)
{
	assert(frameIndex >= 0);

	m_isFinished = false;
	m_frameIndex = frameIndex;
	m_accTime = 0.0f;
}

void CAnimation::SaveData(const wstring& fileName)
{
	wstring filePath = CAssetManager::GetInstance()->GetAssetPath();

	filePath += L"Animation\\" + fileName;

	FILE* file = nullptr;

	_wfopen_s(&file, filePath.c_str(), L"wb");

	assert(file != nullptr);

	// 애니메이션의 이름 쓰기
	WriteString(file, m_name);

	// 애니메이션 정보 쓰기
	size_t frameCount = m_frames.size();

	fwrite(&frameCount, sizeof(size_t), 1, file);
	fwrite(m_frames.data(), sizeof(AnimationFrame), frameCount, file);
	
	// 텍스처 정보 쓰기
	WriteString(file, m_texture->GetFilePath());
	WriteString(file, m_texture->GetName());

	fclose(file);
}

void CAnimation::LoadData(const wstring& fileName)
{
	wstring filePath = CAssetManager::GetInstance()->GetAssetPath();

	filePath += L"Animation\\" + fileName;

	FILE* file = nullptr;

	_wfopen_s(&file, filePath.c_str(), L"rb");

	assert(file != nullptr);

	// 애니메이션의 이름 읽기
	ReadString(file, m_name);

	// 애니메이션의 프레임 읽기
	size_t frameCount = m_frames.size();

	fread(&frameCount, sizeof(size_t), 1, file);

	m_frames.resize(frameCount);
	fread(m_frames.data(), sizeof(AnimationFrame), frameCount, file);

	// 텍스처 정보 읽기
	wstring texturePath, textureName;

	ReadString(file, texturePath);
	ReadString(file, textureName);
	m_texture = CAssetManager::GetInstance()->LoadTexture(texturePath, textureName);

	fclose(file);
}

void CAnimation::Update()
{
	if (m_isFinished)
	{
		return;
	}

	m_accTime += DT;

	if (m_accTime > m_frames[m_frameIndex].m_duration)
	{
		// 축적된 시간이 애니메이션의 한 프레임 지속시간을 넘어서는 경우를 대비하여 0.0f으로 만드는 것이 아니라, 두 값의 차이로 설정한다.
		m_accTime = m_accTime - m_frames[m_frameIndex++].m_duration;

		if (m_frameIndex >= m_frames.size())
		{
			m_isFinished = true;
			m_frameIndex = (int)m_frames.size() - 1;
		}
	}
}

void CAnimation::Render(HDC hDC)
{
	CObject* owner = m_animator->GetOwner();
	Vec2 position = owner->GetPosition();

	position += m_frames[m_frameIndex].m_offset;
	position = CCamera::GetInstance()->WorldToScreen(position);

	TransparentBlt(hDC,
		(int)(position.m_x - 0.5f * m_frames[m_frameIndex].m_sliceSize.m_x),
		(int)(position.m_y - 0.5f * m_frames[m_frameIndex].m_sliceSize.m_y),
		(int)(m_frames[m_frameIndex].m_sliceSize.m_x),
		(int)(m_frames[m_frameIndex].m_sliceSize.m_y),
		m_texture->GetDC(),
		(int)(m_frames[m_frameIndex].m_leftTop.m_x),
		(int)(m_frames[m_frameIndex].m_leftTop.m_y),
		(int)(m_frames[m_frameIndex].m_sliceSize.m_x),
		(int)(m_frames[m_frameIndex].m_sliceSize.m_y),
		RGB(255, 0, 255));
}
