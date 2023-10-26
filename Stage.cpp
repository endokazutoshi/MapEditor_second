//Stage.cpp
#include "Stage.h"
#include "resource.h"
#include "Engine/Model.h"
#include "Engine/Input.h"
#include "Engine/Camera.h"
#include "Engine/Fbx.h"
#include <iostream>


void Stage::SetBlock(int _x, int _z, BLOCKTYPE _type)
{
	table_[_x][_z].type = _type;
}

void Stage::SetBlockHeight(int _x, int _z, int _height)
{
	table_[_x][_z].height = _height;
}

//�R���X�g���N�^
Stage::Stage(GameObject* parent)
	:GameObject(parent, "Stage")
{
	for (int i = 0; i < MODEL_NUM; i++) {
		hModel_[i] = -1;
	}
	for (int z = 0; z < ZSIZE; z++) {
		for (int x = 0; x < XSIZE; x++) {
			SetBlock(x, z, DEFAULT);
		}
	}
}

//�f�X�g���N�^
Stage::~Stage()
{
}

//������
void Stage::Initialize()
{
	string modelname[] = {
		"BoxDefault.fbx",
		"BoxBrick.fbx",
		"BoxGrass.fbx",
		"BoxSand.fbx",
		"BoxWater.fbx"
	};



	string fname_base = "assets/";
	//���f���f�[�^�̃��[�h
	for (int i = 0; i < MODEL_NUM; i++) {
		hModel_[i] = Model::Load(fname_base + modelname[i]);
		assert(hModel_[i] >= 0);
	}
	//table�Ƀu���b�N�̃^�C�v���Z�b�g���Ă�낤�I
	for (int z = 0; z < ZSIZE; z++) {
		for (int x = 0; x < XSIZE; x++) {
			SetBlock(x, z, (BLOCKTYPE)(0));
			SetBlockHeight(x, z, 0);
		}
	}
}

