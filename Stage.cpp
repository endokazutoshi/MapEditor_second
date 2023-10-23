#include "Stage.h"
#include "resource.h"
#include "Engine/Model.h"
#include "Engine/Input.h"
#include "Engine/Camera.h"
#include "Engine/Fbx.h"


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
					else if (radioB_ == IDC_RADIO_DOWN)
					{
						if (table_[x][z].height >= 1)//�e�[�u���̍������P���ゾ������...
							table_[x][z].height--;
					}
					//���W�I�{�^���̑I��
					else if (radioB_ == IDC_RADIO_CHANGE)
					{
						SetBlock(x, z, (BLOCKTYPE)(select_));
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


	//�u�t�@�C����ۑ��v�_�C�A���O�̐ݒ�
								//���O�����ĕۑ��_�C�A���O�̐ݒ�p�\����
	ZeroMemory(&ofn, sizeof(ofn));            	//�\���̏�����
	ofn.lStructSize = sizeof(OPENFILENAME);   	//�\���̂̃T�C�Y
	ofn.lpstrFilter = TEXT("�}�b�v�f�[�^(*.map)\0*.map\0")        //�����t�@�C���̎��
		TEXT("���ׂẴt�@�C��(*.*)\0*.*\0\0");     //����
	ofn.lpstrFile = fileName;               	//�t�@�C����
	ofn.nMaxFile = MAX_PATH;               	//�p�X�̍ő啶����
	ofn.Flags = OFN_OVERWRITEPROMPT;   		//�t���O�i�����t�@�C�������݂�����㏑���m�F�j
	ofn.lpstrDefExt = "map";                  	//�f�t�H���g�g���q

	//�u�t�@�C����ۑ��v�_�C�A���O

	selFile = GetSaveFileName(&ofn);

	//�L�����Z�������璆�f
	if (selFile == FALSE) return;

	hFile = CreateFile(
		fileName,    //�t�@�C����
		GENERIC_WRITE,  //�A�N�Z�X���[�h
		0,
		NULL,
		CREATE_ALWAYS,     //�쐬���@
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		std::wcout << L"�t�@�C���I�[�v���Ɏ��s " << GetLastError() << std::endl;
		return ;
	}
	for (int x = 0; x < XSIZE; x++)
	{
		for (int z = 0; z < ZSIZE; z++)
		{
			data += std::to_string(x) + " " + std::to_string(z) + " " + std::to_string(table_[x][z].type) + " " + std::to_string(table_[x][z].height) + "\n";

		}
	}
		//data.length()
		bytes = 0;
		res = WriteFile(
			hFile,              //�t�@�C���n���h��
			data.c_str(),          //�ۑ�������������
			(DWORD)data.length(),                  //�ۑ����镶����
			&bytes,             //�ۑ������T�C�Y
			NULL
		);

		CloseHandle(hFile);

	
}
void Stage::Load()
{

	//�u�t�@�C����ۑ��v�_�C�A���O�̐ݒ�
	ZeroMemory(&ofn, sizeof(ofn));            	//�\���̏�����
	ofn.lStructSize = sizeof(OPENFILENAME);   	//�\���̂̃T�C�Y
	ofn.lpstrFilter = TEXT("�}�b�v�f�[�^(*.map)\0*.map\0")        //�����t�@�C���̎��
		TEXT("���ׂẴt�@�C��(*.*)\0*.*\0\0");     //����
	ofn.lpstrFile = fileName;               	//�t�@�C����
	ofn.nMaxFile = MAX_PATH;               	//�p�X�̍ő啶����
	ofn.Flags = OFN_FILEMUSTEXIST;   		//�t���O�i�����t�@�C�������݂�����㏑���m�F�j
	//ofn.lpstrDefExt = "map";                  	//�f�t�H���g�g���q

	//�u�t�@�C����ۑ��v�_�C�A���O
	selFile = GetOpenFileName(&ofn);

	//�L�����Z�������璆�f
	if (selFile == FALSE) return;
	hFile = CreateFile(
		fileName,                 //�t�@�C����
		GENERIC_READ,           //�A�N�Z�X���[�h�i�ǂݍ��݁j
		0,                      //���L�i�Ȃ��j
		NULL,                   //�Z�L�����e�B�����i�p�����Ȃ��j
		OPEN_EXISTING,           //�쐬���@
		FILE_ATTRIBUTE_NORMAL,  //�����ƃt���O�i�ݒ�Ȃ��j
		NULL);                  //�g�������i�Ȃ��j
	
	if (hFile != INVALID_HANDLE_VALUE)
	{
		std::wcout << L"�t�@�C���I�[�v���Ɏ��s " << GetLastError() << std::endl;
		return;
	}

	//�t�@�C���̃T�C�Y���擾
	DWORD fileSize = GetFileSize(hFile, NULL);

	//�t�@�C���̃T�C�Y�����������m��
	char* fileData = new char[fileSize];

	bytes = 0; //�ǂݍ��݈ʒu

	res = ReadFile(
		hFile,     //�t�@�C���n���h��
		fileData,      //�f�[�^������ϐ�
		fileSize,  //�ǂݍ��ރT�C�Y
		&bytes,  //�ǂݍ��񂾃T�C�Y
		NULL);     //�I�[�o�[���b�v�h�\���́i����͎g��Ȃ��js
	if (res == FALSE)
	{
		std::wcout << L"�I�[�v���Ɏ��s�h" << GetLastError() << std::endl;
	}
	CloseHandle(hFile);
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
		SendMessage(GetDlgItem(hDlg, IDC_COMBO2), CB_ADDSTRING, 0, (LPARAM)"�f�t�H���g");
		SendMessage(GetDlgItem(hDlg, IDC_COMBO2), CB_ADDSTRING, 0, (LPARAM)"�����K");
		SendMessage(GetDlgItem(hDlg, IDC_COMBO2), CB_ADDSTRING, 0, (LPARAM)"����");
		SendMessage(GetDlgItem(hDlg, IDC_COMBO2), CB_ADDSTRING, 0, (LPARAM)"���n");
		SendMessage(GetDlgItem(hDlg, IDC_COMBO2), CB_ADDSTRING, 0, (LPARAM)"��");
		SendMessage(GetDlgItem(hDlg, IDC_COMBO2), CB_GETCURSEL, 0, 0);
		
		return TRUE;


	
		
	case WM_COMMAND:

		radioB_ = LOWORD(wp);
	    select_ = SendMessage(GetDlgItem(hDlg, IDC_COMBO2), CB_GETCURSEL, 0, 0);

	}
	return FALSE;


}
//
//void Stage::Save()
//{
//	// "�t�@�C����ۑ�" �_�C�A���O�̐ݒ�
//	OPENFILENAME ofn;
//	char fileName[MAX_PATH] = ""; // �t�@�C����
//
//	// �_�C�A���O�{�b�N�X�̏�����
//	ZeroMemory(&ofn, sizeof(ofn));
//	ofn.lStructSize = sizeof(OPENFILENAME);
//	ofn.lpstrFilter = "�e�L�X�g�t�@�C�� (*.txt)\0*.txt\0���ׂẴt�@�C�� (*.*)\0*.*\0";
//	ofn.lpstrFile = fileName;
//	ofn.nMaxFile = MAX_PATH;
//	ofn.Flags = OFN_OVERWRITEPROMPT;
//
//	// "�t�@�C����ۑ�" �_�C�A���O��\��
//	if (GetSaveFileName(&ofn) == TRUE)
//	{
//		// �t�@�C��������ɑI�����ꂽ�ꍇ
//		HANDLE hFile = CreateFile(fileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
//
//		if (hFile != INVALID_HANDLE_VALUE)
//		{
//			// �t�@�C���Ƀf�[�^����������
//			std::string data;
//			for (int x = 0; x < XSIZE; x++)
//			{
//				for (int z = 0; z < ZSIZE; z++)
//				{
//					data += std::to_string(x) + " " + std::to_string(z) + " " + std::to_string(table_[x][z].type) + " " + std::to_string(table_[x][z].height) + "\n";
//				}
//			}
//
//			DWORD bytesWritten;
//			WriteFile(hFile, data.c_str(), static_cast<DWORD>(data.length()), &bytesWritten, NULL);
//
//			CloseHandle(hFile);
//		}
//	}
//}

