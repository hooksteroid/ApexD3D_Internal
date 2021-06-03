#include "Cheat.h"
#include "ActorMgr.h"
#include "math\Vector2D.hpp"
#include "Color.h"
#include "hooks.h"
#include "D3dDraw.h"

//typedef bool(__fastcall *_GameWorldToSceen)(Vector location, Vector2D &out);
//_GameWorldToSceen world2screen;
Draw* D = new Draw;
CCheats *GetCheat()
{
	static CCheats m;
	return &m;
}

CCheats::CCheats()
{

	m_pActor = new CActorMgr;
}

CCheats::~CCheats()
{
	delete m_pActor;
}


void AddItemName(int id, char *pCN, char *pEN, char *pTexture)
{
	ITEM_NAME in;
	lstrcpyA(in.szCN, pCN);
	lstrcpyA(in.szEN, pEN);
	lstrcpyA(in.szTexture, pTexture);
	g_ItemHash[id] = in;
}

void InitItemName()
{
	//SEProtectStartMutation();
	g_ItemHash.clear();
	//Weapons
	AddItemName(GOLD_KRABER, "���ױ����ѻ�ǹ", "Kraber", "��");		//1
	AddItemName(GOLD_MASTIFF, "��Ȯ����ǹ", "Mastiff", "��");			//1
	AddItemName(GOLD_LSTAR, "L-Star EMG", "L-Star EMG", "A");
	AddItemName(GOLD_HAVOC, "���ֿ�", "HowGun", "");
	AddItemName(DEVOTION, "רע���ǹ", "Devotion", "��");
	AddItemName(TRIPLE_TAKE, "����ʽ�ѻ�ǹ", "Triple Take", "��");
	AddItemName(GOLD_TRIPLE_TAKE, "�ƽ�����ʽ�ѻ�ǹ", "Triple Take", "��");
	AddItemName(FLATLINE, "VK-47ƽ�в�ǹ", "Flatline", "");
	AddItemName(GOLD_FLATLINE, "�ƽ�VK-47ƽ�в�ǹ", "Flatline", "");
	AddItemName(HEMLOCK, "��ķ���ͻ����ǹ", "Hemlok", "��");		//1
	AddItemName(G7_SCOUT, "G7���ǹ", "G7 Scout", "��");
	AddItemName(GOLD_G7_SCOUT, "�ƽ�G7���ǹ", "G7 Scout", "��");
	AddItemName(ALTERNATOR, "ת���߳��ǹ", "Alternator", "��");
	AddItemName(GOLD_ALTERNATOR, "�ƽ�ת���߳��ǹ", "Alternator", "��");
	AddItemName(R_99, "R-99���ǹ", "R-99", "��");
	AddItemName(PROWLER, "ǱϮ���ǹ", "Prowler", "˪");
	AddItemName(GOLD_PROWLER, "�ƽ�ǱϮ���ǹ", "Prowler", "˪");
	AddItemName(LONGBOW, "������ȷ��ǹ", "Longbow", "��");
	AddItemName(GOLD_LONGBOW, "�ƽ𳤹���ȷ��ǹ", "Longbow", "��");
	AddItemName(CHARGE_RIFLE, "���ܲ�ǹ", "EntGun", "");
	AddItemName(GOLD_CHARGE_RIFLE, "�ƽ���ܲ�ǹ", "EntGun", "");
	AddItemName(SPITFIRE, "M600������ǹ", "Spitfire", "��");
	AddItemName(R_301, "R-301����ǹ", "R-301", "��");
	AddItemName(EVA_8_AUTO, "EVA-8�Զ�����ǹ", "EVA-8 Auto", "��");	//1
	AddItemName(PEACEKEEPER, "��ƽ����������ǹ", "Peacekeeper", "��");
	AddItemName(GOLD_PEACEKEEPER, "�ƽ��ƽ����������ǹ", "Peacekeeper", "��");
	AddItemName(MOZAMBIQUE, "Īɣ�ȿ�����ǹ", "Mozambique", "Ϊ");
	AddItemName(GOLD_MOZAMBIQUE, "�ƽ�Īɣ�ȿ�����ǹ", "Mozambique", "Ϊ");
	AddItemName(WINGMAN, "������ǹ", "Wingman", "¶");
	AddItemName(GOLD_WINGMAN, "�ƽ�����ǹ", "Wingman", "¶");
	AddItemName(P2020, "P2020��ǹ", "P2020", "��");
	AddItemName(GOLD_P2020, "�ƽ�P2020��ǹ", "P2020", "��");
	AddItemName(RE_45, "RE-45�Զ���ǹ", "RE-45", "��");
	AddItemName(GOLD_RE_45, "�ƽ�RE-45�Զ���ǹ", "RE-45", "��");
	
	//Ammo
	AddItemName(LIGHT_ROUNDS, "���͵�ҩ", "Light Rounds", "");
	AddItemName(ENERGY_AMMO, "������ҩ", "Energy Ammo", "");
	AddItemName(SHOTGUN_SHELLS, "������ҩ", "Shotgun Shells", "");
	AddItemName(HEAVY_ROUNDS, "���͵�ҩ", "Heavy Rounds", "");

	//Healing Items
	AddItemName(ULTIMATE_ACCELERANT, "���м��ټ�", "Ultimate Accelerant", "");
	AddItemName(PHOENIX_KIT, "������ư�", "Phoenix Kit", "��");
	AddItemName(MED_KIT, "ҽ����", "Med Kit", "ˮ");
	AddItemName(SYRINGE, "ע����", "Syringe", "��");
	AddItemName(SHIELD_BATTERY, "���ͻ��ܵ��", "Shield Battery", "��");
	AddItemName(SHIELD_CELL, "С�ͻ��ܵ��", "Shield Cell", "��");

	//Shield Items

	AddItemName(HELMET_LV1, "ͷ��(lv1)", "Helmet Level 1", "�� lv1");
	AddItemName(HELMET_LV2, "ͷ��(lv2)", "Helmet Level 2", "�� lv2");
	AddItemName(HELMET_LV3, "ͷ��(lv3)", "Helmet Level 3", "�� lv3");
	AddItemName(HELMET_LV4, "ͷ��(lv4)", "Helmet Level 4", "�� lv4");
	AddItemName(BODY_ARMOR_LV1, "������(lv1)", "Body Armor Level 1", "�� lv1");
	AddItemName(BODY_ARMOR_LV2, "������(lv2)", "Body Armor Level 2", "�� lv2");
	AddItemName(BODY_ARMOR_LV3, "������(lv3)", "Body Armor Level 3", "�� lv3");
	AddItemName(BODY_ARMOR_LV4, "������(lv4)", "Body Armor Level 4", "�� lv4");
	AddItemName(KNOCKDOWN_SHIELD_LV1, "��������(lv1)", "Knockdown Shield Level 1", "�� lv1");
	AddItemName(KNOCKDOWN_SHIELD_LV2, "��������(lv2)", "Knockdown Shield Level 2", "�� lv2");
	AddItemName(KNOCKDOWN_SHIELD_LV3, "��������(lv3)", "Knockdown Shield Level 3", "�� lv3");
	AddItemName(KNOCKDOWN_SHIELD_LV4, "��������(lv4)", "Knockdown Shield Level 4", "�� lv4");
	AddItemName(BACKPACK_LV1, "����(lv1)", "Backpack Level 1", "�� lv1");
	AddItemName(BACKPACK_LV2, "����(lv2)", "Backpack Level 2", "�� lv2");
	AddItemName(BACKPACK_LV3, "����(lv3)", "Backpack Level 3", "�� lv3");
	AddItemName(BACKPACK_LV4, "����(lv4)", "Backpack Level 4", "�� lv4");

	//Grenades
	AddItemName(THERMITE_GRENADE, "���ȼ�����", "Thermite Grenade", "");
	AddItemName(FRAG_GRENADE, "��Ƭ����", "Frag Grenade", "��");
	AddItemName(ARC_STAR, "����", "Arc Star", "��");


	//Attachment Scopes
	AddItemName(HCOG_CLASSIC, "����ȫϢ", "1x HCOG (Classic)", "ӯ");
	AddItemName(HCOG_BRUISER, "2��ȫϢ", "2x HCOG (Bruiser)", "��");
	AddItemName(HOLO, "������Ӱ", "1x Holo", "�");
	AddItemName(VARIABLE_HOLO, "1-2����Ӱ", "1x-2x Variable Holo", "��");
	AddItemName(DIGITAL_THREAT, "1�����ֻ���", "1x Digital Threat", "��");
	AddItemName(HCOG_RANGER, "3��ȫϢ", "3x HCOG (Ranger)", "��");
	AddItemName(VARIABLE_AOG, "2-4����׼��", "2x-4x Variable AOG", "��");
	AddItemName(SNIPER, "6���ѻ���׼��", "6x Sniper", "��");
	AddItemName(VARIABLE_SNIPER, "4-8����׼��", "4x-8x Variable Sniper", "��");
	AddItemName(DIGITAL_SNIPER_THREAT, "4-10����׼��", "4x-10x Digital Sniper Threat", "��");

	//Attachments
	AddItemName(BARREL_STABILIZER_LV1, "ǹ���ȶ���(lv1)", "Barrel Stabilizer Level 1", "�� lv1");
	AddItemName(BARREL_STABILIZER_LV2, "ǹ���ȶ���(lv2)", "Barrel Stabilizer Level 2", "�� lv2");
	AddItemName(BARREL_STABILIZER_LV3, "ǹ���ȶ���(lv3)", "Barrel Stabilizer Level 3", "�� lv3");
	AddItemName(BARREL_STABILIZER_LV4, "ǹ���ȶ���(lv4)", "Barrel Stabilizer Level 4", "�� lv4");
	AddItemName(LIGHT_MAGAZINE_LV1, "�ӳ�ʽ���͵�ϻlv1)", "Light Magazine Level 1", "�� lv1");
	AddItemName(LIGHT_MAGAZINE_LV2, "�ӳ�ʽ���͵�ϻ(lv2)", "Light Magazine Level 2", "�� lv2");
	AddItemName(LIGHT_MAGAZINE_LV3, "�ӳ�ʽ���͵�ϻ(lv3)", "Light Magazine Level 3", "�� lv3");
	AddItemName(HEAVY_MAGAZINE_LV1, "�ӳ�ʽ���͵�ϻ(lv1)", "Heavy Magazine Level 1", "�� lv1");
	AddItemName(HEAVY_MAGAZINE_LV2, "�ӳ�ʽ���͵�ϻ(lv2)", "Heavy Magazine Level 2", "�� lv2");
	AddItemName(HEAVY_MAGAZINE_LV3, "�ӳ�ʽ���͵�ϻ(lv3)", "Heavy Magazine Level 3", "�� lv3");

	AddItemName(ENERGY_MAGAZINE_LV1, "�ӳ�ʽ������ϻ(lv1)", "ENERGY Magazine Level 1", "�� lv1");
	AddItemName(ENERGY_MAGAZINE_LV2, "�ӳ�ʽ������ϻ(lv2)", "ENERGY Magazine Level 2", "�� lv2");
	AddItemName(ENERGY_MAGAZINE_LV3, "�ӳ�ʽ������ϻ(lv3)", "ENERGY Magazine Level 3", "�� lv3");

	AddItemName(SHOTGUN_BOLT_LV1, "����ǹǹ˨(lv1)", "Shotgun Bolt Level 1", "�� lv1");
	AddItemName(SHOTGUN_BOLT_LV2, "����ǹǹ˨(lv2)", "Shotgun Bolt Level 2", "�� lv2");
	AddItemName(SHOTGUN_BOLT_LV3, "����ǹǹ˨(lv3)", "Shotgun Bolt Level 3", "�� lv3");
	AddItemName(STANDARD_STOCK_LV1, "��׼ǹ��(lv1)", "Standard Stock Level 1", "�� lv1");
	AddItemName(STANDARD_STOCK_LV2, "��׼ǹ��(lv2)", "Standard Stock Level 2", "�� lv2");
	AddItemName(STANDARD_STOCK_LV3, "��׼ǹ��(lv3)", "Standard Stock Level 3", "�� lv3");
	AddItemName(SNIPER_STOCK_LV1, "�ѻ�ǹǹ��(lv1)", "Sniper Stock Level 1", "�� lv1");
	AddItemName(SNIPER_STOCK_LV2, "�ѻ�ǹǹ��(lv2)", "Sniper Stock Level 2", "�� lv2");
	AddItemName(SNIPER_STOCK_LV3, "�ѻ�ǹǹ��(lv3)", "Sniper Stock Level 3", "�� lv3");
	AddItemName(TURBOCHARGER, "������ѹ��", "Turbocharger", "��");
	AddItemName(SELECTFIRE_RECEIVER, "ѡ�����ģʽ��", "Selectfire Receiver", "��");
	AddItemName(PRECISION_CHOKE, "��׼����", "Precision Choke", "��");
	AddItemName(anvilreceiver, "Disruptor Rounds", "Precision Choke", "��");
	AddItemName(doubletaptrigger, "˫����", "doubletaptrigger", "��");
	AddItemName(HAMMERPOINT_ROUNDS, "Hammerpoint Rounds", "Hammerpoint Rounds", "��");
	//AddItemName(89, "��������", "Skullpiercer Rifling", "��");
	//�� ������
	//SEProtectEnd();
}

