#include "d3dUtility.h"
#include <iostream>
#include <Windows.h>
#include <string>
using namespace std;

// Globals
//

IDirect3DDevice9* Device = 0;

const int Width = 900;
const int Height = 650;

ID3DXMesh* Objects[14] = { 0 };
D3DXMATRIX  Worlds[14];
D3DMATERIAL9 Mtrls[14];

static float walk_degree_forward = 0.0f; // y

// positions of walking movements
static float walk_position_sides = 0.0f; // x
static float walk_position_Yz = 0.0f; // y
static float walk_position_forward = 0.0f; // z

// limits of plane with x, y, z
static float plane_x = 20.0f;
static float plane_y = -7.4f;
static float plane_z = 20.0f;

// speed of walking
static float speed = 0.02f;

static bool lightEnable = true;

// last move for w and s
static string last_move = "";
static boolean hands_positivity = true;

// font settings
ID3DXMesh* font_mesh = 0;
D3DXMATRIX result;


IDirect3DTexture9* image_wall_tex = 0;
IDirect3DTexture9* wall_tex = 0;
IDirect3DTexture9* beach_tex = 0;
IDirect3DTexture9* sky_tex = 0;
IDirect3DTexture9* sea_tex = 0;
IDirect3DTexture9* ship_tex = 0;


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

D3DXMATRIX translate(D3DXMATRIX base, float x, float y, float z) {
	D3DXMATRIX translateMatrix(1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		x, y, z, 1);

	D3DXMATRIX result = base * translateMatrix;
	return result;
}

D3DXMATRIX PositionFinder(float length, float lengthBefore, float elementPosition[], float degreeOrigin,
	float degreeFinal, string directionOrigin, string directionFinal) {

	D3DXMATRIX tra2Origin(1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1);

	D3DXMATRIX tra2Length(1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, -lengthBefore - length / 2, 0, 1);

	D3DXMATRIX rotFromLength;
	if (directionFinal == "x")
		rotFromLength = D3DXMATRIX(1, 0, 0, 0,
			0, cosf(degreeFinal), sinf(degreeFinal), 0,
			0, -sinf(degreeFinal), cosf(degreeFinal), 0,
			0, 0, 0, 1);

	else if (directionFinal == "y")
		rotFromLength = D3DXMATRIX(cosf(degreeFinal), 0, -sinf(degreeFinal), 0,
			0, 1, 0, 0,
			sinf(degreeFinal), 0, cosf(degreeFinal), 0,
			0, 0, 0, 1);

	else
		rotFromLength = D3DXMATRIX(cosf(degreeFinal), sinf(degreeFinal), 0, 0,
			-sinf(degreeFinal), cosf(degreeFinal), 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1);

	D3DXMATRIX tra2Final(1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		elementPosition[0], elementPosition[1] + lengthBefore + length / 2, elementPosition[2], 1);

	D3DXMATRIX result = tra2Origin  * tra2Length * rotFromLength * tra2Final;
	return result;
}


