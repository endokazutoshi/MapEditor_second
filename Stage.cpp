//Stage.cpp
#include "Stage.h"
#include "resource.h"
#include "Engine/Model.h"
#include "Engine/Input.h"
#include "Engine/Camera.h"
#include "Engine/Fbx.h"
#include <iostream>
#include <sstream>


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
					 if (radioB_ == IDC_RADIO_RESET)//ワンクリックでリセット
					 {
						 for (int x = 0; x < XSIZE; x++)
						 {
							 for (int z = 0; z < ZSIZE; z++)
							 {
								 SetBlock(x,z, DEFAULT);//ブロックのタイプも全て0にする
								 SetBlockHeight(x, z, 0);//高さも全て0
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
					 if (radioB_ == IDC_RADIO_RANDOMTERRAIN)//ランダム生成
					 {
						 for (int z = 0; z < ZSIZE; z++) 
						 {
							 for (int x = 0; x < XSIZE; x++) 
							 {
								 SetBlockHeight(x, z, rand() % 4);
							 }
						 }
					 }
					 if (radioB_ == IDC_RADIO_RONDOMCOLLAR)
					 {
						 for (int z = 0; z < ZSIZE; z++)
						 {
							 for (int x = 0; x < XSIZE; x++)
							 {
								 SetBlock(x, z, (BLOCKTYPE)(rand() % 5));
							 }
						 }
					 }
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
	OPENFILENAME ofn;
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
					std::stringstream typeStream;
					typeStream << table_[x][z].type;

					std::stringstream heightStream;
					heightStream << table_[x][z].height;

					data += typeStream.str() + " " + heightStream.str() + "\n";
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
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.lpstrFilter = TEXT("テキストファイル (*.txt)\0*.txt\0すべてのファイル (*.*)\0*.*\0");
	ofn.lpstrFile = fileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_FILEMUSTEXIST;

	selFile = GetOpenFileName(&ofn);

	if (selFile == FALSE)
		return;

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
		std::wcout << L"ファイルオープンに失敗 " << GetLastError() << std::endl;
		return;
	}

	DWORD fileSize = GetFileSize(hFile, NULL);
	char* fileData = new char[fileSize];
	
	bytes = 0; 
	res = ReadFile(
		hFile,
		fileData,
		(DWORD)fileSize,
		&bytes,
		NULL);

	if (res == FALSE)
	{
		std::wcout << L"ファイル読み込みに失敗" << GetLastError() << std::endl;
		CloseHandle(hFile);
		return;
	}

	for (int x = 0; x < XSIZE; x++)
	{
		for (int z = 0; z < ZSIZE; z++)
		{
			int type, height;
			if (sscanf_s(fileData, "%d %d", &type, &height) == 2)//sscanf_sで文字列内の指定しに対応するデータを格納する
			{
				table_[x][z].type = type;
				table_[x][z].height = height;
			}

			// 次のデータに進む
			fileData = strchr(fileData, '\n');//指定された文字が文字列内で最初に出現するかどうかを見つけるるる

			if (fileData != nullptr)
				++fileData; // 改行文字をスキップ
		}
	}

	CloseHandle(hFile);
	std::string fileContent(fileData, fileSize);
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
		SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_ADDSTRING, 0, (LPARAM)"デフォルト");
		SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_ADDSTRING, 0, (LPARAM)"レンガ");
		SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_ADDSTRING, 0, (LPARAM)"草原");
		SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_ADDSTRING, 0, (LPARAM)"砂地");
		SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_ADDSTRING, 0, (LPARAM)"水");
		SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_GETCURSEL, 0, 0);

		SendMessage(GetDlgItem(hDlg, IDC_RADIO_ALLUP), BM_SETCHECK, BST_CHECKED, NULL);
		SendMessage(GetDlgItem(hDlg, IDC_COMBO3), CB_ADDSTRING, 0, (LPARAM)"デフォルト");
		SendMessage(GetDlgItem(hDlg, IDC_COMBO3), CB_ADDSTRING, 0, (LPARAM)"レンガ");
		SendMessage(GetDlgItem(hDlg, IDC_COMBO3), CB_ADDSTRING, 0, (LPARAM)"草原");
		SendMessage(GetDlgItem(hDlg, IDC_COMBO3), CB_ADDSTRING, 0, (LPARAM)"砂地");
		SendMessage(GetDlgItem(hDlg, IDC_COMBO3), CB_ADDSTRING, 0, (LPARAM)"水");
		SendMessage(GetDlgItem(hDlg, IDC_COMBO3), CB_GETCURSEL, 0, 0);

		return TRUE;

	case WM_COMMAND:

		radioB_ = LOWORD(wp);
	    select_ = SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_GETCURSEL, 0, 0);
		allSelect_ = SendMessage(GetDlgItem(hDlg, IDC_COMBO3), CB_GETCURSEL, 0, 0);

	}
	return FALSE;


}