//void Stage::Load()
//{
//	OPENFILENAME ofn;
//	char fileName[MAX_PATH] = ""; // File name
//
//	// Initialize the dialog box
//	ZeroMemory(&ofn, sizeof(ofn));
//	ofn.lStructSize = sizeof(OPENFILENAME);
//	ofn.lpstrFilter = "Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
//	ofn.lpstrFile = fileName;
//	ofn.nMaxFile = MAX_PATH;
//	ofn.Flags = OFN_FILEMUSTEXIST;
//
//	// Show the "Open File" dialog
//	if (GetOpenFileName(&ofn) == TRUE)
//	{
//		// File has been selected, now let's read its content
//		HANDLE hFile = CreateFile(fileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
//
//		if (hFile != INVALID_HANDLE_VALUE)
//		{
//			DWORD fileSize = GetFileSize(hFile, NULL);
//			char* fileData = new char[fileSize];
//
//			// Read the content of the file
//			DWORD bytesRead;
//			if (ReadFile(hFile, fileData, fileSize, &bytesRead, NULL) != FALSE)
//			{
//				// File content is now in fileData, you can parse and use it
//				std::string data(fileData, bytesRead);
//				std::istringstream dataStream(data);
//				int x, z, type, height;
//
//				// Parse the loaded data and update your stage data accordingly
//				for (int i = 0; i < XSIZE; i++)
//				{
//					for (int j = 0; j < ZSIZE; j++)
//					{
//						dataStream >> x >> z >> type >> height;
//						table_[x][z].type = static_cast<BLOCKTYPE>(type);
//						table_[x][z].height = height;
//					}
//				}
//			}
//
//			CloseHandle(hFile);
//			delete[] fileData; // Free the memory used to store the file content
//		}
//	}
//}

