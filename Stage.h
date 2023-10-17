#pragma once
#include "Engine/GameObject.h"
#include "windows.h"
#include <iostream>
#include <string>

namespace {
    const int MODEL_NUM{ 5 };
    const int XSIZE{ 15 };
    const int ZSIZE{ 15 };

    enum BLOCKTYPE  //�e��摜
    {
        DEFAULT = 0,    //0 
        BRICK,          //1
        GRASS,          //2
        SAND,           //3
        WATER           //4
    };
}

//���������Ǘ�����N���X
class Stage : public GameObject
{
    int hModel_[MODEL_NUM];    //���f���ԍ�(�z��^)
    //int table_[XSIZE][ZSIZE];
    struct
    {
        int type;
        int height;
    } 
    
    table_[15][15];

    int mode_;      //0:�グ��@1�F������@2�F��ނ�ς���
    int select_;    //���

    int radioB_;    //�R���g���[��ID�̂�[
    int menuID_;


    char fileName[MAX_PATH] = "����map.txt";  //�t�@�C����������ϐ�
    OPENFILENAME ofn;
    BOOL selFile;
    HANDLE hFile;
    DWORD bytes;

public:
    void SetBlock(int _x, int _z, BLOCKTYPE _type);
    void SetBlockHeight(int _x, int _z, int _height);
    //�R���X�g���N�^
    Stage(GameObject* parent);

    //�f�X�g���N�^
    ~Stage();

    //������
    void Initialize() override;

    //�X�V
    void Update() override;

    //�`��
    void Draw() override;

    //�J��
    void Release() override;

    //�Z�[�u
    void Save();
    void Load();
    BOOL DialogProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);
};