BONEIDBYMODEL *GetBoneID(int ModelType)
{
	//SEProtectStartMutation();
	static BONEIDBYMODEL bone;

	if (ModelType == TYPE_MODEL1)				//ѰѪ��Ȯ
	{
		bone.head = 10;					//ͷ
		bone.neck = 9;					//����
		bone.abdomen = 5;				//����
		bone.Left_shoulder = 49;		//���
		bone.Right_shoulder = 16;		//�Ҽ�
		bone.Left_arm = 51;				//���
		bone.Right_arm = 18;			//�ұ�
		bone.Left_arm_1 = 54;			//���1
		bone.Right_arm_1 = 24;			//�ұ�1
		bone.Left_arm_2 = 93;			//���2
		bone.Right_arm_2 = 88;			//�ұ�2
		bone.Left_knee = 94;			//��ϥ��
		bone.Right_knee = 90;			//��ϥ��
		bone.Left_foot = 68;			//���
		bone.Right_foot = 91;			//�ҽ�

		return &bone;
	}

	else if (ModelType == TYPE_MODEL2)				//ֱ������
	{
		bone.head = 10;					//ͷ
		bone.neck = 9;					//����
		bone.abdomen = 5;				//����
		bone.Left_shoulder = 42;		//���
		bone.Right_shoulder = 15;		//�Ҽ�
		bone.Left_arm = 46;				//���
		bone.Right_arm = 19;			//�ұ�
		bone.Left_arm_1 = 49;			//���1
		bone.Right_arm_1 = 21;			//�ұ�1
		bone.Left_arm_2 = 78;			//���2
		bone.Right_arm_2 = 73;			//�ұ�2
		bone.Left_knee = 79;			//��ϥ��
		bone.Right_knee = 74;			//��ϥ��
		bone.Left_foot = 81;			//���
		bone.Right_foot = 76;			//�ҽ�

		return &bone;
	}

	else if (ModelType == TYPE_MODEL3)				//����
	{
		bone.head = 10;					//ͷ
		bone.neck = 9;					//����
		bone.abdomen = 5;				//����
		bone.Left_shoulder = 39;		//���
		bone.Right_shoulder = 14;		//�Ҽ�
		bone.Left_arm = 41;				//���
		bone.Right_arm = 16;			//�ұ�
		bone.Left_arm_1 = 43;			//���1
		bone.Right_arm_1 = 18;			//�ұ�1
		bone.Left_arm_2 = 68;			//���2
		bone.Right_arm_2 = 63;			//�ұ�2
		bone.Left_knee = 70;			//��ϥ��
		bone.Right_knee = 65;			//��ϥ��
		bone.Left_foot = 71;			//���
		bone.Right_foot = 66;			//�ҽ�

		return &bone;
	}

	else if (ModelType == TYPE_MODEL4)				//̽·��
	{
		bone.head = 10;					//ͷ
		bone.neck = 9;					//����
		bone.abdomen = 5;				//����
		bone.Left_shoulder = 27;		//���
		bone.Right_shoulder = 14;		//�Ҽ�
		bone.Left_arm = 29;				//���
		bone.Right_arm = 16;			//�ұ�
		bone.Left_arm_1 = 30;			//���1
		bone.Right_arm_1 = 17;			//�ұ�1
		bone.Left_arm_2 = 70;			//���2
		bone.Right_arm_2 = 66;			//�ұ�2
		bone.Left_knee = 71;			//��ϥ��
		bone.Right_knee = 67;			//��ϥ��
		bone.Left_foot = 72;			//���
		bone.Right_foot = 68;			//�ҽ�

		return &bone;
	}

	else if (ModelType == TYPE_MODEL5)				//����С��(��Ҫ�ռ�)
	{
		bone.head = 10;					//ͷ
		bone.neck = 9;					//����
		bone.abdomen = 5;				//����
		bone.Left_shoulder = 44;		//���
		bone.Right_shoulder = 15;		//�Ҽ�
		bone.Left_arm = 46;				//���
		bone.Right_arm = 17;			//�ұ�
		bone.Left_arm_1 = 48;			//���1
		bone.Right_arm_1 = 19;			//�ұ�1
		bone.Left_arm_2 = 68;			//���2
		bone.Right_arm_2 = 40;			//�ұ�2
		bone.Left_knee = 92;			//��ϥ��
		bone.Right_knee = 82;			//��ϥ��
		bone.Left_foot = 89;			//���
		bone.Right_foot = 84;			//�ҽ�
		return &bone;
	}

	else if (ModelType == TYPE_MODEL6)				//����
	{
		bone.head = 10;					//ͷ
		bone.neck = 9;					//����
		bone.abdomen = 5;				//����
		bone.Left_shoulder = 40;		//���
		bone.Right_shoulder = 15;		//�Ҽ�
		bone.Left_arm = 42;				//���
		bone.Right_arm = 17;			//�ұ�
		bone.Left_arm_1 = 45;			//���1
		bone.Right_arm_1 = 20;			//�ұ�1
		bone.Left_arm_2 = 96;			//���2
		bone.Right_arm_2 = 91;			//�ұ�2
		bone.Left_knee = 97;			//��ϥ��
		bone.Right_knee = 92;			//��ϥ��
		bone.Left_foot = 99;			//���
		bone.Right_foot = 94;			//�ҽ�

		return &bone;
	}

	else if (ModelType == TYPE_MODEL7)				//����޶�
	{
		bone.head = 10;					//ͷ
		bone.neck = 9;					//����
		bone.abdomen = 5;				//����
		bone.Left_shoulder = 43;		//���
		bone.Right_shoulder = 15;		//�Ҽ�
		bone.Left_arm = 46;				//���
		bone.Right_arm = 18;			//�ұ�
		bone.Left_arm_1 = 49;			//���1
		bone.Right_arm_1 = 20;			//�ұ�1
		bone.Left_arm_2 = 86;			//���2
		bone.Right_arm_2 = 81;			//�ұ�2
		bone.Left_knee = 87;			//��ϥ��
		bone.Right_knee = 82;			//��ϥ��
		bone.Left_foot = 89;			//���
		bone.Right_foot = 84;			//�ҽ�

		return &bone;
	}

	else if (ModelType == TYPE_MODEL8)				//��ʴ(��Ҫ�ռ�)
	{
		bone.head = 10;					//ͷ
		bone.neck = 9;					//����
		bone.abdomen = 5;				//����
		bone.Left_shoulder = 39;		//���
		bone.Right_shoulder = 14;		//�Ҽ�
		bone.Left_arm = 41;				//���
		bone.Right_arm = 16;			//�ұ�
		bone.Left_arm_1 = 43;			//���1
		bone.Right_arm_1 = 18;			//�ұ�1
		bone.Left_arm_2 = 68;			//���2
		bone.Right_arm_2 = 63;			//�ұ�2
		bone.Left_knee = 70;			//��ϥ��
		bone.Right_knee = 65;			//��ϥ��
		bone.Left_foot = 71;			//���
		bone.Right_foot = 66;			//�ҽ�

		return &bone;
	}

	else if (ModelType == TYPE_MODEL9)				//����
	{
		bone.head = 10;					//ͷ
		bone.neck = 9;					//����
		bone.abdomen = 5;				//����
		bone.Left_shoulder = 41;		//���
		bone.Right_shoulder = 14;		//�Ҽ�
		bone.Left_arm = 43;				//���
		bone.Right_arm = 16;			//�ұ�
		bone.Left_arm_1 = 45;			//���1
		bone.Right_arm_1 = 19;			//�ұ�1
		bone.Left_arm_2 = 89;			//���2
		bone.Right_arm_2 = 84;			//�ұ�2
		bone.Left_knee = 90;			//��ϥ��
		bone.Right_knee = 85;			//��ϥ��
		bone.Left_foot = 92;			//���
		bone.Right_foot = 87;			//�ҽ�

		return &bone;
	}
	else
	{
		bone.head = 10;					//ͷ
		bone.neck = 9;					//����
		bone.abdomen = 5;				//����
		bone.Left_shoulder = 39;		//���
		bone.Right_shoulder = 14;		//�Ҽ�
		bone.Left_arm = 41;				//���
		bone.Right_arm = 16;			//�ұ�
		bone.Left_arm_1 = 43;			//���1
		bone.Right_arm_1 = 18;			//�ұ�1
		bone.Left_arm_2 = 68;			//���2
		bone.Right_arm_2 = 63;			//�ұ�2
		bone.Left_knee = 70;			//��ϥ��
		bone.Right_knee = 65;			//��ϥ��
		bone.Left_foot = 71;			//���
		bone.Right_foot = 66;			//�ҽ�

		return &bone;
	}
	return NULL;
	//SEProtectEnd();
}

