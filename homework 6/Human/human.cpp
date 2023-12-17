#include "d3dUtility.h"
#include <iostream>
#include <Windows.h>
#include <string>
//#include "cube.cpp"
//
// Globals
//

IDirect3DDevice9* Device = 0;

const int Width = 900;
const int Height = 650;

ID3DXMesh* Objects[14] = { 0 };
D3DXMATRIX  Worlds[14];
D3DMATERIAL9 Mtrls[14];

static float plane_x = 20.0f;
static float plane_y = -7.4f;
static float plane_z = 20.0f;
static bool lightEnable = true;

IDirect3DTexture9* image_wall_tex = 0;
IDirect3DTexture9* wall_tex = 0;

// mirror
D3DMATERIAL9 mirror_material = d3d::WHITE_MTRL;
IDirect3DTexture9* mirror_texture = 0;



IDirect3DVertexBuffer9* VB = 0;

// Vertex structure for planes
struct Vertex {
	Vertex() {}

	Vertex(float x, float y, float z,
		float nx, float ny, float nz,
		float u, float v) {
		_x = x;
		_y = y;
		_z = z;
		_nx = nx;
		_ny = ny;
		_nz = nz;
		_u = u;
		_v = v;
	}

	float _x, _y, _z;
	float _nx, _ny, _nz;
	float _u, _v;

	static const DWORD FVF;
};

const DWORD Vertex::FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;