void Stage::Save()
{
	// Initialize the "Save File" dialog settings
	OPENFILENAME ofn;
	char fileName[MAX_PATH] = ""; // File name
	std::string data; // Define the data variable

	// Initialize the dialog settings
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.lpstrFilter = "Map Data Files (*.map)\0*.map\0All Files (*.*)\0*.*\0";
	ofn.lpstrFile = fileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_OVERWRITEPROMPT;

	// Show the "Save File" dialog
	if (GetSaveFileName(&ofn) == TRUE)
	{
		// File has been selected, now let's create and write to the file
		HANDLE hFile = CreateFile(fileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

		if (hFile != INVALID_HANDLE_VALUE)
		{
			for (int x = 0; x < XSIZE; x++)
			{
				for (int z = 0; z < ZSIZE; z++)
				{
					// Append the data to the string
					data += std::to_string(x) + " " + std::to_string(z) + " " + std::to_string(static_cast<int>(table_[x][z].type)) + " " + std::to_string(table_[x][z].height) + "\n";
				}
			}

			// Write the data to the file
			DWORD bytesWritten;
			if (WriteFile(hFile, data.c_str(), static_cast<DWORD>(data.length()), &bytesWritten, NULL) == FALSE)
			{
				// Handle any errors
				std::wcout << L"Failed to write to the file: " << GetLastError() << std::endl;
			}

			CloseHandle(hFile);
		}
	}
}

void Stage::Load()
{
	// Initialize the "Open File" dialog settings
	OPENFILENAME ofn;
	char fileName[MAX_PATH] = "";

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.lpstrFilter = "Map Data Files (*.map)\0*.map\0All Files (*.*)\0*.*\0";
	ofn.lpstrFile = fileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_FILEMUSTEXIST;

	if (GetOpenFileName(&ofn) == TRUE)
	{
		// File has been selected, now let's read its content
		HANDLE hFile = CreateFile(fileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

		if (hFile != INVALID_HANDLE_VALUE)
		{
			DWORD fileSize = GetFileSize(hFile, NULL);
			char* fileData = new char[fileSize];

			DWORD bytesRead;
			if (ReadFile(hFile, fileData, fileSize, &bytesRead, NULL) != FALSE)
			{
				// Parse the loaded data and update your stage data accordingly
				std::string data(fileData, bytesRead);
				 dataStream(data);

				int x, z, type, height;
				for (int i = 0; i < XSIZE; i++)
				{
					for (int j = 0; j < ZSIZE; j++)
					{
						dataStream >> x >> z >> type >> height;
						table_[x][z].type = static_cast<BLOCKTYPE>(type);
						table_[x][z].height = height;
					}
				}
			}

			CloseHandle(hFile);
			delete[] fileData;
		}
	}
}