//�X�V
void Stage::Update()
{
	if (!Input::IsMouseButtonDown(0))//�}�E�X�{�^������������... 
	{
		return;
	}
	float w = (float)(Direct3D::scrWidth / 2.0f);
	float h = (float)(Direct3D::scrHeight / 2.0f);
	//Offsetx,y ��0
	//minZ =0 maxZ = 1

	XMMATRIX vp =
	{
		 w,  0,  0, 0,
		 0, -h,  0, 0,
		 0,  0,  1, 0,
		 w,  h,  0, 1
	};
	//�r���[�|�[�g
	XMMATRIX invVP = XMMatrixInverse(nullptr, vp);
	//�v���W�F�N�V�����ϊ�
	XMMATRIX invProj = XMMatrixInverse(nullptr, Camera::GetProjectionMatrix());
	//�r���[�ϊ�
	XMMATRIX invView = XMMatrixInverse(nullptr, Camera::GetViewMatrix());
	XMFLOAT3 mousePosFront = Input::GetMousePosition();
	mousePosFront.z = 0.0f;
	XMFLOAT3 mousePosBack = Input::GetMousePosition();
	mousePosBack.z = 1.0f;
	//�@�@mousePosFront���x�N�g���ɕϊ�
	XMVECTOR vMouseFront = XMLoadFloat3(&mousePosFront);
	//�A�@�@��invVP�AinvPrj�AinvView��������
	vMouseFront = XMVector3TransformCoord(vMouseFront, invVP * invProj * invView);
	//�B�@mousePosBack���x�N�g���ɕϊ�
	XMVECTOR vMouseBack = XMLoadFloat3(&mousePosBack);
	//�C�@�B��invVP�AinvPrj�AinvView��������
	vMouseBack = XMVector3TransformCoord(vMouseBack, invVP * invProj * invView);

	std::string data;
	for (int x = 0; x < XSIZE; x++)
	{
		for (int z = 0; z < ZSIZE; z++)
		{
			for (int y = 0; y < table_[x][z].height + 1; y++) // ��u���b�N�͕~����������height + 1�ɂ��Ă���
			{
				//�D�@�A����C�Ɍ������ă��C�����i�Ƃ肠�������f���ԍ���hModel_[0]�j
				RayCastData data;
				XMStoreFloat4(&data.start, vMouseFront);
				XMStoreFloat4(&data.dir, vMouseBack - vMouseFront);
				Transform trans;
				trans.position_.x = x;
				trans.position_.y = y;
				trans.position_.z = z;
				Model::SetTransform(hModel_[0], trans);

				Model::RayCast(hModel_[0], data);
			

				//if�����������班�Ȃ���������...
				// �����Ń��C���ˁA�N���b�N�������������ɂȂ�
				if (data.hit)
				{
					if (radioB_ == IDC_RADIO_UP)
					{
						table_[x][z].height++;
					}
					//���W�I�{�^���̑I��
				    if (radioB_ == IDC_RADIO_DOWN)
					{
						if (table_[x][z].height >= 1)//�e�[�u���̍������P���ゾ������...
							table_[x][z].height--;
					}
					//���W�I�{�^���̑I��
				     if (radioB_ == IDC_RADIO_CHANGE)
					{
						SetBlock(x, z, (BLOCKTYPE)(select_));
					}	
					 if (radioB_ == IDC_RADIO_RESET)//�����N���b�N�Ń��Z�b�g
					 {
						 for (int x = 0; x < XSIZE; x++)
						 {
							 for (int z = 0; z < ZSIZE; z++)
							 {
								 SetBlock(x,z, DEFAULT);//�u���b�N�̃^�C�v���S��0�ɂ���
								 SetBlockHeight(x, z, 0);//�������S��0
							 }
						 }

					 }
				
					 if (radioB_ == IDC_RADIO_ALLUP)
					 {
						 for (int x = 0; x < XSIZE; x++) {
							 for (int z = 0; z < ZSIZE; z++) {
								 table_[x][z].height++;
							 }
						 }
					 }
					 if (radioB_ == IDC_RADIO_ALLDOWN)
					 {
						 for (int x = 0; x < XSIZE; x++) {
							 for (int z = 0; z < ZSIZE; z++) {
								 if(table_[x][z].height >= 1)
								 table_[x][z].height--;
							 }
						 }
					 }
					 if (radioB_ == IDC_RADIO_ALLSELECTION)
					 {
						 for (int x = 0; x < XSIZE; x++)
						 {
							 for (int z = 0; z < ZSIZE; z++)
							 {
                                SetBlock(x, z, (BLOCKTYPE)(allSelect_));
							 }
						 }
						 
					 }
					 return;
				}
			

			}
		}
	}
}



//�`��
void Stage::Draw()
{
	//Model::SetTransform(hModel_, transform_);
	//Model::Draw(hModel_);

	for (int x = 0; x < XSIZE; x++)
	{
		for (int z = 0; z < ZSIZE; z++)
		{
			for (int y = 0; y < table_[x][z].height + 1; y++)
			{
				//table[x][z]����I�u�W�F�N�g�̃^�C�v�����o���ď����I
				int type = table_[x][z].type;
				Transform trans;
				trans.position_.x = x;
				trans.position_.y = y;
				trans.position_.z = z;
				Model::SetTransform(hModel_[type], trans);
				Model::Draw(hModel_[type]);

				
			}
		}
	}

}

//�J��
void Stage::Release()
{
}



void Stage::Save()
{

	// �_�C�A���O�{�b�N�X�̏�����
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.lpstrFilter = "�e�L�X�g�t�@�C�� (*.txt)\0*.txt\0���ׂẴt�@�C�� (*.*)\0*.*\0";
	ofn.lpstrFile = fileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_OVERWRITEPROMPT;

	// "�t�@�C����ۑ�" �_�C�A���O��\��
	if (GetSaveFileName(&ofn) == TRUE)
	{
		// �t�@�C��������ɑI�����ꂽ�ꍇ
		HANDLE hFile = CreateFile(
			fileName,
			GENERIC_WRITE,
			0,
			NULL,
			CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL);
		
		std::string data;

		if (hFile != INVALID_HANDLE_VALUE)
		{
			// �t�@�C���Ƀf�[�^����������
			for (int x = 0; x < XSIZE; x++)
			{
				for (int z = 0; z < ZSIZE; z++)
				{
					data += std::to_string(table_[x][z].type) + " "
                          + std::to_string(table_[x][z].height) + "\n";
				}
			}
			bytes = 0;
			WriteFile(
				hFile,
				data.c_str(),
				(DWORD)data.length(),
				&bytes,
				NULL);

			CloseHandle(hFile);
		}
	}
}

