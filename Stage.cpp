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
					else if (radioB_ == IDC_RADIO_DOWN)
					{
						if (table_[x][z].height >= 1)//テーブルの高さが１つより上だったら...
							table_[x][z].height--;
					}
					//ラジオボタンの選択
					else if (radioB_ == IDC_RADIO_CHANGE)
					{
						SetBlock(x, z, (BLOCKTYPE)(select_));
					}
					if (radioB_ == IDC_RADIO_SELECTION)
					{
						table_[x][y].height++;
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


	//「ファイルを保存」ダイアログの設定
								//名前をつけて保存ダイアログの設定用構造体
	ZeroMemory(&ofn, sizeof(ofn));            	//構造体初期化
	ofn.lStructSize = sizeof(OPENFILENAME);   	//構造体のサイズ
	ofn.lpstrFilter = TEXT("マップデータ(*.map)\0*.map\0")        //─┬ファイルの種類
		TEXT("すべてのファイル(*.*)\0*.*\0\0");     //─┘
	ofn.lpstrFile = fileName;               	//ファイル名
	ofn.nMaxFile = MAX_PATH;               	//パスの最大文字数
	ofn.Flags = OFN_OVERWRITEPROMPT;   		//フラグ（同名ファイルが存在したら上書き確認）
	ofn.lpstrDefExt = "map";                  	//デフォルト拡張子

	//「ファイルを保存」ダイアログ

	selFile = GetSaveFileName(&ofn);

	//キャンセルしたら中断
	if (selFile == FALSE) return;

	hFile = CreateFile(
		fileName,    //ファイル名
		GENERIC_WRITE,  //アクセスモード
		0,
		NULL,
		CREATE_ALWAYS,     //作成方法
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	std::string data;
	for (int x = 0; x < XSIZE; x++)
	{
		for (int z = 0; z < ZSIZE; z++)
		{
			if (x != XSIZE - 1)
			{
				data += table_[x][z].type + " ";
				data += table_[x][z].height + " ";
			}
			else
			{
				data += table_[x][z].type + " ";
				data += table_[x][z].height + "\n";
			}
		
		}
	}


		//data.length()
		bytes = 0;
		WriteFile(
			hFile,              //ファイルハンドル
			data.c_str(),          //保存したい文字列
			(DWORD)data.length(),                  //保存する文字数
			&bytes,             //保存したサイズ
			NULL
		);

		CloseHandle(hFile);
	
}
void Stage::Load()
{
	//「ファイルを保存」ダイアログの設定
	ZeroMemory(&ofn, sizeof(ofn));            	//構造体初期化
	ofn.lStructSize = sizeof(OPENFILENAME);   	//構造体のサイズ
	ofn.lpstrFilter = TEXT("マップデータ(*.map)\0*.map\0")        //─┬ファイルの種類
		TEXT("すべてのファイル(*.*)\0*.*\0\0");     //─┘
	ofn.lpstrFile = fileName;               	//ファイル名
	ofn.nMaxFile = MAX_PATH;               	//パスの最大文字数
	ofn.Flags = OFN_FILEMUSTEXIST;   		//フラグ（同名ファイルが存在したら上書き確認）
	ofn.lpstrDefExt = "map";                  	//デフォルト拡張子

	//「ファイルを保存」ダイアログ
	selFile = GetOpenFileName(&ofn);

	//キャンセルしたら中断
	if (selFile == FALSE) return;
	hFile = CreateFile(
		fileName,                 //ファイル名
		GENERIC_READ,           //アクセスモード（読み込み）
		0,                      //共有（なし）
		NULL,                   //セキュリティ属性（継承しない）
		OPEN_EXISTING,           //作成方法
		FILE_ATTRIBUTE_NORMAL,  //属性とフラグ（設定なし）
		NULL);                  //拡張属性（なし）

	//ファイルのサイズを取得
	DWORD fileSize = GetFileSize(hFile, NULL);

	//ファイルのサイズ分メモリを確保
	char* fileData = new char[fileSize];

	bytes = 0; //読み込み位置

	ReadFile(
		hFile,     //ファイルハンドル
		fileData,      //データを入れる変数
		fileSize,  //読み込むサイズ
		&bytes,  //読み込んだサイズ
		NULL);     //オーバーラップド構造体（今回は使わない）
	CloseHandle(hFile);
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
		
		return TRUE;


	
		
	case WM_COMMAND:

		radioB_ = LOWORD(wp);
	    select_ = SendMessage(GetDlgItem(hDlg, IDC_COMBO2), CB_GETCURSEL, 0, 0);

	}
	return FALSE;


}