void CCheats::DrawBone(ACTOR_INFO *pAI)
{
	//SEProtectStartMutation();
	CBaseEntity* player = (CBaseEntity*)pAI->dwPtr;
	if (!player)
		return;
	Vector out[128];
	Vector2D out2D[128];
	GColor c;
	player->GetBoneMarx(out);
	c.SetGColor(0, 255, 255, 255);
	for (int i = 0; i < 128; i++)
	{
		I::Engine->W2S(out[i], out2D[i]);
	}
	BONEIDBYMODEL *bone = GetBoneID(pAI->modeltype);


	//	ͷ->����
	D->DrawLine(out2D[bone->head], out2D[bone->neck], c.GetRawGColor());

	//	��������->���Ҽ�
	D->DrawLine(out2D[bone->neck], out2D[bone->Left_shoulder], c.GetRawGColor());
	D->DrawLine(out2D[bone->neck], out2D[bone->Right_shoulder], c.GetRawGColor());

	//	���Ҽ�->���ұ�
	D->DrawLine(out2D[bone->Left_shoulder], out2D[bone->Left_arm], c.GetRawGColor());
	D->DrawLine(out2D[bone->Right_shoulder], out2D[bone->Right_arm], c.GetRawGColor());
	D->DrawLine(out2D[bone->Left_arm], out2D[bone->Left_arm_1], c.GetRawGColor());
	D->DrawLine(out2D[bone->Right_arm], out2D[bone->Right_arm_1], c.GetRawGColor());
	D->DrawLine(out2D[bone->Left_arm_1], out2D[bone->Left_arm_2], c.GetRawGColor());
	D->DrawLine(out2D[bone->Right_arm_1], out2D[bone->Right_arm_2], c.GetRawGColor());

	//	����->����
	D->DrawLine(out2D[bone->neck], out2D[bone->abdomen], c.GetRawGColor());


	//	����->����ϥ��
	D->DrawLine(out2D[bone->abdomen], out2D[bone->Left_knee], c.GetRawGColor());
	D->DrawLine(out2D[bone->abdomen], out2D[bone->Right_knee], c.GetRawGColor());

	//	����ϥ��->���ҽ�
	D->DrawLine(out2D[bone->Left_knee], out2D[bone->Left_foot], c.GetRawGColor());
	D->DrawLine(out2D[bone->Right_knee], out2D[bone->Right_foot], c.GetRawGColor());
	D->DrawLine(out2D[bone->Left_foot], out2D[bone->Left_foot], c.GetRawGColor());
	D->DrawLine(out2D[bone->Right_foot], out2D[bone->Right_foot], c.GetRawGColor());

	//SEProtectEnd();


	/*
	Vector out[128];
	GetActor()->GetBone(pAI->dwPtr, pAI->orgin, out);
	Vector2D out2D[128];

	for (int i = 0; i < 128; i++)
	{
	if (world2screen(out[i], out2D[i]))
	{
	//GColor c;
	//c.SetGColor(0, 0, 0, 220);
	//DrawString(13, Vector2D(out2D[i].x, out2D[i].y), c.GetRawGColor(), true, true, "%d", i);
	}
	}


	GColor c;
	c.SetGColor(243, 107, 114, 255);

	//	ͷ->����
	D->DrawLine(out2D[BoneId::head], out2D[BoneId::neck], c.GetRawGColor());

	//	��->��������
	D->DrawLine(out2D[BoneId::neck], out2D[BoneId::Left_clavicle], c.GetRawGColor());
	D->DrawLine(out2D[BoneId::neck], out2D[BoneId::Right_clavicle], c.GetRawGColor());

	//	��������->���Ҽ�
	D->DrawLine(out2D[BoneId::Left_clavicle], out2D[BoneId::Left_shoulder], c.GetRawGColor());
	D->DrawLine(out2D[BoneId::Right_clavicle], out2D[BoneId::Right_shoulder], c.GetRawGColor());

	//	���Ҽ�->���ұ�
	D->DrawLine(out2D[BoneId::Left_shoulder], out2D[BoneId::Left_arm], c.GetRawGColor());
	D->DrawLine(out2D[BoneId::Right_shoulder], out2D[BoneId::Right_arm], c.GetRawGColor());
	D->DrawLine(out2D[BoneId::Left_arm], out2D[BoneId::Left_arm_1], c.GetRawGColor());
	D->DrawLine(out2D[BoneId::Right_arm], out2D[BoneId::Right_arm_1], c.GetRawGColor());
	D->DrawLine(out2D[BoneId::Left_arm_1], out2D[BoneId::Left_arm_3], c.GetRawGColor());
	D->DrawLine(out2D[BoneId::Right_arm_1], out2D[BoneId::Right_arm_3], c.GetRawGColor());

	//	����->��
	D->DrawLine(out2D[BoneId::neck], out2D[BoneId::chest], c.GetRawGColor());

	//	��->����
	D->DrawLine(out2D[BoneId::chest], out2D[BoneId::abdomen], c.GetRawGColor());

	//	����->����ϥ��
	D->DrawLine(out2D[BoneId::abdomen], out2D[BoneId::Left_knee], c.GetRawGColor());
	D->DrawLine(out2D[BoneId::abdomen], out2D[BoneId::Right_knee], c.GetRawGColor());

	//	����ϥ��->���ҽ�
	D->DrawLine(out2D[BoneId::Left_knee], out2D[BoneId::Left_foot], c.GetRawGColor());
	D->DrawLine(out2D[BoneId::Right_knee], out2D[BoneId::Right_foot], c.GetRawGColor());
	D->DrawLine(out2D[BoneId::Left_foot], out2D[BoneId::Left_foot_1], c.GetRawGColor());
	D->DrawLine(out2D[BoneId::Right_foot], out2D[BoneId::Right_foot_1], c.GetRawGColor());
	*/
}