void Stage::Load()
{
	// Configure the "Open File" dialog
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.lpstrFilter = TEXT("�e�L�X�g�t�@�C�� (*.txt)\0*.txt\0���ׂẴt�@�C�� (*.*)\0*.*\0");
	ofn.lpstrFile = fileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_FILEMUSTEXIST;

	// Display the "Open File" dialog
	selFile = GetOpenFileName(&ofn);

	// Check if the user canceled the dialog
	if (selFile == FALSE)
		return;

	// Open the selected file for reading
	hFile = CreateFile(
		fileName,
		GENERIC_READ,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		// Handle file open error, if needed
		std::wcout << L"�t�@�C���I�[�v���Ɏ��s " << GetLastError() << std::endl;
		return;
	}

	// Get the file size
	DWORD fileSize = GetFileSize(hFile, NULL);

	// Allocate memory to read the file content
	char* fileData = new char[fileSize];

	// Read the file content into the allocated memory
	bytes = 0; // Reset the read position
	res = ReadFile(
		hFile,
		fileData,
		(DWORD)fileSize,
		&bytes,
		NULL);

	if (res == FALSE)
	{
		// Handle read error, if needed
		std::wcout << L"�t�@�C���ǂݍ��݂Ɏ��s" << GetLastError() << std::endl;
		CloseHandle(hFile);
		delete[] fileData;
		return;
	}

	// Close the file
	CloseHandle(hFile);

	// Now, you have the file data in the 'fileData' variable
	// You need to parse and process it to update your stage data
	// For example, you can split the data by newline and process it line by line
	std::string fileContent(fileData, fileSize);

	// Parse and update the stage data here

	// Clean up allocated memory
	delete[] fileData;
}


BOOL Stage::DialogProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg)
	{
		//�_�C�A���O���ł���
	case WM_INITDIALOG:
		//���W�I�{�^���̏����l
		SendMessage(GetDlgItem(hDlg, IDC_RADIO_UP), BM_SETCHECK, BST_CHECKED, 0);
		//�R���{�{�b�N�X�̏����l
		SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_ADDSTRING, 0, (LPARAM)"�f�t�H���g");
		SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_ADDSTRING, 0, (LPARAM)"�����K");
		SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_ADDSTRING, 0, (LPARAM)"����");
		SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_ADDSTRING, 0, (LPARAM)"���n");
		SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_ADDSTRING, 0, (LPARAM)"��");
		SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_GETCURSEL, 0, 0);

		SendMessage(GetDlgItem(hDlg, IDC_RADIO_ALLSELECTION), BM_SETCHECK, BST_CHECKED, 0);
		SendMessage(GetDlgItem(hDlg, IDC_RADIO_RESET), BM_SETCHECK, BST_CHECKED, NULL);

		SendMessage(GetDlgItem(hDlg, IDC_RADIO_ALLUP), BM_SETCHECK, BST_CHECKED, NULL);
		SendMessage(GetDlgItem(hDlg, IDC_COMBO3), CB_ADDSTRING, 0, (LPARAM)"�f�t�H���g");
		SendMessage(GetDlgItem(hDlg, IDC_COMBO3), CB_ADDSTRING, 0, (LPARAM)"�����K");
		SendMessage(GetDlgItem(hDlg, IDC_COMBO3), CB_ADDSTRING, 0, (LPARAM)"����");
		SendMessage(GetDlgItem(hDlg, IDC_COMBO3), CB_ADDSTRING, 0, (LPARAM)"���n");
		SendMessage(GetDlgItem(hDlg, IDC_COMBO3), CB_ADDSTRING, 0, (LPARAM)"��");
		SendMessage(GetDlgItem(hDlg, IDC_COMBO3), CB_GETCURSEL, 0, 0);

		
		return TRUE;


	
		
	case WM_COMMAND:

		radioB_ = LOWORD(wp);
	    select_ = SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_GETCURSEL, 0, 0);
		allSelect_ = SendMessage(GetDlgItem(hDlg, IDC_COMBO3), CB_GETCURSEL, 0, 0);

	}
	return FALSE;


}

