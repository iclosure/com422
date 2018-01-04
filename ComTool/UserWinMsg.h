
#ifndef USER_WINDOWS_MESSAGE_H
#define USER_WINDOWS_MESSAGE_H

#define WM_USER0				(WM_USER  + 001)	// 作为一个对话框内部消息用	// (WM_USER + 001) ~ (WM_USER + 200)
#define WM_USER1				(WM_USER0 + 201)	// 作为公共消息用				// (WM_USER + 201) ~ (WM_USER + 400)
#define WM_USER2				(WM_USER1 + 201)	// 作为指令应答消息用			// (WM_USER + 401) ~ (WM_USER + 600)

// 自定义键盘空格键消息
#define WM_KEY_SPACE			(WM_USER1 + 0)	// SendMessage()方式
// 自定义消息：文本、指令、应答信息显示消息
#define WM_USER_VSCROLLEND		(WM_USER1 +  1)	// 在超时时间内接收到数据
#define WM_USER_SAVE_TXT		(WM_USER1 +  2)	// 保存编辑框文本
#define WM_USER_CLEAN_TXT		(WM_USER1 +  3)	// 清除编辑框文本
#define WM_USER_CLEAN_REPORT	(WM_USER1 +  4)	// 清除报表数据
#define WM_USER_DISP_CMD		(WM_USER1 +  5)	// 辑框指令发送信息显示
#define WM_USER_START_RECV		(WM_USER1 +  6)	// 启动应答信息显示
#define WM_USER_DISP_ANS		(WM_USER1 +  7)	// 辑框指令应答信息显示
#define WM_USER_TIMEOUT			(WM_USER1 +  8)	// 辑框应答超时信息显示
#define WM_USER_ANSERROR		(WM_USER1 +  9)	// 辑框应答错误信息显示
#define WM_USER_NOTIMEOUT		(WM_USER1 + 10)	// 在超时时间内接收到数据

#endif  // USER_WINDOWS_MESSAGE_H