// Framework Functions 
	// Create objects.
	//
	void CreateObjects(){

		D3DXCreateBox(Device, 2.0, 2.0, 1.7, &Objects[0], 0);
		D3DXCreateSphere(Device, 0.7f, 20, 20, &Objects[1], 0);
		D3DXCreateTorus(Device, 0.1f, 0.07f, 20, 20, &Objects[2], 0);
		D3DXCreateBox(Device, 0.5, 0.7, 0.2, &Objects[3], 0);
		D3DXCreateBox(Device, 0.5, 2.0, 0.4, &Objects[4], 0);
		D3DXCreateBox(Device, 0.3, 2.0, 0.3, &Objects[5], 0);
		D3DXCreateBox(Device, 0.5, 2.0, 0.4, &Objects[6], 0);
		D3DXCreateTorus(Device, 0.1f, 0.07f, 20, 20, &Objects[7], 0);
		D3DXCreateSphere(Device, 0.3f, 20, 20, &Objects[8], 0);
		D3DXCreateSphere(Device, 0.3f, 20, 20, &Objects[9], 0);
		D3DXCreateBox(Device, 0.3, 2.0, 0.3, &Objects[10], 0);
		D3DXCreateBox(Device, 0.5, 0.1, 0.2, &Objects[11], 0);
		D3DXCreateTorus(Device, 0.1f, 0.07f, 20, 20, &Objects[12], 0);
		D3DXCreateTorus(Device, 0.1f, 0.07f, 20, 20, &Objects[13], 0);

		//
		// Build world matrices - position the objects in world space.
		//

		D3DXMatrixTranslation(&Worlds[0], 0.0f, -4.0f, -3.0f);
		D3DXMatrixTranslation(&Worlds[1], 0.0f, -2.1f, -3.0f);
		D3DXMatrixTranslation(&Worlds[2], 0.3f, -2.1f, -3.7f);
		D3DXMatrixTranslation(&Worlds[3], 0.0f, -2.8f, -3.0f);
		D3DXMatrixTranslation(&Worlds[4], -0.6f, -6.0f, -3.0f);
		D3DXMatrixTranslation(&Worlds[5], 1.5f, -4.1f, -3.0f);
		D3DXMatrixTranslation(&Worlds[6], 0.6f, -6.0f, -3.0f);
		D3DXMatrixTranslation(&Worlds[7], -0.3f, -2.1f, -3.7f);
		D3DXMatrixTranslation(&Worlds[8], 1.2f, -3.0f, -3.0f);
		D3DXMatrixTranslation(&Worlds[9], -1.2f, -3.0f, -3.0f);
		D3DXMatrixTranslation(&Worlds[10], -1.5f, -4.1f, -3.0f);
		D3DXMatrixTranslation(&Worlds[11], 0.0f, -2.4f, -3.6f);
		D3DXMatrixTranslation(&Worlds[12], -0.7f, -2.1f, -3.0f);
		D3DXMatrixTranslation(&Worlds[13], 0.7f, -2.1f, -3.0f);

		//
		// Setup the object's materials.

		Mtrls[0] = d3d::GREEN_MTRL;
		Mtrls[1] = d3d::VIVID2_MTRL;
		Mtrls[2] = d3d::RED_MTRL;
		Mtrls[3] = d3d::VIVID_MTRL;
		Mtrls[4] = d3d::ORANGE_MTRL;
		Mtrls[5] = d3d::SKIN_MTRL;
		Mtrls[5] = d3d::VIVID_MTRL;
		Mtrls[6] = d3d::ORANGE_MTRL;
		Mtrls[7] = d3d::RED_MTRL;
		Mtrls[8] = d3d::SKIN_MTRL;
		Mtrls[9] = d3d::SKIN_MTRL;
		Mtrls[10] = d3d::VIVID_MTRL;
		Mtrls[11] = d3d::RED_MTRL;
		Mtrls[12] = d3d::RED_MTRL;
		Mtrls[13] = d3d::RED_MTRL;
	}
	//
	// Setup a directional light.
	//
	void Light(){

		D3DXVECTOR3 dir(2.0f, 2.0f, 2.0f);
		D3DXCOLOR c;

		// change light color
		c = d3d::WHITE;

		D3DLIGHT9 dirLight = d3d::InitDirectionalLight(&dir, &c);

		// Set and Enable the light.
		Device->SetLight(0, &dirLight);
		Device->LightEnable(0, lightEnable);

		// Set lighting related render states.
		Device->SetRenderState(D3DRS_NORMALIZENORMALS, true);
		Device->SetRenderState(D3DRS_SPECULARENABLE, false);

		// Set the projection matrix.
		D3DXMATRIX proj;
		D3DXMatrixPerspectiveFovLH(
			&proj,
			D3DX_PI * 0.25f, // 45 - degree
			(float)Width / (float)Height,
			1.0f,
			1000.0f);

		Device->SetTransform(D3DTS_PROJECTION, &proj);
	}

	void CreateHouse() {
		Device->CreateVertexBuffer(
			36 * sizeof(Vertex),
			0,
			Vertex::FVF,
			D3DPOOL_MANAGED,
			&VB,
			0);

		Vertex* v = 0;
		VB->Lock(0, 0, (void**)&v, 0);

		// floor vertex
		v[0] = Vertex(-plane_x, plane_y, -plane_z, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f);
		v[1] = Vertex(-plane_x, plane_y, plane_z, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f);
		v[2] = Vertex(plane_x, plane_y, plane_z, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f);

		v[3] = Vertex(-plane_x, plane_y, -plane_z, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f);
		v[4] = Vertex(plane_x, plane_y, plane_z, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f);
		v[5] = Vertex(plane_x, plane_y, -plane_z, 1.0f, 0.0f, -1.0f, 1.0f, 1.0f);

		// back wall vertex
		v[6] = Vertex(-plane_x, plane_y, plane_z, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
		v[7] = Vertex(-plane_x, -plane_y + 15, plane_z, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
		v[8] = Vertex(plane_x, -plane_y + 15, plane_z, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);

		v[9] = Vertex(-plane_x, plane_y, plane_z, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
		v[10] = Vertex(plane_x, -plane_y + 15, plane_z, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
		v[11] = Vertex(plane_x, plane_y, plane_z, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

		// left wall vertex
		v[12] = Vertex(-plane_x, plane_y, -plane_z, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
		v[13] = Vertex(-plane_x, -plane_y + 15, -plane_z, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
		v[14] = Vertex(-plane_x, -plane_y + 15, plane_z, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);

		v[15] = Vertex(-plane_x, plane_y, -plane_z, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
		v[16] = Vertex(-plane_x, -plane_y + 15, plane_z, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
		v[17] = Vertex(-plane_x, plane_y, plane_z, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

		// right wall vertex
		v[18] = Vertex(plane_x, plane_y, -plane_z, 1.0f, 1.0f, -1.0f, 1.0f, -1.0f);
		v[19] = Vertex(plane_x, -plane_y + 15, -plane_z, 1.0f, 1.0f, -1.0f, 1.0f, 0.0f);
		v[20] = Vertex(plane_x, -plane_y + 15, plane_z, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f);

		v[21] = Vertex(plane_x, plane_y, -plane_z, 1.0f, 1.0f, -1.0f, 1.0f, -1.0f);
		v[22] = Vertex(plane_x, -plane_y + 15, plane_z, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f);
		v[23] = Vertex(plane_x, plane_y, plane_z, 1.0f, 1.0f, -1.0f, 0.0f, -1.0f);

		// roof
		v[24] = Vertex(-plane_x, -plane_y + 15, -plane_z, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f);
		v[25] = Vertex(-plane_x, -plane_y + 15, plane_z, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f);
		v[26] = Vertex(plane_x, -plane_y + 15, plane_z, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f);

		v[27] = Vertex(-plane_x, -plane_y + 15, -plane_z, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f);
		v[28] = Vertex(plane_x, -plane_y + 15, plane_z, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f);
		v[29] = Vertex(plane_x, -plane_y + 15, -plane_z, 1.0f, 0.0f, -1.0f, 1.0f, 1.0f);

		// mirror
		v[30] = Vertex(-6.0f, plane_y, plane_z - 0.01, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f),
		v[31] = Vertex(-6.0f, 9, plane_z - 0.01, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f),
		v[32] = Vertex(6.0f, 9, plane_z - 0.01, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f),

		v[33] = Vertex(-6.0f, plane_y, plane_z - 0.01, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f),
		v[34] = Vertex(6.0f, 9, plane_z - 0.01, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f),
		v[35] = Vertex(6.0f, plane_y, plane_z - 0.01, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f),
			
			VB->Unlock();
	}

void Cleanup()
{
	d3d::Release<IDirect3DVertexBuffer9*>(VB);
	for (int i = 0; i < 14; i++)
		d3d::Release<ID3DXMesh*>(Objects[i]);
}

void RenderScene() {
	D3DXMATRIX I;
	D3DXMatrixIdentity(&I);
	Device->SetTransform(D3DTS_WORLD, &I);

	Device->SetStreamSource(0, VB, 0, sizeof(Vertex));
	Device->SetFVF(Vertex::FVF);

	Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	// draw the floor
	Device->SetMaterial(&d3d::WHITE_MTRL);
	Device->SetTexture(0, image_wall_tex);
	Device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);

	// draw the wall
	Device->SetMaterial(&d3d::WHITE_MTRL);
	Device->SetTexture(0, wall_tex);
	Device->DrawPrimitive(D3DPT_TRIANGLELIST, 6, 2);
	Device->DrawPrimitive(D3DPT_TRIANGLELIST, 18, 2);

	// draw the wall
	Device->SetMaterial(&d3d::WHITE_MTRL);
	Device->SetTexture(0, wall_tex);
	Device->DrawPrimitive(D3DPT_TRIANGLELIST, 12, 2);

	// draw the roof
	Device->SetMaterial(&d3d::WHITE_MTRL);
	Device->SetTexture(0, wall_tex);
	Device->DrawPrimitive(D3DPT_TRIANGLELIST, 24, 2);

	// draw the mirror
	Device->SetMaterial(&mirror_material);
	Device->SetTexture(0, mirror_texture);
	Device->DrawPrimitive(D3DPT_TRIANGLELIST, 30, 2);

}


void RenderMirror() {
	Device->SetRenderState(D3DRS_STENCILENABLE, true);
	Device->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS);
	Device->SetRenderState(D3DRS_STENCILREF, 0x1);
	Device->SetRenderState(D3DRS_STENCILMASK, 0xffffffff);
	Device->SetRenderState(D3DRS_STENCILWRITEMASK, 0xffffffff);
	Device->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
	Device->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
	Device->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_REPLACE);

	// disable writes to the depth and back buffers
	Device->SetRenderState(D3DRS_ZWRITEENABLE, false);
	Device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ZERO);
	Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	// draw the mirror to the stencil buffer
	Device->SetStreamSource(0, VB, 0, sizeof(Vertex));
	Device->SetFVF(Vertex::FVF);
	Device->SetMaterial(&mirror_material);
	Device->SetTexture(0, mirror_texture);

	D3DXMATRIX I;
	D3DXMatrixIdentity(&I);
	Device->SetTransform(D3DTS_WORLD, &I);
	Device->DrawPrimitive(D3DPT_TRIANGLELIST, 30, 2);

	// re-enable depth writes
	Device->SetRenderState(D3DRS_ZWRITEENABLE, true);

	// only draw reflected to the pixels where the mirror was drawn to.
	Device->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_EQUAL);
	Device->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);

	// position reflection
	D3DXMATRIX W;
	D3DXMATRIX ref = {
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, -1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
	};

	D3DXMATRIX tra = {
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, plane_z , 1.0f
	};

	ref *= tra;

	// clear depth buffer and blend the reflected teapot with the mirror
	Device->Clear(0, 0, D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
	Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR);
	Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);

	// find the exact reflection in mirror
	W = Worlds[0] * ref;
	Device->SetTransform(D3DTS_WORLD, &W);
	Device->SetMaterial(&Mtrls[0]);
	Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	Objects[0]->DrawSubset(0);

	W = Worlds[1] * ref;
	Device->SetTransform(D3DTS_WORLD, &W);
	Device->SetMaterial(&Mtrls[1]);
	Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	Objects[1]->DrawSubset(0);

	W = Worlds[3] * ref;
	Device->SetTransform(D3DTS_WORLD, &W);
	Device->SetMaterial(&Mtrls[3]);
	Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	Objects[3]->DrawSubset(0);

	W = Worlds[4] * ref;
	Device->SetTransform(D3DTS_WORLD, &W);
	Device->SetMaterial(&Mtrls[4]);
	Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	Objects[4]->DrawSubset(0);

	W = Worlds[5] * ref;
	Device->SetTransform(D3DTS_WORLD, &W);
	Device->SetMaterial(&Mtrls[5]);
	Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	Objects[5]->DrawSubset(0);

	W = Worlds[6] * ref;
	Device->SetTransform(D3DTS_WORLD, &W);
	Device->SetMaterial(&Mtrls[6]);
	Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	Objects[6]->DrawSubset(0);

	W = Worlds[8] * ref;
	Device->SetTransform(D3DTS_WORLD, &W);
	Device->SetMaterial(&Mtrls[8]);
	Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	Objects[8]->DrawSubset(0);

	W = Worlds[9] * ref;
	Device->SetTransform(D3DTS_WORLD, &W);
	Device->SetMaterial(&Mtrls[9]);
	Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	Objects[9]->DrawSubset(0);

	W = Worlds[10] * ref;
	Device->SetTransform(D3DTS_WORLD, &W);
	Device->SetMaterial(&Mtrls[10]);
	Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	Objects[10]->DrawSubset(0);

	W = Worlds[11] * ref;
	Device->SetTransform(D3DTS_WORLD, &W);
	Device->SetMaterial(&Mtrls[11]);
	Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	Objects[11]->DrawSubset(0);

	W = Worlds[12] * ref;
	Device->SetTransform(D3DTS_WORLD, &W);
	Device->SetMaterial(&Mtrls[12]);
	Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	Objects[12]->DrawSubset(0);

	W = Worlds[13] * ref;
	Device->SetTransform(D3DTS_WORLD, &W);
	Device->SetMaterial(&Mtrls[13]);
	Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	Objects[13]->DrawSubset(0);

	// Restore render states.
	Device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	Device->SetRenderState(D3DRS_STENCILENABLE, false);
	Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

bool Display(float timeDelta)
{
	// variables for keeping the position of the camera
	static float pos_x = 0.0f;
	static float pos_y = 0.0f;
	static float pos_z = -20.0f;

	// variables for keeping the position of the target
	static float target_x = 0.0f;
	static float target_y = -4.0f;
	static float target_z = 0.0f;

	if (Device)
	{

		// Update the scene: update camera position.
		if (::GetAsyncKeyState(VK_LEFT) & 0x8000f) // left
			target_x -= 5.0f * timeDelta;

		if (::GetAsyncKeyState(VK_RIGHT) & 0x8000f) // right
			target_x += 5.0f * timeDelta;

		if (::GetAsyncKeyState(VK_UP) & 0x8000f) // up
			target_y += 5.0f * timeDelta;

		if (::GetAsyncKeyState(VK_DOWN) & 0x8000f) // down
			target_y -= 5.0f * timeDelta;

		// change the camera position
		if (GetKeyState(VK_SHIFT) & 0x8000f) // x
			if (::GetAsyncKeyState(0x58) & 0x8000f)
				pos_x += 15.0f * timeDelta;

		if (GetKeyState(VK_SHIFT) != 1) // x
			if (::GetAsyncKeyState(0x58) & 0x8000f)
				pos_x -= 15.0f * timeDelta;

		if (GetKeyState(VK_SHIFT) & 0x8000f) // y
			if (::GetAsyncKeyState(0x59) & 0x8000f)
				pos_y += 15.0f * timeDelta;

		if (GetKeyState(VK_SHIFT) != 1) // y
			if (::GetAsyncKeyState(0x59) & 0x8000f)
				pos_y -= 15.0f * timeDelta;

		if (GetKeyState(VK_SHIFT) & 0x8000f) // z
			if (::GetAsyncKeyState(0x5a) & 0x8000f)
				pos_z += 15.0f * timeDelta;

		if (GetKeyState(VK_SHIFT) != 1) // z
			if (::GetAsyncKeyState(0x5a) & 0x8000f)
				pos_z -= 15.0f * timeDelta;


		D3DXVECTOR3 position(pos_x, pos_y, pos_z);
		D3DXVECTOR3 target(target_x, target_y, target_z);
		D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
		D3DXMATRIX V;
		D3DXMatrixLookAtLH(&V, &position, &target, &up);

		Device->SetTransform(D3DTS_VIEW, &V);

		// Draw the scene:
		Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 0xff000000, 1.0f, 0L);
		Device->BeginScene();
		RenderScene();

		// set material and world matrix for ith object, then render the ith object.
		for (int i = 0; i < 14; i++) {
			Device->SetTransform(D3DTS_WORLD, &Worlds[i]);
			Device->SetMaterial(&Mtrls[i]);
			Objects[i]->DrawSubset(0);
		}

		// mirror
		RenderMirror();

		Device->EndScene();
		Device->Present(0, 0, 0, 0);
	}
	return true;
}



bool Setup() {
	CreateObjects();
	// create floor and wall
	CreateHouse();

	//CreateGround();

	// set and enable light
	Light();

	// load the image to texture
	D3DXCreateTextureFromFileA(Device, "C:\\Users\\RayanService\\Pictures\\Saved Pictures\\wall1.jpg", &image_wall_tex);
	D3DXCreateTextureFromFileA(Device,"C:\\Users\\RayanService\\Pictures\\Saved Pictures\\wall.jpg" , &wall_tex);
	D3DXCreateTextureFromFileA(Device, "C:\\Users\\RayanService\\Pictures\\Saved Pictures\\ice.jpg", &mirror_texture);

	return true;
}

// WndProc
LRESULT CALLBACK d3d::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {

	case WM_DESTROY:
		::PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
			::DestroyWindow(hwnd);
		break;
	}

	return ::DefWindowProc(hwnd, msg, wParam, lParam);
}

// WinMain
int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd) {

	if (!d3d::InitD3D(hinstance, Width, Height, true, D3DDEVTYPE_HAL, &Device)) {
		::MessageBox(0, (LPCWSTR)"InitD3D() - FAILED", 0, 0);
		return 0;
	}

	if (!Setup()) {
		::MessageBox(0, (LPCWSTR)"Setup() - FAILED", 0, 0);
		return 0;
	}

	d3d::EnterMsgLoop(Display);

//	Cleanup();
	Device->Release();

	return 0;
}