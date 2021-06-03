#pragma once
#include <vector>
#include "utils\ArrayBase.h"
#include "math\Vector.hpp"
using namespace std;

#define TYPE_PLAYER 0x100
#define TYPE_ITEM	0x101

#define TYPE_MODEL1	0x200	
#define TYPE_MODEL2	0x201
#define TYPE_MODEL3	0x202
#define TYPE_MODEL4	0x203
#define TYPE_MODEL5	0x204
#define TYPE_MODEL6	0x205
#define TYPE_MODEL7	0x206
#define TYPE_MODEL8	0x207
#define TYPE_MODEL9	0x208


#define ITEM_WEAPONS		0x300	
#define ITEM_AMMO			0x301
#define ITEM_HEALING		0x302
#define ITEM_SHIELD			0x303
#define ITEM_GRENADES		0x304
#define ITEM_SCOPES			0x305
#define ITEM_ATTACHMENTS	0x306

typedef struct
{
	DWORD64 dwPtr;
	DWORD id;
	DWORD nameid;
	bool isAlive;
	int health;
	int maxhealth;
	int shieldhealth;
	int shieldhealthmax;
	int teamid;
	string name;
	Vector orgin;
	Vector head;
	int type;
	int modeltype;
	int itemtpye;
}ACTOR_INFO, *PACTOR_INFO;


typedef struct
{
	int head;				//ͷ
	int neck;				//����
	int abdomen;			//����
	int Left_shoulder;		//���
	int Right_shoulder;		//�Ҽ�
	int Left_arm;			//���
	int Right_arm;			//�ұ�
	int Left_arm_1;			//���1
	int Right_arm_1;		//�ұ�1
	int Left_arm_2;			//���2
	int Right_arm_2;		//�ұ�2
	int Left_knee;			//��ϥ��
	int Right_knee;			//��ϥ��
	int Left_foot;			//���
	int Right_foot;			//�ҽ�
}BONEIDBYMODEL;


enum BoneId {
	head = 10,	//	ͷ
	neck = 9,	//	����
	Left_shoulder = 39,//���
	Right_shoulder = 14,//�Ҽ�
	Left_clavicle = 38,//������
	Right_clavicle = 13,//������
	chest = 7,//��
	abdomen = 6,//����
	perineum = 5,//����
	Left_arm = 41,//���
	Left_arm_1 = 43,//���1
	Left_arm_2 = 68,//���2
	Left_arm_3 = 45,//���3
	Right_arm = 16,//�ұ�
	Right_arm_1 = 18,//�ұ�1
	Right_arm_2 = 63,//�ұ�2
	Right_arm_3 = 20,//�ұ�3
	Left_knee = 70,//��ϥ��
	Right_knee = 65,//��ϥ��
	Left_foot = 71,//���
	Left_foot_1 = 72,//���1
	Right_foot = 66,//�ҽ�
	Right_foot_1 = 67,//�ҽ�1
};//22



class CActorMgr
{
public:
	CActorMgr();
	~CActorMgr();

	//	void GetBone(DWORD64 dwEntry, Vector &EntityPos, Vector *pOutBoneArry);
public:
	void Update();
	ACTOR_INFO *GetLocalPlayer();
	TArrayMgr<ACTOR_INFO> *Get() { return &m_List; }
	DWORD64 GetWeaponPtr();
	int GetItemTpye(int id);
private:

	TArrayMgr<ACTOR_INFO> m_List;
};