void CCheats::DrawHighlight(ACTOR_INFO *pAI, bool bIsVisible)
{
	//SEProtectStartMutation();
	static float fColor[3] = { 0.f, 1.f, 0.5f };
	if (bIsVisible)
	{
		fColor[0] = 0.f;
		fColor[1] = 250.f;
		fColor[2] = 154.f;
	}
	*(bool*)(pAI->dwPtr + 0x390) = true;
	*(bool*)(pAI->dwPtr + 0x310) = true;
	*(float*)(pAI->dwPtr + 0x2FC) = FLT_MAX;
	*(float**)(pAI->dwPtr + 0x1D0) = fColor;
	for (auto i = 0x2D0; i <= 0x2E8; i += 0x4)
		*(float*)(pAI->dwPtr + i) = FLT_MAX;
	//SEProtectEnd();
}

void BulletTrace(CBaseEntity* entity)
{
	//SEProtectStartMutation();
	Vector src3D, dst3D, forward;
	Vector2D  src, dst;
	trace_t tr;
	ZeroMemory(&tr, sizeof(tr));
	Ray_t ray;
	ZeroMemory(&ray, sizeof(ray));

	AngleVectors(entity->GetEyeAngles(), &forward);
	src3D = entity->GetBonePosition(10) - Vector(0, 0, 0);
	dst3D = src3D + (forward * Vars.Visuals.MaxSpot);
	ray.Init(src3D, dst3D);
	I::EngineTrace->TraceRay(ray, 0x640400B, &tr);
	//world2screen = (_GameWorldToSceen)(global::g_hGameImage + OFFSET::g_dwWorldToScreenEP);
	//if (!world2screen(src3D, src) || !world2screen(tr.endpos, dst))
	//	return;
	if (!I::Engine->W2S(src3D, src) || !I::Engine->W2S(tr.endpos, dst))
		return;
	GColor c;
	c.SetGColor(0, 255, 0, 255);
	D->DrawLine(src,dst, c.GetRawGColor());
	//D::DrawRect(dst.x - 3, dst.y - 3, 6, 6, color);
	//SEProtectEnd();
}
void DrawPlayerWeapon(CBaseEntity* entity)
{
	int id = entity->GetWeaponID();
	if (id == -1)
		return;
	string weapon = "unknown";
	switch (id)
	{
	case WEAPONID_G7���ǹ:weapon = "G7"; break;
	case WEAPONID_R301:weapon = "R301"; break;
	}
}
void CCheats::ESP()
{
	//SEProtectStartMutation();

//	world2screen = (_GameWorldToSceen)(global::g_hGameImage + OFFSET::g_dwWorldToScreenEP);
	ACTOR_INFO *pLocalPlayer = GetActor()->GetLocalPlayer();
	if (pLocalPlayer == NULL)
		return;
	if (!Vars.Visuals.Enabled)
		return;

	for (int i = 0; i < GetActor()->Get()->GetCount(); i++)
	{
		ACTOR_INFO *pAI = GetActor()->Get()->GetData(i);
		if (pAI == NULL)
			continue;



		//	���
		if (pAI->type == TYPE_PLAYER && Vars.Visuals.PlayerEnable)
		{
			BONEIDBYMODEL *bone = GetBoneID(pAI->modeltype);
			if ((DWORD64)G::LocalPlayer == pAI->dwPtr)	//�ж��Լ�
				continue;
			if(!pAI->isAlive)
				continue;
			float fDis = (pAI->orgin - pLocalPlayer->orgin).Length() * 0.01905f;	//����
			if (fDis > Vars.Visuals.MaxPlayerDis)
				continue;
			if (pAI->teamid == G::LocalPlayer->m_iTeamNum())
				continue;
			Vector2D screennike, screenHead;
			if (I::Engine->W2S(pAI->orgin, screennike) && I::Engine->W2S(pAI->head, screenHead))
			{
				//DbgPrintA("666666666666666666666666");
				GColor c;
				//	����
				int iMiddle = (screennike.y - screenHead.y);
				int iWidth = iMiddle / 5;
				CBaseEntity* player = (CBaseEntity*)pAI->dwPtr;
				if (Vars.Visuals.Box)
				{
					/*
					Vector2D outHead;
					if (world2screen(player->GetBonePosition(BoneId::neck), outHead))
					{
					D->DrawCircle(outHead, 2, GColor::Red().GetRawGColor(), 16, 1);
					}
					*/
					if (fDis < 300)
					{
						if (player->IsVisible(10))
							c.SetGColor(243, 107, 114, 255);
						else
							c.SetGColor(255, 255, 0);

					}
					else
					{
						c.SetGColor(0, 128, 255);
					}
					

					D->DrawBox(Vector2D(screenHead.x - iWidth, screenHead.y), Vector2D(iWidth * 2, iMiddle), c.GetRawGColor(), 2.f);



					/*
					if (pAI->teamid == pLocalPlayer->teamid)
					{
					c.SetGColor(0, 128, 255);
					D->DrawBox(Vector2D(screenHead.x - iWidth, screenHead.y), Vector2D(iWidth * 2, iMiddle), c.GetRawGColor(), 2.f);
					}
					else
					{
					c.SetGColor(243, 107, 114, 255);
					D->DrawBox(Vector2D(screenHead.x - iWidth, screenHead.y), Vector2D(iWidth * 2, iMiddle), c.GetRawGColor(), 2.f);

					}*/

				}
				//����
				if (Vars.Visuals.Skeleton)
				{
						DrawBone(pAI);
				}

				//����
				if (Vars.Visuals.Glow)
				{
					DrawHighlight(pAI, player->IsVisible(bone->head));
				}

				//	����

				if (Vars.Visuals.Name)
				{
					c.SetGColor(255, 255, 255, 220);
					D->DrawString(12, Vector2D(screennike.x, screennike.y - 35), c.GetRawGColor(), true, true, "%s", (pAI->name.c_str()));
				}

				//	��Ѫ��
				if (Vars.Visuals.Health)
				{
					c.SetGColor(0, 0, 0, 220);
					D->DrawBox(Vector2D(screenHead.x + iWidth + 1, screenHead.y), Vector2D(4, iMiddle), c.GetRawGColor(), 1);
					c.SetGColor(181, 230, 29, 220);
					//	ȡ�ٷֱ�
					float fHP = (float)pAI->health * 100 / (float)pAI->maxhealth;
					D->DrawFilledBox(Vector2D(screenHead.x + iWidth + 2, screenHead.y + 1 + (100 - fHP) / 100 * iMiddle), Vector2D(2, fHP / 100 * iMiddle - 1), c.GetRawGColor());

				}

				//������
				if (Vars.Visuals.Distance)
				{
					c.SetGColor(255, 255, 255, 220);
					D->DrawString(12, Vector2D(screennike.x, screennike.y - 45), c.GetRawGColor(), true, true, "[%.fm]", fDis);
				}


				if (Vars.Visuals.Shield)
				{
					//	������
					c.SetGColor(0, 0, 0, 220);
					D->DrawBox(Vector2D(screenHead.x + iWidth + 5, screenHead.y), Vector2D(4, iMiddle), c.GetRawGColor(), 1);
					c.SetGColor(0, 255, 255, 220);
					//	ȡ�ٷֱ�
					float fFD = (float)pAI->shieldhealth * 100 / (float)pAI->shieldhealthmax;
					D->DrawFilledBox(Vector2D(screenHead.x + iWidth + 6, screenHead.y + 1 + (100 - fFD) / 100 * iMiddle), Vector2D(2, fFD / 100 * iMiddle - 1), c.GetRawGColor());
				}


				//	����׼��
				if (Vars.Visuals.AimSpot)
					BulletTrace((CBaseEntity*)pAI->dwPtr);
			//	if (Vars.Visuals.Weapon)
			//		DrawPlayerWeapon((CBaseEntity*)pAI->dwPtr);
			}



		}
		//int centreX = m_rc.right / 2;
		//int centreY = m_rc.bottom / 2;

		//D->DrawCircle(Vector2D(centreX, centreY), Vars.Rage.FOV , GColor::White().GetRawGColor(), 16, 1);

		//	��Ʒ
		if (pAI->type == TYPE_ITEM && Vars.Visuals.ItemEnable)
		{
			float fDis = (pAI->orgin - pLocalPlayer->orgin).Length() * 0.01905f;	//����
			if (fDis > Vars.Visuals.MaxItemDis)
				continue;
			Vector2D screen;
			float FontSize = 11.f;
			if (I::Engine->W2S(pAI->orgin, screen))
			{
				GColor c;
				
				
				if (pAI->itemtpye == ITEM_WEAPONS && Vars.Visuals.ItemWeapons)	//����
				{
					c.SetGColor(34, 177, 76, 220);
					if (g_ItemHash.count(pAI->nameid))
						D->DrawString(FontSize, Vector2D(screen.x, screen.y), c.GetRawGColor(), true, true, "[%.fm]%s", fDis, g_ItemHash[pAI->nameid].szCN);
					else
					{
						D->DrawString(FontSize, Vector2D(screen.x, screen.y), c.GetRawGColor(), true, true, "[%.fm]%d", fDis, pAI->nameid);
					}
				}
				if (pAI->itemtpye == ITEM_AMMO && Vars.Visuals.ItemAmmo)	//�ӵ�
				{
					c.SetGColor(255, 127, 39, 220);
					if (g_ItemHash.count(pAI->nameid))
						D->DrawString(FontSize, Vector2D(screen.x, screen.y), c.GetRawGColor(), true, true, "[%.fm]%s", fDis, g_ItemHash[pAI->nameid].szCN);
					else
					{
						D->DrawString(FontSize, Vector2D(screen.x, screen.y), c.GetRawGColor(), true, true, "[%.fm]%d", fDis, pAI->nameid);
					}
				}

				if (pAI->itemtpye == ITEM_HEALING && Vars.Visuals.ItemHealing)	//������Ʒ
				{
					c.SetGColor(128, 255, 255, 220);
					if (g_ItemHash.count(pAI->nameid))
						D->DrawString(FontSize, Vector2D(screen.x, screen.y), c.GetRawGColor(), true, true, "[%.fm]%s", fDis, g_ItemHash[pAI->nameid].szCN);
					else
					{
						D->DrawString(FontSize, Vector2D(screen.x, screen.y), c.GetRawGColor(), true, true, "[%.fm]%d", fDis, pAI->nameid);
					}
				}

				if (pAI->itemtpye == ITEM_SHIELD && Vars.Visuals.ItemShield)	//������Ʒ
				{
					c.SetGColor(128, 0, 255, 220);
					if (g_ItemHash.count(pAI->nameid))
						D->DrawString(FontSize, Vector2D(screen.x, screen.y), c.GetRawGColor(), true, true, "[%.fm]%s", fDis, g_ItemHash[pAI->nameid].szCN);
					else
					{
						D->DrawString(FontSize, Vector2D(screen.x, screen.y), c.GetRawGColor(), true, true, "[%.fm]%d", fDis, pAI->nameid);
					}
				}

				if (pAI->itemtpye == ITEM_GRENADES && Vars.Visuals.ItemGrenades)	//Ͷ����Ʒ
				{
					c.SetGColor(255, 0, 0, 220);
					if (g_ItemHash.count(pAI->nameid))
						D->DrawString(FontSize, Vector2D(screen.x, screen.y), c.GetRawGColor(), true, true, "[%.fm]%s", fDis, g_ItemHash[pAI->nameid].szCN);
					else
					{
						D->DrawString(FontSize, Vector2D(screen.x, screen.y), c.GetRawGColor(), true, true, "[%.fm]%d", fDis, pAI->nameid);
					}
				}

				if (pAI->itemtpye == ITEM_SCOPES && Vars.Visuals.ItemScopes)	//��׼��
				{
					c.SetGColor(255, 128, 192, 220);
					if (g_ItemHash.count(pAI->nameid))
						D->DrawString(FontSize, Vector2D(screen.x, screen.y), c.GetRawGColor(), true, true, "[%.fm]%s", fDis, g_ItemHash[pAI->nameid].szCN);
					else
					{
						D->DrawString(FontSize, Vector2D(screen.x, screen.y), c.GetRawGColor(), true, true, "[%.fm]%d", fDis, pAI->nameid);
					}
				}

				if (pAI->itemtpye == ITEM_ATTACHMENTS && Vars.Visuals.ItemAttachments)	//���
				{
					c.SetGColor(255, 255, 0, 220);
					if (g_ItemHash.count(pAI->nameid))
						D->DrawString(FontSize, Vector2D(screen.x, screen.y), c.GetRawGColor(), true, true, "[%.fm]%s", fDis, g_ItemHash[pAI->nameid].szCN);
					else
					{
						D->DrawString(FontSize, Vector2D(screen.x, screen.y), c.GetRawGColor(), true, true, "[%.fm]%d", fDis, pAI->nameid);
					}
				}
			}
		}
	}
	//SEProtectEnd();
}
void CCheats::Process()
{
//	//SEProtectStartMutation();
	if (I::Engine->IsInGame())
	{
		GetClientRect(global::g_hGameWnd, &m_rc);
		G::LocalPlayer = (CBaseEntity*)I::ClientEntList->GetClientEntity(I::Engine->GetLocalPlayerID());
		//G::LocalPlayer->GetEyePosition();
		DbgPrintA("LocalPlayer: %d %d %f ", G::LocalPlayer->IsReloading(), G::LocalPlayer->m_ammoInStockpile(), G::LocalPlayer->GetbulletSpeed());
		//DbgPrintA("LocalPlayer: %f %f %f ", G::LocalPlayer->GetEyePosition().x, G::LocalPlayer->GetEyePosition().y, G::LocalPlayer->GetEyePosition().z);
		//DbgPrintA("LocalPlayer2: %f %f %f ", G::LocalPlayer->GetViewAngles().x, G::LocalPlayer->GetViewAngles().y, G::LocalPlayer->GetViewAngles().z);
		GetActor()->Update();
		ESP();
	}
	else
		E::RageBot->m_ent = NULL;

//	//SEProtectEnd();
}
