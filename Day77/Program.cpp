#include "stdafx.h"
#include "Program.h"
#include "GameObject\Rect.h"

Program::Program()
{
	srand(time(NULL));

	rect = new Rect;
	rect->Init();

	vertices = new Vertex[2];

	vertices[0].position = Vector2(25, -55);
	vertices[0].color = 0xFFFF0000;

	vertices[1].position = Vector2(-50, 25);
	vertices[1].color = 0xFFFF0000;

	D2D::GetDevice()->SetRenderState(
		// ����Ʈ ����
		D3DRS_LIGHTING,
		// ��� ����
		FALSE
	);

	vEye = Vector2(0, 0);	// ī�޶��� ��ġ
	ray = Ray(Vector2(0, 0), Vector2(-1, 0));
}

Program::~Program()
{
	SAFE_DELETE_ARRAY(vertices);
	rect->Release();
	SAFE_DELETE(rect);
}

void Program::Update()
{
	rect->Update();

	// ī�޶� ����°�
	Vector2 vLookAt(0, 0, 1);	// �ٶ󺸴� ��ǥ��
	vLookAt = vEye + Vector2(0, 0, 1);
	Vector2 vUp(0, 1, 0);
	matView = Matrix::View(vEye, vLookAt, vUp);
	matProjection = Matrix::Ortho(0, WINSIZE_X, WINSIZE_Y, 0, 0.0, 1.0f);

	if (GetKeyState(VK_UP) & 0x8000) { ray.direction.y += 0.1f; }
	if (GetKeyState(VK_DOWN) & 0x8000) { ray.direction.y -= 0.1f; }
	if (GetKeyState(VK_RIGHT) & 0x8000) { ray.direction.x += 0.1f; }
	if (GetKeyState(VK_LEFT) & 0x8000) { ray.direction.x -= 0.1f; }

	// �������� �����ؾߵǱ� ������ normalize �ѹ� �������
	ray.direction = ray.direction.Normalize();

	Vector2 position[2];

	Matrix matViewProj = matView * matProjection;

	for (int i = 0; i < 2; i++) {
		// world view projection ��������� �ٵ� ����� identity�� �Ȱ��ص� ��
		position[i] = vertices[i].position.TransformCoord(matViewProj);
	}

	// outPos ȭ�� ��ǥ���� 2D��ǥ�� �ٽ� �ٲ��ֱ����� 
	// matVieProj�� ������� �����ִ� ��
	if (Collision::IntersectRayToLine(
		ray,
		position[0],
		position[1],
		&outPos
	)) {
		float temp;
		Matrix invMatrix = matViewProj.Inverse(temp);

		outPos = outPos.TransformCoord(invMatrix);
	}

	ImGui::ColorEdit3("RayColor", color);
}

void Program::Render()
{
	// ���������� ����ϴ°� Device�� �Ѱ��ְԵ�
	D2D::GetDevice()->SetTransform(D3DTS_VIEW, &matView.ToDXMatrix());
	D2D::GetDevice()->SetTransform(D3DTS_PROJECTION, &matProjection.ToDXMatrix());

	rect->Render();
}