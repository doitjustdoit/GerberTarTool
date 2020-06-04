﻿
// ChangeGerBerNameDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "ChangeGerBerName.h"
#include "ChangeGerBerNameDlg.h"
#include "afxdialogex.h"
#include "zip.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CChangeNameDlg 对话框



CChangeNameDlg::CChangeNameDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MAIN_DLG, pParent)
{
	m_bOutput = FALSE;
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON1);
}

void CChangeNameDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PATH_EDIT, m_cPath);
	DDX_Control(pDX, IDC_BROWER_BTN, m_cBrower);
}

BEGIN_MESSAGE_MAP(CChangeNameDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CChangeNameDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CChangeNameDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BROWER_BTN, &CChangeNameDlg::OnBnClickedBrowerBtn)
	ON_BN_CLICKED(IDC_TAR_BTN, &CChangeNameDlg::OnBnClickedTarBtn)
	ON_BN_CLICKED(IDC_ZIP_BTN, &CChangeNameDlg::OnBnClickedZipBtn)
	ON_BN_CLICKED(IDC_OPENPATH, &CChangeNameDlg::OnBnClickedOpenpath)
END_MESSAGE_MAP()


// CChangeNameDlg 消息处理程序

BOOL CChangeNameDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CChangeNameDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CChangeNameDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CChangeNameDlg::OnBnClickedOk()
{

}


void CChangeNameDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialogEx::OnCancel();
}


void CChangeNameDlg::OnBnClickedBrowerBtn()
{
	m_cPath.GetWindowText(m_szPath);
	if (m_szPath.IsEmpty())
	{
		AfxMessageBox(_T("请先选择路径再进行浏览！"), MB_ICONWARNING);
		return;
	}
	if (m_szPath.Right(1) != _T("\\"))
		m_szPath.Append(_T("\\"));
	m_szPath.Append(_T("Gerber\\"));
	HINSTANCE hInstance = ShellExecute(NULL, _T("open"), m_szPath, NULL, NULL, SW_SHOWNORMAL);
	if ((long)hInstance == ERROR_PATH_NOT_FOUND || ERROR_FILE_NOT_FOUND == (long)hInstance)
		AfxMessageBox(_T("找不到输出文件夹所在路径，请先进行打包操作！"), MB_ICONWARNING);
}

// 拷贝副本到新建的Gerber文件夹并重命名
void CChangeNameDlg::OnBnClickedTarBtn()
{
	CFileFind ff, fDrill, fIPC;
	CString szDestDir, szSrcDir;
	CString szDir; // 新建的目录
	CString postfix; // 后缀
	BOOL bGet = FALSE;

	m_bOutput = FALSE;
	m_cPath.GetWindowText(m_szPath);
	if (m_szPath.IsEmpty()) {
		AfxMessageBox(_T("请先输入路径！"), MB_ICONWARNING);
		return;
	}

	if (m_szPath.Right(1) != _T("\\"))
		m_szPath += _T("\\");
	szDir = m_szPath + _T("Gerber");
	::CreateDirectory(szDir, NULL);

	// 拷贝光绘文件
	BOOL bFind = ff.FindFile(m_szPath + _T("*.G*"));
	while (bFind) {
		bFind = ff.FindNextFile();
		if (!bGet)
		{
			m_fileName = ff.GetFileTitle();
			bGet = TRUE;
		}
		_stscanf(ff.GetFileName().GetString(), _T("%*[^.].%[^\n]"), postfix); // 提取后缀名称
		int n = _tcslen(postfix.GetString());
		szDestDir = szDir + _T("\\") + GetNewName(postfix) + postfix.GetString();
		szSrcDir = ff.GetFilePath();
		CopyFile(szSrcDir, szDestDir, FALSE);
	}
	ff.Close();

	// 拷贝钻孔表数据
	bFind = fDrill.FindFile(m_szPath + m_fileName + _T("*.TX*"));
	while (bFind) {
		bFind = fDrill.FindNextFile();
		_stscanf(fDrill.GetFileName().GetString(), _T("%*[^.].%[^\n]"), postfix); // 提取后缀名称
		int n = _tcslen(postfix.GetString());
		szDestDir = szDir + _T("\\") + GetNewName(postfix) + postfix.GetString();
		szSrcDir = fDrill.GetFilePath();
		CopyFile(szSrcDir, szDestDir, FALSE);
	}
	fDrill.Close();

	// 拷贝IPC网表
	bFind = fIPC.FindFile(m_szPath + _T("*.ipc"));
	while (bFind) {
		bFind = fIPC.FindNextFile();
		_stscanf(fIPC.GetFileName().GetString(), _T("%*[^.].%[^\n]"), postfix); // 提取后缀名称
		int n = _tcslen(postfix.GetString());
		szDestDir = szDir + _T("\\") + GetNewName(postfix) + postfix.GetString();
		szSrcDir = fIPC.GetFilePath();
		CopyFile(szSrcDir, szDestDir, FALSE);
	}
	fIPC.Close();
	m_bOutput = TRUE;
	AfxMessageBox(_T("Gerber 文件拷贝并重命名完成！"));
	ShellExecute(NULL, _T("open"), m_szPath + _T("\\Gerber\\"), NULL, NULL, SW_SHOWNORMAL);
}