// Framework Functions 
	// Create objects.
	//
	void CreateObjects(){

		D3DXCreateBox(Device, 2.0, 2.0, 1.7, &Objects[0], 0); //body
		D3DXCreateSphere(Device, 0.7f, 20, 20, &Objects[1], 0); //head
		D3DXCreateTorus(Device, 0.1f, 0.07f, 20, 20, &Objects[2], 0); //eye1
		D3DXCreateBox(Device, 0.5, 0.7, 0.2, &Objects[3], 0); // neck
		D3DXCreateBox(Device, 0.5, 2.0, 0.4, &Objects[4], 0); //leg1
		D3DXCreateBox(Device, 0.3, 2.0, 0.3, &Objects[5], 0); // arm 1
		D3DXCreateBox(Device, 0.5, 2.0, 0.4, &Objects[6], 0); //leg2
		D3DXCreateTorus(Device, 0.1f, 0.07f, 20, 20, &Objects[7], 0); //eye2
		D3DXCreateSphere(Device, 0.3f, 20, 20, &Objects[8], 0); // shoulder1
		D3DXCreateSphere(Device, 0.3f, 20, 20, &Objects[9], 0); // shoulder2
		D3DXCreateBox(Device, 0.3, 2.0, 0.3, &Objects[10], 0); // arm 2
		D3DXCreateBox(Device, 0.5, 0.1, 0.2, &Objects[11], 0); //lip
		D3DXCreateTorus(Device, 0.1f, 0.07f, 20, 20, &Objects[12], 0); //ear 1
		D3DXCreateTorus(Device, 0.1f, 0.07f, 20, 20, &Objects[13], 0); // ear 2

		//
		// Build world matrices - position the objects in world space.
		//

		D3DXMatrixTranslation(&Worlds[0], 0.0f, -4.0f, -3.0f);
		D3DXMatrixTranslation(&Worlds[1], 0.0f, -2.1f, -3.0f);
		D3DXMatrixTranslation(&Worlds[2], 0.3f, -2.1f, -3.7f);
		D3DXMatrixTranslation(&Worlds[3], 0.0f, -2.8f, -3.0f);
		D3DXMatrixTranslation(&Worlds[7], -0.3f, -2.1f, -3.7f);
		D3DXMatrixTranslation(&Worlds[8], 1.2f, -3.0f, -3.0f);
		D3DXMatrixTranslation(&Worlds[9], -1.2f, -3.0f, -3.0f);
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
		//Mtrls[5] = d3d::SKIN_MTRL;
		Mtrls[5] = d3d::VIVID_MTRL;
		Mtrls[6] = d3d::ORANGE_MTRL;
		Mtrls[7] = d3d::RED_MTRL;
		Mtrls[8] = d3d::SKIN_MTRL;
		Mtrls[9] = d3d::SKIN_MTRL;
		Mtrls[10] = d3d::VIVID_MTRL;
		Mtrls[11] = d3d::RED_MTRL;
		Mtrls[12] = d3d::RED_MTRL;
		Mtrls[13] = d3d::RED_MTRL;

		Worlds[5] = PositionFinder(3, 0, new float[] {-1.5f, -4.1f, -3.0f}, 3.14 / 2, 0, "x", "y");//hand1
		Worlds[10] = PositionFinder(3, 0, new float[] {1.5f, -4.1f, -3.0f}, 3.14 / 2, 0, "x", "y");//hand2
		Worlds[4] = PositionFinder(3, 0, new float[] {-0.6f, -6.0f, -3.0f}, 3.14 / 2, 0, "x", "x");
		Worlds[6] = PositionFinder(3, 0, new float[] {0.6f, -6.0f, -3.0f}, 3.14 / 2, 0, "x", "x");
	}
	
	//
	// Setup a directional light.
	//
	void Light(){

		D3DXVECTOR3 dir(2.0f , 2.0f, 2.0f);
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
			60 * sizeof(Vertex),
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
		v[30] = Vertex(-6.0f, plane_y -3 , plane_z - 0.01, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f),
		v[31] = Vertex(-6.0f, 6, plane_z - 0.01, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f),
		v[32] = Vertex(6.0f, 6, plane_z - 0.01, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f),

		v[33] = Vertex(-6.0f, plane_y -3, plane_z - 0.01, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f),
		v[34] = Vertex(6.0f, 6, plane_z - 0.01, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f),
		v[35] = Vertex(6.0f, plane_y -3, plane_z - 0.01, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f),

		//ground
		v[36] = Vertex(-plane_x*10, -plane_y - 15, -plane_z*5-50, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f);
		v[37] = Vertex(-plane_x*10, -plane_y - 15, plane_z*5-50, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f);
		v[38] = Vertex(plane_x*10, -plane_y - 15, plane_z*5-50, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f);

		v[39] = Vertex(-plane_x*10, -plane_y - 15, -plane_z*5-50, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f);
		v[40] = Vertex(plane_x*10, -plane_y - 15, plane_z*5-50, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f);
		v[41] = Vertex(plane_x*10, -plane_y - 15, -plane_z*5-50, 1.0f, 0.0f, -1.0f, 1.0f, 1.0f);

		//sky
		v[42] = Vertex(-plane_x * 10, -plane_y + 50, -plane_z * 10, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f);
		v[43] = Vertex(-plane_x * 10, -plane_y + 50, plane_z * 10, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f);
		v[44] = Vertex(plane_x * 10, -plane_y + 50, plane_z * 10, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f);

		v[45] = Vertex(-plane_x * 10, -plane_y + 50, -plane_z * 10, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f);
		v[46] = Vertex(plane_x * 10, -plane_y + 50, plane_z * 10, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f);
		v[47] = Vertex(plane_x * 10, -plane_y + 50, -plane_z * 10, 1.0f, 0.0f, -1.0f, 1.0f, 1.0f);

		//sea
		v[48] = Vertex(-plane_x * 20, -plane_y - 15, -plane_z * 10 + 250, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f);
		v[49] = Vertex(-plane_x * 20, -plane_y - 15, plane_z * 10 + 250, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f);
		v[50] = Vertex(plane_x * 20, -plane_y - 15, plane_z * 10 + 250, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f);

		v[51] = Vertex(-plane_x * 20, -plane_y - 15, -plane_z * 10 + 250, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f);
		v[52] = Vertex(plane_x * 20, -plane_y - 15, plane_z * 10 + 250, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f);
		v[53] = Vertex(plane_x * 20, -plane_y - 15, -plane_z * 10 + 250, 1.0f, 0.0f, -1.0f, 1.0f, 1.0f);

		//ship
		v[54] = Vertex(-plane_x * 2 + 200, plane_y * 2 + 9, plane_z + 350, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
		v[55] = Vertex(-plane_x * 2 + 200, -plane_y * 2 + 9, plane_z + 350, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
		v[56] = Vertex(plane_x * 2 + 200, -plane_y * 2 + 9, plane_z + 350, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);

		v[57] = Vertex(-plane_x * 2 + 200, plane_y * 2 + 9, plane_z + 350, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
		v[58] = Vertex(plane_x * 2 + 200, -plane_y * 2 + 9, plane_z + 350, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
		v[59] = Vertex(plane_x * 2 + 200, plane_y * 2 + 9, plane_z + 350, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);
			
			VB->Unlock();
	}

void Cleanup()
{
	d3d::Release<IDirect3DVertexBuffer9*>(VB);
	for (int i = 0; i < 14; i++)
		d3d::Release<ID3DXMesh*>(Objects[i]);
}

void resetBodyWall() {
	Worlds[5] = PositionFinder(3.0f, 0, new float[] {-1.5f - walk_position_sides, -4.1f + walk_position_Yz,
		-3.0f - walk_position_forward}, 3.14 / 2, 0, "x", "x");

	Worlds[10] = PositionFinder(3.0f, 0, new float[] {+1.5f - walk_position_sides, -4.1f + walk_position_Yz,
		-3.0f - walk_position_forward}, 3.14 / 2, 0, "x", "x");

	Worlds[4] = PositionFinder(3.0f, 0, new float[] {-0.6f - walk_position_sides, -6.0f + walk_position_Yz,
		-3.0f - walk_position_forward}, 3.14 / 2, 0, "x", "x");

	Worlds[10]= PositionFinder(3.0f, 0, new float[] {0.6f - walk_position_sides, -6.0f + walk_position_Yz,
		-3.0f - walk_position_forward}, 3.14 / 2, 0, "x", "x");
}

void walkForBack(float degreeToRotate, string direction) { // z
	// prevent from overflowing the plane
	if ((direction == "w" && walk_position_forward > plane_z - 1) ||
		(direction == "s" && walk_position_forward < -plane_z + 1)) {
		resetBodyWall();
		return;
	}

	if (direction == "w") walk_position_forward += speed;
	if (direction == "s") walk_position_forward -= speed;

	// legs new position
	Worlds[5] = PositionFinder(3.0f, 0, new float[] {-1.5f - walk_position_sides, -4.1f + walk_position_Yz,
		-3.0f - walk_position_forward}, 3.14 / 2, degreeToRotate, "x", "x");
	Worlds[10] = PositionFinder(3.0f, 0, new float[] {1.5f - walk_position_sides, -4.1f + walk_position_Yz,
		-3.0f - walk_position_forward}, 3.14 / 2, -degreeToRotate, "x",
		"x");
	Device->SetTransform(D3DTS_WORLD, &Worlds[5]);
	Objects[5]->DrawSubset(0);
	Device->SetTransform(D3DTS_WORLD, &Worlds[10]);
	Objects[10]->DrawSubset(0);

	// hands new position
	Worlds[4] = PositionFinder(3.0f, 0, new float[] {-0.6f - walk_position_sides, -6.0f + walk_position_Yz,
		-3.0f - walk_position_forward}, 3.14 / 2, degreeToRotate, "x", "x");
	Worlds[6] = PositionFinder(3.0f, 0, new float[] {+0.6f - walk_position_sides, -6.0f + walk_position_Yz,
		-3.0f - walk_position_forward}, 3.14 / 2, -degreeToRotate, "x",
		"x");
	Device->SetTransform(D3DTS_WORLD, &Worlds[4]);
	Objects[4]->DrawSubset(0);
	Device->SetTransform(D3DTS_WORLD, &Worlds[6]);
	Objects[6]->DrawSubset(0);

	// translate all objects a "speed"
	if (direction == "w")
		for (int i = 0; i < 16; i++)
			Worlds[i] = translate(Worlds[i], 0.0f, 0.0f, -speed);

	else
		for (int i = 0; i < 16; i++)
			Worlds[i] = translate(Worlds[i], 0.0f, 0.0f, speed);
}

// function to walk forward and backward in Y
void walkUpDown(string direction) { // y
	// prevent from overflowing the plane
	if ((direction == "q" && walk_position_Yz > 9) || (direction == "e" && walk_position_Yz < 0.1))
		return;

	if (direction == "q") walk_position_Yz += speed;
	if (direction == "e") walk_position_Yz -= speed;

	// translate all objects a "speed"
	if (direction == "q")
		for (int i = 0; i < 14; i++)
			Worlds[i] = translate(Worlds[i], 0.0f, +speed, 0.0f);

	else
		for (int i = 0; i < 14; i++)
			Worlds[i] = translate(Worlds[i], 0.0f, -speed, 0.0f);
}

// function to walk forward and backward in X
void walkLeftRight(string direction) {
	// prevent from overflowing the plane
	if ((direction == "a" && walk_position_sides > plane_x - 2) ||
		(direction == "d" && walk_position_sides < -plane_x + 2))
		return;

	if (direction == "a") walk_position_sides += speed;
	if (direction == "d") walk_position_sides -= speed;

	// translate all objects a "speed"
	if (direction == "a")
		for (int i = 0; i < 14; i++)
			Worlds[i] = translate(Worlds[i], -speed, 0.0f, 0.0f);

	else
		for (int i = 0; i < 14; i++)
			Worlds[i] = translate(Worlds[i], speed, 0.0f, 0.0f);
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

	//draw the ground
	Device->SetMaterial(&d3d::WHITE_MTRL);
	Device->SetTexture(0, beach_tex);
	Device->DrawPrimitive(D3DPT_TRIANGLELIST, 36, 2);

	//draw the sky
	Device->SetMaterial(&d3d::WHITE_MTRL);
	Device->SetTexture(0, sky_tex);
	Device->DrawPrimitive(D3DPT_TRIANGLELIST, 42, 2);
	
	//draw the sea
	Device->SetMaterial(&d3d::WHITE_MTRL);
	Device->SetTexture(0, sea_tex);
	Device->DrawPrimitive(D3DPT_TRIANGLELIST, 48, 2);

	//drw the ship
	Device->SetMaterial(&d3d::WHITE_MTRL);
	Device->SetTexture(0, ship_tex);
	Device->DrawPrimitive(D3DPT_TRIANGLELIST, 54, 2);

}

void createName() {
	// Get a handle to a device context.
	HDC hdc = CreateCompatibleDC(0);
	HFONT hFont;
	HFONT hFontOld;

	// Describe the font we want.
	LOGFONT lf;
	ZeroMemory(&lf, sizeof(LOGFONT));

	lf.lfHeight = 12;    // in logical units
	lf.lfWidth = 12;    // in logical units
	lf.lfEscapement = 0;
	lf.lfOrientation = 500;
	lf.lfWeight = 800;   // boldness, range 0(light) - 1000(bold)
	lf.lfItalic = false;
	lf.lfUnderline = false;
	lf.lfStrikeOut = true;
	lf.lfCharSet = DEFAULT_CHARSET;
	lf.lfOutPrecision = 100;
	lf.lfClipPrecision = 0;
	lf.lfQuality = 0;
	lf.lfPitchAndFamily = 0;

	string strFaceName = "Times New Roman";
	copy(strFaceName.begin(), strFaceName.end(), lf.lfFaceName);

	// Create the font and select it with the device context.
	hFont = CreateFontIndirect(&lf);
	hFontOld = (HFONT)SelectObject(hdc, hFont);

	// Create the text mesh based on the selected font in the HDC.
	D3DXCreateTextA(Device, hdc, "WELCOME TO ABOLFAZL'S HOUSE NEAR THE SEA ", 0.001f, 0.4f, &font_mesh, 0, 0);

	// Restore the old font and free the acquired HDC.
	SelectObject(hdc, hFontOld);
	DeleteObject(hFont);
	DeleteDC(hdc);

	D3DXMatrixTranslation(&result, -5.0f, 10.0f, plane_z);
}

// function to render the shadow
void RenderShadow() {
	Device->SetRenderState(D3DRS_STENCILENABLE, true);
	Device->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_EQUAL);
	Device->SetRenderState(D3DRS_STENCILREF, 0x0);
	Device->SetRenderState(D3DRS_STENCILMASK, 0xffffffff);
	Device->SetRenderState(D3DRS_STENCILWRITEMASK, 0xffffffff);
	Device->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
	Device->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
	Device->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_INCR); // increment to 1

	// position shadow
	D3DXVECTOR4 lightDirection(2.0f, 2.0f, 2.0f, 0.0f);
	D3DXPLANE groundPlane(0.0f, 1.0f, 0.0f, 0.0f);

	D3DXMATRIX W, S;
	D3DXMATRIX tra = {
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			plane_y, plane_y, plane_y, 1.0f
	};

	D3DXMatrixShadow(
		&S,
		&lightDirection,
		&groundPlane);

	S *= tra;

	int object_index;

	// alpha blend the shadow
	Device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	D3DMATERIAL9 mtrl = d3d::InitMtrl(d3d::BLACK, d3d::BLACK, d3d::BLACK, d3d::BLACK, 0.0f);
	mtrl.Diffuse.a = 0.5f; // 50% transparency.

	// Disable depth buffer so that z-fighting doesn't occur when we render the shadow on top of the floor.
	Device->SetRenderState(D3DRS_ZENABLE, false);

	for (int i = 0; i < 14; i++) {
		W = Worlds[i] * S;
		Device->SetTransform(D3DTS_WORLD, &W);
		Device->SetMaterial(&mtrl);
		Objects[i]->DrawSubset(0);
	}

	Device->SetRenderState(D3DRS_ZENABLE, true);
	Device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	Device->SetRenderState(D3DRS_STENCILENABLE, false);
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

		if (::GetAsyncKeyState(0x57) & 0x8000f) { // w

			// walk back if last move is different from w
			if (last_move == "s") {
				if (walk_degree_forward > 0) {
					for (int x = 0; x < 1000000; x++) {
						if (x % 100 == 0) {
							walk_degree_forward -= speed / 2.0f;
							walkForBack(-walk_degree_forward, "w");
						}

						if (walk_degree_forward > 0)
							break;
					}

				}
				else {
					for (int x = 0; x < 1000000; x++) {
						if (x % 100 == 0) {
							walk_degree_forward += speed / 2.0f;
							walkForBack(-walk_degree_forward, "w");
						}

						if (walk_degree_forward < 0)
							break;
					}
				}

				Sleep(100);
				last_move = "w";

			}
			else {
				last_move = "w";

				if (hands_positivity)
					walk_degree_forward += speed;

				else
					walk_degree_forward -= speed;

				walkForBack(walk_degree_forward, "w");

				if (walk_degree_forward >= 3.14f / 4)
					hands_positivity = false;

				else if (walk_degree_forward <= -3.14f / 4)
					hands_positivity = true;
			}
		}

		// move backward
		if (::GetAsyncKeyState(0x53) & 0x8000f) { // s

			// walk back if last move is different from s
			if (last_move == "w") {
				if (walk_degree_forward > 0) {
					for (int x = 0; x < 1000000; x++) {
						if (x % 100 == 0) {
							walk_degree_forward -= speed / 2.0f;
							walkForBack(walk_degree_forward, "s");
						}

						if (walk_degree_forward > 0)
							break;
					}
				}
				else {
					for (int x = 0; x < 1000000; x++) {
						if (x % 100 == 0) {
							walk_degree_forward += speed / 2.0f;
							walkForBack(walk_degree_forward, "s");
						}

						if (walk_degree_forward < 0)
							break;
					}
				}

				Sleep(100);
				last_move = "s";

			}
			else {
				last_move = "s";

				if (hands_positivity)
					walk_degree_forward += speed / 2.0f;

				else
					walk_degree_forward -= speed / 2.0f;

				walkForBack(-walk_degree_forward, "s");

				if (walk_degree_forward >= 3.14f / 6)
					hands_positivity = false;

				else if (walk_degree_forward <= -3.14f / 6)
					hands_positivity = true;
			}
		}

		//move left
		if (::GetAsyncKeyState(0x41) & 0x8000f) { // a
			walkLeftRight("a");
		}

		//move right
		if (::GetAsyncKeyState(0x44) & 0x8000f) { // d
			walkLeftRight("d");

		}

		// light on/off
		if (GetKeyState(VK_SHIFT) & 0x8000f) // l
			if (::GetAsyncKeyState(0x4c) & 0x8000f) {
				lightEnable = false;
				Light();
			}

		if (GetKeyState(VK_SHIFT) != 1) // l
			if (::GetAsyncKeyState(0x4c) & 0x8000f) {
				lightEnable = true;
				Light();
			}

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
		RenderShadow();

		// draw font and name, set material and world matrix
		Device->SetTransform(D3DTS_WORLD, &result);
		Device->SetMaterial(&d3d::RED_MTRL);
		font_mesh->DrawSubset(0);

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
	createName();

	// set and enable light
	Light();

	// load the image to texture
	D3DXCreateTextureFromFileA(Device, ".\\floor.jpg", &image_wall_tex);
	D3DXCreateTextureFromFileA(Device,".\\wall.jpg" , &wall_tex);
	D3DXCreateTextureFromFileA(Device, ".\\mirror.jpg", &mirror_texture);
	D3DXCreateTextureFromFileA(Device, ".\\sand.jpg", &beach_tex);
	D3DXCreateTextureFromFileA(Device, ".\\sky.jpg", &sky_tex);
	D3DXCreateTextureFromFileA(Device, ".\\sea.png", &sea_tex);
	D3DXCreateTextureFromFileA(Device, ".\\download (2).jpg", &ship_tex);
	
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