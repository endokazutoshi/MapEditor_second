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

//コンストラクタ
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

//デストラクタ
Stage::~Stage()
{
}

//初期化
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
	//モデルデータのロード
	for (int i = 0; i < MODEL_NUM; i++) {
		hModel_[i] = Model::Load(fname_base + modelname[i]);
		assert(hModel_[i] >= 0);
	}
	//tableにブロックのタイプをセットしてやろう！
	for (int z = 0; z < ZSIZE; z++) {
		for (int x = 0; x < XSIZE; x++) {
			SetBlock(x, z, (BLOCKTYPE)(0));
			SetBlockHeight(x, z, 0);
		}
	}
}

//更新
void Stage::Update()
{
	if (!Input::IsMouseButtonDown(0))//マウスボタンを押したら... 
	{
		return;
	}
	float w = (float)(Direct3D::scrWidth / 2.0f);
	float h = (float)(Direct3D::scrHeight / 2.0f);
	//Offsetx,y は0
	//minZ =0 maxZ = 1

	XMMATRIX vp =
	{
		 w,  0,  0, 0,
		 0, -h,  0, 0,
		 0,  0,  1, 0,
		 w,  h,  0, 1
	};
	//ビューポート
	XMMATRIX invVP = XMMatrixInverse(nullptr, vp);
	//プロジェクション変換
	XMMATRIX invProj = XMMatrixInverse(nullptr, Camera::GetProjectionMatrix());
	//ビュー変換
	XMMATRIX invView = XMMatrixInverse(nullptr, Camera::GetViewMatrix());
	XMFLOAT3 mousePosFront = Input::GetMousePosition();
	mousePosFront.z = 0.0f;
	XMFLOAT3 mousePosBack = Input::GetMousePosition();
	mousePosBack.z = 1.0f;
	//�@　mousePosFrontをベクトルに変換
	XMVECTOR vMouseFront = XMLoadFloat3(&mousePosFront);
	//�A　�@にinvVP、invPrj、invViewをかける
	vMouseFront = XMVector3TransformCoord(vMouseFront, invVP * invProj * invView);
	//�B　mousePosBackをベクトルに変換
	XMVECTOR vMouseBack = XMLoadFloat3(&mousePosBack);
	//�C　�BにinvVP、invPrj、invViewをかける
	vMouseBack = XMVector3TransformCoord(vMouseBack, invVP * invProj * invView);

	std::string data;
	for (int x = 0; x < XSIZE; x++)
	{
		for (int z = 0; z < ZSIZE; z++)
		{
			for (int y = 0; y < table_[x][z].height + 1; y++) // 一ブロックは敷きたいからheight + 1にしている
			{
				//�D　�Aから�Cに向かってレイをうつ（とりあえずモデル番号はhModel_[0]）
				RayCastData data;
				XMStoreFloat4(&data.start, vMouseFront);
				XMStoreFloat4(&data.dir, vMouseBack - vMouseFront);
				Transform trans;
				trans.position_.x = x;
				trans.position_.y = y;
				trans.position_.z = z;
				Model::SetTransform(hModel_[0], trans);

				Model::RayCast(hModel_[0], data);

				//if文が多いから少なくしたいな...
				// ここでレイ発射、クリックした部分だけになる
				if (data.hit)
				{
				 
					
				    if (radioB_ == IDC_RADIO_UP)
					{
						table_[x][z].height++;
					}
					//ラジオボタンの選択
				    if (radioB_ == IDC_RADIO_DOWN)
					{
						if (table_[x][z].height >= 1)//テーブルの高さが１つより上だったら...
							table_[x][z].height--;
					}
					//ラジオボタンの選択
				     if (radioB_ == IDC_RADIO_CHANGE)
					{
						SetBlock(x, z, (BLOCKTYPE)(select_));
					}
					 if (radioB_ == IDC_RADIO_SELECTION)//リセットボタン
					 {
						 // Iterate through the stage and set all blocks of a certain type to be selected
						 for (int x = 0; x < XSIZE; x++)
						 {
							 for (int z = 0; z < ZSIZE; z++)
							 {
								 if (table_[x][z].type == selectedType_)
								 {
									 // Set the selected state for this block
									 table_[x][z].selected = true;
								 }
							 }
						 }
					
					
					 }
					 if (radioB_ == IDC_RADIO_RESET)
					 {
						 for (int x = 0; x < XSIZE; x++)
						 {
							 for (int z = 0; z < ZSIZE; z++)
							 {
								 SetBlock(x, z, DEFAULT);//ブロックのタイプも全て0にする
								 SetBlockHeight(x, z, 0);//高さも全て0
							 }
						 }

					 }
					 return;
				

					return;
				}
			

			}
		}
	}
}



//描画
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
				//table[x][z]からオブジェクトのタイプを取り出して書く！
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

//開放
void Stage::Release()
{
}



void Stage::Save()
{

	// ダイアログボックスの初期化
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.lpstrFilter = "テキストファイル (*.txt)\0*.txt\0すべてのファイル (*.*)\0*.*\0";
	ofn.lpstrFile = fileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_OVERWRITEPROMPT;

	// "ファイルを保存" ダイアログを表示
	if (GetSaveFileName(&ofn) == TRUE)
	{
		// ファイルが正常に選択された場合
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
			// ファイルにデータを書き込む
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
	ofn.lpstrFilter = TEXT("テキストファイル (*.txt)\0*.txt\0すべてのファイル (*.*)\0*.*\0");
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
		std::wcout << L"ファイルオープンに失敗 " << GetLastError() << std::endl;
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
		std::wcout << L"ファイル読み込みに失敗" << GetLastError() << std::endl;
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
		//ダイアログができた
	case WM_INITDIALOG:
		//ラジオボタンの初期値
		SendMessage(GetDlgItem(hDlg, IDC_RADIO_UP), BM_SETCHECK, BST_CHECKED, 0);
		//コンボボックスの初期値
		SendMessage(GetDlgItem(hDlg, IDC_COMBO2), CB_ADDSTRING, 0, (LPARAM)"デフォルト");
		SendMessage(GetDlgItem(hDlg, IDC_COMBO2), CB_ADDSTRING, 0, (LPARAM)"レンガ");
		SendMessage(GetDlgItem(hDlg, IDC_COMBO2), CB_ADDSTRING, 0, (LPARAM)"草原");
		SendMessage(GetDlgItem(hDlg, IDC_COMBO2), CB_ADDSTRING, 0, (LPARAM)"砂地");
		SendMessage(GetDlgItem(hDlg, IDC_COMBO2), CB_ADDSTRING, 0, (LPARAM)"水");
		SendMessage(GetDlgItem(hDlg, IDC_COMBO2), CB_GETCURSEL, 0, 0);

		SendMessage(GetDlgItem(hDlg, IDC_RADIO_SELECTION), BM_SETCHECK, BST_CHECKED, 0);
		SendMessage(GetDlgItem(hDlg, IDC_RADIO_RESET), BM_SETCHECK, BST_CHECKED, NULL);
		
		return TRUE;


	
		
	case WM_COMMAND:

		radioB_ = LOWORD(wp);
	    select_ = SendMessage(GetDlgItem(hDlg, IDC_COMBO2), CB_GETCURSEL, 0, 0);

	}
	return FALSE;


}


