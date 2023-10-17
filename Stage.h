#pragma once
#include "Engine/GameObject.h"
#include "windows.h"
#include <iostream>
#include <string>

namespace {
    const int MODEL_NUM{ 5 };
    const int XSIZE{ 15 };
    const int ZSIZE{ 15 };

    enum BLOCKTYPE  //各種画像
    {
        DEFAULT = 0,    //0 
        BRICK,          //1
        GRASS,          //2
        SAND,           //3
        WATER           //4
    };
}

//◆◆◆を管理するクラス
class Stage : public GameObject
{
    int hModel_[MODEL_NUM];    //モデル番号(配列型)
    //int table_[XSIZE][ZSIZE];
    struct
    {
        int type;
        int height;
    } 
    
    table_[15][15];

    int mode_;      //0:上げる　1：下げる　2：種類を変える
    int select_;    //種類

    int radioB_;    //コントロールIDのやつー
    int menuID_;


    char fileName[MAX_PATH] = "無題map.txt";  //ファイル名を入れる変数
    OPENFILENAME ofn;
    BOOL selFile;
    HANDLE hFile;
    DWORD bytes;

public:
    void SetBlock(int _x, int _z, BLOCKTYPE _type);
    void SetBlockHeight(int _x, int _z, int _height);
    //コンストラクタ
    Stage(GameObject* parent);

    //デストラクタ
    ~Stage();

    //初期化
    void Initialize() override;

    //更新
    void Update() override;

    //描画
    void Draw() override;

    //開放
    void Release() override;

    //セーブ
    void Save();
    void Load();
    BOOL DialogProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);
};