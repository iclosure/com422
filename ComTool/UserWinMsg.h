
#ifndef USER_WINDOWS_MESSAGE_H
#define USER_WINDOWS_MESSAGE_H

#define WM_USER0				(WM_USER  + 001)	// ��Ϊһ���Ի����ڲ���Ϣ��	// (WM_USER + 001) ~ (WM_USER + 200)
#define WM_USER1				(WM_USER0 + 201)	// ��Ϊ������Ϣ��				// (WM_USER + 201) ~ (WM_USER + 400)
#define WM_USER2				(WM_USER1 + 201)	// ��Ϊָ��Ӧ����Ϣ��			// (WM_USER + 401) ~ (WM_USER + 600)

// �Զ�����̿ո����Ϣ
#define WM_KEY_SPACE			(WM_USER1 + 0)	// SendMessage()��ʽ
// �Զ�����Ϣ���ı���ָ�Ӧ����Ϣ��ʾ��Ϣ
#define WM_USER_VSCROLLEND		(WM_USER1 +  1)	// �ڳ�ʱʱ���ڽ��յ�����
#define WM_USER_SAVE_TXT		(WM_USER1 +  2)	// ����༭���ı�
#define WM_USER_CLEAN_TXT		(WM_USER1 +  3)	// ����༭���ı�
#define WM_USER_CLEAN_REPORT	(WM_USER1 +  4)	// �����������
#define WM_USER_DISP_CMD		(WM_USER1 +  5)	// ����ָ�����Ϣ��ʾ
#define WM_USER_START_RECV		(WM_USER1 +  6)	// ����Ӧ����Ϣ��ʾ
#define WM_USER_DISP_ANS		(WM_USER1 +  7)	// ����ָ��Ӧ����Ϣ��ʾ
#define WM_USER_TIMEOUT			(WM_USER1 +  8)	// ����Ӧ��ʱ��Ϣ��ʾ
#define WM_USER_ANSERROR		(WM_USER1 +  9)	// ����Ӧ�������Ϣ��ʾ
#define WM_USER_NOTIMEOUT		(WM_USER1 + 10)	// �ڳ�ʱʱ���ڽ��յ�����

#endif  // USER_WINDOWS_MESSAGE_H