// 通过后缀获取需要重命名的名称
CString CChangeNameDlg::GetNewName(CString postfix)
{
	CString szName;
	if (_tcslen(postfix.GetString()) == 3)
	{
		if (postfix == _T("GTL")) // 顶层线路
			szName = _T("TOP.");
		else if (postfix == _T("GTO")) // 顶层丝印
			szName = _T("SILK_TOP.");
		else if (postfix == _T("GTS")) // 顶层阻焊
			szName = _T("SOLDER_TOP.");
		else if (postfix == _T("GTP")) // 顶层钢网
			szName = _T("PASTE_TOP.");
		else if (postfix == _T("GBO")) // 底层丝印 
			szName = _T("SILK_BOT.");
		else if (postfix == _T("GBP")) // 底层钢网 
			szName = _T("PASTE_TOP.");
		else if (postfix == _T("GBS")) // 底层阻焊
			szName = _T("SOLDER_BOT.");
		else if (postfix == _T("GBL")) // 底层线路
			szName = _T("BOTTOM.");
		else if (postfix == _T("GKO")) // KEEPOUT层
			szName = _T("KEEPOUT.");
		else if (postfix.Find(_T("GG")) != -1) // 钻孔引导层
			szName = _T("DrillGuide") + postfix.Right(1) + _T(".");
		else if (postfix.Find(_T("GD")) != -1) // 钻孔表
			szName = _T("DrillDraw") + postfix.Right(1) + _T(".");
		else if (postfix.Find(_T("GP")) != -1) // 内层负片
			szName = _T("InterPlane") + postfix.Right(1) + _T(".");
		else if (postfix.Find(_T("GM")) != -1) // 机械层
			szName = _T("Mechanical") + postfix.Right(1) + _T(".");
		else if (postfix.Find(_T("TX")) != -1) // 机械层
			szName = _T("DrillParam") + postfix.Right(1) + _T(".");
		else if (postfix == _T("APR")) // 光圈文件
			szName = _T("00-Aperture.");
		else if (postfix == _T("DRL")) // 钻孔二进制数据
			szName = _T("DrillBinData.");
		else if (postfix == _T("ipc")) // IPC网表
			szName = _T("IPC-356A.");
	}
	else if (postfix.Find(_T("G")) != -1) // 正片内层
		szName = _T("InterLayer") + postfix.Right(1) + _T(".");

	return szName;
}

// 打包gerber文件为zip压缩格式
void CChangeNameDlg::OnBnClickedZipBtn()
{
	if (!m_bOutput)
	{
		AfxMessageBox(_T("请先进行文件输出再进行打包操作！"), MB_ICONWARNING);
		return;
	}
	m_cPath.GetWindowText(m_szPath);
	if (m_szPath.IsEmpty()) {
		AfxMessageBox(_T("请先输入路径！"), MB_ICONWARNING);
		return;
	}

	if (m_szPath.Right(1) != _T("\\"))
		m_szPath.Append(_T("\\"));

	m_szPath.Append(_T("Gerber\\"));
	HZIP hz = CreateZip(m_szPath + m_fileName + _T("-Gerber.zip"), 0);
	CFileFind ff;
	BOOL bFind = ff.FindFile(m_szPath + _T("*.*"));
	while (bFind)
	{
		bFind = ff.FindNextFile();
		ZipAdd(hz, ff.GetFileName(), ff.GetFilePath());
	}
	ff.Close();
	CloseZip(hz);
	AfxMessageBox(_T("生成") + m_fileName + _T("-Gerber.zip")+ _T("完成！"));
	ShellExecute(NULL, _T("open"), m_szPath, NULL, NULL, SW_SHOWNORMAL);
}

// 选择路径
void CChangeNameDlg::OnBnClickedOpenpath()
{
	BROWSEINFO bi;
	TCHAR szPath[MAX_PATH];
	CString m_saveFilePath;

	bi.hwndOwner = ::GetFocus();
	bi.pidlRoot = NULL;
	bi.pszDisplayName = szPath;
	bi.lpszTitle = _T("请选择一个文件夹");
	bi.ulFlags = BIF_BROWSEFORCOMPUTER | BIF_DONTGOBELOWDOMAIN | BIF_RETURNONLYFSDIRS;
	bi.lpfn = NULL;
	bi.lParam = 0;
	bi.iImage = 0;

	LPITEMIDLIST pItemIDList = ::SHBrowseForFolder(&bi);
	if (pItemIDList)
	{
		if (SHGetPathFromIDList(pItemIDList, szPath))
			m_szPath = CString(szPath) + _T("\\");

		// use IMalloc interface for avoiding memory leak  
		IMalloc* pMalloc;
		if (SHGetMalloc(&pMalloc) != NOERROR)
			AfxMessageBox(_T("Can't get the IMalloc interface\n"));

		pMalloc->Free(pItemIDList);
		if (pMalloc)
			pMalloc->Release();
		m_cPath.SetWindowText(m_szPath);
	}
}
