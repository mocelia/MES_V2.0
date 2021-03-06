// WriteIMEIDlg.cpp : implementation file
//

#include "stdafx.h"
#include "WriteIMEI.h"
#include "WriteIMEIDlg.h"
#include "ColReadOnlyEdit.h"
#include "AdoInterface.h"

#include "math.h"

#include "mmsystem.h"           //导入声音头文件
#pragma comment(lib,"winmm.lib")//导入声音头文件库

//MTK
#include "brom.h"
#include "DOWNLOAD.h"
#include "external_memory_setting.h"
#include "flashtool_handle.h"// Flash Download/Readback/Format Library.   
#include "meta.h"

namespace SPMETA_DLL 
{
    #include "spmeta.h"
};

#include "mtk_mcu.h"
#include "mtk_resource.h"
#include "mtk_status.h"
#include "SLA.h"
#include "AUTH.h"

#ifndef  _SCERT_H_
#include "SCERT.h"
#endif
//MTK

///////加密///////////
#include "yxyDES.h"
yxyDES* my_des = new yxyDES();

/////数据库/////
#include "DataBase.h"

//#include "CSR_Function_Test.h"
//#include "UIThread.h"
/////解锁码/////
#include "md5.h"


#include "LibincStory.h"
//联芯4G模块写号
#include "LeadcoreChip.h"
#include "FunctionContext.h"
//打印条形码
#include "ZebraPrinter.h"

//多线程AT写IMEI号
#include "IMEIWrite_MulAT.h"
//贴纸批量打印
#include "TagsBatchPrint.h"

//一次装箱
#include "PackingPrint.h"

#define RFT_SUCCESS     0
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define END_TIME  10000
CWriteIMEIDlg* pDlg;
/////////////////////////////////////////////////////////////////////////////
// CWriteIMEIDlg dialog

CWriteIMEIDlg::CWriteIMEIDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWriteIMEIDlg::IDD, pParent)
	, m_BT_Addr(_T(""))
	, BtAddrCount(_T(""))
	, BtAddrTotal(_T(""))
	, m_Result_C(_T(""))
	, m_SN(_T(""))
	, SN_Count(0)
	, SN_PRE(_T(""))
	, IMEI_PRE(_T(""))
	, m_Type_S(_T(""))
	, Wait_time(0)
	, MSN_Relative_IMEI(FALSE)
	, BT_Write_Enable(FALSE)
	, Machine_Type(_T(""))
	, Recover_Status(FALSE)
	, CHECK_GSM_TEST_Enable(FALSE)
	, CHECK_BT_TEST_Enable(FALSE)
	, m_IMEI2(_T(""))
	, m_IMEI3(_T(""))
	, m_IMEI4(_T(""))
	, m_strWifi(_T(""))
	, BT_PRE(_T(""))
	, WIFI_PRE(_T(""))
	, IMEI_Count(_T(""))
	, m_sPath(_T(""))
	, BARCODE_Input(_T(""))
	, m_sPath_AP(_T(""))
	, NETCODE_PRE(_T(""))
	, NETCODE_Input(_T(""))
	, NETCODE_Count(0)
	, PrinterScript(_T(""))
{
	//{{AFX_DATA_INIT(CWriteIMEIDlg)
	m_IMEI = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	pDlg=this;
	if(!m_UDPSocket.Create(myPORT,SOCK_DGRAM))//没有连接网络时，无法创建socket
	{
		AfxMessageBox("Failed Create Socket");
	}
}

void CWriteIMEIDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWriteIMEIDlg)
	DDX_Control(pDX, IDC_AUTO, m_Auto);
	DDX_Control(pDX, IDC_STATIC_TYPE, m_Static_Type);
	DDX_Control(pDX, IDC_WRITE, m_Write);
	DDX_Control(pDX, IDC_TYPE, m_Type);
	DDX_Control(pDX, IDC_BAUD, m_Baud);
	DDX_Control(pDX, IDC_RESULT, m_Result);
	DDX_Control(pDX, IDC_PORT, m_Port);
	DDX_Control(pDX, IDC_FINALRESULT, m_Finalresult);
	DDX_Text(pDX, IDC_IMEI, m_IMEI);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_IMEI, IMEI_Control);
	DDX_Control(pDX, IDC_STATIC_TYPE2, m_Static_Type_BT);
	DDX_Text(pDX, IDC_BT_Addr, m_BT_Addr);
	DDX_Text(pDX, IDC_EDIT1, BtAddrCount);
	DDX_Text(pDX, IDC_EDIT3, BtAddrTotal);
	DDX_Text(pDX, IDC_RESULT, m_Result_C);
	DDX_Control(pDX, IDC_MSN, SN_Control);
	DDX_Text(pDX, IDC_MSN, m_SN);
	DDX_Text(pDX, IDC_IMEI6, SN_Count);
	DDX_Text(pDX, IDC_IMEI5, SN_PRE);
	DDX_Text(pDX, IDC_IMEI4, IMEI_PRE);
	DDX_Control(pDX, IDC_IMEI4, IMEI_PRE_Control);
	DDX_Control(pDX, IDC_IMEI5, SN_PRE_Control);
	DDX_Control(pDX, IDC_IMEI6, SN_Count_Control);
	DDX_CBString(pDX, IDC_TYPE, m_Type_S);
	DDX_Text(pDX, IDC_EDIT2, Wait_time);
	DDX_Check(pDX, IDC_CHECK1, MSN_Relative_IMEI);
	DDX_Control(pDX, IDC_CHECK1, MSN_Relative_IMEI_Control);
	DDX_Check(pDX, IDC_CHECK2, BT_Write_Enable);
	DDX_Control(pDX, IDC_CHECK2, BT_Write_Enable_Control);
	DDX_Control(pDX, IDC_BT_Addr, m_BT_Addr_Control);
	DDX_Text(pDX, IDC_IMEI8, Machine_Type);
	DDX_Check(pDX, IDC_CHECK3, Recover_Status);
	DDX_Control(pDX, IDC_CHECK3, Recover_Status_Control);
	DDX_Check(pDX, IDC_CHECK4, CHECK_GSM_TEST_Enable);
	DDX_Check(pDX, IDC_CHECK5, CHECK_BT_TEST_Enable);
	DDX_Control(pDX, IDC_CHECK4, CHECK_GSM_TEST_Enable_Control);
	DDX_Control(pDX, IDC_CHECK5, CHECK_BT_TEST_Enable_Control);
	DDX_Text(pDX, IDC_IMEI7, m_IMEI2);
	DDX_Text(pDX, IDC_IMEI10, m_IMEI3);
	DDX_Text(pDX, IDC_IMEI9, m_IMEI4);
	DDX_Text(pDX, IDC_BT_Addr2, m_strWifi);
	DDX_Control(pDX, IDC_IMEI7, IMEI2_Control);
	DDX_Control(pDX, IDC_IMEI10, IMEI3_Control);
	DDX_Control(pDX, IDC_IMEI9, IMEI4_Control);
	DDX_Control(pDX, IDC_BT_Addr2, m_WIFI_Addr_Control);
	DDX_Text(pDX, IDC_EDIT19, BT_PRE);
	DDX_Control(pDX, IDC_EDIT19, BT_PRE_Control);
	DDX_Text(pDX, IDC_EDIT21, WIFI_PRE);
	DDX_Control(pDX, IDC_EDIT21, WIFI_PRE_Control);
	DDX_Text(pDX, IDC_EDIT26, IMEI_Count);
	DDX_Control(pDX, IDC_EDIT1, BtAddrCount_Control);
	DDX_Control(pDX, IDC_EDIT26, IMEI_Count_Control);
	DDX_Text(pDX, IDC_EDIT20, m_sPath);
	DDX_Control(pDX, IDC_AUTO2, Plat_Form);
	DDX_Control(pDX, IDC_BUTTON6, Modem_Database_Control);
	DDX_Control(pDX, IDC_EDIT2, Wait_time_Control);
	DDX_Control(pDX, IDC_BUTTON2, SaveCurrentSetting_Control);
	DDX_Control(pDX, IDC_BT_Addr3, BARCODE_Input_Control);
	DDX_Text(pDX, IDC_BT_Addr3, BARCODE_Input);
	DDX_Text(pDX, IDC_EDIT34, m_sPath_AP);
	DDX_Control(pDX, IDC_BUTTON7, AP_Database_Control);
	DDX_Control(pDX, IDC_BUTTON8, DB_Find_Control);
	DDX_Control(pDX, IDC_BUTTON10, Sensor_Test_Thread);
	DDX_Control(pDX, IDC_NETCODE, NETCODE_Control);
	DDX_Control(pDX, IDC_EDIT35, NETCODE_PRE_Control);
	DDX_Text(pDX, IDC_EDIT35, NETCODE_PRE);
	DDX_Text(pDX, IDC_NETCODE, NETCODE_Input);
	DDX_Control(pDX, IDC_IMEI11, NETCODE_Count_Control);
	DDX_Text(pDX, IDC_IMEI11, NETCODE_Count);
	DDX_Text(pDX, IDC_EDIT27, PrinterScript);
	DDX_Control(pDX, IDC_EDIT27, PrinterScript_Control);
	DDX_Control(pDX, IDC_BUTTON11, Manul_Print_Control);
}

BEGIN_MESSAGE_MAP(CWriteIMEIDlg, CDialog)
	//{{AFX_MSG_MAP(CWriteIMEIDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_WRITE, OnWrite)
	ON_CBN_SELCHANGE(IDC_AUTO, OnSelchangeAuto)
	ON_EN_CHANGE(IDC_IMEI, OnChangeImei)
	ON_WM_CLOSE()

	ON_WM_RBUTTONUP()   //右键弹起
	ON_WM_CONTEXTMENU() //菜单
	ON_COMMAND(ID_FRESH_32771, OnCbnSelchangeCombo2_Fresh)//刷新端口

	ON_WM_CTLCOLOR()//改变颜色
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, &CWriteIMEIDlg::OnBnClickedButtonstart1)
	ON_BN_CLICKED(IDC_BUTTON2, &CWriteIMEIDlg::OnBnClickedButton2)
	ON_EN_CHANGE(IDC_MSN, &CWriteIMEIDlg::OnEnChangeImei3)
	ON_BN_CLICKED(IDC_BUTTON3, &CWriteIMEIDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CWriteIMEIDlg::OnBnClickedButton4)
	ON_EN_CHANGE(IDC_IMEI7, &CWriteIMEIDlg::OnEnChangeImei7)
	ON_EN_CHANGE(IDC_IMEI10, &CWriteIMEIDlg::OnEnChangeImei10)
	ON_EN_CHANGE(IDC_IMEI9, &CWriteIMEIDlg::OnEnChangeImei9)
	ON_EN_CHANGE(IDC_BT_Addr, &CWriteIMEIDlg::OnEnChangeBtAddr)
	ON_EN_CHANGE(IDC_BT_Addr2, &CWriteIMEIDlg::OnEnChangeBtAddr2)
	ON_EN_CHANGE(IDC_EDIT1, &CWriteIMEIDlg::OnEnChangeEdit1)
	ON_BN_CLICKED(IDC_BUTTON5, &CWriteIMEIDlg::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON6, &CWriteIMEIDlg::OnBnClickedButton6)
	ON_EN_CHANGE(IDC_BT_Addr3, &CWriteIMEIDlg::OnEnChangeBtAddr3)
	ON_BN_CLICKED(IDOK, &CWriteIMEIDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON7, &CWriteIMEIDlg::OnBnClickedButton7)
	ON_BN_CLICKED(IDC_BUTTON8, &CWriteIMEIDlg::OnBnClickedButton8)
	ON_BN_CLICKED(IDC_BUTTON10, &CWriteIMEIDlg::OnBnClickedButtonstart10)
	ON_EN_CHANGE(IDC_NETCODE, &CWriteIMEIDlg::OnEnChangeNetcode)
	ON_BN_CLICKED(IDC_BUTTON11, &CWriteIMEIDlg::OnBnClickedButtonstart11)
	ON_BN_CLICKED(IDC_BUTTON13, &CWriteIMEIDlg::OnBnClickedButtonstart13)
	ON_BN_CLICKED(IDC_BUTTON24, &CWriteIMEIDlg::OnBnClickedButton24)
	ON_BN_CLICKED(IDC_BUTTON33, &CWriteIMEIDlg::OnBnClickedButton33)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWriteIMEIDlg message handlers





BOOL CWriteIMEIDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	m_Auto.EnableWindow(false);
	//Plat_Form.EnableWindow(false);
	DB_Find_Control.EnableWindow(false);
	AP_Database_Control.EnableWindow(false);
	//Manul_Print_Control.EnableWindow(false);
	Modem_Database_Control.EnableWindow(false);
	Wait_time_Control.EnableWindow(false);
	SaveCurrentSetting_Control.EnableWindow(false);

	IMEI_PRE_Control.EnableWindow(false);
	SN_PRE_Control.EnableWindow(false);
	BT_PRE_Control.EnableWindow(false);
	WIFI_PRE_Control.EnableWindow(false);

	PrinterScript_Control.EnableWindow(false);

	NETCODE_PRE_Control.EnableWindow(false);
	NETCODE_Count_Control.EnableWindow(false);

	BtAddrCount_Control.EnableWindow(false);
	IMEI_Count_Control.EnableWindow(false);

	SN_Count_Control.EnableWindow(false);
	m_Type.EnableWindow(false);
	Plat_Form.EnableWindow(false);

	MSN_Relative_IMEI_Control.EnableWindow(false);
	BT_Write_Enable_Control.EnableWindow(false);
	Recover_Status_Control.EnableWindow(false);

	CHECK_GSM_TEST_Enable_Control.EnableWindow(false);
	CHECK_BT_TEST_Enable_Control.EnableWindow(false);
	

	//set the IMEI input box font
	CFont* ffont;
	ffont = new CFont;
	ffont->CreateFont(36,0,0,0,700,0,0,0,ANSI_CHARSET,OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH|FF_DONTCARE,
		"arial"); 
	// Cause the label to use the new font
	GetDlgItem(IDC_IMEI)->SetFont(ffont);  //IMEI1
	GetDlgItem(IDC_IMEI7)->SetFont(ffont); //IMEI2
	GetDlgItem(IDC_IMEI10)->SetFont(ffont);//IMEI3
	GetDlgItem(IDC_IMEI9)->SetFont(ffont); //IMEI4

	GetDlgItem(IDC_BT_Addr)->SetFont(ffont);
	GetDlgItem(IDC_BT_Addr2)->SetFont(ffont); //WIFI
	GetDlgItem(IDC_BT_Addr3)->SetFont(ffont); //板号
	GetDlgItem(IDC_MSN)->SetFont(ffont);

	GetDlgItem(IDC_NETCODE)->SetFont(ffont);  //入网证号

	CFont* font2;
	font2 = new CFont;
	font2->CreateFontA(15,7,0,0,FW_MEDIUM,0,0,0,ANSI_CHARSET,OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH|FF_DONTCARE,
		"宋体"); 

	GetDlgItem(IDC_RESULT)->SetFont(font2);

	IMEIorSN=0;

	//Wait_time=1000;
	extern COLORREF gColor;
	extern CString gFinalResult;
	gColor = RGB(0,100,100);
	gFinalResult.Format(" IDLE...");
	RedrawWindow();  m_Result.UpdateWindow(); 
	//init baud
	m_Baud.AddString("115200");
	m_Baud.AddString("57600");
	m_Baud.AddString("56000");
	m_Baud.AddString("38400");
	m_Baud.AddString("19200");
	m_Baud.SetCurSel(0);

	
	//初始化选择话机的模式 
	Setdlg.TestMode_S=TestMode_M;

	Setdlg.Unlock_code_S=Unlock_code_M;
	//init port
	m_Port.AddString("USB");
	for( int i=1; i<128; i++)
	{
		CString sPort;
		sPort.Format(_T("\\\\.\\COM%d"),i);
		BOOL bSuccess=FALSE;
		HANDLE hPort=::CreateFile(sPort, GENERIC_READ|GENERIC_WRITE, 0, 0,
			OPEN_EXISTING, 0, 0);
		if( hPort == INVALID_HANDLE_VALUE)
		{
			DWORD dwError=GetLastError();
			if( dwError == ERROR_ACCESS_DENIED)
				bSuccess=TRUE;
		}
		else
		{
			bSuccess=TRUE;
			CloseHandle(hPort);       //OnInitDialog寻找端口
		}
		if(  bSuccess )
		{
			CString str;
			str.Format("COM%d",i);
			m_Port.AddString(str);
		}
	}
	
	m_Port.SetCurSel(0);

	//init type
	m_Type.AddString("Single");
	m_Type.AddString("Double");
	if(m_Type_S=="Single")
		m_Type_N=0;
	else if(m_Type_S=="Double")
		m_Type_N=1;
	else
		m_Type_N=1;
	
	m_Type.SetCurSel(m_Type_N);
	m_Type.GetWindowTextA(m_Type_S);



	

	BtAddrCount_int=0;
	BtAddrTotal_int=0;
	WIFIAddrTotal_int=0;
	WIFIAddrCount_int=0;

	IMEI_Count_int=0;
	
	IMEI2_Result=true;//初始化置1处理
	IMEI3_Result=true;
	IMEI4_Result=true;
	BT_Result=true;
	WIFI_Result=true;

	LoadConfig();     //仅初始化装载配置，但设置的时候，对变量实现了更新
	UpdateData(false);//AfxMessageBox(BtAddrTotal);
	

	//////////////////////////////////
	////MTK_写号的初始化//////////////
	//////////////////////////////////
	m_Static_Type.SetWindowText("IMEI:");
	CEdit *pEdit1;
	pEdit1=(CEdit*)GetDlgItem(IDC_IMEI);
	pEdit1->SetLimitText(32);
	pEdit1->SetWindowText("");
	m_Static_Type_BT.SetWindowText("BT_Addr:");
	CEdit *pEdit2;
	pEdit2=(CEdit*)GetDlgItem(IDC_BT_Addr);
	pEdit2->SetLimitText(12);
	pEdit2->SetWindowText("");
	CEdit *pEdit3;
	pEdit3=(CEdit*)GetDlgItem(IDC_IMEI4);
	pEdit3->SetLimitText(SN_Count);

	CEdit *pEdit4;
	pEdit4=(CEdit*)GetDlgItem(IDC_EDIT35);
	pEdit4->SetLimitText(NETCODE_Count);

	/*if(USE_Times_Count==9999)//以后使用
		USE_Times_Count=9999;
	else if(USE_Times_Count==0)
		USE_Times_Count=0;
	else
		USE_Times_Count--;*/
	//功能限制，需要连接数据库服务器
	INITIAL_POWER=true;      //初始化为true
	/*if((Initial_Connect_DB_M=="false"))//不使用数据库的情况下：不能参数检查、检查合法性，就是一个普通的工具
	{
		INITIAL_POWER=false; //没有连接数据库
	}
	else
	{
		OnTimerFunction(25);//校验服务器信息
	}
	//SaveConfig();//功能限制后保存,=====必须屏蔽，否则《生效影响》  无法实现---MAIN界面 选项控件不能正常工作，因为UpdateData(false\true)
	
	//init auto
	if(paraArray[5].Low_Limit_Value.Left(1)=="1")
	{
		//AfxMessageBox("100000000000");
		LoadResult_Time();
		if(atoi(USE_Times_Count_SERVER_CS)>1)
		{
			USE_Times_Count=atoi(USE_Times_Count_SERVER_CS)-1;
			SaveResult_Time();//以后使用
		}
		else
		{
			OnOK();
		}
	}
	if((USE_Times_Count>1000)&&(USE_Times_Count<2000))
	{
		//DB_Find_Control.EnableWindow(true);
		m_Auto.AddString("读取检查");
		m_Auto.AddString("写号");
		m_Auto.SetCurSel(1);
	}
	else
	{
		//DB_Find_Control.EnableWindow(false);
		m_Auto.AddString("写号");
		m_Auto.SetCurSel(0);
	}
	if(USE_Times_Count-1000>0)//1000代表数据库功能
	{
		if(Plat_Limit_Enable=="001")
		{
			Plat_Form.AddString("展讯平台");
			Plat_Form.SetCurSel(0);
		}
		else if(Plat_Limit_Enable=="010")
		{
			Plat_Form.AddString("MTK功能机");
			Plat_Form.SetCurSel(0);
		}
		else if(Plat_Limit_Enable=="100")
		{
			Plat_Form.AddString("MTK智能机");
			Plat_Form.SetCurSel(0);
		}
		else if(Plat_Limit_Enable=="011")
		{
			Plat_Form.AddString("展讯平台");
			Plat_Form.AddString("MTK功能机");

			if(Plat_Form_S=="展讯平台")
				Plat_Form.SetCurSel(0);
			else if(Plat_Form_S=="MTK功能机")
				Plat_Form.SetCurSel(1);
		}
		else if(Plat_Limit_Enable=="101")
		{
			Plat_Form.AddString("展讯平台");
			Plat_Form.AddString("MTK智能机");

			if(Plat_Form_S=="展讯平台")
				Plat_Form.SetCurSel(0);
			else if(Plat_Form_S=="MTK智能机")
				Plat_Form.SetCurSel(1);
		}
		else if(Plat_Limit_Enable=="110")
		{
			Plat_Form.AddString("MTK功能机");
			Plat_Form.AddString("MTK智能机");
			if(Plat_Form_S=="MTK功能机")
				Plat_Form.SetCurSel(0);
			else if(Plat_Form_S=="MTK智能机")
				Plat_Form.SetCurSel(1);
		}
		else if(Plat_Limit_Enable=="111")
		{
			Plat_Form.AddString("展讯平台");
			Plat_Form.AddString("MTK功能机");
			Plat_Form.AddString("MTK智能机");

			if(Plat_Form_S=="展讯平台")
				Plat_Form.SetCurSel(0);
			else if(Plat_Form_S=="MTK功能机")
				Plat_Form.SetCurSel(1);
			else if(Plat_Form_S=="MTK智能机")
				Plat_Form.SetCurSel(2);
		}
		
	}*/
	//else//以后使用
	{
		//Plat_Form.AddString("展讯平台");
		Plat_Form.AddString("展讯平台");
		Plat_Form.AddString("MTK功能机");
		Plat_Form.AddString("MTK智能机");

		Plat_Form.AddString("M255-AT");
		Plat_Form.AddString("联芯4G_MIFI");

		if(Plat_Form_S=="展讯平台")
			Plat_Form.SetCurSel(0);
		else if(Plat_Form_S=="MTK功能机")
			Plat_Form.SetCurSel(1);
		else if(Plat_Form_S=="MTK智能机")
			Plat_Form.SetCurSel(2);
		else if(Plat_Form_S=="M255-AT")
			Plat_Form.SetCurSel(3);
		else if(Plat_Form_S=="联芯4G_MIFI")
			Plat_Form.SetCurSel(4);
		
	}
	////////////////////////////////////


	//////////////////////////////////
	////MTK_写号的初始化//////////////
	//////////////////////////////////
	m_sPath=m_strExchangeFileName;
	m_sPath_AP= m_strExchangeFileName_Ap;//智能机
	UpdateData(false);
	MetaContextInit ();

	

	/*CEdit *pEdit4;
	pEdit4=(CEdit*)GetDlgItem(IDC_IMEI3);
	pEdit4->SetLimitText(SN_Count);*/
	if(UP_DB_Data_M=="true")//9
		Setdlg.UP_DB_Data=TRUE;
	else
		Setdlg.UP_DB_Data=FALSE;


	if(Initial_Connect_DB_M=="true")//10
		Setdlg.Initial_Connect_DB=TRUE;
	else
		Setdlg.Initial_Connect_DB=FALSE;

	if(DB_CONNECT_M=="true")//11
		Setdlg.DB_CONNECT=TRUE;
	else
		Setdlg.DB_CONNECT=FALSE;

	if(IMEI1_Choose_M=="true")//12
		Setdlg.IMEI1_Choose=TRUE;
	else
		Setdlg.IMEI1_Choose=FALSE;

	if(IMEI2_Choose_M=="true")//13
		Setdlg.IMEI2_Choose=TRUE;
	else
		Setdlg.IMEI2_Choose=FALSE;

	if(IMEI3_Choose_M=="true")//14
		Setdlg.IMEI3_Choose=TRUE;
	else
		Setdlg.IMEI3_Choose=FALSE;
	if(IMEI4_Choose_M=="true")//15
		Setdlg.IMEI4_Choose=TRUE;
	else
		Setdlg.IMEI4_Choose=FALSE;

	if(IMEI_CheckValid_Choose_M=="true")//18
		Setdlg.IMEI_CheckValid_Choose=TRUE;
	else
		Setdlg.IMEI_CheckValid_Choose=FALSE;
	if(AutoIMEI_Choose_M=="true")//19
		Setdlg.AutoIMEI_Choose=TRUE;
	else
		Setdlg.AutoIMEI_Choose=FALSE;
	if(UseSameIMEI_Choose_M=="true")//20
		Setdlg.UseSameIMEI_Choose=TRUE;
	else
		Setdlg.UseSameIMEI_Choose=FALSE;

	if(IMEI_DB_Choose_M=="true")//23
		Setdlg.IMEI_DB_Choose=TRUE;
	else
		Setdlg.IMEI_DB_Choose=FALSE;

	if(MAC_DB_Choose_M=="true")//24
		Setdlg.MAC_DB_Choose=TRUE;
	else
		Setdlg.MAC_DB_Choose=FALSE;

	if(MSN_Upload_Choose_M=="true")//25
		Setdlg.MSN_Upload_Choose=TRUE;
	else
		Setdlg.MSN_Upload_Choose=FALSE;

	if(ORACLE_Connect_Choose_M=="true")//27
		Setdlg.ORACLE_Connect_Choose=TRUE;
	else
		Setdlg.ORACLE_Connect_Choose=FALSE;

	if(Check_repeat_Choose_M=="true")//26
		Setdlg.Check_repeat_Choose=TRUE;
	else
		Setdlg.Check_repeat_Choose=FALSE;

	if(AutoBTAddr_Choose_M=="true")//21
		Setdlg.AutoBTAddr_Choose=TRUE;
	else
		Setdlg.AutoBTAddr_Choose=FALSE;

	if(AutoWIFIAddr_Choose_M=="true")//22
		Setdlg.AutoWIFIAddr_Choose=TRUE;
	else
		Setdlg.AutoWIFIAddr_Choose=FALSE;




	if(BT_MAC_ADDR_Choose_M=="true")//16
		Setdlg.BT_MAC_ADDR_Choose=TRUE;
	else
		Setdlg.BT_MAC_ADDR_Choose=FALSE;

	if(WIFI_MAC_ADDR_Choose_M=="true")//17
		Setdlg.WIFI_MAC_ADDR_Choose=TRUE;
	else
		Setdlg.WIFI_MAC_ADDR_Choose=FALSE;

	if(BARCODE_Choose_M=="true")//22 
		Setdlg.BARCODE_Choose=TRUE;
	else
		Setdlg.BARCODE_Choose=FALSE;

	if(ChangeEN_Choose_M=="true")//23
		Setdlg.ChangeEN_Choose=TRUE;
	else
		Setdlg.ChangeEN_Choose=FALSE;

	if(Unlock_Code_Choose_M=="true")//24
		Setdlg.Unlock_Code_Choose=TRUE;
	else
		Setdlg.Unlock_Code_Choose=FALSE;

	if(NETCODE_Choose_M=="true")//25
		Setdlg.NETCODE_Choose=TRUE;
	else
		Setdlg.NETCODE_Choose=FALSE;

	if(NETCODE_Only_Num_Choose_M=="true")//26
		Setdlg.NETCODE_Only_Num_Choose=TRUE;
	else
		Setdlg.NETCODE_Only_Num_Choose=FALSE;

	if(JUST_UpDate_DB_Choose_M=="true")//27
		Setdlg.JUST_UpDate_DB_Choose=TRUE;
	else
		Setdlg.JUST_UpDate_DB_Choose=FALSE;

	if(UpdateIMEI_MSN_Choose_M=="true")//28
		Setdlg.UpdateIMEI_MSN_Choose=TRUE;
	else
		Setdlg.UpdateIMEI_MSN_Choose=FALSE;

	if(IMEI_Exist_Update_NC_Choose_M=="true")//29
		Setdlg.IMEI_Exist_Update_NC_Choose=TRUE;
	else
		Setdlg.IMEI_Exist_Update_NC_Choose=FALSE;

	if(Machine_Type_NC_CompareChoose_M=="true")//30
		Setdlg.Machine_Type_NC_CompareChoose=TRUE;
	else
		Setdlg.Machine_Type_NC_CompareChoose=FALSE;

	if(Software_Version_Check_Choose_M=="true")//18
		Setdlg.Software_Version_Check_Choose=TRUE;
	else
		Setdlg.Software_Version_Check_Choose=FALSE;

	if(IMEI_MAC_Check_Choose_M=="true")//19
		Setdlg.IMEI_MAC_Check_Choose=TRUE;
	else
		Setdlg.IMEI_MAC_Check_Choose=FALSE;

	if(Scan_Choose_M=="true")//20
		Setdlg.Scan_Choose=TRUE;
	else
		Setdlg.Scan_Choose=FALSE;

	if(Repeat_AutoStart_M=="true")//20
		Setdlg.Repeat_AutoStart=TRUE;
	else
		Setdlg.Repeat_AutoStart=FALSE;

	if(MSN_Relative_IMEI_S=="true")
	{
		MSN_Relative_IMEI=TRUE;
		SN_Control.EnableWindow(false);
		UpdateData(false);
	}
	else
	{
		MSN_Relative_IMEI=FALSE;
		SN_Control.EnableWindow(true);
		UpdateData(false);
	}

	///////////////生效影响////////////////////
	if(BT_Write_Enable_S=="true")
	{
		BT_Write_Enable=TRUE;
		UpdateData(false);
	}
	else
	{
		BT_Write_Enable=FALSE;
		UpdateData(false);
	}

	if(Recover_Status_S=="true")
	{
		Recover_Status=TRUE;
		UpdateData(false);
	}
	else
	{
		Recover_Status=FALSE;
		UpdateData(false);
	}

	if(CHECK_GSM_TEST_Enable_S=="true")
	{
		CHECK_GSM_TEST_Enable=TRUE;
		UpdateData(false);
	}
	else
	{
		CHECK_GSM_TEST_Enable=FALSE;
		UpdateData(false);
	}
	if(CHECK_BT_TEST_Enable_S=="true")
	{
		CHECK_BT_TEST_Enable=TRUE;
		UpdateData(false);
	}
	else
	{
		CHECK_BT_TEST_Enable=FALSE;
		UpdateData(false);
	}
   /////////////////////////////////////////////////
	//UP_ONE_OK_SECOND_FAIL="false";
	INITIAL_PROCESS=true;
	INITIAL_Paint=127;
	if(0)//if(Initial_Connect_DB_M=="true")
	{
		//连接SQL SERVER
		CString Conn="";
		Conn.Format("driver={SQL Server};Server=%s;DATABASE=%s;UID=%s;PWD=%s", m_server_M,m_db_M,m_user_M, m_pwd_M);
		//Conn="10.5.8.129;Persist Security Info=True;User ID=sa;Password=lbcfyly1";
		//Conn.Format("Provider=SQLOLEDB.1;Persist Scurity Info=True;User ID=%s;Password=%s;Initial Catalog=%s;Data Source=%s",m_user_M, m_pwd_M,m_db_M, m_server_M);
		//Conn.Format("Provider=sqloledb;Data Source=DESAI-DBBBC3D03\LIPURAN;Initial Catalog=%s;;User ID=%s;Password=%s",m_db_M,m_user_M, m_pwd_M);
		CString DB_FAIL=Setdlg.myado.ConnecDataLibrary(Conn,"","",adModeUnknown);//初始化连接数据库
		if(DB_FAIL=="FAIL")
		{
			m_Result.GetWindowText(LOG_Show_Old); 
			m_Result.SetWindowText("连接数据库失败！！！ \r\n\r\n"+LOG_Show_Old);
			gColor = RGB(255,0,0);
			gFinalResult.Format("CO_Fail");
			RedrawWindow();  m_Result.UpdateWindow(); 
			INITIAL_PROCESS=true;
		}
		else
		{
			m_Result.GetWindowText(LOG_Show_Old); 
			m_Result.SetWindowText("连接数据库成功！！！ \r\n\r\n"+LOG_Show_Old);
			gColor = RGB(0,0,255);
			gFinalResult.Format("CO_OK");
			INITIAL_PROCESS=false;
		}
		UpdateData(true);  //把中间值赋给变量
		UpdateData(false); //把变量显示在界面
		m_Result.UpdateWindow();
		/*/连接oracle数据库
		HRESULT hr;
		//CString Conn_ZTE="";
		//Conn_ZTE="Provider=MSDAORA.1;Persist Security Info=True;Password=barcode_59.33.252.107;User ID=barcode;Data Source=barcode";
		
		Conn.Format("Provider=SQLOLEDB.1;Persist Scurity Info=True;User ID=%s;Password=%s;Initial Catalog=%s;Data Source=%s",m_user_M, m_pwd_M,m_db_M, m_server_M);
		//hr = Setdlg.myado.m_pCon.CreateInstance("ADODB.Connection");
		hr = Setdlg.myado.m_pCon.CreateInstance(__uuidof(Connection));
		Setdlg.myado.m_pCon->CursorLocation=adUseClient;
		//CString strConnect;
		if(SUCCEEDED(hr))
		{
			try
			{
				//strConnect="Provider=MSDAORA.1;Data Source=barcode_59.33.252.107";
				Setdlg.myado.m_pCon->Open((_bstr_t)Conn,"","",adModeUnknown);      //已经屏蔽
			}
		
			catch(_com_error e)
			{  
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("连接  SQL数据库  失败===连接! \r\n\r\n"+LOG_Show_Old);
				CATCH_ERROR;
				return FALSE ;
			} // 	
			m_Result.GetWindowText(LOG_Show_Old);  
			m_Result.SetWindowText("连接  SQL数据库  成功! \r\n\r\n"+LOG_Show_Old);
		}
		else
		{
			m_Result.GetWindowText(LOG_Show_Old);  
			m_Result.SetWindowText("连接  SQL数据库  失败===! \r\n\r\n"+LOG_Show_Old);
			return FALSE;	
		}*/
	}
	OnBnClickedButton4();//预处理
	m_hExitThread = CreateEvent(NULL, TRUE, FALSE, NULL);
    m_hEnterModeSuccess = CreateEvent(NULL, TRUE, FALSE, NULL);
	UpdateData(false);

	if (Permission() == FALSE)
	{
		HWND ADCTHwnd = NULL;
		ADCTHwnd = ::FindWindow("ADCT", "AutoDownloadATETest");
		MessageBox(_T("服务不通过！"), _T("Warning"), MB_SYSTEMMODAL | MB_ICONEXCLAMATION | MB_OK);
		::PostMessage(ADCTHwnd, WM_USER + 1001, 3, 99);
		OnCancel();
	}
	else
	{
		//三合一实现自动登陆
		if (g_ADCTFlag == 0)
		{
			AfxMessageBox("连接服务OK");//如果是三合一的话就不要显示
		}
		else if (g_ADCTFlag == 1)
		{
			OnBnClickedButtonstart13();
		}
	}
	//Story_Show();//显示Story
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CWriteIMEIDlg::OnPaint() 
{
	if (1)
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
		dc.FillSolidRect(rect,RGB(0,155,100)); 
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CWriteIMEIDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CWriteIMEIDlg::PrintError(CString cs)
{
	//AfxMessageBox(cs);
	extern COLORREF gColor;
	extern CString gFinalResult;
	gColor = RGB(255,0,0);

	CTime t = CTime::GetCurrentTime();
	szTemp.Format("%02d:%02d:%02d ",t.GetHour(),t.GetMinute(),t.GetSecond());
	//PlaySound((LPCTSTR)IDR_WAVE_fail, AfxGetInstanceHandle(), SND_RESOURCE | SND_ASYNC);
	m_Result.GetWindowText(LOG_Show_Old);

	m_Result.SetWindowText(szTemp+cs+"\r\n"+LOG_Show_Old);
	m_Result.UpdateWindow();
	m_Write.EnableWindow(true);

	gFinalResult.Format("     Fail");
	RedrawWindow();  m_Result.UpdateWindow(); 
	m_Result.GetWindowText(LOG_Show_Old);
}






int CWriteIMEIDlg::CharToHexChar(char ch)
{
   if((ch>='0')&&(ch<='9'))
   {
       return ch-0x30;

   }
   else if((ch>='A')&&(ch<='F'))
   {
       return ch-'A'+10;
   }
   else if((ch>='a')&&(ch<='f'))
   {
       return ch-'a'+10;  
   }      
   else
   {
       return -1;
   }
}


void CWriteIMEIDlg::OnWrite() 
{
	// TODO: Add your control notification handler code here
	//AfxMessageBox("test1");
	extern CString gFinalResult;
	extern COLORREF gColor;

	//AfxMessageBox(m_IMEI);
	int IMEI_length=m_IMEI.GetLength();//-SN_Count;
	
	int debug=0;//for debug use
	CString sPort,sBaud;//,sTemp;
	int port,baud,type;
	int IMEI_NUM;
	m_Result_C.Empty();//变量值清空
	//UpdateData(true);//把中间值值赋给变量
	//UpdateData(false);//把变量显示在界面
	m_Result.GetWindowText(LOG_Show_Old);  
	m_Result.SetWindowText(LOG_Show_Old);
	m_Result.UpdateWindow();
	//get port
	m_Port.GetWindowText(sPort);
	sPort=sPort.Right(sPort.GetLength()-3);
	port=atoi(sPort);
	sPort.Format(_T("\\\\.\\COM%d"),port);

	//get baud
	m_Baud.GetWindowText(sBaud);
	baud=atoi(sBaud);

	//get IMEI_NUM
	IMEI_NUM = m_Type.GetCurSel();

	extern COLORREF gColor;
	extern CString gFinalResult;
	gColor = RGB(0,100,100);
	gFinalResult.Format(" IDLE...");
	RedrawWindow();  m_Result.UpdateWindow(); 

	//disable write button
	m_Write.EnableWindow(FALSE);

	//open com port
	HANDLE hPort;
	hPort=CreateFile(sPort, GENERIC_READ|GENERIC_WRITE,0, NULL, 
		OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL, NULL);
	if(hPort == INVALID_HANDLE_VALUE)
	{
		CString csTemp;
		m_Port.GetWindowText(csTemp);
		PrintError("Can't open "+csTemp);
		hPort=NULL;
		m_Write.EnableWindow(true);
		OnSelchangeAuto();
		IMEI_Control.SetFocus();
		IMEI_Control.SetSel(0,-1);
		m_Result.GetWindowText(LOG_Show_Old);
		return;
	}

	//config the com port
	DCB dcb;
   	dcb.DCBlength = sizeof(DCB);
	GetCommState( hPort, &dcb ) ;
	SetupComm( hPort, 4096, 4096 ) ;
	PurgeComm(hPort,PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);
	dcb.Parity = NOPARITY;
	dcb.ByteSize = 8;
	dcb.StopBits = ONESTOPBIT;
	dcb.BaudRate = baud;     //57600(MT6205B), 115200 (MT6218B)			
	dcb.fBinary = TRUE;
	dcb.fParity = FALSE;
	dcb.fOutxCtsFlow = FALSE;
	dcb.fOutxDsrFlow = FALSE;
	dcb.fDtrControl = DTR_CONTROL_DISABLE;
	dcb.fDsrSensitivity = FALSE;
	dcb.fTXContinueOnXoff = FALSE;
	dcb.fOutX = FALSE;
	dcb.fInX = FALSE;
	dcb.fErrorChar = FALSE;
	dcb.fNull = FALSE;
	dcb.fRtsControl = RTS_CONTROL_ENABLE;
	dcb.fAbortOnError = FALSE;
	dcb.XonChar = 0;
	dcb.XoffChar = 0;
	dcb.ErrorChar = 0;
	dcb.EofChar = 0;
	dcb.EvtChar = 0;
	SetCommState(hPort, &dcb);

	//set time out struct
	COMMTIMEOUTS timeouts;
	timeouts.ReadIntervalTimeout = 0xFFFFFFFF;
	timeouts.ReadTotalTimeoutMultiplier = 0;
	timeouts.ReadTotalTimeoutConstant = 0;
	timeouts.WriteTotalTimeoutMultiplier = 0;
	timeouts.WriteTotalTimeoutConstant = 2000;
	SetCommTimeouts(hPort, &timeouts);

	//SetCommMask(hPort, EV_RXCHAR|EV_TXEMPTY );//设置事件驱动的类型

	//read and write the com port
	BOOL bReadStatus,bWriteStat;
	DWORD dwBytesWritten,dwBytesRead;
	char *buffer;
	char *p=NULL;
	CString command;
	char buf[1024];
	int timeout=1;
	int i;

	//write "AT"
	gFinalResult.Format("运行中..");
	RedrawWindow();  m_Result.UpdateWindow(); 
	for(i=0;i<5000;i++)
	{
		//send command
		//m_Result.GetWindowText(LOG_Show_Old);//不更新旧的LOG信息
		
		CString Times_wait;
		Times_wait.Format("%d",i);
		if(i%5==0)
		{
			m_Result.SetWindowText("==============================等待开机计数:"+Times_wait+"......\r\n"+LOG_Show_Old);
		}
		m_Result.UpdateWindow();


		command.Empty();
		command.Format("AT\r\n");
		buffer=command.GetBuffer(command.GetLength());
		bWriteStat = WriteFile( hPort, buffer, command.GetLength(), &dwBytesWritten,NULL );
		if( dwBytesWritten != command.GetLength() )
		{
			PrintError("串口没连接好，请检查串口！");

			if(debug)	PrintError("AT");
			CloseHandle(hPort);
			m_Write.EnableWindow(true);
			OnSelchangeAuto();
			IMEI_Control.SetFocus();
			IMEI_Control.SetSel(0,-1);
			return;
		}

		if(IMEI_length== 0)//异常处理
		{
			//AfxMessageBox("test2");
			//OnSelchangeAuto();    //此句会导致死循环 
			m_Result.GetWindowText(LOG_Show_Old);
			m_Result.SetWindowText("数据库LBC..\r\n\r\n"+LOG_Show_Old);
			m_Result.UpdateWindow();

			IMEI_Control.SetFocus();
			IMEI_Control.SetSel(0,-1);
			return;
		}
		else
		{
			//AfxMessageBox("test3"); 
		}
		
		//receive response
		Sleep(200);
		memset(buf,0,sizeof(buf));
		buffer=buf;
		bReadStatus = ReadFile( hPort, buffer, 10, &dwBytesRead, NULL);
		if(debug)
			PrintError(CString(buffer)+CString("."));
		if(dwBytesRead != 0)
		{
			p=strstr(buffer,"OK");
			if(p)
			{
				m_Result.GetWindowText(LOG_Show_Old);
				m_Result.SetWindowText("==============================手机已经开机！串口通信正常......\r\n"+LOG_Show_Old);
				m_Result.UpdateWindow();
				timeout=0;
				Sleep(500);
				break;
			}
		}
		Sleep(100);
	}
	//check if failed
	if(timeout == 1)
	{
		PrintError("串口没连接好，请检查串口！");
		//AfxMessageBox("串口没连接好，请检查串口！");
		if(debug)	PrintError("AT");
		CloseHandle(hPort);
		m_Write.EnableWindow(true);
		OnSelchangeAuto();
		IMEI_Control.SetFocus();
		IMEI_Control.SetSel(0,-1);
		return;
	}
	//make sure rx data cleaned
	Sleep(100);
	bReadStatus = ReadFile( hPort, buffer, 10, &dwBytesRead, NULL);
	//check imei
	//UpdateData(TRUE);
	
	
	//int IMEI_length=m_IMEI.GetLength();//-SN_Count;
	for(type=-3;type<0;type++)
	{
		timeout = 1;
		Sleep(1000);
		Sleep(50);
		for(i=0;i<1;i++)
		{
			//send command
			command.Empty();

			if(type==-3)
			{
				command.Format("AT+RPAR=2\r\n");//包含综测校准的板号
			}
			if(type==-2)
			{
				//command.Format("AT+CGMR\r\n");
				//command.Format("AT+RPAR=8\r\n");
				command=Order_Software_version+"\r\n";
			}
			else if(type==-1)
			{
				//command.Format("AT+RPAR=15\r\n");
				command=Order_Barcode+"\r\n";
			}
			else if(type == 0)
			{
				command.Format("ATD112");
			}
			else if(type == 1)
			{
				command.Format("ATH");	
			}
			buffer=command.GetBuffer(command.GetLength());
			bWriteStat = WriteFile( hPort, buffer, command.GetLength(), &dwBytesWritten,NULL );
			//Sleep(1000);
			if( dwBytesWritten != command.GetLength() )
			{
				PrintError("*写号失败，请重试！");

				CloseHandle(hPort);
				m_Write.EnableWindow(true);
				OnSelchangeAuto();
				IMEI_Control.SetFocus();
				IMEI_Control.SetSel(0,-1);
				return;
			}

			//receive response
			Sleep(200);	
			memset(buf,0,sizeof(buf));
			buffer=buf;
			if(type == 0)
				bReadStatus = ReadFile( hPort, buffer, 256, &dwBytesRead, NULL);
			else
				bReadStatus = ReadFile( hPort, buffer, 256, &dwBytesRead, NULL);
			if(debug)
				PrintError(CString(buffer));
			if(dwBytesRead != 0)
			{
				p=strstr(buffer,"OK");
				
				if(p)
				{
					 if(type==-1)
					 {
						 CString BoardNumber_temp;
						 BoardNumber_temp=CString(buffer);
						 int   nPosStr = BoardNumber_temp.Find(":");
						 CString  selPort = BoardNumber_temp.Mid(nPosStr+1);
						 nPosStr = selPort.Find("OK");
						 selPort= selPort.Left(nPosStr);	
						 selPort.Replace("\n","");
						 selPort.Replace("\r","");
						 {	
							CString string_sn_r;
							int npos=selPort.Find('"');
							if(npos>=0)
							{
								string_sn_r=selPort.Mid(npos+1);
								BoardNumber=string_sn_r.Left(string_sn_r.Find('"'));
							}
							else
							{
								BoardNumber=string_sn_r;
							}
							BARCODE_Input=BoardNumber;
						 }
					 }
					 if(type==-2)
					 {
						 CString Software_version_temp;
						 Software_version_temp=CString(buffer);
						 int   nPosStr = Software_version_temp.Find(":");
						 CString  selPort = Software_version_temp.Mid(nPosStr+1);
						 nPosStr = selPort.Find("OK");
						 selPort= selPort.Left(nPosStr);	
						 selPort.Replace("\r","");
						 selPort.Replace("\n","");
						 {	
							CString string_sn_r;
							int npos=selPort.Find('"');
							
							if(npos>=0)
							{
								string_sn_r=selPort.Mid(npos+1);
								Software_version=string_sn_r.Left(string_sn_r.Find('"'));
							}
							else
							{
								string_sn_r=selPort.Mid(npos+1);
								Software_version=string_sn_r;
							}
							if(Software_version.Find(Software_Version_M)>-1)
							{
								m_Result.GetWindowText(LOG_Show_Old);
								m_Result.SetWindowText("软件版本:"+Software_version+"正确！\r\n"+LOG_Show_Old);
								m_Result.UpdateWindow();
							}
							else
							{
								m_Result.GetWindowText(LOG_Show_Old);
								m_Result.SetWindowText("软件版本:"+Software_version+"错误！请再次核对话机软件版本！\r\n"+LOG_Show_Old);
								CloseHandle(hPort);     //软件版本判断失败
								m_Result.UpdateWindow();
								return;
							}
						 }
					 }

					 if(type==-3)
					 {
						 CString Software_version_temp;
						 Software_version_temp=CString(buffer);
						 int   nPosStr = Software_version_temp.Find(":");
						 CString  selPort = Software_version_temp.Mid(nPosStr+1);
						 nPosStr = selPort.Find("OK");
						 selPort= selPort.Left(nPosStr);	
						 selPort.Replace("\r","");
						 selPort.Replace("\n","");
						 {	
							CString string_sn_r;
							int npos=selPort.Find('"');
							
							if(npos>=0)
							{
								string_sn_r=selPort.Mid(npos+1);
								Software_version=string_sn_r.Left(string_sn_r.Find('"'));
							}
							else
							{
								string_sn_r=selPort.Mid(npos+1);
								Software_version=string_sn_r;
							}
							if(BT_Write_Enable==TRUE)//用于选择是否判断“M255中的10P”
							{
								if(Software_version.Find("10P")>-1)
								{
									m_Result.GetWindowText(LOG_Show_Old);
									m_Result.SetWindowText("PCBA板号:"+Software_version+"已经校准！\r\n"+LOG_Show_Old);
									m_Result.UpdateWindow();
								}
								else
								{
									m_Result.GetWindowText(LOG_Show_Old);
									m_Result.SetWindowText("PCBA板号:"+Software_version+"没有校准！请重新校准！\r\n"+LOG_Show_Old);
									CloseHandle(hPort);   //“10P”
									m_Result.UpdateWindow();

									gColor = RGB(255,0,0);
									gFinalResult.Format("PCBA_ERR");
									RedrawWindow();  m_Result.UpdateWindow(); 
									return;
								}
							}
							else
							{
								m_Result.GetWindowText(LOG_Show_Old);
								m_Result.SetWindowText("PCBA板号:"+Software_version+"没有校验是否校准，继续...\r\n"+LOG_Show_Old);
								m_Result.UpdateWindow();
							}
						 }
					 }

					 timeout=0;
					 break;
				}
			}

		}
		//check if failed
		//AfxMessageBox("IMEI_ok");
		if(timeout == 1)
		{
			PrintError("**写号的时候，话机没完全开机或者是串口不通==请重试！");

			CloseHandle(hPort);
			m_Write.EnableWindow(true);
			OnSelchangeAuto();
			IMEI_Control.SetFocus();
			IMEI_Control.SetSel(0,-1);
			return;
		}	
	}
	
	if(type == 0)
	{
				
		m_Write.EnableWindow(true);

		CTime CurrentTime=CTime::GetCurrentTime();
		CString strTime;
		strTime =CurrentTime.Format( "%Y-%m-%d %H:%M:%S ");

		
		//if(DB_CONNECT_M=="true")//AT指令模式下，暂时不用
		if(1)
		{
			/*int i;
			int ID[10];
			CString NAME[10];
			int TYPE[10];
			double VALUE[10];
	
			//CoInitialize(NULL);//初始化com环境
			Setdlg.myado.m_pRec->Open("select * from TB_LOADFORE_RESULT",_variant_t((IDispatch *) m_pCon,true),adOpenDynamic,adLockOptimistic, adCmdText);//打开新的记录集  
			
			Setdlg.myado.m_pRec->AddNew(); 
			Setdlg.myado.m_pRec->Fields->GetItem(_variant_t("DEVICEID"))->Value=_variant_t((short)ID[i]);
			Setdlg.myado.m_pRec->Fields->GetItem(_variant_t("DEVICENAME"))->Value=_bstr_t(NAME[i]);
			Setdlg.myado.m_pRec->Fields->GetItem(_variant_t("POINTTYPE"))->Value=_variant_t((short)TYPE[i]);
			Setdlg.myado.m_pRec->Update(); 

			//m_pRec->Close();
			//m_pCon->Close();
			//Setdlg.myado.m_pRec->PutCollect("state_time",(_variant_t)DataTime);

			Setdlg.myado.m_pRec->AddNew();    //加入一条记录
		    Setdlg.myado.m_pRec->Fields->GetItem("INTERID")->PutValue((long)1);  //INTERID字段加上一条记录
		    Setdlg.myado.m_pRec->Update();

		    _variant_t var_value;
		    var_value=pRst->Fields->GetItem("INTERID")->GetValue(); //读出一条记录*/

			m_Result.GetWindowText(LOG_Show_Old);
			m_Result.SetWindowText("==============================数据库数据判断......\r\n"+LOG_Show_Old);
			m_Result.UpdateWindow();
			
			
			
			CString Conn="";
			//Conn.Format("driver={SQL Server};Server=%s;DATABASE=%s;UID=%s;PWD=%s", Setdlg.m_server,Setdlg.m_db,Setdlg.m_user, Setdlg.m_pwd);
			Conn.Format("driver={SQL Server};Server=%s;DATABASE=%s;UID=%s;PWD=%s", m_server_M,m_db_M,m_user_M, m_pwd_M);
			//CString Conn="driver={SQL Server};Server=59.33.252.107;DATABASE=wipdb;UID=sa;PWD=wip";
			//if(MSN_Relative_IMEI==FALSE)
			{
				//////////////////////////////////////////////////////
				CString DB_FAIL;
				if(Setdlg.myado.m_pCon==NULL)
				{
					DB_FAIL=Setdlg.myado.ConnecDataLibrary(Conn,"","",adModeUnknown);
				}
				if(Setdlg.myado.m_pCon->State==0)  //1表示已经打开,0表示关闭，数据库断开，重连
				{
					m_Result.GetWindowText(LOG_Show_Old);  
					m_Result.SetWindowText("=================================数据库断开状态，进行连接........\r\n\r\n"+LOG_Show_Old);
					m_Result.UpdateWindow();

					DB_FAIL=Setdlg.myado.ConnecDataLibrary(Conn,"","",adModeUnknown);//数据库意外断开，重连
				}
				else
				{
					m_Result.GetWindowText(LOG_Show_Old);  
					m_Result.SetWindowText("=================================数据库连接正常中........\r\n\r\n"+LOG_Show_Old);
					m_Result.UpdateWindow();
				}
				////////////////////////////////////////////////////
				//////////////////////////////////////////////////////
				if(DB_FAIL=="FAIL")
				{
					//PlaySound((LPCTSTR)IDR_WAVE_fail, AfxGetInstanceHandle(), SND_RESOURCE | SND_ASYNC);
					m_Result.GetWindowText(LOG_Show_Old);
					m_Result.SetWindowText(strTime+":连接SQL数据库失败"+" \r\n\r\n"+LOG_Show_Old);
					gColor = RGB(255,0,0);
					gFinalResult.Format("UP_Fail");
					RedrawWindow();  m_Result.UpdateWindow(); 
					CloseHandle(hPort);   //连接SQL数据库失败
					return;
				}
			}

			bool UP_FAIL=false;
			bool UP_FAIL_ZTE=false;
			
			if(CHECK_GSM_TEST_Enable==TRUE)
			{
				Setdlg.myado.OpenSheet("select * from dse_test_record_table WHERE BoardNumber ='"+BoardNumber+"'");
				//BOOL Board_Check_GSM=Setdlg.myado.Find("BoardNumber='"+BoardNumber+"'");
				BOOL Board_Check_GSM=Setdlg.myado.Find("BoardNumber='"+BoardNumber+"'");
				Setdlg.myado.CloseSheet();
				if(Board_Check_GSM!=TRUE)
				{
					//PlaySound((LPCTSTR)IDR_WAVE_fail, AfxGetInstanceHandle(), SND_RESOURCE | SND_ASYNC);
					m_Result.GetWindowText(LOG_Show_Old);  
					m_Result.SetWindowText("================================板号:"+BoardNumber+"   漏掉了 GSM耦合 工位！！数据不上传，请检查，再次耦合==然后重新写号！！！\r\n\r\n"+LOG_Show_Old);
					gColor = RGB(255,0,0);
					gFinalResult.Format("     Fail");
					RedrawWindow();  m_Result.UpdateWindow(); 
					CloseHandle(hPort);  //漏掉了 GSM耦合 工位
					return ;

				}
				else
				{
					m_Result.GetWindowText(LOG_Show_Old);  
					m_Result.SetWindowText("=================================板号:"+BoardNumber+"   对应话机已经 GSM耦合 通过，继续写号！\r\n\r\n"+LOG_Show_Old);
					m_Result.UpdateWindow();
				}
				
			}
			else
			{
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("=================================板号:"+BoardNumber+"   对应话机没有检查 GSM耦合工位，继续写号！\r\n\r\n"+LOG_Show_Old);
				m_Result.UpdateWindow();
			}


			if(CHECK_BT_TEST_Enable==TRUE)
			{
				Setdlg.myado.OpenSheet("select * from GPSWifiBtTest_Table WHERE BoardNumber ='"+BoardNumber+"'");
				BOOL Board_Check_BT=Setdlg.myado.Find("BoardNumber='"+BoardNumber+"'");
				Setdlg.myado.CloseSheet();
				
				if(Board_Check_BT==FALSE)
				{
					//PlaySound((LPCTSTR)IDR_WAVE_fail, AfxGetInstanceHandle(), SND_RESOURCE | SND_ASYNC);
					m_Result.GetWindowText(LOG_Show_Old);  
					m_Result.SetWindowText("================================板号:"+BoardNumber+"   漏掉了 BT耦合 工位！！数据不上传，请检查，再次耦合==然后重新写号！！！\r\n\r\n"+LOG_Show_Old);
					gColor = RGB(255,0,0);
					gFinalResult.Format("     Fail");
					RedrawWindow();  m_Result.UpdateWindow(); 
					CloseHandle(hPort);//漏掉了 BT耦合 工位
					return ;

				}
				else
				{
					m_Result.GetWindowText(LOG_Show_Old);  
					m_Result.SetWindowText("=================================板号:"+BoardNumber+"   对应话机已经  BT耦合 通过，继续写号！\r\n\r\n"+LOG_Show_Old);
					m_Result.UpdateWindow();
				}
				
				
			}
			else
			{
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("=================================板号:"+BoardNumber+"   对应话机没有检查  BT耦合工位，继续写号！\r\n\r\n"+LOG_Show_Old);
				m_Result.UpdateWindow();
			}


		
			Setdlg.myado.OpenSheet("select barcode from dse_barcode_imei WHERE barcode ='"+BoardNumber+"'");
			BOOL Board_Check=Setdlg.myado.Find("barcode='"+BoardNumber+"'");
			Setdlg.myado.CloseSheet();
			if(Board_Check==TRUE)
			{
				//AfxMessageBox(Order_Num_up_M);
				//PlaySound((LPCTSTR)IDR_WAVE_fail, AfxGetInstanceHandle(), SND_RESOURCE | SND_ASYNC);
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("================================板号:"+BoardNumber+"   重复！！数据不上传，请检查后==重新写号！！！\r\n\r\n"+LOG_Show_Old);
				gColor = RGB(255,0,0);
				gFinalResult.Format("     Fail");
				RedrawWindow();  m_Result.UpdateWindow(); 
				CloseHandle(hPort);   //板号重复
				return ;

			}
			else
			{
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("=================================板号:"+BoardNumber+"   正常\r\n\r\n"+LOG_Show_Old);
				m_Result.UpdateWindow();
			}
			

			
			////////////////////////////////先检查数据库，成后写号////////////////////////////////////////////////////
			////////////////////////////////////////////////////////////////////////////////////
			for(type=0;type<4;type++)
			{
							
				//write imei
				timeout = 1;
				Sleep(50);
				for(i=0;i<2;i++)
				{
					//send command
					command.Empty();
					if(type==-2)
					{
						//command.Format("AT+CGMR\r\n");
						command=Order_Software_version+"\r\n";
					}
					else if(type==-1)
					{
						//command.Format("AT+RPAR=15\r\n");
						command=Order_Barcode+"\r\n";
					}
					else if(type == 0)
					{
						//command.Format("AT+WPAR=1,");//command.Format("AT+IMEI?");AT+WPAR=1,"123456789123456""123456789123456"，Order_IMEI
						if(IMEI1_Choose_M=="true") 
						{
							if(IMEI2_Choose_M=="true")
							{
								command=Order_IMEI+"\""+m_IMEI+"\"\""+m_IMEI+"\"\r\n";
								IMEI_Num="双(SIM)卡";
							}
							else
							{
								command=Order_IMEI+"\""+m_IMEI+"\"\r\n";
								IMEI_Num="单(SIM)卡";
							}
							
						}
						else
						{
							command="AT\r\n";
						}

					}
					else if(type == 1)
					{
						if(BT_MAC_ADDR_Choose_M=="true") //写蓝牙地址
						{
							command=Order_BT+"\""+m_BT_Addr+"\"\r\n";
						}
						else
						{
							command="AT\r\n";
							//command.Format("AT\r\n");
						}
						
						
					}
					else if(type == 3)
					{

						if(Recover_Status==TRUE)
						{
							command=Order_Recover_Status+"\r\n";
						}
						else
						{
							command="AT\r\n";
							//command.Format("AT\r\n");
						}
					}
					else if(type == 2)//数据备份 AT+ESDS=0;
					{
						if(0)
						{
							command=Order_Copy_Para+"\r\n";
						}
						else
						{
							command="AT\r\n";
							//command.Format("AT\r\n");
						}
						
						
					}
					buffer=command.GetBuffer(command.GetLength());
					bWriteStat = WriteFile( hPort, buffer, command.GetLength(), &dwBytesWritten,NULL );
					//Sleep(1000);
					if( dwBytesWritten != command.GetLength() )
					{
						PrintError("*写号失败，请重试！");
						CloseHandle(hPort);
						m_Write.EnableWindow(true);
						OnSelchangeAuto();
						IMEI_Control.SetFocus();
						IMEI_Control.SetSel(0,-1);
						return;
					}

					//receive response
					Sleep(500);	
					if(type == 2)
						Sleep(500);	//备份需要多等一下
					memset(buf,0,sizeof(buf));
					buffer=buf;
					if(type == 0)
						bReadStatus = ReadFile( hPort, buffer, 256, &dwBytesRead, NULL);
					else
						bReadStatus = ReadFile( hPort, buffer, 256, &dwBytesRead, NULL);
					if(debug)
						PrintError(CString(buffer));
					if(dwBytesRead != 0)
					{
						p=strstr(buffer,"OK");
						
						if(p)
						{
							 timeout=0;
							 break;
						}
					}

				}
				//check if failed
				//AfxMessageBox("IMEI_ok");
				if(timeout == 1)
				{
					PrintError("**写号的时候，话机没完全开机或者是串口不通==请重试！");
					CloseHandle(hPort);
					m_Write.EnableWindow(true);
					OnSelchangeAuto();
					IMEI_Control.SetFocus();
					IMEI_Control.SetSel(0,-1);
					return;
				}

			

				CTime t = CTime::GetCurrentTime();
				CString szTemp;
				szTemp.Format("%02d:%02d:%02d ",t.GetHour(),t.GetMinute(),t.GetSecond());
				m_Result.GetWindowText(LOG_Show_Old);
				//AfxMessageBox(LOG_Show_Old);
				if(type == 0)
				{	
					if(IMEI2_Choose_M=="true")
					{
						m_Result.SetWindowText(szTemp+"Succeed writing IMEI1 :"+m_IMEI+"======IMEI2 :"+m_IMEI2+".\r\n"+LOG_Show_Old);
						m_Result.UpdateWindow();
					}
					else
					{
						m_Result.SetWindowText(szTemp+"Succeed writing IMEI1:"+m_IMEI+".\r\n"+LOG_Show_Old);
						m_Result.UpdateWindow();
					}
				}
				else if((type == 1)&&(BT_MAC_ADDR_Choose_M=="true"))
				{

					m_Result.SetWindowText(szTemp+"Succeed writing BT_Addr:"+m_BT_Addr+".\r\n"+LOG_Show_Old);
					m_Result.UpdateWindow();

				}
				else if((type == 3)&&(Recover_Status==TRUE))
				{	
					m_Result.SetWindowText(szTemp+"成功发送恢复出厂设置指令:"+Order_Recover_Status+"    等待话机重启\r\n\r\n============若是电池供电，可拔下数据线，测试下一台话机！！！\r\n\r\n"+LOG_Show_Old);
					m_Result.UpdateWindow();
				}
				else if(type == 2)
				{	
					//AfxMessageBox("IMEI_OK");
					m_Result.SetWindowText(szTemp+"成功发送备份参数指令:"+Order_Copy_Para+"\r\n\r\n"+LOG_Show_Old);
					m_Result.UpdateWindow();
				}
			}
			CloseHandle(hPort);  //成功写完号

			///////////////////解锁码///////////////////////////////////////
			if(Unlock_Code_Choose_M=="true")
			{
				TranslateMuch(m_IMEI);
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("获取IMEI："+m_IMEI+"对应的解锁码："+Unlock_code_Number+"\r\n"+LOG_Show_Old);
				m_Result.UpdateWindow();
			}
			///////////////////解锁码///////////////////////////////////////

			//CString strSQL = "Insert into dse_barcode_imei(imei,IMEI_NUM,barcode,msn,WIP_ENTITY_NAME,TYPE,MOBCLASS,LINENUM,classnum,PRODADDRESS,create_time,is_return,SOFTVERSION,HARDVERSION,IMEI_WRITER,WIFI_MAC,BLUE_MAC) values('"+m_IMEI+"','"+IMEI_Num+"','"+BoardNumber+"','"+m_SN+"','"+Order_Num_up_M+"','"+Machine_Type_M+"','"+Modulation_Tppe_M+"','"+Line_Number_M+"','"+Floor_M+"','"+Product_Address_M+"','"+strTime+"','"+Is_Return_M+"','"+Software_Version_M+"','"+Hardware_Version_M+"','"+Operator_M+"','','"+m_BT_Addr+"')";
			CString strSQL = "Insert into dse_barcode_imei(imei,NetCode,Unlock_Code,IMEI2,IMEI3,IMEI4,barcode,msn,WIP_ENTITY_NAME,TYPE,LINENUM,create_time,is_return,SOFTVERSION,HARDVERSION,WIFI_MAC,BLUE_MAC)\
							 values('"+m_IMEI+"','"+NETCODE_Input+"','"+Unlock_code_Number+"','"+m_IMEI2+"','"+m_IMEI3+"','"+m_IMEI4+"','"+BARCODE_Input+"','"+m_SN+"','"+Order_Num_up_M+"','"+Machine_Type_M+"','"+Line_Number_M+"','"+strTime+"','"+Is_Return_M+"','"+Software_Version_M+"','"+Hardware_Version_M+"','"+m_strWifi+"','"+m_BT_Addr+"')";	
						
			Setdlg.myado.OpenSheet("select * from dse_barcode_imei");
			UP_FAIL=Setdlg.myado.Execute(strSQL);
			Setdlg.myado.CloseSheet();
			Sleep(500);
			Setdlg.myado.OpenSheet("select imei from dse_barcode_imei WHERE imei ='"+m_IMEI+"'");
			BOOL IMEI_Check_UP=Setdlg.myado.Find("imei='"+m_IMEI+"'");
			Setdlg.myado.CloseSheet();
			
			BOOL UP_WritedFlag;
			if((UP_FAIL==true&&IMEI_Check_UP==TRUE))//||UP_ONE_OK_SECOND_FAIL==BoardNumber)
			{//////////////////////////////////////////////////////////////////
				///////////////////////////////////////////////////////////////
				//AfxMessageBox("上传SQL成功！");
				


				m_Result.GetWindowText(LOG_Show_Old);
				m_Result.SetWindowText("成功写号！！！并且成功上传SQL_SERVER数据库！！！\r\n\r\n"+LOG_Show_Old);
				//UpdateData(true);  //把中间值赋给变量
				//UpdateData(false); //把变量显示在界面
				if(UpdateIMEI_MSN_Choose_M=="true")
				{
					CString strSQL_WritedFlag="UPDATE dse_imei_sn SET WritedFlag = '1' WHERE imei ='"+m_IMEI+"'";
					Setdlg.myado.OpenSheet("select * from dse_imei_sn WHERE imei ='"+m_IMEI+"'");
					UP_WritedFlag=Setdlg.myado.Execute(strSQL_WritedFlag);
					Setdlg.myado.CloseSheet();

					if(UP_WritedFlag==TRUE)
					{
						m_Result.GetWindowText(LOG_Show_Old);
						m_Result.SetWindowText("更新贴纸表标志成功！\r\n\r\n"+LOG_Show_Old);
					}
					else
					{
						m_Result.GetWindowText(LOG_Show_Old);
						m_Result.SetWindowText("更新贴纸表标志失败！！！\r\n\r\n"+LOG_Show_Old);
					}
					m_Result.UpdateWindow();
				}
				else
				{
					m_Result.GetWindowText(LOG_Show_Old);
					m_Result.SetWindowText("没有勾选更新贴纸表标志..\r\n\r\n"+LOG_Show_Old);
					m_Result.UpdateWindow();
					UP_WritedFlag=TRUE;
				}
				bool ORACLE_UpDatabase_RESULT=true;//上传ORACLE数据库结果
			    if(ORACLE_Connect_Choose_M=="true")//上传ORACLE数据库
				{
					//AfxMessageBox("Update_Oracle_Database");
					ORACLE_UpDatabase_RESULT=Update_Oracle_Database();   //AT指令写号
				}
				//if(ORACLE_UpDatabase_RESULT==true)
				if((ORACLE_UpDatabase_RESULT==true)&&(UP_WritedFlag==TRUE))
				{
					///////自动生成计数处理/////////////////////
					BtAddrCount_int++;
					BtAddrCount.Format("%d",BtAddrCount_int);

					IMEI_Count.Format("%d",IMEI_Count_int);  //IMEI_Count_int 已经递增
					SaveConfig_Change();
					//////////自动生成计数处理//////////////////
					
					m_Result.GetWindowText(LOG_Show_Old);
					m_Result.SetWindowText("数据库上传所有操作OK..\r\n\r\n"+LOG_Show_Old);
					m_Result.UpdateWindow();
					m_Result.GetWindowText(LOG_Show_Old_ALL);

					gColor = RGB(0,255,0);
					gFinalResult.Format("     OK");
					RedrawWindow();  m_Result.UpdateWindow(); 
					IMEI_OK_LAST=m_IMEI;
					OnSelchangeAuto();    //不将IMEI1清零 
					//clear IMEI
					//m_IMEI="";
					CEdit *pEdit1;
					pEdit1=(CEdit*)GetDlgItem(IDC_IMEI);
					pEdit1->SetWindowText("");
					Sleep(100);
					pEdit1->SetWindowText("m_IMEI");

					pEdit1->SetWindowText("");
					m_IMEI="";

					
					IMEI_Control.SetFocus();
					IMEI_Control.SetSel(0,-1);
				}
				else 
				{
					if(UP_WritedFlag!=TRUE)
					{
						m_Result.GetWindowText(LOG_Show_Old);
						m_Result.SetWindowText("成功写号上传数据！！！但更新贴纸表标志失败！！！！！！！！！！！！！\r\n\r\n"+LOG_Show_Old);
						m_Result.UpdateWindow();
					}

					gColor = RGB(255,0,0);
					gFinalResult.Format("UPDA_F");
					RedrawWindow();  m_Result.UpdateWindow(); 
				}
				
			
			}
			else
			{
				//PlaySound((LPCTSTR)IDR_WAVE_fail, AfxGetInstanceHandle(), SND_RESOURCE | SND_ASYNC);
				m_Result.GetWindowText(LOG_Show_Old);
				m_Result.SetWindowText("成功写号！！！但上传数据库失败！！！！！！！！！！！！！\r\n\r\n"+LOG_Show_Old);
				//UpdateData(true);  //把中间值赋给变量
				//UpdateData(false); //把变量显示在界面
				m_Result.UpdateWindow();

				gColor = RGB(255,0,0);
				gFinalResult.Format("UP_FAIL");
				RedrawWindow();  m_Result.UpdateWindow(); 
			}
			
		  }
		  else
		  {
		
			  BtAddrCount_int++;
			  BtAddrCount.Format("%d",BtAddrCount_int);

			  SaveConfig_Change();
			  m_Result.GetWindowText(LOG_Show_Old);
			  m_Result.SetWindowText(strTime+":数据库关闭了，手机板号为："+BoardNumber+"对应的IMEI数据"+"\r\n=======================不上传数据库......"+" \r\n\r\n"+LOG_Show_Old);
			  m_Result.UpdateWindow();

			  gColor = RGB(0,255,0);
			  gFinalResult.Format("     OK");
			  RedrawWindow();  m_Result.UpdateWindow(); 
		  }	
	}
	OnSelchangeAuto();
	//IMEI_Control.SetFocus();
	//IMEI_Control.SetSel(0,-1);
	

}

	/*//read imei and check
		timeout = 1;
		Sleep(50);
		for(i=0;i<8;i++)
		{
			//send command
			command.Empty();
			if(type == 0)
				//command.Format("AT+EGMR=0,5\r\n");
				command.Format("AT+IMEI?\r\n");
			else
				command.Format("AT+WPAR=6\r\n");
			buffer=command.GetBuffer(command.GetLength());
			bWriteStat = WriteFile( hPort, buffer, command.GetLength(), &dwBytesWritten,NULL );
			if( dwBytesWritten != command.GetLength() )
			{
				PrintError("***写号失败，请重试！");
				//if(debug)	PrintError("AT+EGMR=0");
				CloseHandle(hPort);     //AT读出IMEI\MAC检查  已屏蔽
				m_Write.EnableWindow(true);
				return;
			}

			//receive response
			Sleep(200);	
			memset(buf,0,sizeof(buf));
			buffer=buf;
			if(type == 0)
				bReadStatus = ReadFile( hPort, buffer, 80, &dwBytesRead, NULL);
			else
				bReadStatus = ReadFile( hPort, buffer, 60, &dwBytesRead, NULL);
			if(debug)
				PrintError(CString(buffer));
			if(dwBytesRead != 0)
			{
				//check readback imei
				p=strstr(buffer,"OK");
				if(p)
				{
					timeout=0;

					CString aa;
					aa.Format("%s",buffer);
					int nPosStr = aa.Find(":");
		            CString  selPort = aa.Mid(nPosStr+1);
		            nPosStr = selPort.Find("OK");
			        selPort= selPort.Left(nPosStr);	
				    selPort.Replace("\r\n","");
				    if(0)
				    {	
						CString string_BT_r;
						int npos=selPort.Find('"');
						if(npos>=0)
							string_BT_r=selPort.Mid(npos+1);
						string_BT=string_BT_r.Left(string_BT_r.Find('"'));
						AfxMessageBox(string_BT);
				    }

					break;
				}
			}

		}
		//check if failed
		if(timeout == 1)
		{
			PrintError("****写号失败，请重试！");
			if(debug)	PrintError("AT+EGMR=0");
			CloseHandle(hPort);    //AT读出IMEI\MAC检查  已屏蔽
			m_Write.EnableWindow(true);
			return;
		}*/

void CWriteIMEIDlg::OnSelchangeAuto() 
{
	// TODO: Add your control notification handler code here
	//m_Result.UpdateWindow();  //因为RedrawWindow();  m_Result.UpdateWindow(); 
	m_Write.EnableWindow(TRUE);

	//clear IMEI
	//CEdit *pEdit1;
	//pEdit1=(CEdit*)GetDlgItem(IDC_IMEI);
	//pEdit1->SetWindowText("");
}

void CWriteIMEIDlg::BT_WIFI_Choose() //BI/WIFI地址写选择,另外+入网标
{
	if(((AutoBTAddr_Choose_M=="true")||(AutoWIFIAddr_Choose_M=="true"))  &&  (NETCODE_Choose_M=="false")  )//BT/WIFI地址递增生成
	{
		if((Scan_Choose_M=="true")&&(INITIAL_POWER==true))
			MAIN_Function();
	}
	else if((WIFI_MAC_ADDR_Choose_M=="false")&&(BT_MAC_ADDR_Choose_M=="false")&&(NETCODE_Choose_M=="false"))
	{
		if((Scan_Choose_M=="true")&&(INITIAL_POWER==true))
			MAIN_Function();//只有IMEI号
		else
		{
			IMEI_Control.SetFocus();
			IMEI_Control.SetSel(0,-1);
			return;
		}
	}
	else if(((AutoBTAddr_Choose_M=="true")||(AutoWIFIAddr_Choose_M=="true"))  &&(NETCODE_Choose_M=="true"))
	{
		NETCODE_Control.SetFocus();
		NETCODE_Control.SetSel(0,-1);
	}
	else if(BT_MAC_ADDR_Choose_M=="true")
	{
		m_BT_Addr_Control.SetFocus();
		m_BT_Addr_Control.SetSel(0,-1);
	}
	else if(WIFI_MAC_ADDR_Choose_M=="true")
	{
		m_WIFI_Addr_Control.SetFocus();
		m_WIFI_Addr_Control.SetSel(0,-1);
	}
	else if(NETCODE_Choose_M=="true")
	{
		NETCODE_Control.SetFocus();
		NETCODE_Control.SetSel(0,-1);
	}
}
void CWriteIMEIDlg::OnChangeImei() //选择IMEI1
{
	UpdateData(TRUE);
	UpdateData(false);//把变量显示在界面
	int IMEI_length=m_IMEI.GetLength();//-SN_Count;
	if(IMEI_length== 0)//异常处理
	{
		//OnSelchangeAuto();    //此句会导致死循环
		IMEI_Control.SetFocus();
		IMEI_Control.SetSel(0,-1);
		return;
	}

	//extern COLORREF gColor;
	//extern CString gFinalResult;
	if(INITIAL_PROCESS==false)
	{
		m_Result.SetWindowText("开始运行......\r\n\r\n");
		INITIAL_PROCESS=true;
	}

	//int IMEI_length=m_IMEI.GetLength();//-SN_Count;
	if(IMEI_length== 0)//异常处理
	{
		//OnSelchangeAuto();    //此句会导致死循环
		IMEI_Control.SetFocus();
		IMEI_Control.SetSel(0,-1);
		return;
	}
	LOG_Show_Old="";
	m_Result.SetWindowText(LOG_Show_Old);
	UpdateData(true);  //把中间值赋给变量
	UpdateData(false); //把变量显示在界面
	m_Result.UpdateWindow();

	

	if(m_IMEI.Find(IMEI_PRE)!=-1&&   ((m_IMEI.Find("\r")!=-1)|| (m_IMEI.Find("\n")!=-1))  )//选择了IMEI1  &&IMEI_length==15
	{
		//check whether digits
		m_IMEI.Replace("\r","");
		m_IMEI.Replace("\n","");
		IMEI_length=m_IMEI.GetLength();
		UpdateData(FALSE);
		for(int i=0;i<IMEI_length;i++)
		{
			//if(IMEI_length== 15&&(m_IMEI.GetAt(i) <'0' || m_IMEI.GetAt(i) >'9'))
			if(IMEI_length!= 15||(m_IMEI.GetAt(i) <'0' || m_IMEI.GetAt(i) >'9'))
			{
				PrintError("Please check IMEI. Only digits allowed!");
				m_Write.EnableWindow(true);
				OnSelchangeAuto();
				IMEI_Control.SetFocus();
				IMEI_Control.SetSel(0,-1);
				return;
			}
		}
		//////////////////////////////////////////////////
		//////////////////////////////////////////////////
		CString IMEIStart_M_temp;
		//long    IMEIStart_Number;                //作为全局变量
		IMEIStart_M_temp=IMEIStart_M.Left(14);
		IMEIStart_M_temp=IMEIStart_M_temp.Right(6);
		IMEIStart_Number=atol(IMEIStart_M_temp);   //IMEI起始号

		CString IMEIEnd_M_temp;  
		//long    IMEIEnd_Number;                  //作为全局变量
		IMEIEnd_M_temp=IMEIEnd_M.Left(14);
		IMEIEnd_M_temp=IMEIEnd_M_temp.Right(6);
		IMEIEnd_Number=atol(IMEIEnd_M_temp);       //IMEI结束号
		//////////////////////////////////////////////////
		//////////////////////////////////////////////////

		CString m_IMEI_Limit;
		long    m_IMEI_Number;
		m_IMEI_Limit=m_IMEI.Left(14);
		m_IMEI_Limit=m_IMEI_Limit.Right(6);
		m_IMEI_Number=atol(m_IMEI_Limit);          //IMEI1的6位串号
		if( ((m_IMEI_Number>IMEIEnd_Number)||(m_IMEI_Number<IMEIStart_Number)) )//检查号段
		{
			PrintError("输入的IMEI已超过限定的号段，请重新扫描! \r\n\r\n");
			m_Write.EnableWindow(true);
			OnSelchangeAuto();
			IMEI_Control.SetFocus();
			IMEI_Control.SetSel(0,-1);
			return;
		}
		else
		{
			m_Result.GetWindowText(LOG_Show_Old);
			m_Result.SetWindowText("输入的IMEI在限定的号段内，继续中...OK-OK-OK-OK-OK-OK-OK-OK-OK-OK-OK-OK-OK-OK-OK-OK-OK-OK-OK-OK-OK-OK-OK-OK-OK-OK-OK! \r\nOK-OK-OK-OK-OK-OK-OK-OK-OK-OK-OK-OK-OK-OK-OK-OK-OK-OK-OK-OK-OK-OK-OK-OK-OK-OK-OK!\r\n"+LOG_Show_Old);
			UpdateData(true);  //把中间值赋给变量
			UpdateData(false); //把变量显示在界面
			m_Result.UpdateWindow();
		}

		CString Conn="";
		CString DB_FAIL="";
		CString strSQL;
		bool UP_FAIL=false;
		_variant_t var;
		CString Number_temp= "N'"+m_IMEI+"'";
		LPCTSTR  SNO="sno";
		//////////////////////////////////////////////////////
		////////MSN//////////////////////////
		if(MSN_Upload_Choose_M=="true")//将上传MSN
		{
			//必须先检查是否存在该IMEI
			Setdlg.myado.OpenSheet("select imei from dse_imei_sn WHERE imei ='"+m_IMEI+"'");//1
			BOOL IMEI_Check_Exist=Setdlg.myado.Find("imei='"+m_IMEI+"'");                   //检查IMEI是否在数据库表dse_imei_sn中存在
			Setdlg.myado.CloseSheet();
			//必须先检查是否存在该IMEI
			strSQL="select * FROM dse_imei_sn WHERE (imei = "+Number_temp+")";
			UP_FAIL=Setdlg.myado.Execute(strSQL);	
			if((UP_FAIL==true)&&(IMEI_Check_Exist==TRUE))
			{
				Setdlg.myado.m_pRec.CreateInstance(__uuidof(Recordset));
				Setdlg.myado.m_pRec->Open((_bstr_t)strSQL,Setdlg.myado.m_pCon.GetInterfacePtr(),adOpenDynamic,adLockOptimistic,adCmdText);
				var= Setdlg.myado.m_pRec->GetCollect(SNO);//var= Setdlg.myado.m_pRec->GetCollect(_variant_t((long)3));
				if(var.vt != VT_NULL)
					m_SN.Format("%s",(LPCSTR)(_bstr_t)var);
				else
				{	
					m_SN="NULL";
					MessageBox("贴纸表中对应的MSN数据为空，若本身为空，请不要勾选“上传MSN”","严重警告",MB_ICONWARNING);
					AfxMessageBox("贴纸表中对应的MSN数据为空，若本身为空，请不要勾选“上传MSN”");
				}
				Setdlg.myado.m_pRec->Close();
				UpdateData(false);//把变量显示在界面,这一步非常重要，  在执行UpdateData(TRUE); 之前
			}
			else
			{
				if(IMEI_Check_Exist!=TRUE)
				{
					m_Result.GetWindowText(LOG_Show_Old);
					m_Result.SetWindowText("数据库中表 dse_imei_sn 不存在IMEI"+m_IMEI+", 所以无法从数据库获取MSN \r\n\r\n"+LOG_Show_Old);
					m_Result.UpdateWindow();
				}
				else
				{
					m_Result.GetWindowText(LOG_Show_Old);
					m_Result.SetWindowText("连接数据库失败，无法从数据库获取MSN \r\n\r\n"+LOG_Show_Old);
					m_Result.UpdateWindow();
				}
				var.vt = VT_NULL;
				OnSelchangeAuto();
				IMEI_Control.SetFocus();
				IMEI_Control.SetSel(0,-1);
				
				return;
			}

			int m_sn_check=m_SN.GetLength();
			if((m_sn_check!=SN_Count)||(m_SN.Find(SN_PRE)==-1))
			{
				//PlaySound((LPCTSTR)IDR_WAVE_fail, AfxGetInstanceHandle(), SND_RESOURCE | SND_ASYNC);
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("===================================SN:"+m_SN+"   前缀或位数异常，请重试！！！\r\n\r\n"+LOG_Show_Old);	
				m_Result.UpdateWindow();
				IMEI_Control.SetFocus();
				IMEI_Control.SetSel(0,-1);
				return;
			}
			else
			{
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("===================================SN:"+m_SN+"   前缀和位数校验正常，继续...\r\n\r\n"+LOG_Show_Old);	
				m_Result.UpdateWindow();
			}

			if(Setdlg.myado.m_pCon==NULL)
			{
				DB_FAIL=Setdlg.myado.ConnecDataLibrary(Conn,"","",adModeUnknown);   
			}
			if(Setdlg.myado.m_pCon->State==0)  //1表示已经打开,0表示关闭，数据库意外断开，重连
			{
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("=================================数据库意外断开，重连........\r\n\r\n"+LOG_Show_Old);
				m_Result.UpdateWindow();
				DB_FAIL=Setdlg.myado.ConnecDataLibrary(Conn,"","",adModeUnknown);//数据库意外断开，重连
			}
			else
			{
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("=================================数据库连接正常中...\r\n\r\n"+LOG_Show_Old);
				m_Result.UpdateWindow();
			}

			if(DB_FAIL=="FAIL")
			{
				//OnSelchangeAuto();
				IMEI_Control.SetFocus();
				IMEI_Control.SetSel(0,-1);
				m_Result.GetWindowText(LOG_Show_Old);
				m_Result.SetWindowText("连接数据库失败，无法获取IMEI1对应的其他MSN，请检查网络.......\r\n\r\n"+LOG_Show_Old);
				return;
			}

			Setdlg.myado.OpenSheet("select msn from dse_barcode_imei WHERE msn ='"+m_SN+"'");
			BOOL SN_Check=Setdlg.myado.Find("msn='"+m_SN+"'");
			Setdlg.myado.CloseSheet();
			if(SN_Check==TRUE)
			{
				//PlaySound((LPCTSTR)IDR_WAVE_fail, AfxGetInstanceHandle(), SND_RESOURCE | SND_ASYNC);
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("===================================SN:"+m_SN+"   重复！！数据不上传，请检查后==重新写号！！！\r\n\r\n"+LOG_Show_Old);
				IMEI_Control.SetFocus();
				IMEI_Control.SetSel(0,-1);
				return;

			}
			else
			{
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("====================================SN:"+m_SN+"   数据库正常\r\n\r\n"+LOG_Show_Old);
				m_Result.UpdateWindow();
			}
		}
		UpdateData(TRUE);//这一步非常重要，把m_Result窗口的值  赋给了对应的变量，在执行  UpdateData(false);之前
		//m_Result.SetWindowText(LOG_Show_Old);
		//m_Result.UpdateWindow();
		//m_Result.GetWindowText(LOG_Show_Old); 
		////////MSN//////////////////////////
		//////////////////////////////////////////////////////

		if((UseSameIMEI_Choose_M=="true"))         //采用相同的IMEI，则判断是否需要些BT/WIFI MAC
		{
			if(IMEI2_Choose_M=="true")
				m_IMEI2=m_IMEI;
			if(IMEI3_Choose_M=="true")
				m_IMEI3=m_IMEI;
			if(IMEI4_Choose_M=="true")
				m_IMEI4=m_IMEI;
			UpdateData(false);
			IMEI2_Result=true;
			IMEI3_Result=true;
			IMEI4_Result=true;
			
			BT_WIFI_Choose();		//采用相同的IMEI，则判断是否需要写BT/WIFI MAC----无需做从数据库获取，采用自动生成最好
		}//(Initial_Connect_DB_M=="true")初始化连接了数据库才能获取，权限限制
		else if(     (Initial_Connect_DB_M=="true")&&( ((UseSameIMEI_Choose_M=="false")&&(IMEI_DB_Choose_M=="true"))||(MAC_DB_Choose_M=="true") )   )//采样不同IMEI,且根据数据库关联其他IMEI或者BT/WIFI MAC
		{
			//其他IMEI---从数据库获取
			int  INPUT_IMEI_NUMBER=0;//扫面输入的是数据库中贴纸表中的第几个IMEI号。
			
			Conn.Format("driver={SQL Server};Server=%s;DATABASE=%s;UID=%s;PWD=%s", m_server_M,m_db_M,m_user_M, m_pwd_M);

			/*if((DB_CONNECT_M=="true")  ||  ((ORACLE_Connect_Choose_M=="true")&&(UP_DB_Data_M=="true")))//每次操作数据库时候，重连一次，否则初始化时必须连接数据库  ====另外连接SQL_SERVER和ORACLE数据库上传时，因为涉及数据连接的转换，需要连接
				DB_FAIL=Setdlg.myado.ConnecDataLibrary(Conn,"","",adModeUnknown);//数据库获取数据时，重连数据库，==========初始化连接了数据库
			else
				DB_FAIL="SUCCESS";*/
			if(Setdlg.myado.m_pCon==NULL)
			{
				DB_FAIL=Setdlg.myado.ConnecDataLibrary(Conn,"","",adModeUnknown);   
			}
			if(Setdlg.myado.m_pCon->State==0)  //1表示已经打开,0表示关闭，数据库意外断开，重连
			{
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("=================================数据库意外断开，重连........\r\n\r\n"+LOG_Show_Old);
				m_Result.UpdateWindow();
				DB_FAIL=Setdlg.myado.ConnecDataLibrary(Conn,"","",adModeUnknown);//数据库意外断开，重连
			}
			else
			{
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("=================================数据库连接正常中...\r\n\r\n"+LOG_Show_Old);
				m_Result.UpdateWindow();
			}

			if(DB_FAIL=="FAIL")
			{
				OnSelchangeAuto();
				IMEI_Control.SetFocus();
				IMEI_Control.SetSel(0,-1);
				m_Result.GetWindowText(LOG_Show_Old);
				m_Result.SetWindowText("连接数据库失败，无法获取IMEI1对应的其他IMEI、BT_WIFI MAC，请检查网络.......\r\n\r\n"+LOG_Show_Old);
				return;
			}
			else
			{
				
				LPCTSTR  IMEI1="imei";
				LPCTSTR  IMEI2="IMEI2";
				LPCTSTR  IMEI3="IMEI3";
				LPCTSTR  IMEI4="IMEI4";

				LPCTSTR  BT_MAC="BLUE_MAC";
				LPCTSTR  WIFI_MAC="WIFI_MAC";
				


				//首先判断IMEI1在数据库中是否存在
				Setdlg.myado.OpenSheet("select imei from dse_imei_sn WHERE imei ='"+m_IMEI+"'");
				BOOL IMEI_Check=Setdlg.myado.Find("imei='"+m_IMEI+"'");
				Setdlg.myado.CloseSheet();
				if(IMEI_Check==TRUE)
				{
					//PlaySound((LPCTSTR)IDR_WAVE_fail, AfxGetInstanceHandle(), SND_RESOURCE | SND_ASYNC);
					m_Result.GetWindowText(LOG_Show_Old);  
					m_Result.SetWindowText("=================================IMEI:"+m_IMEI+"   数据库贴纸表中存在，正在获取IMEI1对应的其他IMEI、BT_WIFI MAC........\r\n\r\n"+LOG_Show_Old);
					m_Result.UpdateWindow();
					INPUT_IMEI_NUMBER=1;
				}
				else
				{
					if(Repeat_AutoStart_M=="false")
					{
						m_Result.GetWindowText(LOG_Show_Old);  
						m_Result.SetWindowText("==================================IMEI:"+m_IMEI+"   数据库贴纸表中 不存在，无法获取IMEI1对应的其他IMEI、BT_WIFI MAC！！！！！\r\n\r\n"+LOG_Show_Old);
						m_Result.UpdateWindow();
						////////////////////////////////////////////////////////////////////////////////////////////////////IMEI2
					}
					else
					{
						
					//////////////////对其他IMEI做判断//////////////////////////
					//////////////////对其他IMEI做判断//////////////////////////
					//////////////////对其他IMEI做判断//////////////////////////
					//首先判断IMEI2在数据库中是否存在
						m_Result.GetWindowText(LOG_Show_Old);  
						m_Result.SetWindowText("==================================IMEI:"+m_IMEI+"   数据库贴纸表中 不存在，判断是否为IMEI2进行中.....\r\n\r\n"+LOG_Show_Old);
						m_Result.UpdateWindow();

						Setdlg.myado.OpenSheet("select IMEI2 from dse_imei_sn WHERE IMEI2 ='"+m_IMEI+"'");
						BOOL IMEI_Check=Setdlg.myado.Find("IMEI2='"+m_IMEI+"'");
						Setdlg.myado.CloseSheet();
						if(IMEI_Check==TRUE)
						{
							m_Result.GetWindowText(LOG_Show_Old);  
							m_Result.SetWindowText("=================================IMEI2:"+m_IMEI+"   数据库贴纸表中存在，正在获取IMEI2对应的其他IMEI、BT_WIFI MAC........\r\n\r\n"+LOG_Show_Old);
							m_Result.UpdateWindow();
							INPUT_IMEI_NUMBER=2;
						}
						else
						{
							m_Result.GetWindowText(LOG_Show_Old);  
							m_Result.SetWindowText("==================================IMEI2:"+m_IMEI+"   数据库贴纸表中 不存在，判断是否为IMEI3进行中.....\r\n\r\n"+LOG_Show_Old);
							m_Result.UpdateWindow();
							////////////////////////////////////////////////////////////////////////////////////////////////////IMEI3
							Setdlg.myado.OpenSheet("select IMEI3 from dse_imei_sn WHERE IMEI3 ='"+m_IMEI+"'");
							BOOL IMEI_Check=Setdlg.myado.Find("IMEI3='"+m_IMEI+"'");
							Setdlg.myado.CloseSheet();
							if(IMEI_Check==TRUE)
							{
								m_Result.GetWindowText(LOG_Show_Old);  
								m_Result.SetWindowText("=================================IMEI3:"+m_IMEI+"   数据库贴纸表中存在，正在获取IMEI3对应的其他IMEI、BT_WIFI MAC........\r\n\r\n"+LOG_Show_Old);
								m_Result.UpdateWindow();
								INPUT_IMEI_NUMBER=3;
							}
							else
							{
								m_Result.GetWindowText(LOG_Show_Old);  
								m_Result.SetWindowText("==================================IMEI3:"+m_IMEI+"   数据库贴纸表中 不存在，判断是否为IMEI4进行中.....\r\n\r\n"+LOG_Show_Old);
								m_Result.UpdateWindow();
								////////////////////////////////////////////////////////////////////////////////////////////////////IMEI4
								Setdlg.myado.OpenSheet("select IMEI4 from dse_imei_sn WHERE IMEI4 ='"+m_IMEI+"'");
								BOOL IMEI_Check=Setdlg.myado.Find("IMEI4='"+m_IMEI+"'");
								Setdlg.myado.CloseSheet();
								if(IMEI_Check==TRUE)
								{
									m_Result.GetWindowText(LOG_Show_Old);  
									m_Result.SetWindowText("=================================IMEI4:"+m_IMEI+"   数据库贴纸表中存在，正在获取IMEI4对应的其他IMEI、BT_WIFI MAC........\r\n\r\n"+LOG_Show_Old);
									m_Result.UpdateWindow();
									INPUT_IMEI_NUMBER=4;
								}
								else
								{
									m_Result.GetWindowText(LOG_Show_Old);  
									m_Result.SetWindowText("==================================IMEI4:"+m_IMEI+"   数据库贴纸表中 不存在，=====数据库中不存在此IMEI号！！！！！！！！！！！！！\r\n\r\n"+LOG_Show_Old);
									m_Result.UpdateWindow();
									INPUT_IMEI_NUMBER=0;
								}
							}
						}
					}
					//////////////////对其他IMEI做判断//////////////////////////
					if(INPUT_IMEI_NUMBER==0)    //数据库中表：dse_imei_sn 不存在输入的IMEI，不会再调用myado.m_pRec->GetCollect   直接返回，不会再出错
					{
						IMEI_Control.SetFocus();
						IMEI_Control.SetSel(0,-1);
						return;
					}
				}
				
				////////////////////////////////
				
				
				
				if(INPUT_IMEI_NUMBER==1)
					strSQL="select * FROM dse_imei_sn WHERE (imei = "+Number_temp+")";// WHERE (IMEI LIKE '[1]%'
				else if(INPUT_IMEI_NUMBER==2)
					strSQL="select * FROM dse_imei_sn WHERE (IMEI2 = "+Number_temp+")";
				else if(INPUT_IMEI_NUMBER==3)
					strSQL="select * FROM dse_imei_sn WHERE (IMEI3 = "+Number_temp+")";
				else if(INPUT_IMEI_NUMBER==4)
					strSQL="select * FROM dse_imei_sn WHERE (IMEI4 = "+Number_temp+")";


				UP_FAIL=Setdlg.myado.Execute(strSQL);	
				if(UP_FAIL==true)
				{
					Setdlg.myado.m_pRec.CreateInstance(__uuidof(Recordset));
					Setdlg.myado.m_pRec->Open((_bstr_t)strSQL,Setdlg.myado.m_pCon.GetInterfacePtr(),adOpenDynamic,adLockOptimistic,adCmdText);
					
					if((UseSameIMEI_Choose_M=="false")&&(IMEI_DB_Choose_M=="true"))//排除一下情况：数据库关联只关联MAC时
					{
						if(IMEI1_Choose_M=="true")
						{
							var= Setdlg.myado.m_pRec->GetCollect(IMEI1);
							if(var.vt != VT_NULL)
								m_IMEI.Format("%s",(LPCSTR)(_bstr_t)var);
							else
							{
								m_IMEI="NULL";
								MessageBox("贴纸表中对应的IMEI1数据为空，若本身为空，请不要勾选“IMEI1”","严重警告",MB_ICONWARNING);
								AfxMessageBox("贴纸表中对应的IMEI1数据为空，若本身为空，请不要勾选“IMEI1”");
							}
						}
						if(IMEI2_Choose_M=="true")
						{
							var= Setdlg.myado.m_pRec->GetCollect(IMEI2);
							if(var.vt != VT_NULL)
								m_IMEI2.Format("%s",(LPCSTR)(_bstr_t)var);
							else
							{
								m_IMEI2="NULL";
								MessageBox("贴纸表中对应的IMEI2数据为空，若本身为空，请不要勾选“IMEI2”","严重警告",MB_ICONWARNING);
								AfxMessageBox("贴纸表中对应的IMEI2数据为空，若本身为空，请不要勾选“IMEI2”");
							}

						}
						if(IMEI3_Choose_M=="true")
						{
							var= Setdlg.myado.m_pRec->GetCollect(IMEI3);
							if(var.vt != VT_NULL)
								m_IMEI3.Format("%s",(LPCSTR)(_bstr_t)var);
							else
							{
								m_IMEI3="NULL";
								MessageBox("贴纸表中对应的IMEI3数据为空，若本身为空，请不要勾选“IMEI3”","严重警告",MB_ICONWARNING);
								AfxMessageBox("贴纸表中对应的IMEI3数据为空，若本身为空，请不要勾选“IMEI3”");
							}
						}
						if(IMEI4_Choose_M=="true")
						{
							var= Setdlg.myado.m_pRec->GetCollect(IMEI4);
							if(var.vt != VT_NULL)
								m_IMEI4.Format("%s",(LPCSTR)(_bstr_t)var);
							else 
							{
								m_IMEI4="NULL";
								MessageBox("贴纸表中对应的IMEI4数据为空，若本身为空，请不要勾选“IMEI4”","严重警告",MB_ICONWARNING);
								AfxMessageBox("贴纸表中对应的IMEI4数据为空，若本身为空，请不要勾选“IMEI4”");
							}
						}
					}


					UpdateData(false);	
					if(MAC_DB_Choose_M=="true")
					{
						if(BT_MAC_ADDR_Choose_M=="true")
						{
							var= Setdlg.myado.m_pRec->GetCollect(BT_MAC);
							if(var.vt != VT_NULL)
								m_BT_Addr.Format("%s",(LPCSTR)(_bstr_t)var);
							else
							{
								m_BT_Addr="NULL";
								MessageBox("贴纸表中对应的BT_MAC地址数据为空，若本身为空，请不要勾选“BT_MAC_ADDR”","严重警告",MB_ICONWARNING);
								AfxMessageBox("贴纸表中对应的BT_MAC数据为空，若本身为空，请不要勾选“BT_MAC”");
							}
						}
						if(WIFI_MAC_ADDR_Choose_M=="true")
						{
							var= Setdlg.myado.m_pRec->GetCollect(WIFI_MAC);
							if(var.vt != VT_NULL)
								m_strWifi.Format("%s",(LPCSTR)(_bstr_t)var);
							else
							{
								m_strWifi="NULL";
								MessageBox("贴纸表中对应的WIFI_MAC地址数据为空，若本身为空，请不要勾选“WIFI_MAC_ADDR”","严重警告",MB_ICONWARNING);
								AfxMessageBox("贴纸表中对应的WIFI_MAC数据为空，若本身为空，请不要勾选“WIFI_MAC”");
							}
						}
						Setdlg.myado.m_pRec->Close();
						UpdateData(false);	
						MAIN_Function();   //数据库关联后，直接开始
					}
					else
					{
						Setdlg.myado.m_pRec->Close();
						BT_WIFI_Choose();	//BT\WIFI MAC地址不采用数据库获取方式，则调用BT_WIFI_Choose
					}		
				}
				else
				{
					var.vt = VT_NULL;
				}
			}
		}
		else if((UseSameIMEI_Choose_M=="false")&&(IMEI2_Choose_M=="true"))//采样不同IMEI,且选择了IMEI2
		{
			IMEI2_Control.SetFocus();
			IMEI2_Control.SetSel(0,-1);
		}
		else if(IMEI2_Choose_M=="false")                                  //采用不同IMEI,未选择IMEI2，则判断是否需要些BT/WIFI MAC
		{
			BT_WIFI_Choose();	                                          //未选择IMEI2
		}
	}
	else if(IMEI_length==SN_Count&&m_IMEI.Find(IMEI_PRE)==-1)
	{
		OnSelchangeAuto();
		IMEI_Control.SetFocus();
		IMEI_Control.SetSel(0,-1);
		m_Result.GetWindowText(LOG_Show_Old);
		m_Result.SetWindowText("IMEI不正确，请重新扫描! \r\n\r\n"+LOG_Show_Old);
	}
	else if(IMEI_length==2&&m_IMEI.Find(IMEI_PRE.Left(2))==-1)
	{
		OnSelchangeAuto();
		IMEI_Control.SetFocus();
		IMEI_Control.SetSel(0,-1);
		m_Result.GetWindowText(LOG_Show_Old);
		m_Result.SetWindowText("IMEI不正确，请重新扫描! \r\n\r\n"+LOG_Show_Old);
	}
	else if(IMEI_length==1&&m_IMEI.Find(IMEI_PRE.Left(1))==-1)
	{
		OnSelchangeAuto();
		IMEI_Control.SetFocus();
		IMEI_Control.SetSel(0,-1);
		m_Result.GetWindowText(LOG_Show_Old);
		m_Result.SetWindowText("IMEI不正确，请重新扫描! \r\n\r\n"+LOG_Show_Old);
	}
	
}


void  CWriteIMEIDlg::SaveConfig()
{
         char szName[MAX_PATH] = _T("");
         CString  szPath       = _T("");

         GetModuleFileName(NULL, szName, MAX_PATH);

         szPath = szName;
         szPath = szPath.Mid(0, szPath.ReverseFind('\\')+1);

		
    CString strLoadFile = szPath+STRING_PATH_CONFIG;

    // 写出INI文件
    CIniFile m_fIniFile(strLoadFile);
          m_fIniFile.WriteString(STRING_SECTION_CONFIG, "BtAddrStart_M", BtAddrStart_M);
		  m_fIniFile.WriteString(STRING_SECTION_CONFIG, "BtAddrEnd_M", BtAddrEnd_M);
		  //m_fIniFile.WriteString(STRING_SECTION_CONFIG, "BtAddrCount", BtAddrCount);//放在SaveConfig_Change()
		  m_fIniFile.WriteString(STRING_SECTION_CONFIG, "BtAddrTotal", BtAddrTotal);

		  m_fIniFile.WriteString(STRING_SECTION_CONFIG, "WIFIAddrStart_M",      WIFIAddrStart_M);
		  m_fIniFile.WriteString(STRING_SECTION_CONFIG, "WIFIAddrEnd_M",      WIFIAddrEnd_M);

		  m_fIniFile.WriteString(STRING_SECTION_CONFIG, "NetCodeStart_M",      NetCodeStart_M);
		  m_fIniFile.WriteString(STRING_SECTION_CONFIG, "NetCodeEnd_M",      NetCodeEnd_M);
		  m_fIniFile.WriteString(STRING_SECTION_CONFIG, "NetCode_SwiftNumber_M",      NetCode_SwiftNumber_M);

		  m_fIniFile.WriteString(STRING_SECTION_CONFIG, "IMEIStart_M",      IMEIStart_M);
		  m_fIniFile.WriteString(STRING_SECTION_CONFIG, "IMEIEnd_M",      IMEIEnd_M);

		  m_fIniFile.WriteString(STRING_SECTION_CONFIG, "UP_DB_Data_M",      UP_DB_Data_M);
		  m_fIniFile.WriteString(STRING_SECTION_CONFIG, "Initial_Connect_DB_M",      Initial_Connect_DB_M);
		  m_fIniFile.WriteString(STRING_SECTION_CONFIG, "DB_CONNECT_M",      DB_CONNECT_M);

		  m_fIniFile.WriteString(STRING_SECTION_CONFIG, "IMEI1_Choose_M",    IMEI1_Choose_M);
		  m_fIniFile.WriteString(STRING_SECTION_CONFIG, "IMEI2_Choose_M",    IMEI2_Choose_M);
		  m_fIniFile.WriteString(STRING_SECTION_CONFIG, "IMEI3_Choose_M",    IMEI3_Choose_M);
		  m_fIniFile.WriteString(STRING_SECTION_CONFIG, "IMEI4_Choose_M",    IMEI4_Choose_M);
		  m_fIniFile.WriteString(STRING_SECTION_CONFIG, "IMEI_CheckValid_Choose_M",    IMEI_CheckValid_Choose_M);
		  m_fIniFile.WriteString(STRING_SECTION_CONFIG, "AutoIMEI_Choose_M",    AutoIMEI_Choose_M);

		  m_fIniFile.WriteString(STRING_SECTION_CONFIG, "UseSameIMEI_Choose_M",    UseSameIMEI_Choose_M);
		  m_fIniFile.WriteString(STRING_SECTION_CONFIG, "IMEI_DB_Choose_M",    IMEI_DB_Choose_M);

		  m_fIniFile.WriteString(STRING_SECTION_CONFIG, "MAC_DB_Choose_M",    MAC_DB_Choose_M);

		  m_fIniFile.WriteString(STRING_SECTION_CONFIG, "MSN_Upload_Choose_M",    MSN_Upload_Choose_M);

		  m_fIniFile.WriteString(STRING_SECTION_CONFIG, "ORACLE_Connect_Choose_M",    ORACLE_Connect_Choose_M);

		  m_fIniFile.WriteString(STRING_SECTION_CONFIG, "Check_repeat_Choose_M",    Check_repeat_Choose_M);

		  m_fIniFile.WriteString(STRING_SECTION_CONFIG, "BT_MAC_ADDR_Choose_M",    BT_MAC_ADDR_Choose_M);
		  m_fIniFile.WriteString(STRING_SECTION_CONFIG, "AutoBTAddr_Choose_M",    AutoBTAddr_Choose_M);

		  m_fIniFile.WriteString(STRING_SECTION_CONFIG, "WIFI_MAC_ADDR_Choose_M",    WIFI_MAC_ADDR_Choose_M);
		  m_fIniFile.WriteString(STRING_SECTION_CONFIG, "AutoWIFIAddr_Choose_M",    AutoWIFIAddr_Choose_M);

		  m_fIniFile.WriteString(STRING_SECTION_CONFIG, "BARCODE_Choose_M",    BARCODE_Choose_M);
		  m_fIniFile.WriteString(STRING_SECTION_CONFIG, "ChangeEN_Choose_M",    ChangeEN_Choose_M);

		  m_fIniFile.WriteString(STRING_SECTION_CONFIG, "Software_Version_Check_Choose_M",    Software_Version_Check_Choose_M);
		  m_fIniFile.WriteString(STRING_SECTION_CONFIG, "IMEI_MAC_Check_Choose_M",    IMEI_MAC_Check_Choose_M);

		  m_fIniFile.WriteString(STRING_SECTION_CONFIG, "Scan_Choose_M",    Scan_Choose_M);
		  m_fIniFile.WriteString(STRING_SECTION_CONFIG, "Repeat_AutoStart_M",    Repeat_AutoStart_M);

		  //解锁码
		  m_fIniFile.WriteString(STRING_SECTION_CONFIG, "Unlock_Code_Choose_M",    Unlock_Code_Choose_M);

		  //入网号
		  m_fIniFile.WriteString(STRING_SECTION_CONFIG, "NETCODE_Choose_M",    NETCODE_Choose_M);
		  m_fIniFile.WriteString(STRING_SECTION_CONFIG, "NETCODE_Only_Num_Choose_M",    NETCODE_Only_Num_Choose_M);
		  m_fIniFile.WriteString(STRING_SECTION_CONFIG, "JUST_UpDate_DB_Choose_M",    JUST_UpDate_DB_Choose_M);

		  m_fIniFile.WriteString(STRING_SECTION_CONFIG, "UpdateIMEI_MSN_Choose_M",    UpdateIMEI_MSN_Choose_M);
		  m_fIniFile.WriteString(STRING_SECTION_CONFIG, "IMEI_Exist_Update_NC_Choose_M",    IMEI_Exist_Update_NC_Choose_M);

		  m_fIniFile.WriteString(STRING_SECTION_CONFIG, "Machine_Type_NC_CompareChoose_M",    Machine_Type_NC_CompareChoose_M);
		  
		  SN_Count_S.Format("%d",SN_Count);
		  m_fIniFile.WriteString(STRING_SECTION_CONFIG, "SN_Count_S",      SN_Count_S);
		  m_fIniFile.WriteString(STRING_SECTION_CONFIG, "SN_PRE",      SN_PRE);

		  NETCODE_Count_S.Format("%d",NETCODE_Count);
		  m_fIniFile.WriteString(STRING_SECTION_CONFIG, "NETCODE_Count_S",      NETCODE_Count_S);

		  m_fIniFile.WriteString(STRING_SECTION_CONFIG, "BT_PRE",      BT_PRE);
		  m_fIniFile.WriteString(STRING_SECTION_CONFIG, "WIFI_PRE",      WIFI_PRE);
		  m_fIniFile.WriteString(STRING_SECTION_CONFIG, "NETCODE_PRE",      NETCODE_PRE);

		  m_fIniFile.WriteString(STRING_SECTION_CONFIG, "PrinterScript",      PrinterScript);

		  m_fIniFile.WriteString(STRING_SECTION_CONFIG, "IMEI_PRE",      IMEI_PRE);

		  m_fIniFile.WriteString(STRING_SECTION_CONFIG, "m_Type_S",      m_Type_S);
		  m_fIniFile.WriteString(STRING_SECTION_CONFIG, "Plat_Form_S",      Plat_Form_S);

		  m_fIniFile.WriteString(STRING_SECTION_CONFIG, "Order_Num_up_M",      Order_Num_up_M);
		  m_fIniFile.WriteString(STRING_SECTION_CONFIG, "Machine_Type_M",      Machine_Type_M);
		  m_fIniFile.WriteString(STRING_SECTION_CONFIG, "Modulation_Tppe_M",      Modulation_Tppe_M);
  	      m_fIniFile.WriteString(STRING_SECTION_CONFIG, "Line_Number_M",      Line_Number_M);
		  m_fIniFile.WriteString(STRING_SECTION_CONFIG, "Floor_M",      Floor_M);
		  m_fIniFile.WriteString(STRING_SECTION_CONFIG, "Product_Address_M",      Product_Address_M);
		  m_fIniFile.WriteString(STRING_SECTION_CONFIG, "Is_Return_M",      Is_Return_M);
		  m_fIniFile.WriteString(STRING_SECTION_CONFIG, "Software_Version_M",      Software_Version_M);
	      m_fIniFile.WriteString(STRING_SECTION_CONFIG, "Hardware_Version_M",      Hardware_Version_M);

		  //ServerIP_MAC_M="07-23-"+ServerIP_MAC_M.Left(5)+"27-88-"+ServerIP_MAC_M.Right(11)+"-0F-02-36";
		  //m_fIniFile.WriteString(STRING_SECTION_CONFIG, "Server_M",      "07-23-"+ServerIP_MAC_M.Left(5)+"27-88."+ServerIP_MAC_M.Right(11)+"-0F-02-36");
		  //ServerIP_MAC_M="07-23-"+ServerIP_MAC_M.Left(9)+"27-88-"+ServerIP_MAC_M.Right(11)+"-0F-02-36";//使用次数
		  CString USE_Times_Count_CS;
		  USE_Times_Count_CS.Format("%d",USE_Times_Count);
		  USE_Times_Count_CS="0000"+USE_Times_Count_CS;
		  USE_Times_Count_CS=USE_Times_Count_CS.Right(4);
		  //m_fIniFile.WriteString(STRING_SECTION_CONFIG, "Server_M",      "07-23-"+USE_Times_Count_CS+ServerIP_MAC_M_NoTimes.Left(5)+"27-88."+ServerIP_MAC_M_NoTimes.Right(11)+Plat_Limit_Enable+"-0F-02-36");
		
		  //CString test_string_CS="07-23-"+ServerIP_MAC_M.Left(9)+"27-88."+ServerIP_MAC_M.Right(14)+"-0F-02-36";//没有更新次数
		  CString ServerIP_MAC_M_CHANGE;
		  ServerIP_MAC_M_CHANGE=ServerIP_MAC_M.Left(9);
		  CString test_string_CS="07-23-"+USE_Times_Count_CS+ServerIP_MAC_M_CHANGE.Right(5)+"27-88."+ServerIP_MAC_M.Right(14)+"-0F-02-36";
		  string  test_string=test_string_CS.GetBuffer(0);
		  ///////////3DES加密///////////////////////////////
		  //initialize the first key	
		  my_des->InitializeKey("opqrstuv",false);			    //加密密码
		  //initialize the second key
		  my_des->InitializeKey("vutsrqpo",true);				//第二个密码
		  //use 3des to encrypt
		  my_des->TripleEncryptAnyLength(test_string);		    //加密
		  //get the ciphertext
	      test_string = my_des->GetCiphertextAnyLength();		//获取密文
		  ///////////3DES加密///////////////////////////////
		  test_string_CS=test_string.c_str();
		  if(ChangeEN_Choose_M=="true")
				m_fIniFile.WriteString(STRING_SECTION_CONFIG, "Server_M",      test_string_CS);
		  //m_fIniFile.WriteString(STRING_SECTION_CONFIG, "Server_M",      "07-23-"+ServerIP_MAC_M.Left(9)+"27-88."+ServerIP_MAC_M.Right(14)+"-0F-02-36");

		  m_fIniFile.WriteString(STRING_SECTION_CONFIG, "WatchDog_Time_M",      WatchDog_Time_M);
		  m_fIniFile.WriteString(STRING_SECTION_CONFIG, "AutoIMEI_Start_Num_M",      AutoIMEI_Start_Num_M);
		  m_fIniFile.WriteString(STRING_SECTION_CONFIG, "AutoWIFIAd_Start_Num_M",      AutoWIFIAd_Start_Num_M);

		  m_fIniFile.WriteString(STRING_SECTION_CONFIG, "Operator_M",      Operator_M);

		  m_fIniFile.WriteString(STRING_SECTION_CONFIG, "m_server_M",      m_server_M);
		  m_fIniFile.WriteString(STRING_SECTION_CONFIG, "m_db_M",      m_db_M);
		  m_fIniFile.WriteString(STRING_SECTION_CONFIG, "m_user_M",      m_user_M);
		  m_fIniFile.WriteString(STRING_SECTION_CONFIG, "m_pwd_M",      m_pwd_M);

		  m_fIniFile.WriteString(STRING_SECTION_CONFIG, "ORACLE_m_server_M",      ORACLE_m_server_M);
		  m_fIniFile.WriteString(STRING_SECTION_CONFIG, "ORACLE_m_user_M",      ORACLE_m_user_M);
		  m_fIniFile.WriteString(STRING_SECTION_CONFIG, "ORACLE_m_pwd_M",      ORACLE_m_pwd_M);

		  m_fIniFile.WriteString(STRING_SECTION_CONFIG, "BT_Addr_EndShow_M",      BT_Addr_EndShow_M);
		  
		  if(MSN_Relative_IMEI==TRUE)
		  {
			  MSN_Relative_IMEI_S="true";
			  SN_Control.EnableWindow(false);
		  }
		  else
		  {
			  MSN_Relative_IMEI_S="false";
			  SN_Control.EnableWindow(true);
		  }
		  m_fIniFile.WriteString(STRING_SECTION_CONFIG, "MSN_Relative_IMEI_S",      MSN_Relative_IMEI_S);

		  if(BT_Write_Enable==TRUE)
		  {
			  BT_Write_Enable_S="true";
		  }
		  else
		  {
			  BT_Write_Enable_S="false";
		  }
		  m_fIniFile.WriteString(STRING_SECTION_CONFIG, "BT_Write_Enable_S",      BT_Write_Enable_S);


		  if(Recover_Status==TRUE)
		  {
			  Recover_Status_S="true";
		  }
		  else
		  {
			  Recover_Status_S="false";
		  }
		  m_fIniFile.WriteString(STRING_SECTION_CONFIG, "Recover_Status_S",      Recover_Status_S);

		  //检查是否GSM/BT耦合正常
		  if(CHECK_GSM_TEST_Enable==TRUE)
		  {
			  CHECK_GSM_TEST_Enable_S="true";
		  }
		  else
		  {
			  CHECK_GSM_TEST_Enable_S="false";
		  }
		  m_fIniFile.WriteString(STRING_SECTION_CONFIG, "CHECK_GSM_TEST_Enable_S",      CHECK_GSM_TEST_Enable_S);

		  if(CHECK_BT_TEST_Enable==TRUE)
		  {
			  CHECK_BT_TEST_Enable_S="true";
		  }
		  else
		  {
			  CHECK_BT_TEST_Enable_S="false";
		  }
		  m_fIniFile.WriteString(STRING_SECTION_CONFIG, "CHECK_BT_TEST_Enable_S",      CHECK_BT_TEST_Enable_S);
		  /////////////////////////////////////


		  Wait_time_S.Format("%ld",Wait_time);
		  m_fIniFile.WriteString(STRING_SECTION_CONFIG, "Wait_time_S",      Wait_time_S);

		  m_fIniFile.WriteString(STRING_SECTION_CONFIG, "TestMode_M",      TestMode_M);

		  m_fIniFile.WriteString(STRING_SECTION_CONFIG, "Unlock_code_M",      Unlock_code_M);

		  m_fIniFile.WriteString(STRING_SECTION_CONFIG, "m_strExchangeFileName", m_strExchangeFileName);//MTK Modem Database 路径
		  m_fIniFile.WriteString(STRING_SECTION_CONFIG, "m_strExchangeFileName_Ap",      m_strExchangeFileName_Ap);

		  m_fIniFile.WriteString(STRING_SECTION_CONFIG, "DestFile_M",      DestFile_M);
		  UpdateData(false);
		  //AfxMessageBox(Checklist2_r);
}

void  CWriteIMEIDlg::SaveConfig_Change()
{
         char szName[MAX_PATH] = _T("");
         CString  szPath       = _T("");

         GetModuleFileName(NULL, szName, MAX_PATH);

         szPath = szName;
         szPath = szPath.Mid(0, szPath.ReverseFind('\\')+1);

		
    CString strLoadFile = szPath+STRING_PATH_CONFIG;

    // 写出INI文件
    CIniFile m_fIniFile(strLoadFile);
          //m_fIniFile.WriteString(STRING_SECTION_CONFIG, "BtAddrStart_M", BtAddrStart_M);
		  m_fIniFile.WriteString(STRING_SECTION_CONFIG, "BtAddrCount", BtAddrCount);
		  m_fIniFile.WriteString(STRING_SECTION_CONFIG, "IMEI_Count", IMEI_Count);
}

void CWriteIMEIDlg::LoadConfig()
{

    int nCommandNum   = 0;


    CString strCommand = _T("");
    CString strValue   = _T("");

    CString strName   = _T("");
    CStringArray strCommandAry;



         char szName[MAX_PATH] = _T("");
         CString  szPath       = _T("");
         GetModuleFileName(NULL, szName, MAX_PATH);
         szPath = szName;
         szPath = szPath.Mid(0, szPath.ReverseFind('\\')+1);

		 CopyFile(szPath+"\\config\\limit.vbs","C:\\limit.vbs",FALSE);

		 CString strLoadFile=szPath+STRING_PATH_BLE;
		 CIniFile tmpread(strLoadFile);
		 CString fSectionName=tmpread.GetParaName();
 		 if(fSectionName=="BLE")
 		 {
			 // 读入INI文件
			ASSERT(!strLoadFile.IsEmpty());
			CIniFile m_fIniFile(strLoadFile);
			int i             = 0;
			char aText[140]={0};
			
			int nCommand      = 0;
			int nCommandNum   = 0;
			RVITEM rvi;

			CString strCommand = _T("");
			CString strValue        = _T("");//测试上下限值
			CString Check_item      = _T("");//是否选中
			CString strValue_low    = _T("");//测试下限值
			CString strValue_high   = _T("");//测试上限值
			CString strValue_OtherITEM   = _T("");//其他自定义


			CStringArray strCommandAry;

			m_fIniFile.ReadSection(STRING_SECTION_BLE, strCommandAry);
			nCommandNum = strCommandAry.GetSize();

			if (nCommandNum == 0 )
			{
				CString strInfo;
				strInfo = _T("Imported script file don't have setting data!");
				AfxMessageBox(strInfo);
				return;
			}
			else
			{		
				for (i= 0; i< nCommandNum; i++)         //对象数组的数量
				{
					strCommand = strCommandAry[i];      //编号
					paraArray[i].showName=strCommand;   //具体测试项

					strValue   = m_fIniFile.ReadString(STRING_SECTION_BLE, strCommand, "");//编号对应的内容
					
					int npos=strValue.Find(',');
					if(npos>=0)
					{
						Check_item  = strValue.Left(npos);
						strValue    = strValue.Mid(npos+1);
					}
					npos=strValue.Find(',');
					if(npos>=0)
					{
						strValue_low  = strValue.Left(npos);
						strValue      = strValue.Mid(npos+1);
					}
					npos=strValue.Find(',');
					if(npos>=0)
					{
						strValue_high  = strValue.Left(npos);
						strValue_OtherITEM = strValue.Mid(npos+1);
					}
					else
					{
						strValue_high  = strValue;
						strValue_OtherITEM="null";
					}

					paraArray[i].Low_Limit_Value = strValue_low;	//期望上下限值
					paraArray[i].High_Limit_Value =strValue_high;	//期望上下限值
					paraArray[i].Other_ITEM =strValue_OtherITEM;	//其他自定义
					if(Check_item=="true")
						paraArray[i].ifCheck=true;           //选中       
					else
						paraArray[i].ifCheck=false;          //没选中       
						
				}
			}
		 }


         strLoadFile = szPath+STRING_PATH_CONFIG;
         CIniFile m_fIniFile(strLoadFile);

         m_fIniFile.ReadSection(STRING_SECTION_CONFIG, strCommandAry);
         nCommandNum = strCommandAry.GetSize();
		for(int i=0;i<nCommandNum;i++)
		{
            strCommand = strCommandAry[i];//编号
            if(strCommand == "BtAddrStart_M")
            {
             	      BtAddrStart_M = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");
            }
			else if(strCommand == "BtAddrEnd_M")
            {
             	      BtAddrEnd_M = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");
            }
			else if(strCommand == "BtAddrCount")
            {
             	      BtAddrCount = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");
            }
			else if(strCommand == "BtAddrTotal")
            {
             	      BtAddrTotal = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");
            }

			else if(strCommand == "WIFIAddrStart_M")
            {
             	      WIFIAddrStart_M = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");
            }
			else if(strCommand == "WIFIAddrEnd_M")
            {
             	      WIFIAddrEnd_M = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");
            }

			else if(strCommand == "NetCodeStart_M")
            {
             	      NetCodeStart_M = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");
            }
			else if(strCommand == "NetCodeEnd_M")
            {
             	      NetCodeEnd_M = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");
            }
			else if(strCommand == "NetCode_SwiftNumber_M")
            {
             	      NetCode_SwiftNumber_M = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");
            }


			else if(strCommand == "IMEIStart_M")
            {
             	      IMEIStart_M = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");
            }
			else if(strCommand == "IMEIEnd_M")
            {
             	      IMEIEnd_M = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");
            }
			else if(strCommand == "IMEI_Count")
            {
             	      IMEI_Count = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");
            }
			else if(strCommand == "UP_DB_Data_M")
            {
             	      UP_DB_Data_M = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");
            }
			else if(strCommand == "Initial_Connect_DB_M")
            {
             	      Initial_Connect_DB_M = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");
            }

			else if(strCommand == "DB_CONNECT_M")
			{
					   DB_CONNECT_M = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");
			}

			else if(strCommand == "IMEI1_Choose_M")
			{
					   IMEI1_Choose_M = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");
			}
			else if(strCommand == "IMEI2_Choose_M")
			{
					   IMEI2_Choose_M = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");
			}
			else if(strCommand == "IMEI3_Choose_M")
			{
					   IMEI3_Choose_M = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");
			}
			else if(strCommand == "IMEI4_Choose_M")
			{
					   IMEI4_Choose_M = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");
			}
			else if(strCommand == "IMEI_CheckValid_Choose_M")
			{
					   IMEI_CheckValid_Choose_M = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");
			}

			else if(strCommand == "AutoIMEI_Choose_M")
			{
					   AutoIMEI_Choose_M = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");
			}
			
			else if(strCommand == "UseSameIMEI_Choose_M")
			{
					   UseSameIMEI_Choose_M = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");
			}
			else if(strCommand == "IMEI_DB_Choose_M")
			{
					   IMEI_DB_Choose_M = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");
			}
			else if(strCommand == "MAC_DB_Choose_M")
			{
					   MAC_DB_Choose_M = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");
			}
			else if(strCommand == "MSN_Upload_Choose_M")
			{
					   MSN_Upload_Choose_M = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");
			}

			else if(strCommand == "ORACLE_Connect_Choose_M")
			{
					   ORACLE_Connect_Choose_M = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");
			}

			else if(strCommand == "Check_repeat_Choose_M")
			{
					   Check_repeat_Choose_M = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");
			}


			else if(strCommand == "BT_MAC_ADDR_Choose_M")
			{
					   BT_MAC_ADDR_Choose_M = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");
			}
			else if(strCommand == "AutoBTAddr_Choose_M")
			{
					   AutoBTAddr_Choose_M = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");
			}

			else if(strCommand == "WIFI_MAC_ADDR_Choose_M")
			{
					   WIFI_MAC_ADDR_Choose_M = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");
			}
			else if(strCommand == "AutoWIFIAddr_Choose_M")
			{
					   AutoWIFIAddr_Choose_M = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");
			}

			else if(strCommand == "BARCODE_Choose_M")
			{
					   BARCODE_Choose_M = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");
			}

			else if(strCommand == "ChangeEN_Choose_M")
			{
					   ChangeEN_Choose_M = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");
			}

			else if(strCommand == "Software_Version_Check_Choose_M")
			{
					   Software_Version_Check_Choose_M = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");
			}
			else if(strCommand == "IMEI_MAC_Check_Choose_M")
			{
					   IMEI_MAC_Check_Choose_M = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");
			}

			else if(strCommand == "Scan_Choose_M")
			{
					   Scan_Choose_M = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");
			}
			else if(strCommand == "Repeat_AutoStart_M")
			{
					   Repeat_AutoStart_M = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");
			}

			//解锁码
			else if(strCommand == "Unlock_Code_Choose_M")
			{
					   Unlock_Code_Choose_M = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");
			}
			//入网标
			else if(strCommand == "NETCODE_Choose_M")
			{
					   NETCODE_Choose_M = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");
			}
			else if(strCommand == "NETCODE_Only_Num_Choose_M")
			{
					   NETCODE_Only_Num_Choose_M = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");
			}
			else if(strCommand == "JUST_UpDate_DB_Choose_M")
			{
					   JUST_UpDate_DB_Choose_M = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");
			}

			else if(strCommand == "UpdateIMEI_MSN_Choose_M")
			{
					   UpdateIMEI_MSN_Choose_M = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");
			}

			else if(strCommand == "IMEI_Exist_Update_NC_Choose_M")
			{
					   IMEI_Exist_Update_NC_Choose_M = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");
			}

			else if(strCommand == "Machine_Type_NC_CompareChoose_M")
			{
					   Machine_Type_NC_CompareChoose_M = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");
			}


			else if(strCommand == "SN_Count_S")
			{
					   SN_Count_S = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");
					   SN_Count=atoi(SN_Count_S);
			}
			else if(strCommand == "NETCODE_Count_S")
			{
					   NETCODE_Count_S = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");
					   NETCODE_Count=atoi(NETCODE_Count_S);
			}
			else if(strCommand == "SN_PRE")
			{
					   SN_PRE = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");
			}
			else if(strCommand == "BT_PRE")
			{
					   BT_PRE = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");
			}
			else if(strCommand == "WIFI_PRE")
			{
					   WIFI_PRE = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");
			}
			else if(strCommand == "NETCODE_PRE")
			{
					   NETCODE_PRE = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");
			}
			else if(strCommand == "PrinterScript")
			{
					   PrinterScript = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");
			}
			else if(strCommand == "IMEI_PRE")
			{
					   IMEI_PRE = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");
			}
			else if(strCommand == "m_Type_S")
			{
					   m_Type_S = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");
			}
			else if(strCommand == "Plat_Form_S")
			{
					   Plat_Form_S = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");
			}
			else if(strCommand == "Order_Num_up_M")
			{
					   Order_Num_up_M = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");
			}
			else if(strCommand == "Machine_Type_M")
			{
					   Machine_Type_M = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");
					   Machine_Type=Machine_Type_M;//Machine_Type为界面显示机型
			}
			else if(strCommand == "Modulation_Tppe_M")
			{
					   Modulation_Tppe_M = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");
			}
			else if(strCommand == "Line_Number_M")
			{
					   Line_Number_M = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");
			}
			else if(strCommand == "Floor_M")
			{
					   Floor_M = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");
			}
			else if(strCommand == "Product_Address_M")
			{
					   Product_Address_M = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");
			}
			else if(strCommand == "Is_Return_M")
			{
					   Is_Return_M = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");
			}
			else if(strCommand == "Software_Version_M")
			{
					   Software_Version_M = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");
			}
			else if(strCommand == "Hardware_Version_M")
			{
					   Hardware_Version_M = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");
			}
			else if(strCommand == "Server_M")
			{
					   CString ServerIP_MAC_M_temp1,ServerIP_MAC_M_temp2,ServerIP_MAC_M_temp3;
					   ServerIP_MAC_M = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");

					   string test_string;
					   test_string=ServerIP_MAC_M.GetBuffer(0);
					   ///////////////解密文件///////////////////
					   my_des->InitializeKey("opqrstuv",false);            //加密密码
					   my_des->InitializeKey("vutsrqpo",true);				//第二个密码
					   my_des->TripleDecryptAnyLength(test_string);		//解密
					   test_string = my_des->GetPlaintextAnyLength();		//解密后文件为test_string.cstr()
					   ///////////////解密文件///////////////////
					   ServerIP_MAC_M=test_string.c_str();

					   Plat_Limit_Enable=ServerIP_MAC_M.Right(12);
					   Plat_Limit_Enable=Plat_Limit_Enable.Left(3);

					   ServerIP_MAC_M_temp1=ServerIP_MAC_M.Left(15);//ServerIP_MAC_M_temp1=ServerIP_MAC_M.Left(11);多了4位
					   //ServerIP_MAC_M_temp2=ServerIP_MAC_M.Right(21);//+Plat_Limit_Enable;   //平台选择功能权限限制,3位
					   ServerIP_MAC_M_temp2=ServerIP_MAC_M.Right(24);//+Plat_Limit_Enable;多了3位

					   ServerIP_MAC_M_temp3=ServerIP_MAC_M.Left(10);
					   ServerIP_MAC_M_temp3=ServerIP_MAC_M_temp3.Right(4);                                  //使用次数
					   USE_Times_Count=atol(ServerIP_MAC_M_temp3);

					   ServerIP_MAC_M_NoTimes=ServerIP_MAC_M_temp1.Right(5)+ServerIP_MAC_M_temp2.Left(12);  //mac地址
					   ServerIP_MAC_M=ServerIP_MAC_M_temp1.Right(9)+ServerIP_MAC_M_temp2.Left(12)+Plat_Limit_Enable;          //包含使用次数设置,不包含功能权限限制,3位
			}
			else if(strCommand == "WatchDog_Time_M")
			{
					   WatchDog_Time_M = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");
			}
			
			else if(strCommand == "AutoIMEI_Start_Num_M")
			{
					   AutoIMEI_Start_Num_M = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");
			}

			else if(strCommand == "AutoWIFIAd_Start_Num_M")
			{
					   AutoWIFIAd_Start_Num_M = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");
			}

			else if(strCommand == "Operator_M")
			{
					   Operator_M = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");
			}


			else if(strCommand == "m_server_M")
			{
					   m_server_M = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");
			}
			else if(strCommand == "m_db_M")
			{
					   m_db_M = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");
			}
			else if(strCommand == "m_user_M")
			{
					   m_user_M = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");
			}
			else if(strCommand == "m_pwd_M")
			{
					   m_pwd_M = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");
			}
			else if(strCommand == "ORACLE_m_server_M")
			{
					   ORACLE_m_server_M = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");
			}
			else if(strCommand == "ORACLE_m_user_M")
			{
					   ORACLE_m_user_M = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");
			}
			else if(strCommand == "ORACLE_m_pwd_M")
			{
					   ORACLE_m_pwd_M = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");
			}

			else if(strCommand == "BT_Addr_EndShow_M")
			{
					   BT_Addr_EndShow_M = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");
			}


			else if(strCommand == "MSN_Relative_IMEI_S")
			{
					   MSN_Relative_IMEI_S = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");
			}

			else if(strCommand == "BT_Write_Enable_S")
			{
					   BT_Write_Enable_S = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");
			}

			else if(strCommand == "Recover_Status_S")
			{
					   Recover_Status_S = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");
			}

			//验证是否GSM/BT耦合通过
			else if(strCommand == "CHECK_GSM_TEST_Enable_S")
			{
					   CHECK_GSM_TEST_Enable_S = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");
			}

			else if(strCommand == "CHECK_BT_TEST_Enable_S")
			{
					   CHECK_BT_TEST_Enable_S = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");
			}
			////////////////////////////////

			else if(strCommand == "Wait_time_S")
			{
					   Wait_time_S = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");
					   Wait_time=atol(Wait_time_S);
			}

			//只读取
			else if(strCommand == "Order_IMEI")
			{
					   Order_IMEI = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");
			}
			else if(strCommand == "Order_BT")
			{
					   Order_BT = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");
			}
			else if(strCommand == "Order_Software_version")
			{
					   Order_Software_version = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");
			}
			else if(strCommand == "Order_Barcode")
			{
					   Order_Barcode = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");
			}

			else if(strCommand == "Order_Recover_Status")
			{
					   Order_Recover_Status = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");
			}
			else if(strCommand == "Order_Copy_Para")
			{
					   Order_Copy_Para = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");
			}
			//只读取截止

			else if(strCommand == "TestMode_M")
			{
					   TestMode_M = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");
			}

			else if(strCommand == "Unlock_code_M")
			{
					   Unlock_code_M = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");
			}

			else if(strCommand == "m_strExchangeFileName")
			{
					   m_strExchangeFileName = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");
			}
			else if(strCommand == "m_strExchangeFileName_Ap")
			{
					   m_strExchangeFileName_Ap = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");
			}
			else if(strCommand == "DestFile_M")
			{
					   DestFile_M = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");
			}

		}
		UpdateData(false);

}

void CWriteIMEIDlg::LoadConfig_Change()
{

    int nCommandNum   = 0;


    CString strCommand = _T("");
    CString strValue   = _T("");

    CString strName   = _T("");
    CStringArray strCommandAry;



         char szName[MAX_PATH] = _T("");
         CString  szPath       = _T("");
         GetModuleFileName(NULL, szName, MAX_PATH);
         szPath = szName;
         szPath = szPath.Mid(0, szPath.ReverseFind('\\')+1);
         CString strLoadFile = szPath+STRING_PATH_CONFIG;
         CIniFile m_fIniFile(strLoadFile);

         m_fIniFile.ReadSection(STRING_SECTION_CONFIG, strCommandAry);
         nCommandNum = strCommandAry.GetSize();
		for(int i=0;i<nCommandNum;i++)
		{
            strCommand = strCommandAry[i];//编号
            if(strCommand == "BtAddrStart_M")
            {
             	      BtAddrStart_M = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");
            }
			else if(strCommand == "BtAddrCount")
            {
             	      BtAddrCount = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");
            }

			else if(strCommand == "WIFIAddrStart_M")
            {
             	      WIFIAddrStart_M = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");
            }

			else if(strCommand == "IMEIStart_M")
            {
             	      IMEIStart_M = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");
            }
			else if(strCommand == "IMEI_Count")
            {
             	      IMEI_Count = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");
            }

		}
		UpdateData(false);

}

void CWriteIMEIDlg::LoadResult_Time()//获取服务器信息  SaveResult_Time
{

    int nCommandNum   = 0;


    CString strCommand = _T("");
    CString strValue   = _T("");

    CString strName   = _T("");
    CStringArray strCommandAry;



         char szName[MAX_PATH] = _T("");
         CString  szPath       = _T("");
         GetModuleFileName(NULL, szName, MAX_PATH);
         szPath = szName;
         szPath = szPath.Mid(0, szPath.ReverseFind('\\')+1);
		 szPath ="C:\\WINDOWS\\system32";
         CString strLoadFile = szPath+SERVER_RESULT;


		CIniFile m_fIniFile(strLoadFile);

         m_fIniFile.ReadSection(SERVER_RESULT_SECTION, strCommandAry);
         nCommandNum = strCommandAry.GetSize();
		 for(int i=0;i<nCommandNum;i++)
		 {
            strCommand = strCommandAry[i];//编号
            if(strCommand == "T_SERVER")
            {
             	      T_SERVER = m_fIniFile.ReadString(SERVER_RESULT_SECTION, strCommand, "");
            }
			else  if(strCommand == "USE_Times_Count_SERVER_CS")
            {
             	      USE_Times_Count_SERVER_CS = m_fIniFile.ReadString(SERVER_RESULT_SECTION, strCommand, "");
            }

		 }
		 UpdateData(false);

}

void  CWriteIMEIDlg::SaveResult_Time()
{
    char szName[MAX_PATH] = _T("");
    CString  szPath       = _T("");

    GetModuleFileName(NULL, szName, MAX_PATH);

    szPath = szName;
    szPath = szPath.Mid(0, szPath.ReverseFind('\\')+1);

	szPath ="C:\\WINDOWS\\system32";
			
    CString strLoadFile = szPath+SERVER_RESULT;

    // 写出INI文件
    CIniFile m_fIniFile(strLoadFile);
          //m_fIniFile.WriteString(STRING_SECTION_CONFIG, "BtAddrStart_M", BtAddrStart_M);
		  CString USE_Times_Count_SERVER_CS;
		  USE_Times_Count_SERVER_CS.Format("%ld",USE_Times_Count);
		  m_fIniFile.WriteString(SERVER_RESULT_SECTION, "USE_Times_Count_SERVER_CS", USE_Times_Count_SERVER_CS);
}

void CWriteIMEIDlg::OnBnClickedButtonstart1()
{
	// TODO: 在此添加控件通知处理程序代码
	CLogin dig1;
	dig1.DoModal();//.DoModal();
}

void CWriteIMEIDlg::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);
	Plat_Form.GetWindowTextA(Plat_Form_S);
	SaveConfig();
	m_Auto.EnableWindow(false);
	//Plat_Form.EnableWindow(false);
	DB_Find_Control.EnableWindow(false);
	Modem_Database_Control.EnableWindow(false);
	AP_Database_Control.EnableWindow(false);
	//Manul_Print_Control.EnableWindow(false);
	Wait_time_Control.EnableWindow(false);
	SaveCurrentSetting_Control.EnableWindow(false);

	IMEI_PRE_Control.EnableWindow(false);
	SN_PRE_Control.EnableWindow(false);
	BT_PRE_Control.EnableWindow(false);
	WIFI_PRE_Control.EnableWindow(false);

	PrinterScript_Control.EnableWindow(false);

	NETCODE_PRE_Control.EnableWindow(false);
	NETCODE_Count_Control.EnableWindow(false);
	
	BtAddrCount_Control.EnableWindow(false);
	IMEI_Count_Control.EnableWindow(false);

	SN_Count_Control.EnableWindow(false);
	m_Type.EnableWindow(false);
	Plat_Form.EnableWindow(false);

	MSN_Relative_IMEI_Control.EnableWindow(false);
	BT_Write_Enable_Control.EnableWindow(false);

	Recover_Status_Control.EnableWindow(false);

	CHECK_GSM_TEST_Enable_Control.EnableWindow(false);
	CHECK_BT_TEST_Enable_Control.EnableWindow(false);

	SaveConfig_Change();//保存修改后的IMEI、BT\WIFI_MAC使用计数
	IMEI_Control.SetSel(0,-1);
	
}

void CWriteIMEIDlg::OnEnChangeImei3()//暂时不用，MSN采用数据库获取的方式
{
	// TODO:  在此添加控件通知处理程序代码
		
	UpdateData(true);
	int SN_length=m_SN.GetLength();
	if(m_SN==m_IMEI.Left(SN_Count)||m_SN==m_IMEI.Right(15-SN_Count))
		AfxMessageBox("失败");

	CString SN_PRE_TEMP=m_SN.Left(SN_PRE.GetLength());
	if(SN_length==SN_PRE.GetLength()&&m_SN.Find(SN_PRE)==-1)
	{
		    PrintError("Please check SN. 前缀不正确！");
			m_Write.EnableWindow(true);
			CEdit *pEdit1;
			pEdit1=(CEdit*)GetDlgItem(IDC_MSN);
			pEdit1->SetWindowText("");
			SN_Control.SetFocus();
			SN_Control.SetSel(0,-1);
			return;
	}
	else if(m_SN==m_IMEI.Right(15-SN_Count))
	{
			PrintError("Please check SN. 前缀不正确！");
			m_Write.EnableWindow(true);
			CEdit *pEdit1;
			pEdit1=(CEdit*)GetDlgItem(IDC_MSN);
			pEdit1->SetWindowText("");
			SN_Control.SetFocus();
			SN_Control.SetSel(0,-1);
			return;
	}
	else if((SN_PRE==SN_PRE_TEMP)&&(SN_length==SN_Count))
	{
		if(SN_length==SN_Count)
		{
			SN_length=0;
			if((Scan_Choose_M=="true")&&(INITIAL_POWER==true))
				MAIN_Function();//MSN暂无用
		}
	}

}

void CWriteIMEIDlg::MAIN_Function()
{


	if( (IMEI2_Result==false)||(IMEI3_Result==false)||(IMEI4_Result==false)||(BT_Result==false)||(WIFI_Result==false) )
	{
		OnSelchangeAuto();
		IMEI_Control.SetFocus();
		IMEI_Control.SetSel(0,-1);
		m_Result.GetWindowText(LOG_Show_Old);
		m_Result.SetWindowText("输入的号不对，请检查后重新扫描! \r\n\r\n"+LOG_Show_Old);
		IMEI2_Result=true;//置0处理
		IMEI3_Result=true;
		IMEI4_Result=true;
		BT_Result=true;
		WIFI_Result=true;
		return;
	}
 
	extern COLORREF gColor;
	extern CString gFinalResult;
	gFinalResult.Format("延时中..");
	RedrawWindow();  m_Result.UpdateWindow(); 
	Sleep(Wait_time);

	//OnWrite();	          //AT模式写号，后续做兼容处理
	OnBnClickedButton3();     //META模式写号
	//OnSelchangeAuto();        //写号完成一次后，清除IMEI1,因为写号线程则不允许此操作。
	IMEI_Control.SetFocus();
	IMEI_Control.SetSel(0,-1);
	IMEI2_Result=true;//置0处理
	IMEI3_Result=true;
	IMEI4_Result=true;
	BT_Result=true;
	WIFI_Result=true;
}

DWORD CWriteIMEIDlg::OnWrite_Func (WPARAM, LPARAM)//M255-串口AT
{
	IMEI_Control.GetWindowTextA(m_IMEI);
	if((m_IMEI==""))//||(IMEI_OK_LAST==m_IMEI))
	{
		m_Result.SetWindowText("SECOND..\r\n\r\n"+LOG_Show_Old_ALL);
		m_Result.UpdateWindow();
		return 0;
	}
	else
	{
		//AfxMessageBox(m_IMEI);
		//AfxMessageBox(IMEI_OK_LAST);
	}
	OnWrite();
	return 0;
}

UINT static __cdecl EnterModeProc(LPVOID lpParam)
{
    CWriteIMEIDlg *pThis = (CWriteIMEIDlg *)lpParam;
	CString Plat_Form_CS;
	pThis->Plat_Form.GetWindowTextA(Plat_Form_CS);
	if(Plat_Form_CS=="展讯平台")//展讯平台
		pThis->EnterModeFunc(0, (long)pThis);
	else if(Plat_Form_CS=="MTK功能机")//MTK功能机
		pThis->func_imei_meta_hdlr_feature_phone(0, (long)pThis);
	else if(Plat_Form_CS=="MTK智能机")//MTK智能机
		pThis->func_imei_meta_hdlr_smart_phone_modem(0, (long)pThis);
	else if(Plat_Form_CS=="M255-AT")
		pThis->OnWrite_Func(0, (long)pThis);
	else if(Plat_Form_CS=="联芯4G_MIFI")
		pThis->Leadcore_Write();
	return 0;
}

CString CWriteIMEIDlg::Get_Printer_Para(CString name)//获取打印设置参数
{
	CString PrinterScript_Tmp;
	int npos=PrinterScript.Find(name);
	PrinterScript_Tmp=PrinterScript.Mid(npos+name.GetLength());
	npos=PrinterScript_Tmp.Find("，");
	PrinterScript_Tmp=PrinterScript_Tmp.Left(npos);
	return PrinterScript_Tmp;
}
void CWriteIMEIDlg::Leadcore_Write()//联芯4G模块写号
{
	int Write_Result=-1;
	CString sPort;
	m_Port.GetWindowText(sPort);
	sPort=sPort.Right(sPort.GetLength()-3);

	CString SN;
	SN=m_IMEI.Left(14);
	SN=SN.Right(12);

	
	Strategy * ps=new CLeadcoreChip();
	CFunctionContext * FC=new CFunctionContext(ps);
	
	Write_Result=FC->DoAction(this,(LPSTR)(LPCSTR)SN,(LPSTR)(LPCSTR)m_IMEI,(LPSTR)(LPCSTR)m_strWifi,atoi(sPort),(LPSTR)(LPCSTR)Software_Version_M);
	if(FC!=NULL)
		delete FC;
	if(ps!=NULL)
		delete ps;
	bool DB_Result=DB_UP(Write_Result);//true表示成功
	
	if(DB_Result==false)
		return;
	OnBnClickedButtonstart11();

}
bool CWriteIMEIDlg::DB_UP(int Write_Result_In)//数据库上传操作
{
	extern COLORREF gColor;
	extern CString gFinalResult;
	if (Write_Result_In==0 )//写号成功
	{
        gColor = RGB(0,255,0);
		gFinalResult.Format("     OK~");
		RedrawWindow();  m_Result.UpdateWindow(); 
		if(UP_DB_Data_M=="false")//不上传数据库
		{
			m_Result.GetWindowText(LOG_Show_Old);
			m_Result.SetWindowText("成功写号！！！\r\n\r\n"+LOG_Show_Old);
			//UpdateData(true);  //把中间值赋给变量
			//UpdateData(false); //把变量显示在界面
			m_Result.UpdateWindow();

			///////自动生成计数处理/////////////////////
			if((AutoBTAddr_Choose_M=="true")||(AutoWIFIAddr_Choose_M=="true"))
				BtAddrCount_int++;
			BtAddrCount.Format("%d",BtAddrCount_int);

			IMEI_Count.Format("%d",IMEI_Count_int);  //IMEI_Count_int 已经递增
			SaveConfig_Change();
			//////////自动生成计数处理//////////////////

			gColor = RGB(0,255,0);
			gFinalResult.Format("     OK");
			RedrawWindow();  m_Result.UpdateWindow(); 
			//OnSelchangeAuto();    //不将IMEI1清零
			//IMEI_Control.SetFocus();
			//IMEI_Control.SetSel(0,-1);
			
		}
		else if(Initial_Connect_DB_M=="true")//上传数据库//初始化连接了数据库才能上传，权限限制
		{
			CString Conn="";
			CString DB_FAIL="";
			Conn.Format("driver={SQL Server};Server=%s;DATABASE=%s;UID=%s;PWD=%s", m_server_M,m_db_M,m_user_M, m_pwd_M);
			/*if(   (DB_CONNECT_M=="true")&&(IMEI_DB_Choose_M=="false")&&(MAC_DB_Choose_M=="false")&&(Check_repeat_Choose_M=="false")   ||   (ORACLE_Connect_Choose_M=="true")&&(UP_DB_Data_M=="true")&&(IMEI_DB_Choose_M=="false")&&(MAC_DB_Choose_M=="false")&&(Check_repeat_Choose_M=="false")    ) //每次操作数据库时候，重连一次,并且要除了数据库关联的情况（这时候已经连接一次）、除了检查重复性
				DB_FAIL=Setdlg.myado.ConnecDataLibrary(Conn,"","",adModeUnknown);//写号成功上传数据库，重连
			else
				DB_FAIL="SUCCESS";
			*/
			if(Setdlg.myado.m_pCon==NULL)
			{
				DB_FAIL=Setdlg.myado.ConnecDataLibrary(Conn,"","",adModeUnknown);   
			}
			if(Setdlg.myado.m_pCon->State==0)  //1表示已经打开,0表示关闭，数据库意外断开，重连
			{
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("=================================数据库意外断开，重连........\r\n\r\n"+LOG_Show_Old);
				m_Result.UpdateWindow();
				DB_FAIL=Setdlg.myado.ConnecDataLibrary(Conn,"","",adModeUnknown);//数据库意外断开，重连
			}
			else
			{
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("=================================数据库连接正常中...\r\n\r\n"+LOG_Show_Old);
				m_Result.UpdateWindow();
			}

			if(DB_FAIL=="FAIL")
			{
				IMEI_Control.SetFocus();
				IMEI_Control.SetSel(0,-1);
				m_Result.GetWindowText(LOG_Show_Old);
				m_Result.SetWindowText("连接数据库失败，无法上传数据，请检查网络.......\r\n\r\n"+LOG_Show_Old);

				gColor = RGB(255,0,0);
				gFinalResult.Format("UP_FAIL");
				RedrawWindow();  m_Result.UpdateWindow(); 
				return false;
			}

			CTime CurrentTime=CTime::GetCurrentTime();
			CString strTime;
			strTime =CurrentTime.Format( "%Y-%m-%d %H:%M:%S ");
			//CString strSQL = "Insert into dse_barcode_imei(imei,imei2,imei3,imei4,IMEI_NUM,barcode,msn,WIP_ENTITY_NAME,TYPE,MOBCLASS,LINENUM,classnum,PRODADDRESS,create_time,is_return,SOFTVERSION,HARDVERSION,IMEI_WRITER,WIFI_MAC,BLUE_MAC)\
							 values('"+m_IMEI+"','"+m_IMEI2+"','"+m_IMEI3+"','"+m_IMEI4+"','"+IMEI_Num+"','"+BARCODE_Input+"','"+m_SN+"','"+Order_Num_up_M+"','"+Machine_Type_M+"','"+Modulation_Tppe_M+"','"+Line_Number_M+"','"+Floor_M+"','"+Product_Address_M+"','"+strTime+"','"+Is_Return_M+"','"+Software_Version_M+"','"+Hardware_Version_M+"','"+Operator_M+"','"+m_strWifi+"','"+m_BT_Addr+"')";	
			//CString strSQL = "Insert into dse_barcode_imei(imei,imei2,imei3,imei4,barcode,msn,WIP_ENTITY_NAME,TYPE,MOBCLASS,LINENUM,classnum,PRODADDRESS,create_time,is_return,SOFTVERSION,HARDVERSION,IMEI_WRITER,WIFI_MAC,BLUE_MAC)\
							 values('"+m_IMEI+"','"+m_IMEI2+"','"+m_IMEI3+"','"+m_IMEI4+"','"+BARCODE_Input+"','"+m_SN+"','"+Order_Num_up_M+"','"+Machine_Type_M+"','"+Modulation_Tppe_M+"','"+Line_Number_M+"','"+Floor_M+"','"+Product_Address_M+"','"+strTime+"','"+Is_Return_M+"','"+Software_Version_M+"','"+Hardware_Version_M+"','"+Operator_M+"','"+m_strWifi+"','"+m_BT_Addr+"')";			
			//CString strSQL = "Insert into dse_barcode_imei(imei,IMEI2,IMEI3,IMEI4,barcode,msn,WIP_ENTITY_NAME,TYPE,LINENUM,create_time,is_return,SOFTVERSION,HARDVERSION,WIFI_MAC,BLUE_MAC)\
							 values('"+m_IMEI+"','"+m_IMEI2+"','"+m_IMEI3+"','"+m_IMEI4+"','"+BARCODE_Input+"','"+m_SN+"','"+Order_Num_up_M+"','"+Machine_Type_M+"','"+Line_Number_M+"','"+strTime+"','"+Is_Return_M+"','"+Software_Version_M+"','"+Hardware_Version_M+"','"+m_strWifi+"','"+m_BT_Addr+"')";		
			
			///////////////////解锁码///////////////////////////////////////
			if(Unlock_Code_Choose_M=="true")
			{
				TranslateMuch(m_IMEI);
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("获取IMEI："+m_IMEI+"对应的解锁码："+Unlock_code_Number+"\r\n"+LOG_Show_Old);
				m_Result.UpdateWindow();
			}
			///////////////////解锁码///////////////////////////////////////

			//CString strSQL = "Insert into dse_barcode_imei(imei,IMEI_NUM,barcode,msn,WIP_ENTITY_NAME,TYPE,MOBCLASS,LINENUM,classnum,PRODADDRESS,create_time,is_return,SOFTVERSION,HARDVERSION,IMEI_WRITER,WIFI_MAC,BLUE_MAC) values('"+m_IMEI+"','"+IMEI_Num+"','"+BoardNumber+"','"+m_SN+"','"+Order_Num_up_M+"','"+Machine_Type_M+"','"+Modulation_Tppe_M+"','"+Line_Number_M+"','"+Floor_M+"','"+Product_Address_M+"','"+strTime+"','"+Is_Return_M+"','"+Software_Version_M+"','"+Hardware_Version_M+"','"+Operator_M+"','','"+m_BT_Addr+"')";
			CString strSQL = "Insert into dse_barcode_imei(imei,NetCode,Unlock_Code,IMEI2,IMEI3,IMEI4,barcode,msn,WIP_ENTITY_NAME,TYPE,LINENUM,create_time,is_return,SOFTVERSION,HARDVERSION,WIFI_MAC,BLUE_MAC)\
							 values('"+m_IMEI+"','"+NETCODE_Input+"','"+Unlock_code_Number+"','"+m_IMEI2+"','"+m_IMEI3+"','"+m_IMEI4+"','"+BARCODE_Input+"','"+m_SN+"','"+Order_Num_up_M+"','"+Machine_Type_M+"','"+Line_Number_M+"','"+strTime+"','"+Is_Return_M+"','"+Software_Version_M+"','"+Hardware_Version_M+"','"+m_strWifi+"','"+m_BT_Addr+"')";	
			
			
			Setdlg.myado.OpenSheet("select * from dse_barcode_imei");
			BOOL UP_FAIL=Setdlg.myado.Execute(strSQL);
			Setdlg.myado.CloseSheet();
			Sleep(500);
			Setdlg.myado.OpenSheet("select imei from dse_barcode_imei WHERE imei ='"+m_IMEI+"'");
			BOOL IMEI_Check_UP=Setdlg.myado.Find("imei='"+m_IMEI+"'");
			Setdlg.myado.CloseSheet();
			
			BOOL UP_WritedFlag;
			if((UP_FAIL==TRUE&&IMEI_Check_UP==TRUE))
			{
				

				//AfxMessageBox("上传SQL成功！");
				m_Result.GetWindowText(LOG_Show_Old);
				m_Result.SetWindowText("成功写号！！！并且成功上传SQL_SERVER数据库！！！\r\n\r\n"+LOG_Show_Old);
				if(UpdateIMEI_MSN_Choose_M=="true")
				{
					CString strSQL_WritedFlag="UPDATE dse_imei_sn SET WritedFlag = '1' WHERE imei ='"+m_IMEI+"'";
					Setdlg.myado.OpenSheet("select * from dse_imei_sn WHERE imei ='"+m_IMEI+"'");
					UP_WritedFlag=Setdlg.myado.Execute(strSQL_WritedFlag);
					Setdlg.myado.CloseSheet();

					if(UP_WritedFlag==TRUE)
					{
						m_Result.GetWindowText(LOG_Show_Old);
						m_Result.SetWindowText("更新贴纸表标志成功！\r\n\r\n"+LOG_Show_Old);
					}
					else
					{
						m_Result.GetWindowText(LOG_Show_Old);
						m_Result.SetWindowText("更新贴纸表标志失败！！！\r\n\r\n"+LOG_Show_Old);
					}
					//UpdateData(true);  //把中间值赋给变量
					//UpdateData(false); //把变量显示在界面
					m_Result.UpdateWindow();
				}
				else
				{
					m_Result.GetWindowText(LOG_Show_Old);
					m_Result.SetWindowText("没有勾选更新贴纸表标志..\r\n\r\n"+LOG_Show_Old);
					m_Result.UpdateWindow();
					UP_WritedFlag=TRUE;
				}
				bool ORACLE_UpDatabase_RESULT=true;//上传ORACLE数据库结果
			    if(ORACLE_Connect_Choose_M=="true")//上传ORACLE数据库
				{
					ORACLE_UpDatabase_RESULT=Update_Oracle_Database();
				}
				if((ORACLE_UpDatabase_RESULT==true)&&(UP_WritedFlag==TRUE))
				{
					///////自动生成计数处理/////////////////////
					if(WIFIAddrCount_int>BtAddrCount_int)	//当只有wifi的时候，BtAddrCount_int为0；采用WIFIAddrCount_int
						BtAddrCount_int=WIFIAddrCount_int;
					BtAddrCount_int++;
					BtAddrCount.Format("%d",BtAddrCount_int);

					IMEI_Count.Format("%d",IMEI_Count_int);  //IMEI_Count_int 已经递增
					SaveConfig_Change();
					//////////自动生成计数处理//////////////////

					gColor = RGB(0,255,0);
					gFinalResult.Format("     OK");
					RedrawWindow();  m_Result.UpdateWindow(); 
					//OnSelchangeAuto();    //不将IMEI1清零
					//IMEI_Control.SetFocus();
					//IMEI_Control.SetSel(0,-1);
				}
				else if(UP_WritedFlag!=TRUE)
				{
					m_Result.GetWindowText(LOG_Show_Old);
					m_Result.SetWindowText("成功写号上传数据！！！但更新贴纸表标志失败！！！！！！！！！！！！！\r\n\r\n"+LOG_Show_Old);
					m_Result.UpdateWindow();

					gColor = RGB(255,0,0);
					gFinalResult.Format("UPDA_F");
					RedrawWindow();  m_Result.UpdateWindow(); 
					return false;
				}
			}
			else
			{
				m_Result.GetWindowText(LOG_Show_Old);
				m_Result.SetWindowText("成功写号！！！但上传数据库失败！！！！！！！！！！！！！\r\n\r\n"+LOG_Show_Old);
				//UpdateData(true);  //把中间值赋给变量
				//UpdateData(false); //把变量显示在界面
				m_Result.UpdateWindow();

				gColor = RGB(255,0,0);
				gFinalResult.Format("UP_FAIL");
				RedrawWindow();  m_Result.UpdateWindow(); 
				//OnSelchangeAuto();    //不将IMEI1清零
				//IMEI_Control.SetFocus();
				//IMEI_Control.SetSel(0,-1);
				return false;
			}
		}
		else
		{
			m_Result.GetWindowText(LOG_Show_Old);  
			m_Result.SetWindowText("写号成功，但因没有选择   初始化时连接数据库   无法上传数据库\r\n\r\n"+LOG_Show_Old);
			gColor = RGB(255,0,0);
			gFinalResult.Format("     UP_Fail");
			RedrawWindow();  m_Result.UpdateWindow(); 
			return false;
		}
        //m_strMsg.LoadString( "PASS! 按Enter开始下一次测试");
	}
	else
    {
        m_Result.GetWindowText(LOG_Show_Old);  
		m_Result.SetWindowText("写号失败！！！\r\n\r\n"+LOG_Show_Old);
		gColor = RGB(255,0,0);
		gFinalResult.Format("     Fail");
		RedrawWindow();  m_Result.UpdateWindow(); 
		//IMEI_Control.SetFocus();
		//IMEI_Control.SetSel(0,-1);
		return false;
	}

	RedrawWindow();  m_Result.UpdateWindow(); 
	IMEI_Control.SetFocus();
	IMEI_Control.SetSel(0,-1);
	//初始化时连接了数据库、每次重连数据库、
	/*if(DB_CONNECT_M=="true")//数据库关闭处理
	{
		Setdlg.myado.CloseDataLibrary();//连接关闭必须在一个子函数内，不然出错
	}*/
	LOG_Show_Old="";
	return true;
}
void CWriteIMEIDlg::OnCbnSelchangeCombo2_Fresh()
{
	//int Date_Count;//时间点计数
	//Date_Count=m_Port.GetCount();
	UpdateData(TRUE);
	for(int del=0;del<64;del++)
	{
		m_Port.DeleteString(0);//
	}

	int Current_Port=0;
	m_Port.AddString("USB");
	for( int i=1; i<128; i++ )
	{
		CString sPort;
		sPort.Format(_T("\\\\.\\COM%d"),i);
		BOOL bSuccess=FALSE;
		HANDLE hPort=::CreateFile(sPort, GENERIC_READ|GENERIC_WRITE, 0, 0,
			OPEN_EXISTING, 0, 0);
		if( hPort == INVALID_HANDLE_VALUE)
		{
			hPort=NULL;
			DWORD dwError=GetLastError();
			if( dwError == ERROR_ACCESS_DENIED)
				bSuccess=TRUE;
		}
		else
		{
			bSuccess=TRUE;
			CloseHandle(hPort);
			Current_Port++;
		}
		if(  bSuccess )
		{
			CString str;
			str.Format("COM%d",i);
			m_Port.AddString(str);
		}
	}
	m_Port.SetCurSel(Current_Port-1);
	UpdateData(FALSE);
}

void CWriteIMEIDlg::OnContextMenu(CWnd*, CPoint point)//菜单
{
	if (point.x == -1 && point.y == -1)
	{
		//keystroke invocation
		CRect rect;
		GetClientRect(rect);
		ClientToScreen(rect);

		point = rect.TopLeft();
		point.Offset(5, 5);
	}

	CMenu menu;
	VERIFY(menu.LoadMenu(IDR_MENU1));

	CMenu* pPopup = menu.GetSubMenu(0);
	ASSERT(pPopup != NULL);
	CWnd* pWndPopupOwner = this;

	while (pWndPopupOwner->GetStyle() & WS_CHILD)
		pWndPopupOwner = pWndPopupOwner->GetParent();

	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, point.x, point.y,
		pWndPopupOwner);
}


void CWriteIMEIDlg::OnRButtonUp(UINT nFlags, CPoint point) //右键弹起
{
	// TODO: Add your message handler code here and/or call default
	
	CDialog::OnRButtonUp(nFlags, point);
}

void CWriteIMEIDlg::OnBnClickedButton3()//META模式写号
{
	//Update_Oracle_Database();
    // TODO: Add your control notification handler code here
	UpdateData(TRUE);
	CLeadcoreChip::Tstop=0;  
	/*if(INITIAL_POWER!=true)
	{
		AfxMessageBox("请授权后使用...");
		return;
	}*/
	extern COLORREF gColor;
	extern CString gFinalResult;

	CString Plat_Form_CS;
	Plat_Form.GetWindowTextA(Plat_Form_CS);
	if(Plat_Form_CS=="MTK智能机")
	{
		if(m_strExchangeFileName_Ap.IsEmpty())
		{ 
			MessageBox("请选择MTK智能机配置文件先!","错误",MB_OK);
			return ;
		}
	}
	if(Plat_Form_CS=="MTK功能机")
	{
		if(m_strExchangeFileName.IsEmpty())
		{ 
			MessageBox("请选择MTK功能机配置文件先!","错误",MB_OK);
			return ;
		}
	}
	LOG_Show_Old=LOG_Show_Old.Left(200);
	m_Result.SetWindowText("META/AT模式写号....\r\n\r\n"+LOG_Show_Old);
	m_Result.UpdateWindow();//直接点击的时候，清除旧的log信息
	gFinalResult.Format("运行中..");
	RedrawWindow();  m_Result.UpdateWindow(); 
	m_nComPort=-1;
    DWORD dwEnterModeID = 0;
    UpdateData(FALSE); //开始后更新显示
	///////////根据号段生成IMEI//////////////////
	LoadConfig_Change();//读取IMEI、MAC计数值

	////////////////
	CHAR szTemp[50]={0};
    _tcscpy(szTemp,IMEIStart_M); 
    m_int64DefaultIMEI=  _ttoi64(szTemp);
	IMEI_Count_int=atol(IMEI_Count);  
	m_int64DefaultIMEI=m_int64DefaultIMEI+IMEI_Count_int;

	if((AutoIMEI_Choose_M=="true")&&(IMEI1_Choose_M=="true"))                             //自动生成，不使用扫描枪模式
    {
        AutoCreateIMEI(szTemp);
        m_IMEI.Format(("%s"),szTemp);
		IMEI_Count_int++;//计数加1，为下次开始准备
		m_int64DefaultIMEI++;
    }
    if((AutoIMEI_Choose_M=="true")&&(IMEI2_Choose_M=="true"))   //自动生成，不使用扫描枪模式
    {
        if(UseSameIMEI_Choose_M=="false")
        {
            AutoCreateIMEI(szTemp);
            m_IMEI2.Format(("%s"),szTemp);
			IMEI_Count_int++;//计数加1
			m_int64DefaultIMEI++;
        }
        else
        {
            m_IMEI2=m_IMEI;
        }
    }  
    if((AutoIMEI_Choose_M=="true")&&(IMEI3_Choose_M=="true"))   //自动生成，不使用扫描枪模式
    {
        if(UseSameIMEI_Choose_M=="false")
        {
            AutoCreateIMEI(szTemp);
            m_IMEI3.Format(("%s"),szTemp);
			IMEI_Count_int++;//计数加1
			m_int64DefaultIMEI++;
        }
        else
        {
            m_IMEI3=m_IMEI;
        }
    } 
    if((AutoIMEI_Choose_M=="true")&&(IMEI4_Choose_M=="true"))   //自动生成，不使用扫描枪模式
    {
        if(UseSameIMEI_Choose_M=="false")
        {
            AutoCreateIMEI(szTemp);
            m_IMEI4.Format(("%s"),szTemp);
			IMEI_Count_int++;//计数加1，若4个IMEI全选，则IMEI_Count_int增加了4，XXXXXXXXXXXXX0Y--->XXXXXXXXXXXXX3Y--->XXXXXXXXXXXXX4Y,成功后保存
			m_int64DefaultIMEI++;
        }
        else
        {
            m_IMEI4=m_IMEI;
        }
    } 
	UpdateData(false); //把变量显示在界面

	///////////根据号段生成IMEI//////////////////
	///////////根据号段生成mac///////////////////
	if((AutoBTAddr_Choose_M=="true")&&(BT_MAC_ADDR_Choose_M=="true"))
	{
		/*char a[100];
		CString str(”aaaaaa”);
		strncpy(a,(LPCTSTR)str,sizeof(a));*/
		int  BT_addr1[6];
		long BT_addr2=0;
		long BT_addr3=0;
		long BT_addr4=0;
		char m_BT_Addr_Temp3[5];

		for(int i=1;i<7;i++)
		{
			m_BT_Addr_Temp1=BtAddrStart_M.Right(i);
			m_BT_Addr_Temp2=m_BT_Addr_Temp1.Left(1);
			strcpy(m_BT_Addr_Temp3,(LPCTSTR)m_BT_Addr_Temp2);	//strncpy(m_BT_Addr_Temp3,(LPCTSTR)m_BT_Addr_Temp2,sizeof(m_BT_Addr_Temp2));	
			BT_addr1[i-1]=CharToHexChar(m_BT_Addr_Temp3[0]);    //BT_addr1[i-1]=atoi(m_BT_Addr_Temp3);
			BT_addr2+=BT_addr1[i-1]*(pow((double)16,(i-1)));
		}
		BtAddrCount_int=atol(BtAddrCount);                      //BT地址计数
		BT_addr3=BtAddrCount_int+BT_addr2;
		m_BT_Addr_Temp1.Format("%X",BT_addr3);

		m_BT_Addr_Temp1="000000"+m_BT_Addr_Temp1;//弥补从右往左第6位为0的bug  OnWrite
		m_BT_Addr_Temp1=m_BT_Addr_Temp1.Right(6);//弥补从右往左第6位为0的bug

		m_BT_Addr_Temp2=BtAddrStart_M.Left(6);
		m_BT_Addr=m_BT_Addr_Temp2+m_BT_Addr_Temp1;              //自动生成，对m_BT_Addr赋值
		UpdateData(false); //把变量显示在界面
	}
	if((AutoWIFIAddr_Choose_M=="true")&&(WIFI_MAC_ADDR_Choose_M=="true"))
	{
		int BT_addr1[6];
		long BT_addr2=0;
		long BT_addr3=0;
		long BT_addr4=0;
		char m_BT_Addr_Temp3[5];

		for(int i=1;i<7;i++)//wifi起始地址低6位转化为10进制
		{
			m_BT_Addr_Temp1=WIFIAddrStart_M.Right(i);
			m_BT_Addr_Temp2=m_BT_Addr_Temp1.Left(1);
			strcpy(m_BT_Addr_Temp3,(LPCTSTR)m_BT_Addr_Temp2);	//strncpy(m_BT_Addr_Temp3,(LPCTSTR)m_BT_Addr_Temp2,sizeof(m_BT_Addr_Temp2));	
			BT_addr1[i-1]=CharToHexChar(m_BT_Addr_Temp3[0]);    //BT_addr1[i-1]=atoi(m_BT_Addr_Temp3);
			BT_addr2+=BT_addr1[i-1]*(pow((double)16,(i-1)));
		}
		WIFIAddrCount_int=atol(BtAddrCount);                      //WIFI_MAC地址计数
		BT_addr3=WIFIAddrCount_int+BT_addr2;
		m_BT_Addr_Temp1.Format("%X",BT_addr3);

		m_BT_Addr_Temp1="000000"+m_BT_Addr_Temp1;//弥补从右往左第6位为0的bug  OnWrite
		m_BT_Addr_Temp1=m_BT_Addr_Temp1.Right(6);//弥补从右往左第6位为0的bug

		m_BT_Addr_Temp2=WIFIAddrStart_M.Left(6);
		m_strWifi=m_BT_Addr_Temp2+m_BT_Addr_Temp1;              //得到当前WIFI的MAC地址
		UpdateData(false); //把变量显示在界面
	}
	///////////根据号段生成mac///////////////////
	
	//if(IMEI_CheckValid_Choose_M=="true")//不需要总开关
	{
		if(!CheckIMEI())   //检查输入正确否,每个IMEI\mac单独检查
		{   
			m_Result.GetWindowText(LOG_Show_Old);  
			m_Result.SetWindowText(LOG_Show_Old+CheckIMEI_LOG);
			m_Result.UpdateWindow();
			IMEI_Control.SetFocus();
			IMEI_Control.SetSel(0,-1);
			RedrawWindow();  m_Result.UpdateWindow(); 
			return;
		}
		
	}
	//仅上传IMEI和网标、不写号选择
	if(JUST_UpDate_DB_Choose_M=="true")
	{
		//上传数据
		Only_Update_Netcode();
		return;
	}
	
	//仅上传IMEI和网标、不写号选择
	//OnWrite();//串口AT指令模式

	m_Result.GetWindowText(LOG_Show_Old);  
	if(Plat_Form_CS=="展讯平台")//展讯平台
	{
		m_Result.SetWindowText("==================================IMEI_BTMAC_WIFIMAC合法，请在"+WatchDog_Time_M+"毫秒内连接上手机并加电!\r\n\r\n"+LOG_Show_Old);
	}
	else if(Plat_Form_S=="M255-AT")
		m_Result.SetWindowText("M255串口AT写号开始...\r\n\r\n"+LOG_Show_Old);
	else if((Plat_Form_CS=="MTK功能机")||(Plat_Form_CS=="MTK智能机"))
		m_Result.SetWindowText("==================================IMEI_BTMAC_WIFIMAC合法，请在"+WatchDog_Time_M+"毫秒内将数据线连接上MTK手机(若使用虚拟串口，请提前连接上)...........\r\n\r\n"+LOG_Show_Old);

	UpdateData(true);  //把中间值赋给变量
	UpdateData(false); //把变量显示在界面
	m_Result.UpdateWindow();
    //EnableWindow(FALSE);//Disable 
	//进入模式：
	if(Plat_Form_CS=="展讯平台")//展讯平台
	{
		g_hDiagPhone=INVALID_PHONE_HANDLE;//添加
		if(g_hDiagPhone)
		{
			SP_ReleasePhone(g_hDiagPhone);
		}
		g_hDiagPhone = SP_CreatePhone(NULL);
		m_Port.GetWindowText(m_bUsbMode);
		if (m_bUsbMode!="USB")
		{   // 串口
			m_bUsbMode=m_bUsbMode.Right(m_bUsbMode.GetLength()-3);
			m_nComPort=atoi(m_bUsbMode);
			ca.ChannelType  = CHANNEL_TYPE_COM;
			ca.Com.dwPortNum  = m_nComPort;
			ca.Com.dwBaudRate  = 115200;
		}
		else
		{   // USB
			ca.ChannelType = CHANNEL_TYPE_USBMON;
		}
		if(RFT_SUCCESS!=SP_BeginPhoneTest(g_hDiagPhone, &ca))
		{
			m_Result.GetWindowText(LOG_Show_Old);  
			m_Result.SetWindowText("打开串口"+m_bUsbMode+"失败!！！！\r\n\r\n"+LOG_Show_Old);
			gColor = RGB(255,0,0);
			gFinalResult.Format("     Fail");
			RedrawWindow();  m_Result.UpdateWindow(); 
			IMEI_Control.SetFocus();
			IMEI_Control.SetSel(0,-1);
			return;
		}
		else
		{
			LOG_Show_Old="打开串口"+m_bUsbMode+"成功！！！\r\n\r\n"+LOG_Show_Old;//不去更新log信息
		}
	}
    

    ResetEvent(m_hExitThread);            //                                     
    ResetEvent(m_hEnterModeSuccess);
    //m_hEnterModeThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)EnterModeProc,this, NULL, &dwEnterModeID);
	m_hEnterModeThread=AfxBeginThread(EnterModeProc,(LPVOID)this,THREAD_PRIORITY_NORMAL,0,0,NULL);//创建测试线程

	if(Plat_Form_CS=="展讯平台")//展讯平台
	{
		bool UsbMode;
		if(m_bUsbMode=="USB")
		{
			UsbMode=true;
		}
		else
		{
			UsbMode=false;
		}
		/*if(TestMode_M=="PC_MC_RM_CALIBR_NV_WRITE_MODE")
		{
			SP_EnterModeProcess(UsbMode, PC_MC_RM_CALIBR_NV_WRITE_MODE, m_hEnterModeSuccess, 0);
		}
		else if(TestMode_M=="PC_MC_CALIBRATION_MODE")
		{
			SP_EnterModeProcess(UsbMode, PC_MC_CALIBRATION_MODE, m_hEnterModeSuccess, 0);
		}*/

		//HANDLE hMonitorEvent = CreateEvent (NULL, TRUE, FALSE, NULL);
		//if(RFT_SUCCESS!=SP_EnterModeProcess (g_hDiagPhone, UsbMode, -1, RM_CALIBR_POST_MODE, hMonitorEvent, atol(WatchDog_Time_M)))//WatchDog_Time_M  ms后超时
		int RESULT=0;
		if(TestMode_M=="RM_NORMAL_MODE")
		{
			RESULT=SP_EnterModeProcess (g_hDiagPhone, UsbMode, -1, RM_NORMAL_MODE, m_hEnterModeSuccess, atol(WatchDog_Time_M));
		}
		else if(TestMode_M=="RM_CALIBRATION_MODE")
		{
			RESULT=SP_EnterModeProcess (g_hDiagPhone, UsbMode, -1, RM_CALIBRATION_MODE, m_hEnterModeSuccess, atol(WatchDog_Time_M));
		}
		else if(TestMode_M=="RM_CALIBR_POST_MODE")
		{
			RESULT=SP_EnterModeProcess (g_hDiagPhone, UsbMode, -1, RM_CALIBR_POST_MODE, m_hEnterModeSuccess, atol(WatchDog_Time_M));
		}
		else if(TestMode_M=="RM_CALIBR_POST_NO_LCM_MODE")
		{
			RESULT=SP_EnterModeProcess (g_hDiagPhone, UsbMode, -1, RM_CALIBR_POST_NO_LCM_MODE, m_hEnterModeSuccess, atol(WatchDog_Time_M));
		}


		else if(TestMode_M=="RM_TD_CALITRATION_MODE")
		{
			RESULT=SP_EnterModeProcess (g_hDiagPhone, UsbMode, -1, RM_TD_CALITRATION_MODE, m_hEnterModeSuccess, atol(WatchDog_Time_M));
		}
		else if(TestMode_M=="RM_TD_CALIBR_POST_MODE")
		{
			RESULT=SP_EnterModeProcess (g_hDiagPhone, UsbMode, -1, RM_TD_CALIBR_POST_MODE, m_hEnterModeSuccess, atol(WatchDog_Time_M));
		}
		else if(TestMode_M=="RM_TD_CALIBR_POST_NO_LCM_MODE")
		{
			RESULT=SP_EnterModeProcess (g_hDiagPhone, UsbMode, -1, RM_TD_CALIBR_POST_NO_LCM_MODE, m_hEnterModeSuccess, atol(WatchDog_Time_M));
		}
		else if(TestMode_M=="RM_CALIBR_NV_ACCESS_MODE")
		{
			RESULT=SP_EnterModeProcess (g_hDiagPhone, UsbMode, -1, RM_CALIBR_NV_ACCESS_MODE, m_hEnterModeSuccess, atol(WatchDog_Time_M));
		}
		else
		{
			RESULT=SP_EnterModeProcess (g_hDiagPhone, UsbMode, -1, RM_CALIBR_NV_ACCESS_MODE, m_hEnterModeSuccess, atol(WatchDog_Time_M));
		}

		if(RFT_SUCCESS!=RESULT)//WatchDog_Time_M  ms后超时
		{
			//AfxMessageBox("手机连接失败！！！");
			ResetEvent (m_hEnterModeSuccess);
			ResetEvent(m_hExitThread);
			SP_StopModeProcess(g_hDiagPhone);//超时处理
			EnableWindow(TRUE);
			m_Result.GetWindowText(LOG_Show_Old);  
			m_Result.SetWindowText("手机连接失败！！！\r\n\r\n"+LOG_Show_Old);
			gColor = RGB(255,0,0);
			gFinalResult.Format("     Fail");
			RedrawWindow();  m_Result.UpdateWindow(); 
			IMEI_Control.SetFocus();
			IMEI_Control.SetSel(0,-1);
			return;//OutLog("ERROR:Failed to Enter Mode!");
		}

	}
	///////////////8810平台///////////////////////////

	///////////////8810平台///////////////////////////
}



__int64 CWriteIMEIDlg::ComputeCD(__int64 nImei)
{
    int nTime = 0;
    __int64 nCD = 0;  
    while(nImei != 0)
    {
        __int64 d = nImei % 10;
        if(nTime % 2 == 0)//将IMEI高14位的偶数位  乘以2，计算个位数和十位数之和，
        {
            d *= 2;
            if(d >= 10)
            {
                d = d % 10 + d / 10;
            }
        }   
        nCD += d;   //再加上奇位数
        nTime++;
        nImei /= 10;
    }  
    nCD %= 10;      //算出上述和 的个位数
    if(nCD != 0)    //该个位数是0 ，则校验位为0，否则为10-该个位数
    {
        nCD = 10 - nCD;
    }  
    return nCD;
}

BOOL CWriteIMEIDlg::CheckIMEI()
{
	extern COLORREF gColor;
	extern CString gFinalResult;
    UpdateData(true);
	CheckIMEI_LOG="";//每次清空
    if((IMEI1_Choose_M=="false")&&(IMEI2_Choose_M=="false")&&(IMEI3_Choose_M=="false")&&(IMEI4_Choose_M=="false")&&(BT_MAC_ADDR_Choose_M=="false")&&(WIFI_MAC_ADDR_Choose_M=="false"))
    {
        //m_strMsg.LoadString("请在设置对话框里选择测试项!");
        return FALSE;
    }
	if(UseSameIMEI_Choose_M=="false")//检查多个IMEI是否相同，防止扫描错误
	{
		if((IMEI1_Choose_M=="true")&&(IMEI2_Choose_M=="true")&&(IMEI3_Choose_M=="true")&&(IMEI4_Choose_M=="true"))
		{
			if((m_IMEI==m_IMEI2)||(m_IMEI==m_IMEI3)||(m_IMEI==m_IMEI4)||    (m_IMEI2==m_IMEI3)||(m_IMEI2==m_IMEI4)||    (m_IMEI3==m_IMEI4))
			{
				PrintError("IMEI输入有相同号，可能是扫描出错！！！！！！");
				m_Write.EnableWindow(true);
				CheckIMEI_LOG="IMEI输入有相同号，可能是扫描出错！！！！！！\r\n\r\n"+LOG_Show_Old;
				OnSelchangeAuto();
				IMEI_Control.SetFocus();
				IMEI_Control.SetSel(0,-1);
				return FALSE;   
			}
			else 
			{
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("IMEI号输入是唯一的，正常继续...\r\n\r\n"+LOG_Show_Old);
				m_Result.UpdateWindow();
			}

		}
		else if((IMEI1_Choose_M=="true")&&(IMEI2_Choose_M=="true")&&(IMEI3_Choose_M=="true")&&(IMEI4_Choose_M=="false"))
		{
			if((m_IMEI==m_IMEI2)||(m_IMEI==m_IMEI3)||    (m_IMEI2==m_IMEI3))
			{
				PrintError("IMEI输入有相同号，可能是扫描出错！！！！！！");
				m_Write.EnableWindow(true);
				CheckIMEI_LOG="IMEI输入有相同号，可能是扫描出错！！！！！！\r\n\r\n"+LOG_Show_Old;
				OnSelchangeAuto();
				IMEI_Control.SetFocus();
				IMEI_Control.SetSel(0,-1);
				return FALSE;   
			}
			else 
			{
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("IMEI号输入是唯一的，正常继续...\r\n\r\n"+LOG_Show_Old);
				m_Result.UpdateWindow();
			}
		}
		else if((IMEI1_Choose_M=="true")&&(IMEI2_Choose_M=="true")&&(IMEI3_Choose_M=="false")&&(IMEI4_Choose_M=="false"))
		{
			if((m_IMEI==m_IMEI2))
			{
				PrintError("IMEI输入有相同号，可能是扫描出错！！！！！！");
				m_Write.EnableWindow(true);
				CheckIMEI_LOG="IMEI输入有相同号，可能是扫描出错！！！！！！\r\n\r\n"+LOG_Show_Old;
				OnSelchangeAuto();
				IMEI_Control.SetFocus();
				IMEI_Control.SetSel(0,-1);
				return FALSE;   
			}
			else 
			{
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("IMEI号输入是唯一的，正常继续...\r\n\r\n"+LOG_Show_Old);
				m_Result.UpdateWindow();
			}
		}
		else 
		{
			m_Result.GetWindowText(LOG_Show_Old);  
			m_Result.SetWindowText("IMEI号输入是唯一的，正常继续...\r\n\r\n"+LOG_Show_Old);
			m_Result.UpdateWindow();
		}
	}
    /*if(m_bReadFun)
    {
        return TRUE;
    }*/





	/////////////////////检查号段/////////////////////////////
	//////////////////////////////////////////////////
	//////////////////////////////////////////////////
	CString IMEIStart_M_temp;
	//long    IMEIStart_Number;                //作为全局变量
	IMEIStart_M_temp=IMEIStart_M.Left(14);
	IMEIStart_M_temp=IMEIStart_M_temp.Right(6);
	IMEIStart_Number=atol(IMEIStart_M_temp);   //IMEI起始号

	CString IMEIEnd_M_temp;  
	//long    IMEIEnd_Number;                  //作为全局变量
	IMEIEnd_M_temp=IMEIEnd_M.Left(14);
	IMEIEnd_M_temp=IMEIEnd_M_temp.Right(6);
	IMEIEnd_Number=atol(IMEIEnd_M_temp);       //IMEI结束号
	//////////////////////////////////////////////////
	//////////////////////////////////////////////////
	//IMEI1
	if(IMEI1_Choose_M=="true")
	{
		CString m_IMEI_Limit;
		long    m_IMEI_Number;
		m_IMEI_Limit=m_IMEI.Left(14);
		m_IMEI_Limit=m_IMEI_Limit.Right(6);
		m_IMEI_Number=atol(m_IMEI_Limit);          //IMEI1的6位串号
		if( ((m_IMEI_Number>IMEIEnd_Number)||(m_IMEI_Number<IMEIStart_Number)) )//检查号段
		{
			PrintError("输入的IMEI已超过限定的号段，请重新扫描! \r\n\r\n");
			m_Write.EnableWindow(true);
			OnSelchangeAuto();
			IMEI_Control.SetFocus();
			IMEI_Control.SetSel(0,-1);
			return FALSE;
		}
		else
		{
			m_Result.GetWindowText(LOG_Show_Old);
			m_Result.SetWindowText("输入的IMEI在限定的号段内，继续中...! \r\n\r\n"+LOG_Show_Old);
			UpdateData(true);  //把中间值赋给变量
			UpdateData(false); //把变量显示在界面
			m_Result.UpdateWindow();
		}
	}

	//IMEI2
	if(IMEI2_Choose_M=="true")
	{
		CString m_IMEI2_Limit;
		long    m_IMEI2_Number;
		m_IMEI2_Limit=m_IMEI2.Left(14);
		m_IMEI2_Limit=m_IMEI2_Limit.Right(6);
		m_IMEI2_Number=atol(m_IMEI2_Limit);          //IMEI1的6位串号
		if( ((m_IMEI2_Number>IMEIEnd_Number)||(m_IMEI2_Number<IMEIStart_Number)) )//检查号段
		{
			PrintError("输入的IMEI2已超过限定的号段，请重新扫描! \r\n\r\n");
			m_Write.EnableWindow(true);
			IMEI2_Control.SetFocus();
			IMEI2_Control.SetSel(0,-1);
			IMEI2_Result=false;
			return FALSE;
		}
		else
		{
			m_Result.GetWindowText(LOG_Show_Old);
			IMEI2_Result=true;
			m_Result.SetWindowText("输入的IMEI2在限定的号段内，继续中...! \r\n\r\n"+LOG_Show_Old);
			UpdateData(true);  //把中间值赋给变量
			UpdateData(false); //把变量显示在界面
			m_Result.UpdateWindow();
		}
	}

	//IMEI3
	if(IMEI3_Choose_M=="true")
	{
		CString m_IMEI3_Limit;
		long    m_IMEI3_Number;
		m_IMEI3_Limit=m_IMEI3.Left(14);
		m_IMEI3_Limit=m_IMEI3_Limit.Right(6);
		m_IMEI3_Number=atol(m_IMEI3_Limit);          //IMEI1的6位串号
		if( ((m_IMEI3_Number>IMEIEnd_Number)||(m_IMEI3_Number<IMEIStart_Number)) )//检查号段
		{
			PrintError("输入的IMEI3已超过限定的号段，请重新扫描! \r\n\r\n");
			m_Write.EnableWindow(true);
			IMEI3_Control.SetFocus();
			IMEI3_Control.SetSel(0,-1);
			IMEI3_Result=false;
			return FALSE;
		}
		else
		{
			IMEI3_Result=true;
			m_Result.GetWindowText(LOG_Show_Old);
			m_Result.SetWindowText("输入的IMEI3在限定的号段内，继续中...! \r\n\r\n"+LOG_Show_Old);
			UpdateData(true);  //把中间值赋给变量
			UpdateData(false); //把变量显示在界面
			m_Result.UpdateWindow();

		}
	}

	//IMEI4
	if(IMEI4_Choose_M=="true")
	{
		CString m_IMEI4_Limit;
		long    m_IMEI4_Number;
		m_IMEI4_Limit=m_IMEI4.Left(14);
		m_IMEI4_Limit=m_IMEI4_Limit.Right(6);
		m_IMEI4_Number=atol(m_IMEI4_Limit);          //IMEI1的6位串号
		if( ((m_IMEI4_Number>IMEIEnd_Number)||(m_IMEI4_Number<IMEIStart_Number)) )//检查号段
		{
			PrintError("输入的IMEI4已超过限定的号段，请重新扫描! \r\n\r\n");
			m_Write.EnableWindow(true);
			IMEI4_Control.SetFocus();
			IMEI4_Control.SetSel(0,-1);
			IMEI4_Result=false;
			return FALSE;
		}
		else
		{
			IMEI4_Result=true;
			m_Result.GetWindowText(LOG_Show_Old);
			m_Result.SetWindowText("输入的IMEI4在限定的号段内，继续中...! \r\n\r\n"+LOG_Show_Old);
			UpdateData(true);  //把中间值赋给变量
			UpdateData(false); //把变量显示在界面
			m_Result.UpdateWindow();
		}
	}

	//BT
	if(BT_MAC_ADDR_Choose_M=="true")
	{
		LoadConfig_Change();
		/*char a[100];
		CString str(”aaaaaa”);
		strncpy(a,(LPCTSTR)str,sizeof(a));*/
		int BT_addr1[12];
		long BT_addr2=0;
		long BT_addr3=0;
		long BT_addr4=0;
		char m_BT_Addr_Temp3[5];

		

		int i=0;
		//蓝牙地址的低6位
		for(i=1;i<7;i++)
		{
			m_BT_Addr_Temp1=BT_Addr_EndShow_M.Right(i);
			m_BT_Addr_Temp2=m_BT_Addr_Temp1.Left(1);
			strcpy(m_BT_Addr_Temp3,(LPCTSTR)m_BT_Addr_Temp2);	//strncpy(m_BT_Addr_Temp3,(LPCTSTR)m_BT_Addr_Temp2,sizeof(m_BT_Addr_Temp2));	
			BT_addr1[i-1]=CharToHexChar(m_BT_Addr_Temp3[0]);    //BT_addr1[i-1]=atoi(m_BT_Addr_Temp3);
			BT_addr4+=BT_addr1[i-1]*(pow((double)16,(i-1)));    //BT结束地址后6位
		}

		for(i=1;i<7;i++)
		{
			m_BT_Addr_Temp1=BtAddrStart_M.Right(i);
			m_BT_Addr_Temp2=m_BT_Addr_Temp1.Left(1);
			strcpy(m_BT_Addr_Temp3,(LPCTSTR)m_BT_Addr_Temp2);	//strncpy(m_BT_Addr_Temp3,(LPCTSTR)m_BT_Addr_Temp2,sizeof(m_BT_Addr_Temp2));	
			BT_addr1[i-1]=CharToHexChar(m_BT_Addr_Temp3[0]);    //BT_addr1[i-1]=atoi(m_BT_Addr_Temp3);
			BT_addr2+=BT_addr1[i-1]*(pow((double)16,(i-1)));
		}

		long BT_addr2_High=0;
		long BT_addr4_High=0;
		//蓝牙地址高6位
		for(i=7;i<13;i++)
		{
			m_BT_Addr_Temp1=BT_Addr_EndShow_M.Right(i);
			m_BT_Addr_Temp2=m_BT_Addr_Temp1.Left(1);
			strcpy(m_BT_Addr_Temp3,(LPCTSTR)m_BT_Addr_Temp2);	//strncpy(m_BT_Addr_Temp3,(LPCTSTR)m_BT_Addr_Temp2,sizeof(m_BT_Addr_Temp2));	
			BT_addr1[i-1]=CharToHexChar(m_BT_Addr_Temp3[0]);    //BT_addr1[i-1]=atoi(m_BT_Addr_Temp3);
			BT_addr4_High+=BT_addr1[i-1]*(pow((double)16,(i-7)));    //BT结束地址后6位
		}

		for(i=7;i<13;i++)
		{
			m_BT_Addr_Temp1=BtAddrStart_M.Right(i);
			m_BT_Addr_Temp2=m_BT_Addr_Temp1.Left(1);
			strcpy(m_BT_Addr_Temp3,(LPCTSTR)m_BT_Addr_Temp2);	//strncpy(m_BT_Addr_Temp3,(LPCTSTR)m_BT_Addr_Temp2,sizeof(m_BT_Addr_Temp2));	
			BT_addr1[i-1]=CharToHexChar(m_BT_Addr_Temp3[0]);    //BT_addr1[i-1]=atoi(m_BT_Addr_Temp3);
			BT_addr2_High+=BT_addr1[i-1]*(pow((double)16,(i-7)));
		}
		


		long BT_addr_Input=0;
		long BT_addr_Input_High=0;
		//输入的蓝牙地址
		for(i=1;i<7;i++)
		{
			m_BT_Addr_Temp1=m_BT_Addr.Right(i);
			m_BT_Addr_Temp2=m_BT_Addr_Temp1.Left(1);
			strcpy(m_BT_Addr_Temp3,(LPCTSTR)m_BT_Addr_Temp2);	//strncpy(m_BT_Addr_Temp3,(LPCTSTR)m_BT_Addr_Temp2,sizeof(m_BT_Addr_Temp2));	
			BT_addr1[i-1]=CharToHexChar(m_BT_Addr_Temp3[0]);    //BT_addr1[i-1]=atoi(m_BT_Addr_Temp3);
			BT_addr_Input+=BT_addr1[i-1]*(pow((double)16,(i-1)));
		}

		for(i=7;i<13;i++)
		{
			m_BT_Addr_Temp1=m_BT_Addr.Right(i);
			m_BT_Addr_Temp2=m_BT_Addr_Temp1.Left(1);
			strcpy(m_BT_Addr_Temp3,(LPCTSTR)m_BT_Addr_Temp2);	//strncpy(m_BT_Addr_Temp3,(LPCTSTR)m_BT_Addr_Temp2,sizeof(m_BT_Addr_Temp2));	
			BT_addr1[i-1]=CharToHexChar(m_BT_Addr_Temp3[0]);    //BT_addr1[i-1]=atoi(m_BT_Addr_Temp3);
			BT_addr_Input_High+=BT_addr1[i-1]*(pow((double)16,(i-7)));    //BT结束地址后6位
		}
		
		long BtAddrEnd_M_int=BT_addr4-BT_addr2+1;
		BtAddrEnd_M.Format("%ld",BtAddrEnd_M_int);
		BtAddrTotal=BtAddrEnd_M;                                //BtAddrTotal桌面显示总数，BtAddrEnd_M为设置后的总数，BtAddrEnd--》地址总数
		
		BtAddrCount_int=atol(BtAddrCount);                      //BT地址计数
		//BT_addr3=BtAddrCount_int+BT_addr2;
		//m_BT_Addr_Temp1.Format("%X",BT_addr3);
		//m_BT_Addr_Temp2=BtAddrStart_M.Left(6);
		//m_BT_Addr=m_BT_Addr_Temp2+m_BT_Addr_Temp1;            //得到当前BT_MAC地址,无论扫描输入或是自动生成，不再对m_BT_Addr重新赋值

		BtAddrTotal_int=atol(BtAddrTotal);                      //BT地址总数

		UpdateData(false);

		/*
		 if(strCommand == "BtAddrStart_M")
		{
     			  BtAddrStart_M = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");
		}
		else if(strCommand == "BtAddrCount")
		{
     			  BtAddrCount = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");
		}
		*/
		
		//check BT_Addr                                                                                      
		if( ((BtAddrTotal_int<=BtAddrCount_int)||  (((BT_addr_Input<BT_addr2)||(BT_addr_Input>BT_addr4))&&((BT_addr_Input_High>=BT_addr2_High)||(BT_addr_Input_High<=BT_addr4_High))) \
			||(BT_addr_Input_High<BT_addr2_High)||(BT_addr_Input_High>BT_addr4_High)) \
			 )//检查号段
		{
			PrintError("BT_Addr超过范围！");
			m_BT_Addr_Control.SetFocus();
			m_BT_Addr_Control.SetSel(0,-1);
			return FALSE;
		}
		else
		{
			m_Result.GetWindowText(LOG_Show_Old);
			m_Result.SetWindowText("输入的BT_Addr在限定的号段内，继续中...! \r\n\r\n"+LOG_Show_Old);
			UpdateData(true);  //把中间值赋给变量
			UpdateData(false); //把变量显示在界面
			m_Result.UpdateWindow();
		}
	}
	/////////////////////////检查号段/////////////////////////////










    //检查长度
    if(((IMEI1_Choose_M=="true") && (m_IMEI.GetLength() != IMEI_STR_LENGTH -1)) 
        ||((IMEI2_Choose_M=="true") && (m_IMEI2.GetLength() != IMEI_STR_LENGTH -1)) 
        ||((IMEI3_Choose_M=="true") && (m_IMEI3.GetLength() != IMEI_STR_LENGTH -1)) 
        ||((IMEI4_Choose_M=="true") && (m_IMEI4.GetLength() != IMEI_STR_LENGTH -1)))
    {
		PrintError("IMEI长度必须15位!");
		m_Result.GetWindowText(LOG_Show_Old);
		//m_Result.SetWindowText("IMEI长度必须15位!\r\n\r\n"+LOG_Show_Old);
		//m_Result.UpdateWindow();
		CheckIMEI_LOG="IMEI长度必须15位！！！！！！\r\n\r\n"+LOG_Show_Old;
		OnSelchangeAuto();
		IMEI_Control.SetFocus();
		IMEI_Control.SetSel(0,-1);
        return FALSE;   
    }  

    if((BT_MAC_ADDR_Choose_M=="true")&& (m_BT_Addr.GetLength()!= BT_ADDR_STR_LENGTH - 1))
    {
		PrintError("输入的 蓝牙地址长度不正确!");
		m_Write.EnableWindow(true);
		m_Result.GetWindowText(LOG_Show_Old);
		CheckIMEI_LOG="输入的 蓝牙地址长度不正确！！！！！！\r\n\r\n"+LOG_Show_Old;

		OnSelchangeAuto();
		IMEI_Control.SetFocus();
		IMEI_Control.SetSel(0,-1);
        return FALSE;   
    }
    //检查wifi
	 if((WIFI_MAC_ADDR_Choose_M=="true")&& (m_strWifi.GetLength()!= BT_ADDR_STR_LENGTH - 1))
    {
		PrintError("输入的 WIFI地址长度不正确!");
		m_Write.EnableWindow(true);
		m_Result.GetWindowText(LOG_Show_Old);
		CheckIMEI_LOG="输入的 WIFI地址长度不正确！！！！！！\r\n\r\n"+LOG_Show_Old;



		OnSelchangeAuto();
		IMEI_Control.SetFocus();
		IMEI_Control.SetSel(0,-1);
        return FALSE;   
    }
    /*if((WIFI_MAC_ADDR_Choose_M=="true"))
    {
        CString strTemp;
        strTemp= m_strWifi.Left(2);
        if(0!=_tcstol(strTemp, NULL,16)%2)
        {
             //m_strMsg.LoadString( "WIFI起始第一个字节必须偶数");
             return FALSE;   
        }
        strTemp= m_strWifi.Left(6);
        if(strTemp!=AutoWIFIAd_Start_Num_M)
        {
             //m_strMsg.LoadString( "WIFI前6位必须跟设置文件中设置ForeWIFI一致");
             return FALSE;  
        }
        
    }*/
    //检查合法性
    if(IMEI_CheckValid_Choose_M=="true")
	{
        if((IMEI1_Choose_M=="true"))//&& (AutoIMEI_Choose_M=="false"))
        {
            __int64 int64IMEI =  _ttoi64(m_IMEI);
            __int64 int64CD = ComputeCD(int64IMEI / 10);
            __int64 int64IMEICD = int64IMEI % 10;
            if(int64IMEICD != int64CD)
            {
                //m_strMsg.Format("输入的IMEI %d不合法!", 1);
				PrintError("输入的IMEI1不合法!");
				m_Write.EnableWindow(true);
				m_Result.GetWindowText(LOG_Show_Old);
				CheckIMEI_LOG="输入的IMEI1校验位不合法!\r\n\r\n"+LOG_Show_Old;

				OnSelchangeAuto();
				IMEI_Control.SetFocus();
				IMEI_Control.SetSel(0,-1);
                return FALSE;
            }
			else
			{
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("IMEI1校验位正常继续...\r\n\r\n"+LOG_Show_Old);
				m_Result.UpdateWindow();
			}
        }    
        if((IMEI2_Choose_M=="true"))// && (AutoIMEI_Choose_M=="false"))
        {
            __int64 int64IMEI =  _ttoi64(m_IMEI2);
            __int64 int64CD = ComputeCD(int64IMEI / 10);
            __int64 int64IMEICD = int64IMEI % 10;
            if(int64IMEICD != int64CD)
            {
                PrintError("输入的IMEI2不合法!");
				m_Write.EnableWindow(true);
				m_Result.GetWindowText(LOG_Show_Old);
				CheckIMEI_LOG="输入的IMEI2校验位不合法!\r\n\r\n"+LOG_Show_Old;
				OnSelchangeAuto();
				IMEI_Control.SetFocus();
				IMEI_Control.SetSel(0,-1);
                return FALSE;
            }
			else
			{
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("IMEI2校验位正常继续...\r\n\r\n"+LOG_Show_Old);
				m_Result.UpdateWindow();
			}
        } 
        if((IMEI3_Choose_M=="true"))// && (AutoIMEI_Choose_M=="false"))
        {
            __int64 int64IMEI =  _ttoi64(m_IMEI3);
            __int64 int64CD = ComputeCD(int64IMEI / 10);
            __int64 int64IMEICD = int64IMEI % 10;
            if(int64IMEICD != int64CD)
            {
                PrintError("输入的IMEI3不合法!");
				m_Write.EnableWindow(true);
				m_Result.GetWindowText(LOG_Show_Old);
				CheckIMEI_LOG="输入的IMEI3校验位不合法!\r\n\r\n"+LOG_Show_Old;

				OnSelchangeAuto();
				IMEI_Control.SetFocus();
				IMEI_Control.SetSel(0,-1);
                return FALSE;
            }
			else
			{
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("IMEI3校验位正常继续...\r\n\r\n"+LOG_Show_Old);
				m_Result.UpdateWindow();
			}
        } 
        if((IMEI4_Choose_M=="true"))// && (AutoIMEI_Choose_M=="false"))
        {
            __int64 int64IMEI =  _ttoi64(m_IMEI4);
            __int64 int64CD = ComputeCD(int64IMEI / 10);
            __int64 int64IMEICD = int64IMEI % 10;
            if(int64IMEICD != int64CD)
            {
                PrintError("输入的IMEI4不合法!");
				m_Write.EnableWindow(true);
				m_Result.GetWindowText(LOG_Show_Old);
				CheckIMEI_LOG="输入的IMEI4校验位不合法!\r\n\r\n"+LOG_Show_Old;

				OnSelchangeAuto();
				IMEI_Control.SetFocus();
				IMEI_Control.SetSel(0,-1);
                return FALSE;
            }
			else
			{
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("IMEI4校验位正常继续...\r\n\r\n"+LOG_Show_Old);
				m_Result.UpdateWindow();
			}
        } 
	}
	//连接数据库检查重复性
    if( (Check_repeat_Choose_M=="true")&&(Initial_Connect_DB_M=="true") )//(Initial_Connect_DB_M=="true")初始化连接了数据库才能检查重复性，权限限制
	{
		CString Conn="";
		CString DB_FAIL="";
		Conn.Format("driver={SQL Server};Server=%s;DATABASE=%s;UID=%s;PWD=%s", m_server_M,m_db_M,m_user_M, m_pwd_M);//||  ((ORACLE_Connect_Choose_M=="true")&&(UP_DB_Data_M=="true"))
		/*if(   (DB_CONNECT_M=="true")&&(IMEI_DB_Choose_M=="false")&&(MAC_DB_Choose_M=="false")  ||  (ORACLE_Connect_Choose_M=="true")&&(UP_DB_Data_M=="true")&&(IMEI_DB_Choose_M=="false")&&(MAC_DB_Choose_M=="false")   ) //每次操作数据库时候，重连一次,并且要除了数据库关联的情况（这时候已经连接一次）
			DB_FAIL=Setdlg.myado.ConnecDataLibrary(Conn,"","",adModeUnknown);//检查重复性，重连数据库
		else
			DB_FAIL="SUCCESS";
		*/
		if(Setdlg.myado.m_pCon==NULL)
		{
			DB_FAIL=Setdlg.myado.ConnecDataLibrary(Conn,"","",adModeUnknown);   
		}
		if(Setdlg.myado.m_pCon->State==0)  //1表示已经打开,0表示关闭，数据库意外断开，重连
			{
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("=================================数据库意外断开，重连........\r\n\r\n"+LOG_Show_Old);
				m_Result.UpdateWindow();
				DB_FAIL=Setdlg.myado.ConnecDataLibrary(Conn,"","",adModeUnknown);//数据库意外断开，重连
			}
			else
			{
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("=================================数据库连接正常中...\r\n\r\n"+LOG_Show_Old);
				m_Result.UpdateWindow();
			}

		if(DB_FAIL=="FAIL")
		{
			IMEI_Control.SetFocus();
			IMEI_Control.SetSel(0,-1);
			m_Result.GetWindowText(LOG_Show_Old);
			m_Result.SetWindowText("连接数据库失败，无法检查IMEI、BT_WIFI MAC的重复性，请检查网络.......\r\n\r\n"+LOG_Show_Old);
			return FALSE;
		}
		bool Repeat_Result_All=true;//显示所有的重复状态
		////////IMEI1/////////////////////	
		if(IMEI1_Choose_M=="true") 
		{
			Setdlg.myado.OpenSheet("select imei from dse_barcode_imei WHERE imei ='"+m_IMEI+"'");
			BOOL IMEI_Check=Setdlg.myado.Find("imei='"+m_IMEI+"'");
			Setdlg.myado.CloseSheet();
			if(IMEI_Check==TRUE)
			{
				if(JUST_UpDate_DB_Choose_M=="true")
				{
					m_Result.GetWindowText(LOG_Show_Old);  
					m_Result.SetWindowText("==================================IMEI:"+m_IMEI+"   数据库已经存在，后续更新网标...\r\n\r\n"+LOG_Show_Old);
					m_Result.UpdateWindow();
					IMEI1_Exist=true;
				}
				else
				{
					//PlaySound((LPCTSTR)IDR_WAVE_fail, AfxGetInstanceHandle(), SND_RESOURCE | SND_ASYNC);
					m_Result.GetWindowText(LOG_Show_Old);  
					m_Result.SetWindowText("=================================IMEI:"+m_IMEI+"   重复！！数据不上传，请检查后==重新写号！！！\r\n\r\n"+LOG_Show_Old);
					gColor = RGB(255,0,0);
					gFinalResult.Format("     Fail");
					RedrawWindow();  m_Result.UpdateWindow(); 
					IMEI_Control.SetFocus();
					IMEI_Control.SetSel(0,-1);
					Repeat_Result_All=false;//return FALSE;
				}

			}
			else
			{
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("==================================IMEI:"+m_IMEI+"   数据库正常\r\n\r\n"+LOG_Show_Old);
				//UpdateData(true);  //把中间值赋给变量
				//UpdateData(false); //把变量显示在界面
				m_Result.UpdateWindow();
				IMEI1_Exist=false;
			}
		}
		////////IMEI1/////////////////////		

		////////IMEI2/////////////////////	
		if(IMEI2_Choose_M=="true") 
		{
			Setdlg.myado.OpenSheet("select IMEI2 from dse_barcode_imei WHERE IMEI2 ='"+m_IMEI2+"'");
			BOOL IMEI_Check=Setdlg.myado.Find("IMEI2='"+m_IMEI2+"'");
			Setdlg.myado.CloseSheet();
			if(IMEI_Check==TRUE)
			{
				//PlaySound((LPCTSTR)IDR_WAVE_fail, AfxGetInstanceHandle(), SND_RESOURCE | SND_ASYNC);
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("=================================IMEI:"+m_IMEI2+"   重复！！数据不上传，请检查后==重新写号！！！\r\n\r\n"+LOG_Show_Old);
				gColor = RGB(255,0,0);
				gFinalResult.Format("     Fail");
				RedrawWindow();  m_Result.UpdateWindow(); 
				IMEI_Control.SetFocus();
				IMEI_Control.SetSel(0,-1);
				Repeat_Result_All=false;//return FALSE;

			}
			else
			{
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("==================================IMEI:"+m_IMEI2+"   数据库正常\r\n\r\n"+LOG_Show_Old);
				//UpdateData(true);  //把中间值赋给变量
				//UpdateData(false); //把变量显示在界面
				m_Result.UpdateWindow();
			}
		}
		////////IMEI2/////////////////////	

		////////IMEI3/////////////////////	
		if(IMEI3_Choose_M=="true") 
		{
			Setdlg.myado.OpenSheet("select IMEI3 from dse_barcode_imei WHERE IMEI3 ='"+m_IMEI3+"'");
			BOOL IMEI_Check=Setdlg.myado.Find("IMEI3='"+m_IMEI3+"'");
			Setdlg.myado.CloseSheet();
			if(IMEI_Check==TRUE)
			{
				//PlaySound((LPCTSTR)IDR_WAVE_fail, AfxGetInstanceHandle(), SND_RESOURCE | SND_ASYNC);
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("=================================IMEI:"+m_IMEI3+"   重复！！数据不上传，请检查后==重新写号！！！\r\n\r\n"+LOG_Show_Old);
				gColor = RGB(255,0,0);
				gFinalResult.Format("     Fail");
				RedrawWindow();  m_Result.UpdateWindow(); 
				IMEI_Control.SetFocus();
				IMEI_Control.SetSel(0,-1);
				Repeat_Result_All=false;//return FALSE;

			}
			else
			{
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("==================================IMEI:"+m_IMEI3+"   数据库正常\r\n\r\n"+LOG_Show_Old);
				//UpdateData(true);  //把中间值赋给变量
				//UpdateData(false); //把变量显示在界面
				m_Result.UpdateWindow();
			}
		}
		////////IMEI3/////////////////////	

		////////IMEI4/////////////////////	
		if(IMEI4_Choose_M=="true") 
		{
			Setdlg.myado.OpenSheet("select IMEI4 from dse_barcode_imei WHERE IMEI4 ='"+m_IMEI4+"'");
			BOOL IMEI_Check=Setdlg.myado.Find("IMEI4='"+m_IMEI4+"'");
			Setdlg.myado.CloseSheet();
			if(IMEI_Check==TRUE)
			{
				//PlaySound((LPCTSTR)IDR_WAVE_fail, AfxGetInstanceHandle(), SND_RESOURCE | SND_ASYNC);
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("=================================IMEI:"+m_IMEI4+"   重复！！数据不上传，请检查后==重新写号！！！\r\n\r\n"+LOG_Show_Old);
				gColor = RGB(255,0,0);
				gFinalResult.Format("     Fail");
				RedrawWindow();  m_Result.UpdateWindow(); 
				IMEI_Control.SetFocus();
				IMEI_Control.SetSel(0,-1);
				Repeat_Result_All=false;//return FALSE;

			}
			else
			{
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("==================================IMEI:"+m_IMEI4+"   数据库正常\r\n\r\n"+LOG_Show_Old);
				//UpdateData(true);  //把中间值赋给变量
				//UpdateData(false); //把变量显示在界面
				m_Result.UpdateWindow();
			}
		}
		////////IMEI4/////////////////////	

		////////BT_MAC/////////////////////	
		if(BT_MAC_ADDR_Choose_M=="true") 
		{
			Setdlg.myado.OpenSheet("select BLUE_MAC from dse_barcode_imei WHERE BLUE_MAC ='"+m_BT_Addr+"'");
			BOOL BT_MAC_Check=Setdlg.myado.Find("BLUE_MAC='"+m_BT_Addr+"'");
			Setdlg.myado.CloseSheet();
			if(BT_MAC_Check==TRUE)
			{
				//PlaySound((LPCTSTR)IDR_WAVE_fail, AfxGetInstanceHandle(), SND_RESOURCE | SND_ASYNC);
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("=================================BT_MAC:"+m_BT_Addr+"   重复！！数据不上传，请检查后==重新写号！！！\r\n\r\n"+LOG_Show_Old);
				gColor = RGB(255,0,0);
				gFinalResult.Format("     Fail");
				RedrawWindow();  m_Result.UpdateWindow(); 
				IMEI_Control.SetFocus();
				IMEI_Control.SetSel(0,-1);
				Repeat_Result_All=false;//return FALSE;

			}
			else
			{
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("==================================BT_MAC:"+m_BT_Addr+"   数据库正常\r\n\r\n"+LOG_Show_Old);
				//UpdateData(true);  //把中间值赋给变量
				//UpdateData(false); //把变量显示在界面
				m_Result.UpdateWindow();
			}
		}
		////////BT_MAC/////////////////////	

		////////WIFI_MAC/////////////////////	
		if(WIFI_MAC_ADDR_Choose_M=="true") 
		{
			Setdlg.myado.OpenSheet("select WIFI_MAC from dse_barcode_imei WHERE WIFI_MAC ='"+m_strWifi+"'");
			BOOL WIFI_MAC_Check=Setdlg.myado.Find("WIFI_MAC='"+m_strWifi+"'");
			Setdlg.myado.CloseSheet();
			if(WIFI_MAC_Check==TRUE)
			{
				//PlaySound((LPCTSTR)IDR_WAVE_fail, AfxGetInstanceHandle(), SND_RESOURCE | SND_ASYNC);
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("=================================WIFI_MAC:"+m_strWifi+"   重复！！数据不上传，请检查后==重新写号！！！\r\n\r\n"+LOG_Show_Old);
				gColor = RGB(255,0,0);
				gFinalResult.Format("     Fail");
				RedrawWindow();  m_Result.UpdateWindow(); 
				IMEI_Control.SetFocus();
				IMEI_Control.SetSel(0,-1);
				Repeat_Result_All=false;//return FALSE;

			}
			else
			{
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("==================================WIFI_MAC:"+m_strWifi+"   数据库正常\r\n\r\n"+LOG_Show_Old);
				//UpdateData(true);  //把中间值赋给变量
				//UpdateData(false); //把变量显示在界面
				m_Result.UpdateWindow();
			}
		}
		////////WIFI_MAC/////////////////////	

		////////网标对应的机型是否正常///
		if(Machine_Type_NC_CompareChoose_M=="true")
		{
			LPCTSTR  BeginNo="BeginNo";
			LPCTSTR  EndNo="EndNo";
			_variant_t var;
			//CString NetCodeStart_DB_M;//数据库中的网标起始号
			//CString NetCodeEnd_DB_M;
			CString NetCodeStart_DB_M[128]={""};//数据库中的网标起始号----多个号段
			CString NetCodeEnd_DB_M[128]={""};
			long NetCodeStart_Number[128]={0};
			long NetCodeEnd_Number[128]={0};
			int m=0;
			//必须先检查是否存在该机型
			/*
			SELECT IMEI_NAL_RECEIVE.BeginNo, IMEI_NAL_RECEIVE.EndNo, Pub_TelType.TelTypeFull \
FROM IMEI_NAL_RECEIVE INNER JOIN \
      Pub_TelType ON IMEI_NAL_RECEIVE.TelType_ID = Pub_TelType.TelType_ID \
WHERE (Pub_TelType.TelTypeFull = 'SC')"+Machine_Type_M+"
			*/
			//Setdlg.myado.OpenSheet("select TelTypeFull from Pub_TelType WHERE TelTypeFull ='"+Machine_Type_M+"'");//1
			Setdlg.myado.OpenSheet("SELECT Pub_TelType.TelTypeFull \
									FROM Pub_TelType INNER JOIN \
										  IMEI_NAL_RECEIVE ON IMEI_NAL_RECEIVE.TelType_ID = Pub_TelType.TelType_ID \
									WHERE (Pub_TelType.TelTypeFull = '"+Machine_Type_M+"')");
			BOOL TelType_ID_Check_Exist=Setdlg.myado.Find("TelTypeFull='"+Machine_Type_M+"'");                   //检查Machine_Type_M是否在数据库表IMEI_NAL_RECEIVE 中存在
			Setdlg.myado.CloseSheet();
			//必须先检查是否存在该机型
			bool UP_FAIL=false;
			//CString strSQL="select * FROM IMEI_NAL_RECEIVE WHERE (TelType_ID = '"+Machine_Type_M+"')";
			//CString strSQL="select * FROM IMEI_NAL_RECEIVE WHERE TelType_ID ='"+Machine_Type_M+"'";
			CString strSQL="SELECT IMEI_NAL_RECEIVE.BeginNo, IMEI_NAL_RECEIVE.EndNo, Pub_TelType.TelTypeFull \
									FROM IMEI_NAL_RECEIVE INNER JOIN \
										  Pub_TelType ON IMEI_NAL_RECEIVE.TelType_ID = Pub_TelType.TelType_ID \
									WHERE (Pub_TelType.TelTypeFull = '"+Machine_Type_M+"')";
			UP_FAIL=Setdlg.myado.Execute(strSQL);	
			if((UP_FAIL==true)&&(TelType_ID_Check_Exist==TRUE))
			{
				Setdlg.myado.m_pRec.CreateInstance(__uuidof(Recordset));//Setdlg.myado.m_pRec->get_RecordCount();Setdlg.myado.m_pRec->MoveNext();Setdlg.myado.m_pRec->GetadoEOF();
				Setdlg.myado.m_pRec->Open((_bstr_t)strSQL,Setdlg.myado.m_pCon.GetInterfacePtr(),adOpenDynamic,adLockOptimistic,adCmdText);
				/*
				var= Setdlg.myado.m_pRec->GetCollect(BeginNo);//var= Setdlg.myado.m_pRec->GetCollect(_variant_t((long)3));
				if(var.vt != VT_NULL)
					NetCodeStart_DB_M.Format("%s",(LPCSTR)(_bstr_t)var);
				else
				{	
					NetCodeStart_DB_M="";
				}

				var= Setdlg.myado.m_pRec->GetCollect(EndNo);//var= Setdlg.myado.m_pRec->GetCollect(_variant_t((long)3));
				if(var.vt != VT_NULL)
					NetCodeEnd_DB_M.Format("%s",(LPCSTR)(_bstr_t)var);
				else
				{	
					NetCodeEnd_DB_M="";
				}

				if((NetCodeStart_DB_M=="")||(NetCodeEnd_DB_M==""))
				{
					var.vt = VT_NULL;

					PrintError("数据库中表 Pub_TelType 机型"+Machine_Type_M+"对应的网标起始号OR结束号为空"+"起始号="+NetCodeStart_DB_M+";结束号="+NetCodeEnd_DB_M);
					m_Write.EnableWindow(true);
					NETCODE_Control.SetFocus();
					NETCODE_Control.SetSel(0,-1);	
					return FALSE;
				}*/
				//数据库中的网标----多个号段
				Setdlg.myado.m_pRec->MoveFirst();
				//for(int m=0;m<Setdlg.myado.m_pRec->get_RecordCount();m++)
				
				do
				{
					var= Setdlg.myado.m_pRec->GetCollect(BeginNo);
					if(var.vt != VT_NULL)
					{
						NetCodeStart_DB_M[m].Format("%s",(LPCSTR)(_bstr_t)var);
					}

					var= Setdlg.myado.m_pRec->GetCollect(EndNo);
					if(var.vt != VT_NULL)
					{
						NetCodeEnd_DB_M[m].Format("%s",(LPCSTR)(_bstr_t)var);
					}

					if((NetCodeStart_DB_M[m]!="")&&(NetCodeEnd_DB_M[m]!=""))
					{
						m++;
					}

					Setdlg.myado.m_pRec->MoveNext();

				}while(!(Setdlg.myado.m_pRec->adoEOF ));

				//UpdateData(false);//把变量显示在界面,这一步非常重要，  在执行UpdateData(TRUE); 之前
			}
			else
			{
				if(TelType_ID_Check_Exist!=TRUE)
				{
					m_Result.GetWindowText(LOG_Show_Old);
					m_Result.SetWindowText("数据库中表 Pub_TelType 不存在机型"+Machine_Type_M+", 或者该机型对应的机型ID：TelType_ID，不存在所以无法从数据库获取该机型的网标起始结束号段 \r\n\r\n"+LOG_Show_Old);
					m_Result.UpdateWindow();
				}
				else
				{
					m_Result.GetWindowText(LOG_Show_Old);
					m_Result.SetWindowText("连接数据库失败，无法从数据库获取机型："+Machine_Type_M+" \r\n\r\n"+LOG_Show_Old);
					m_Result.UpdateWindow();
				}
				var.vt = VT_NULL;

				PrintError("请检查数据库！！！");
				m_Write.EnableWindow(true);
				NETCODE_Control.SetFocus();
				NETCODE_Control.SetSel(0,-1);	
				return FALSE;
			}
			

			
			long NetCodeCurr_Number=0;
			//NetCodeStart_Number=atol(NetCodeStart_DB_M.Right(atoi(NetCode_SwiftNumber_M)));
			//NetCodeEnd_Number=atol(NetCodeEnd_DB_M.Right(atoi(NetCode_SwiftNumber_M)));
			//NetCodeCurr_Number=atol(NETCODE_Input.Right(atoi(NetCode_SwiftNumber_M)));
			for(int k=0;k<m;k++)
			{
				NetCodeStart_DB_M[k]=NetCodeStart_DB_M[k]+"000000000000000";
				NetCodeStart_DB_M[k]=NetCodeStart_DB_M[k].Left(atoi(NETCODE_Count_S));

				NetCodeEnd_DB_M[k]=NetCodeEnd_DB_M[k]+"000000000000000";
				NetCodeEnd_DB_M[k]=NetCodeEnd_DB_M[k].Left(atoi(NETCODE_Count_S));

				NetCodeStart_Number[k]=atol(NetCodeStart_DB_M[k].Mid(atoi(NetCode_SwiftNumber_M)-1));
				NetCodeEnd_Number[k]=atol(NetCodeEnd_DB_M[k].Mid(atoi(NetCode_SwiftNumber_M)-1));
			}

			NetCodeCurr_Number=atol(NETCODE_Input.Mid(atoi(NetCode_SwiftNumber_M)-1));
			bool NETCODE_OK=false;
			int  NETCODE_OK_Second=-1;
			CString Relative_Second="";
			for(int c=0;c<m;c++)
			{
				if((NetCodeCurr_Number<=NetCodeEnd_Number[c])&&(NetCodeCurr_Number>=NetCodeStart_Number[c]))
				{
					NETCODE_OK=true;
					NETCODE_OK_Second=c;
					break;
				}
				else
				{
					Relative_Second+="号段:"+NetCodeStart_DB_M[c]+"-->"+NetCodeEnd_DB_M[c];
				}
			}
			
			if(NETCODE_OK==false)
			{
				PrintError("Please check 网标. 超出了数据库中对应机型设定的范围！！"+Relative_Second);
				m_Write.EnableWindow(true);
				NETCODE_Control.SetFocus();
				NETCODE_Control.SetSel(0,-1);
				return FALSE;
			}
			else
			{
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText(" 网标在数据库中对应机型设定范围内，""起始号："+NetCodeStart_DB_M[NETCODE_OK_Second]+"结束号："+NetCodeEnd_DB_M[NETCODE_OK_Second]+"继续..\r\n\r\n"+LOG_Show_Old);
				m_Result.UpdateWindow();
			}
			
		}
		////////网标对应的机型是否正常///
		////////网标/////////////////////	
		if(NETCODE_Choose_M=="true") 
		{
			Setdlg.myado.OpenSheet("select NetCode from dse_barcode_imei WHERE NetCode ='"+NETCODE_Input+"'");
			BOOL NetCode_Check=Setdlg.myado.Find("NetCode='"+NETCODE_Input+"'");
			Setdlg.myado.CloseSheet();
			if(NetCode_Check==TRUE)
			{
				//PlaySound((LPCTSTR)IDR_WAVE_fail, AfxGetInstanceHandle(), SND_RESOURCE | SND_ASYNC);
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("=================================网标:"+NETCODE_Input+"   重复！！数据不上传，请检查后==重新写号！！！\r\n\r\n"+LOG_Show_Old);
				gColor = RGB(255,0,0);
				gFinalResult.Format("     Fail");
				RedrawWindow();  m_Result.UpdateWindow(); 
				IMEI_Control.SetFocus();
				IMEI_Control.SetSel(0,-1);
				Repeat_Result_All=false;//return FALSE;

			}
			else
			{
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("==================================网标:"+NETCODE_Input+"   数据库正常\r\n\r\n"+LOG_Show_Old);
				//UpdateData(true);  //把中间值赋给变量
				//UpdateData(false); //把变量显示在界面
				m_Result.UpdateWindow();
			}
		}
		////////网标/////////////////////	
		
		if(Repeat_Result_All==false)
			return FALSE;
		UpdateData(true);  //把中间值赋给变量
		UpdateData(false); //把变量显示在界面
		m_Result.UpdateWindow();
	}
    return TRUE;
}

DWORD CWriteIMEIDlg::EnterModeFunc(WPARAM, LPARAM)
{
	extern COLORREF gColor;
	extern CString gFinalResult;
    HANDLE hHandle[]={m_hExitThread,m_hEnterModeSuccess};
    DWORD dwRet =0;
	//int m_nTimeout =30000;
    //dwRet = WaitForMultipleObjects(2,hHandle,FALSE,m_nTimeout);//
	dwRet = WaitForMultipleObjects(2,hHandle,FALSE,atol(WatchDog_Time_M));
    if(WAIT_TIMEOUT == dwRet)
    {
		ResetEvent (m_hEnterModeSuccess);
		ResetEvent(m_hExitThread);
        SP_StopModeProcess(g_hDiagPhone);//超时处理
		/*EnableWindow(TRUE);
		m_Result.GetWindowText(LOG_Show_Old);  
		m_Result.SetWindowText("操作超时！！！\r\n\r\n"+LOG_Show_Old);
		gColor = RGB(255,0,0);
		gFinalResult.Format("     Fail");
		RedrawWindow();  m_Result.UpdateWindow(); 
		IMEI_Control.SetFocus();
		IMEI_Control.SetSel(0,-1);
        return -1;*/
		OnBnClickedButton5();//停止操作手机
    }
    else if(0 == dwRet- WAIT_OBJECT_0)
    {
        ResetEvent(m_hExitThread);
        SP_StopModeProcess(g_hDiagPhone);  //终止测试
    }         
    else if (1 == dwRet- WAIT_OBJECT_0)
	{ 
        m_dwStartTime = ::GetTickCount();
        ResetEvent(m_hEnterModeSuccess);
        //GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(FALSE); 
        if(m_bUsbMode=="USB")
        {
            /*m_nComPort = SP_GetUsbPort(g_hDiagPhone);
            if(RFT_SUCCESS!=SP_BeginPhoneTest(g_hDiagPhone, &ca))//USB方式获取虚拟串口开始测试
            {
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("SP_BeginPhoneTest失败！！！\r\n\r\n"+LOG_Show_Old);
				gColor = RGB(255,0,0);
				gFinalResult.Format("     Fail");
				RedrawWindow();  m_Result.UpdateWindow(); 
				IMEI_Control.SetFocus();
				IMEI_Control.SetSel(0,-1);
				return -1;
            }*/
        }  
        else
        {
			m_dwEnterModeWait=1000;
            Sleep(m_dwEnterModeWait);    //串口需要waiting 一定时间才进模式
        }
        //m_strMsg.LoadString("手机连接成功! 正在操作手机，请不要断电...");       //connect success
		m_Result.GetWindowText(LOG_Show_Old);  
		m_Result.SetWindowText("手机连接成功! 正在操作手机，请不要断电...\r\n\r\n"+LOG_Show_Old);
		m_Result.UpdateWindow();
		//autow = m_Auto.GetCurSel();
		CString autow_CS;
		m_Auto.GetWindowTextA(autow_CS);
        if(autow_CS=="读取检查")
        {
            ReadBack();
        }
        else if(autow_CS=="写号")
        {  
            WriteFun(); 
        }  
    } 
    SP_EndPhoneTest(g_hDiagPhone);
	
	RedrawWindow();  m_Result.UpdateWindow(); 
	IMEI_Control.SetFocus();// 自动生成号段时，自动循环开始写号--->不自动循环了
	IMEI_Control.SetSel(0,-1);
	//初始化时连接了数据库、每次重连数据库、
	/*if(DB_CONNECT_M=="true")//数据库关闭处理
	{
		Setdlg.myado.CloseDataLibrary();//连接关闭必须在一个子函数内，不然出错
	}*/
	LOG_Show_Old="";
    return 0;  
}
void CWriteIMEIDlg::ReadBack()
{
	extern COLORREF gColor;
	extern CString gFinalResult;
    USES_CONVERSION;//为A2W使用做准备，new delete 
    CString strTemp;
    int nErrorCode= RFT_SUCCESS;


    char  szImei1[IMEI_STR_LENGTH] = {0};//缓存MAC
    char  szImei2[IMEI_STR_LENGTH] = {0};
    char  szImei3[IMEI_STR_LENGTH] = {0};   
    char  szImei4[IMEI_STR_LENGTH] = {0};
    char  szBt[BT_ADDR_STR_LENGTH] = {0};
    char  szWifi[BT_ADDR_STR_LENGTH] = {0}; 
	CString m_IMEI_Read;
	CString m_IMEI2_Read;
	CString m_IMEI3_Read;
	CString m_IMEI4_Read;
	CString m_BT_Addr_Read;
	CString m_strWifi_Read;
	bool m_IMEI_Result=true;
	bool m_IMEI2_Result=true;
	bool m_IMEI3_Result=true;
	bool m_IMEI4_Result=true;

	bool m_BT_Addr_Result=true;
	bool m_strWifi_Result=true;

	bool Version_Result=true;

	/////////////////////////////
	FASTNVACCESS_T NvWriteParam;
	ZeroMemory(&NvWriteParam, sizeof(FASTNVACCESS_T));
	if(IMEI1_Choose_M=="true")
	{
		NvWriteParam.dwOperMask |=FNAMASK_WRITE_IMEI1;
	}
	if(IMEI2_Choose_M=="true")
	{
		NvWriteParam.dwOperMask |=FNAMASK_WRITE_IMEI2;
	}
	if(IMEI3_Choose_M=="true")
	{
		NvWriteParam.dwOperMask |=FNAMASK_WRITE_IMEI3;
	}
	if((IMEI4_Choose_M=="true"))
	{
		NvWriteParam.dwOperMask |=FNAMASK_WRITE_IMEI4;
	}
	if(BT_MAC_ADDR_Choose_M=="true")
	{
		NvWriteParam.dwOperMask |= FNAMASK_WRITE_BTADDR;
	}
	if(WIFI_MAC_ADDR_Choose_M=="true")
	{
		NvWriteParam.dwOperMask |= FNAMASK_WRITE_WIFIADDR;
	}
	for(int icount = 0; icount < 3;icount ++)
	{
		nErrorCode = SP_FastReadNvInfo(g_hDiagPhone,&NvWriteParam);
		Sleep(100);
		if(nErrorCode == RFT_SUCCESS)
		{
			break;
		}
	}
	/////////////////////////////
    if(RFT_SUCCESS==nErrorCode)
    {
        /*strcpy(szImei1,NvWriteParam.szImei1);
        strcpy(szImei2,NvWriteParam.szImei2);
		strcpy(szImei3,NvWriteParam.szImei3);
        strcpy(szImei4,NvWriteParam.szImei4);
        strcpy(szBt,NvWriteParam.szBTAddr);
        strcpy(szWifi,NvWriteParam.szWIFIAddr);*/
    }
	else
	{
		m_Result.GetWindowText(LOG_Show_Old);  
		m_Result.SetWindowText("读取IMEI/蓝牙地址/WIFI地址失败！！！\r\n\r\n"+LOG_Show_Old);
		gColor = RGB(255,0,0);
		gFinalResult.Format("     Fail");
		RedrawWindow();  m_Result.UpdateWindow(); 
		IMEI_Control.SetFocus();
		IMEI_Control.SetSel(0,-1);
	}
    if(RFT_SUCCESS ==nErrorCode)
    {
        if((IMEI1_Choose_M=="true"))
        {     
            //m_strEditIMEI1= A2W(szImei1); 
			m_IMEI_Read.Format("%s",NvWriteParam.szImei1);
			if(IMEI_MAC_Check_Choose_M=="false")
			{
				m_IMEI=m_IMEI_Read;//无效，无法更新显示
			}
			else
			{
				if(m_IMEI==m_IMEI_Read)
				{
					m_IMEI_Result=true;
				}
				else
				{
					m_IMEI_Result=false;
				}
			}
        }
        if((IMEI2_Choose_M=="true"))
        {
			m_IMEI2_Read.Format("%s",NvWriteParam.szImei2);
			if(IMEI_MAC_Check_Choose_M=="false")
			{
				m_IMEI2=m_IMEI2_Read;//无效，无法更新显示
			}
			else
			{
				for(int i = 0; i < m_IMEI2.GetLength(); i ++)
				{
					if(m_IMEI2.GetAt(i) < '0' || m_IMEI2.GetAt(i) > '9')
					{
						m_IMEI2=("");
						m_Result.GetWindowText(LOG_Show_Old);  
						m_Result.SetWindowText("IMEI2 not exist！！！\r\n\r\n"+LOG_Show_Old);
						gColor = RGB(255,0,0);
						gFinalResult.Format("     Fail");
						RedrawWindow();  m_Result.UpdateWindow(); 
						IMEI_Control.SetFocus();
						IMEI_Control.SetSel(0,-1);
						return;
					}
				}
				if(m_IMEI2==m_IMEI2_Read)
				{
					m_IMEI2_Result=true;
				}
				else
				{
					m_IMEI2_Result=false;
				}
			}
        }
        if((IMEI3_Choose_M=="true"))
        {
			m_IMEI3_Read.Format("%s",NvWriteParam.szImei3);
			if(IMEI_MAC_Check_Choose_M=="false")
			{
				m_IMEI3=m_IMEI3_Read;//无效，无法更新显示
			}
			else
			{
				for(int i = 0; i < m_IMEI3.GetLength(); i ++)
				{
					if(m_IMEI3.GetAt(i) < '0' || m_IMEI3.GetAt(i) > '9')
					{
						m_IMEI3=("");
						m_Result.GetWindowText(LOG_Show_Old);  
						m_Result.SetWindowText("IMEI3 not exist！！！\r\n\r\n"+LOG_Show_Old);
						gColor = RGB(255,0,0);
						gFinalResult.Format("     Fail");
						RedrawWindow();  m_Result.UpdateWindow(); 
						IMEI_Control.SetFocus();
						IMEI_Control.SetSel(0,-1);
						return;
					}
				}
				if(m_IMEI3==m_IMEI3_Read)
				{
					m_IMEI3_Result=true;
				}
				else
				{
					m_IMEI3_Result=false;
				}
			}
        }
        if((IMEI4_Choose_M=="true"))
        {
			m_IMEI4_Read.Format("%s",NvWriteParam.szImei4);
			if(IMEI_MAC_Check_Choose_M=="false")
			{
				m_IMEI4=m_IMEI4_Read;//无效，无法更新显示
			}
			else
			{
				for(int i = 0; i < m_IMEI4.GetLength(); i ++)
				{
					if(m_IMEI4.GetAt(i) < '0' || m_IMEI4.GetAt(i) > '9')
					{
						m_IMEI4=("");
						m_Result.GetWindowText(LOG_Show_Old);  
						m_Result.SetWindowText("IMEI4 not exist！！！\r\n\r\n"+LOG_Show_Old);
						gColor = RGB(255,0,0);
						gFinalResult.Format("     Fail");
						RedrawWindow();  m_Result.UpdateWindow(); 
						IMEI_Control.SetFocus();
						IMEI_Control.SetSel(0,-1);
						return;
					}
				}
				if(m_IMEI4==m_IMEI4_Read)
				{
					m_IMEI4_Result=true;
				}
				else
				{
					m_IMEI4_Result=false;
				}
			}
        }
        if(BT_MAC_ADDR_Choose_M=="true")
        {
			m_BT_Addr_Read.Format("%s",NvWriteParam.szBTAddr);
			if(IMEI_MAC_Check_Choose_M=="false")
			{
				m_BT_Addr=m_BT_Addr_Read;//无效，无法更新显示
			}
			else
			{
				if(m_BT_Addr==m_BT_Addr_Read)
				{
					m_BT_Addr_Result=true;
				}
				else
				{
					m_BT_Addr_Result=false;
				}
			}
        }
        if(WIFI_MAC_ADDR_Choose_M=="true")
        {
			m_strWifi_Read.Format("%s",NvWriteParam.szWIFIAddr);
			if(IMEI_MAC_Check_Choose_M=="false")
			{
				m_strWifi=m_strWifi_Read;//无效，无法更新显示
			}
			else
			{
				if(m_strWifi==m_strWifi_Read)
				{
					m_strWifi_Result=true;
				}
				else
				{
					m_strWifi_Result=false;
				}
			}
        }
		
		if((Software_Version_Check_Choose_M=="true"))   //check version  Software_Version_M
		{ 
		   if(!CheckVersion())
		   {
			   Version_Result=false;
		   }
		}

		m_Result.GetWindowText(LOG_Show_Old);  
		m_Result.SetWindowText("\r\n读手机成功！\r\n\r\n"+LOG_Show_Old);
		/*if(IMEI_MAC_Check_Choose_M=="false")//会自动循环开始
		{
			gColor = RGB(0,255,0);
			gFinalResult.Format("     OK");
			m_Result.UpdateWindow();
			IMEI_Control.SetWindowTextA(m_IMEI_Read);
			IMEI2_Control.SetWindowTextA(m_IMEI2_Read);
			IMEI3_Control.SetWindowTextA(m_IMEI3_Read);
			IMEI4_Control.SetWindowTextA(m_IMEI4_Read);
			m_BT_Addr_Control.SetWindowTextA(m_BT_Addr_Read);
			m_WIFI_Addr_Control.SetWindowTextA(m_strWifi_Read);
			RedrawWindow();  m_Result.UpdateWindow(); 

			//OnSelchangeAuto();    //不将IMEI1清零
			IMEI_Control.SetFocus();
			IMEI_Control.SetSel(0,-1);
			return;
		}*/
		if( (m_IMEI_Result==true)&&(m_IMEI2_Result==true)&&(m_IMEI3_Result==true)&&(m_IMEI4_Result==true)&&(m_BT_Addr_Result==true)&&(m_strWifi_Result==true)&&( Version_Result==true) )
		{
			m_Result.GetWindowText(LOG_Show_Old);  
			m_Result.SetWindowText("参数检查成功！\r\n\r\nIMEI1:"+m_IMEI_Read+"\r\nIMEI2:"+m_IMEI2_Read+"\r\nIMEI3:"+m_IMEI3_Read+"\r\nIMEI4:"+m_IMEI4_Read+"\r\nBTMAC:"+m_BT_Addr+"\r\nWIMAC:"+m_strWifi+"\r\nSoft_version:"+Software_Version_M+LOG_Show_Old);
			gColor = RGB(0,255,0);
			gFinalResult.Format("     OK");

			//OnSelchangeAuto();    //不将IMEI1清零
			IMEI_Control.SetFocus();
			IMEI_Control.SetSel(0,-1);
		}
		else
		{
			if(m_IMEI_Result==false)
			{
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("IMEI1检查不正确！！！\r\n手机内为："+m_IMEI_Read+"\r\n输入的为："+m_IMEI+"\r\n\r\n"+LOG_Show_Old);
			}
			else
			{
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("IMEI1检查正确！！！\r\n手机内为："+m_IMEI_Read+"\r\n输入的为："+m_IMEI+"\r\n\r\n"+LOG_Show_Old);
			}
			//////////////////////
			if(m_IMEI2_Result==false)
			{
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("IMEI2检查不正确！！！\r\n手机内为："+m_IMEI2_Read+"\r\n输入的为："+m_IMEI2+"\r\n\r\n"+LOG_Show_Old);
			}
			else
			{
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("IMEI2检查正确！！！\r\n手机内为："+m_IMEI2_Read+"\r\n输入的为："+m_IMEI2+"\r\n\r\n"+LOG_Show_Old);
			}
			//////////////////////
			if(m_IMEI3_Result==false)
			{
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("IMEI3检查不正确！！！\r\n手机内为："+m_IMEI3_Read+"\r\n输入的为："+m_IMEI3+"\r\n\r\n"+LOG_Show_Old);
			}
			else
			{
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("IMEI3检查正确！！！\r\n手机内为："+m_IMEI3_Read+"\r\n输入的为："+m_IMEI3+"\r\n\r\n"+LOG_Show_Old);
			}
			if(m_IMEI4_Result==false)
			{
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("IMEI4检查不正确！！！\r\n手机内为："+m_IMEI4_Read+"\r\n输入的为："+m_IMEI4+"\r\n\r\n"+LOG_Show_Old);
			}
			else
			{
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("IMEI4检查正确！！！\r\n手机内为："+m_IMEI4_Read+"\r\n输入的为："+m_IMEI4+"\r\n\r\n"+LOG_Show_Old);
			}
			if(m_BT_Addr_Result==false)
			{
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("BT_MAC检查不正确！！！\r\n手机内为："+m_BT_Addr_Read+"\r\n输入的为："+m_BT_Addr+"\r\n\r\n"+LOG_Show_Old);
			} 
			else
			{
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("BT_MAC检查正确！！！\r\n手机内为："+m_BT_Addr_Read+"\r\n输入的为："+m_BT_Addr+"\r\n\r\n"+LOG_Show_Old);
			} 
			if(m_strWifi_Result==false)
			{
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("WIFI_MAC检查不正确！！！\r\n手机内为："+m_strWifi_Read+"\r\n输入的为："+m_strWifi+"\r\n\r\n"+LOG_Show_Old);
			}
			else
			{
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("WIFI_MAC检查正确！！！\r\n手机内为："+m_strWifi_Read+"\r\n输入的为："+m_strWifi+"\r\n\r\n"+LOG_Show_Old);
			}
			if(Version_Result==false)
			{
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("软件版本检查不正确！！！分析如下：\r\n"+Version_SoftWare+LOG_Show_Old);
			}
			else 
			{
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("软件版本检查正确！！！分析如下：\r\n"+Version_SoftWare+LOG_Show_Old);
			}
			gColor = RGB(255,0,0);
			gFinalResult.Format("     Fail");

		}
		m_Result.UpdateWindow();
		RedrawWindow();  m_Result.UpdateWindow(); 
		IMEI_Control.SetFocus();
		IMEI_Control.SetSel(0,-1);
    }
    else
    {
		m_Result.GetWindowText(LOG_Show_Old);  
		m_Result.SetWindowText("读手机失败！！！\r\n\r\n"+LOG_Show_Old);
		gColor = RGB(255,0,0);
		gFinalResult.Format("     Fail");
		RedrawWindow();  m_Result.UpdateWindow(); 
		IMEI_Control.SetFocus();
		IMEI_Control.SetSel(0,-1);
		return;
    }
	//m_Result.SetWindowText("");
}

bool CWriteIMEIDlg::WriteAndThenReadBack()
{
	extern COLORREF gColor;
	extern CString gFinalResult;
    USES_CONVERSION;//为A2W使用做准备，new delete 
    CString strTemp;
    int nErrorCode= RFT_SUCCESS;


    char  szImei1[IMEI_STR_LENGTH] = {0};//缓存MAC
    char  szImei2[IMEI_STR_LENGTH] = {0};
    char  szImei3[IMEI_STR_LENGTH] = {0};   
    char  szImei4[IMEI_STR_LENGTH] = {0};
    char  szBt[BT_ADDR_STR_LENGTH] = {0};
    char  szWifi[BT_ADDR_STR_LENGTH] = {0}; 
	CString m_IMEI_Read;
	CString m_IMEI2_Read;
	CString m_IMEI3_Read;
	CString m_IMEI4_Read;
	CString m_BT_Addr_Read;
	CString m_strWifi_Read;
	bool m_IMEI_Result=true;
	bool m_IMEI2_Result=true;
	bool m_IMEI3_Result=true;
	bool m_IMEI4_Result=true;

	bool m_BT_Addr_Result=true;
	bool m_strWifi_Result=true;

	bool BARCODE_Input_Result=true;

	bool Version_Result=true;

	/////////////////////////////
	FASTNVACCESS_T NvWriteParam;
	ZeroMemory(&NvWriteParam, sizeof(FASTNVACCESS_T));
	if(IMEI1_Choose_M=="true")
	{
		NvWriteParam.dwOperMask |=FNAMASK_WRITE_IMEI1;
	}
	if(IMEI2_Choose_M=="true")
	{
		NvWriteParam.dwOperMask |=FNAMASK_WRITE_IMEI2;
	}
	if(IMEI3_Choose_M=="true")
	{
		NvWriteParam.dwOperMask |=FNAMASK_WRITE_IMEI3;
	}
	if((IMEI4_Choose_M=="true"))
	{
		NvWriteParam.dwOperMask |=FNAMASK_WRITE_IMEI4;
	}
	if(BT_MAC_ADDR_Choose_M=="true")
	{
		NvWriteParam.dwOperMask |= FNAMASK_WRITE_BTADDR;
	}
	if(WIFI_MAC_ADDR_Choose_M=="true")
	{
		NvWriteParam.dwOperMask |= FNAMASK_WRITE_WIFIADDR;
	}
	for(int icount = 0; icount < 3;icount ++)
	{
		nErrorCode = SP_FastReadNvInfo(g_hDiagPhone,&NvWriteParam);
		Sleep(100);
		if(nErrorCode == RFT_SUCCESS)
		{
			break;
		}
	}
	/////////////////////////////
    if(RFT_SUCCESS==nErrorCode)
    {
        /*strcpy(szImei1,NvWriteParam.szImei1);
        strcpy(szImei2,NvWriteParam.szImei2);
		strcpy(szImei3,NvWriteParam.szImei3);
        strcpy(szImei4,NvWriteParam.szImei4);
        strcpy(szBt,NvWriteParam.szBTAddr);
        strcpy(szWifi,NvWriteParam.szWIFIAddr);*/
    }
	else
	{
		m_Result.GetWindowText(LOG_Show_Old);  
		m_Result.SetWindowText("读取IMEI/蓝牙地址/WIFI地址失败！！！\r\n\r\n"+LOG_Show_Old);
		gColor = RGB(255,0,0);
		gFinalResult.Format("     Fail");
		RedrawWindow();  m_Result.UpdateWindow(); 
		IMEI_Control.SetFocus();
		IMEI_Control.SetSel(0,-1);
		return false;
	}
    if(RFT_SUCCESS ==nErrorCode)
    {
        if((IMEI1_Choose_M=="true"))
        {     
            //m_strEditIMEI1= A2W(szImei1); 
			m_IMEI_Read.Format("%s",NvWriteParam.szImei1);
			if(m_IMEI==m_IMEI_Read)
			{
				m_IMEI_Result=true;
			}
			else
			{
				m_IMEI_Result=false;
			}
        }
        if((IMEI2_Choose_M=="true"))
        {
			m_IMEI2_Read.Format("%s",NvWriteParam.szImei2);
			for(int i = 0; i < m_IMEI2.GetLength(); i ++)
            {
                if(m_IMEI2.GetAt(i) < '0' || m_IMEI2.GetAt(i) > '9')
                {
                    m_IMEI2=("");
                    m_Result.GetWindowText(LOG_Show_Old);  
					m_Result.SetWindowText("IMEI2 not exist！！！\r\n\r\n"+LOG_Show_Old);
					gColor = RGB(255,0,0);
					gFinalResult.Format("     Fail");
					RedrawWindow();  m_Result.UpdateWindow(); 
					IMEI_Control.SetFocus();
					IMEI_Control.SetSel(0,-1);
					return false;
                }
            }
			if(m_IMEI2==m_IMEI2_Read)
			{
				m_IMEI2_Result=true;
			}
			else
			{
				m_IMEI2_Result=false;
			}
        }
        if((IMEI3_Choose_M=="true"))
        {
			m_IMEI3_Read.Format("%s",NvWriteParam.szImei3);
            for(int i = 0; i < m_IMEI3.GetLength(); i ++)
            {
                if(m_IMEI3.GetAt(i) < '0' || m_IMEI3.GetAt(i) > '9')
                {
                    m_IMEI3=("");
                    m_Result.GetWindowText(LOG_Show_Old);  
					m_Result.SetWindowText("IMEI3 not exist！！！\r\n\r\n"+LOG_Show_Old);
					gColor = RGB(255,0,0);
					gFinalResult.Format("     Fail");
					RedrawWindow();  m_Result.UpdateWindow(); 
					IMEI_Control.SetFocus();
					IMEI_Control.SetSel(0,-1);
					return false;
                }
            }
			if(m_IMEI3==m_IMEI3_Read)
			{
				m_IMEI3_Result=true;
			}
			else
			{
				m_IMEI3_Result=false;
			}
        }
        if((IMEI4_Choose_M=="true"))
        {
			m_IMEI4_Read.Format("%s",NvWriteParam.szImei4);
            for(int i = 0; i < m_IMEI4.GetLength(); i ++)
            {
                if(m_IMEI4.GetAt(i) < '0' || m_IMEI4.GetAt(i) > '9')
                {
                    m_IMEI4=("");
                    m_Result.GetWindowText(LOG_Show_Old);  
					m_Result.SetWindowText("IMEI4 not exist！！！\r\n\r\n"+LOG_Show_Old);
					gColor = RGB(255,0,0);
					gFinalResult.Format("     Fail");
					RedrawWindow();  m_Result.UpdateWindow(); 
					IMEI_Control.SetFocus();
					IMEI_Control.SetSel(0,-1);
					return false;
                }
            }
			if(m_IMEI4==m_IMEI4_Read)
			{
				m_IMEI4_Result=true;
			}
			else
			{
				m_IMEI4_Result=false;
			}
        }
        if(BT_MAC_ADDR_Choose_M=="true")
        {
			m_BT_Addr_Read.Format("%s",NvWriteParam.szBTAddr);
			if(m_BT_Addr==m_BT_Addr_Read)
			{
				m_BT_Addr_Result=true;
			}
			else
			{
				m_BT_Addr_Result=true;//展讯8810平台;wifi/btMAC写入后重启才生效（META方式）
			}
        }
        if(WIFI_MAC_ADDR_Choose_M=="true")
        {
			m_strWifi_Read.Format("%s",NvWriteParam.szWIFIAddr);

			if(m_strWifi==m_strWifi_Read)
			{
				m_strWifi_Result=true;
			}
			else
			{
				m_strWifi_Result=true;//展讯8810平台;wifi/btMAC写入后重启才生效（META方式）
			}
        }
		
		if((Software_Version_Check_Choose_M=="true"))   //check version  Software_Version_M
		{ 
		   if(!CheckVersion())
		   {
			   Version_Result=false;
		   }
		}
		if(BARCODE_Choose_M=="true")
		{
			BYTE SN[30]={0};
			for(int icount = 0; icount < 3;icount ++)
			{
				nErrorCode = SP_ReadSN(g_hDiagPhone,true,(void*)SN,sizeof(SN));
				Sleep(100);
				if(nErrorCode == RFT_SUCCESS)
				{
					break;
				}
			}

			if(nErrorCode == RFT_SUCCESS)
			{
				BARCODE_Input.Format("%s",SN);
				BARCODE_Input=BARCODE_Input.Left(15);//限制板号的长度
				BARCODE_Input_Control.SetWindowTextA(BARCODE_Input);;
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("\r\n读手机板号成功！\r\n\r\n板号为:"+BARCODE_Input+"\r\n"+LOG_Show_Old);
				if(Check_repeat_Choose_M=="true")
				{
					/////////连接数据库检查板号是否重复///////////////////////////////	
					Setdlg.myado.OpenSheet("select barcode from dse_barcode_imei WHERE barcode ='"+BARCODE_Input+"'");
					BOOL IMEI_Check=Setdlg.myado.Find("barcode='"+BARCODE_Input+"'");
					Setdlg.myado.CloseSheet();
					if(IMEI_Check==TRUE)
					{
						//PlaySound((LPCTSTR)IDR_WAVE_fail, AfxGetInstanceHandle(), SND_RESOURCE | SND_ASYNC);
						m_Result.GetWindowText(LOG_Show_Old);  
						m_Result.SetWindowText("=================================板号:"+BARCODE_Input+"   重复！！数据不上传，请检查后==重新写号！！！\r\n\r\n"+LOG_Show_Old);
						gColor = RGB(255,0,0);
						gFinalResult.Format("     Fail");
						RedrawWindow();  m_Result.UpdateWindow(); 
						IMEI_Control.SetFocus();
						IMEI_Control.SetSel(0,-1);
						BARCODE_Input_Result=false;//return FALSE;

					}
					else
					{
						m_Result.GetWindowText(LOG_Show_Old);  
						m_Result.SetWindowText("==================================板号:"+BARCODE_Input+"   数据库正常\r\n\r\n"+LOG_Show_Old);
						//UpdateData(true);  //把中间值赋给变量
						//UpdateData(false); //把变量显示在界面
						//m_Result.UpdateWindow();
					}
					 
					/////////////////////////////////////////
				}
			}
			else
			{
				BARCODE_Input_Result=false;
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("\r\n读手机板号失败！！！\r\n\r\n"+LOG_Show_Old);
				m_Result.UpdateWindow();
			}
			
		}

		m_Result.GetWindowText(LOG_Show_Old);  
		m_Result.SetWindowText("\r\n读手机成功！\r\n\r\n"+LOG_Show_Old);
		
		if( (m_IMEI_Result==true)&&(m_IMEI2_Result==true)&&(m_IMEI3_Result==true)&&(m_IMEI4_Result==true)&&(m_BT_Addr_Result==true)&&(m_strWifi_Result==true)&&( Version_Result==true)&&(BARCODE_Input_Result==true) )
		{
			m_Result.GetWindowText(LOG_Show_Old);  
			m_Result.SetWindowText("参数检查成功！从手机中读取的数据如下：\r\n\r\nIMEI1:"+m_IMEI_Read+"\r\nIMEI2:"+m_IMEI2_Read+"\r\nIMEI3:"+m_IMEI3_Read+"\r\nIMEI4:"+m_IMEI4_Read+"\r\nBTMAC:"+m_BT_Addr+"\r\nWIMAC:"+m_strWifi+"\r\nSoft_version:"+Software_Version_M+LOG_Show_Old);
			m_Result.UpdateWindow();
			gColor = RGB(0,255,0);
			gFinalResult.Format("     OK");
			RedrawWindow();  m_Result.UpdateWindow(); 
			//OnSelchangeAuto();    //不将IMEI1清零
			//IMEI_Control.SetFocus();// 自动生成号段时，自动循环开始写号--->导致异常，放在后面执行
			//IMEI_Control.SetSel(0,-1);
			return true;
		}
		else
		{
			if(m_IMEI_Result==false)
			{
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("IMEI1检查不正确！！！\r\n手机内为："+m_IMEI_Read+"\r\n输入的为："+m_IMEI+"\r\n\r\n"+LOG_Show_Old);
			}
			else
			{
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("IMEI1检查正确！！！\r\n手机内为："+m_IMEI_Read+"\r\n输入的为："+m_IMEI+"\r\n\r\n"+LOG_Show_Old);
			}
			//////////////////////
			if(m_IMEI2_Result==false)
			{
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("IMEI2检查不正确！！！\r\n手机内为："+m_IMEI2_Read+"\r\n输入的为："+m_IMEI2+"\r\n\r\n"+LOG_Show_Old);
			}
			else
			{
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("IMEI2检查正确！！！\r\n手机内为："+m_IMEI2_Read+"\r\n输入的为："+m_IMEI2+"\r\n\r\n"+LOG_Show_Old);
			}
			//////////////////////
			if(m_IMEI3_Result==false)
			{
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("IMEI3检查不正确！！！\r\n手机内为："+m_IMEI3_Read+"\r\n输入的为："+m_IMEI3+"\r\n\r\n"+LOG_Show_Old);
			}
			else
			{
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("IMEI3检查正确！！！\r\n手机内为："+m_IMEI3_Read+"\r\n输入的为："+m_IMEI3+"\r\n\r\n"+LOG_Show_Old);
			}
			if(m_IMEI4_Result==false)
			{
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("IMEI4检查不正确！！！\r\n手机内为："+m_IMEI4_Read+"\r\n输入的为："+m_IMEI4+"\r\n\r\n"+LOG_Show_Old);
			}
			else
			{
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("IMEI4检查正确！！！\r\n手机内为："+m_IMEI4_Read+"\r\n输入的为："+m_IMEI4+"\r\n\r\n"+LOG_Show_Old);
			}
			if(m_BT_Addr_Result==false)
			{
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("BT_MAC检查不正确！！！\r\n手机内为："+m_BT_Addr_Read+"\r\n输入的为："+m_BT_Addr+"\r\n\r\n"+LOG_Show_Old);
			} 
			else
			{
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("BT_MAC检查正确！！！\r\n手机内为："+m_BT_Addr_Read+"\r\n输入的为："+m_BT_Addr+"\r\n\r\n"+LOG_Show_Old);
			} 
			if(m_strWifi_Result==false)
			{
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("WIFI_MAC检查不正确！！！\r\n手机内为："+m_strWifi_Read+"\r\n输入的为："+m_strWifi+"\r\n\r\n"+LOG_Show_Old);
			}
			else
			{
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("WIFI_MAC检查正确！！！\r\n手机内为："+m_strWifi_Read+"\r\n输入的为："+m_strWifi+"\r\n\r\n"+LOG_Show_Old);
			}
			
			if(Version_Result==false)
			{
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("软件版本检查不正确！！！分析如下：\r\n"+Version_SoftWare+LOG_Show_Old);
			}
			else 
			{
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("软件版本检查正确！！！分析如下：\r\n"+Version_SoftWare+LOG_Show_Old);
			}
			if(BARCODE_Input_Result==false)
			{
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("\r\n板号异常,可能是重号或者读取失败！！！！！！！！！！！\r\n手机内板号为："+BARCODE_Input+"\r\n\r\n"+LOG_Show_Old);
			}
			else
			{
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("板号正常！！！\r\n手机内板号为："+BARCODE_Input+"\r\n\r\n"+LOG_Show_Old);
			}
			gColor = RGB(255,0,0);
			gFinalResult.Format("     Fail");

		}
		m_Result.UpdateWindow();
		RedrawWindow();  m_Result.UpdateWindow(); 
		//IMEI_Control.SetFocus();// 自动生成号段时，自动循环开始写号--->导致异常，放在后面执行
		//IMEI_Control.SetSel(0,-1);
    }
    else
    {
		m_Result.GetWindowText(LOG_Show_Old);  
		m_Result.SetWindowText("读手机失败！！！\r\n\r\n"+LOG_Show_Old);
		gColor = RGB(255,0,0);
		gFinalResult.Format("     Fail");
		RedrawWindow();  m_Result.UpdateWindow(); 
		//IMEI_Control.SetFocus();// 自动生成号段时，自动循环开始写号--->导致异常，放在后面执行
		//IMEI_Control.SetSel(0,-1);
    }
	return false;
	//m_Result.SetWindowText("");
}

void CWriteIMEIDlg::WriteFun()
{
	extern COLORREF gColor;
	extern CString gFinalResult;
    int nErrorCode = RFT_SUCCESS;
   
    /*int nCount=0;              //新展讯平台不支持
	SP_FastReaIMEICount(&nCount);
    if(IMEI4_Choose_M=="true")
    {
        if(nCount<4)
        {
			m_Result.GetWindowText(LOG_Show_Old);  
			m_Result.SetWindowText("手机不支持IMEI4！！！\r\n\r\n"+LOG_Show_Old);
			gColor = RGB(255,0,0);
			gFinalResult.Format("     Fail");
			RedrawWindow();  m_Result.UpdateWindow(); 
			IMEI_Control.SetFocus();
			IMEI_Control.SetSel(0,-1);
			return;
        }
            
    }
	if(IMEI3_Choose_M=="true")
    {
        if(nCount<3)
        {
			m_Result.GetWindowText(LOG_Show_Old);  
			m_Result.SetWindowText("手机不支持IMEI3！！！\r\n\r\n"+LOG_Show_Old);
			gColor = RGB(255,0,0);
			gFinalResult.Format("     Fail");
			RedrawWindow();  m_Result.UpdateWindow(); 
			IMEI_Control.SetFocus();
			IMEI_Control.SetSel(0,-1);
			return;
        }
            
    }
	if(IMEI2_Choose_M=="true")
    {
        if(nCount<=2)
        {
			m_Result.GetWindowText(LOG_Show_Old);  
			m_Result.SetWindowText("手机不支持IMEI2！！！\r\n\r\n"+LOG_Show_Old);
			gColor = RGB(255,0,0);
			gFinalResult.Format("     Fail");
			RedrawWindow();  m_Result.UpdateWindow(); 
			IMEI_Control.SetFocus();
			IMEI_Control.SetSel(0,-1);
			return;
        }
            
    }
        
    if(MOCOR_VER_PRE09A37==WriteMocChoose)      
    { 
        nErrorCode = WriteMoc1();                        //新展讯平台不支持
    }
    else // && MOCOR_VER_AFTER09A37==m_WriteParam.nMocver)*/

    nErrorCode = WriteMoc2();     
    
    if (RFT_SUCCESS==nErrorCode )
	{
        if(::GetTickCount() - m_dwStartTime > (DWORD)atol(WatchDog_Time_M))  
        {
            //m_strMsg.Format( "写入时间超过%d 毫秒!",m_nWriteTime);
			m_Result.GetWindowText(LOG_Show_Old);  
			m_Result.SetWindowText("写入时间超过"+WatchDog_Time_M+" 毫秒！！！\r\n\r\n"+LOG_Show_Old);
			gColor = RGB(255,0,0);
			gFinalResult.Format("     Fail");
			RedrawWindow();  m_Result.UpdateWindow(); 
			//IMEI_Control.SetFocus(); // 自动生成号段时，自动循环开始写号--->导致异常，放在后面执行
			//IMEI_Control.SetSel(0,-1);
			return;
        }
		if(UP_DB_Data_M=="false")//不上传数据库
		{
			m_Result.GetWindowText(LOG_Show_Old);
			m_Result.SetWindowText("成功写号！！！\r\n\r\n"+LOG_Show_Old);
			//UpdateData(true);  //把中间值赋给变量
			//UpdateData(false); //把变量显示在界面
			m_Result.UpdateWindow();

			if(false==WriteAndThenReadBack())
				return;
			///////自动生成计数处理/////////////////////
			if((AutoBTAddr_Choose_M=="true")||(AutoWIFIAddr_Choose_M=="true"))
				BtAddrCount_int++;
			BtAddrCount.Format("%d",BtAddrCount_int);

			IMEI_Count.Format("%d",IMEI_Count_int);  //IMEI_Count_int 已经递增
			SaveConfig_Change();
			//////////自动生成计数处理//////////////////

			gColor = RGB(0,255,0);
			gFinalResult.Format("     OK");
			RedrawWindow();  m_Result.UpdateWindow(); 
			//OnSelchangeAuto();    //不将IMEI1清零
			//IMEI_Control.SetFocus();// 自动生成号段时，自动循环开始写号--->导致异常，放在后面执行
			//IMEI_Control.SetSel(0,-1);
			
		}
		else if(Initial_Connect_DB_M=="true")//上传数据库//初始化连接了数据库才能上传，权限限制
		{
			if(false==WriteAndThenReadBack())
				return;
			CString Conn="";
			CString DB_FAIL="";
			Conn.Format("driver={SQL Server};Server=%s;DATABASE=%s;UID=%s;PWD=%s", m_server_M,m_db_M,m_user_M, m_pwd_M);
			
			/*if(   (DB_CONNECT_M=="true")&&(IMEI_DB_Choose_M=="false")&&(MAC_DB_Choose_M=="false")&&(Check_repeat_Choose_M=="false")   ||   (ORACLE_Connect_Choose_M=="true")&&(UP_DB_Data_M=="true")&&(IMEI_DB_Choose_M=="false")&&(MAC_DB_Choose_M=="false")&&(Check_repeat_Choose_M=="false")   ) //每次操作数据库时候，重连一次,并且要除了数据库关联的情况（这时候已经连接一次）、除了检查重复性情况
				DB_FAIL=Setdlg.myado.ConnecDataLibrary(Conn,"","",adModeUnknown);//写号成功上传数据库，重连=====展讯平台
			else
				DB_FAIL="SUCCESS";
			*/
			if(Setdlg.myado.m_pCon==NULL)
			{
				DB_FAIL=Setdlg.myado.ConnecDataLibrary(Conn,"","",adModeUnknown);   
			}

			if(Setdlg.myado.m_pCon->State==0)  //1表示已经打开,0表示关闭，数据库意外断开，重连
			{
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("=================================数据库意外断开，重连........\r\n\r\n"+LOG_Show_Old);
				m_Result.UpdateWindow();
				DB_FAIL=Setdlg.myado.ConnecDataLibrary(Conn,"","",adModeUnknown);//数据库意外断开，重连
			}
			else
			{
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("=================================数据库连接正常中...\r\n\r\n"+LOG_Show_Old);
				m_Result.UpdateWindow();
			}

			if(DB_FAIL=="FAIL")
			{
				IMEI_Control.SetFocus();
				IMEI_Control.SetSel(0,-1);
				m_Result.GetWindowText(LOG_Show_Old);
				m_Result.SetWindowText("连接数据库失败，无法上传数据，请检查网络.......\r\n\r\n"+LOG_Show_Old);

				gColor = RGB(255,0,0);
				gFinalResult.Format("UP_FAIL");
				RedrawWindow();  m_Result.UpdateWindow(); 
				return ;
			}

			CTime CurrentTime=CTime::GetCurrentTime();
			CString strTime;
			strTime =CurrentTime.Format( "%Y-%m-%d %H:%M:%S ");
			//CString strSQL = "Insert into dse_barcode_imei(imei,imei2,imei3,imei4,IMEI_NUM,barcode,msn,WIP_ENTITY_NAME,TYPE,MOBCLASS,LINENUM,classnum,PRODADDRESS,create_time,is_return,SOFTVERSION,HARDVERSION,IMEI_WRITER,WIFI_MAC,BLUE_MAC)\
							 values('"+m_IMEI+"','"+m_IMEI2+"','"+m_IMEI3+"','"+m_IMEI4+"','"+IMEI_Num+"','"+BARCODE_Input+"','"+m_SN+"','"+Order_Num_up_M+"','"+Machine_Type_M+"','"+Modulation_Tppe_M+"','"+Line_Number_M+"','"+Floor_M+"','"+Product_Address_M+"','"+strTime+"','"+Is_Return_M+"','"+Software_Version_M+"','"+Hardware_Version_M+"','"+Operator_M+"','"+m_strWifi+"','"+m_BT_Addr+"')";	
			//CString strSQL = "Insert into dse_barcode_imei(imei,imei2,imei3,imei4,barcode,msn,WIP_ENTITY_NAME,TYPE,MOBCLASS,LINENUM,classnum,PRODADDRESS,create_time,is_return,SOFTVERSION,HARDVERSION,IMEI_WRITER,WIFI_MAC,BLUE_MAC)\
							 values('"+m_IMEI+"','"+m_IMEI2+"','"+m_IMEI3+"','"+m_IMEI4+"','"+BARCODE_Input+"','"+m_SN+"','"+Order_Num_up_M+"','"+Machine_Type_M+"','"+Modulation_Tppe_M+"','"+Line_Number_M+"','"+Floor_M+"','"+Product_Address_M+"','"+strTime+"','"+Is_Return_M+"','"+Software_Version_M+"','"+Hardware_Version_M+"','"+Operator_M+"','"+m_strWifi+"','"+m_BT_Addr+"')";			
			//CString strSQL = "Insert into dse_barcode_imei(imei,IMEI2,IMEI3,IMEI4,barcode,msn,WIP_ENTITY_NAME,TYPE,LINENUM,create_time,is_return,SOFTVERSION,HARDVERSION,WIFI_MAC,BLUE_MAC)\
							 values('"+m_IMEI+"','"+m_IMEI2+"','"+m_IMEI3+"','"+m_IMEI4+"','"+BARCODE_Input+"','"+m_SN+"','"+Order_Num_up_M+"','"+Machine_Type_M+"','"+Line_Number_M+"','"+strTime+"','"+Is_Return_M+"','"+Software_Version_M+"','"+Hardware_Version_M+"','"+m_strWifi+"','"+m_BT_Addr+"')";		
			///////////////////解锁码///////////////////////////////////////
			if(Unlock_Code_Choose_M=="true")
			{
				TranslateMuch(m_IMEI);
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("获取IMEI："+m_IMEI+"对应的解锁码："+Unlock_code_Number+"\r\n"+LOG_Show_Old);
				m_Result.UpdateWindow();
			}
			///////////////////解锁码///////////////////////////////////////

			//CString strSQL = "Insert into dse_barcode_imei(imei,IMEI_NUM,barcode,msn,WIP_ENTITY_NAME,TYPE,MOBCLASS,LINENUM,classnum,PRODADDRESS,create_time,is_return,SOFTVERSION,HARDVERSION,IMEI_WRITER,WIFI_MAC,BLUE_MAC) values('"+m_IMEI+"','"+IMEI_Num+"','"+BoardNumber+"','"+m_SN+"','"+Order_Num_up_M+"','"+Machine_Type_M+"','"+Modulation_Tppe_M+"','"+Line_Number_M+"','"+Floor_M+"','"+Product_Address_M+"','"+strTime+"','"+Is_Return_M+"','"+Software_Version_M+"','"+Hardware_Version_M+"','"+Operator_M+"','','"+m_BT_Addr+"')";
			CString strSQL = "Insert into dse_barcode_imei(imei,NetCode,Unlock_Code,IMEI2,IMEI3,IMEI4,barcode,msn,WIP_ENTITY_NAME,TYPE,LINENUM,create_time,is_return,SOFTVERSION,HARDVERSION,WIFI_MAC,BLUE_MAC)\
							 values('"+m_IMEI+"','"+NETCODE_Input+"','"+Unlock_code_Number+"','"+m_IMEI2+"','"+m_IMEI3+"','"+m_IMEI4+"','"+BARCODE_Input+"','"+m_SN+"','"+Order_Num_up_M+"','"+Machine_Type_M+"','"+Line_Number_M+"','"+strTime+"','"+Is_Return_M+"','"+Software_Version_M+"','"+Hardware_Version_M+"','"+m_strWifi+"','"+m_BT_Addr+"')";	
			
			
			Setdlg.myado.OpenSheet("select * from dse_barcode_imei");
			BOOL UP_FAIL=Setdlg.myado.Execute(strSQL);
			Setdlg.myado.CloseSheet();
			Sleep(500);
			Setdlg.myado.OpenSheet("select imei from dse_barcode_imei WHERE imei ='"+m_IMEI+"'");
			BOOL IMEI_Check_UP=Setdlg.myado.Find("imei='"+m_IMEI+"'");
			Setdlg.myado.CloseSheet();
			
			if((UP_FAIL==TRUE&&IMEI_Check_UP==TRUE))//if(ORACLE_Connect_Choose_M=="true")//上传ORACLE数据库  展讯平台没有兼容ORACLE数据库
			{
				//AfxMessageBox("上传SQL成功！");
				m_Result.GetWindowText(LOG_Show_Old);
				m_Result.SetWindowText("成功写号！！！并且成功上传数据库！！！\r\n\r\n"+LOG_Show_Old);
				//UpdateData(true);  //把中间值赋给变量
				//UpdateData(false); //把变量显示在界面
				m_Result.UpdateWindow();
				BOOL UP_WritedFlag;
				if(UpdateIMEI_MSN_Choose_M=="true")
				{
					CString strSQL_WritedFlag="UPDATE dse_imei_sn SET WritedFlag = '1' WHERE imei ='"+m_IMEI+"'";
					Setdlg.myado.OpenSheet("select * from dse_imei_sn WHERE imei ='"+m_IMEI+"'");
					UP_WritedFlag=Setdlg.myado.Execute(strSQL_WritedFlag);
					Setdlg.myado.CloseSheet();

					if(UP_WritedFlag==TRUE)
					{
						m_Result.GetWindowText(LOG_Show_Old);
						m_Result.SetWindowText("更新贴纸表标志成功！\r\n\r\n"+LOG_Show_Old);
					}
					else
					{
						m_Result.GetWindowText(LOG_Show_Old);
						m_Result.SetWindowText("更新贴纸表标志失败！！！\r\n\r\n"+LOG_Show_Old);
					}
					m_Result.UpdateWindow();
				}
				else
				{
					m_Result.GetWindowText(LOG_Show_Old);
					m_Result.SetWindowText("没有勾选更新贴纸表标志..\r\n\r\n"+LOG_Show_Old);
					m_Result.UpdateWindow();
					UP_WritedFlag=TRUE;
				}
				if(UP_WritedFlag==TRUE)
				{
					///////自动生成计数处理/////////////////////
					BtAddrCount_int++;
					BtAddrCount.Format("%d",BtAddrCount_int);

					IMEI_Count.Format("%d",IMEI_Count_int);  //IMEI_Count_int 已经递增
					SaveConfig_Change();
					//////////自动生成计数处理//////////////////

					gColor = RGB(0,255,0);
					gFinalResult.Format("     OK");
					RedrawWindow();  m_Result.UpdateWindow(); 
					//OnSelchangeAuto();    //不将IMEI1清零
					//IMEI_Control.SetFocus();// 自动生成号段时，自动循环开始写号--->导致异常，放在后面执行
					//IMEI_Control.SetSel(0,-1);
				}
				else
				{
					gColor = RGB(255,0,0);
					gFinalResult.Format("UP_FAIL");
					RedrawWindow();  m_Result.UpdateWindow(); 
				}
			}
			else
			{
				m_Result.GetWindowText(LOG_Show_Old);
				m_Result.SetWindowText("成功写号！！！但上传数据库失败！！！！！！！！！！！！！\r\n\r\n"+LOG_Show_Old);
				//UpdateData(true);  //把中间值赋给变量
				//UpdateData(false); //把变量显示在界面
				m_Result.UpdateWindow();

				gColor = RGB(255,0,0);
				gFinalResult.Format("UP_FAIL");
				RedrawWindow();  m_Result.UpdateWindow(); 
				//OnSelchangeAuto();    //不将IMEI1清零
				//IMEI_Control.SetFocus();
				//IMEI_Control.SetSel(0,-1);// 自动生成号段时，自动循环开始写号--->导致异常，放在后面执行
			}
		}
        //m_strMsg.LoadString( "PASS! 按Enter开始下一次测试");
	}
    else
    {
        m_Result.GetWindowText(LOG_Show_Old);  
		m_Result.SetWindowText("写号失败！！！\r\n\r\n"+LOG_Show_Old);
		gColor = RGB(255,0,0);
		gFinalResult.Format("     Fail");
		RedrawWindow();  m_Result.UpdateWindow(); 
		//IMEI_Control.SetFocus();
		//IMEI_Control.SetSel(0,-1);// 自动生成号段时，自动循环开始写号--->导致异常，放在后面执行
    }
	
}

BOOL CWriteIMEIDlg::CheckVersion()
{
    USES_CONVERSION;
	char szVersionInfo[500]={0}; 
	if(0 != SP_GetMsVersion(g_hDiagPhone, szVersionInfo, sizeof(szVersionInfo)))
    {
        
        Version_SoftWare="读软件版本失败\r\n";
        return FALSE;
    }
    else
    {
        CString  strTemp;
		strTemp.Format("%s",szVersionInfo);
		strTemp.Replace("\r","");
		strTemp.Replace("\n","");
		strTemp.Replace(" ","%");
        if(NULL==_tcsstr(strTemp,Software_Version_M))
        {
			Version_SoftWare="软件版本不匹配:\r\n话机软件版本为："+strTemp+"\r\n设置软件版本为："+Software_Version_M+"\r\n";
            return FALSE;
        }   
		else
			Version_SoftWare="软件版本匹配正确:\r\n话机软件版本为："+strTemp+"\r\n设置软件版本为："+Software_Version_M+"\r\n";
    }
    return  TRUE;
}
int CWriteIMEIDlg::WriteMoc1()
{
    return 0;
}


int CWriteIMEIDlg::WriteMoc2()
{
    //USES_CONVERSION;
	FASTNVACCESS_T NvWriteParam;
	ZeroMemory(&NvWriteParam, sizeof(FASTNVACCESS_T));

	//char szTemp[50]={0};
    int nErrorCode  = RFT_SUCCESS;  

    if (IMEI1_Choose_M=="true")
    {
        //strcpy(szTemp,(m_IMEI));  
        //strncpy(NvWriteParam.szImei1, szTemp, IMEI_STR_LENGTH+28);
		NvWriteParam.dwOperMask |=FNAMASK_WRITE_IMEI1;
		strncpy((char*)NvWriteParam.szImei1, m_IMEI.GetBuffer(0), MAX_IMEI_STR_LENGTH);
    }
    if (IMEI2_Choose_M=="true")
    {  
		//strcpy(szTemp,(m_IMEI2));   
		//strncpy(NvWriteParam.szImei2, szTemp, IMEI_STR_LENGTH);
		NvWriteParam.dwOperMask |=FNAMASK_WRITE_IMEI2;
		strncpy((char*)NvWriteParam.szImei2, m_IMEI2.GetBuffer(0), MAX_IMEI_STR_LENGTH);
    }
	if(IMEI3_Choose_M=="true")
    {
       //strcpy(szTemp,(m_IMEI3));  
       //strncpy(NvWriteParam.szImei3, szTemp, IMEI_STR_LENGTH);
		NvWriteParam.dwOperMask |=FNAMASK_WRITE_IMEI3;
		strncpy((char*)NvWriteParam.szImei3, m_IMEI3.GetBuffer(0), MAX_IMEI_STR_LENGTH);
    }
	if(IMEI4_Choose_M=="true")
    {
       //strcpy(szTemp,(m_IMEI4)); 
       //strncpy(NvWriteParam.szImei4, szTemp, IMEI_STR_LENGTH);
		NvWriteParam.dwOperMask |=FNAMASK_WRITE_IMEI4;
		strncpy((char*)NvWriteParam.szImei4, m_IMEI4.GetBuffer(0), MAX_IMEI_STR_LENGTH);
    }

    if (BT_MAC_ADDR_Choose_M=="true")
    {
        //strcpy(szTemp,(m_BT_Addr));     
        //strncpy(NvWriteParam.szBtAddr, szTemp, BT_ADDR_STR_LENGTH);
		NvWriteParam.dwOperMask |= FNAMASK_WRITE_BTADDR;
		strncpy((char*)NvWriteParam.szBTAddr, m_BT_Addr.GetBuffer(0), 12);
		
    }	 
    if(WIFI_MAC_ADDR_Choose_M=="true")
    {
       //strcpy(szTemp,(m_strWifi));  
       //strncpy(NvWriteParam.szWifi, szTemp, BT_ADDR_STR_LENGTH);
	   NvWriteParam.dwOperMask |= FNAMASK_WRITE_WIFIADDR;
	   strncpy((char*)NvWriteParam.szWIFIAddr, m_strWifi.GetBuffer(0), 12);
    }
    nErrorCode=SP_FastWriteNvInfo(g_hDiagPhone,&NvWriteParam);   
    return nErrorCode;

}

void CWriteIMEIDlg::ReflashUI()//暂时无用
{ 
    //TCHAR szTemp[50]={0};
	CHAR szTemp[50]={0};
    _tcscpy(szTemp,AutoIMEI_Start_Num_M); 
    m_int64DefaultIMEI=  _ttoi64(szTemp);
    if((AutoIMEI_Choose_M=="true"))         //暂时无用，自动生成IMEI号-----自动生成没有实际的意义，全部根据号段来生成
    {
        AutoCreateIMEI(szTemp);
        m_IMEI.Format(("%s"),szTemp);
    }
    if((AutoIMEI_Choose_M=="true")&&(IMEI2_Choose_M=="true"))//暂时无用
    {
        if(UseSameIMEI_Choose_M=="false")
        {
            m_int64DefaultIMEI++;
            AutoCreateIMEI(szTemp);
            m_IMEI2.Format(("%s"),szTemp);
        }
        else
        {
            m_IMEI2=m_IMEI;
        }
    }  
    if((AutoIMEI_Choose_M=="true")&&(IMEI3_Choose_M=="true"))//暂时无用
    {
        if(UseSameIMEI_Choose_M=="false")
        {
            m_int64DefaultIMEI++;
            AutoCreateIMEI(szTemp);
            m_IMEI3.Format(("%s"),szTemp);
        }
        else
        {
            m_IMEI3=m_IMEI;
        }
    } 
    if((AutoIMEI_Choose_M=="true")&&(IMEI4_Choose_M=="true"))//暂时无用
    {
        if(UseSameIMEI_Choose_M=="false")
        {
            m_int64DefaultIMEI++;
            AutoCreateIMEI(szTemp);
            m_IMEI4.Format(("%s"),szTemp);
        }
        else
        {
            m_IMEI4=m_IMEI;
        }
    } 
    if(AutoBTAddr_Choose_M=="true")
    {
        // AutoCreateBTAddr(m_szBT);
        AutoCreateBTAddr(szTemp);
        m_BT_Addr.Format(("%s"),szTemp);
    }
    if(AutoWIFIAddr_Choose_M=="true")
    {
        // AutoCreateBTAddr(m_szBT);
        AutoCreateWifiAddr(szTemp);
        m_strWifi.Format(("%s"),szTemp);
    }
    if((AutoIMEI_Choose_M=="false")&&(IMEI1_Choose_M=="true"))//暂时无用
    {
        IMEI_Control.SetFocus(); 
    } 
    else if((AutoIMEI_Choose_M=="false")&&(AutoBTAddr_Choose_M=="true"))//(m_WriteParam.bChkBTAddr&&(!m_WriteParam.bChkAutoCreateBT))
    {
        m_BT_Addr_Control.SetFocus();    
    }
    else
    {
        ;//GetDlgItem(IDC_BUTTON_START)->SetFocus();
        //m_bEnter = TRUE;  
    }

    UpdateData(FALSE);
}


void CWriteIMEIDlg::AutoCreateIMEI(LPTSTR lpszIMEI)
{
    __int64 int64CD = ComputeCD(m_int64DefaultIMEI);
    __int64 int64IMEI = m_int64DefaultIMEI * 10 + int64CD;
    _i64tot(int64IMEI, lpszIMEI, 10); //10进制IMEI转化成字符串
}
void CWriteIMEIDlg::AutoCreateBTAddr(LPTSTR lpszTempBT)
{
    ASSERT(NULL != lpszTempBT);
    srand((unsigned)time( NULL));
    int nRand = rand();
    CTime CurrTime = CTime::GetCurrentTime();
    _stprintf(lpszTempBT, ("%04d%02d%02d%02d%02d"), nRand % 10000, 
        CurrTime.GetDay(), CurrTime.GetHour(), CurrTime.GetMinute(), CurrTime.GetSecond());
}

void CWriteIMEIDlg::AutoCreateWifiAddr(LPTSTR lpszTempWIFI)
{
    ASSERT(NULL != lpszTempWIFI);
    srand((unsigned)time( NULL));
    DWORD dwTime = GetTickCount();
    _stprintf(lpszTempWIFI, ("%s%02d%02d%x%x"),AutoWIFIAd_Start_Num_M, 
      rand()%99,dwTime%99, rand()%16,rand()%16);
}

void CWriteIMEIDlg::OnBnClickedButton4()//预处理
{
	// TODO: 在此添加控件通知处理程序代码
	if(IMEI1_Choose_M=="true")
	{
		IMEI_Control.EnableWindow(true);
	}
	else
	{
		IMEI_Control.EnableWindow(false);
	}

	if(IMEI2_Choose_M=="true")
	{
		IMEI2_Control.EnableWindow(true);
	}
	else
	{
		IMEI2_Control.EnableWindow(false);
	}

	if(IMEI3_Choose_M=="true")
	{
		IMEI3_Control.EnableWindow(true);
	}
	else
	{
		IMEI3_Control.EnableWindow(false);
	}

	if(IMEI4_Choose_M=="true")
	{
		IMEI4_Control.EnableWindow(true);
	}
	else
	{
		IMEI4_Control.EnableWindow(false);
	}


	if(BT_MAC_ADDR_Choose_M=="true")
	{
		m_BT_Addr_Control.EnableWindow(true);
	}
	else
	{
		m_BT_Addr_Control.EnableWindow(false);
	}

	if(WIFI_MAC_ADDR_Choose_M=="true")
	{
		m_WIFI_Addr_Control.EnableWindow(true);
	}
	else
	{
		m_WIFI_Addr_Control.EnableWindow(false);
	}
}

void CWriteIMEIDlg::OnEnChangeImei7()//选择IMEI2
{
	// TODO:  如果该控件是 RICHEDIT 控件，则它将不会
	UpdateData(TRUE);
	UpdateData(false);//把变量显示在界面

	int IMEI_length=m_IMEI2.GetLength();//-SN_Count;
	
	//if(m_IMEI2.Find(IMEI_PRE)!=-1&&IMEI_length==15)//选择了IMEI1、IMEI2
	if(m_IMEI2.Find(IMEI_PRE)!=-1&&   ((m_IMEI2.Find("\r")!=-1)|| (m_IMEI2.Find("\n")!=-1))  )//选择了IMEI1  &&IMEI_length==15
	{

		m_IMEI2.Replace("\r","");
		m_IMEI2.Replace("\n","");
		IMEI_length=m_IMEI2.GetLength();
		//check whether digits
		for(int i=0;i<IMEI_length;i++)
		{
			//if(IMEI_length== 15&&(m_IMEI2.GetAt(i) <'0' || m_IMEI2.GetAt(i) >'9'))
			if(IMEI_length!= 15||(m_IMEI2.GetAt(i) <'0' || m_IMEI2.GetAt(i) >'9'))
			{
				PrintError("Please check IMEI2. Only digits allowed!");
				m_Write.EnableWindow(true);
				IMEI2_Control.SetFocus();
				IMEI2_Control.SetSel(0,-1);
				return;
			}
		}
		
		CString m_IMEI2_Limit;
		long    m_IMEI2_Number;
		m_IMEI2_Limit=m_IMEI2.Left(14);
		m_IMEI2_Limit=m_IMEI2_Limit.Right(6);
		m_IMEI2_Number=atol(m_IMEI2_Limit);          //IMEI1的6位串号
		if( ((m_IMEI2_Number>IMEIEnd_Number)||(m_IMEI2_Number<IMEIStart_Number)) )//检查号段
		{
			PrintError("输入的IMEI2已超过限定的号段，请重新扫描! \r\n\r\n");
			m_Write.EnableWindow(true);
			IMEI2_Control.SetFocus();
			IMEI2_Control.SetSel(0,-1);
			IMEI2_Result=false;
			return;
		}
		else
		{
			m_Result.GetWindowText(LOG_Show_Old);
			IMEI2_Result=true;
			m_Result.SetWindowText("输入的IMEI2在限定的号段内，继续中...! \r\n\r\n"+LOG_Show_Old);
			UpdateData(true);  //把中间值赋给变量
			UpdateData(false); //把变量显示在界面
			m_Result.UpdateWindow();
		}


		if((UseSameIMEI_Choose_M=="true"))        //不会出现这种情况，已经在IMEI1处理判断
		{
			BT_WIFI_Choose();		//输入完IMEI2，用相同的IMEI ；//不会出现这种情况，已经在IMEI1处理判断
		}
		else if((UseSameIMEI_Choose_M=="false")&&(IMEI3_Choose_M=="true"))//采样不同IMEI,且选择了IMEI2、IMEI3
		{
			IMEI3_Control.SetFocus();
			IMEI3_Control.SetSel(0,-1);
		}
		else if(IMEI3_Choose_M=="false")                                  //采用不同IMEI,选择了IMEI2，未选择IMEI3，则判断是否需要些BT/WIFI MAC
		{
			BT_WIFI_Choose();	//输入完IMEI2
		}
	}
	else if(IMEI_length==SN_Count&&m_IMEI2.Find(IMEI_PRE)==-1)
	{
		IMEI2_Control.SetFocus();
		IMEI2_Control.SetSel(0,-1);
		m_Result.GetWindowText(LOG_Show_Old);
		m_Result.SetWindowText("IMEI2不正确，请重新扫描! \r\n\r\n"+LOG_Show_Old);
	}
	else if(IMEI_length==2&&m_IMEI2.Find(IMEI_PRE.Left(2))==-1)
	{
		IMEI2_Control.SetFocus();
		IMEI2_Control.SetSel(0,-1);
		m_Result.GetWindowText(LOG_Show_Old);
		m_Result.SetWindowText("IMEI2不正确，请重新扫描! \r\n\r\n"+LOG_Show_Old);
	}
	else if(IMEI_length==1&&m_IMEI2.Find(IMEI_PRE.Left(1))==-1)
	{
		IMEI2_Control.SetFocus();
		IMEI2_Control.SetSel(0,-1);
		m_Result.GetWindowText(LOG_Show_Old);
		m_Result.SetWindowText("IMEI2不正确，请重新扫描! \r\n\r\n"+LOG_Show_Old);
	}
	
}

void CWriteIMEIDlg::OnEnChangeImei10()//选择IMEI3
{
	// TODO:  如果该控件是 RICHEDIT 控件，则它将不会
	UpdateData(TRUE);
	UpdateData(false);//把变量显示在界面

	int IMEI_length=m_IMEI3.GetLength();//-SN_Count;
	
	//if(m_IMEI3.Find(IMEI_PRE)!=-1&&IMEI_length==15)//选择了IMEI1、IMEI2、IMEI3
	if(m_IMEI3.Find(IMEI_PRE)!=-1&&   ((m_IMEI3.Find("\r")!=-1)|| (m_IMEI3.Find("\n")!=-1))  )//选择了IMEI1  &&IMEI_length==15
	{
		//check whether digits
		m_IMEI3.Replace("\r","");
		m_IMEI3.Replace("\n","");
		IMEI_length=m_IMEI3.GetLength();


		//check whether digits
		for(int i=0;i<IMEI_length;i++)
		{
			//if(IMEI_length== 15&&(m_IMEI3.GetAt(i) <'0' || m_IMEI3.GetAt(i) >'9'))
			if(IMEI_length!= 15||(m_IMEI3.GetAt(i) <'0' || m_IMEI3.GetAt(i) >'9'))
			{
				PrintError("Please check IMEI3. Only digits allowed!");
				m_Write.EnableWindow(true);
				IMEI3_Control.SetFocus();
				IMEI3_Control.SetSel(0,-1);
				return;
			}
		}
		
		CString m_IMEI3_Limit;
		long    m_IMEI3_Number;
		m_IMEI3_Limit=m_IMEI3.Left(14);
		m_IMEI3_Limit=m_IMEI3_Limit.Right(6);
		m_IMEI3_Number=atol(m_IMEI3_Limit);          //IMEI1的6位串号
		if( ((m_IMEI3_Number>IMEIEnd_Number)||(m_IMEI3_Number<IMEIStart_Number)) )//检查号段
		{
			PrintError("输入的IMEI3已超过限定的号段，请重新扫描! \r\n\r\n");
			m_Write.EnableWindow(true);
			IMEI3_Control.SetFocus();
			IMEI3_Control.SetSel(0,-1);
			IMEI3_Result=false;
			return;
		}
		else
		{
			IMEI3_Result=true;
			m_Result.GetWindowText(LOG_Show_Old);
			m_Result.SetWindowText("输入的IMEI3在限定的号段内，继续中...! \r\n\r\n"+LOG_Show_Old);
			UpdateData(true);  //把中间值赋给变量
			UpdateData(false); //把变量显示在界面
			m_Result.UpdateWindow();

		}




		if((UseSameIMEI_Choose_M=="true"))        //不会出现这种情况，已经在IMEI1处理判断
		{
			BT_WIFI_Choose();		//输入完IMEI3，用相同的IMEI ；//不会出现这种情况，已经在IMEI1处理判断
		}
		else if((UseSameIMEI_Choose_M=="false")&&(IMEI4_Choose_M=="true"))//采样不同IMEI,且选择了IMEI2、IMEI3、IMEI4
		{
			IMEI4_Control.SetFocus();
			IMEI4_Control.SetSel(0,-1);
		}
		else if(IMEI4_Choose_M=="false")                                  //采用不同IMEI,选择了IMEI2、IMEI3，未选择IMEI4，则判断是否需要些BT/WIFI MAC
		{
			BT_WIFI_Choose();	//输入完IMEI3
		}
	}
	else if(IMEI_length==SN_Count&&m_IMEI3.Find(IMEI_PRE)==-1)
	{
		IMEI3_Control.SetFocus();
		IMEI3_Control.SetSel(0,-1);
		m_Result.GetWindowText(LOG_Show_Old);
		m_Result.SetWindowText("IMEI3不正确，请重新扫描! \r\n\r\n"+LOG_Show_Old);
	}
	else if(IMEI_length==2&&m_IMEI3.Find(IMEI_PRE.Left(2))==-1)
	{
		IMEI3_Control.SetFocus();
		IMEI3_Control.SetSel(0,-1);
		m_Result.GetWindowText(LOG_Show_Old);
		m_Result.SetWindowText("IMEI3不正确，请重新扫描! \r\n\r\n"+LOG_Show_Old);
	}
	else if(IMEI_length==1&&m_IMEI3.Find(IMEI_PRE.Left(1))==-1)
	{
		IMEI3_Control.SetFocus();
		IMEI3_Control.SetSel(0,-1);
		m_Result.GetWindowText(LOG_Show_Old);
		m_Result.SetWindowText("IMEI3不正确，请重新扫描! \r\n\r\n"+LOG_Show_Old);
	}
}

void CWriteIMEIDlg::OnEnChangeImei9()//选择IMEI4
{
	UpdateData(TRUE);
	UpdateData(false);//把变量显示在界面

	int IMEI_length=m_IMEI4.GetLength();//-SN_Count;
	
	//if(m_IMEI4.Find(IMEI_PRE)!=-1&&IMEI_length==15)//选择了IMEI1、IMEI2
	if(m_IMEI4.Find(IMEI_PRE)!=-1&&   ((m_IMEI4.Find("\r")!=-1)|| (m_IMEI4.Find("\n")!=-1))  )//选择了IMEI1  &&IMEI_length==15
	{
		//check whether digits
		m_IMEI4.Replace("\r","");
		m_IMEI4.Replace("\n","");
		IMEI_length=m_IMEI4.GetLength();

		//check whether digits
		for(int i=0;i<IMEI_length;i++)
		{
			//if(IMEI_length== 15&&(m_IMEI4.GetAt(i) <'0' || m_IMEI4.GetAt(i) >'9'))
			if(IMEI_length!= 15||(m_IMEI4.GetAt(i) <'0' || m_IMEI4.GetAt(i) >'9'))
			{
				PrintError("Please check IMEI4. Only digits allowed!");
				m_Write.EnableWindow(true);
				IMEI4_Control.SetFocus();
				IMEI4_Control.SetSel(0,-1);
				return;
			}
		}
		
		CString m_IMEI4_Limit;
		long    m_IMEI4_Number;
		m_IMEI4_Limit=m_IMEI4.Left(14);
		m_IMEI4_Limit=m_IMEI4_Limit.Right(6);
		m_IMEI4_Number=atol(m_IMEI4_Limit);          //IMEI1的6位串号
		if( ((m_IMEI4_Number>IMEIEnd_Number)||(m_IMEI4_Number<IMEIStart_Number)) )//检查号段
		{
			PrintError("输入的IMEI4已超过限定的号段，请重新扫描! \r\n\r\n");
			m_Write.EnableWindow(true);
			IMEI4_Control.SetFocus();
			IMEI4_Control.SetSel(0,-1);
			IMEI4_Result=false;
			return;
		}
		else
		{
			IMEI4_Result=true;
			m_Result.GetWindowText(LOG_Show_Old);
			m_Result.SetWindowText("输入的IMEI4在限定的号段内，继续中...! \r\n\r\n"+LOG_Show_Old);
			UpdateData(true);  //把中间值赋给变量
			UpdateData(false); //把变量显示在界面
			m_Result.UpdateWindow();
		}


		BT_WIFI_Choose();  //输入完IMEI4
		/*if((BT_MAC_ADDR_Choose_M=="true"))//采样不同IMEI,且选择了IMEI2、IMEI3、IMEI4、BT_mac
		{
			m_BT_Addr_Control.SetFocus();
			m_BT_Addr_Control.SetSel(0,-1);
		}
		else if(WIFI_MAC_ADDR_Choose_M=="true")                                  //采用不同IMEI,选择了IMEI2、IMEI3、IMEI4，未选择BT MAC，则判断是否需要写WIFI MAC
		{
			m_WIFI_Addr_Control.SetFocus();
			m_WIFI_Addr_Control.SetSel(0,-1);
		}
		else
		{
			if((Scan_Choose_M=="true")&&(INITIAL_POWER==true))
				MAIN_Function();//只有IMEI号
		}*/
	}
	else if(IMEI_length==SN_Count&&m_IMEI4.Find(IMEI_PRE)==-1)
	{
		IMEI4_Control.SetFocus();
		IMEI4_Control.SetSel(0,-1);
		m_Result.GetWindowText(LOG_Show_Old);
		m_Result.SetWindowText("IMEI不正确，请重新扫描! \r\n\r\n"+LOG_Show_Old);
	}
	else if(IMEI_length==2&&m_IMEI4.Find(IMEI_PRE.Left(2))==-1)
	{
		IMEI4_Control.SetFocus();
		IMEI4_Control.SetSel(0,-1);
		m_Result.GetWindowText(LOG_Show_Old);
		m_Result.SetWindowText("IMEI不正确，请重新扫描! \r\n\r\n"+LOG_Show_Old);
	}
	else if(IMEI_length==1&&m_IMEI4.Find(IMEI_PRE.Left(1))==-1)
	{
		IMEI4_Control.SetFocus();
		IMEI4_Control.SetSel(0,-1);
		m_Result.GetWindowText(LOG_Show_Old);
		m_Result.SetWindowText("IMEI不正确，请重新扫描! \r\n\r\n"+LOG_Show_Old);
	}
}

void CWriteIMEIDlg::OnEnChangeBtAddr()//BT_MAC
{
	//int i;
	if(BT_MAC_ADDR_Choose_M=="true")
	{
		UpdateData(TRUE);
		UpdateData(false);//把变量显示在界面

		int BT_length=m_BT_Addr.GetLength();//-SN_Count;
		
		//if(m_BT_Addr.Find(BT_PRE)!=-1&&BT_length==12)//选择了BT
		if(m_BT_Addr.Find(BT_PRE)!=-1&&   ((m_BT_Addr.Find("\r")!=-1)|| (m_BT_Addr.Find("\n")!=-1))  )//选择了IMEI1  &&IMEI_length==15
		{
			m_BT_Addr.Replace("\r","");
			m_BT_Addr.Replace("\n","");
			BT_length=m_BT_Addr.GetLength();
			////////////////////BT地址判断///////////////////
			LoadConfig_Change();
			/*char a[100];
			CString str(”aaaaaa”);
			strncpy(a,(LPCTSTR)str,sizeof(a));*/
			int BT_addr1[12];
			long BT_addr2=0;
			long BT_addr3=0;
			long BT_addr4=0;
			char m_BT_Addr_Temp3[5];

			


			//蓝牙地址的低6位
			for(int i=1;i<7;i++)
			{
				m_BT_Addr_Temp1=BT_Addr_EndShow_M.Right(i);
				m_BT_Addr_Temp2=m_BT_Addr_Temp1.Left(1);
				strcpy(m_BT_Addr_Temp3,(LPCTSTR)m_BT_Addr_Temp2);	//strncpy(m_BT_Addr_Temp3,(LPCTSTR)m_BT_Addr_Temp2,sizeof(m_BT_Addr_Temp2));	
				BT_addr1[i-1]=CharToHexChar(m_BT_Addr_Temp3[0]);    //BT_addr1[i-1]=atoi(m_BT_Addr_Temp3);
				BT_addr4+=BT_addr1[i-1]*(pow((double)16,(i-1)));    //BT结束地址后6位
			}

			for(int i=1;i<7;i++)
			{
				m_BT_Addr_Temp1=BtAddrStart_M.Right(i);
				m_BT_Addr_Temp2=m_BT_Addr_Temp1.Left(1);
				strcpy(m_BT_Addr_Temp3,(LPCTSTR)m_BT_Addr_Temp2);	//strncpy(m_BT_Addr_Temp3,(LPCTSTR)m_BT_Addr_Temp2,sizeof(m_BT_Addr_Temp2));	
				BT_addr1[i-1]=CharToHexChar(m_BT_Addr_Temp3[0]);    //BT_addr1[i-1]=atoi(m_BT_Addr_Temp3);
				BT_addr2+=BT_addr1[i-1]*(pow((double)16,(i-1)));
			}

			long BT_addr2_High=0;
			long BT_addr4_High=0;
			//蓝牙地址高6位
			for(int i=7;i<13;i++)
			{
				m_BT_Addr_Temp1=BT_Addr_EndShow_M.Right(i);
				m_BT_Addr_Temp2=m_BT_Addr_Temp1.Left(1);
				strcpy(m_BT_Addr_Temp3,(LPCTSTR)m_BT_Addr_Temp2);	//strncpy(m_BT_Addr_Temp3,(LPCTSTR)m_BT_Addr_Temp2,sizeof(m_BT_Addr_Temp2));	
				BT_addr1[i-1]=CharToHexChar(m_BT_Addr_Temp3[0]);    //BT_addr1[i-1]=atoi(m_BT_Addr_Temp3);
				BT_addr4_High+=BT_addr1[i-1]*(pow((double)16,(i-7)));    //BT结束地址后6位
			}

			for(int i=7;i<13;i++)
			{
				m_BT_Addr_Temp1=BtAddrStart_M.Right(i);
				m_BT_Addr_Temp2=m_BT_Addr_Temp1.Left(1);
				strcpy(m_BT_Addr_Temp3,(LPCTSTR)m_BT_Addr_Temp2);	//strncpy(m_BT_Addr_Temp3,(LPCTSTR)m_BT_Addr_Temp2,sizeof(m_BT_Addr_Temp2));	
				BT_addr1[i-1]=CharToHexChar(m_BT_Addr_Temp3[0]);    //BT_addr1[i-1]=atoi(m_BT_Addr_Temp3);
				BT_addr2_High+=BT_addr1[i-1]*(pow((double)16,(i-7)));
			}
			


			long BT_addr_Input=0;
			long BT_addr_Input_High=0;
			//输入的蓝牙地址
			for(int i=1;i<7;i++)
			{
				m_BT_Addr_Temp1=m_BT_Addr.Right(i);
				m_BT_Addr_Temp2=m_BT_Addr_Temp1.Left(1);
				strcpy(m_BT_Addr_Temp3,(LPCTSTR)m_BT_Addr_Temp2);	//strncpy(m_BT_Addr_Temp3,(LPCTSTR)m_BT_Addr_Temp2,sizeof(m_BT_Addr_Temp2));	
				BT_addr1[i-1]=CharToHexChar(m_BT_Addr_Temp3[0]);    //BT_addr1[i-1]=atoi(m_BT_Addr_Temp3);
				BT_addr_Input+=BT_addr1[i-1]*(pow((double)16,(i-1)));
			}

			for(int i=7;i<13;i++)
			{
				m_BT_Addr_Temp1=m_BT_Addr.Right(i);
				m_BT_Addr_Temp2=m_BT_Addr_Temp1.Left(1);
				strcpy(m_BT_Addr_Temp3,(LPCTSTR)m_BT_Addr_Temp2);	//strncpy(m_BT_Addr_Temp3,(LPCTSTR)m_BT_Addr_Temp2,sizeof(m_BT_Addr_Temp2));	
				BT_addr1[i-1]=CharToHexChar(m_BT_Addr_Temp3[0]);    //BT_addr1[i-1]=atoi(m_BT_Addr_Temp3);
				BT_addr_Input_High+=BT_addr1[i-1]*(pow((double)16,(i-7)));    //BT结束地址后6位
			}
			
			long BtAddrEnd_M_int=BT_addr4-BT_addr2+1;
			BtAddrEnd_M.Format("%ld",BtAddrEnd_M_int);
			BtAddrTotal=BtAddrEnd_M;                                //BtAddrTotal桌面显示总数，BtAddrEnd_M为设置后的总数，BtAddrEnd--》地址总数
			
			BtAddrCount_int=atol(BtAddrCount);                      //BT地址计数
			//BT_addr3=BtAddrCount_int+BT_addr2;
			//m_BT_Addr_Temp1.Format("%X",BT_addr3);
			//m_BT_Addr_Temp2=BtAddrStart_M.Left(6);
			//m_BT_Addr=m_BT_Addr_Temp2+m_BT_Addr_Temp1;            //得到当前BT_MAC地址,无论扫描输入或是自动生成，不再对m_BT_Addr重新赋值

			BtAddrTotal_int=atol(BtAddrTotal);                      //BT地址总数

			UpdateData(false);

			/*
			 if(strCommand == "BtAddrStart_M")
            {
             	      BtAddrStart_M = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");
            }
			else if(strCommand == "BtAddrCount")
            {
             	      BtAddrCount = m_fIniFile.ReadString(STRING_SECTION_CONFIG, strCommand, "");
            }
			*/
			
			//check BT_Addr                                                                                      
			if( ((BtAddrTotal_int<=BtAddrCount_int)||  (((BT_addr_Input<BT_addr2)||(BT_addr_Input>BT_addr4))&&((BT_addr_Input_High>=BT_addr2_High)||(BT_addr_Input_High<=BT_addr4_High))) \
				||(BT_addr_Input_High<BT_addr2_High)||(BT_addr_Input_High>BT_addr4_High)) \
				 )//检查号段
			{
				PrintError("BT_Addr超过范围！");
				m_BT_Addr_Control.SetFocus();
				m_BT_Addr_Control.SetSel(0,-1);
				return;
			}

			if(m_BT_Addr.GetLength() != 12 )
			{
				PrintError("Please check BT_Addr.");
				m_Write.EnableWindow(true);
				m_BT_Addr_Control.SetFocus();
				m_BT_Addr_Control.SetSel(0,-1);
				return;
			}
			if(m_BT_Addr.GetLength() == 12)
			{
				for(int i=0;i<m_BT_Addr.GetLength();i++)
				{
					if((m_BT_Addr.GetAt(i) >='0' && m_BT_Addr.GetAt(i) <='9')  ||  (m_BT_Addr.GetAt(i) >='a' && m_BT_Addr.GetAt(i) <='f')||  (m_BT_Addr.GetAt(i) >='A' && m_BT_Addr.GetAt(i) <='F'))
					{
						if(i==11)
							BT_Result=true;
						
						
					}
					else
					{
						PrintError("Please check BT_Addr OK,16进制 allowed!");
						m_Write.EnableWindow(true);
						m_BT_Addr_Control.SetFocus();
						m_BT_Addr_Control.SetSel(0,-1);
						BT_Result=false;
						return;
					}
				}
				if((Scan_Choose_M=="true")&&(INITIAL_POWER==true))
				{
					m_Result.GetWindowText(LOG_Show_Old);
					m_Result.SetWindowText("输入的BT_MAC在限定的号段内正常，继续中...! \r\n\r\n"+LOG_Show_Old);
					UpdateData(true);  //把中间值赋给变量
					UpdateData(false); //把变量显示在界面
					m_Result.UpdateWindow();
				}
			}
			else
			{
				m_BT_Addr_Control.SetFocus();
				m_BT_Addr_Control.SetSel(0,-1);
				m_Result.GetWindowText(LOG_Show_Old);
				m_Result.SetWindowText("BT位数不正确，请重新扫描! \r\n\r\n"+LOG_Show_Old);
			}
			////////////////////BT地址判断///////////////////

			if(WIFI_MAC_ADDR_Choose_M=="true")                                //选择了BT MAC，判断是否需要写WIFI MAC
			{
				m_WIFI_Addr_Control.SetFocus();
				m_WIFI_Addr_Control.SetSel(0,-1);
			}
			else if(NETCODE_Choose_M=="true")
			{
				NETCODE_Control.SetFocus();
				NETCODE_Control.SetSel(0,-1);
			}
			else
			{
				if((Scan_Choose_M=="true")&&(INITIAL_POWER==true))
					MAIN_Function();//
			}
		}
		else if(BT_length==SN_Count&&m_BT_Addr.Find(BT_PRE)==-1)
		{
			m_BT_Addr_Control.SetFocus();
			m_BT_Addr_Control.SetSel(0,-1);
			m_Result.GetWindowText(LOG_Show_Old);
			m_Result.SetWindowText("BT不正确，请重新扫描! \r\n\r\n"+LOG_Show_Old);
		}
		else if(BT_length==2&&m_BT_Addr.Find(BT_PRE.Left(2))==-1)
		{
			m_BT_Addr_Control.SetFocus();
			m_BT_Addr_Control.SetSel(0,-1);
			m_Result.GetWindowText(LOG_Show_Old);
			m_Result.SetWindowText("BT不正确，请重新扫描! \r\n\r\n"+LOG_Show_Old);
		}
		else if(BT_length==1&&m_BT_Addr.Find(BT_PRE.Left(1))==-1)
		{
			m_BT_Addr_Control.SetFocus();
			m_BT_Addr_Control.SetSel(0,-1);
			m_Result.GetWindowText(LOG_Show_Old);
			m_Result.SetWindowText("BT不正确，请重新扫描! \r\n\r\n"+LOG_Show_Old);
		}
	}
}

void CWriteIMEIDlg::OnEnChangeBtAddr2()//WIFI_MAC
{
	int i;
	if(WIFI_MAC_ADDR_Choose_M=="true")
	{
		UpdateData(TRUE);
		UpdateData(false);//把变量显示在界面

		int WIFI_length=m_strWifi.GetLength();//-SN_Count;
		
		//if(m_strWifi.Find(WIFI_PRE)!=-1&&WIFI_length==12)//选择了WIFI

		if(m_strWifi.Find(WIFI_PRE)!=-1&&   ((m_strWifi.Find("\r")!=-1)|| (m_strWifi.Find("\n")!=-1))  )//选择了IMEI1  &&IMEI_length==15
		{
			m_strWifi.Replace("\r","");
			m_strWifi.Replace("\n","");
			WIFI_length=m_strWifi.GetLength();
			////////////////////WIFI地址判断///////////////////
			LoadConfig_Change();
			/*char a[100];
			CString str(”aaaaaa”);
			strncpy(a,(LPCTSTR)str,sizeof(a));*/
			int BT_addr1[6];
			long BT_addr2=0;
			long BT_addr3=0;
			long BT_addr4=0;
			char m_BT_Addr_Temp3[5];


			for(i=1;i<7;i++)//wifi结束地址低6位转化为10进制
			{
				m_BT_Addr_Temp1=WIFIAddrEnd_M.Right(i);
				m_BT_Addr_Temp2=m_BT_Addr_Temp1.Left(1);
				strcpy(m_BT_Addr_Temp3,(LPCTSTR)m_BT_Addr_Temp2);	//strncpy(m_BT_Addr_Temp3,(LPCTSTR)m_BT_Addr_Temp2,sizeof(m_BT_Addr_Temp2));	
				BT_addr1[i-1]=CharToHexChar(m_BT_Addr_Temp3[0]);    //BT_addr1[i-1]=atoi(m_BT_Addr_Temp3);
				BT_addr4+=BT_addr1[i-1]*(pow((double)16,(i-1)));    //WIFI结束地址后6位
			}

			for(i=1;i<7;i++)//wifi起始地址低6位转化为10进制
			{
				m_BT_Addr_Temp1=WIFIAddrStart_M.Right(i);
				m_BT_Addr_Temp2=m_BT_Addr_Temp1.Left(1);
				strcpy(m_BT_Addr_Temp3,(LPCTSTR)m_BT_Addr_Temp2);	//strncpy(m_BT_Addr_Temp3,(LPCTSTR)m_BT_Addr_Temp2,sizeof(m_BT_Addr_Temp2));	
				BT_addr1[i-1]=CharToHexChar(m_BT_Addr_Temp3[0]);    //BT_addr1[i-1]=atoi(m_BT_Addr_Temp3);
				BT_addr2+=BT_addr1[i-1]*(pow((double)16,(i-1)));
			}

			WIFIAddrTotal_int=BT_addr4-BT_addr2+1;                  //WIFI地址总数
			
			WIFIAddrCount_int=atol(BtAddrCount);                      //WIFI_MAC地址计数
			//BT_addr3=WIFIAddrCount_int+BT_addr2;
			//m_BT_Addr_Temp1.Format("%X",BT_addr3);
			//m_BT_Addr_Temp2=WIFIAddrStart_M.Left(6);
			//m_strWifi=m_BT_Addr_Temp2+m_BT_Addr_Temp1;              //得到当前WIFI的MAC地址


			UpdateData(false);

			
			//check WIFI_Addr
			if( (WIFIAddrTotal_int<=WIFIAddrCount_int) )//检查号段
			{
				PrintError("WIFI_Addr超过范围！");
				m_WIFI_Addr_Control.SetFocus();
				m_WIFI_Addr_Control.SetSel(0,-1);
				return;
			}

			if(m_strWifi.GetLength() != 12 )
			{
				PrintError("Please check WIFI_Addr.");
				m_Write.EnableWindow(true);
				m_WIFI_Addr_Control.SetFocus();
				m_WIFI_Addr_Control.SetSel(0,-1);
				return;
			}
			if(m_strWifi.GetLength() == 12)
			{
				for(i=0;i<m_strWifi.GetLength();i++)
				{
					if((m_strWifi.GetAt(i) >='0' && m_strWifi.GetAt(i) <='9')  ||  (m_strWifi.GetAt(i) >='a' && m_strWifi.GetAt(i) <='f')||  (m_strWifi.GetAt(i) >='A' && m_strWifi.GetAt(i) <='F'))
					{
						if(i==11)//全部满足要求
							WIFI_Result=true;
						
						
					}
					else
					{
						PrintError("Please check WIFI_Addr OK,16进制 allowed!");
						m_Write.EnableWindow(true);
						m_WIFI_Addr_Control.SetFocus();
						m_WIFI_Addr_Control.SetSel(0,-1);
						WIFI_Result=false;
						return;
					}	
				}
				if((Scan_Choose_M=="true")&&(INITIAL_POWER==true))
				{
					m_Result.GetWindowText(LOG_Show_Old);
					m_Result.SetWindowText("输入的WIFI_MAC在限定的号段内正常，继续中...! \r\n\r\n"+LOG_Show_Old);
					UpdateData(true);  //把中间值赋给变量
					UpdateData(false); //把变量显示在界面
					m_Result.UpdateWindow();
				}
			}
			else
			{
				m_WIFI_Addr_Control.SetFocus();
				m_WIFI_Addr_Control.SetSel(0,-1);
				m_Result.GetWindowText(LOG_Show_Old);
				m_Result.SetWindowText("WIFI位数不正确，请重新扫描! \r\n\r\n"+LOG_Show_Old);
			}
			////////////////////WIFI地址判断///////////////////
			if(NETCODE_Choose_M=="true")
			{
				NETCODE_Control.SetFocus();
				NETCODE_Control.SetSel(0,-1);
			}
			else
			{
				if((Scan_Choose_M=="true")&&(INITIAL_POWER==true))
					MAIN_Function();//
			}
		}
		else if(WIFI_length==SN_Count&&m_strWifi.Find(WIFI_PRE)==-1)
		{
			m_WIFI_Addr_Control.SetFocus();
			m_WIFI_Addr_Control.SetSel(0,-1);
			m_Result.GetWindowText(LOG_Show_Old);
			m_Result.SetWindowText("WIFI不正确，请重新扫描! \r\n\r\n"+LOG_Show_Old);
		}
		else if(WIFI_length==2&&m_strWifi.Find(WIFI_PRE.Left(2))==-1)
		{
			m_WIFI_Addr_Control.SetFocus();
			m_WIFI_Addr_Control.SetSel(0,-1);
			m_Result.GetWindowText(LOG_Show_Old);
			m_Result.SetWindowText("WIFI不正确，请重新扫描! \r\n\r\n"+LOG_Show_Old);
		}
		else if(WIFI_length==1&&m_strWifi.Find(WIFI_PRE.Left(1))==-1)
		{
			m_WIFI_Addr_Control.SetFocus();
			m_WIFI_Addr_Control.SetSel(0,-1);
			m_Result.GetWindowText(LOG_Show_Old);
			m_Result.SetWindowText("WIFI不正确，请重新扫描! \r\n\r\n"+LOG_Show_Old);
		}
	}
}

void CWriteIMEIDlg::OnEnChangeEdit1()
{
	UpdateData(true);//将界面上的值赋给控件变量
}

void CWriteIMEIDlg::OnBnClickedButton5()//停止操作手机
{
	// TODO: 在此添加控件通知处理程序代码 
	CLeadcoreChip::Tstop=1;
	CString Plat_Form_CS;
	Plat_Form.GetWindowTextA(Plat_Form_CS);
	
	m_Result.GetWindowText(LOG_Show_Old);
	m_Result.SetWindowText("正在退出线程...请稍后 \r\n\r\n"+LOG_Show_Old);
	m_Result.UpdateWindow();
	//Sleep(3000);
	if(NULL != m_hEnterModeThread)
	{
		SetEvent(m_hExitThread);                                          //此处没有作用，因为非WaitForSingleObject(m_hExitThread,	3000)
		TerminateThread(m_hEnterModeThread, 0);
		m_hEnterModeThread = NULL;
	}
	if(Plat_Form_CS=="联芯4G_MIFI")
	{
		//Sleep(2000);
	}
	else
	{
		PrintError("或因超时停止写号！！！");
	}
	m_Write.EnableWindow(true);
	m_Result.UpdateWindow();
	IMEI_Control.SetFocus();
	IMEI_Control.SetSel(0,-1); 
    return;
}

void CWriteIMEIDlg::OnClose() 
{
    // TODO: Add your message handler code here and/or call default
    //SaveSetting();//退出前记得保存
    if(NULL != m_hEnterModeSuccess)
    {
        CloseHandle(m_hEnterModeSuccess);  
    }
    
    if(NULL != m_hExitThread)
    {
        CloseHandle(m_hExitThread);
    }
    if(NULL != m_hEnterModeThread)
    {    
        //TerminateThread(m_hEnterModeThread, 0);    
        //CloseHandle(m_hEnterModeThread);
        m_hEnterModeThread = NULL;
    }  
	//MessageBox("最好不要这样退出!","哈哈",MB_ICONWARNING);
	//AfxMessageBox("退出");
	MetaContextDeInit();//清除MTK的初始化
    CDialog::OnClose();
}
void CWriteIMEIDlg::SendIP_Ping(int Select_Order)
{
	SECURITY_ATTRIBUTES sa;     //安全属性       
	HANDLE hRead,hWrite;        //管道句柄

	PingOutput="";

	sa.nLength = sizeof(SECURITY_ATTRIBUTES);   
	sa.lpSecurityDescriptor = NULL;   
	sa.bInheritHandle = TRUE;              //CreatPipe()创建管道读、写句柄将会被继承，可调用DuplicateHandle()函数改变管道句柄的继承
	
	if(!CreatePipe(&hRead,&hWrite,&sa,0))  //参数：读句柄的指针、写句柄的指针、安全属性、管道大小
	{   
		MessageBox("CreatePipe Failed");   
		return;   
	}   
    STARTUPINFO si;   
    PROCESS_INFORMATION pi;   
   
    ZeroMemory(&si,sizeof(STARTUPINFO));   
    si.cb = sizeof(STARTUPINFO);   
    GetStartupInfo(&si);   
    si.hStdError = hWrite;   
    si.hStdOutput = hWrite;   
    si.wShowWindow = SW_HIDE;   
    si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;   

   	char cmdline[200];   
	CString tmp,OrderCmd;   

	char buffer[4096] = {0};   
	  
	DWORD bytesRead;   
	
	OrderCmd=IPCommand(Select_Order);
	tmp.Format("cmd /C %s",OrderCmd);   

	sprintf(cmdline,"%s",tmp);   
	if(!CreateProcess(NULL,cmdline,NULL,NULL,TRUE,NULL,NULL,NULL,&si,&pi))   
	{   
		MessageBox("CreateProcess failed!");   return;   
	}   
	 
	CloseHandle(hWrite);   		   
	while(1)   
	{   
		if(NULL == ReadFile(hRead,buffer,4095,&bytesRead,NULL))   
		   break;   
		PingOutput += buffer;   
		UpdateWindow();   
		Sleep(1000);   
	}   
	CloseHandle(hRead);  
}

CString CWriteIMEIDlg::IPCommand(int Select_Order)
{
	CString Order;
	switch(Select_Order)
	{
	case 0:
		{
			Order="ping "+m_server_M+" -n 1 -l 8";
			break;
		}
	case 5:
		{
			Order="arp -a";      
			break;
		}
	case 6:
		{
			Order="arp -d";      
			break;
		}
	case 7:
		{
		    Order="cmd /C C:\\limit.vbs";
			break;
		}
	}
	return Order;
}

void CWriteIMEIDlg::OnTimerFunction(UINT nIDEvent)
//非定时器
{
	extern CString gFinalResult;
	extern COLORREF gColor;
	int PingOutput_Rate=100;
	
	int Try_Time=0;
	int PingOutput_Rate_Zero_Time=0;
	switch(nIDEvent)
	{
	case 25:
		do
		{	
			SendIP_Ping(0);
		
			m_Result.GetWindowText(LOG_Show_Old);
			m_Result.SetWindowText("网络连接状态：\r\n"+PingOutput+LOG_Show_Old);
			UpdateData(true);  //把中间值赋给变量
			UpdateData(false); //把变量显示在界面
			m_Result.UpdateWindow();
	
			int nRateEnd = PingOutput.Find('%');
			PingOutput=PingOutput.Left(nRateEnd);

			int nRateStart = PingOutput.ReverseFind('(');
			PingOutput=PingOutput.Right(nRateEnd-nRateStart-1);

			PingOutput_Rate=atoi(PingOutput);
			if(PingOutput_Rate==0)
			{
				PingOutput_Rate_Zero_Time++;
			}
			Try_Time++;
			Sleep(200);
		}while( ((Try_Time<=3)&&(PingOutput_Rate_Zero_Time!=1))  );

		if(PingOutput_Rate==0)
		{
			/*SendIP_Ping(6);
			SendIP_Ping(0);
			SendIP_Ping(5);
			PingOutput.Replace("\r","");
			PingOutput.Replace("\r","");
			PingOutput.Replace(" ","");
			int IP_DB_Type = PingOutput.Find("Type");
			PingOutput=PingOutput.Mid(IP_DB_Type);
			int IP_DB = PingOutput.Find(m_server_M);
			PingOutput=PingOutput.Mid(IP_DB+m_server_M.GetLength());
			//PingOutput=PingOutput.Left(32)
			PingOutput=PingOutput.Left(17);
			//PingOutput="07-23-"+PingOutput.Left(6)+"27-88-"+PingOutput.Right(11)+"-0F-02-36";
			if(ServerIP_MAC_M==PingOutput)
			{
				AfxMessageBox("授权成功连接服务器！");
			}*/
			//SendIP_Ping(7);//确认时间限制
			//system(szPath+"C:\\limit.vbs");
			//WinExec("C:\\limit.vbs",SW_SHOW);


			/*SHELLEXECUTEINFO shei;
			shei.cbSize=sizeof(shei);
			shei.fMask=SEE_MASK_INVOKEIDLIST;
			shei.lpDirectory="C:";
			shei.lpFile="limit.vbs";
			shei.lpVerb="open";
			ShellExecute(NULL,shei.lpVerb,shei.lpFile,"",shei.lpDirectory,SW_SHOW);*/

			//
			/*if(ChangeEN_Choose_M=="true")      //使用次数限制屏蔽
			{
				LoadResult_Time();//获取服务器信息USE_Times_Count
				CString T_SERVER_temp;
				T_SERVER_temp=T_SERVER.Left(4);
				if((atoi(T_SERVER_temp)<2015)&&(T_SERVER_temp!="")&&(USE_Times_Count_SERVER_CS!="")&&( atoi(USE_Times_Count_SERVER_CS)-1000>0 ))
					OnTimerFunction(26);
				else
				{
					AfxMessageBox("该电脑没有权限使用！！请联系当事人:李，邮箱：lbcfyly@126.com   QQ:250626555");
					OnOK();//此举让软件无法打开
				}
			}
			else*/
				OnTimerFunction(26);

		}
		else 
		{
			AfxMessageBox("网络故障，无法授权！");
			/*Software_Version_Check_Choose_M="false";
			IMEI_MAC_Check_Choose_M="false";
			IMEI_CheckValid_Choose_M="false";
			AutoIMEI_Choose_M="false";
			AutoBTAddr_Choose_M="false";
			AutoWIFIAddr_Choose_M="false";*/
			INITIAL_POWER=false;//网络故障，无法授权
		}
		break;
	case 26:
		/////////////////////////////////////////////////////////////////////////////
		// CNBTSTATDlg dialog
		//--------------------------------var-------------------------------
		BYTE bs[50]={0x0,0x00,0x0,0x10,0x0,0x1,0x0,0x0,0x0,0x0,
					 0x0,0x0,0x20,0x43,0x4b,0x41,0x41,0x41,0x41,0x41,
					 0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,
					 0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,
					 0x41,0x41,0x41,0x41,0x41,0x0,0x0,0x21,0x0,0x1};     //数据报

		//unsigned char B1[4],B2[4];
		//HANDLE wait_handle;
		//wait_handle=CreateEvent(NULL,true,false,"receive data");//创建处于非触发状态的事件。类型为手动
		

		//获取服务器MAC地址屏蔽
		/*int len = m_UDPSocket.SendTo((void*)bs,50,destPORT,m_server_M,0);//向指定的ip发数据报
		if (len == 50)
		{
			OnReceive();
		}
		else
		{
			AfxMessageBox("网络故障，无法授权！");
			//Software_Version_Check_Choose_M="false";
			//IMEI_MAC_Check_Choose_M="false";
			//IMEI_CheckValid_Choose_M="false";
			//AutoIMEI_Choose_M="false";
			//AutoBTAddr_Choose_M="false";
			//AutoWIFIAddr_Choose_M="false";
			INITIAL_POWER=false;           //获取网络服务器的MAC地址失败
			//OnOK();//此举让软件无法打开
		}*/
		m_Result.GetWindowText(LOG_Show_Old); 
		m_Result.SetWindowText("授权成功--->连接数据库成功！！！ \r\n\r\n"+LOG_Show_Old);
		UpdateData(true);  //把中间值赋给变量
		UpdateData(false); //把变量显示在界面
		m_Result.UpdateWindow();
		INITIAL_POWER=true;    //授权成功
		break;
	}
}

void CWriteIMEIDlg::OnReceive()
{	
	BYTE Buf[512];
	memset(Buf, 0, sizeof(Buf));
	CString str,strIP,strHost,strHex,strMac,Host,Group,User;
	UINT dport;
	m_UDPSocket.ReceiveFrom(Buf,512,strIP,dport,0);//接收数据
    //如果接收到的ip为空或者与原来接收到的ip相同，则返回
	if(strIP==(char)NULL)
		return;

	strHost=""; //机器名字
	strHex="";  //MAC地址
	User="?";   //
	Host="\\";
	int tem=0,num=0;
	bool bAdd=true;
	//根据数据报规则取出相应的信息
	int i;
	for(i=57;i<500;i++) //57-72
	{
		//if(Buf[i]==0xcc)
		//break; 
		if (num>3)
			break; 
		if(Buf[i]==0x20)
			bAdd=false;
		if(bAdd)
		{
			str.Format("%c",Buf[i]);
			if(Buf[i]>=' ')
				strHost+=str;

			str.Format("%02x.",Buf[i]);
			strHex+=str;
		}

		if((++tem)%18==0)
		{
            bAdd=true; 
			strHost.TrimRight((char)NULL);
			if(strHost=="")
			{
   				strMac.Delete(17,strMac.GetLength()-17);
				//MAC地址
				num++;
				break;
			}


			if(num==0&&strHost!="")
			{
				//机器名
				Host=strHost;
				num++;
			}
			else
			{
				if(Host!=strHost && num==1&& strHost!="")
				{
					//工作组名
					Group=strHost;
				    num++;
				}
				else 
				{
					if(strHost!=Host&&strHost!=Group&&num==2&&strHost!="")
					{
						User=strHost;
						if(User!="__MSBROWSE__")
						{
							num++;
						}
					}
				}

			}
			
			strMac=strHex;
			strHost="";
			strHex="";
			
		}
		
	}
	if(ServerIP_MAC_M_NoTimes==strMac)
	{
		m_Result.GetWindowText(LOG_Show_Old); 
		m_Result.SetWindowText("授权成功--->连接数据库成功！！！ \r\n\r\n"+LOG_Show_Old);
		UpdateData(true);  //把中间值赋给变量
		UpdateData(false); //把变量显示在界面
		m_Result.UpdateWindow();
		INITIAL_POWER=true;     //服务器MAC地址   等于   设置好的---主函数已经屏蔽
	}
	else
	{

		AfxMessageBox("授权失败！");
		/*Software_Version_Check_Choose_M="false";
		IMEI_MAC_Check_Choose_M="false";
		IMEI_CheckValid_Choose_M="false";
		AutoIMEI_Choose_M="false";
		AutoBTAddr_Choose_M="false";
		AutoWIFIAddr_Choose_M="false";*/
		INITIAL_POWER=false;           //服务器MAC地址   不等于   设置好的---主函数已经屏蔽
		//OnOK();//此举让软件无法打开
	}
	//触发事件，导致线程函数的继续执行
	//SetEvent(wait_handle);
	//AfxMessageBox(strMac);
	//AfxMessageBox(Host);
	//AfxMessageBox(Group);
	//AfxMessageBox(User);
}

//功能机META连接----对比
METAAPP_RESULT CWriteIMEIDlg::Initialization(void)
{
	
	META_Connect_Req META_connect_req;
	int bootstop;
	Bool bAntennaOn = 0; 

	char BB_Chip_Type[12]="MT6250";  	   // to solve Global char pointer issue
	BBCHIP_TYPE bb_chip=AUTO_DETECT_BBCHIP;//MT6250
	
	META_Connect_Report META_connect_report;


	unsigned short us_com_port;
    META_ConnectByUSB_Req req;
    META_ConnectByUSB_Report report;

	char usb_ifo[128];
	META_RESULT MetaResult = META_SUCCESS;
    char StrBuf[512];
    int META_Retry = 2;
	int EnterCount = 0;

	RfMsCapabilityEx3_S m_sRfMsCapabilityEx3;
	RfMsCapabilityEx3_REQ_S Cap_req;


	Brom_DebugOn();//显示log//Brom_DebugOff();
	
	META_DebugOn();//显示log//META_DebugOff;
	
	int debug=0;
	MetaAppResult = METAAPP_GetAvailableHandle(&HandleForMETA);
	if(MetaAppResult!=METAAPP_SUCCESS)
	{
		if(debug==1)
			AfxMessageBox("获取META句柄失败！");
	}
	MetaAppResult = METAAPP_Init(HandleForMETA);
	if(MetaAppResult!=METAAPP_SUCCESS)
	{	
		if(debug==1)
		
			AfxMessageBox("初始化METAAPP失败！");
	}
	MetaAppResult = METAAPP_NVRAM_Init(HandleForMETA,"C:\\M255\\BPLGUInfoCustomAppSrcP_MT6250_S00_R255_V1_0_0B01");
	if(MetaAppResult!=METAAPP_SUCCESS)
	{
		if(debug==1)
			AfxMessageBox("初始化NVRAM失败！");
	}
	
    ////////////////////////////////////////////////////////
    //--------------------Determine chipset type for Connect with Target-------------------------------------
    memset (&META_connect_req, 0x0, sizeof(META_connect_req));
    //---------------------Connect with target ----------------------------------
    //------- check RS232 or USB----------------
	m_Port.GetWindowText(m_bUsbMode);
	CString m_bUsbMode_temp=m_bUsbMode.Right(m_bUsbMode.GetLength()-3);
	META_connect_req.com_port = atoi(m_bUsbMode_temp);
    if (m_bUsbMode!="USB")
    {
        META_connect_req.boot_meta_arg.m_usb_enable = _FALSE;  //使用串口
    }
    else
    {
        META_connect_req.boot_meta_arg.m_usb_enable = _TRUE;
    }

    META_connect_req.boot_meta_arg.m_bbchip_type = bb_chip;
    META_connect_req.boot_meta_arg.m_ext_clock = AUTO_DETECT_EXT_CLOCK;
    META_connect_req.baudrate[0] = META_BAUD115200;
    META_connect_req.baudrate[1] = META_BAUD230400;
    META_connect_req.baudrate[2] = META_BAUD460800;
    META_connect_req.baudrate[3] = META_BAUD921600;
    META_connect_req.baudrate[4] = META_BAUD57600;
    META_connect_req.baudrate[5] = META_BAUD_END;
    META_connect_req.baudrate[6] = META_BAUD2400;
    META_connect_req.baudrate[7] = META_BAUD2400;
    META_connect_req.baudrate[8] = META_BAUD2400;
    META_connect_req.baudrate[9] = META_BAUD2400;
    META_connect_req.baudrate[10] = META_BAUD2400;
    META_connect_req.flowctrl = META_SW_FLOWCTRL;
    META_connect_req.ms_connect_timeout = 30000;
    META_connect_req.boot_meta_arg.m_ms_boot_timeout = 10000;
    META_connect_req.boot_meta_arg.m_scert_handle = NULL;
    bootstop = 0;
    //meta_init_com = 0;

#if defined (SecuritySource)
    
    META_connect_req.boot_meta_arg.m_cb_in_brom_stage = NULL;                  //security level1
    META_connect_req.boot_meta_arg.m_cb_in_brom_stage_arg = NULL;
    
    META_connect_req.boot_meta_arg.m_speedup_brom_baudrate = _TRUE;            // security level2 //115200
    META_connect_req.boot_meta_arg.m_ready_power_on_wnd_handle = NULL;
    META_connect_req.boot_meta_arg.m_ready_power_on_wparam = NULL;
    META_connect_req.boot_meta_arg.m_ready_power_on_lparam = NULL;
    META_connect_req.boot_meta_arg.m_auth_handle = Get_AuthHandle();
    META_connect_req.boot_meta_arg.m_cb_sla_challenge = SLA_Challenge;         //SLA_Challenge(NULL,NULL,0,NULL,NULL);
    META_connect_req.boot_meta_arg.m_cb_sla_challenge_arg = NULL;
    META_connect_req.boot_meta_arg.m_cb_sla_challenge_end = SLA_Challenge_END; // NULL;
    META_connect_req.boot_meta_arg.m_cb_sla_challenge_end_arg = NULL;
#endif   

    if (1)//(EDGE_PCCARD==0)// EDGE PC card
    {
        META_connect_req.boot_meta_arg.m_cb_com_init_stage = NULL;
    }
    else
    {
        //META_connect_req.boot_meta_arg.m_cb_com_init_stage = cb_PowerOnOxford;
    }
    META_connect_req.boot_meta_arg.m_cb_com_init_stage_arg = NULL;
    //-----------  Enter into META until OK -------------------------
    // Manually power on the DUT
	do
	{
		/*if(StopSignal==true)//多线程运行时
		{
			StopSignal=false;
			return METAAPP_INIT_FAIL;
		}*/
        if(META_connect_req.com_port!=USB_COM_PORT)
        {
			//AfxMessageBox("请重新给话机 上电！");
			int Meta_In=0;
			do
			{
				/*if(StopSignal==true)//多线程运行时
				{
					StopSignal=false;
					return METAAPP_INIT_FAIL;
				}*/
				Meta_In++;
				if(Meta_In==3)
				{
					break;//AfxMessageBox("手机无法进入META模式，请测试下一部手机！");
				}
				MetaAppResult = METAAPP_ConnectWithTarget(HandleForMETA, &META_connect_req, &bootstop, &META_connect_report);  ///ADDYCG 0815
			}   while(MetaAppResult != METAAPP_NVRAM_DB_INCONSISTENT&&MetaAppResult != METAAPP_SUCCESS);
			if(MetaAppResult==METAAPP_NVRAM_DB_INCONSISTENT)
			{
				if(debug==1)					AfxMessageBox("MetaAppResult==METAAPP_NVRAM_DB_INCONSISTENT");
			}
			else if(MetaAppResult == METAAPP_SUCCESS)
			{
				if(debug==1)					AfxMessageBox("MetaAppResult== METAAPP_SUCCESS");
			}
			else
			{
				if(debug==1)					AfxMessageBox("手机无法进入META模式!");
			}
		}
		else//USB通信方式
		{
			//-------------------------------------------------------------------------------------
			int DataCardSwitchToolOn=0;

			int Meta_In=0;
			do
			{
				/*if(StopSignal==true)//多线程运行时
				{
					StopSignal=false;
					return METAAPP_INIT_FAIL;
				}*/
				Meta_In++;
				if(Meta_In==3)
				{
					break;//AfxMessageBox("手机无法进入META模式，请测试下一部手机！");
				}
				if(DataCardSwitchToolOn)//数据卡开关
				{
            		MetaResult = META_GetDynamicUSBComPortExWithFilter_r(HandleForMETA, META_connect_req.ms_connect_timeout, &us_com_port, &bootstop, StrBuf, 512, usb_ifo);
				}
				else
				{
            		MetaResult = META_GetDynamicUSBComPort_r(HandleForMETA, META_connect_req.ms_connect_timeout, &us_com_port, &bootstop);
				}
			}   while(MetaResult != METAAPP_NVRAM_DB_INCONSISTENT&& MetaResult != METAAPP_SUCCESS);

			if(MetaResult !=METAAPP_SUCCESS) 
				continue;//继续下一次连接手机循环

			req.com_port = us_com_port;  //us_com_port  选用usb方式获取的串口
			req.ms_connect_timeout = META_connect_req.ms_connect_timeout;
			req.boot_meta_arg = META_connect_req.boot_meta_arg;
			req.boot_meta_arg.m_usb_enable = _TRUE;
			MetaResult = META_ConnectWithTargetByUSB_r(HandleForMETA, &req, &bootstop, &report);
			if ((report.boot_meta_ret != S_DONE) &&
					(report.boot_meta_ret != S_STOP) &&
					(report.boot_meta_ret != S_BROM_FAIL_TO_GET_BBCHIP_HW_VER) )
			{
				//sprintf (StrBuf, "\n =====WARNING!! USB Connect Fail =====\n");
			}
			else
			{
				META_connect_report.boot_result.m_bbchip_hw_code                 = report.boot_result.m_bbchip_hw_code;
				META_connect_report.boot_result.m_bbchip_hw_ver                  = report.boot_result.m_bbchip_hw_ver;
				strcpy(META_connect_report.boot_result.m_bbchip_name ,          report.boot_result.m_bbchip_name);

				META_connect_report.boot_result.m_bbchip_secure_ver				 = report.boot_result.m_bbchip_secure_ver;
				META_connect_report.boot_result.m_bbchip_sw_ver                  = report.boot_result.m_bbchip_sw_ver;
				META_connect_report.boot_result.m_bbchip_type                    = report.boot_result.m_bbchip_type;
				META_connect_report.boot_result.m_ext_clock                      = report.boot_result.m_ext_clock;
				META_connect_report.boot_result.m_msp_err_code                   = report.boot_result.m_msp_err_code;
			}

			if (MetaResult == META_MAUI_DB_INCONSISTENT)
			{
				MetaAppResult = METAAPP_NVRAM_DB_INCONSISTENT;
			}
			else if (MetaResult == META_SUCCESS)
			{
				MetaAppResult = METAAPP_SUCCESS;
			}
			else
			{
				MetaAppResult = METAAPP_BOOT_FAIL;  ///DB_INCONSISTENT
			}
			//----------------------------------------------------------------
		}//USB连接模式结束

		if(MetaAppResult == METAAPP_SUCCESS)
		{
			break;
		}
       
        if (MetaAppResult == METAAPP_NVRAM_DB_INCONSISTENT)
        {
            MetaAppResult = METAAPP_SUCCESS;
			break;
            //sprintf(StrBuf, "\n =====WARNING!! METAAPP_NVRAM_DB_INCONSISTENT =====\n");           
        }

		//bb_chip = META_connect_report.boot_result.m_bbchip_type ;//获取芯片型号
		if (MetaAppResult != METAAPP_SUCCESS)
		{
			//Connect Target failed, com port must be closed//meta_init_com = 0;
			MetaResult = META_DisconnectInMetaMode_r(HandleForMETA);
			//MetaResult = METAAPP_DisconnectWithTarget(HandleForMETA);
			//METAAPP_DeInit(HandleForMETA);
			EnterCount++;
		}
		
	} while(EnterCount<META_Retry);//循环通过META模式连接话机

	if(EnterCount==META_Retry)
	{
		//AfxMessageBox("手机META模式进入失败，请重新开始！");
		return METAAPP_INIT_FAIL;
	}
	

	MetaResult = META_QueryIfFunctionSupportedByTarget_r(HandleForMETA, 500, "META_Rf_QueryMSCapabilityEx3_r");
	if(META_SUCCESS == MetaResult)
	{                   
		Cap_req.calibrationItemsSize = sizeof(m_sRfMsCapabilityEx3.calibrationItems);
		Cap_req.capabilityItemsSize = sizeof(m_sRfMsCapabilityEx3.capabilityItems);
		META_Rf_QueryMSCapabilityEx3_r(HandleForMETA, 300, &Cap_req, &m_sRfMsCapabilityEx3);
	}
	else
	{
		MetaResult = META_DisconnectInMetaMode_r(HandleForMETA);
		AfxMessageBox("该手机不支持META模式，请重新测试或者测试下一部话机！");
		return METAAPP_INIT_FAIL;
	}
	////////////////////////////////////////////////////////////////////////////////////////////
       ///strcpy (StrBuf, "\n\t Enter into META Mode OK");////////////////////////////////////////
	//------------------------if enter meta mode successful ,and then follow up-----------
	////////////////////////////////////////////////////////////////////////////////////////////
    return MetaAppResult;

	/*else if(dual_talk_enable) // Add dual talk,2012.6.19
	{
		// step1: Get Preloader Port
		char asPreloaderPortFilter[128];  
		MetaResult = META_GetDynamicUSBComPortExWithFilter_r(HandleForMETA, META_connect_req.ms_connect_timeout, &us_com_port, &bootstop, StrBuf, 512, asPreloaderPortFilter);
	
		// step2: Switch to Modem
		if (EBOOT_SUCCESS != SP_SwitchModem(us_com_port, 115200, p_stop_flag))
		{
	    	if(debug==1)
				AfxMessageBox("Switch To Modem Fail!");
		}
	
		// step3: Get Modem Brom Port
		MetaResult = META_GetDynamicUSBComPortExWithFilter_r(HandleForMETA, META_connect_req.ms_connect_timeout, &us_com_port, &bootstop, StrBuf, 512, asBROMPortFilter);

		// All OK
		sprintf (StrBuf, "\n =======OK=======\n");
	}*/
}


//智能机META连接----对比
int CWriteIMEIDlg::PowerOn2Connect_SP(int WorkingMS) // Smart phone version of PowerOn2Connect
{
	/*typedef enum
	{
		PreloaderUSB = 0,
		BootROMUSB = 1
	}USBType;*/
	META_Connect_Report META_connect_report;

    int enter_count = 0;

    unsigned short us_com_port;
    METAAPP_RESULT MetaAppResult = METAAPP_SUCCESS;
    int HandleForMETA = 0;


    int META_Retry = 5;
    int stopflag = 0;
    //***********************************************************************
    int ReturnValue = 999;
    META_RESULT MetaResult = META_SUCCESS;
    int MetaHandle = 0;

    META_ConnectInMETA_Req BootRequest;
    int BootStop;
    META_ConnectInMETA_Report BootReport;
    

    char * m_exist_symbolic_set[20]= {"\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0"};
    char *m_exist_port_set[20]= {"\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0"};

    int device_num = 0;
    int loopIndex = 0;
    int temp_com_port = 0;
	
	//static const GUID GUID_PORT_CLASS_USB2SER ={0x4D36E978L,0xE325,0x11CE,{0xBF,0xC1,0x08,0x00,0x2B,0xE1,0x03,0x18}};      

    META_ConnectByUSB_Req usbRequest;
    META_ConnectByUSB_Report usbReport;

	//########### META connection flow ###############
	SP_BOOT_ARG_S stArg;
	int iRet;
	USBType eType;

	char * ppFilter[2];

	SP_COM_FILTER_LIST_S sCOMFilter;
	SP_COM_PROPERTY_S sCOMProperty;

    memset (&sCOMFilter, 0x0, sizeof(SP_COM_FILTER_LIST_S));
    memset (&usbRequest, 0x0, sizeof(META_ConnectByUSB_Req));
    memset (&usbReport, 0x0, sizeof(META_ConnectByUSB_Report));
	memset (&stArg, 0x0, sizeof(SP_BOOT_ARG_S));


    //--------------------Determine chipset type for Connect with Target---------------------------------------
	m_Port.GetWindowText(m_bUsbMode);
	CString m_bUsbMode_temp=m_bUsbMode.Right(m_bUsbMode.GetLength()-3);
	temp_com_port = atoi(m_bUsbMode_temp);
    if (m_bUsbMode=="USB")
    {
        temp_com_port=USB_COM_PORT; //使用USB口
    }

    //---------------------Connect with target ----------------------------------
    BootRequest.com_port = temp_com_port;//MMS_INFO[WorkingMS].ComPort;
    // [META] META stage baudrate polling array, it must end with META_BAUD_END.
    BootRequest.baudrate[0] = META_BAUD115200;
    BootRequest.baudrate[1] = META_BAUD230400;
    BootRequest.baudrate[2] = META_BAUD460800;
    BootRequest.baudrate[3] = META_BAUD921600;
    BootRequest.baudrate[4] = META_BAUD57600;
    BootRequest.baudrate[5] = META_BAUD_END;

    BootRequest.flowctrl = META_SW_FLOWCTRL;
    BootRequest.ms_connect_timeout = 60000;  //30000;

	//Old parameters
	    //stArg.m_bbchip_type = bb_chip;
		stArg.m_bbchip_type = SP_AUTO_DETECT_BBCHIP;
	    stArg.m_ext_clock = SP_AUTO_DETECT_EXT_CLOCK;
	    stArg.m_ms_boot_timeout = SP_BOOT_INFINITE;                                //不能是  BOOT_STOP----对比
	    stArg.m_max_start_cmd_retry_count = SP_DEFAULT_BROM_START_CMD_RETRY_COUNT;

   //New parameters
	    stArg.m_uTimeout = 20000;
	    stArg.m_uRetryTime = 2000;
	    stArg.m_uInterval = 10;
	    stArg.m_uBaudrate = CBR_115200;

	    //=============================
   // Serial Link Authentication 
	    //stArg.m_auth_handle = SPATE_Get_AuthHandle();
	    //stArg.m_scert_handle = SPATE_Get_ScertHandle();
	    stArg.m_cb_sla_challenge = SLA_Challenge;
	    stArg.m_cb_sla_challenge_arg = NULL;
	    stArg.m_cb_sla_challenge_end = SLA_Challenge_END;
	    stArg.m_cb_sla_challenge_end_arg = NULL;
	
		//1. Get META arguments
		stArg.m_pStopFlag = &BootStop;
		BootStop = SP_BOOT_INFINITE;                                         //不能是  SP_BOOT_STOP----对比
		stArg.m_bIsUSBEnable = temp_com_port == USB_COM_PORT?1:0;
		stArg.m_bIsSymbolicEnable = false;
		bool m_isCompositeUSB=1;                         //可配置          
		stArg.m_bIsCompositeDeviceEnable=m_isCompositeUSB;
	

		stArg.m_euBootMode = SP_META_BOOT;

    //Eboot_DebugOn();
    //-----------  Enter into META until OK -------------------------
    do
    {
		// Manually power on the DUT
		//2. Set COM port for META connection
		if(temp_com_port != USB_COM_PORT)
		{
			//Use UART port, unnecessary to scan
			stArg.m_uPortNumber = temp_com_port;
			eType = PreloaderUSB;
		}
		else
		{	
			/*
			BROM Port Filter = VID_0E8D&PID_0003&REV_0100
			Preloader Port Filter = VID_0E8D&PID_2000&REV_0100
			Kernel Port Filter = VID_0BB4&PID_0005&REV_0255
			*/

			/*INI文件的设置
			BROM Port Filter = "VID_0E8D&PID_0003"
			Preloader Port Filter = "VID_0E8D&PID_2000"
			Kernel Port Filter = "VID_0BB4&PID_0005&REV_0255"
			*/
			ppFilter[0] = "VID_0E8D&PID_2000";//asPreloaderPortFilter;
			ppFilter[1] = "VID_0E8D&PID_0003";//asBROMPortFilter;
			int LegacyAutoDetection=1;        //可配置
			sCOMFilter.m_uCount = (LegacyAutoDetection) ? 1: 2;
			sCOMFilter.m_eType=SP_WHITE_LIST;
			sCOMFilter.m_ppFilterID = ppFilter;
			
			iRet = SP_GetIncrementCOMPortWithFilter(&sCOMFilter, &sCOMProperty, NULL, true,&BootStop,30); 
			if(0 == iRet)
			{
				switch(sCOMProperty.m_iFilterIndex)
				{
					case 0:
						eType = PreloaderUSB;
						break;
					case 1:
						eType = BootROMUSB;
						break;
					default:
						//Application->MessageBox("Filter index error!", "Fail", MB_OK);
						continue;
				}
	
				stArg.m_uPortNumber = sCOMProperty.m_uNumber;
			}
			else
			{
				 //Application->MessageBox("Search new COM port fail!", "Fail", MB_OK);
				 continue;
			}
		}

 	       
		//3. Do META connection
		if(BootROMUSB == eType)
		{	   
			iRet = SP_BootROM_BootMode(&stArg);
		}
		else if(PreloaderUSB == eType)
		{
			iRet = SP_Preloader_BootMode(&stArg);
		}
		

        if(iRet != SP_S_DONE)//1)
        {
            //SP DUT not boot into META
            MetaResult = META_FAILED;
            //enter_count = META_Retry;//退出连接手机，停止再次尝试
			enter_count++;
        }
        else
        {
            if (temp_com_port == USB_COM_PORT)
			{
				memset (&sCOMFilter, 0x0, sizeof(SP_COM_FILTER_LIST_S));
				memset (&sCOMProperty, 0x0, sizeof(SP_COM_PROPERTY_S));
				BootStop=SP_BOOT_INFINITE;                                //不能是  SP_BOOT_STOP----对比

				//ppFilter[0] = asPreloaderPortFilter;//已定义
			    //ppFilter[1] = asBROMPortFilter;     //已定义
				sCOMFilter.m_uCount = 2;         
				//sCOMFilter.m_ppFilterID = ppFilter; //已定义
				sCOMFilter.m_eType=SP_BLACK_LIST;
				 
				int SearchKernelPortTimeOut=35;          
				iRet = SP_GetIncrementCOMPortWithFilter(&sCOMFilter, &sCOMProperty, &GUID_PORT_CLASS_USB2SER, 0, &BootStop, SearchKernelPortTimeOut);
				switch(iRet)
				{
					case SP_S_DONE:
						us_com_port = sCOMProperty.m_uNumber;							   
						break;
					case SP_S_STOP:					
						//search_kernel_port_success=0;
						continue;
				
					case SP_S_TIMEOUT:
					{
						continue;//MessagePopup("Search Kernel USB COM port timout!", "Fail");
					}
					default:
					{
						continue;//MessagePopup("Search Kernel USB COM port fail!", "Fail");
					 }
				}

                if (iRet == SP_S_DONE)
                {
                    //MMS_INFO[WorkingMS].ComPort = us_com_port;
                    usbRequest.com_port = us_com_port;
                    usbRequest.ms_connect_timeout = 80000;
                    MetaResult = META_ConnectInMetaModeByUSB_r (HandleForMETA, &usbRequest, &BootStop, &usbReport);
                }
			}
			else
			{
				MetaResult = META_ConnectInMetaMode_r(HandleForMETA, &BootRequest, &BootStop, &BootReport);
			}
			if (MetaResult == META_MAUI_DB_INCONSISTENT)
			{
				MetaResult = META_SUCCESS;
				//sprintf(StrBuf, "\n =====WARNING!! META_MAUI_DB_INCONSISTENT =====\n");
			}
		}

    }
    while(enter_count < META_Retry );

    if (MetaResult != META_SUCCESS)
    {
        //Connect Target failed, com port must be closed
        METAAPP_DisconnectWithTarget(HandleForMETA);     
		return -1;
    }
    META_connect_report.final_baudrate 				= BootReport.final_baudrate;				// [META] Detected META stage baudrate.
    META_connect_report.meta_ver_required_by_target = BootReport.meta_ver_required_by_target;	// [META] Target required META_DLL version.

	//VerInfo_Cnf Ver_Info_Cnf;
    //MetaAppResult = METAAPP_GetTargetVerInfo(HandleForMETA, 10000, &Ver_Info_Cnf);
    //------------------------if enter meta mode successful ,and then follow up-----------

     return 0;
}         





//初始化Modem
bool CWriteIMEIDlg::MetaModemHandleInit () 
{
	META_DebugOn();
	Brom_DebugOn();
	SP_Brom_DebugOn();
	if ( METAAPP_GetAvailableHandle ( &g_Meta.iMeta_handle) )
	{
		//MessagePopup ("WARNING", "Get Meta handle fail!");
		return true;
	} 
	if ( METAAPP_Init( g_Meta.iMeta_handle) )
	{
		//MessagePopup ("WARNING", "Init Meta handle fail!");
		return true;
	}
	return false;
}


//////////////////////////////////////////////////
//////////////////////////////////////////////////
//初始化MetaAP
static void cbMETA_Init ( META_CNF_ERR_CODE  mr )       //MetaAPHandleInit子函数
{
	return;
}
bool CWriteIMEIDlg::MetaAPHandleInit () 
{

	if ( SP_META_GetAvailableHandle ( &g_Meta.iMetaAP_handle) )
	{
		//MessagePopup ("WARNING", "Get Meta handle fail!");
		return true;
	} 
	if ( SP_META_Init_r ( g_Meta.iMetaAP_handle, (META_Error_CallBack) cbMETA_Init ))
	{

		//MessagePopup ("WARNING", "Init Meta handle fail!");
		return true;
	}
	return false;

}
////////////////////////////////////////////
////////////////////////////////////////////
//初始化Authentication
bool CWriteIMEIDlg::REQ_AUTH_Create( void )            //AuthHandleInit子函数
{
    int ret;
    ret = AUTH_Create( &g_Meta.t_AuthHandle );
    if( ret != 0 )
        return true;    
	else
		return false;   
}
bool CWriteIMEIDlg::REQ_AUTH_Load(const char *filepath )//AuthHandleInit子函数
{
    int ret;
    if( g_Meta.t_AuthHandle == NULL)
        return true;
    ret = AUTH_Load( g_Meta.t_AuthHandle, filepath );
    if( ret != 0 )
        return true;    
	else
		return false;       
}
bool CWriteIMEIDlg::AuthHandleInit () 
{
	if ( REQ_AUTH_Create () ) 
	{
		//MessagePopup ("WARNING", "Init Authentication handle fail!");
		return true;
	}	
	REQ_AUTH_Load ( "" );//路径可无=====功能机
	return false;

}

//////////////////////////////////////////
/////////////////////////////////////////
//初始化SP Authentication
bool CWriteIMEIDlg::SPATE_AUTH_Create( void )          //SpAuthHandleInit子函数
{
    int ret = SP_AUTH_Create( &SPATE_m_sAuthHandle );
    if( ret != 0 )
        return true;
	else
		return false;
}
bool CWriteIMEIDlg::SPATE_AUTH_Load( const char *auth_filepath )
{
	int ret;
    if( NULL == SPATE_m_sAuthHandle )
        return true;
    ret = SP_AUTH_Load( SPATE_m_sAuthHandle, auth_filepath );
    if( ret != 0 )
        return true;
	else
		return false;
}

bool CWriteIMEIDlg::SpAuthHandleInit () 
{
	if ( !SPATE_AUTH_Create () ) 
	{
			//MessagePopup ("WARNING", "Init SP Authentication handle fail!");
			return true;
	}	
	SPATE_AUTH_Load ( "");//路径可无====智能机
	return false;

}
////////////////////////////////////////
///////////////////////////////////////
//初始化SPATE Authentication
/*bool CWriteIMEIDlg::SPATE_AUTH_Create( void )                  //SpSecuHandleInit子函数
{
    int ret = SP_AUTH_Create( &SPATE_m_sAuthHandle );
    if( ret != 0 )
        return false;
	else
		return true;
}
bool CWriteIMEIDlg::SPATE_AUTH_Load( const char *auth_filepath )//SpSecuHandleInit子函数
{
	int ret;
    if( NULL == SPATE_m_sAuthHandle )
        return true;
    ret = SP_AUTH_Load( SPATE_m_sAuthHandle, auth_filepath );
    if( ret != 0 )
        return true;
    return false;
}*/
bool CWriteIMEIDlg::SpSecuHandleInit () 
{
	if ( !SPATE_AUTH_Create () ) 
	{
		//MessagePopup ("WARNING", "Init Authentication handle fail!");
		return true;
	}	
	SPATE_AUTH_Load ( "");//路径可无===智能机
	return false;

}
///////////////////////////////////
///////////////////////////////////
//META相关内容初始化总函数
void CWriteIMEIDlg::MetaContextInit (void)
{
	g_Meta.bIsConnected = false;
	//g_Meta.bootstop = BOOT_STOP;                        //只适用于功能机=================智能机---这样不能进入META模式
	//g_Meta.bootstop = BOOT_INFINITE;              
	g_Meta.bAuthenEnable = false;
	g_Meta.bIsDatabaseInitialized [0] = false;
	g_Meta.bIsDatabaseInitialized [1] = false;
	MetaModemHandleInit ();
	MetaAPHandleInit ();
	AuthHandleInit ();
	SpAuthHandleInit ();
	SpSecuHandleInit ();
}

//////////////////////////////////
//////////////////////////////////
//META相关内容初始化清理子函数
int CWriteIMEIDlg::REQ_AUTH_Unload( void )
{
    int ret; 
    if( g_Meta.t_AuthHandle == NULL)
        return -1;
    ret = AUTH_Unload( g_Meta.t_AuthHandle );
    if( ret != 0 )
        return -1;   
	else
		return 0;    
}
//META相关内容初始化清理子函数
int CWriteIMEIDlg::REQ_AUTH_Destroy( void )
{
    int ret; 
    if( g_Meta.t_AuthHandle == NULL)
        return -1;
    ret = AUTH_Destroy( &g_Meta.t_AuthHandle );
    if( ret != 0 )
       return -1; 
	else
		return 0;   
}
//META相关内容初始化清理
void CWriteIMEIDlg::MetaContextDeInit (void)
{
	REQ_AUTH_Unload ();
    REQ_AUTH_Destroy ();
	METAAPP_DeInit ( g_Meta.iMeta_handle );   
	SP_META_Deinit_r( &g_Meta.iMetaAP_handle );   
}
//////////////////////////////////
/////////////////////////////////
//功能机写IMEI号总函数
void CWriteIMEIDlg::swap_c ( unsigned char* x, unsigned char* y )
{
	unsigned char z;
	z = *x;
	*x = *y;
	*y = z;

}
//MTK平台软硬件版本的全局变量
CString S_HW_Version;//软硬件版本
static void __stdcall CNF_Read_Barcode(const VerInfo_Cnf *cnf, const short token, void *usrData)//if((Software_Version_Check_Choose_M=="true")) 
{
	S_HW_Version.Format("%s",cnf->SW_VER);
}


void CWriteIMEIDlg::TranslateMuch(CString m_old) 
{
	// TODO: Add extra validation here
	MD5 mb;
	//UpdateData();
	mb.update(m_old,strlen(m_old));	
	Unlock_code_Number=(mb.toString()).c_str();

	Unlock_code_Number=Unlock_code_Number.Right(atoi(Unlock_code_M));

	char *pStr =  (LPSTR)(LPCTSTR)Unlock_code_Number; //转换为字符数组
	int string_sn_int[12]; 

	CString string_sn_int_r,string_sn_int_r1;
	string_sn_int_r1="";
	for(int i=0;i<atoi(Unlock_code_M);i++)
	{
		if((pStr[i]<='9')&&(pStr[i]>='0'))                    //0~9之间的字符
		{
            string_sn_int[i]=pStr[i]-'0';
			//temp[i]='0'+string_sn_int[i];
		}
        else if((pStr[i]<='F')&&(pStr[i]>='A'))               //A~F之间的字符
		{
            string_sn_int[i]=pStr[i]-'A'+10;
			//temp[i]='0'+string_sn_int[i];
		}
        else if((pStr[i]<='f')&&(pStr[i]>='a'))               //a~f之间的字符
            string_sn_int[i]=pStr[i]-'a'+10;
		if(string_sn_int[i]>9)
			string_sn_int[i]=string_sn_int[i]-10;

		string_sn_int_r.Format("%d",string_sn_int[i]);
		string_sn_int_r1=string_sn_int_r1+string_sn_int_r;
		//AfxMessageBox(string_sn_int[i]);
	}

	Unlock_code_Number=string_sn_int_r1;
}



DWORD  CWriteIMEIDlg::func_imei_meta_hdlr_feature_phone(WPARAM, LPARAM)
{
	extern COLORREF gColor;
	extern CString gFinalResult;

	#define BARCODE_MAX_LENGTH  64 
	IMEISV_struct_T tIMEIScanBack,tImei_Svn_Pad;
	//unsigned char strPad [4], strSvn [4];
	unsigned char i;
	
	unsigned int status;

	unsigned short int p_spare=0, iLoop = 0;

	bool IMEI_Write_Result=true;
	if ( IsNvramDatabaseInit () )
	{
		//sprintf ( strLog, "   Error: Init feature phone Database failed.\n");
		IMEI_Write_Result=false;
		m_Result.GetWindowText(LOG_Show_Old);  
		m_Result.SetWindowText("初始化Modem database 参数失败！！！\r\n\r\n"+LOG_Show_Old);
		m_Result.UpdateWindow();
		gColor = RGB(255,0,0);
		gFinalResult.Format("     Fail");
		RedrawWindow();  m_Result.UpdateWindow(); 
		IMEI_Control.SetFocus();
		IMEI_Control.SetSel(0,-1);
		//初始化时连接了数据库、每次重连数据库、
		/*if(DB_CONNECT_M=="true")//数据库关闭处理
		{
			Setdlg.myado.CloseDataLibrary();//连接关闭必须在一个子函数内，不然出错
		}*/
		return 1;
	}
	else
	{
		m_Result.GetWindowText(LOG_Show_Old);  
		m_Result.SetWindowText("初始化Modem database 参数成功！！！\r\n\r\n"+LOG_Show_Old);
	}
	//sprintf ( strLog, "   OK: Init feature phone Database successfully.\n");
	SetMetaConnectReq ();
	m_Result.GetWindowText(LOG_Show_Old);  
	m_Result.SetWindowText("完成META相关配置开始与手机通信...\r\n\r\n"+LOG_Show_Old);
	m_Result.UpdateWindow();
	int IMEI_Count_plus=0;
	if(IMEI1_Choose_M=="true")
	{
		IMEI_Count_plus++;
		strcpy(Imei[0],m_IMEI);
	}
	if(IMEI2_Choose_M=="true")
	{
		IMEI_Count_plus++;
		strcpy(Imei[1],m_IMEI2);
	}
	if(IMEI3_Choose_M=="true")
	{
		IMEI_Count_plus++;
		strcpy(Imei[2],m_IMEI3);
	}
	if(IMEI4_Choose_M=="true")
	{
		IMEI_Count_plus++;
		strcpy(Imei[3],m_IMEI4);
	}
	
	//sprintf ( strLog, ">>Step: Start to Connect with target.\n" );	
	if ( EnterMetaMode () )//放在for循环外，是为了MTK功能机只写BT\WIFI地址的时候
	{
		IMEI_Write_Result=false;
		//METAAPP_DisconnectWithTarget( g_Meta.iMeta_handle ); 
		m_Result.GetWindowText(LOG_Show_Old);  
		m_Result.SetWindowText("连接手机失败，退出META模式！！！\r\n\r\n"+LOG_Show_Old);
		//sprintf ( strLog, "   Error: Connect failed, Exit Meta Mode and turn to next phone.\n" );
	}
	
	for ( i = 0; i< IMEI_Count_plus; i++ )//N为IMEI个数
	{
		strcpy(tImei_Svn_Pad.imei, Imei[i]);

		if ( 0)
		{	
			m_Result.GetWindowText(LOG_Show_Old);  
			m_Result.SetWindowText("连接手机成功，读取SVN、PAD...\r\n\r\n"+LOG_Show_Old);
			m_Result.UpdateWindow();
			//sprintf ( strLog, ">>Step: To get SVN and PAD from phone.\n" );				
			if ( METAAPP_readIMEIfromNVRAM_Ex3 ( g_Meta.iMeta_handle, 5000, i+1, &tIMEIScanBack))
			{
				IMEI_Write_Result=false;
				//METAAPP_DisconnectWithTarget( g_Meta.iMeta_handle ); 
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("读取SVN、PAD失败，退出META模式！！！\r\n\r\n"+LOG_Show_Old);
				break;
			}
			tImei_Svn_Pad.pad = tIMEIScanBack.pad ;
			tImei_Svn_Pad.svn = tIMEIScanBack.svn ;
		} 
		else 
		{

			tImei_Svn_Pad.pad = '0';
			tImei_Svn_Pad.svn = '0';
		}

		if (1)//GetIsAutoCheckSum () 
		{
			CString pad_CS,svn_CS;
			/*unsigned char* pad_svn[2];
			pad_svn[0]=&tImei_Svn_Pad.pad;
			pad_svn[1]=&tImei_Svn_Pad.svn;
			pad_CS.Format("%s",pad_svn[0]);
			svn_CS.Format("%s",pad_svn[1]);*/

			m_Result.GetWindowText(LOG_Show_Old);  
			m_Result.SetWindowText("读取SVN、PAD成功！！！\r\n\r\nsvn="+svn_CS+"\r\npad="+pad_CS+"\r\n"+LOG_Show_Old);
			m_Result.UpdateWindow();
			if(IMEI_CheckValid_Choose_M=="true")
			{
				METAAPP_Calcualte_IMEI_Spare ( g_Meta.iMeta_handle, tImei_Svn_Pad.imei, &p_spare);//已经做检查
				tImei_Svn_Pad.imei [14] = p_spare + '0';
			}
		}
					
		/*if ( tImei_Svn_Pad.svn < 10)
		{  
			strSvn[1] = strSvn[0]; 
			strSvn[0] = '0';
			strSvn[2] = '\0';
		}
		swap_c ( &strSvn[0], &strSvn[1]);//交换两个变量*/
		//sprintf ( strLog,  ">>Step: Start to write. CheckSum=%d, IMEILock=%d.\n", GetIsImeiCheckSum (), GetIsImeiLock());
		//g_MetaModem.eMetaApp_Result = METAAPP_writeIMEItoNVRAM_Ex3( g_Meta.iMeta_handle, 5000, GetIsImeiCheckSum (), GetIsImeiLock(), i+1, &tImei_Svn_Pad);
		if(IMEI_CheckValid_Choose_M=="true")
			g_MetaModem.eMetaApp_Result = METAAPP_writeIMEItoNVRAM_Ex3( g_Meta.iMeta_handle, 5000, 1, 0, i+1, &tImei_Svn_Pad);//1、0对应：校验IMEI、不锁IMEI(锁了后需要重烧软件才能再写入)
		else
			g_MetaModem.eMetaApp_Result = METAAPP_writeIMEItoNVRAM_Ex3( g_Meta.iMeta_handle, 5000, 0, 0, i+1, &tImei_Svn_Pad);//0、0对应：不校验IMEI、不锁IMEI(锁了后需要重烧软件才能再写入)

		CString Imei_temp;//显示用处
		Imei_temp.Format("%s",Imei[i]);
		switch ( g_MetaModem.eMetaApp_Result )
    	{
			case METAAPP_SUCCESS:
				//sprintf ( strLog, "    OK: Write IMEI = \"%s\".\n", tImei_Svn_Pad.imei );
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("写IMEI:"+Imei_temp+"号成功！！！\r\n\r\n"+LOG_Show_Old);
				m_Result.UpdateWindow();
				break;
		
			case METAAPP_NVRAM_COMPOSE_IMEI_FAIL:
				//不会出现此情况
				IMEI_Write_Result=false;
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("写IMEI:"+Imei_temp+"号不校验位失败，写号不成功，此情况一般不出现！！！\r\n\r\n"+LOG_Show_Old);
				m_Result.UpdateWindow();
				break;
				
			default: 
				//sprintf ( strLog,  "   Error: METAAPP_writeIMEItoNVRAM_Ex3()!\n" );
				IMEI_Write_Result=false;
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("写IMEI:"+Imei_temp+"号失败！！！\r\n\r\n"+LOG_Show_Old);
				m_Result.UpdateWindow();
				break;	
		}
		
	} // for (;;)
	if((IMEI1_Choose_M=="true")||(IMEI2_Choose_M=="true")||(IMEI3_Choose_M=="true")||(IMEI4_Choose_M=="true"))//有选择写IMEI号，才进行下面操作
	{
		if (IMEI_Write_Result==true )
		{
			g_MetaModem.eMeta_Result = META_QueryIfFunctionSupportedByTarget_r(/*meta_handle*/0, 500, "META_MISC_EX_BackupCalibrationToStorage_r");
			if(METAAPP_SUCCESS == g_MetaModem.eMeta_Result)
			{
				//sprintf ( strLog, ">> Set the SDS\n");
				if(META_MISC_EX_BackupCalibrationToStorage_r(g_Meta.iMeta_handle,5000,0,&status))
				{
					//sprintf ( strLog, "   Error: BackupCalibrationToStorage fail, Exit Meta Mode and turn to next phone.\n" );
					IMEI_Write_Result=false;
					m_Result.GetWindowText(LOG_Show_Old);  
					m_Result.SetWindowText("BackupCalibrationToStorage fail！！！\r\n\r\n"+LOG_Show_Old);
				}
				else
				{
					//sprintf ( strLog, "   OK: BackupCalibrationToStorage OK, Exit Meta Mode and turn to next phone.\n" );
					m_Result.GetWindowText(LOG_Show_Old);  
					m_Result.SetWindowText("BackupCalibrationToStorage OK！！！\r\n\r\n"+LOG_Show_Old);
					m_Result.UpdateWindow();

				}
			}
		}
	}

	////////////////////////////////////


	////////////////////////////////////读取IME检查
	CString m_IMEI_Read;
	CString m_IMEI2_Read;
	CString m_IMEI3_Read;
	CString m_IMEI4_Read;
	for ( i = 0; i< IMEI_Count_plus; i++ )//N为IMEI个数
	{
		g_MetaModem.eMetaApp_Result = METAAPP_readIMEIfromNVRAM_Ex3( g_Meta.iMeta_handle, 5000,i+1, &tImei_Svn_Pad);
		
		CString IMEI_Count_plus_CS;
		IMEI_Count_plus_CS.Format("%d",i+1);//第几个IMEI
		switch ( g_MetaModem.eMetaApp_Result )
    	{
			case METAAPP_SUCCESS:
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("读取第( "+IMEI_Count_plus_CS+" )个IMEI号成功！\r\n\r\n"+LOG_Show_Old);
				m_Result.UpdateWindow();
				break;
		
			case METAAPP_NVRAM_COMPOSE_IMEI_FAIL:
				//不会出现此情况
				IMEI_Write_Result=false;
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("读取第( "+IMEI_Count_plus_CS+" )个IMEI号失败！！！\r\n\r\n"+LOG_Show_Old);
				m_Result.UpdateWindow();
				break;
				
			default: 
				IMEI_Write_Result=false;
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("读取第( "+IMEI_Count_plus_CS+" )个IMEI号失败！！！\r\n\r\n"+LOG_Show_Old);
				m_Result.UpdateWindow();
				break;	
		}

		if(IMEI_Write_Result==true )//读取IMEI成功
		{
			strcpy(Imei[i],tImei_Svn_Pad.imei);
			if(i == 0)
			{
				m_IMEI_Read.Format("%s",Imei[i]);
				if(m_IMEI_Read!=m_IMEI)
				{
					IMEI_Write_Result=false;
					m_Result.GetWindowText(LOG_Show_Old);  
					m_Result.SetWindowText("读取第( "+IMEI_Count_plus_CS+" )个IMEI号:"+m_IMEI_Read+"与输入的IMEI:"+m_IMEI+"不相同=======失败！！！\r\n\r\n"+LOG_Show_Old);
					m_Result.UpdateWindow();
				}
				else
				{
					m_Result.GetWindowText(LOG_Show_Old);  
					m_Result.SetWindowText("读取第( "+IMEI_Count_plus_CS+" )个IMEI号:"+m_IMEI_Read+"与输入的IMEI:"+m_IMEI+"一致=======成功继续...\r\n\r\n"+LOG_Show_Old);
					m_Result.UpdateWindow();
				}
			}
			else if(i == 1)
			{
				m_IMEI2_Read.Format("%s",Imei[i]);
				if(m_IMEI2_Read!=m_IMEI2)
				{
					IMEI_Write_Result=false;
					m_Result.GetWindowText(LOG_Show_Old);  
					m_Result.SetWindowText("读取第( "+IMEI_Count_plus_CS+" )个IMEI号:"+m_IMEI2_Read+"与输入的IMEI:"+m_IMEI2+"不相同=======失败！！！\r\n\r\n"+LOG_Show_Old);
					m_Result.UpdateWindow();
				}
				else
				{
					m_Result.GetWindowText(LOG_Show_Old);  
					m_Result.SetWindowText("读取第( "+IMEI_Count_plus_CS+" )个IMEI号:"+m_IMEI2_Read+"与输入的IMEI:"+m_IMEI2+"一致=======成功继续...\r\n\r\n"+LOG_Show_Old);
					m_Result.UpdateWindow();
				}
			}
			else if(i == 2)
			{
				m_IMEI3_Read.Format("%s",Imei[i]);
				if(m_IMEI3_Read!=m_IMEI3)
				{
					IMEI_Write_Result=false;
					m_Result.GetWindowText(LOG_Show_Old);  
					m_Result.SetWindowText("读取第( "+IMEI_Count_plus_CS+" )个IMEI号:"+m_IMEI3_Read+"与输入的IMEI:"+m_IMEI3+"不相同=======失败！！！\r\n\r\n"+LOG_Show_Old);
					m_Result.UpdateWindow();
				}
				else
				{
					m_Result.GetWindowText(LOG_Show_Old);  
					m_Result.SetWindowText("读取第( "+IMEI_Count_plus_CS+" )个IMEI号:"+m_IMEI3_Read+"与输入的IMEI:"+m_IMEI3+"一致=======成功继续...\r\n\r\n"+LOG_Show_Old);
					m_Result.UpdateWindow();
				}
			}
			else if(i == 3)
			{
				m_IMEI4_Read.Format("%s",Imei[i]);
				if(m_IMEI4_Read!=m_IMEI4)
				{
					IMEI_Write_Result=false;
					m_Result.GetWindowText(LOG_Show_Old);  
					m_Result.SetWindowText("读取第( "+IMEI_Count_plus_CS+" )个IMEI号:"+m_IMEI4_Read+"与输入的IMEI:"+m_IMEI4+"不相同=======失败！！！\r\n\r\n"+LOG_Show_Old);
					m_Result.UpdateWindow();
				}
				else
				{
					m_Result.GetWindowText(LOG_Show_Old);  
					m_Result.SetWindowText("读取第( "+IMEI_Count_plus_CS+" )个IMEI号:"+m_IMEI4_Read+"与输入的IMEI:"+m_IMEI4+"一致=======成功继续...\r\n\r\n"+LOG_Show_Old);
					m_Result.UpdateWindow();
				}
			}
			else
				m_IMEI_Read="NULL";
		}
	}

	////////////////////////////////////读取IME检查

	//读取MTK平台的板号
	if(BARCODE_Choose_M=="true")//MTK功能机
    {
		char barcode[90];
		memset(barcode,0x0,sizeof(barcode));
        if (METAAPP_SUCCESS!=METAAPP_readBarcodefromNVRAM(g_Meta.iMeta_handle, 5000, barcode) )
        {
			IMEI_Write_Result=false;
			m_Result.GetWindowText(LOG_Show_Old);  
			m_Result.SetWindowText("\r\n读手机板号失败！！！\r\n\r\n"+LOG_Show_Old);
			m_Result.UpdateWindow();
        }
        else
        {
			BARCODE_Input.Format("%s",barcode);
			BARCODE_Input=BARCODE_Input.Left(15);//限制板号的长度
			BARCODE_Input_Control.SetWindowTextA(BARCODE_Input);;
			m_Result.GetWindowText(LOG_Show_Old);  
			m_Result.SetWindowText("\r\n读手机板号成功！\r\n\r\n板号为:"+BARCODE_Input+"\r\n"+LOG_Show_Old);
			if(Check_repeat_Choose_M=="true")
			{
				CString DB_FAIL="";
				CString Conn="";
				Conn.Format("driver={SQL Server};Server=%s;DATABASE=%s;UID=%s;PWD=%s", m_server_M,m_db_M,m_user_M, m_pwd_M);

				if(Setdlg.myado.m_pCon==NULL)
				{
					DB_FAIL=Setdlg.myado.ConnecDataLibrary(Conn,"","",adModeUnknown);   
				}
				if(Setdlg.myado.m_pCon->State==0)  //1表示已经打开,0表示关闭，数据库意外断开，重连
				{
					m_Result.GetWindowText(LOG_Show_Old);  
					m_Result.SetWindowText("=================================数据库意外断开，重连........\r\n\r\n"+LOG_Show_Old);
					m_Result.UpdateWindow();
					DB_FAIL=Setdlg.myado.ConnecDataLibrary(Conn,"","",adModeUnknown);//数据库意外断开，重连
				}
				else
				{
					m_Result.GetWindowText(LOG_Show_Old);  
					m_Result.SetWindowText("=================================数据库连接正常中...\r\n\r\n"+LOG_Show_Old);
					m_Result.UpdateWindow();
				}

				if(DB_FAIL=="FAIL")
				{
					IMEI_Write_Result=false;
					m_Result.GetWindowText(LOG_Show_Old);
					m_Result.SetWindowText("连接数据库失败，无法检查板号的重复性，请检查网络.......\r\n\r\n"+LOG_Show_Old);
					m_Result.UpdateWindow();
				}
				else
				{
					/////////连接数据库检查板号是否重复///////////////////////////////	
					Setdlg.myado.OpenSheet("select barcode from dse_barcode_imei WHERE barcode ='"+BARCODE_Input+"'");
					BOOL IMEI_Check=Setdlg.myado.Find("barcode='"+BARCODE_Input+"'");
					Setdlg.myado.CloseSheet();
					if(IMEI_Check==TRUE)
					{
						IMEI_Write_Result=false;
						m_Result.GetWindowText(LOG_Show_Old);  
						m_Result.SetWindowText("=================================板号:"+BARCODE_Input+"   重复！！数据不上传，请检查后==重新写号！！！\r\n\r\n"+LOG_Show_Old);
						m_Result.UpdateWindow();
					}
					else
					{
						m_Result.GetWindowText(LOG_Show_Old);  
						m_Result.SetWindowText("==================================板号:"+BARCODE_Input+"   数据库正常\r\n\r\n"+LOG_Show_Old);
						m_Result.UpdateWindow();
					}
					/////////////////////////////////////////
				}
			}
			
        }
	}
	if((Software_Version_Check_Choose_M=="true"))   //MTK功能机
	{
		signed short int Version_token;
		if(META_SUCCESS!=META_GetTargetVerInfo_r(g_Meta.iMeta_handle,CNF_Read_Barcode,&Version_token,NULL))
		{
			IMEI_Write_Result=false;
			m_Result.GetWindowText(LOG_Show_Old);
			m_Result.SetWindowText("读软件版本失败！！！\r\n\r\n"+LOG_Show_Old);
			m_Result.UpdateWindow();
		}
		else
		{
			CString  strTemp;
			strTemp=S_HW_Version;
			strTemp.Replace("\r","");
			strTemp.Replace("\n","");
			strTemp.Replace(" ","---");
			if(NULL==_tcsstr(strTemp,Software_Version_M))
			{
				IMEI_Write_Result=false;
				m_Result.GetWindowText(LOG_Show_Old);
				m_Result.SetWindowText("软件版本不匹配！！！:\r\n话机软件版本为："+strTemp+"\r\n设置软件版本为："+Software_Version_M+"\r\n"+LOG_Show_Old);
				m_Result.UpdateWindow();
			}   
			else
			{
				m_Result.GetWindowText(LOG_Show_Old);
				m_Result.SetWindowText("软件版本匹配:\r\n话机软件版本为："+strTemp+"\r\n设置软件版本为："+Software_Version_M+"\r\n"+LOG_Show_Old);
				m_Result.UpdateWindow();
			}
		}
	}
	//写BT地址：

	if((IMEI_Write_Result==true )&&(BT_MAC_ADDR_Choose_M=="true"))//写IMEI成功，或者不写IMEI
	{
		IMEI_Write_Result&=func_bt_address_meta_hdlr_feature_phone();//成功返回true
	}
	//写WIFI地址：
	if((IMEI_Write_Result==true )&&(WIFI_MAC_ADDR_Choose_M=="true"))//写IMEI成功、写BT地址成功，或者不写IMEI、不写BT地址
	{
		IMEI_Write_Result&=func_wifi_mac_meta_hdlr_feature_phone ();//成功返回true
	}

	////写号结束/////////////////////////////
	


	if(METAAPP_SUCCESS==METAAPP_DisconnectWithTarget( g_Meta.iMeta_handle ))     //地址成功！！！\r\n\r\n"+LOG_Show_Old);
	{
		m_Result.GetWindowText(LOG_Show_Old);  
		m_Result.SetWindowText("Meta Disconnect with target ,and turn to next phone\r\n\r\n"+LOG_Show_Old);
		m_Result.UpdateWindow();
	}
	else
	{
		IMEI_Write_Result=false;
		m_Result.GetWindowText(LOG_Show_Old);  
		m_Result.SetWindowText("disconnect form meta mode FAIL！！！请取下电池后重试\r\n\r\n"+LOG_Show_Old);
		m_Result.UpdateWindow();
	}

	if (IMEI_Write_Result==true )//写号成功
	{
        gColor = RGB(0,255,0);
		gFinalResult.Format("     ~~~");
		RedrawWindow();  m_Result.UpdateWindow(); 
		if(UP_DB_Data_M=="false")//不上传数据库
		{
			m_Result.GetWindowText(LOG_Show_Old);
			m_Result.SetWindowText("成功写号！！！\r\n\r\n"+LOG_Show_Old);
			//UpdateData(true);  //把中间值赋给变量
			//UpdateData(false); //把变量显示在界面
			m_Result.UpdateWindow();

			///////自动生成计数处理/////////////////////
			if((AutoBTAddr_Choose_M=="true")||(AutoWIFIAddr_Choose_M=="true"))
				BtAddrCount_int++;
			BtAddrCount.Format("%d",BtAddrCount_int);

			IMEI_Count.Format("%d",IMEI_Count_int);  //IMEI_Count_int 已经递增
			SaveConfig_Change();
			//////////自动生成计数处理//////////////////

			gColor = RGB(0,255,0);
			gFinalResult.Format("     OK");
			RedrawWindow();  m_Result.UpdateWindow(); 
			//OnSelchangeAuto();    //不将IMEI1清零
			//IMEI_Control.SetFocus();
			//IMEI_Control.SetSel(0,-1);
			
		}
		else if(Initial_Connect_DB_M=="true")//上传数据库//初始化连接了数据库才能上传，权限限制
		{
	
			CString Conn="";
			CString DB_FAIL="";
			Conn.Format("driver={SQL Server};Server=%s;DATABASE=%s;UID=%s;PWD=%s", m_server_M,m_db_M,m_user_M, m_pwd_M);
			/*if(   (DB_CONNECT_M=="true")&&(IMEI_DB_Choose_M=="false")&&(MAC_DB_Choose_M=="false")&&(Check_repeat_Choose_M=="false")   ||   (ORACLE_Connect_Choose_M=="true")&&(UP_DB_Data_M=="true")&&(IMEI_DB_Choose_M=="false")&&(MAC_DB_Choose_M=="false")&&(Check_repeat_Choose_M=="false")    ) //每次操作数据库时候，重连一次,并且要除了数据库关联的情况（这时候已经连接一次）、除了检查重复性
				DB_FAIL=Setdlg.myado.ConnecDataLibrary(Conn,"","",adModeUnknown);//写号成功上传数据库，重连
			else
				DB_FAIL="SUCCESS";
			*/
			if(Setdlg.myado.m_pCon==NULL)
			{
				DB_FAIL=Setdlg.myado.ConnecDataLibrary(Conn,"","",adModeUnknown);   
			}
			if(Setdlg.myado.m_pCon->State==0)  //1表示已经打开,0表示关闭，数据库意外断开，重连
			{
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("=================================数据库意外断开，重连........\r\n\r\n"+LOG_Show_Old);
				m_Result.UpdateWindow();
				DB_FAIL=Setdlg.myado.ConnecDataLibrary(Conn,"","",adModeUnknown);//数据库意外断开，重连
			}
			else
			{
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("=================================数据库连接正常中...\r\n\r\n"+LOG_Show_Old);
				m_Result.UpdateWindow();
			}

			if(DB_FAIL=="FAIL")
			{
				IMEI_Control.SetFocus();
				IMEI_Control.SetSel(0,-1);
				m_Result.GetWindowText(LOG_Show_Old);
				m_Result.SetWindowText("连接数据库失败，无法上传数据，请检查网络.......\r\n\r\n"+LOG_Show_Old);

				gColor = RGB(255,0,0);
				gFinalResult.Format("UP_FAIL");
				RedrawWindow();  m_Result.UpdateWindow(); 

				return 1;
			}
			///////////////////解锁码///////////////////////////////////////
			if(Unlock_Code_Choose_M=="true")
			{
				TranslateMuch(m_IMEI);
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("获取IMEI："+m_IMEI+"对应的解锁码："+Unlock_code_Number+"\r\n"+LOG_Show_Old);
				m_Result.UpdateWindow();
			}
			///////////////////解锁码///////////////////////////////////////
			CTime CurrentTime=CTime::GetCurrentTime();
			CString strTime;
			strTime =CurrentTime.Format( "%Y-%m-%d %H:%M:%S ");
			//CString strSQL = "Insert into dse_barcode_imei(imei,imei2,imei3,imei4,IMEI_NUM,barcode,msn,WIP_ENTITY_NAME,TYPE,MOBCLASS,LINENUM,classnum,PRODADDRESS,create_time,is_return,SOFTVERSION,HARDVERSION,IMEI_WRITER,WIFI_MAC,BLUE_MAC)\
							 values('"+m_IMEI+"','"+m_IMEI2+"','"+m_IMEI3+"','"+m_IMEI4+"','"+IMEI_Num+"','"+BARCODE_Input+"','"+m_SN+"','"+Order_Num_up_M+"','"+Machine_Type_M+"','"+Modulation_Tppe_M+"','"+Line_Number_M+"','"+Floor_M+"','"+Product_Address_M+"','"+strTime+"','"+Is_Return_M+"','"+Software_Version_M+"','"+Hardware_Version_M+"','"+Operator_M+"','"+m_strWifi+"','"+m_BT_Addr+"')";	
			//CString strSQL = "Insert into dse_barcode_imei(imei,imei2,imei3,imei4,barcode,msn,WIP_ENTITY_NAME,TYPE,MOBCLASS,LINENUM,classnum,PRODADDRESS,create_time,is_return,SOFTVERSION,HARDVERSION,IMEI_WRITER,WIFI_MAC,BLUE_MAC)\
							 values('"+m_IMEI+"','"+m_IMEI2+"','"+m_IMEI3+"','"+m_IMEI4+"','"+BARCODE_Input+"','"+m_SN+"','"+Order_Num_up_M+"','"+Machine_Type_M+"','"+Modulation_Tppe_M+"','"+Line_Number_M+"','"+Floor_M+"','"+Product_Address_M+"','"+strTime+"','"+Is_Return_M+"','"+Software_Version_M+"','"+Hardware_Version_M+"','"+Operator_M+"','"+m_strWifi+"','"+m_BT_Addr+"')";			
			CString strSQL = "Insert into dse_barcode_imei(imei,NetCode,Unlock_Code,IMEI2,IMEI3,IMEI4,barcode,msn,WIP_ENTITY_NAME,TYPE,LINENUM,create_time,is_return,SOFTVERSION,HARDVERSION,WIFI_MAC,BLUE_MAC)\
							 values('"+m_IMEI+"','"+NETCODE_Input+"','"+Unlock_code_Number+"','"+m_IMEI2+"','"+m_IMEI3+"','"+m_IMEI4+"','"+BARCODE_Input+"','"+m_SN+"','"+Order_Num_up_M+"','"+Machine_Type_M+"','"+Line_Number_M+"','"+strTime+"','"+Is_Return_M+"','"+Software_Version_M+"','"+Hardware_Version_M+"','"+m_strWifi+"','"+m_BT_Addr+"')";	
			
			Setdlg.myado.OpenSheet("select * from dse_barcode_imei");
			BOOL UP_FAIL=Setdlg.myado.Execute(strSQL);
			Setdlg.myado.CloseSheet();
			Sleep(500);
			Setdlg.myado.OpenSheet("select imei from dse_barcode_imei WHERE imei ='"+m_IMEI+"'");
			BOOL IMEI_Check_UP=Setdlg.myado.Find("imei='"+m_IMEI+"'");
			Setdlg.myado.CloseSheet();
			
			BOOL UP_WritedFlag;
			if((UP_FAIL==TRUE&&IMEI_Check_UP==TRUE))
			{
				//AfxMessageBox("上传SQL成功！");
				

				m_Result.GetWindowText(LOG_Show_Old);
				m_Result.SetWindowText("成功写号！！！并且成功上传SQL_SERVER数据库！！！\r\n\r\n"+LOG_Show_Old);
				//UpdateData(true);  //把中间值赋给变量
				//UpdateData(false); //把变量显示在界面
				if(UpdateIMEI_MSN_Choose_M=="true")
				{
					CString strSQL_WritedFlag="UPDATE dse_imei_sn SET WritedFlag = '1' WHERE imei ='"+m_IMEI+"'";
					Setdlg.myado.OpenSheet("select * from dse_imei_sn WHERE imei ='"+m_IMEI+"'");
					UP_WritedFlag=Setdlg.myado.Execute(strSQL_WritedFlag);
					Setdlg.myado.CloseSheet();

					if(UP_WritedFlag==TRUE)
					{
						m_Result.GetWindowText(LOG_Show_Old);
						m_Result.SetWindowText("更新贴纸表标志成功！\r\n\r\n"+LOG_Show_Old);
					}
					else
					{
						m_Result.GetWindowText(LOG_Show_Old);
						m_Result.SetWindowText("更新贴纸表标志失败！！！\r\n\r\n"+LOG_Show_Old);
					}
					m_Result.UpdateWindow();
				}
				else
				{
					m_Result.GetWindowText(LOG_Show_Old);
					m_Result.SetWindowText("没有勾选更新贴纸表标志..\r\n\r\n"+LOG_Show_Old);
					m_Result.UpdateWindow();
					UP_WritedFlag=TRUE;
				}

				bool ORACLE_UpDatabase_RESULT=true;//上传ORACLE数据库结果
			    if(ORACLE_Connect_Choose_M=="true")//上传ORACLE数据库
				{
					ORACLE_UpDatabase_RESULT=Update_Oracle_Database();
				}
				//if(ORACLE_UpDatabase_RESULT==true)
				if((ORACLE_UpDatabase_RESULT==true)&&(UP_WritedFlag==TRUE))
				{
					///////自动生成计数处理/////////////////////
					BtAddrCount_int++;
					BtAddrCount.Format("%d",BtAddrCount_int);

					IMEI_Count.Format("%d",IMEI_Count_int);  //IMEI_Count_int 已经递增
					SaveConfig_Change();
					//////////自动生成计数处理//////////////////

					gColor = RGB(0,255,0);
					gFinalResult.Format("     OK");
					RedrawWindow();  m_Result.UpdateWindow(); 
					//OnSelchangeAuto();    //不将IMEI1清零
					//IMEI_Control.SetFocus();
					//IMEI_Control.SetSel(0,-1);
				}
				else if(UP_WritedFlag!=TRUE)
				{
					m_Result.GetWindowText(LOG_Show_Old);
					m_Result.SetWindowText("成功写号上传数据！！！但更新贴纸表标志失败！！！！！！！！！！！！！\r\n\r\n"+LOG_Show_Old);
					m_Result.UpdateWindow();

					gColor = RGB(255,0,0);
					gFinalResult.Format("UPDA_F");
					RedrawWindow();  m_Result.UpdateWindow(); 
				}
			}
			else
			{
				m_Result.GetWindowText(LOG_Show_Old);
				m_Result.SetWindowText("成功写号！！！但上传数据库失败！！！！！！！！！！！！！\r\n\r\n"+LOG_Show_Old);
				//UpdateData(true);  //把中间值赋给变量
				//UpdateData(false); //把变量显示在界面
				m_Result.UpdateWindow();

				gColor = RGB(255,0,0);
				gFinalResult.Format("UP_FAIL");
				RedrawWindow();  m_Result.UpdateWindow(); 
				OnSelchangeAuto();    //不将IMEI1清零
				//IMEI_Control.SetFocus();
				//IMEI_Control.SetSel(0,-1);
			}
		}
		else
		{
			m_Result.GetWindowText(LOG_Show_Old);  
			m_Result.SetWindowText("写号成功，但因没有选择   初始化时连接数据库   无法上传数据库\r\n\r\n"+LOG_Show_Old);
			gColor = RGB(255,0,0);
			gFinalResult.Format("     UP_Fail");
			RedrawWindow();  m_Result.UpdateWindow(); 
		}
        //m_strMsg.LoadString( "PASS! 按Enter开始下一次测试");
	}
	else
    {
        m_Result.GetWindowText(LOG_Show_Old);  
		m_Result.SetWindowText("写号失败！！！\r\n\r\n"+LOG_Show_Old);
		gColor = RGB(255,0,0);
		gFinalResult.Format("     Fail");
		RedrawWindow();  m_Result.UpdateWindow(); 
		//IMEI_Control.SetFocus();
		//IMEI_Control.SetSel(0,-1);
	}

	RedrawWindow();  m_Result.UpdateWindow(); 
	IMEI_Control.SetFocus();
	IMEI_Control.SetSel(0,-1);
	//初始化时连接了数据库、每次重连数据库、
	/*if(DB_CONNECT_M=="true")//数据库关闭处理
	{
		Setdlg.myado.CloseDataLibrary();//连接关闭必须在一个子函数内，不然出错
	}*/
	LOG_Show_Old="";
	return 0;
	
}
//-----------------------------------------------------------------------
////////////////////////////////
//Nvram初始化
bool CWriteIMEIDlg::NvramDatabaseInit () //IsNvramDatabaseInit (void )子函数
{
	unsigned long db;
	const char * ModemDatabasePath;
	ModemDatabasePath=(LPCTSTR)m_strExchangeFileName;
	ModemDatabasePath=m_strExchangeFileName.GetBuffer(sizeof(m_strExchangeFileName));
	if ( META_SUCCESS!=META_NVRAM_Init_r ( g_Meta.iMeta_handle, ModemDatabasePath, &db))//初始化内存参数
	{
		//MessagePopup ("WARNING", "Initial Database fail!, Please reselect a database.");
		return true;
	}
	g_Meta.bIsDatabaseInitialized [0] = true;  
	return false;
}
bool CWriteIMEIDlg::IsNvramDatabaseInit (void )
{
	//extern int Modem_Database_Select_Click_Shell ( void );
	if ( false == g_Meta.bIsDatabaseInitialized [0] ) 
	{

		if ( NvramDatabaseInit () )
		{
			//sprintf ( strLog, ">>Step: Reselect database and start to initiaize.\n");
			//Modem_Database_Select_Click_Shell ();
			//return NvramDatabaseInit();
			return true;
		}	
		
	}
	return false;
}




////////////////////////////////
//META连接参数设置
void CWriteIMEIDlg::SetMetaConnectReq ( void ) 
{
	int temp_com_port = 0;
	m_Port.GetWindowText(m_bUsbMode);
	CString m_bUsbMode_temp=m_bUsbMode.Right(m_bUsbMode.GetLength()-3);
	temp_com_port = atoi(m_bUsbMode_temp);
    if (m_bUsbMode=="USB")
    {
        temp_com_port=USB_COM_PORT; //使用USB口
    }
	BBCHIP_TYPE bb_chip=AUTO_DETECT_BBCHIP;//MT6250
	 
	g_MetaModem.tMETA_Connect_Req.com_port = temp_com_port;
	g_MetaModem.tMETA_Connect_Req.boot_meta_arg.m_bbchip_type = bb_chip;    

	g_MetaModem.tMETA_Connect_Req.boot_meta_arg.m_ext_clock = AUTO_DETECT_EXT_CLOCK;

	g_MetaModem.tMETA_Connect_Req.baudrate[0] = META_BAUD115200;
	g_MetaModem.tMETA_Connect_Req.baudrate[1] = META_BAUD460800;
	g_MetaModem.tMETA_Connect_Req.baudrate[2] = META_BAUD921600;
	g_MetaModem.tMETA_Connect_Req.baudrate[3] = META_BAUD_END;	

		
	g_MetaModem.tMETA_Connect_Req.flowctrl = META_SW_FLOWCTRL;
	g_MetaModem.tMETA_Connect_Req.ms_connect_timeout = META_CONNECT_TIME_OUT;
	g_MetaModem.tMETA_Connect_Req.boot_meta_arg.m_ms_boot_timeout = BOOT_INFINITE;                        //不能是  BOOT_STOP---SetMetaConnectReq---功能机
	g_MetaModem.tMETA_Connect_Req.boot_meta_arg.m_max_start_cmd_retry_count = META_BOOT_TIMERS;

	g_MetaModem.tMETA_Connect_Req.boot_meta_arg.m_cb_in_brom_stage = NULL;
	g_MetaModem.tMETA_Connect_Req.boot_meta_arg.m_cb_in_brom_stage_arg = NULL;
	g_MetaModem.tMETA_Connect_Req.boot_meta_arg.m_speedup_brom_baudrate = _TRUE; //115200

	g_MetaModem.tMETA_Connect_Req.boot_meta_arg.m_ready_power_on_wnd_handle = NULL;
	g_MetaModem.tMETA_Connect_Req.boot_meta_arg.m_ready_power_on_wparam = NULL;
	g_MetaModem.tMETA_Connect_Req.boot_meta_arg.m_ready_power_on_lparam = NULL;

	g_MetaModem.tMETA_Connect_Req.boot_meta_arg.m_auth_handle = g_Meta.t_AuthHandle;//Get_AuthHandle();
	g_MetaModem.tMETA_Connect_Req.boot_meta_arg.m_cb_sla_challenge = SLA_Challenge;//SLA_Challenge(NULL,NULL,0,NULL,NULL);
	g_MetaModem.tMETA_Connect_Req.boot_meta_arg.m_cb_sla_challenge_arg = NULL;
	g_MetaModem.tMETA_Connect_Req.boot_meta_arg.m_cb_sla_challenge_end = SLA_Challenge_END; // NULL;
	g_MetaModem.tMETA_Connect_Req.boot_meta_arg.m_cb_sla_challenge_end_arg = NULL;
	 
	            
	    // EDGE PC card
	g_MetaModem.tMETA_Connect_Req.boot_meta_arg.m_cb_com_init_stage_arg = NULL;

	E_TARGET_TYPE eTargetType; 
	CString Plat_Form_CS;
	Plat_Form.GetWindowTextA(Plat_Form_CS);
	if(Plat_Form_CS=="MTK功能机")
		eTargetType=TARGET_FEATURE_PHONE;
	else if(Plat_Form_CS=="MTK智能机")//MTK智能机
		eTargetType=TARGET_SMARTPHONE;

	switch ( eTargetType)//选择话机类型
	{
		case TARGET_FEATURE_PHONE:
			g_MetaModem.tMETA_Connect_Req.boot_meta_arg.m_cb_com_init_stage = NULL;
			
			break;

		case TARGET_SMARTPHONE:
			g_MetaModem.tMETA_Connect_Req.boot_meta_arg.m_cb_com_init_stage = NULL;
			break;

		//case PCMCIA_CARD:
			//g_MetaModem.tMETA_Connect_Req.boot_meta_arg.m_cb_com_init_stage = cb_PowerOnOxford;
			//break;
		default:
			break;
	}
	            
	//usb mode
	if (m_bUsbMode!="USB")
    {
        g_MetaModem.tMETA_Connect_Req.boot_meta_arg.m_usb_enable = _FALSE;  //使用串口
    }
    else
    {
        g_MetaModem.tMETA_Connect_Req.boot_meta_arg.m_usb_enable = _TRUE;
    }
	//g_MetaModem.tMETA_Connect_Req.boot_meta_arg.m_usb_enable = IsUSBCOMPort();

	g_MetaModem.usb_req.ms_connect_timeout = META_CONNECT_TIME_OUT;
	g_MetaModem.usb_req.boot_meta_arg = g_MetaModem.tMETA_Connect_Req.boot_meta_arg;
	if (m_bUsbMode!="USB")
    {
        g_MetaModem.usb_req.boot_meta_arg.m_usb_enable = _FALSE;  //使用串口
    }
    else
    {
        g_MetaModem.usb_req.boot_meta_arg.m_usb_enable= _TRUE;
    }
	//g_MetaModem.usb_req.boot_meta_arg.m_usb_enable = IsUSBCOMPort();
	//g_MetaModem.tMETA_Connect_Req.boot_meta_arg.m_enable_without_battery = _TRUE;//mtk71409
}  

//META模式连接手机
bool CWriteIMEIDlg::EnterMetaMode () 
{
	META_RESULT  META_Result;
	char info[1000];
	Brom_DebugOn ();//Brom_DebugOff ();
	META_DebugOn ();//META_DebugOff ();

	if (m_bUsbMode!="USB")
	{
		//----------------------------Meta Connect to target-------------
		m_Result.GetWindowText(LOG_Show_Old);  
		m_Result.SetWindowText("正通过虚拟串口连接.....\r\n\r\n"+LOG_Show_Old);
		m_Result.UpdateWindow();

		g_Meta.bootstop = BOOT_STOP; 

		g_MetaModem.eMetaApp_Result = METAAPP_ConnectWithTarget (g_Meta.iMeta_handle, &g_MetaModem.tMETA_Connect_Req, &g_Meta.bootstop, &g_MetaModem.tMETA_Connect_Report); 
		switch ( g_MetaModem.eMetaApp_Result )
		{
			case METAAPP_SUCCESS:
				 return false; 
				 break;	
			case METAAPP_NVRAM_DB_INCONSISTENT:			
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("Modem NVRAM database inconsistent with target...\r\n\r\n"+LOG_Show_Old);
				m_Result.UpdateWindow();
				break;
			case METAAPP_OPEN_UART_FAIL:
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("Open COM port fail...\r\n\r\n"+LOG_Show_Old);
				m_Result.UpdateWindow();
				break;
			case METAAPP_COM_ERROR:
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("COM port doesn't exist...\r\n\r\n"+LOG_Show_Old);
				m_Result.UpdateWindow();
				break;
			case METAAPP_TIMEOUT_FAIL:	
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("Connecting timeout...\r\n\r\n"+LOG_Show_Old);
				m_Result.UpdateWindow();
				break;
			case 	METAAPP_BOOT_FAIL:
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("Can't connect to target.\nPlease Enable Meta Dll.Log to get detail cause...\r\n\r\n"+LOG_Show_Old);
				m_Result.UpdateWindow();
				break;
			default: 
				break;
		}	
	} 
	else
	{    
		int DataCardSwitchToolOn=0;
		char valid_usb_info[100];  
	    if(DataCardSwitchToolOn)
		{
		     //strcat ( strLog,  " META_GetDynamicUSBComPortExWithFilter_r \n");	
             //strcpy(valid_usb_info,"VID_0e8d&PID_0003  VID_0e8d&PID_0023"); 
		     META_Result = META_GetDynamicUSBComPortExWithFilter_r( g_Meta.iMeta_handle, g_MetaModem.usb_req.ms_connect_timeout, (unsigned short int*)&g_MetaModem.usb_req.com_port, &g_Meta.bootstop, info, 1000, valid_usb_info);
		}

		else if(0)      //mtk71409 added 
		{
			// step1: Get Preloader Port
			//sprintf( strLog,  " step1: Get Preloader Port...\n");
		    META_Result = META_GetDynamicUSBComPortExWithFilter_r( g_Meta.iMeta_handle, g_MetaModem.usb_req.ms_connect_timeout, (unsigned short int*)&g_MetaModem.usb_req.com_port, &g_Meta.bootstop, info, 1000, "VID_0E8D&PID_2000");
			if(META_SUCCESS != META_Result)
			{
				return true;                 
			}
			// step2: Switch to Modem
			//sprintf ( strLog,  "step2: Switch to Modem...\n");
			//if (EBOOT_SUCCESS != SP_SwitchModem(g_MetaModem.usb_req.com_port, CBR_115200,  &g_Meta.bootstop))
			//{     
				//return true; 	
			//}
			
			// step3: Get Modem Brom Port
			//Delay(20.0); //can't delay?
			META_Result = META_GetDynamicUSBComPortExWithFilter_r( g_Meta.iMeta_handle, g_MetaModem.usb_req.ms_connect_timeout, (unsigned short int*)&g_MetaModem.usb_req.com_port, &g_Meta.bootstop, info, 512, "VID_0E8D&PID_000");
			if(META_SUCCESS != META_Result)
			{
				return true; 
			}
			// All OK
			//sprintf ( strLog,  "ALL is OK...\n");;
		}//mtk71409  added by JiXiang
        else
        {
			m_Result.GetWindowText(LOG_Show_Old);  
			m_Result.SetWindowText("正通过USB获取通信端口port,========================请给MTK手机上电...\r\n\r\n"+LOG_Show_Old);
			m_Result.UpdateWindow();
			META_Result = META_GetDynamicUSBComPort_r (g_Meta.iMeta_handle, g_MetaModem.usb_req.ms_connect_timeout, (unsigned short int*)&g_MetaModem.usb_req.com_port,&g_Meta.bootstop);
		}

		CString USB_PORT;
		USB_PORT.Format("%d",g_MetaModem.usb_req.com_port);
		if ((META_Result != META_SUCCESS) )//&& (META_Result == META_STOP_ENUM_USB_PROCEDURE))
		{
			//MessagePopup ( "Meta Warning",  "Get dynamic USB COM fail, please Reconnect to target" );
			m_Result.GetWindowText(LOG_Show_Old);  
			m_Result.SetWindowText("Get dynamic USB COM fail, please Reconnect to target...\r\n\r\n"+LOG_Show_Old);
			m_Result.UpdateWindow();
        }
		else
		{
			m_Result.GetWindowText(LOG_Show_Old);  
			m_Result.SetWindowText("Get dynamic USB COM Success...\r\n\r\n端口PORT="+USB_PORT+LOG_Show_Old);
			m_Result.UpdateWindow();
		}
		if (META_SUCCESS == META_Result)
		{
			int META_Connec=0;
			do
			{
				META_Result = META_ConnectWithTargetByUSB_r (g_Meta.iMeta_handle, &g_MetaModem.usb_req,  &g_Meta.bootstop, &g_MetaModem.usb_report);
				if ((g_MetaModem.usb_report.boot_meta_ret != S_DONE) && (g_MetaModem.usb_report.boot_meta_ret != S_STOP) && (g_MetaModem.usb_report.boot_meta_ret != S_BROM_FAIL_TO_GET_BBCHIP_HW_VER))
				{
					//MessagePopup ("Meta Error", "Connecting timeout...");
					m_Result.GetWindowText(LOG_Show_Old);  
					m_Result.SetWindowText("USB_Connecting timeout...\r\n\r\n"+LOG_Show_Old);
					m_Result.UpdateWindow();

				}
				else if ((META_Result != META_SUCCESS) && (META_Result != META_STOP_ENUM_USB_PROCEDURE) &&  (META_Result != META_MAUI_DB_INCONSISTENT) )
				{
					//MessagePopup ("Meta Error", "Connecting timeout...");
					m_Result.GetWindowText(LOG_Show_Old);  
					m_Result.SetWindowText("USB_Connecting timeout...\r\n\r\n"+LOG_Show_Old);
					m_Result.UpdateWindow();
				}
				else if ( META_SUCCESS == META_Result )
				{
					m_Result.GetWindowText(LOG_Show_Old);  
					m_Result.SetWindowText("USB_Connecting OK...\r\n\r\n"+LOG_Show_Old);
					m_Result.UpdateWindow();
					 return false; 
				}
				META_Connec++;
				Sleep(1000);
			}while(META_Connec<3);
		 }
	}
	return true;

}













//////////////////////////////////////
//////////////////////////////////////
//写蓝牙地址函数

bool CWriteIMEIDlg::func_bt_address_meta_hdlr_feature_phone ( void )
{
    unsigned char i, iBCD_L, iBCD_H;
	
	BT_ModuleID_S BT_ID;

	S_HANDSET_BT_ADRRESS tBtAddress_W, tBtAddress_R;
	unsigned char strBtAddress_W [ BT_ADDRESS_MAX_LENGTH + 1 ];
	unsigned short int iLoop = 0;
	unsigned int status;
	
	/*//strcat ( strLog,  "        Write BT address to feature phone nvram\n");
	if ( IsNvramDatabaseInit () )
	{
		//sprintf ( strLog, "   Error: Init feature phone Database failed.\n");
		return;
	}
	SetMetaConnectReq ();
	if ( EnterMetaMode () ) 
	{
		METAAPP_DisconnectWithTarget( g_Meta.iMeta_handle ); 
		//sprintf ( strLog, "   Error: Connect failed, Exit Meta Mode and turn to next phone.\n" );
	}
	//sprintf ( strLog,  ">>Step: Start to write BT Address.\n" );
	*/
	

	if ( !META_QueryIfFunctionSupportedByTarget_r ( g_Meta.iMeta_handle, 5000, "META_BT_QueryModuleID_r" ))
	{
		if ( META_BT_QueryModuleID_r (g_Meta.iMeta_handle, 5000, &BT_ID ))
		{
			//CString BTMODULE_CS;//蓝牙模块名
			//BTMODULE_CS.Format("%s",BT_ID.id);
			m_Result.GetWindowText(LOG_Show_Old);  
			m_Result.SetWindowText("Query BT Module fail！！！\r\n\r\n"+LOG_Show_Old);
			m_Result.UpdateWindow();
			return false;
		}
		else
		{
			m_Result.GetWindowText(LOG_Show_Old);  
			m_Result.SetWindowText("Query BT Module Sucess,Continue....\r\n\r\n"+LOG_Show_Old);
			m_Result.UpdateWindow();
		}
	}
	else
	{	
		//MessagePopup ("BT Address Error", " Please Make sure the handset have the BT Module First");
		m_Result.GetWindowText(LOG_Show_Old);  
		m_Result.SetWindowText("Query BT Module fail！！！\r\n\r\n"+LOG_Show_Old);
		m_Result.UpdateWindow();
		return false;
	}

	if(MT6236 == g_MetaModem.tMETA_Connect_Report.boot_result.m_bbchip_type||g_MetaModem.usb_report.boot_result.m_bbchip_type==MT6236)
	{
		//sprintf ( strLog, "Detect bbchip is MT6236.\n" );
		BT_ID.id = BTMODULE_MT6612;
	}
	//char*strBtAddress_W_temp;
	//strcpy(strBtAddress_W_temp,m_BT_Addr);
	strcpy((char *)strBtAddress_W,m_BT_Addr);

	for ( i = 0; i < BT_ADDRESS_MAX_LENGTH; i += 2 )
	{
		 iBCD_H = Ascii2BCD( strBtAddress_W [i] );
		 iBCD_L = Ascii2BCD( strBtAddress_W [i + 1] );

		if ( BTMODULE_MT6601 == BT_ID.id || BTMODULE_MT6611 == BT_ID.id ||
			BTMODULE_MT6612 == BT_ID.id || BTMODULE_MT6616 == BT_ID.id||
			BTMODULE_MT6622 == BT_ID.id ||BTMODULE_MT6250==BT_ID.id
			||BTMODULE_MT6276)
		{
			tBtAddress_W.bd_addr [BD_ADDR_LEN-1 - i/2] = iBCD_H<<4 | iBCD_L;
		}
		else
		{	
			tBtAddress_W.bd_addr [i/2] = iBCD_H<<4 | iBCD_L;
		}
	}

	if ( 0 )
	{
		if ( META_BT_DisableNvramOnlineUpdate_r ( g_Meta.iMeta_handle, 5000 ))
		{
			//MessagePopup ( "Meta  API Error", " META_BT_DisableNvramOnlineUpdate_r error" );
		}
	}

	g_MetaModem.eMetaApp_Result = METAAPP_SUCCESS;
	g_MetaModem.eMeta_Result = META_SUCCESS;
	
	switch ( BT_ID.id )	
	{
	
		case BTMODULE_MT6601:  //6601
			if ( g_MetaModem.eMetaApp_Result== METAAPP_BT_writeMT6601BTAddrtoNVRAM ( g_Meta.iMeta_handle, 5000, &tBtAddress_W ))
			{
				//sprintf ( strLog, "   Error: Write BT Address  fail. Meta Result = %d\n", MR );
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("蓝牙模块BTMODULE_MT6601--->写蓝牙地址成功！！！\r\n\r\n"+LOG_Show_Old);
				m_Result.UpdateWindow();
				
			}
			else
			{
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("蓝牙模块BTMODULE_MT6601--->写蓝牙地址失败！！！\r\n\r\n"+LOG_Show_Old);
				m_Result.UpdateWindow();
				return false;
			}
			
			if ( g_MetaModem.eMetaApp_Result== METAAPP_BT_readMT6601BTAddrfromNVRAM ( g_Meta.iMeta_handle, 5000, &tBtAddress_R ))
			{
				//sprintf ( strLog, "   Error: Write BT Address  fail. Meta Result = %d\n", MR );
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("蓝牙模块BTMODULE_MT6601--->读蓝牙地址成功！！！\r\n\r\n"+LOG_Show_Old);
				m_Result.UpdateWindow();
				
			}
			else
			{
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("蓝牙模块BTMODULE_MT6601--->读蓝牙地址失败！！！\r\n\r\n"+LOG_Show_Old);
				m_Result.UpdateWindow();
				return false;
			}
			break;
			
		case BTMODULE_MT6611:
		case BTMODULE_MT6612:
		case BTMODULE_MT6616:
		case BTMODULE_MT6622:
		case BTMODULE_MT6250:
		case BTMODULE_MT6276:
			if ( g_MetaModem.eMeta_Result == REQ_WriteBT6611Addr2NVRAM_Start ( BT_ID.id, tBtAddress_W.bd_addr ))//写BT6611BT地址
			{
				//sprintf ( strLog, "   Error: Write BT Address  fail. Meta Result = %d\n", MR );
				//METAAPP_DisconnectWithTarget( g_Meta.iMeta_handle );   //ExitMetaMode ();  
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("蓝牙模块BTMODULE_MT6611/12/16、BTMODULE_MT6622/50/76--->写蓝牙地址成功！！！\r\n\r\n"+LOG_Show_Old);
				m_Result.UpdateWindow();
				
			}
			else
			{
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("蓝牙模块BTMODULE_MT6611/12/16、BTMODULE_MT6622/50/76--->写蓝牙地址失败！！！\r\n\r\n"+LOG_Show_Old);
				m_Result.UpdateWindow();
				return false;
			}
			
			if (g_MetaModem.eMeta_Result== REQ_ReadBT6611AddrFromNVRAM_Start ( BT_ID.id, tBtAddress_R.bd_addr ))//读BT6611BT地址
			{
				//sprintf ( strLog, "   Error: Write BT Address  fail. Meta Result = %d\n", MR );
				//METAAPP_DisconnectWithTarget( g_Meta.iMeta_handle );   //ExitMetaMode ();   
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("蓝牙模块BTMODULE_MT6611/12/16、BTMODULE_MT6622/50/76--->读蓝牙地址成功！！！\r\n\r\n"+LOG_Show_Old);
				m_Result.UpdateWindow();
				
			}
			else
			{
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("蓝牙模块BTMODULE_MT6611/12/16、BTMODULE_MT6622/50/76--->读蓝牙地址失败！！！\r\n\r\n"+LOG_Show_Old);
				m_Result.UpdateWindow();
				return false;
			}
			break;
			
		case BTMODULE_RFMD3500:  //RFMD
			if ( g_MetaModem.eMetaApp_Result== METAAPP_BT_writeRFMD3500BTAddrtoNVRAM ( g_Meta.iMeta_handle, 5000, &tBtAddress_W ))
			{
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("蓝牙模块BTMODULE_RFMD3500--->写蓝牙地址成功！！！\r\n\r\n"+LOG_Show_Old);
				m_Result.UpdateWindow();
			}
			else
			{
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("蓝牙模块BTMODULE_RFMD3500--->写蓝牙地址失败！！！\r\n\r\n"+LOG_Show_Old);
				m_Result.UpdateWindow();
				return false;
			}

			if ( g_MetaModem.eMetaApp_Result== METAAPP_BT_readRFMD3500BTAddrfromNVRAM ( g_Meta.iMeta_handle, 5000, &tBtAddress_R ))
			{
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("蓝牙模块BTMODULE_RFMD3500--->读蓝牙地址成功！！！\r\n\r\n"+LOG_Show_Old);
				m_Result.UpdateWindow();
			}
			else
			{
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("蓝牙模块BTMODULE_RFMD3500--->读蓝牙地址失败！！！\r\n\r\n"+LOG_Show_Old);
				m_Result.UpdateWindow();
				return false;
			}
			break;
		default:
			//MessagePopup ("BT Address Error", "Not support BT Chip.");  
			m_Result.GetWindowText(LOG_Show_Old);  
			m_Result.SetWindowText("话机不支持蓝牙！！！\r\n\r\n"+LOG_Show_Old);
			m_Result.UpdateWindow();
			return false;
			//break;
	}

	char* strLog;
	CString strLog_CS;
	strLog=new char[128];
	strLog_CS="";
	//sprintf ( strLog, "   OK: Write BT Address successfully.\n"  );//sprintf ( strLog, "   Step: Start to check BT Address.\n"  );
	sprintf ( strLog, "   从话机读出的. BT Address = \"%x %x %x %x %x %x\".\n",  tBtAddress_R.bd_addr[0],
																									 tBtAddress_R.bd_addr[1],
																									 tBtAddress_R.bd_addr[2],
																									 tBtAddress_R.bd_addr[3],
																									 tBtAddress_R.bd_addr[4],
																									 tBtAddress_R.bd_addr[5]);
	strLog_CS.Format("%s",strLog);

	if ( memcmp ( tBtAddress_W.bd_addr, tBtAddress_R.bd_addr, BD_ADDR_LEN )) 
	{			
		m_Result.GetWindowText(LOG_Show_Old);  
		m_Result.SetWindowText(strLog_CS+"\r\n写入的与读出的蓝牙不一致，请重新写入！！！\r\n\r\n"+LOG_Show_Old);
		m_Result.UpdateWindow();
		return false;
	}
	else
	{
		/*sprintf ( strLog, "   OK: Check BT Address. BT Address  = \"%x %x %x %x %x %x\".\n.", tBtAddress_R.bd_addr[0],
																    					      tBtAddress_R.bd_addr[1],
																  					          tBtAddress_R.bd_addr[2],
																   					          tBtAddress_R.bd_addr[3],
																   					          tBtAddress_R.bd_addr[4],
																   					          tBtAddress_R.bd_addr[5]);*/	
		m_Result.GetWindowText(LOG_Show_Old);  
		m_Result.SetWindowText(strLog_CS+"\r\n写入的与读出的蓝牙一致，写BT地址确认OK！！！\r\n\r\n"+LOG_Show_Old);
		m_Result.UpdateWindow();
	}
	
	
    g_MetaModem.eMeta_Result = META_QueryIfFunctionSupportedByTarget_r(/*meta_handle*/0, 500, "META_MISC_EX_BackupCalibrationToStorage_r");

    if(META_SUCCESS == g_MetaModem.eMeta_Result)
	{
		//sprintf ( strLog, ">> Set the SDS\n");
	    if(META_MISC_EX_BackupCalibrationToStorage_r(g_Meta.iMeta_handle,5000,0,&status))
	    {
			//sprintf ( strLog, "   Error: BackupCalibrationToStorage fail, Exit Meta Mode and turn to next phone.\n" );
			m_Result.GetWindowText(LOG_Show_Old);  
			m_Result.SetWindowText("bt_BackupCalibrationToStorage fail！！！\r\n\r\n"+LOG_Show_Old);
			m_Result.UpdateWindow();
			return false;
	    }
		else
		{
	        //sprintf ( strLog, "   OK: BackupCalibrationToStorage OK, Exit Meta Mode and turn to next phone.\n" );
			m_Result.GetWindowText(LOG_Show_Old);  
			m_Result.SetWindowText("bt_BackupCalibrationToStorage OK！！！\r\n\r\n"+LOG_Show_Old);
			m_Result.UpdateWindow();
	    }
	}
	return true;//写BT_MAC--->OK
	//METAAPP_DisconnectWithTarget( g_Meta.iMeta_handle );//ExitMetaMode (); 
}
    

signed char  CWriteIMEIDlg::Ascii2BCD ( unsigned char  iAscii )
{
	signed char  iBCD;
	if ( iAscii>= '0' && iAscii<= '9')
	{
        iBCD = iAscii - '0';
			
	} else if ( iAscii>= 'a' && iAscii<= 'f' )
	{
        iBCD = iAscii - 'a' + 0x0a;
		
	} else if ( iAscii>= 'A' && iAscii<= 'F' )
	{
        iBCD = iAscii - 'A' + 0x0a;
	} else
	{
		return -1;
	}

	return iBCD;

}

//写蓝牙地址函数子函数-----------------写BT6611BT地址
META_RESULT CWriteIMEIDlg::REQ_WriteBT6611Addr2NVRAM_Start (unsigned int iBTId, unsigned char* pBTAddress)//REQ_WriteBTAddr2NVRAM_Start
{
    signed int m_usRID = 1;
    signed int m_iBTBufSize;
    const char *pLID;
    nvram_ef_btradio_mt6611_struct  tBT;
    nvram_ef_btradio_mtk_bt_chip_struct tBT1;
    signed int  isbt=0;
	switch ( iBTId )	
	{
		case BTMODULE_MT6611:
			pLID = "NVRAM_EF_BTRADIO_MT6611_LID"; 
			break;
			
		case BTMODULE_MT6612:
			pLID="NVRAM_EF_BTRADIO_MT6612_LID";
			if(META_NVRAM_QueryIsLIDExist(pLID))
			{
				isbt=1;
				pLID= "NVRAM_EF_BTRADIO_MTK_BT_CHIP_LID";
			}
			break;
		case BTMODULE_MT6616:
			pLID = "NVRAM_EF_BTRADIO_MT6616_LID"; 
			break;
		case BTMODULE_MT6622:
		case BTMODULE_MT6250:
		case BTMODULE_MT6276:
			pLID = "NVRAM_EF_BTRADIO_MTK_BT_CHIP_LID";
			break;

		default:
			break;
	}
    if(MT6236 == g_MetaModem.tMETA_Connect_Report.boot_result.m_bbchip_type||g_MetaModem.usb_report.boot_result.m_bbchip_type==MT6236)
    { 
		isbt=0;
		if(META_NVRAM_QueryIsLIDExist(pLID))
		{
		  pLID="NVRAM_EF_BTRADIO_MT6236_LID"; 
		}
    }  
	g_MetaModem.eMeta_Result = META_NVRAM_GetRecLen ( pLID, &m_iBTBufSize );
    if ( g_MetaModem.eMeta_Result != META_SUCCESS )
    {
        //TODO: LOG
        return g_MetaModem.eMeta_Result;
    }

	/*if ( NULL != g_MetaModem.tNVRAM_ReadCnf.buf )
	{
		free ( g_MetaModem.tNVRAM_ReadCnf.buf );	
	}*/

	g_MetaModem.tNVRAM_ReadReq.LID = pLID;
    g_MetaModem.tNVRAM_ReadReq.RID = m_usRID;
    g_MetaModem.tNVRAM_ReadCnf.len = m_iBTBufSize;
    g_MetaModem.tNVRAM_ReadCnf.buf = (unsigned char*) malloc (m_iBTBufSize*sizeof(unsigned char));

	g_MetaModem.eMeta_Result =  REQ_ReadFromNVRAM ();//读NVRAM------功能机BT
	if ( g_MetaModem.eMeta_Result!=META_SUCCESS )
    {
		if (NULL != g_MetaModem.tNVRAM_ReadCnf.buf)
		{
			free (g_MetaModem.tNVRAM_ReadCnf.buf);	//释放分配的存储空间--read------功能机，读NVRAM失败
		}
    	return g_MetaModem.eMeta_Result;
    }


    if(isbt==0)
    {
		g_MetaModem.eMeta_Result = META_NVRAM_BT_Decompose_MT6611Radio ( &tBT, (const char *)(char*)g_MetaModem.tNVRAM_ReadCnf.buf, m_iBTBufSize);
    }
    else
    {
      	g_MetaModem.eMeta_Result = META_NVRAM_BT_Decompose_MediatekBtChip ( &tBT1, (const char *)(char*)g_MetaModem.tNVRAM_ReadCnf.buf, m_iBTBufSize);
    }

	if ( g_MetaModem.eMeta_Result != META_SUCCESS )
    {
		if (NULL != g_MetaModem.tNVRAM_ReadCnf.buf)
		{
			free (g_MetaModem.tNVRAM_ReadCnf.buf);	//释放分配的存储空间--read---功能机，Decompose出错
		}
    	return g_MetaModem.eMeta_Result;
    }
	
	if(isbt==0)
    {
		memcpy ( tBT.BDAddr, pBTAddress, BD_ADDR_LEN ); 
    }
	else
	{
		memcpy ( tBT1.BDAddr, pBTAddress, BD_ADDR_LEN ); 
	}
	
	if (NULL != g_MetaModem.tNVRAM_ReadCnf.buf)
	{
		free (g_MetaModem.tNVRAM_ReadCnf.buf);	//最后，释放分配的存储空间--read--功能机
	}


	////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	switch ( iBTId )	
	{
		case BTMODULE_MT6601:  //6601
			pLID = "NVRAM_EF_BTRADIO_MT6601_LID";
			break;
			
		case BTMODULE_MT6611:
			pLID = "NVRAM_EF_BTRADIO_MT6611_LID"; 

		case BTMODULE_MT6612:
			pLID="NVRAM_EF_BTRADIO_MT6612_LID";
			if(META_NVRAM_QueryIsLIDExist(pLID))
			{
				isbt=1;
				pLID= "NVRAM_EF_BTRADIO_MTK_BT_CHIP_LID";
			}
			
			break;
		case BTMODULE_MT6616:
			pLID = "NVRAM_EF_BTRADIO_MT6616_LID"; 
			break;
		case BTMODULE_MT6622:
		case BTMODULE_MT6250:
		case BTMODULE_MT6276:
			pLID = "NVRAM_EF_BTRADIO_MTK_BT_CHIP_LID";
			break;

		default:
			break;
	}
	if(MT6236 == g_MetaModem.tMETA_Connect_Report.boot_result.m_bbchip_type||g_MetaModem.usb_report.boot_result.m_bbchip_type==MT6236)
    { 
		isbt=0;
        if(META_NVRAM_QueryIsLIDExist(pLID))
	    {
              pLID="NVRAM_EF_BTRADIO_MT6236_LID"; 
	    }  
  	}    
	g_MetaModem.eMeta_Result = META_NVRAM_GetRecLen ( pLID, &m_iBTBufSize );
	if ( g_MetaModem.eMeta_Result != META_SUCCESS )
    {
        //TODO: LOG
        return g_MetaModem.eMeta_Result;
    }

	m_usRID = 1;
	g_MetaModem.tNVRAM_WriteReq.LID = pLID;
    g_MetaModem.tNVRAM_WriteReq.RID = m_usRID;
    g_MetaModem.tNVRAM_WriteReq.len = m_iBTBufSize;
    g_MetaModem.tNVRAM_WriteReq.buf = (unsigned char*) malloc ( m_iBTBufSize*sizeof(unsigned char) );

	if(isbt==0)
    {
		g_MetaModem.eMeta_Result = META_NVRAM_BT_Compose_MT6611Radio ( &tBT,(char*)g_MetaModem.tNVRAM_WriteReq.buf, m_iBTBufSize);
    }
    else
    {
      	g_MetaModem.eMeta_Result = META_NVRAM_BT_Compose_MediatekBtChip ( &tBT1, (char*)g_MetaModem.tNVRAM_WriteReq.buf, m_iBTBufSize);
    }
 	if ( g_MetaModem.eMeta_Result != META_SUCCESS )
    {
        //TODO: LOG
		if (NULL != g_MetaModem.tNVRAM_WriteReq.buf)
		{
			free (g_MetaModem.tNVRAM_WriteReq.buf);	//释放分配的存储空间--Write-------功能机Compose出错
		}
        return g_MetaModem.eMeta_Result;
    }
	g_MetaModem.eMeta_Result = REQ_WriteNVRAM ();
    
	if ( g_MetaModem.eMeta_Result != META_SUCCESS )
    {
		if (NULL != g_MetaModem.tNVRAM_WriteReq.buf)
		{
			free (g_MetaModem.tNVRAM_WriteReq.buf);	//释放分配的存储空间--Write----功能机，写NVRAM失败
		}
    	return g_MetaModem.eMeta_Result;
    }

	if (NULL != g_MetaModem.tNVRAM_WriteReq.buf)
	{
		free (g_MetaModem.tNVRAM_WriteReq.buf);	//最后，释放分配的存储空间--Write----功能机
	}
    return META_SUCCESS;
}

//写蓝牙地址函数子函数-----------------读BT6611BT地址
META_RESULT CWriteIMEIDlg::REQ_ReadBT6611AddrFromNVRAM_Start ( unsigned int iBTId, unsigned char* pBTAddress )//REQ_ReadBTAddrFromNVRAM_Start
{
    signed int m_usRID = 1, m_iBTBufSize;
	//unsigned char *pLID;
	const char *pLID;
	nvram_ef_btradio_mt6611_struct  tBT;
    nvram_ef_btradio_mtk_bt_chip_struct tBT1;
    signed int  isbt=0;
	
	switch ( iBTId )	
	{
		case BTMODULE_MT6611:
			pLID = "NVRAM_EF_BTRADIO_MT6611_LID"; 
			break;

		case BTMODULE_MT6612:
			pLID="NVRAM_EF_BTRADIO_MT6612_LID";
			if(META_NVRAM_QueryIsLIDExist(pLID))
			{
				isbt=1;
				pLID= "NVRAM_EF_BTRADIO_MTK_BT_CHIP_LID";
			}
			break;
		case BTMODULE_MT6616:
			pLID = "NVRAM_EF_BTRADIO_MT6616_LID"; 
			break;
		case BTMODULE_MT6622:
		case BTMODULE_MT6250:
		case BTMODULE_MT6276:
			pLID = "NVRAM_EF_BTRADIO_MTK_BT_CHIP_LID";
			break;
		default:
			break;
	}
	if(MT6236 == g_MetaModem.tMETA_Connect_Report.boot_result.m_bbchip_type||g_MetaModem.usb_report.boot_result.m_bbchip_type==MT6236)
    { 
		isbt=0;
      	if(META_NVRAM_QueryIsLIDExist(pLID))
		{
			pLID="NVRAM_EF_BTRADIO_MT6236_LID"; 
		}    
     }  
    g_MetaModem.eMeta_Result = META_NVRAM_GetRecLen( pLID, &m_iBTBufSize);
    if ( g_MetaModem.eMeta_Result!=META_SUCCESS )
    {
    	return g_MetaModem.eMeta_Result;
    }

	/*if ( NULL != g_MetaModem.tNVRAM_ReadCnf.buf )
	{
		free ( g_MetaModem.tNVRAM_ReadCnf.buf );	
	}*/

	g_MetaModem.tNVRAM_ReadReq.LID = pLID;
    g_MetaModem.tNVRAM_ReadReq.RID = m_usRID;
    g_MetaModem.tNVRAM_ReadCnf.len = m_iBTBufSize;
    g_MetaModem.tNVRAM_ReadCnf.buf = (unsigned char*) malloc ( m_iBTBufSize*sizeof(unsigned char) );

	g_MetaModem.eMeta_Result =  REQ_ReadFromNVRAM ();//读NVRAM-----功能机BT
	if ( g_MetaModem.eMeta_Result!=META_SUCCESS )
    {
		if (NULL != g_MetaModem.tNVRAM_ReadCnf.buf)
		{
			free (g_MetaModem.tNVRAM_ReadCnf.buf);	//读NVRAM失败，释放分配的存储空间--REQ_Read---功能机
		}
    	return g_MetaModem.eMeta_Result;
    }

	if(isbt==0)
    {
		g_MetaModem.eMeta_Result = META_NVRAM_BT_Decompose_MT6611Radio ( &tBT, (const char *)(char*)g_MetaModem.tNVRAM_ReadCnf.buf, m_iBTBufSize);
    }
    else
    {
      	g_MetaModem.eMeta_Result = META_NVRAM_BT_Decompose_MediatekBtChip ( &tBT1, (const char *)(char*)g_MetaModem.tNVRAM_ReadCnf.buf, m_iBTBufSize);
    }

	if ( g_MetaModem.eMeta_Result!=META_SUCCESS )
    {
		if (NULL != g_MetaModem.tNVRAM_ReadCnf.buf)
		{
			free (g_MetaModem.tNVRAM_ReadCnf.buf);	//释放分配的存储空间--REQ_Read---功能机
		}
    	return g_MetaModem.eMeta_Result;
    }
	if(isbt==0)
    {
		memcpy ( pBTAddress,  tBT.BDAddr, BD_ADDR_LEN ); //转存读取的BT地址
    }
	else
	{
		memcpy ( pBTAddress,  tBT1.BDAddr, BD_ADDR_LEN ); 
	}
	if (NULL != g_MetaModem.tNVRAM_ReadCnf.buf)
	{
		free (g_MetaModem.tNVRAM_ReadCnf.buf);	//最后，释放分配的存储空间--REQ_Read---功能机
	}
	
	return g_MetaModem.eMeta_Result;
}
////////////////////////////////////
///////////////////////////////////
//读NVRAM子函数
static void __stdcall CNF_ReadFromNVRAM(const FT_NVRAM_READ_CNF *cnf, const short token, void *usrData)
{
     if (cnf->status != META_SUCCESS)
    {  
       	//ConfirmCallback (META_FAILED);  
		return; 
	}
	
	SetEvent (g_MetaModem.m_ReadFromNVRAMEvent);   
}

META_RESULT CWriteIMEIDlg::REQ_ReadFromNVRAM ( void )
{
    DWORD wait_result;
	signed short int m_sNVRAM_OPID;
	 
    g_MetaModem.m_ReadFromNVRAMEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
	
    ResetEvent ( g_MetaModem.m_ReadFromNVRAMEvent );

    g_MetaModem.eMeta_Result = META_NVRAM_Read_r ( g_Meta.iMeta_handle,
                                	          &g_MetaModem.tNVRAM_ReadReq,
                                              &g_MetaModem.tNVRAM_ReadCnf,
                                              CNF_ReadFromNVRAM,
                                              &m_sNVRAM_OPID,
                                              NULL );

    wait_result = WaitForSingleObject ( g_MetaModem.m_ReadFromNVRAMEvent, 15000 );

    //CloseHandle ( g_MetaModem.m_ReadFromNVRAMEvent );
    if ( WAIT_TIMEOUT == wait_result )
    {
        return META_TIMEOUT;
    }

    if ( g_MetaModem.eMeta_Result != META_SUCCESS )
    {
        return g_MetaModem.eMeta_Result;
    }

    return META_SUCCESS;
}

////////////////////////////////////////
///////////////////////////////////////
//写NVRAM子函数
static  void __stdcall CNF_WriteNVRAM ( const FT_NVRAM_WRITE_CNF *cnf, const short token, void *usrData)
{
    if (cnf->status != META_SUCCESS)
    {  
		return; 
	}
	SetEvent (g_MetaModem.m_WriteToNVRAMEvent);   
}

//-----------------------------------------------
META_RESULT CWriteIMEIDlg::REQ_WriteNVRAM(void)
{
    signed short int m_sNVRAM_OPID;
	DWORD wait_result;
	
    g_MetaModem.m_WriteToNVRAMEvent = CreateEvent( NULL, TRUE, FALSE, NULL );

    ResetEvent ( g_MetaModem.m_WriteToNVRAMEvent );

    g_MetaModem.eMeta_Result = META_NVRAM_Write_r ( g_Meta.iMeta_handle,
                                                   &g_MetaModem.tNVRAM_WriteReq,
                                                   CNF_WriteNVRAM,
                                                   &m_sNVRAM_OPID,
                                                   NULL);
    wait_result = WaitForSingleObject ( g_MetaModem.m_WriteToNVRAMEvent, 15000 );
    //CloseHandle ( g_MetaModem.m_WriteToNVRAMEvent );
    if ( WAIT_TIMEOUT == wait_result )
    {
        return META_TIMEOUT;
    }
    if ( g_MetaModem.eMeta_Result != META_SUCCESS )
    {
        return g_MetaModem.eMeta_Result;
    }
    return META_SUCCESS;
}



//////////////////////////////////////////
//////////////////////////////////////////
//写WIFImac地址
#define WIFI_MAC_MAX_LENGTH  12 
bool CWriteIMEIDlg::func_wifi_mac_meta_hdlr_feature_phone ( void )
{
	unsigned char i, iBCD_L, iBCD_H;
	
	WiFi_MacAddress_S tWifiMac_W, tWifiMac_R;
	unsigned char strWifiMac_W [ WIFI_MAC_MAX_LENGTH + 1 ];
	unsigned short int iLoop = 0;
	unsigned int iMacHeaderLen = 0;
	unsigned int status;


	/*//sprintf ( strLog, ">>Step: Start to Init feature phone Database...\n");
	if ( IsNvramDatabaseInit () ) 
	{
		//sprintf ( strLog, "   Error: Init feature phone Database failed.\n");
		return;
	}
	SetMetaConnectReq ();	
	if ( EnterMetaMode () )
	{
		//Exit_MetaModemMode ();
		METAAPP_DisconnectWithTarget ( g_Meta.iMeta_handle );
	    META_ShutDownTarget_r ( g_Meta.iMeta_handle );
		//sprintf ( strLog, "    Error: Connect failed, Exit Meta Mode and turn to next phone.\n" );
	}*/

	strcpy((char *)strWifiMac_W,m_strWifi);


	for ( i = 0; i < WIFI_MAC_MAX_LENGTH; i+=2 )
	{
		 
		 iBCD_H = Ascii2BCD( strWifiMac_W [i] );
		 iBCD_L = Ascii2BCD( strWifiMac_W [i + 1] );

		 tWifiMac_W.mac_addr [i/2] = iBCD_H<<4 | iBCD_L;
	}


	g_MetaModem.eMetaApp_Result = METAAPP_SUCCESS;
	//g_MetaModem.eMeta_Result = META_SUCCESS;
	
	if ( g_MetaModem.eMetaApp_Result == METAAPP_WiFi_writeWiFiMacAddressToNVRAM ( g_Meta.iMeta_handle, 5000, &tWifiMac_W ))
	{
		m_Result.GetWindowText(LOG_Show_Old);  
		m_Result.SetWindowText("写WIFI_MAC地址成功！！！\r\n\r\n"+LOG_Show_Old);
		m_Result.UpdateWindow();
	}
	else
	{
		m_Result.GetWindowText(LOG_Show_Old);  
		m_Result.SetWindowText("写WIFI_MAC地址成功！！！\r\n\r\n"+LOG_Show_Old);
		m_Result.UpdateWindow();
		return false;
		//METAAPP_DisconnectWithTarget( g_Meta.iMeta_handle );//ExitMetaMode ();
	}   
	
	//Start to check Wifi MAC
	if ( g_MetaModem.eMetaApp_Result == METAAPP_WiFi_readWiFiMacAddressFromNVRAM ( g_Meta.iMeta_handle,5000,&tWifiMac_R))
	{
		m_Result.GetWindowText(LOG_Show_Old);  
		m_Result.SetWindowText("写WIFI_MAC地址成功！！！\r\n\r\n"+LOG_Show_Old);
		m_Result.UpdateWindow();
	}
	else
	{
		m_Result.GetWindowText(LOG_Show_Old);  
		m_Result.SetWindowText("写WIFI_MAC地址成功！！！\r\n\r\n"+LOG_Show_Old);
		m_Result.UpdateWindow();
		return false;
		//METAAPP_DisconnectWithTarget( g_Meta.iMeta_handle );//ExitMetaMode ();   
	}
    
	char* strLog;
	CString strLog_CS;
	strLog=new char[128];
	strLog_CS="";

	sprintf ( strLog, "   从话机读出的.Wifi MAC = \"%x %x %x %x %x %x\".\n",  tWifiMac_R.mac_addr[0],
																			  tWifiMac_R.mac_addr[1],
																			  tWifiMac_R.mac_addr[2],
																			  tWifiMac_R.mac_addr[3],
																			  tWifiMac_R.mac_addr[4],
																			  tWifiMac_R.mac_addr[5]);
	strLog_CS.Format("%s",strLog);

	if ( memcmp ( tWifiMac_W.mac_addr, tWifiMac_R.mac_addr, MAC_ADDR_LEN )) 
	{			
		m_Result.GetWindowText(LOG_Show_Old);  
		m_Result.SetWindowText(strLog_CS+"\r\n写入的与读出的WIFI_MAC不一致，请重新写入！！！\r\n\r\n"+LOG_Show_Old);
		m_Result.UpdateWindow();
		return false;
	}
	else
	{	
		m_Result.GetWindowText(LOG_Show_Old);  
		m_Result.SetWindowText(strLog_CS+"\r\n写入的与读出的WIFI_MAC一致，写WIFI地址确认OK！！！\r\n\r\n"+LOG_Show_Old);
		m_Result.UpdateWindow();
	}
	
	g_MetaModem.eMeta_Result = META_QueryIfFunctionSupportedByTarget_r(/*meta_handle*/0, 500, "META_MISC_EX_BackupCalibrationToStorage_r");

    if(META_SUCCESS == g_MetaModem.eMeta_Result)
	{
		//sprintf ( strLog, ">> Set the SDS\n");
	    if(META_MISC_EX_BackupCalibrationToStorage_r(g_Meta.iMeta_handle,5000,0,&status))
	    {
			//sprintf ( strLog, "   Error: BackupCalibrationToStorage fail, Exit Meta Mode and turn to next phone.\n" );
			m_Result.GetWindowText(LOG_Show_Old);  
			m_Result.SetWindowText("wifi_BackupCalibrationToStorage fail！！！\r\n\r\n"+LOG_Show_Old);
			m_Result.UpdateWindow();
			return false;
	    }
		else
		{
	        //sprintf ( strLog, "   OK: BackupCalibrationToStorage OK, Exit Meta Mode and turn to next phone.\n" );
			m_Result.GetWindowText(LOG_Show_Old);  
			m_Result.SetWindowText("wifi_BackupCalibrationToStorage OK！！！\r\n\r\n"+LOG_Show_Old);
			m_Result.UpdateWindow();
	    }
	}
	return true;//写WIFI_MAC--->OK	
	//METAAPP_DisconnectWithTarget( g_Meta.iMeta_handle );//ExitMetaMode ();
	
}



//////////////////////////////////////////////////////////////
////////////////////////////智能机////////////////////////////
////////////////////////////智能机////////////////////////////
//////////////////////////////////////////////////////////////
//智能机写IMEI号总函数
DWORD CWriteIMEIDlg::func_imei_meta_hdlr_smart_phone_modem (WPARAM, LPARAM)//func_four_in_one_hdlr_smart_phone
{
	extern COLORREF gColor;
	extern CString gFinalResult;

	IMEISV_struct_T tIMEIScanBack,tImei_Svn_Pad;
	//unsigned char strSvn[4];
	unsigned char i;
	unsigned int BackupSecondIMEI;
	unsigned short int p_spare, iLoop = 0;
	unsigned int status;
	
	bool IMEI_Write_Result=true;
	if ( IsNvramDatabaseInit () )
	{
		//sprintf ( strLog, "   Error: Init Modem Nvram Database failed.\n");
		IMEI_Write_Result=false;
		m_Result.GetWindowText(LOG_Show_Old);  
		m_Result.SetWindowText("智能机初始化Modem Nvram database 参数失败！！！\r\n\r\n"+LOG_Show_Old);
		m_Result.UpdateWindow();
		gColor = RGB(255,0,0);
		gFinalResult.Format("     Fail");
		RedrawWindow();  m_Result.UpdateWindow(); 
		IMEI_Control.SetFocus();
		IMEI_Control.SetSel(0,-1);
		return 1;
	}
	else
	{
		m_Result.GetWindowText(LOG_Show_Old);  
		m_Result.SetWindowText("智能机初始化Modem database 参数成功！\r\n\r\n"+LOG_Show_Old);
		m_Result.UpdateWindow();
	}

	if((BT_MAC_ADDR_Choose_M=="true")||(WIFI_MAC_ADDR_Choose_M=="true"))
	{
		if ( IsAPNvramDatabaseInit() ) 
		{ 
			IMEI_Write_Result=false;
			m_Result.GetWindowText(LOG_Show_Old);  
			m_Result.SetWindowText("Init AP Nvram Database失败！！！...\r\n\r\n"+LOG_Show_Old);
			m_Result.UpdateWindow();
			gColor = RGB(255,0,0);
			gFinalResult.Format("     Fail");
			RedrawWindow();  m_Result.UpdateWindow(); 
			IMEI_Control.SetFocus();
			IMEI_Control.SetSel(0,-1);
			return 1;
		}
		else
		{
			m_Result.GetWindowText(LOG_Show_Old);  
			m_Result.SetWindowText("智能机初始化 AP Nvram database 参数成功！\r\n\r\n"+LOG_Show_Old);
			m_Result.UpdateWindow();
		}
	}
	
	BackupSecondIMEI=1;        //BackupSecondIMEI = read_BackupFlag();  // for smartphone, we must do it
	Seg_MetaModemConnectReq ();//智能机
	m_Result.GetWindowText(LOG_Show_Old);  
	m_Result.SetWindowText("完成META相关配置开始与智能机通信...\r\n\r\n"+LOG_Show_Old);
	m_Result.UpdateWindow();

	int IMEI_Count_plus=0;
	if(IMEI1_Choose_M=="true")
	{
		IMEI_Count_plus++;
		strcpy(Imei[0],m_IMEI);
	}
	if(IMEI2_Choose_M=="true")
	{
		IMEI_Count_plus++;
		strcpy(Imei[1],m_IMEI2);
	}
	if(IMEI3_Choose_M=="true")
	{
		IMEI_Count_plus++;
		strcpy(Imei[2],m_IMEI3);
	}
	if(IMEI4_Choose_M=="true")
	{
		IMEI_Count_plus++;
		strcpy(Imei[3],m_IMEI4);
	}
		
	if ( EnterMetaModemMode () ) 
	{
		//METAAPP_DisconnectWithTarget ( g_Meta.iMeta_handle );
		//META_ShutDownTarget_r ( g_Meta.iMeta_handle );
		IMEI_Write_Result=false;
		m_Result.GetWindowText(LOG_Show_Old);  
		m_Result.SetWindowText("连接智能手机失败，退出META模式！！！\r\n\r\n"+LOG_Show_Old);
		m_Result.UpdateWindow();
		gColor = RGB(255,0,0);
		gFinalResult.Format("     Fail");
		RedrawWindow();  m_Result.UpdateWindow(); 
		IMEI_Control.SetFocus();
		IMEI_Control.SetSel(0,-1);
		return 1;
	}
	//sprintf ( strLog, "   OK: Connect successfully.\n" );

	for ( i = 0; i< IMEI_Count_plus; i++ )//N为IMEI个数
	{
		strcpy(tImei_Svn_Pad.imei, Imei[i]);

		if (1)
		{	
			m_Result.GetWindowText(LOG_Show_Old);  
			m_Result.SetWindowText("连接手机成功，读取SVN、PAD...\r\n\r\n"+LOG_Show_Old);
			m_Result.UpdateWindow();
			if ( METAAPP_readIMEIfromNVRAM_Ex3 ( g_Meta.iMeta_handle, 5000, i+1, &tIMEIScanBack))
			{
				IMEI_Write_Result=false;
				//METAAPP_DisconnectWithTarget( g_Meta.iMeta_handle ); 
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("读取SVN、PAD失败，退出META模式！！！\r\n\r\n"+LOG_Show_Old);
				break;
		
			}
						
			tImei_Svn_Pad.pad = tIMEIScanBack.pad ;
			tImei_Svn_Pad.svn = tIMEIScanBack.svn ;
		} 
		else 
		{

			tImei_Svn_Pad.pad = '0';
			tImei_Svn_Pad.svn = '0';//GetSVNPAD().svn;
		}

		CString pad_CS,svn_CS;


		m_Result.GetWindowText(LOG_Show_Old);  
		m_Result.SetWindowText("智能机读取SVN、PAD成功！！！\r\n\r\nsvn="+svn_CS+"\r\npad="+pad_CS+"\r\n"+LOG_Show_Old);
		m_Result.UpdateWindow();
		if(IMEI_CheckValid_Choose_M=="true")
		{
			METAAPP_Calcualte_IMEI_Spare ( g_Meta.iMeta_handle, tImei_Svn_Pad.imei, &p_spare);
			tImei_Svn_Pad.imei [14] = p_spare + '0';
		}
		//sprintf ( strLog,  ">>Step: Start to write. CheckSum=%d, IMEILock=%d.\n", GetIsImeiCheckSum (), GetIsImeiLock());
		if(IMEI_CheckValid_Choose_M=="true")                     
			g_Meta6516Modem.eMetaApp_Result = METAAPP_writeIMEItoNVRAM_Ex3( g_Meta.iMeta_handle, 5000, 1, 0, i+1, &tImei_Svn_Pad);
		else
			g_Meta6516Modem.eMetaApp_Result = METAAPP_writeIMEItoNVRAM_Ex3( g_Meta.iMeta_handle, 5000, 0, 0, i+1, &tImei_Svn_Pad);

		CString Imei_temp;//显示用处
		Imei_temp.Format("%s",Imei[i]);
		switch ( g_Meta6516Modem.eMeta_Result )
    	{
			case METAAPP_SUCCESS:
				//sprintf ( strLog, "   OK: Write IMEI successfully.\n"  );
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("写IMEI:"+Imei_temp+"号成功！！！\r\n\r\n"+LOG_Show_Old);
				m_Result.UpdateWindow();
				break;
		
			case METAAPP_NVRAM_COMPOSE_IMEI_FAIL:
				//不会出现此情况
				IMEI_Write_Result=false;
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("写IMEI:"+Imei_temp+"号不校验位失败，写号不成功，此情况一般不出现！！！\r\n\r\n"+LOG_Show_Old);
				m_Result.UpdateWindow();
				break;
			default: 
				IMEI_Write_Result=false;
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("写IMEI:"+Imei_temp+"号失败！！！\r\n\r\n"+LOG_Show_Old);
				m_Result.UpdateWindow();
				break;	
		}
	} // for (;;)

	if((IMEI1_Choose_M=="true")||(IMEI2_Choose_M=="true")||(IMEI3_Choose_M=="true")||(IMEI4_Choose_M=="true"))//有选择写IMEI号，才进行下面操作
	{
		if (IMEI_Write_Result==true )
		{
			g_MetaModem.eMeta_Result = META_QueryIfFunctionSupportedByTarget_r(0, 500, "META_MISC_EX_BackupCalibrationToStorage_r");
			if(METAAPP_SUCCESS == g_MetaModem.eMeta_Result)
			{
				//sprintf ( strLog, ">> Set the SDS\n");
				if(META_MISC_EX_BackupCalibrationToStorage_r(g_Meta.iMeta_handle,5000,0,&status))
				{
					//sprintf ( strLog, "   Error: BackupCalibrationToStorage fail, Exit Meta Mode and turn to next phone.\n" );
					IMEI_Write_Result=false;
					m_Result.GetWindowText(LOG_Show_Old);  
					m_Result.SetWindowText("BackupCalibrationToStorage fail！！！\r\n\r\n"+LOG_Show_Old);
				}
				else
				{
					//sprintf ( strLog, "   OK: BackupCalibrationToStorage OK, Exit Meta Mode and turn to next phone.\n" );
					m_Result.GetWindowText(LOG_Show_Old);  
					m_Result.SetWindowText("BackupCalibrationToStorage OK！！！\r\n\r\n"+LOG_Show_Old);
					m_Result.UpdateWindow();

				}
			}
		}
	}

	////////////////////////////////////////////////
	////////////////////////////////////读取IME检查
	CString m_IMEI_Read;
	CString m_IMEI2_Read;
	CString m_IMEI3_Read;
	CString m_IMEI4_Read;
	for ( i = 0; i< IMEI_Count_plus; i++ )//N为IMEI个数
	{
		g_MetaModem.eMetaApp_Result = METAAPP_readIMEIfromNVRAM_Ex3( g_Meta.iMeta_handle, 5000,i+1, &tImei_Svn_Pad);
		
		CString IMEI_Count_plus_CS;
		IMEI_Count_plus_CS.Format("%d",i+1);//第几个IMEI
		switch ( g_MetaModem.eMetaApp_Result )
    	{
			case METAAPP_SUCCESS:
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("读取第( "+IMEI_Count_plus_CS+" )个IMEI号成功！\r\n\r\n"+LOG_Show_Old);
				m_Result.UpdateWindow();
				break;
		
			case METAAPP_NVRAM_COMPOSE_IMEI_FAIL:
				//不会出现此情况
				IMEI_Write_Result=false;
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("读取第( "+IMEI_Count_plus_CS+" )个IMEI号失败！！！\r\n\r\n"+LOG_Show_Old);
				m_Result.UpdateWindow();
				break;
				
			default: 
				IMEI_Write_Result=false;
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("读取第( "+IMEI_Count_plus_CS+" )个IMEI号失败！！！\r\n\r\n"+LOG_Show_Old);
				m_Result.UpdateWindow();
				break;	
		}

		if(IMEI_Write_Result==true )//读取IMEI成功
		{
			strcpy(Imei[i],tImei_Svn_Pad.imei);
			if(i == 0)
			{
				m_IMEI_Read.Format("%s",Imei[i]);
				if(m_IMEI_Read!=m_IMEI)
				{
					IMEI_Write_Result=false;
					m_Result.GetWindowText(LOG_Show_Old);  
					m_Result.SetWindowText("读取第( "+IMEI_Count_plus_CS+" )个IMEI号:"+m_IMEI_Read+"与输入的IMEI:"+m_IMEI+"不相同=======失败！！！\r\n\r\n"+LOG_Show_Old);
					m_Result.UpdateWindow();
				}
				else
				{
					m_Result.GetWindowText(LOG_Show_Old);  
					m_Result.SetWindowText("读取第( "+IMEI_Count_plus_CS+" )个IMEI号:"+m_IMEI_Read+"与输入的IMEI:"+m_IMEI+"一致=======成功继续...\r\n\r\n"+LOG_Show_Old);
					m_Result.UpdateWindow();
				}
			}
			else if(i == 1)
			{
				m_IMEI2_Read.Format("%s",Imei[i]);
				if(m_IMEI2_Read!=m_IMEI2)
				{
					IMEI_Write_Result=false;
					m_Result.GetWindowText(LOG_Show_Old);  
					m_Result.SetWindowText("读取第( "+IMEI_Count_plus_CS+" )个IMEI号:"+m_IMEI2_Read+"与输入的IMEI:"+m_IMEI2+"不相同=======失败！！！\r\n\r\n"+LOG_Show_Old);
					m_Result.UpdateWindow();
				}
				else
				{
					m_Result.GetWindowText(LOG_Show_Old);  
					m_Result.SetWindowText("读取第( "+IMEI_Count_plus_CS+" )个IMEI号:"+m_IMEI2_Read+"与输入的IMEI:"+m_IMEI2+"一致=======成功继续...\r\n\r\n"+LOG_Show_Old);
					m_Result.UpdateWindow();
				}
			}
			else if(i == 2)
			{
				m_IMEI3_Read.Format("%s",Imei[i]);
				if(m_IMEI3_Read!=m_IMEI3)
				{
					IMEI_Write_Result=false;
					m_Result.GetWindowText(LOG_Show_Old);  
					m_Result.SetWindowText("读取第( "+IMEI_Count_plus_CS+" )个IMEI号:"+m_IMEI3_Read+"与输入的IMEI:"+m_IMEI3+"不相同=======失败！！！\r\n\r\n"+LOG_Show_Old);
					m_Result.UpdateWindow();
				}
				else
				{
					m_Result.GetWindowText(LOG_Show_Old);  
					m_Result.SetWindowText("读取第( "+IMEI_Count_plus_CS+" )个IMEI号:"+m_IMEI3_Read+"与输入的IMEI:"+m_IMEI3+"一致=======成功继续...\r\n\r\n"+LOG_Show_Old);
					m_Result.UpdateWindow();
				}
			}
			else if(i == 3)
			{
				m_IMEI4_Read.Format("%s",Imei[i]);
				if(m_IMEI4_Read!=m_IMEI4)
				{
					IMEI_Write_Result=false;
					m_Result.GetWindowText(LOG_Show_Old);  
					m_Result.SetWindowText("读取第( "+IMEI_Count_plus_CS+" )个IMEI号:"+m_IMEI4_Read+"与输入的IMEI:"+m_IMEI4+"不相同=======失败！！！\r\n\r\n"+LOG_Show_Old);
					m_Result.UpdateWindow();
				}
				else
				{
					m_Result.GetWindowText(LOG_Show_Old);  
					m_Result.SetWindowText("读取第( "+IMEI_Count_plus_CS+" )个IMEI号:"+m_IMEI4_Read+"与输入的IMEI:"+m_IMEI4+"一致=======成功继续...\r\n\r\n"+LOG_Show_Old);
					m_Result.UpdateWindow();
				}
			}
			else
				m_IMEI_Read="NULL";
		}
	}
	////////////////////////////////////读取IME检查		
	///////////////////////////////////////////////
	//读取MTK平台的板号
	if(BARCODE_Choose_M=="true")//MTK智能机
    {
		char barcode[90];
		memset(barcode,0x0,sizeof(barcode));
        if (METAAPP_SUCCESS!=METAAPP_readBarcodefromNVRAM(g_Meta.iMeta_handle, 5000, barcode) )
        {
			IMEI_Write_Result=false;
			m_Result.GetWindowText(LOG_Show_Old);  
			m_Result.SetWindowText("\r\n读手机板号失败！！！\r\n\r\n"+LOG_Show_Old);
			m_Result.UpdateWindow();
        }
        else
        {
			BARCODE_Input.Format("%s",barcode);
			BARCODE_Input=BARCODE_Input.Left(15);//限制板号的长度
			BARCODE_Input_Control.SetWindowTextA(BARCODE_Input);;
			m_Result.GetWindowText(LOG_Show_Old);  
			m_Result.SetWindowText("\r\n读手机板号成功！\r\n\r\n板号为:"+BARCODE_Input+"\r\n"+LOG_Show_Old);
			if(Check_repeat_Choose_M=="true")
			{
				CString DB_FAIL="";
				CString Conn="";
				Conn.Format("driver={SQL Server};Server=%s;DATABASE=%s;UID=%s;PWD=%s", m_server_M,m_db_M,m_user_M, m_pwd_M);
				if(Setdlg.myado.m_pCon==NULL)
				{
					DB_FAIL=Setdlg.myado.ConnecDataLibrary(Conn,"","",adModeUnknown);   
				}
				if(Setdlg.myado.m_pCon->State==0)  //1表示已经打开,0表示关闭，数据库意外断开，重连
				{
					IMEI_Write_Result=false;
					m_Result.GetWindowText(LOG_Show_Old);  
					m_Result.SetWindowText("=================================数据库意外断开，重连........\r\n\r\n"+LOG_Show_Old);
					m_Result.UpdateWindow();
					DB_FAIL=Setdlg.myado.ConnecDataLibrary(Conn,"","",adModeUnknown);//数据库意外断开，重连
				}
				else
				{
					m_Result.GetWindowText(LOG_Show_Old);  
					m_Result.SetWindowText("=================================数据库连接正常中...\r\n\r\n"+LOG_Show_Old);
					m_Result.UpdateWindow();
				}

				if(DB_FAIL=="FAIL")
				{
					IMEI_Write_Result=false;
					m_Result.GetWindowText(LOG_Show_Old);
					m_Result.SetWindowText("连接数据库失败，无法检查板号的重复性，请检查网络.......\r\n\r\n"+LOG_Show_Old);
					m_Result.UpdateWindow();
				}
				else
				{
					/////////连接数据库检查板号是否重复///////////////////////////////	
					Setdlg.myado.OpenSheet("select barcode from dse_barcode_imei WHERE barcode ='"+BARCODE_Input+"'");
					BOOL IMEI_Check=Setdlg.myado.Find("barcode='"+BARCODE_Input+"'");
					Setdlg.myado.CloseSheet();
					if(IMEI_Check==TRUE)
					{
						IMEI_Write_Result=false;
						m_Result.GetWindowText(LOG_Show_Old);  
						m_Result.SetWindowText("=================================板号:"+BARCODE_Input+"   重复！！数据不上传，请检查后==重新写号！！！\r\n\r\n"+LOG_Show_Old);
						m_Result.UpdateWindow();
					}
					else
					{
						m_Result.GetWindowText(LOG_Show_Old);  
						m_Result.SetWindowText("==================================板号:"+BARCODE_Input+"   数据库正常\r\n\r\n"+LOG_Show_Old);
						m_Result.UpdateWindow();
					}
					/////////////////////////////////////////
				}
			}	
		}
	}
	if((Software_Version_Check_Choose_M=="true"))   //MTK智能机
	{
		signed short int Version_token;
		if(META_SUCCESS!=META_GetTargetVerInfo_r(g_Meta.iMeta_handle,CNF_Read_Barcode,&Version_token,NULL))
		{
			IMEI_Write_Result=false;
			m_Result.GetWindowText(LOG_Show_Old);
			m_Result.SetWindowText("读软件版本失败！！！\r\n\r\n"+LOG_Show_Old);
			m_Result.UpdateWindow();
		}
		else
		{
			CString  strTemp;
			strTemp=S_HW_Version;
			strTemp.Replace("\r","");
			strTemp.Replace("\n","");
			strTemp.Replace(" ","---");
			if(NULL==_tcsstr(strTemp,Software_Version_M))
			{
				IMEI_Write_Result=false;
				m_Result.GetWindowText(LOG_Show_Old);
				m_Result.SetWindowText("软件版本不匹配！！！:\r\n话机软件版本为："+strTemp+"\r\n设置软件版本为："+Software_Version_M+"\r\n"+LOG_Show_Old);
				m_Result.UpdateWindow();
			}   
			else
			{
				m_Result.GetWindowText(LOG_Show_Old);
				m_Result.SetWindowText("软件版本匹配:\r\n话机软件版本为："+strTemp+"\r\n设置软件版本为："+Software_Version_M+"\r\n"+LOG_Show_Old);
				m_Result.UpdateWindow();
			}
		}
	}
	// very important.
	// write second IMEI in smart phone ! means 1048 MP load. modify 20101228 for sp nvram backup. EnterMetaAPMode
	if((1 == BackupSecondIMEI)) //必须执行
	{
		//sprintf ( strLog, "\n>> Step: To switch Meta to AP side...\n" );
		{
			g_Meta6516Modem.eMeta_Result = META_DisconnectInMetaMode_r ( g_Meta.iMeta_handle );                
			switch ( g_Meta6516Modem.eMeta_Result )
			{
				case METAAPP_SUCCESS:
					m_Result.GetWindowText(LOG_Show_Old);  
					m_Result.SetWindowText("META_DisconnectInMetaMode_r OK!\r\n\r\n"+LOG_Show_Old);
					m_Result.UpdateWindow();
					break;
					
				default: 
					IMEI_Write_Result=false;
					m_Result.GetWindowText(LOG_Show_Old);  
					m_Result.SetWindowText("META_DisconnectInMetaMode_r FAIL！！！\r\n\r\n"+LOG_Show_Old);
					m_Result.UpdateWindow();
					break;	
			}
		}

		//if ( Switch2WM_META_Mode ( g_Meta.iMeta_handle ) )   //成功返回false 相当于if ( EnterMetaAPMode () ) 
		if(EnterMetaAPMode())//EnterMetaModemMode
		{
			IMEI_Write_Result=false;
			m_Result.GetWindowText(LOG_Show_Old);  
			m_Result.SetWindowText("Switch2WM_META_Mode 切换模式失败！！！\r\n\r\n"+LOG_Show_Old);
			m_Result.UpdateWindow();
		}
		else
		{
			m_Result.GetWindowText(LOG_Show_Old);  
			m_Result.SetWindowText("Switch2WM_META_Mode 切换模式成功!\r\n\r\n"+LOG_Show_Old);
			m_Result.UpdateWindow();
		}

		
		//sprintf ( strLog, "\n>>Step: To Backup NVRAM to Bin Region, and Set Clean Boot Flag\n" );
		Sleep(200);
		//if ( META_SUCCESS != WM_META_SetCleanBootFlag_Customize ( g_Meta.iMeta_handle ) )   ///// Clean boot is very Importnant for Customer.
		if(META_SUCCESS!=SP_META_SetCleanBootFlag_r ( g_Meta.iMetaAP_handle, 5000, &req, &cnf ))
		{
			IMEI_Write_Result=false;
			m_Result.GetWindowText(LOG_Show_Old);  
			m_Result.SetWindowText("IMEI---Fail to Backup NVRAM and WM_META_SetCleanBootFlag_Customize！！！\r\n\r\n"+LOG_Show_Old);
			m_Result.UpdateWindow();
		}
		else
		{
			m_Result.GetWindowText(LOG_Show_Old);  
			m_Result.SetWindowText("IMEI---Success to Backup NVRAM and WM_META_SetCleanBootFlag_Customize...继续...\r\n\r\n"+LOG_Show_Old);
			m_Result.UpdateWindow();
		}
		Sleep(100);
		
		
		//写BT地址：

		if((IMEI_Write_Result==true )&&(BT_MAC_ADDR_Choose_M=="true"))//写IMEI成功，或者不写IMEI
		{
			IMEI_Write_Result&=func_bt_address_meta_hdlr_smart_phone();//成功返回true
		}
		//写WIFI地址：
		if((IMEI_Write_Result==true )&&(WIFI_MAC_ADDR_Choose_M=="true"))//写IMEI成功、写BT地址成功，或者不写IMEI、不写BT地址
		{
			IMEI_Write_Result&=func_wifi_mac_meta_hdlr_smart_phone_Ex ();//成功返回true
		}

		
		
		
		
		if(META_SUCCESS==SP_META_DisconnectWithTarget_r ( g_Meta.iMetaAP_handle ))//Exit_MetaAPMode ();	
		{
			m_Result.GetWindowText(LOG_Show_Old);  
			m_Result.SetWindowText("disconnect form AP meta mode Success ,and turn to next phone！\r\n\r\n"+LOG_Show_Old);
			m_Result.UpdateWindow();
		}
		else
		{
			IMEI_Write_Result=false;
			m_Result.GetWindowText(LOG_Show_Old);  
			m_Result.SetWindowText("disconnect form AP meta mode FAIL！！！请取下电池后重试\r\n\r\n"+LOG_Show_Old);
			m_Result.UpdateWindow();
		}
		
		/*SP_META_Deinit_r ( &g_Meta.iMetaAP_handle );//关闭软件的时候执行，或者每次重新执行SP_META_Init_r
		{
			m_Result.GetWindowText(LOG_Show_Old);  
			m_Result.SetWindowText("SP_META_Deinit_r成功！\r\n\r\n"+LOG_Show_Old);
			m_Result.UpdateWindow();
		}*/
	}

	////////////////////////////////////////////////////////////////////
	////////////////////////////此情况不执行==============暂时不考虑
	else //write one IMEI here
	{
		g_Meta6516Modem.eMetaApp_Result = METAAPP_DisconnectWithTarget( g_Meta.iMeta_handle );//ExitMetaMode();
		if(g_Meta6516Modem.eMetaApp_Result == METAAPP_SUCCESS)
		{
			m_Result.GetWindowText(LOG_Show_Old);  
			m_Result.SetWindowText("disconnect form AP meta mode Success！\r\n\r\n"+LOG_Show_Old);
			m_Result.UpdateWindow();
		}
		else
		{
			IMEI_Write_Result=false;
			m_Result.GetWindowText(LOG_Show_Old);  
			m_Result.SetWindowText("disconnect form AP meta mode FAIL！！！\r\n\r\n"+LOG_Show_Old);
			m_Result.UpdateWindow();
		}
	}
	/////////////////////////////////此情况不执行==============暂时不考虑
	////////////////////////////////////////////////////////////////////////
	
	////写号结束/////////////////////////////
	


	if (IMEI_Write_Result==true )//写号成功
	{
        gColor = RGB(0,255,0);
		gFinalResult.Format("     OK~");
		RedrawWindow();  m_Result.UpdateWindow(); 
		if(UP_DB_Data_M=="false")//不上传数据库
		{
			m_Result.GetWindowText(LOG_Show_Old);
			m_Result.SetWindowText("成功写号！！！\r\n\r\n"+LOG_Show_Old);
			//UpdateData(true);  //把中间值赋给变量
			//UpdateData(false); //把变量显示在界面
			m_Result.UpdateWindow();

			///////自动生成计数处理/////////////////////
			if((AutoBTAddr_Choose_M=="true")||(AutoWIFIAddr_Choose_M=="true"))
				BtAddrCount_int++;
			BtAddrCount.Format("%d",BtAddrCount_int);

			IMEI_Count.Format("%d",IMEI_Count_int);  //IMEI_Count_int 已经递增
			SaveConfig_Change();
			//////////自动生成计数处理//////////////////

			gColor = RGB(0,255,0);
			gFinalResult.Format("     OK");
			RedrawWindow();  m_Result.UpdateWindow(); 
			//OnSelchangeAuto();    //不将IMEI1清零
			//IMEI_Control.SetFocus();
			//IMEI_Control.SetSel(0,-1);
			
		}
		else if(Initial_Connect_DB_M=="true")//上传数据库//初始化连接了数据库才能上传，权限限制
		{
			CString Conn="";
			CString DB_FAIL="";
			Conn.Format("driver={SQL Server};Server=%s;DATABASE=%s;UID=%s;PWD=%s", m_server_M,m_db_M,m_user_M, m_pwd_M);
			/*if(   (DB_CONNECT_M=="true")&&(IMEI_DB_Choose_M=="false")&&(MAC_DB_Choose_M=="false")&&(Check_repeat_Choose_M=="false")   ||   (ORACLE_Connect_Choose_M=="true")&&(UP_DB_Data_M=="true")&&(IMEI_DB_Choose_M=="false")&&(MAC_DB_Choose_M=="false")&&(Check_repeat_Choose_M=="false")    ) //每次操作数据库时候，重连一次,并且要除了数据库关联的情况（这时候已经连接一次）、除了检查重复性
				DB_FAIL=Setdlg.myado.ConnecDataLibrary(Conn,"","",adModeUnknown);//写号成功上传数据库，重连
			else
				DB_FAIL="SUCCESS";
			*/
			if(Setdlg.myado.m_pCon==NULL)
			{
				DB_FAIL=Setdlg.myado.ConnecDataLibrary(Conn,"","",adModeUnknown);   
			}
			if(Setdlg.myado.m_pCon->State==0)  //1表示已经打开,0表示关闭，数据库意外断开，重连
			{
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("=================================数据库意外断开，重连........\r\n\r\n"+LOG_Show_Old);
				m_Result.UpdateWindow();
				DB_FAIL=Setdlg.myado.ConnecDataLibrary(Conn,"","",adModeUnknown);//数据库意外断开，重连
			}
			else
			{
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("=================================数据库连接正常中...\r\n\r\n"+LOG_Show_Old);
				m_Result.UpdateWindow();
			}

			if(DB_FAIL=="FAIL")
			{
				IMEI_Control.SetFocus();
				IMEI_Control.SetSel(0,-1);
				m_Result.GetWindowText(LOG_Show_Old);
				m_Result.SetWindowText("连接数据库失败，无法上传数据，请检查网络.......\r\n\r\n"+LOG_Show_Old);

				gColor = RGB(255,0,0);
				gFinalResult.Format("UP_FAIL");
				RedrawWindow();  m_Result.UpdateWindow(); 
				return 1;
			}

			CTime CurrentTime=CTime::GetCurrentTime();
			CString strTime;
			strTime =CurrentTime.Format( "%Y-%m-%d %H:%M:%S ");
			//CString strSQL = "Insert into dse_barcode_imei(imei,imei2,imei3,imei4,IMEI_NUM,barcode,msn,WIP_ENTITY_NAME,TYPE,MOBCLASS,LINENUM,classnum,PRODADDRESS,create_time,is_return,SOFTVERSION,HARDVERSION,IMEI_WRITER,WIFI_MAC,BLUE_MAC)\
							 values('"+m_IMEI+"','"+m_IMEI2+"','"+m_IMEI3+"','"+m_IMEI4+"','"+IMEI_Num+"','"+BARCODE_Input+"','"+m_SN+"','"+Order_Num_up_M+"','"+Machine_Type_M+"','"+Modulation_Tppe_M+"','"+Line_Number_M+"','"+Floor_M+"','"+Product_Address_M+"','"+strTime+"','"+Is_Return_M+"','"+Software_Version_M+"','"+Hardware_Version_M+"','"+Operator_M+"','"+m_strWifi+"','"+m_BT_Addr+"')";	
			//CString strSQL = "Insert into dse_barcode_imei(imei,imei2,imei3,imei4,barcode,msn,WIP_ENTITY_NAME,TYPE,MOBCLASS,LINENUM,classnum,PRODADDRESS,create_time,is_return,SOFTVERSION,HARDVERSION,IMEI_WRITER,WIFI_MAC,BLUE_MAC)\
							 values('"+m_IMEI+"','"+m_IMEI2+"','"+m_IMEI3+"','"+m_IMEI4+"','"+BARCODE_Input+"','"+m_SN+"','"+Order_Num_up_M+"','"+Machine_Type_M+"','"+Modulation_Tppe_M+"','"+Line_Number_M+"','"+Floor_M+"','"+Product_Address_M+"','"+strTime+"','"+Is_Return_M+"','"+Software_Version_M+"','"+Hardware_Version_M+"','"+Operator_M+"','"+m_strWifi+"','"+m_BT_Addr+"')";			
			//CString strSQL = "Insert into dse_barcode_imei(imei,IMEI2,IMEI3,IMEI4,barcode,msn,WIP_ENTITY_NAME,TYPE,LINENUM,create_time,is_return,SOFTVERSION,HARDVERSION,WIFI_MAC,BLUE_MAC)\
							 values('"+m_IMEI+"','"+m_IMEI2+"','"+m_IMEI3+"','"+m_IMEI4+"','"+BARCODE_Input+"','"+m_SN+"','"+Order_Num_up_M+"','"+Machine_Type_M+"','"+Line_Number_M+"','"+strTime+"','"+Is_Return_M+"','"+Software_Version_M+"','"+Hardware_Version_M+"','"+m_strWifi+"','"+m_BT_Addr+"')";		
			
			///////////////////解锁码///////////////////////////////////////
			if(Unlock_Code_Choose_M=="true")
			{
				TranslateMuch(m_IMEI);
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("获取IMEI："+m_IMEI+"对应的解锁码："+Unlock_code_Number+"\r\n"+LOG_Show_Old);
				m_Result.UpdateWindow();
			}
			///////////////////解锁码///////////////////////////////////////

			//CString strSQL = "Insert into dse_barcode_imei(imei,IMEI_NUM,barcode,msn,WIP_ENTITY_NAME,TYPE,MOBCLASS,LINENUM,classnum,PRODADDRESS,create_time,is_return,SOFTVERSION,HARDVERSION,IMEI_WRITER,WIFI_MAC,BLUE_MAC) values('"+m_IMEI+"','"+IMEI_Num+"','"+BoardNumber+"','"+m_SN+"','"+Order_Num_up_M+"','"+Machine_Type_M+"','"+Modulation_Tppe_M+"','"+Line_Number_M+"','"+Floor_M+"','"+Product_Address_M+"','"+strTime+"','"+Is_Return_M+"','"+Software_Version_M+"','"+Hardware_Version_M+"','"+Operator_M+"','','"+m_BT_Addr+"')";
			CString strSQL = "Insert into dse_barcode_imei(imei,NetCode,Unlock_Code,IMEI2,IMEI3,IMEI4,barcode,msn,WIP_ENTITY_NAME,TYPE,LINENUM,create_time,is_return,SOFTVERSION,HARDVERSION,WIFI_MAC,BLUE_MAC)\
							 values('"+m_IMEI+"','"+NETCODE_Input+"','"+Unlock_code_Number+"','"+m_IMEI2+"','"+m_IMEI3+"','"+m_IMEI4+"','"+BARCODE_Input+"','"+m_SN+"','"+Order_Num_up_M+"','"+Machine_Type_M+"','"+Line_Number_M+"','"+strTime+"','"+Is_Return_M+"','"+Software_Version_M+"','"+Hardware_Version_M+"','"+m_strWifi+"','"+m_BT_Addr+"')";	
			
			
			Setdlg.myado.OpenSheet("select * from dse_barcode_imei");
			BOOL UP_FAIL=Setdlg.myado.Execute(strSQL);
			Setdlg.myado.CloseSheet();
			Sleep(500);
			Setdlg.myado.OpenSheet("select imei from dse_barcode_imei WHERE imei ='"+m_IMEI+"'");
			BOOL IMEI_Check_UP=Setdlg.myado.Find("imei='"+m_IMEI+"'");
			Setdlg.myado.CloseSheet();
			
			BOOL UP_WritedFlag;
			if((UP_FAIL==TRUE&&IMEI_Check_UP==TRUE))
			{
				

				//AfxMessageBox("上传SQL成功！");
				m_Result.GetWindowText(LOG_Show_Old);
				m_Result.SetWindowText("成功写号！！！并且成功上传SQL_SERVER数据库！！！\r\n\r\n"+LOG_Show_Old);
				if(UpdateIMEI_MSN_Choose_M=="true")
				{
					CString strSQL_WritedFlag="UPDATE dse_imei_sn SET WritedFlag = '1' WHERE imei ='"+m_IMEI+"'";
					Setdlg.myado.OpenSheet("select * from dse_imei_sn WHERE imei ='"+m_IMEI+"'");
					UP_WritedFlag=Setdlg.myado.Execute(strSQL_WritedFlag);
					Setdlg.myado.CloseSheet();

					if(UP_WritedFlag==TRUE)
					{
						m_Result.GetWindowText(LOG_Show_Old);
						m_Result.SetWindowText("更新贴纸表标志成功！\r\n\r\n"+LOG_Show_Old);
					}
					else
					{
						m_Result.GetWindowText(LOG_Show_Old);
						m_Result.SetWindowText("更新贴纸表标志失败！！！\r\n\r\n"+LOG_Show_Old);
					}
					//UpdateData(true);  //把中间值赋给变量
					//UpdateData(false); //把变量显示在界面
					m_Result.UpdateWindow();
				}
				else
				{
					m_Result.GetWindowText(LOG_Show_Old);
					m_Result.SetWindowText("没有勾选更新贴纸表标志..\r\n\r\n"+LOG_Show_Old);
					m_Result.UpdateWindow();
					UP_WritedFlag=TRUE;
				}
				bool ORACLE_UpDatabase_RESULT=true;//上传ORACLE数据库结果
			    if(ORACLE_Connect_Choose_M=="true")//上传ORACLE数据库
				{
					ORACLE_UpDatabase_RESULT=Update_Oracle_Database();
				}
				if((ORACLE_UpDatabase_RESULT==true)&&(UP_WritedFlag==TRUE))
				{
					///////自动生成计数处理/////////////////////
					BtAddrCount_int++;
					BtAddrCount.Format("%d",BtAddrCount_int);

					IMEI_Count.Format("%d",IMEI_Count_int);  //IMEI_Count_int 已经递增
					SaveConfig_Change();
					//////////自动生成计数处理//////////////////

					gColor = RGB(0,255,0);
					gFinalResult.Format("     OK");
					RedrawWindow();  m_Result.UpdateWindow(); 
					//OnSelchangeAuto();    //不将IMEI1清零
					//IMEI_Control.SetFocus();
					//IMEI_Control.SetSel(0,-1);
				}
				else if(UP_WritedFlag!=TRUE)
				{
					m_Result.GetWindowText(LOG_Show_Old);
					m_Result.SetWindowText("成功写号上传数据！！！但更新贴纸表标志失败！！！！！！！！！！！！！\r\n\r\n"+LOG_Show_Old);
					m_Result.UpdateWindow();

					gColor = RGB(255,0,0);
					gFinalResult.Format("UPDA_F");
					RedrawWindow();  m_Result.UpdateWindow(); 
				}
			}
			else
			{
				m_Result.GetWindowText(LOG_Show_Old);
				m_Result.SetWindowText("成功写号！！！但上传数据库失败！！！！！！！！！！！！！\r\n\r\n"+LOG_Show_Old);
				//UpdateData(true);  //把中间值赋给变量
				//UpdateData(false); //把变量显示在界面
				m_Result.UpdateWindow();

				gColor = RGB(255,0,0);
				gFinalResult.Format("UP_FAIL");
				RedrawWindow();  m_Result.UpdateWindow(); 
				//OnSelchangeAuto();    //不将IMEI1清零
				//IMEI_Control.SetFocus();
				//IMEI_Control.SetSel(0,-1);
			}
		}
		else
		{
			m_Result.GetWindowText(LOG_Show_Old);  
			m_Result.SetWindowText("写号成功，但因没有选择   初始化时连接数据库   无法上传数据库\r\n\r\n"+LOG_Show_Old);
			gColor = RGB(255,0,0);
			gFinalResult.Format("     UP_Fail");
			RedrawWindow();  m_Result.UpdateWindow(); 
		}
        //m_strMsg.LoadString( "PASS! 按Enter开始下一次测试");
	}
	else
    {
        m_Result.GetWindowText(LOG_Show_Old);  
		m_Result.SetWindowText("写号失败！！！\r\n\r\n"+LOG_Show_Old);
		gColor = RGB(255,0,0);
		gFinalResult.Format("     Fail");
		RedrawWindow();  m_Result.UpdateWindow(); 
		//IMEI_Control.SetFocus();
		//IMEI_Control.SetSel(0,-1);
	}

	RedrawWindow();  m_Result.UpdateWindow(); 
	IMEI_Control.SetFocus();
	IMEI_Control.SetSel(0,-1);
	//初始化时连接了数据库、每次重连数据库、
	/*if(DB_CONNECT_M=="true")//数据库关闭处理
	{
		Setdlg.myado.CloseDataLibrary();//连接关闭必须在一个子函数内，不然出错
	}*/
	LOG_Show_Old="";
	return 0;
	
}


//智能机----META连接参数设置
void CWriteIMEIDlg::Seg_MetaModemConnectReq ( void ) 
{
//-----------------Baseband chipset select-----------------------
//-----------Boot ROM and Sync with Target ----------------------
	int temp_com_port = 0;
	m_Port.GetWindowText(m_bUsbMode);
	CString m_bUsbMode_temp=m_bUsbMode.Right(m_bUsbMode.GetLength()-3);
	temp_com_port = atoi(m_bUsbMode_temp);
    if (m_bUsbMode=="USB")
    {
        temp_com_port=USB_COM_PORT; //使用USB口
    }
    g_Meta6516Modem.tMETA_Connect_Req.com_port = temp_com_port;
    g_Meta6516Modem.tMETA_Connect_Req.baudrate[0] = META_BAUD115200;
    g_Meta6516Modem.tMETA_Connect_Req.baudrate[1] = META_BAUD460800;
    g_Meta6516Modem.tMETA_Connect_Req.baudrate[2] = META_BAUD921600;
    g_Meta6516Modem.tMETA_Connect_Req.baudrate[3] = META_BAUD_END;	
	
    g_Meta6516Modem.tMETA_Connect_Req.flowctrl = META_SW_FLOWCTRL;
    g_Meta6516Modem.tMETA_Connect_Req.ms_connect_timeout = META_CONNECT_TIME_OUT;

    memset (&m_stModeArg, 0x0, sizeof(SP_BOOT_ARG_S));
    m_stModeArg.m_bbchip_type=SP_AUTO_DETECT_BBCHIP;
    m_stModeArg.m_ext_clock=SP_AUTO_DETECT_EXT_CLOCK;
    m_stModeArg.m_ms_boot_timeout=SP_BOOT_INFINITE;                    //不能是  SP_BOOT_STOP---Seg_MetaModemConnectReq  

    m_stModeArg.m_uBaudrate=CBR_115200;
    m_stModeArg.m_uTimeout=20000;
    m_stModeArg.m_uRetryTime=2000;
    m_stModeArg.m_uInterval=10;
 
	
    m_stModeArg.m_auth_handle=SPATE_m_sAuthHandle;//SPATE_Get_AuthHandle();
    m_stModeArg.m_scert_handle=SPATE_m_sScertHandle;//SPATE_Get_ScertHandle();
    m_stModeArg.m_cb_sla_challenge=SLA_Challenge;
    m_stModeArg.m_cb_sla_challenge_arg=NULL;
    m_stModeArg.m_cb_sla_challenge_end=SLA_Challenge_END;
    m_stModeArg.m_cb_sla_challenge_end_arg=NULL;	
}

bool CWriteIMEIDlg::EnterMetaModemMode ( void )
{
   
	META_ConnectByUSB_Req usbRequest;
    META_ConnectByUSB_Report usbReport;

    int temp_com_port = 0;
	m_Port.GetWindowText(m_bUsbMode);
	CString m_bUsbMode_temp=m_bUsbMode.Right(m_bUsbMode.GetLength()-3);
	temp_com_port = atoi(m_bUsbMode_temp);
    if (m_bUsbMode=="USB")
    {
        temp_com_port=USB_COM_PORT; //使用USB口
    }
	m_Result.GetWindowText(LOG_Show_Old);  
	m_Result.SetWindowText("Search  preloader or brom COM port for META connection  and handshake with preloader.....\r\n\r\n"+LOG_Show_Old);
	m_Result.UpdateWindow();
	m_Result.GetWindowText(LOG_Show_Old);  
	m_Result.SetWindowText("请给手机上电.....\r\n\r\n请给手机上电.....\r\n\r\n"+LOG_Show_Old);
	m_Result.UpdateWindow();
    //CmtScheduleThreadPoolFunction(DEFAULT_THREAD_POOL_HANDLE,Connect_with_preloader,NULL,&functionId); //创建线程   
	Connect_with_preloader();
	if(search_preloader_port_success==0)  
	{
        //sprintf ( strLog,  " COM port searching and preloader handshake  fail\n");	
		m_Result.GetWindowText(LOG_Show_Old);  
		m_Result.SetWindowText("Search  preloader or brom COM port for META connection  and handshake with preloader===失败！\r\n\r\n"+LOG_Show_Old);
		m_Result.UpdateWindow();
		return true;	
	}
	else
	{
		m_Result.GetWindowText(LOG_Show_Old);  
		m_Result.SetWindowText("Search  preloader or brom COM port for META connection  and handshake with preloader===成功！\r\n\r\n"+LOG_Show_Old);
		m_Result.UpdateWindow();
	}




	m_Result.GetWindowText(LOG_Show_Old);  
	m_Result.SetWindowText("kernel  COM port searching and Kernel handshake.....\r\n\r\n"+LOG_Show_Old);
	m_Result.UpdateWindow();
    //CmtScheduleThreadPoolFunction(DEFAULT_THREAD_POOL_HANDLE,search_kernel_port,NULL,&functionId);   //创建线程
	search_kernel_port();
    if(search_kernel_port_success==0)
	{
		m_Result.GetWindowText(LOG_Show_Old);  
		m_Result.SetWindowText("kernel  COM port searching and Kernel handshake===失败！\r\n\r\n"+LOG_Show_Old);
		m_Result.UpdateWindow();
	    return true;
	}
	else
	{
		m_Result.GetWindowText(LOG_Show_Old);  
		m_Result.SetWindowText("kernel  COM port searching and Kernel handshake===成功！\r\n\r\n"+LOG_Show_Old);
		m_Result.UpdateWindow();
	}
	g_Meta.bootstop = BOOT_INFINITE;   //智能机---这样才能进入META模式
    if (m_bUsbMode=="USB")//if(IsUSBCOMPort())
    {
		//Sleep(1000);//delay for USB com port ready
		//memset (&usbRequest, 0x0, sizeof(META_ConnectByUSB_Req));
        usbRequest.com_port = kernel_port;
        usbRequest.ms_connect_timeout = g_Meta6516Modem.tMETA_Connect_Req.ms_connect_timeout;
		g_Meta6516Modem.eMeta_Result = META_ConnectInMetaModeByUSB_r(g_Meta.iMeta_handle, &usbRequest, &g_Meta.bootstop, &usbReport);
		/*const char* ERROR_CODE;
		ERROR_CODE=META_GetErrorString(g_Meta6516Modem.eMeta_Result);
		*/
		//g_Meta6516Modem.tMETA_Connect_Req.com_port = kernel_port;
		//g_Meta6516Modem.eMeta_Result = META_ConnectInMetaMode_r (g_Meta.iMeta_handle, &g_Meta6516Modem.tMETA_Connect_Req, &g_Meta.bootstop, &g_Meta6516Modem.tMETA_Connect_Report); 
        
    }
    else
    { 
	   g_Meta6516Modem.eMeta_Result = META_ConnectInMetaMode_r (g_Meta.iMeta_handle, &g_Meta6516Modem.tMETA_Connect_Req, &g_Meta.bootstop, &g_Meta6516Modem.tMETA_Connect_Report); 
    }

	switch ( g_Meta6516Modem.eMeta_Result )
	{
		case META_SUCCESS:
			 return false; 
			 break;

		case META_INCORRECT_TARGET_VER:
			m_Result.GetWindowText(LOG_Show_Old);  
			m_Result.SetWindowText("Modem NVRAM database inconsistent with target...\r\n\r\n"+LOG_Show_Old);
			m_Result.UpdateWindow();
			break;

		case META_COMM_FAIL:
			m_Result.GetWindowText(LOG_Show_Old);  
			m_Result.SetWindowText("Open COM port fail...\r\n\r\n"+LOG_Show_Old);
			m_Result.UpdateWindow();
			break;

		case META_COM_ERROR:
			m_Result.GetWindowText(LOG_Show_Old);  
			m_Result.SetWindowText("COM port doesn't exist..\r\n\r\n"+LOG_Show_Old);
			m_Result.UpdateWindow();
			break;

		case META_WAIT_FOR_TARGET_READY_TIMEOUT:	
			//MessagePopup ("Meta Error", "");
			m_Result.GetWindowText(LOG_Show_Old);  
			m_Result.SetWindowText("Connecting timeout...\n May be incorrect modem database！\r\n\r\n"+LOG_Show_Old);
			m_Result.UpdateWindow();
			break;

		case META_FAILED:
			m_Result.GetWindowText(LOG_Show_Old);  
			m_Result.SetWindowText("Can't connect to target.\nPlease Enable Meta Dll.Log to get detail cause...\r\n\r\n"+LOG_Show_Old);
			m_Result.UpdateWindow();
			break;

		default: 
			break;
    }	
    return true;

}

int CWriteIMEIDlg::Connect_with_preloader(void)
{
	//for secured meta 
	unsigned int eType;   //0:BootROMUSB,1:PreloaderUSB
    SP_COM_FILTER_LIST_S sCOMFilter;
	SP_COM_PROPERTY_S sCOMProperty;
	char *ppFilter[2];
	int  BootStop;
	unsigned int  BootROMUSBDetection;
    int iRet;
	
	//GetUILegacyUSBautodetection(KAL_TRUE,&BootROMUSBDetection);
    
	//read_SpBromPortFilter(asBROMPortFilter);
	//read_SpPreloaderPortFilter(asPreloaderPortFilter);

	//IsCompositeDeviceEnable=read_Is_Composite_Device_Enable();
	int temp_com_port = 0;
	m_Port.GetWindowText(m_bUsbMode);
	CString m_bUsbMode_temp=m_bUsbMode.Right(m_bUsbMode.GetLength()-3);
	temp_com_port = atoi(m_bUsbMode_temp);
    if (m_bUsbMode=="USB")
    {
        temp_com_port=USB_COM_PORT; //使用USB口
    }

	ppFilter[0] = "VID_0E8D&PID_2000";//asPreloaderPortFilter;
	ppFilter[1] = "VID_0E8D&PID_0003";//asBROMPortFilter;
    BootStop=SP_BOOT_INFINITE;                                        //不能是  SP_BOOT_STOP---Connect_with_preloader
    m_stModeArg.m_pStopFlag=&BootStop;

    m_stModeArg.m_bIsUSBEnable=temp_com_port == USB_COM_PORT?1:0;
	m_stModeArg.m_bIsSymbolicEnable=false;
	m_stModeArg.m_euBootMode=SP_META_BOOT;
	bool m_isCompositeUSB=1;  
	m_stModeArg.m_bIsCompositeDeviceEnable=m_isCompositeUSB;

	BootROMUSBDetection=0;     //可配置
	if(BootROMUSBDetection)
	{
		sCOMFilter.m_uCount=2;
	}
	else
	{
		sCOMFilter.m_uCount=1;
	}
	sCOMFilter.m_ppFilterID=ppFilter;
	sCOMFilter.m_eType=SP_WHITE_LIST;
	if (m_bUsbMode!="USB")
    {
		m_stModeArg.m_uPortNumber=temp_com_port;
	    eType=PreloaderUSB;
		search_preloader_port_success=1;// 
    }
    else
    {
		iRet=SP_GetIncrementCOMPortWithFilter(&sCOMFilter, &sCOMProperty, NULL, _TRUE,&BootStop,30); 
	    if(0 == iRet)
	    {
			switch(sCOMProperty.m_iFilterIndex)
	        {
				case 0:
					eType=PreloaderUSB;
			        search_preloader_port_success=1;
					break;
			    case 1:
			        eType=BootROMUSB;
				    search_preloader_port_success=1;
				    break;
			    default:
			  		//MessagePopup ("Filter index error!", "fail.");
				    search_preloader_port_success=0;
			        return true;
			}
	        m_stModeArg.m_uPortNumber=sCOMProperty.m_uNumber;
		}
	    else
	    {
	         //MessagePopup ("Search new COM port fail!", "fail.");
			 search_preloader_port_success=0;
			 return true;
	    }
	}

    if(BootROMUSB==eType)
    {
		iRet=SP_BootROM_BootMode(&m_stModeArg);
	}
	else if(PreloaderUSB==eType)
	{
		iRet=SP_Preloader_BootMode(&m_stModeArg);
	}
    if(0!=iRet)
    {
		//MessagePopup ("META mode entry fail!", "fail.");
		search_preloader_port_success=0;
		return true;
	}
	return false;
}
int CWriteIMEIDlg::search_kernel_port(void )
{
	SP_COM_FILTER_LIST_S sCOMFilter;
	SP_COM_PROPERTY_S sCOMProperty;
	char *ppFilter_kernel_port[2];
    int  BootStop;
	signed short int Search_Kernel_Port_Time=35;
	
	memset (&sCOMFilter, 0x0, sizeof(SP_COM_FILTER_LIST_S));
    memset (&sCOMProperty, 0x0, sizeof(SP_COM_PROPERTY_S));
    BootStop=SP_BOOT_INFINITE;                                 //不能是  SP_BOOT_STOP----search_kernel_port
		 
    //Use USB port, necessary to scan  
    //Search_Kernel_Port_Time= read_Search_Kernel_Port_Time();
    //read_Kernel_COMPort_Filter(asKernelPortFilter);
	int temp_com_port = 0;
	m_Port.GetWindowText(m_bUsbMode);
	CString m_bUsbMode_temp=m_bUsbMode.Right(m_bUsbMode.GetLength()-3);
	temp_com_port = atoi(m_bUsbMode_temp);
    if (m_bUsbMode=="USB")
    {
        temp_com_port=USB_COM_PORT; //使用USB口
    }

    ppFilter_kernel_port[0]="VID_0E8D&PID_2000";//asPreloaderPortFilter;
	ppFilter_kernel_port[1]="VID_0E8D&PID_0003";//asBROMPortFilter;
	sCOMFilter.m_uCount = 2;
    sCOMFilter.m_ppFilterID = ppFilter_kernel_port;	
	sCOMFilter.m_eType=SP_BLACK_LIST;
    search_kernel_port_success=1;
 
    //if(IsUSBCOMPort())
	if (m_bUsbMode=="USB")
	{
		int iRet= SP_GetIncrementCOMPortWithFilter(&sCOMFilter, &sCOMProperty, &GUID_PORT_CLASS_USB2SER, 0, &BootStop, Search_Kernel_Port_Time);
		switch(iRet)
	    {
			case SP_S_DONE:
				kernel_port=sCOMProperty.m_uNumber;
	            break;
	        case SP_S_STOP:	
                search_kernel_port_success=0;
			    return true;	
	        case SP_S_TIMEOUT:
	            {
	                //MessagePopup("Search Kernel USB COM port timout!", "Fail");
	                search_kernel_port_success=0;
	                return true;
	            }
	        default:
	            {
	                //MessagePopup("Search Kernel USB COM port fail!", "Fail");
					search_kernel_port_success=0;
	                return true;
	            }
		}
	 }
	  return false;
   
}
//切换模式
bool CWriteIMEIDlg::Switch2WM_META_Mode ( const int meta_handle )//相当于if ( EnterMetaAPMode () ) 
{
	//unsigned short us_com_port;
	//Eboot_DebugOn ();    //Eboot_DebugOff ();
	SP_META_DebugOn ();    //SP_META_DebugOff ();

	Seg_MetaAPConnectReq ();

	g_Meta.bootstop = BOOT_INFINITE;   //智能机---这样才能进入META模式
    if (m_bUsbMode=="USB")
    {
		g_MetaAP.tMETA_Connect_Req.com_port = kernel_port;
    }
	g_MetaAP.eMetaAP_Result = SP_META_ConnectInMetaMode_r ( g_Meta.iMetaAP_handle, &g_MetaAP.tMETA_Connect_Req, &g_Meta.bootstop, &g_MetaAP.tMETA_Connect_Report); 
    switch ( g_MetaAP.eMetaAP_Result )
    {
		case META_SUCCESS:
			 return false; 
			 break;

		case META_INCORRECT_TARGET_VER:
			
			m_Result.GetWindowText(LOG_Show_Old);  
			m_Result.SetWindowText("Modem NVRAM database inconsistent with target...\r\n\r\n"+LOG_Show_Old);
			m_Result.UpdateWindow();
			break;

		case META_COMM_FAIL:

			m_Result.GetWindowText(LOG_Show_Old);  
			m_Result.SetWindowText("Open COM port fail..\r\n\r\n"+LOG_Show_Old);
			m_Result.UpdateWindow();
			break;

		case META_COM_ERROR:

			m_Result.GetWindowText(LOG_Show_Old);  
			m_Result.SetWindowText("COM port doesn't exist...\r\n\r\n"+LOG_Show_Old);
			m_Result.UpdateWindow();
			break;

		case META_TIMEOUT:	

			m_Result.GetWindowText(LOG_Show_Old);  
			m_Result.SetWindowText("Connecting timeout...\r\n\r\n"+LOG_Show_Old);
			m_Result.UpdateWindow();
			break;

		case META_FAILED:

			m_Result.GetWindowText(LOG_Show_Old);  
			m_Result.SetWindowText("Can't connect to target.\nPlease Enable Meta Dll.Log to get detail cause..\r\n\r\n"+LOG_Show_Old);
			m_Result.UpdateWindow();
			break;

		default: 
			break;
    }	

     return true;
     
}

void CWriteIMEIDlg::Seg_MetaAPConnectReq ( void ) 
{
//-------------------Baseband chipset select----------------------------  
//  -----------Boot ROM and Sync with Target ----------------------
	int temp_com_port = 0;
	m_Port.GetWindowText(m_bUsbMode);
	CString m_bUsbMode_temp=m_bUsbMode.Right(m_bUsbMode.GetLength()-3);
	temp_com_port = atoi(m_bUsbMode_temp);
    if (m_bUsbMode=="USB")
    {
        temp_com_port=USB_COM_PORT; //使用USB口
    }
    g_MetaAP.tMETA_Connect_Req.com_port = temp_com_port;
	
    g_MetaAP.tMETA_Connect_Req.baudrate[0] = META_BAUD115200;
    g_MetaAP.tMETA_Connect_Req.baudrate[1] = META_BAUD230400;
    g_MetaAP.tMETA_Connect_Req.baudrate[2] = META_BAUD460800;
    g_MetaAP.tMETA_Connect_Req.baudrate[2] = META_BAUD921600;
    g_MetaAP.tMETA_Connect_Req.baudrate[3] = META_BAUD_END;	
	
    g_MetaAP.tMETA_Connect_Req.flowctrl = META_NO_FLOWCTRL;    //for Modem side SW_FLOWCTRL
    g_MetaAP.tMETA_Connect_Req.ms_connect_timeout = META_CONNECT_TIME_OUT;

    memset (&m_stModeArg, 0x0, sizeof(SP_BOOT_ARG_S));
    m_stModeArg.m_bbchip_type=SP_AUTO_DETECT_BBCHIP;

    m_stModeArg.m_ext_clock=SP_AUTO_DETECT_EXT_CLOCK;
    m_stModeArg.m_ms_boot_timeout=SP_BOOT_INFINITE;     //不能是 SP_BOOT_STOP---Seg_MetaAPConnectReq
    m_stModeArg.m_uBaudrate=CBR_115200;
    m_stModeArg.m_uTimeout=20000;
    m_stModeArg.m_uRetryTime=2000;
    m_stModeArg.m_uInterval=10;

	
    m_stModeArg.m_auth_handle=SPATE_m_sAuthHandle;//SPATE_Get_AuthHandle();
    m_stModeArg.m_scert_handle=SPATE_m_sScertHandle;//SPATE_Get_ScertHandle();
    m_stModeArg.m_cb_sla_challenge=SLA_Challenge;
    m_stModeArg.m_cb_sla_challenge_arg=NULL;
    m_stModeArg.m_cb_sla_challenge_end=SLA_Challenge_END;
    m_stModeArg.m_cb_sla_challenge_end_arg=NULL;	
}

//智能机写BT地址总函数
static void __stdcall CNF_WMReadFromNVRAM(const FT_NVRAM_READ_CNF *cnf, const short token, void *usrData)
{
     if (cnf->status != META_SUCCESS)
    {  
       	//ConfirmCallback (META_FAILED);  
		return; 
	}
	
	SetEvent (g_MetaAP.m_ReadFromNVRAMEvent);   
}
META_RESULT CWriteIMEIDlg::REQ_ReadFromNVRAM_AP( void )
{
    DWORD wait_result;
	signed short int m_sNVRAM_OPID;
	 
    g_MetaAP.m_ReadFromNVRAMEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
	
    ResetEvent ( g_MetaAP.m_ReadFromNVRAMEvent );

    g_MetaAP.eMetaAP_Result = SP_META_NVRAM_Read_r ( g_Meta.iMetaAP_handle,
                                	          &g_MetaAP.tNVRAM_ReadReq,
                                              &g_MetaAP.tNVRAM_ReadCnf,
                                              CNF_WMReadFromNVRAM,
                                              &m_sNVRAM_OPID,
                                              NULL );

    wait_result = WaitForSingleObject ( g_MetaAP.m_ReadFromNVRAMEvent, 15000 );

    //CloseHandle ( g_MetaAP.m_ReadFromNVRAMEvent );
    if ( WAIT_TIMEOUT == wait_result )
    {
        return META_TIMEOUT;
    }
	else if(WAIT_OBJECT_0)
    {
        CloseHandle (g_MetaAP.m_ReadFromNVRAMEvent);
    }

    if ( g_MetaAP.eMetaAP_Result != META_SUCCESS )
    {
        return g_MetaAP.eMetaAP_Result;
    }

    return META_SUCCESS;
}
bool CWriteIMEIDlg::func_bt_address_meta_hdlr_smart_phone ( void )
{
	unsigned char i, iBCD_L, iBCD_H;
	
	BT_ModuleID_S BT_ID;
	
	S_HANDSET_BT_ADRRESS tBtAddress_W, tBtAddress_R;
		
	unsigned char strBtAddress_W [ BT_ADDRESS_MAX_LENGTH + 1 ];
	unsigned short int iLoop = 0;
	unsigned int BackupSecondImei;
	
	char* strLog;
	CString strLog_CS;
	strLog=new char[128];
	//BT_ID.id = BTMODULE_MT6611;	g_MetaAP.eMeta_Result = SP_META_NVRAM_BT_QueryChipByStructSize(&Chip_id);
	BT_ID.id=MT6592;
	//strcat ( strLog,  "        Write BT Address to smartphone AP nvram.\n");//sprintf ( strLog, ">>Step: Start to Init AP Nvram Database...\n");

	BackupSecondImei=1;//GetUISetCleanBootFlag(KAL_TRUE, &BackupSecondImei);
	
	/*Seg_MetaAPConnectReq ();			
	if ( EnterMetaAPMode () ) 
	{
		SP_META_DisconnectWithTarget_r ( g_Meta.iMetaAP_handle );//Exit_MetaAPMode ();//sprintf ( strLog, "   Error: Connect failed, Exit Meta Mode and turn to next phone.\n" );
		return false;
	}*/
	//-------------------------------------------------------------------/
	// Not support the meta function yet
	/*if ( META_QueryIfFunctionSupportedByTarget_r ( g_Meta.iMeta_handle, 5000, "META_BT_QueryModuleID_r" ))
	{
		if ( META_BT_QueryModuleID_r (g_Meta.iMeta_handle, 5000, &BT_ID ))
		{
			m_Result.GetWindowText(LOG_Show_Old);  
			m_Result.SetWindowText("Query BT Module 失败！！！\r\n\r\n"+LOG_Show_Old);
			m_Result.UpdateWindow();
			return false;   // " Query BT Module fail!"
		}
		else
		{
			m_Result.GetWindowText(LOG_Show_Old);  
			m_Result.SetWindowText("Query BT Module 成功！！！\r\n\r\n"+LOG_Show_Old);
			m_Result.UpdateWindow();
		}

	}*/
	m_Result.GetWindowText(LOG_Show_Old);  
	m_Result.SetWindowText("开始写蓝牙地址...\r\n\r\n"+LOG_Show_Old);
	m_Result.UpdateWindow();

	strcpy((char *)strBtAddress_W,m_BT_Addr);

	for ( i = 0; i < BT_ADDRESS_MAX_LENGTH; i += 2 )
	{
		 
		 iBCD_H = Ascii2BCD( strBtAddress_W [i] );
		 iBCD_L = Ascii2BCD( strBtAddress_W [i + 1] );

		 tBtAddress_W.bd_addr [i/2] = iBCD_H<<4 | iBCD_L;
		
	}

	switch ( BT_ID.id )	
	{
		case BTMODULE_MT6601:  //6601
		case BTMODULE_MT6611:
		//case BTMODULE_MT6612:
			if ( META_SUCCESS != REQ_WriteBTAddr2NVRAM_Start ( BT_ID.id, tBtAddress_W.bd_addr) )
			{
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("蓝牙模块BTMODULE_MT6601、BTMODULE_MT6611-->写蓝牙地址失败！！！\r\n\r\n"+LOG_Show_Old);
				m_Result.UpdateWindow();
				return false;
			}
			else
			{
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("蓝牙模块BTMODULE_MT6601、BTMODULE_MT6611-->写蓝牙地址成功！\r\n\r\n"+LOG_Show_Old);
				m_Result.UpdateWindow();
			}
			break;
		case BTMODULE_RFMD3500:  //RFMD
		case BTMODULE_RFMD4020:
			if ( META_SUCCESS != REQ_WriteBTAddr2NVRAM_Start ( BT_ID.id, tBtAddress_W.bd_addr ) )
			{
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("蓝牙模块BTMODULE_RFMD3500、BTMODULE_RFMD4020-->写蓝牙地址失败！！！\r\n\r\n"+LOG_Show_Old);
				m_Result.UpdateWindow();
				return false;//SP_META_DisconnectWithTarget_r ( g_Meta.iMetaAP_handle );//Exit_MetaAPMode ();
			}
			else
			{
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("蓝牙模块BTMODULE_RFMD3500、BTMODULE_RFMD4020-->写蓝牙地址成功！\r\n\r\n"+LOG_Show_Old);
				m_Result.UpdateWindow();
			}
			
			break;
		case MT6592: 
			goto MT6592BT;
			break;
		default:
			m_Result.GetWindowText(LOG_Show_Old);  
			m_Result.SetWindowText("话机不支持蓝牙！！！\r\n\r\n"+LOG_Show_Old);
			m_Result.UpdateWindow();
			return false;
			break;
	}

	if (META_SUCCESS == REQ_ReadBTAddrFromNVRAM_Start ( BT_ID.id, tBtAddress_R.bd_addr) )
	{
		m_Result.GetWindowText(LOG_Show_Old);  
		m_Result.SetWindowText("蓝牙模块-->读蓝牙地址成功！\r\n\r\n"+LOG_Show_Old);
		m_Result.UpdateWindow();
	}
	else
	{
		m_Result.GetWindowText(LOG_Show_Old);  
		m_Result.SetWindowText("蓝牙模块--->读蓝牙地址失败！！！\r\n\r\n"+LOG_Show_Old);
		m_Result.UpdateWindow();
		return false;
	}
	


	
	strLog_CS="";
	//sprintf ( strLog, "   OK: Write BT Address successfully.\n"  );//sprintf ( strLog, "   Step: Start to check BT Address.\n"  );
	sprintf ( strLog, "   从智能机读出的. BT Address = \"%x %x %x %x %x %x\".\n",  tBtAddress_R.bd_addr[0],
																									 tBtAddress_R.bd_addr[1],
																									 tBtAddress_R.bd_addr[2],
																									 tBtAddress_R.bd_addr[3],
																									 tBtAddress_R.bd_addr[4],
																									 tBtAddress_R.bd_addr[5]);
	strLog_CS.Format("%s",strLog);

	if ( memcmp ( tBtAddress_W.bd_addr, tBtAddress_R.bd_addr, BD_ADDR_LEN )) 
	{			
		m_Result.GetWindowText(LOG_Show_Old);  
		m_Result.SetWindowText(strLog_CS+"\r\n写入的与读出的蓝牙地址不一致，请重新写入！！！\r\n\r\n"+LOG_Show_Old);
		m_Result.UpdateWindow();
		return false;
	}
	else
	{
		m_Result.GetWindowText(LOG_Show_Old);  
		m_Result.SetWindowText(strLog_CS+"\r\n写入的与读出的蓝牙地址一致，写BT地址确认OK！！！\r\n\r\n"+LOG_Show_Old);
		m_Result.UpdateWindow();
	}

MT6592BT:

	if (REQ_WriteAP_NVRAM_Start(WRITE_BT, (char *)strBtAddress_W, 1) == META_SUCCESS)
	{

	}
	else
	{
		return false;
	}

	if(BackupSecondImei==1)
	{
		//sprintf ( strLog, "\n>>Step: To Backup NVRAM to Bin Region, and Set Clean Boot Flag\n" );
	  	//if ( WM_META_SetCleanBootFlag_Customize ( g_Meta.iMeta_handle ) )   ///// Clean boot is very Importnant for Customer.
		if(META_SUCCESS!=SP_META_SetCleanBootFlag_r ( g_Meta.iMetaAP_handle, 5000, &req, &cnf ))
		{
			m_Result.GetWindowText(LOG_Show_Old);  
			m_Result.SetWindowText("BT_MAC---Fail to Backup NVRAM and WM_META_SetCleanBootFlag_Customize！！！\r\n\r\n"+LOG_Show_Old);
			m_Result.UpdateWindow();
			return false;//IMEI_Write_Result=false;
		}
		else
		{
			m_Result.GetWindowText(LOG_Show_Old);  
			m_Result.SetWindowText("BT_MAC---Success to Backup NVRAM and WM_META_SetCleanBootFlag_Customize...继续...\r\n\r\n"+LOG_Show_Old);
			m_Result.UpdateWindow();
		}
	}
	return true;
	//sprintf ( strLog, ">>Step: Meta Disconnect with target.\n");	
	//SP_META_DisconnectWithTarget_r ( g_Meta.iMetaAP_handle );//Exit_MetaAPMode ();   
}
//智能机Nvram初始化
bool CWriteIMEIDlg::APNvramDatabaseInit () 
{
	unsigned long db;
	const char * APDatabasePath;   //配置相应的路径
	APDatabasePath=(LPCTSTR)m_strExchangeFileName_Ap;
	
	if ( META_SUCCESS != SP_META_NVRAM_Init_r ( g_Meta.iMetaAP_handle, APDatabasePath, &db) )
	{
		//MessagePopup ("WARNING", "Initial AP Database fail!, Please reselect a database.");
		return true;
	}
	g_Meta.bIsDatabaseInitialized [1] = true;  
	return false;
}
bool CWriteIMEIDlg::IsAPNvramDatabaseInit ()
{
	//extern int AP_Database_Select_Click_Shell ( void );
	if ( false == g_Meta.bIsDatabaseInitialized [1] ) 
	{
		if ( APNvramDatabaseInit () )
		{
			//AP_Database_Select_Click_Shell ();
			//return APNvramDatabaseInit ();
			return true;
		}	
	} 
	return false;
}
//
//return false -> OK
//return true  -> ERROR
//智能机----METAAP模式连接手机(写蓝牙地址)
bool CWriteIMEIDlg::EnterMetaAPMode ( void )//Switch2WM_META_Mode
{
    //Meta_Boot_Arg meta_arg;
	//EBOOT_RESULT Eboot_Result;

	SP_META_DebugOn ();
	SP_Brom_DebugOn();           //Brom_DebugOn();
	Seg_MetaAPConnectReq ();     //每次进入模式的时候，需要设置一次

	// 写IMEI的时候已经处理
	/* Search  preloader or brom COM port for META connection  and handshake with preloader
    //CmtScheduleThreadPoolFunction(DEFAULT_THREAD_POOL_HANDLE,Connect_with_preloader,NULL,&functionId); //创建线程   
	Connect_with_preloader();
	if(search_preloader_port_success==0)  
	{
		return true;//sprintf ( strLog,  " COM port searching and preloader handshake  fail\n");	
	}     

    //CmtScheduleThreadPoolFunction(DEFAULT_THREAD_POOL_HANDLE,search_kernel_port,NULL,&functionId);   //创建线程
	search_kernel_port();
    if(search_kernel_port_success==0)
	     return true;
	*/

	g_Meta.bootstop = BOOT_INFINITE;   //智能机---这样才能进入META模式
	if (m_bUsbMode=="USB")
    {
		//Sleep(1000);   //delay for USB com port ready
        g_MetaAP.tMETA_Connect_Req.com_port = kernel_port;
		g_MetaAP.eMeta_Result = SP_META_ConnectInMetaMode_r (g_Meta.iMetaAP_handle, &g_MetaAP.tMETA_Connect_Req, &g_Meta.bootstop, &g_MetaAP.tMETA_Connect_Report); 
    }
    else
    { 
	    //g_Meta6516Modem.eMeta_Result = META_ConnectInMetaMode_r (g_Meta.iMeta_handle, &g_Meta6516Modem.tMETA_Connect_Req, &g_Meta.bootstop, &g_Meta6516Modem.tMETA_Connect_Report); 
		g_MetaAP.eMeta_Result = SP_META_ConnectInMetaMode_r (g_Meta.iMetaAP_handle, &g_MetaAP.tMETA_Connect_Req, &g_Meta.bootstop, &g_MetaAP.tMETA_Connect_Report); 
    } 
	switch ( g_MetaAP.eMeta_Result )
	{
		case META_SUCCESS:
			 return false; 
			 break;

		case META_INCORRECT_TARGET_VER:
			
			m_Result.GetWindowText(LOG_Show_Old);  
			m_Result.SetWindowText("Warning: SP NVRAM database inconsistent with target...\r\n\r\n"+LOG_Show_Old);
			m_Result.UpdateWindow();
			//return false; 
			break;

		case META_COMM_FAIL:

			m_Result.GetWindowText(LOG_Show_Old);  
			m_Result.SetWindowText("Open COM port fail...\r\n\r\n"+LOG_Show_Old);
			m_Result.UpdateWindow();
			break;

		case META_COM_ERROR:

			m_Result.GetWindowText(LOG_Show_Old);  
			m_Result.SetWindowText("COM port doesn't exist...\r\n\r\n"+LOG_Show_Old);
			m_Result.UpdateWindow();
			break;

		case META_TIMEOUT:	

			m_Result.GetWindowText(LOG_Show_Old);  
			m_Result.SetWindowText("Connecting timeout...\r\n\r\n"+LOG_Show_Old);
			m_Result.UpdateWindow();
			break;

		case META_FAILED:
			m_Result.GetWindowText(LOG_Show_Old);  
			m_Result.SetWindowText("Can't connect to target---failed....\r\n\r\n"+LOG_Show_Old);
			m_Result.UpdateWindow();
			break;

		default: 
			break;
	}	

	return true;

};

//智能机----写蓝牙地址函数子函数
META_RESULT CWriteIMEIDlg::REQ_WriteBTAddr2NVRAM_Start (unsigned int iBTId, unsigned char* pBTAddress)
{
    unsigned int m_usRID = 1;
    int m_iBTBufSize;
    const char*pLID;
	unsigned int pBTv;
    NVRAM_BTRADIO_MT6611_S  tBT;
    NVRAM_BTRADIO_MT6620_S  tBT1;
    NVRAM_BTRADIO_MT6626_S  tBT2;
    BT_CHIP_ID_E  Chip_id;
	
	switch ( iBTId )	
	{
		case BTMODULE_MT6601:  //6601
		case BTMODULE_MT6611:
		//case BTMODULE_MT6612:
			if (!SP_META_NVRAM_GetRecLen ( "AP_CFG_RDCL_FILE_BT_ADDR_LID", &m_iBTBufSize ))     //===META_SUCCESS    to Test LID exist
			{
				pLID = "AP_CFG_RDCL_FILE_BT_ADDR_LID";  //after DUMA 1004
			} 
			else if (!SP_META_NVRAM_GetRecLen ( "AP_CFG_RDEB_FILE_BT_ADDR_LID", &m_iBTBufSize ))//===META_SUCCESS
			{
				pLID = "AP_CFG_RDEB_FILE_BT_ADDR_LID";  //before DUMA 1004
			} 
			else
			{
				return META_LID_INVALID;
			}
			break;
			
		case BTMODULE_RFMD3500:  //RFMD
			pLID = "AP_CFG_RDEB_FILE_BT_ADDR_LID"; 
			break;
		case BTMODULE_RFMD4020:
			
			pLID = "AP_CFG_RDEB_FILE_BT_ADDR_LID"; 
			break;
		default:
			break;
	}

	g_MetaAP.eMeta_Result = SP_META_NVRAM_GetRecLen ( pLID, &m_iBTBufSize );
    if ( g_MetaAP.eMeta_Result != META_SUCCESS )
    {
        //TODO: LOG
        return g_MetaAP.eMeta_Result;
    }

    

	g_MetaAP.tNVRAM_ReadReq.LID = pLID;
    g_MetaAP.tNVRAM_ReadReq.RID = m_usRID;
    g_MetaAP.tNVRAM_ReadCnf.len = m_iBTBufSize;
    g_MetaAP.tNVRAM_ReadCnf.buf = (unsigned char*) malloc (m_iBTBufSize*sizeof(unsigned char));

	m_Result.GetWindowText(LOG_Show_Old);  
	m_Result.SetWindowText("开始读取智能机的NVRAM...\r\n\r\n"+LOG_Show_Old);
	m_Result.UpdateWindow();


	g_MetaAP.eMeta_Result =  REQ_ReadFromNVRAM_AP ();  //读NVRAM----智能机BT
	if ( g_MetaAP.eMeta_Result!=META_SUCCESS )
    {
		if ( NULL != g_MetaAP.tNVRAM_ReadCnf.buf )
		{
			free ( g_MetaAP.tNVRAM_ReadCnf.buf );	//读NVRAM失败，释放分配的存储空间--read---智能机
			g_MetaAP.tNVRAM_ReadCnf.buf = NULL;
		}
		m_Result.GetWindowText(LOG_Show_Old);  
		m_Result.SetWindowText("读取智能机的NVRAM失败！！！\r\n\r\n"+LOG_Show_Old);
		m_Result.UpdateWindow();
    	return g_MetaAP.eMeta_Result;
    }
	else
	{
		m_Result.GetWindowText(LOG_Show_Old);  
		m_Result.SetWindowText("读取智能机的NVRAM成功，继续...\r\n\r\n"+LOG_Show_Old);
		m_Result.UpdateWindow();
	}
    g_MetaAP.eMeta_Result=SP_META_BT_GetChipID_r(g_Meta.iMetaAP_handle, 5000, &pBTv);
	if(pBTv>6)
	{
		/////更新蓝牙芯片//////////////////////////////////////////////////////////////////////////////////
	}
    if(pBTv==BT_CHIP_ID_MT6622||pBTv==BT_CHIP_ID_MT6626)
    {
		g_MetaAP.eMeta_Result= SP_META_NVRAM_BT_Decompose_MT6626Radio(&tBT2, (const char*)(char *)g_MetaAP.tNVRAM_ReadCnf.buf,m_iBTBufSize); 		
    }
	else
    {
		g_MetaAP.eMeta_Result = SP_META_NVRAM_BT_QueryChipByStructSize(&Chip_id);
		if ( g_MetaAP.eMeta_Result != META_SUCCESS )
		{
			if ( NULL != g_MetaAP.tNVRAM_ReadCnf.buf )
			{
				free ( g_MetaAP.tNVRAM_ReadCnf.buf );	//识别蓝牙芯片失败，释放分配的存储空间--read---智能机
				g_MetaAP.tNVRAM_ReadCnf.buf = NULL;
			}
			m_Result.GetWindowText(LOG_Show_Old);  
			m_Result.SetWindowText("智能机的识别蓝牙芯片失败！！！\r\n\r\n"+LOG_Show_Old);
			m_Result.UpdateWindow();
			return g_MetaAP.eMeta_Result;
		}
		if(Chip_id==BT_CHIP_ID_MT6611) 
        {
			g_MetaAP.eMeta_Result = SP_META_NVRAM_BT_Decompose_MT6611Radio ( &tBT, (const char *)(char*)g_MetaAP.tNVRAM_ReadCnf.buf, m_iBTBufSize);
		}
        else 
        {
			g_MetaAP.eMeta_Result = SP_META_NVRAM_BT_Decompose_MT6620Radio( &tBT1, (const char *)(char*)g_MetaAP.tNVRAM_ReadCnf.buf, m_iBTBufSize);
        }
    }  
	if ( g_MetaAP.eMeta_Result != META_SUCCESS )
    {
		if ( NULL != g_MetaAP.tNVRAM_ReadCnf.buf )
		{
			free ( g_MetaAP.tNVRAM_ReadCnf.buf );	//释放分配的存储空间--read---智能机---Decompose失败
			g_MetaAP.tNVRAM_ReadCnf.buf = NULL;
		}
		m_Result.GetWindowText(LOG_Show_Old);  
		m_Result.SetWindowText("智能机的NVRAM_BT_Deompose失败！！！\r\n\r\n"+LOG_Show_Old);
		m_Result.UpdateWindow();
    	return g_MetaAP.eMeta_Result;
    }
	else
	{
		m_Result.GetWindowText(LOG_Show_Old);  
		m_Result.SetWindowText("智能机的NVRAM_BT_Deompose成功，继续...\r\n\r\n"+LOG_Show_Old);
		m_Result.UpdateWindow();
	}


    if(pBTv==BT_CHIP_ID_MT6622||pBTv==BT_CHIP_ID_MT6626)
    {  
        memcpy(tBT2.addr,pBTAddress,BD_ADDR_LEN);
    }
	else
	{
		if(Chip_id==BT_CHIP_ID_MT6611) 
        {
			memcpy ( tBT.addr, pBTAddress, BD_ADDR_LEN ); 
		}
		else
		{
			memcpy ( tBT1.addr, pBTAddress, BD_ADDR_LEN ); 
		}
	}

	
	switch ( iBTId )	
	{
		case BTMODULE_MT6601:  //6601
		case BTMODULE_MT6611:
		//case BTMODULE_MT6612:
			if (!SP_META_NVRAM_GetRecLen ( "AP_CFG_RDCL_FILE_BT_ADDR_LID", &m_iBTBufSize )) // to Test LID exist
			{
				pLID = "AP_CFG_RDCL_FILE_BT_ADDR_LID";  //after DUMA 1004
			} 
			else if (!SP_META_NVRAM_GetRecLen ( "AP_CFG_RDEB_FILE_BT_ADDR_LID", &m_iBTBufSize ))
			{
				pLID = "AP_CFG_RDEB_FILE_BT_ADDR_LID";  //before DUMA 1004
			} 
			else
			{
				return META_LID_INVALID;
			}
			break;
			
		case BTMODULE_RFMD3500:  //RFMD
			pLID = "AP_CFG_RDEB_FILE_BT_ADDR_LID"; 
			break;
		case BTMODULE_RFMD4020:
			
			pLID = "AP_CFG_RDEB_FILE_BT_ADDR_LID"; 
			break;
		default:
			break;
	}

	g_MetaAP.eMeta_Result = SP_META_NVRAM_GetRecLen ( pLID, &m_iBTBufSize );
    if ( g_MetaAP.eMeta_Result != META_SUCCESS )
    {
        return g_MetaAP.eMeta_Result;
    }
	m_usRID = 1;
	g_MetaAP.tNVRAM_WriteReq.LID = pLID;
    g_MetaAP.tNVRAM_WriteReq.RID = m_usRID;
    g_MetaAP.tNVRAM_WriteReq.len = m_iBTBufSize;
    g_MetaAP.tNVRAM_WriteReq.buf = (unsigned char*) malloc ( m_iBTBufSize*sizeof(unsigned char) );

	g_MetaAP.eMeta_Result=SP_META_BT_GetChipID_r(g_Meta.iMetaAP_handle, 5000, &pBTv);

	m_Result.GetWindowText(LOG_Show_Old);  
	m_Result.SetWindowText("智能机的NVRAM_BT_Compose...\r\n\r\n"+LOG_Show_Old);
	m_Result.UpdateWindow();
	if(pBTv==BT_CHIP_ID_MT6622||pBTv==BT_CHIP_ID_MT6626)
    {
         g_MetaAP.eMeta_Result= SP_META_NVRAM_BT_Compose_MT6626Radio(&tBT2, (char*)g_MetaAP.tNVRAM_WriteReq.buf,m_iBTBufSize); 
		
    }
    else
    {
		g_MetaAP.eMeta_Result = SP_META_NVRAM_BT_QueryChipByStructSize(&Chip_id);
		if ( g_MetaAP.eMeta_Result != META_SUCCESS )
		{
			if (NULL != g_MetaModem.tNVRAM_WriteReq.buf)
			{
				free (g_MetaModem.tNVRAM_WriteReq.buf);	//释放分配的存储空间--Write----智能机---_BT_QueryChipByStructSize出错
			}
			return g_MetaAP.eMeta_Result;
		}
		if(Chip_id==BT_CHIP_ID_MT6611) 
        {
			g_MetaAP.eMeta_Result = SP_META_NVRAM_BT_Compose_MT6611Radio( &tBT,  (char*)g_MetaAP.tNVRAM_WriteReq.buf,  m_iBTBufSize );
		}
        else 
        {
			g_MetaAP.eMeta_Result = SP_META_NVRAM_BT_Compose_MT6620Radio( &tBT1,  (char*)g_MetaAP.tNVRAM_WriteReq.buf,  m_iBTBufSize );
		}

	}
	if ( g_MetaAP.eMeta_Result != META_SUCCESS )
    {
		if (NULL != g_MetaModem.tNVRAM_WriteReq.buf)
		{
			free (g_MetaModem.tNVRAM_WriteReq.buf);	//释放分配的存储空间--Write----智能机---Compose出错
		}
		m_Result.GetWindowText(LOG_Show_Old);  
		m_Result.SetWindowText("智能机的NVRAM_BT_Compose失败！！！\r\n\r\n"+LOG_Show_Old);
		m_Result.UpdateWindow();
    	return g_MetaAP.eMeta_Result;
    }
	else
	{
		m_Result.GetWindowText(LOG_Show_Old);  
		m_Result.SetWindowText("智能机的NVRAM_BT_Compose成功，继续...\r\n\r\n"+LOG_Show_Old);
		m_Result.UpdateWindow();
	}
	   
	m_Result.GetWindowText(LOG_Show_Old);  
	m_Result.SetWindowText("开始写入智能机的NVRAM...\r\n\r\n"+LOG_Show_Old);
	m_Result.UpdateWindow();
	g_MetaAP.eMeta_Result = REQ_Write2NVRAM ();
	if ( g_MetaAP.eMeta_Result != META_SUCCESS )
    {
		if (NULL != g_MetaModem.tNVRAM_WriteReq.buf)
		{
			free (g_MetaModem.tNVRAM_WriteReq.buf);	//释放分配的存储空间--Write---智能机--写NVRAM失败
		}
		m_Result.GetWindowText(LOG_Show_Old);  
		m_Result.SetWindowText("写入智能机的NVRAM失败！！！\r\n\r\n"+LOG_Show_Old);
		m_Result.UpdateWindow();
		return g_MetaAP.eMeta_Result;
    }
	else
	{
		m_Result.GetWindowText(LOG_Show_Old);  
		m_Result.SetWindowText("写入智能机的NVRAM成功，继续...\r\n\r\n"+LOG_Show_Old);
		m_Result.UpdateWindow();
	}

    if ( NULL != g_MetaAP.tNVRAM_ReadCnf.buf )
	{
		free ( g_MetaAP.tNVRAM_ReadCnf.buf );   //最后，释放分配的存储空间--read---智能机
		g_MetaAP.tNVRAM_ReadCnf.buf = NULL;
	}

	if (NULL != g_MetaAP.tNVRAM_WriteReq.buf)
	{
		free (g_MetaAP.tNVRAM_WriteReq.buf);	//最后，释放分配的存储空间--Write---智能机
		g_MetaAP.tNVRAM_WriteReq.buf = NULL;
	}
	
    return META_SUCCESS;
}


static void __stdcall CNF_WMWriteNVRAM ( const FT_NVRAM_WRITE_CNF *cnf, const short token, void *usrData)
{
    /*if (cnf->status != META_SUCCESS)//由于变量FT_NVRAM_WRITE_CNF没有与功能机作区分，所以返回值不为0，为1,屏蔽，但写入后再通过读出来做对比，验证是否写入OK。
    {  
    	//ConfirmCallback (META_FAILED);  
		return; 
	}*/
	Sleep(1000);//加入1秒的延时
	SetEvent (g_MetaAP.m_WriteToNVRAMEvent);   
}
void __stdcall CNF_SPWriteToNVRAM ( const FT_NVRAM_WRITE_CNF *cnf, const short token, void *usrData)
{
    if (cnf->status == META_SUCCESS)
    {  
        //SetEvent(*((HANDLE*)usrData));
		SetEvent (g_MetaAP.m_WriteToNVRAMEvent);   
    }  
	//Sleep(1000);//加入1秒的延时
	//SetEvent (g_MetaAP.m_WriteToNVRAMEvent);   
}
META_RESULT CWriteIMEIDlg::REQ_Write2NVRAM(void)
{
    signed short int m_sNVRAM_OPID;
	DWORD wait_result;
	
	
   
    g_MetaAP.m_WriteToNVRAMEvent = CreateEvent( NULL, TRUE, FALSE, NULL );

    ResetEvent ( g_MetaAP.m_WriteToNVRAMEvent );

    g_MetaAP.eMeta_Result = SP_META_NVRAM_Write_r ( g_Meta.iMetaAP_handle,
                                                   &g_MetaAP.tNVRAM_WriteReq,
                                                   CNF_SPWriteToNVRAM,//CNF_WMWriteNVRAM,
                                                   &m_sNVRAM_OPID,
                                                    (void*)g_MetaAP.m_WriteToNVRAMEvent);//NULL);


    wait_result = WaitForSingleObject ( g_MetaAP.m_WriteToNVRAMEvent, 15000 );
	
    //CloseHandle ( g_MetaAP.m_WriteToNVRAMEvent );
	
    if (WAIT_TIMEOUT == wait_result)
    {
        //ConfirmCallback (META_TIMEOUT);
        return META_TIMEOUT;
    }
	else if(WAIT_OBJECT_0)
    {
        CloseHandle (g_MetaAP.m_WriteToNVRAMEvent);
    }

    if (g_MetaAP.eMeta_Result != META_SUCCESS)
    {
        return g_MetaAP.eMeta_Result;
    }
    return META_SUCCESS;
}

META_RESULT CWriteIMEIDlg::REQ_ReadBTAddrFromNVRAM_Start ( unsigned int iBTId, unsigned char* pBTAddress )
{
    signed int m_usRID = 1, m_iBTBufSize;
	const char *pLID;
	//nvram_ef_btradio_mt6611_struct  tBT;
    unsigned int pBTv;
	NVRAM_BTRADIO_MT6611_S  tBT;
    NVRAM_BTRADIO_MT6620_S  tBT1;
	NVRAM_BTRADIO_MT6626_S  tBT2;
    BT_CHIP_ID_E  Chip_id;
	
	switch ( iBTId )	
	{
		case BTMODULE_MT6601:  //6601
		case BTMODULE_MT6611:
			if (!SP_META_NVRAM_GetRecLen ( "AP_CFG_RDCL_FILE_BT_ADDR_LID", &m_iBTBufSize )) // to Test LID exist
			{
				pLID = "AP_CFG_RDCL_FILE_BT_ADDR_LID";  //after DUMA 1004
			} 
			else if (!SP_META_NVRAM_GetRecLen ( "AP_CFG_RDEB_FILE_BT_ADDR_LID", &m_iBTBufSize ))
			{
				pLID = "AP_CFG_RDEB_FILE_BT_ADDR_LID";  //before DUMA 1004
			} 
			else
			{
				return META_LID_INVALID;
			}
			break;
			
		case BTMODULE_RFMD3500:  //RFMD
			pLID = "AP_CFG_RDEB_FILE_BT_ADDR_LID"; 
			break;
		case BTMODULE_RFMD4020:
			
			pLID = "AP_CFG_RDEB_FILE_BT_ADDR_LID"; 
			break;
		default:
			break;
	}

    g_MetaAP.eMeta_Result = SP_META_NVRAM_GetRecLen( pLID, &m_iBTBufSize);
    if ( g_MetaAP.eMeta_Result!=META_SUCCESS )
    {
    	return g_MetaAP.eMeta_Result;
    }


	g_MetaAP.tNVRAM_ReadReq.LID = pLID;
    g_MetaAP.tNVRAM_ReadReq.RID = m_usRID;
    g_MetaAP.tNVRAM_ReadCnf.len = m_iBTBufSize;
    g_MetaAP.tNVRAM_ReadCnf.buf = (unsigned char*) malloc ( m_iBTBufSize*sizeof(unsigned char) );

	g_MetaAP.eMeta_Result =  REQ_ReadFromNVRAM_AP ();//读NVRAM----智能机BT
	if ( g_MetaAP.eMeta_Result!=META_SUCCESS )
    {
		if ( NULL != g_MetaAP.tNVRAM_ReadCnf.buf )
		{
			free ( g_MetaAP.tNVRAM_ReadCnf.buf );//读NVRAM失败，释放分配的存储空间--REQ_Read---智能机
			g_MetaAP.tNVRAM_ReadCnf.buf = NULL;	
		}
		m_Result.GetWindowText(LOG_Show_Old);  
		m_Result.SetWindowText("读取智能机的NVRAM失败！！！\r\n\r\n"+LOG_Show_Old);
		m_Result.UpdateWindow();
    	return g_MetaAP.eMeta_Result;
    }
	else
	{
		m_Result.GetWindowText(LOG_Show_Old);  
		m_Result.SetWindowText("读取智能机的NVRAM成功！继续...\r\n\r\n"+LOG_Show_Old);
		m_Result.UpdateWindow();
	}

    g_MetaAP.eMeta_Result=SP_META_BT_GetChipID_r(g_Meta.iMetaAP_handle, 5000, &pBTv);
	m_Result.GetWindowText(LOG_Show_Old);  
	m_Result.SetWindowText("智能机的NVRAM_BT_Decompose...\r\n\r\n"+LOG_Show_Old);
	m_Result.UpdateWindow();
    if(pBTv==BT_CHIP_ID_MT6622||pBTv==BT_CHIP_ID_MT6626)
    {
         g_MetaAP.eMeta_Result= SP_META_NVRAM_BT_Decompose_MT6626Radio(&tBT2,(const char *)(char*)g_MetaAP.tNVRAM_ReadCnf.buf,m_iBTBufSize); 
    }
	else
    {
		g_MetaAP.eMeta_Result = SP_META_NVRAM_BT_QueryChipByStructSize(&Chip_id);
		if ( g_MetaAP.eMeta_Result != META_SUCCESS )
		{
			if ( NULL != g_MetaAP.tNVRAM_ReadCnf.buf )
			{
				free ( g_MetaAP.tNVRAM_ReadCnf.buf );//释放分配的存储空间--REQ_Read---智能机
				g_MetaAP.tNVRAM_ReadCnf.buf = NULL;	
			}
			m_Result.GetWindowText(LOG_Show_Old);  
			m_Result.SetWindowText("智能机的识别蓝牙芯片失败！！！\r\n\r\n"+LOG_Show_Old);
			m_Result.UpdateWindow();
			return g_MetaAP.eMeta_Result;
		}
		else
		{
			m_Result.GetWindowText(LOG_Show_Old);  
			m_Result.SetWindowText("智能机的识别蓝牙芯片成功，继续...\r\n\r\n"+LOG_Show_Old);
			m_Result.UpdateWindow();
		}
		if(Chip_id==BT_CHIP_ID_MT6611) 
        {
			g_MetaAP.eMeta_Result = SP_META_NVRAM_BT_Decompose_MT6611Radio ( &tBT, (const char *)(char*)g_MetaAP.tNVRAM_ReadCnf.buf, m_iBTBufSize);
		}
        else 
        {
			g_MetaAP.eMeta_Result = SP_META_NVRAM_BT_Decompose_MT6620Radio( &tBT1, (const char *)(char*)g_MetaAP.tNVRAM_ReadCnf.buf, m_iBTBufSize);
        }
	   
   }

	if ( g_MetaAP.eMeta_Result!=META_SUCCESS )
    {
		if ( NULL != g_MetaAP.tNVRAM_ReadCnf.buf )
		{
			free ( g_MetaAP.tNVRAM_ReadCnf.buf );//释放分配的存储空间--REQ_Read---智能机
			g_MetaAP.tNVRAM_ReadCnf.buf = NULL;	
		}
		m_Result.GetWindowText(LOG_Show_Old);  
		m_Result.SetWindowText("智能机的NVRAM_BT_Decompose失败！！！\r\n\r\n"+LOG_Show_Old);
		m_Result.UpdateWindow();
    	return g_MetaAP.eMeta_Result;
    }
	else
	{
		m_Result.GetWindowText(LOG_Show_Old);  
		m_Result.SetWindowText("智能机的NVRAM_BT_Decompose成功，继续...\r\n\r\n"+LOG_Show_Old);
		m_Result.UpdateWindow();
	}
    if(pBTv==BT_CHIP_ID_MT6622||pBTv==BT_CHIP_ID_MT6626)
    {
       memcpy ( pBTAddress, tBT2.addr, BD_ADDR_LEN ); 
    }
    else
    {
		if(Chip_id==BT_CHIP_ID_MT6611) 
		{
			memcpy ( pBTAddress, tBT.addr, BD_ADDR_LEN ); 
		}
		else
		{
			memcpy ( pBTAddress, tBT1.addr, BD_ADDR_LEN ); 	
		}
    }
	if ( NULL != g_MetaAP.tNVRAM_ReadCnf.buf )
	{
		free ( g_MetaAP.tNVRAM_ReadCnf.buf );//最后，释放分配的存储空间--REQ_Read---智能机
		g_MetaAP.tNVRAM_ReadCnf.buf = NULL;
		
	}
	return g_MetaAP.eMeta_Result;
}




//智能机写WIFI_MAC地址总函数
bool CWriteIMEIDlg::func_wifi_mac_meta_hdlr_smart_phone_Ex ( void )
{
	int m_usRID = 1;
	//META_RESULT metaResult = 0;
	unsigned short value = 0;
    int sp_nv_to_nvram = 0;
    

    ////
	unsigned char i, iBCD_L, iBCD_H;
	
	WiFi_MacAddress_S tWifiMac_W, tWifiMac_R;
	unsigned char strWifiMac_W [ WIFI_MAC_MAX_LENGTH + 1 ];
	unsigned short int iLoop = 0;
	unsigned int iMacHeaderLen = 0,BackupSecondImei=1;

	
	/*Seg_MetaAPConnectReq ();

	//sprintf ( strLog, ">>Step: Start to Connect with target.\n" );
    if ( EnterMetaAPMode () ) 
	{
		SP_META_DisconnectWithTarget_r ( g_Meta.iMetaAP_handle );//Exit_MetaAPMode ();
		return false;
	}*/
	m_Result.GetWindowText(LOG_Show_Old);  
	m_Result.SetWindowText("开始写WIFI_MAC地址...\r\n\r\n"+LOG_Show_Old);
	m_Result.UpdateWindow();


	strcpy((char *)strWifiMac_W,m_strWifi);


	for ( i = 0; i < WIFI_MAC_MAX_LENGTH; i+=2 )
	{
		iBCD_H = Ascii2BCD( strWifiMac_W [i] );
		iBCD_L = Ascii2BCD( strWifiMac_W [i+1] );
        if ( (i/2)%2 == 0 )
        {
			tWifiMac_W.mac_addr [i/2+1] = iBCD_H<<4 | iBCD_L;
        }
        else
        {
            tWifiMac_W.mac_addr [i/2-1] = iBCD_H<<4 | iBCD_L;
        }
	}
    if ( META_SUCCESS != REQ_WriteWifiMAC2NVRAM_Start (&tWifiMac_W) )
	{
		m_Result.GetWindowText(LOG_Show_Old);  
		m_Result.SetWindowText("WIFI模块--->写WIFI地址失败！！！\r\n\r\n"+LOG_Show_Old);
		m_Result.UpdateWindow();
		SP_META_DisconnectWithTarget_r ( g_Meta.iMetaAP_handle );//Exit_MetaAPMode ();
		return false;
	}   
	else
	{
		m_Result.GetWindowText(LOG_Show_Old);  
		m_Result.SetWindowText("WIFI模块--->写WIFI地址成功！！！\r\n\r\n"+LOG_Show_Old);                                                                         
		m_Result.UpdateWindow();
	}

	m_Result.GetWindowText(LOG_Show_Old);  
	m_Result.SetWindowText("开始读取WIFI_MAC地址...\r\n\r\n"+LOG_Show_Old);
	m_Result.UpdateWindow();
    if ( META_SUCCESS != REQ_ReadWifiMACFromNVRAM_Start (&tWifiMac_R) )
	{
		m_Result.GetWindowText(LOG_Show_Old);  
		m_Result.SetWindowText("WIFI模块--->读WIFI地址失败！！！\r\n\r\n"+LOG_Show_Old);
		m_Result.UpdateWindow();
		SP_META_DisconnectWithTarget_r ( g_Meta.iMetaAP_handle );//Exit_MetaAPMode ();   
		return false;
	}
	else
	{
		m_Result.GetWindowText(LOG_Show_Old);  
		m_Result.SetWindowText("WIFI模块--->读WIFI地址成功！！！\r\n\r\n"+LOG_Show_Old);
		m_Result.UpdateWindow();
	}



	char* strLog;
	CString strLog_CS;
	strLog=new char[128];
	strLog_CS="";
	sprintf ( strLog, "   从智能机读出的. Wifi MAC = \"%x %x %x %x %x %x\".\n",  tWifiMac_R.mac_addr[0],
																			  tWifiMac_R.mac_addr[1],
																			  tWifiMac_R.mac_addr[2],
																			  tWifiMac_R.mac_addr[3],
																			  tWifiMac_R.mac_addr[4],
																			  tWifiMac_R.mac_addr[5]);
	strLog_CS.Format("%s",strLog);

	if ( memcmp  ( tWifiMac_W.mac_addr, tWifiMac_R.mac_addr, MAC_ADDR_LEN))
	{			
		m_Result.GetWindowText(LOG_Show_Old);  
		m_Result.SetWindowText(strLog_CS+"\r\n写入的与读出的WIFI_MAC不一致，请重新写入！！！\r\n\r\n"+LOG_Show_Old);
		m_Result.UpdateWindow();
		return false;
	}
	else
	{
		m_Result.GetWindowText(LOG_Show_Old);  
		m_Result.SetWindowText(strLog_CS+"\r\n写入的与读出的WIFI_MAC一致，写WIFI_MAC地址确认OK！！！\r\n\r\n"+LOG_Show_Old);
		m_Result.UpdateWindow();
	}

	if(BackupSecondImei)
	{
		//sprintf ( strLog, "\n>>Step: To Backup NVRAM to Bin Region, and Set Clean Boot Flag\n" );
	  	//if ( WM_META_SetCleanBootFlag_Customize ( g_Meta.iMeta_handle ) )   ///// Clean boot is very Importnant for Customer.
		if(META_SUCCESS!=SP_META_SetCleanBootFlag_r ( g_Meta.iMetaAP_handle, 5000, &req, &cnf ))
		{
			m_Result.GetWindowText(LOG_Show_Old);  
			m_Result.SetWindowText("WIFI_MAC---Fail to Backup NVRAM and WM_META_SetCleanBootFlag_Customize！！！\r\n\r\n"+LOG_Show_Old);
			m_Result.UpdateWindow();
			return false;//IMEI_Write_Result=false;
		}
		else
		{
			m_Result.GetWindowText(LOG_Show_Old);  
			m_Result.SetWindowText("WIFI_MAC---Success to Backup NVRAM and WM_META_SetCleanBootFlag_Customize...继续...\r\n\r\n"+LOG_Show_Old);
			m_Result.UpdateWindow();
		}
	}
	return true;
	//sprintf ( strLog, ">>Step: Meta Disconnect with target.\n");
	//SP_META_DisconnectWithTarget_r ( g_Meta.iMetaAP_handle );//Exit_MetaAPMode ();
	
}
META_RESULT CWriteIMEIDlg::REQ_WriteWifiMAC2NVRAM_Start (WiFi_MacAddress_S *MACAddress)
{
    signed int m_usRID = 1;
    signed int m_iBTBufSize;
    const char *pLID = "AP_CFG_RDEB_FILE_WIFI_LID";
    //nvram_ef_btradio_mt6611_struct  tBT;
    int index = 0;
    unsigned short int tempValue = 0;
    unsigned long ChipVersion;
  

    g_MetaAP.eMetaAP_Result = SP_META_NVRAM_GetRecLen ( pLID, &m_iBTBufSize );
    if ( g_MetaAP.eMetaAP_Result != META_SUCCESS )
    {
        return g_MetaAP.eMetaAP_Result;
    }


	m_Result.GetWindowText(LOG_Show_Old);  
	m_Result.SetWindowText("获取WIFI芯片型号...\r\n\r\n"+LOG_Show_Old);
	m_Result.UpdateWindow();

    g_MetaAP.eMetaAP_Result = SP_META_WiFi_GetChipVersion_r(g_Meta.iMetaAP_handle, 5000, &ChipVersion);
    if(g_MetaAP.eMetaAP_Result != META_SUCCESS)
    {
		m_Result.GetWindowText(LOG_Show_Old);  
		m_Result.SetWindowText("获取WIFI芯片型号失败！！！\r\n\r\n"+LOG_Show_Old);
		m_Result.UpdateWindow();
		return g_MetaAP.eMetaAP_Result;
    }
	else
	{
		m_Result.GetWindowText(LOG_Show_Old);  
		m_Result.SetWindowText("获取WIFI芯片型号成功，继续...\r\n\r\n"+LOG_Show_Old);
		m_Result.UpdateWindow();
	}

    if(ChipVersion == MT6620)
    {
		g_MetaAP.tNVRAM_ReadReq.LID = pLID;
	   	g_MetaAP.tNVRAM_ReadReq.RID = m_usRID;
	    g_MetaAP.tNVRAM_ReadCnf.len = m_iBTBufSize;
	    g_MetaAP.tNVRAM_ReadCnf.buf = (unsigned char*) malloc (m_iBTBufSize*sizeof(unsigned char));

		g_MetaAP.eMetaAP_Result =  REQ_ReadFromNVRAM_AP ();//读NVRAM----智能机WIFI(MT6620)
		if ( g_MetaAP.eMetaAP_Result!=META_SUCCESS )
       	{
			if ( NULL != g_MetaAP.tNVRAM_ReadCnf.buf )
			{
				free ( g_MetaAP.tNVRAM_ReadCnf.buf );	//读NVRAM失败，释放分配的存储空间--Read----智能机WIFI(MT6620)
				g_MetaAP.tNVRAM_ReadCnf.buf = NULL;
			}
			m_Result.GetWindowText(LOG_Show_Old);  
			m_Result.SetWindowText("(WIFI)读取NVRAM失败！！！\r\n\r\n"+LOG_Show_Old);
			m_Result.UpdateWindow();
    	    return g_MetaAP.eMetaAP_Result;
       	}
		else
		{
			m_Result.GetWindowText(LOG_Show_Old);  
			m_Result.SetWindowText("(WIFI)读取NVRAM成功，继续...\r\n\r\n"+LOG_Show_Old);
			m_Result.UpdateWindow();
		}

	       //Modify MAC Address Part
       	for (index = 0; index <6; index=index+2)
       	{
            g_MetaAP.tNVRAM_ReadCnf.buf[0x4+index+1] = MACAddress->mac_addr[index];
            g_MetaAP.tNVRAM_ReadCnf.buf[0x4+index] = MACAddress->mac_addr[index+1];
       	}

	    Checksum_Generation (g_MetaAP.tNVRAM_ReadCnf.buf);
			
		m_usRID = 1;
		g_MetaAP.tNVRAM_WriteReq.LID = pLID;
       	g_MetaAP.tNVRAM_WriteReq.RID = m_usRID;
       	g_MetaAP.tNVRAM_WriteReq.len = m_iBTBufSize;
       	g_MetaAP.tNVRAM_WriteReq.buf = g_MetaAP.tNVRAM_ReadCnf.buf;
	       
		m_Result.GetWindowText(LOG_Show_Old);  
		m_Result.SetWindowText("ChipVersion == MT6620(WIFI)开始写NVRAM...\r\n\r\n"+LOG_Show_Old);
		m_Result.UpdateWindow();
		g_MetaAP.eMetaAP_Result = REQ_Write2NVRAM ();
	    
		if ( g_MetaAP.eMetaAP_Result != META_SUCCESS )
       	{
			m_Result.GetWindowText(LOG_Show_Old);  
			m_Result.SetWindowText("ChipVersion == MT6620(WIFI)写NVRAM失败！！！\r\n\r\n"+LOG_Show_Old);
			m_Result.UpdateWindow();
    	    return g_MetaAP.eMetaAP_Result;
       	}
		else
		{
			m_Result.GetWindowText(LOG_Show_Old);  
			m_Result.SetWindowText("ChipVersion == MT6620(WIFI)写NVRAM成功，继续...\r\n\r\n"+LOG_Show_Old);
			m_Result.UpdateWindow();
		}


	    if ( NULL != g_MetaAP.tNVRAM_ReadCnf.buf )
		{
			free ( g_MetaAP.tNVRAM_ReadCnf.buf );  //最后，释放分配的存储空间--Read---智能机(WIFI)
			g_MetaAP.tNVRAM_ReadCnf.buf = NULL;
		}

    }
	else// if(ChipVersion == MT5921)
	{
		g_MetaAP.tNVRAM_ReadReq.LID = pLID;
	    g_MetaAP.tNVRAM_ReadReq.RID = m_usRID;
	    g_MetaAP.tNVRAM_ReadCnf.len = m_iBTBufSize;
	    g_MetaAP.tNVRAM_ReadCnf.buf = (unsigned char*) malloc (m_iBTBufSize*sizeof(unsigned char));

		g_MetaAP.eMetaAP_Result =  REQ_ReadFromNVRAM_AP ();//读NVRAM----智能机WIFI
		if ( g_MetaAP.eMetaAP_Result!=META_SUCCESS )
	    {
			if ( NULL != g_MetaAP.tNVRAM_ReadCnf.buf )
			{
				free ( g_MetaAP.tNVRAM_ReadCnf.buf );	//读NVRAM失败，释放分配的存储空间--Read---智能机(WIFI)
				g_MetaAP.tNVRAM_ReadCnf.buf = NULL;
			}
			m_Result.GetWindowText(LOG_Show_Old);  
			m_Result.SetWindowText("(WIFI)读取NVRAM失败！！！\r\n\r\n"+LOG_Show_Old);
			m_Result.UpdateWindow();
			return g_MetaAP.eMetaAP_Result;
	    }
		else
		{
			m_Result.GetWindowText(LOG_Show_Old);  
			m_Result.SetWindowText("(WIFI)读取NVRAM成功，继续...\r\n\r\n"+LOG_Show_Old);
			m_Result.UpdateWindow();
		}

	     //Modify MAC Address Part
	       
	    for (index = 0; index <6; index=index+2)
	    {
			g_MetaAP.tNVRAM_ReadCnf.buf[0x26+index+1] = MACAddress->mac_addr[index];
	        g_MetaAP.tNVRAM_ReadCnf.buf[0x26+index] = MACAddress->mac_addr[index+1];
	    }

	    Checksum_Generation (g_MetaAP.tNVRAM_ReadCnf.buf);
		m_usRID = 1;
		g_MetaAP.tNVRAM_WriteReq.LID = pLID;
	    g_MetaAP.tNVRAM_WriteReq.RID = m_usRID;
	    g_MetaAP.tNVRAM_WriteReq.len = m_iBTBufSize;
	    g_MetaAP.tNVRAM_WriteReq.buf = g_MetaAP.tNVRAM_ReadCnf.buf;
	    
		m_Result.GetWindowText(LOG_Show_Old);  
		m_Result.SetWindowText("(WIFI)开始写NVRAM...\r\n\r\n"+LOG_Show_Old);
		m_Result.UpdateWindow();
		g_MetaAP.eMetaAP_Result = REQ_Write2NVRAM ();
	
		if ( g_MetaAP.eMetaAP_Result != META_SUCCESS )
       	{
			m_Result.GetWindowText(LOG_Show_Old);  
			m_Result.SetWindowText("(WIFI)写NVRAM失败！！！\r\n\r\n"+LOG_Show_Old);
			m_Result.UpdateWindow();
    	    return g_MetaAP.eMetaAP_Result;
       	}
		else
		{
			m_Result.GetWindowText(LOG_Show_Old);  
			m_Result.SetWindowText("(WIFI)写NVRAM成功，继续...\r\n\r\n"+LOG_Show_Old);
			m_Result.UpdateWindow();
		}

	    if ( NULL != g_MetaAP.tNVRAM_ReadCnf.buf )
		{
			free ( g_MetaAP.tNVRAM_ReadCnf.buf );  //最后，释放分配的存储空间--Read---智能机(WIFI)
			g_MetaAP.tNVRAM_ReadCnf.buf = NULL;
		}
    }
	
    return META_SUCCESS;
}
void CWriteIMEIDlg::Checksum_Generation(unsigned char StorageBuf[])
{
	int u2Len = (EEPROM_NIC_CHKSUM_ADD_DW - EEPROM_NIC_CHKSUM_START_ADD_DW + 1) * 2;

	/* 5921 Start NIC EEPROM Checksum from 0x0D ~ 0x9F */
	int StartIndex = EEPROM_NIC_CHKSUM_START_ADD_DW * 2;	//from word to byte
	int EndIndex = EEPROM_NIC_CHKSUM_ADD_DW * 2;
	int LoopIndex = 0;
	unsigned char Chksum = 0;
	unsigned char New_Chksum = 0;
	unsigned char Old_Chksum = 0;
	
    //for (LoopIndex = StartIndex; LoopIndex <= EndIndex; LoopIndex++)
    for (LoopIndex = 0; LoopIndex < u2Len - 1; LoopIndex++)//not count 0x13F
    {
          Chksum = (Chksum + StorageBuf[LoopIndex + StartIndex]) & 0x00FF;
    }
    Old_Chksum = StorageBuf[0x13F];
	New_Chksum = (0x01FF - Chksum) & 0x00FF;
    
    StorageBuf[0x13F] = (0x01FF - Chksum) & 0x00FF;
}
META_RESULT CWriteIMEIDlg::REQ_ReadWifiMACFromNVRAM_Start ( WiFi_MacAddress_S *MACAddress )
{
	signed int m_usRID = 1;
    signed int m_iBTBufSize;
    const char *pLID = "AP_CFG_RDEB_FILE_WIFI_LID";
    int index = 0;

    unsigned long ChipVersion;
	
    g_MetaAP.eMetaAP_Result = SP_META_NVRAM_GetRecLen ( pLID, &m_iBTBufSize );
    if ( g_MetaAP.eMetaAP_Result != META_SUCCESS )
    {
		return g_MetaAP.eMetaAP_Result;
    }

    /*if ( NULL != g_MetaAP.tNVRAM_ReadCnf.buf )
    {
	    free ( g_MetaAP.tNVRAM_ReadCnf.buf );	//提前，释放分配的存储空间--Read---智能机（WIFI）
	    g_MetaAP.tNVRAM_ReadCnf.buf = NULL;
    }*/
	g_MetaAP.eMetaAP_Result = SP_META_WiFi_GetChipVersion_r(g_Meta.iMetaAP_handle, 5000, &ChipVersion);
    if(g_MetaAP.eMetaAP_Result != META_SUCCESS)
    {
		return g_MetaAP.eMetaAP_Result;
    }

    if(ChipVersion == MT6620)
    {
		g_MetaAP.tNVRAM_ReadReq.LID = pLID;
	    g_MetaAP.tNVRAM_ReadReq.RID = m_usRID;
	    g_MetaAP.tNVRAM_ReadCnf.len = m_iBTBufSize;
	    g_MetaAP.tNVRAM_ReadCnf.buf = (unsigned char*) malloc (m_iBTBufSize*sizeof(unsigned char));

		g_MetaAP.eMetaAP_Result =  REQ_ReadFromNVRAM_AP ();//读NVRAM----智能机WIFI(MT6620)
		if ( g_MetaAP.eMetaAP_Result!=META_SUCCESS )
	    {
			if ( NULL != g_MetaAP.tNVRAM_ReadCnf.buf )
			{
				free ( g_MetaAP.tNVRAM_ReadCnf.buf );	   //读NVRAM失败，释放分配的存储空间--Read---智能机(WIFI)
				g_MetaAP.tNVRAM_ReadCnf.buf = NULL;
			}
			m_Result.GetWindowText(LOG_Show_Old);  
			m_Result.SetWindowText("(WIFI)读取NVRAM失败！！！\r\n\r\n"+LOG_Show_Old);
			m_Result.UpdateWindow();
	        return g_MetaAP.eMetaAP_Result;
	    }
		else
		{
			m_Result.GetWindowText(LOG_Show_Old);  
			m_Result.SetWindowText("(WIFI)读取NVRAM成功，继续...\r\n\r\n"+LOG_Show_Old);
			m_Result.UpdateWindow();
		}

	    for (index=0; index < 6; index=index+2)
	    {
	    	MACAddress->mac_addr[index] = g_MetaAP.tNVRAM_ReadCnf.buf[0x4+index+1];
	    	MACAddress->mac_addr[index+1] = g_MetaAP.tNVRAM_ReadCnf.buf[0x4+index];
	    }	
	}
	else //MT5921
	{
		g_MetaAP.tNVRAM_ReadReq.LID = pLID;
	    g_MetaAP.tNVRAM_ReadReq.RID = m_usRID;
	    g_MetaAP.tNVRAM_ReadCnf.len = m_iBTBufSize;
	    g_MetaAP.tNVRAM_ReadCnf.buf = (unsigned char*) malloc (m_iBTBufSize*sizeof(unsigned char));
		g_MetaAP.eMetaAP_Result =  REQ_ReadFromNVRAM_AP ();//读NVRAM----智能机WIFI
		if ( g_MetaAP.eMetaAP_Result!=META_SUCCESS )
	    {
			if ( NULL != g_MetaAP.tNVRAM_ReadCnf.buf )
			{
				free ( g_MetaAP.tNVRAM_ReadCnf.buf );	//读NVRAM失败，释放分配的存储空间--Read---智能机(WIFI)
				g_MetaAP.tNVRAM_ReadCnf.buf = NULL;
			}
			m_Result.GetWindowText(LOG_Show_Old);  
			m_Result.SetWindowText("(WIFI)读取NVRAM失败！！！\r\n\r\n"+LOG_Show_Old);
			m_Result.UpdateWindow();
	        return g_MetaAP.eMetaAP_Result;
	    }
		else
		{
			m_Result.GetWindowText(LOG_Show_Old);  
			m_Result.SetWindowText("(WIFI)读取NVRAM成功，继续...\r\n\r\n"+LOG_Show_Old);
			m_Result.UpdateWindow();
		}

	    for (index=0; index < 6; index=index+2)
	    {
	    	MACAddress->mac_addr[index] = g_MetaAP.tNVRAM_ReadCnf.buf[0x26+index+1];
	    	MACAddress->mac_addr[index+1] = g_MetaAP.tNVRAM_ReadCnf.buf[0x26+index];
	    }
	}
	
    if ( NULL != g_MetaAP.tNVRAM_ReadCnf.buf )
    {
      		free ( g_MetaAP.tNVRAM_ReadCnf.buf );	//最后，释放分配的存储空间--Read---智能机(WIFI)
			g_MetaAP.tNVRAM_ReadCnf.buf = NULL;
    }

    return META_SUCCESS;
}
//ApMeta转换Modem
META_RESULT CWriteIMEIDlg::ApMetaToModemMeta ( void )//Switch2WM_META_Mode
{
    META_RESULT MetaResult=META_SUCCESS;
	META_ConnectInMETA_Req BootRequest;
	int BootStop=0;
	META_ConnectInMETA_Report BootReport;



	META_ConnectByUSB_Req usbRequest;
    META_ConnectByUSB_Report usbReport;

    //MetaResult = AP_META_DisconnectInMetaMode_r (g_Meta.iMetaAP_handle);
	//MetaResult = WM_META_DisconnectInMetaMode_r(g_Meta.iMetaAP_handle);
	if(META_SUCCESS != SP_META_DisconnectInMetaMode_r(g_Meta.iMetaAP_handle))
	{
        // MessagePopup ("SP", "DisconnectFromApMeta Fail!");
		return MetaResult;
	}

	//---------------------Connect with target ----------------------------------
	
	int temp_com_port = 0;
	m_Port.GetWindowText(m_bUsbMode);
	CString m_bUsbMode_temp=m_bUsbMode.Right(m_bUsbMode.GetLength()-3);
	temp_com_port = atoi(m_bUsbMode_temp);
    if (m_bUsbMode=="USB")
    {
        temp_com_port=USB_COM_PORT; //使用USB口
    }
	BootRequest.com_port = temp_com_port;//GetCOMPort ();
	// [META] META stage baudrate polling array, it must end with META_BAUD_END.
	BootRequest.baudrate[0] = META_BAUD115200;
	BootRequest.baudrate[1] = META_BAUD230400;
	BootRequest.baudrate[2] = META_BAUD460800;
	BootRequest.baudrate[3] = META_BAUD921600;
	BootRequest.baudrate[4] = META_BAUD57600;
	BootRequest.baudrate[5] = META_BAUD_END;

	BootRequest.flowctrl = META_SW_FLOWCTRL;
	BootRequest.ms_connect_timeout = 80000; //30000;

	g_Meta.bootstop = BOOT_INFINITE;   //智能机---这样才能进入META模式，待定

    if (m_bUsbMode=="USB")
    {
        
	    usbRequest.com_port = kernel_port;
        usbRequest.ms_connect_timeout =8000;
        MetaResult = META_ConnectInMetaModeByUSB_r(g_Meta.iMeta_handle, &usbRequest, &g_Meta.bootstop, &usbReport);
	}
	else
	{
		MetaResult = META_ConnectInMetaMode_r(g_Meta.iMeta_handle, &BootRequest, &BootStop, &BootReport);
	}

	
	if(META_SUCCESS != MetaResult && META_MAUI_DB_INCONSISTENT != MetaResult)
	{
        // MessagePopup ("SP", "META_ConnectInMetaMode_r Fail!");
		return MetaResult;
	}

	return META_SUCCESS;
}
//ModemA转换pMeta
META_RESULT CWriteIMEIDlg::ModemMetaToAPMeta (void)
{
	META_RESULT MetaResult;
    WM_META_ConnectInMETA_Req BootRequest;
	int BootStop = 0;
	WM_META_ConnectInMETA_Report BootReport;

    MetaResult = META_DisconnectInMetaMode_r (g_Meta.iMeta_handle);
    //MetaResult = FP_META_DisconnectInMetaMode_r (g_Meta.iMeta_handle);
	if (META_SUCCESS != MetaResult)
	{
		return MetaResult;
	}

	int temp_com_port = 0;
	m_Port.GetWindowText(m_bUsbMode);
	CString m_bUsbMode_temp=m_bUsbMode.Right(m_bUsbMode.GetLength()-3);
	temp_com_port = atoi(m_bUsbMode_temp);
    if (m_bUsbMode=="USB")
    {
        temp_com_port=USB_COM_PORT; //使用USB口
    }
    BootRequest.com_port = temp_com_port;
	BootRequest.baudrate[0] = META_BAUD115200;
	BootRequest.baudrate[1] = META_BAUD230400;
	BootRequest.baudrate[2] = META_BAUD460800;
	BootRequest.baudrate[3] = META_BAUD921600;
	BootRequest.baudrate[4] = META_BAUD57600;
	BootRequest.baudrate[5] = META_BAUD_END;

	BootRequest.flowctrl = META_NO_FLOWCTRL;//META_SW_FLOWCTRL;
	BootRequest.ms_connect_timeout = 80000;

	 if (m_bUsbMode=="USB")
     {
            BootRequest.com_port = kernel_port;
        }

	MetaResult = SP_META_ConnectInMetaMode_r(g_Meta.iMetaAP_handle, &BootRequest, &BootStop, &BootReport);
	Sleep(100);
	if (META_SUCCESS != MetaResult)
	{
		return MetaResult;
	}

	return MetaResult;
}

void CWriteIMEIDlg::OnBnClickedButton6()// MTK_META规则文件路径 Modem Database
{
	// TODO: 在此添加控件通知处理程序代码 m_strExchangeFileName_Ap
	 CFileDialog fDlg(TRUE,
                     _T("ini"),
                     NULL,
                     OFN_HIDEREADONLY|OFN_FILEMUSTEXIST,
                     _T("MTK Modem database (*.*)|*.*|所有文件 (*.*)|*.*||"),
                     this);

    // 打开的路径为本程序所在的路径
    char szName[MAX_PATH] = _T("");
    CString  szPath       = _T("");

    GetModuleFileName(NULL, szName, MAX_PATH);

    szPath = szName;
    szPath = szPath.Mid(0, szPath.ReverseFind('\\')+1);
    fDlg.m_ofn.lpstrInitialDir = szPath;
    //AfxMessageBox(szPath);
    // 改变标题：Open - means 导入脚本文件
    fDlg.m_ofn.lpstrTitle = _T("导入MTK Modem database文件:");
	//AfxMessageBox("wait");

    if ( IDOK == fDlg.DoModal() )
    {
        // 依据得到的路径和文件名
        m_strExchangeFileName = fDlg.GetPathName(); 
		//AfxMessageBox(m_strExchangeFileName);
        m_sPath=m_strExchangeFileName;
        // 此处不再验证文件的有效性
        ASSERT(!m_strExchangeFileName.IsEmpty());
    }
	
    UpdateData(FALSE);	
}



//上传ORACLE数据库
bool CWriteIMEIDlg::Update_Oracle_Database()
{
				
	//AfxMessageBox("上传SQL成功！");
	//Setdlg.myado.CloseDataLibrary();////连接关闭必须在一个子函数内，不然出错=================================================成功上传SQL_SERVER数据库后，关闭SQL_SERVER数据库后再连接ORACLE
	extern COLORREF gColor;
	extern CString gFinalResult;
	HRESULT hr;
	CString Conn_ZTE="";
	//Conn_ZTE="Provider=MSDAORA.1;Persist Security Info=True;Password=barcode_59.33.252.107;User ID=barcode;Data Source=barcode";
	////////////////////////////////ORACLE数据库判断////////////////////////////
	CString DB_FAIL="";
	bool UP_FAIL_ZTE=true;
	if(Setdlg_Oracle.myado.m_pCon==NULL)//Setdlg
	{
		hr = Setdlg_Oracle.myado.m_pCon.CreateInstance("ADODB.Connection");
		CString strConnect;
		UP_FAIL_ZTE=true;
		if(SUCCEEDED(hr))
		{
			try
			{
				strConnect="Provider=MSDAORA.1;Data Source="+ORACLE_m_server_M;
				Setdlg_Oracle.myado.m_pCon->Open((_bstr_t)strConnect,(_bstr_t)ORACLE_m_user_M,(_bstr_t)ORACLE_m_pwd_M,adModeUnknown);
			}
			catch(_com_error e)
			{  // 捕捉连接异常
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("连接  ORACLE数据库  失败1! \r\n\r\n"+LOG_Show_Old);
				//gColor = RGB(255,0,0);
				//gFinalResult.Format("UP_Fail");
				//RedrawWindow();  
				//m_Result.UpdateWindow(); 
				UP_FAIL_ZTE=false;//return false;不能退出
			} // 未出现异常表示连接成功
		}
		else
		{
			m_Result.GetWindowText(LOG_Show_Old);  
			m_Result.SetWindowText("连接  ORACLE数据库  失败2! \r\n\r\n"+LOG_Show_Old);
			//gColor = RGB(255,0,0);
			//gFinalResult.Format("UP_Fail");
			//RedrawWindow();  
			m_Result.UpdateWindow(); 
			UP_FAIL_ZTE=false;//return false;不能退出,因为后续要删除SQL数据库中的错误数据
		}
	}
	if(Setdlg_Oracle.myado.m_pCon->State==0)  //1表示已经打开,0表示关闭，数据库意外断开，重连
	{
		m_Result.GetWindowText(LOG_Show_Old);  
		m_Result.SetWindowText("=================================数据库意外断开，重连........\r\n\r\n"+LOG_Show_Old);
		m_Result.UpdateWindow();
		///////////重连数据库/////////////////////////////////
		hr = Setdlg_Oracle.myado.m_pCon.CreateInstance("ADODB.Connection");
		CString strConnect;
		UP_FAIL_ZTE=true;
		if(SUCCEEDED(hr))
		{
			try
			{
				strConnect="Provider=MSDAORA.1;Data Source="+ORACLE_m_server_M;
				Setdlg_Oracle.myado.m_pCon->Open((_bstr_t)strConnect,(_bstr_t)ORACLE_m_user_M,(_bstr_t)ORACLE_m_pwd_M,adModeUnknown);
			}
			catch(_com_error e)
			{  // 捕捉连接异常
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("连接  ORACLE数据库  失败1! \r\n\r\n"+LOG_Show_Old);
				//gColor = RGB(255,0,0);
				//gFinalResult.Format("UP_Fail");
				//RedrawWindow();  
				m_Result.UpdateWindow(); 
				UP_FAIL_ZTE=false;//return false;不能退出
			} // 未出现异常表示连接成功
			
		}
		else
		{
			m_Result.GetWindowText(LOG_Show_Old);  
			m_Result.SetWindowText("连接  ORACLE数据库  失败2! \r\n\r\n"+LOG_Show_Old);
			//gColor = RGB(255,0,0);
			//gFinalResult.Format("UP_Fail");
			//RedrawWindow();  
			m_Result.UpdateWindow(); 
			UP_FAIL_ZTE=false;//return false;不能退出,因为后续要删除SQL数据库中的错误数据
		}
		//////////重连数据库ORACLE

	}
	else
	{
		m_Result.GetWindowText(LOG_Show_Old);  
		m_Result.SetWindowText("=================================ORACLE 数据库连接正常中...\r\n\r\n"+LOG_Show_Old);
		m_Result.UpdateWindow();
		COleDateTime Time_zte;
		//CString strSQL = "Insert into ZTE_MBC_BARCODE_IMEI(IMEI,NETCODE,SP_LOCK,BARCODE,NETCODE,MSN,WIP_ENTITY_NAME,LINENUM, CLASSNUM,PRODADDRESS, SOFTVERSION,HARDVERSION,TYPE,IMEI_WRITER,MOBCLASS,MEMO,WORKAGAIN,BLUE_MAC) values('"+m_IMEI+"','"+NETCODE_Input+"','"+Unlock_code_Number+"','"+BARCODE_Input+"','','"+m_SN+"','"+Order_Num_up_M+"','"+Line_Number_M+"','"+Floor_M+"','"+Product_Address_M+"','"+Software_Version_M+"','"+Hardware_Version_M+"','"+Machine_Type_M+"','"+Operator_M+"','"+Modulation_Tppe_M+"','Y','"+Is_Return_M+"','"+m_BT_Addr+"')";
		//CString strSQL = "Insert into ZTE_MBC_BARCODE_IMEI(IMEI,NETCODE,SP_LOCK,BARCODE,MSN,WIP_ENTITY_NAME,LINENUM,CLASSNUM,SOFTVERSION,HARDVERSION,TYPE,MOBCLASS,MEMO,WORKAGAIN,BLUE_MAC) values('"+m_IMEI+"','"+NETCODE_Input+"','"+Unlock_code_Number+"','"+BARCODE_Input+"','"+m_SN+"','"+Order_Num_up_M+"','"+Line_Number_M+"','"+Floor_M+"','"+Software_Version_M+"','"+Hardware_Version_M+"','"+Machine_Type_M+"','"+Modulation_Tppe_M+"','Y','"+Is_Return_M+"','"+m_BT_Addr+"')";
		
		//CString strSQL = "Insert into ZTE_MBC_BARCODE_IMEI(IMEI,BARCODE,NETCODE,MSN,WIP_ENTITY_NAME,LINENUM, CLASSNUM,PRODADDRESS, SOFTVERSION,HARDVERSION,TYPE,IMEI_WRITER,MOBCLASS,MEMO,WORKAGAIN,BLUE_MAC) values('"+m_IMEI+"','"+BoardNumber+"','','"+m_SN+"','"+Order_Num_up_M+"','"+Line_Number_M+"','"+Floor_M+"','"+Product_Address_M+"','"+Software_Version_M+"','"+Hardware_Version_M+"','"+Machine_Type_M+"','"+Operator_M+"','"+Modulation_Tppe_M+"','Y','"+Is_Return_M+"','"+m_BT_Addr+"')";
						

		CString strSQL = "Insert into ZTE_MBC_BARCODE_IMEI(IMEI,NETCODE,BARCODE,MSN,WIP_ENTITY_NAME,LINENUM,CLASSNUM,SOFTVERSION,HARDVERSION,TYPE,MOBCLASS,MEMO,WORKAGAIN,BLUE_MAC) values('"+m_IMEI+"','"+NETCODE_Input+"','"+BARCODE_Input+"','"+m_SN+"','"+Order_Num_up_M+"','"+Line_Number_M+"','"+Floor_M+"','"+Software_Version_M+"','"+Hardware_Version_M+"','"+Machine_Type_M+"','"+Modulation_Tppe_M+"','Y','"+Is_Return_M+"','"+m_BT_Addr+"')";

		Setdlg_Oracle.myado.OpenSheet("select * from ZTE_MBC_BARCODE_IMEI");
		UP_FAIL_ZTE=Setdlg_Oracle.myado.Execute(strSQL);	
		Setdlg_Oracle.myado.CloseSheet();

		if(UP_FAIL_ZTE!=true)
		{
			m_Result.GetWindowText(LOG_Show_Old);
			m_Result.SetWindowText("上传  ORACLE数据库表ZTE_MBC_BARCODE_IMEI失败！！！！！！！！！！！！！！！！！！！！！！！！ \r\nSQL语句:"+strSQL+"\r\n\r\n"+LOG_Show_Old);
			m_Result.UpdateWindow();
		}

		if(Unlock_Code_Choose_M=="true")
		{
			strSQL = "Insert into ZTE_LOCK_IMEI(IMEI,UNLOCK_CODE,BARCODE) values('"+m_IMEI+"','"+Unlock_code_Number+"','"+BARCODE_Input+"')";
					
			Setdlg_Oracle.myado.OpenSheet("select * from ZTE_LOCK_IMEI");
			UP_FAIL_ZTE &=Setdlg_Oracle.myado.Execute(strSQL);	
			Setdlg_Oracle.myado.CloseSheet();

			if(UP_FAIL_ZTE!=true)
			{
				m_Result.GetWindowText(LOG_Show_Old);
				m_Result.SetWindowText("上传  ORACLE数据库表ZTE_MBC_BARCODE_IMEI或者ZTE_LOCK_IMEI失败！！！！！！！！！！！！！！！！！！！！！！！！ \r\nSQL语句:"+strSQL+"\r\n\r\n"+LOG_Show_Old);
				m_Result.UpdateWindow();
			}
		}
	}
	
	////////////////////////////////ORACLE数据库判断///////////////////////////////////////////
	
	BOOL IMEI_Check_UP_ZTE=TRUE;
	if(UP_FAIL_ZTE==true)
	{
		Sleep(500);
		Setdlg_Oracle.myado.OpenSheet("select IMEI from ZTE_MBC_BARCODE_IMEI WHERE IMEI ='"+m_IMEI+"'");
		IMEI_Check_UP_ZTE=Setdlg_Oracle.myado.Find("IMEI='"+m_IMEI+"'");
		Setdlg_Oracle.myado.CloseSheet();

		if(IMEI_Check_UP_ZTE!=TRUE)
		{
			m_Result.GetWindowText(LOG_Show_Old);
			m_Result.SetWindowText("上传  ORACLE数据库表ZTE_MBC_BARCODE_IMEI 再次确认失败！！！！！！！！！！！！！！！！！！！！！！！！ \r\nSQL语句:select IMEI from ZTE_MBC_BARCODE_IMEI WHERE IMEI ='"+m_IMEI+"'"+"\r\n\r\n"+LOG_Show_Old);
			m_Result.UpdateWindow();
		}
		
		if(Unlock_Code_Choose_M=="true")
		{
			Setdlg_Oracle.myado.OpenSheet("select IMEI from ZTE_LOCK_IMEI WHERE IMEI ='"+m_IMEI+"'");
			IMEI_Check_UP_ZTE &=Setdlg_Oracle.myado.Find("IMEI='"+m_IMEI+"'");
			Setdlg_Oracle.myado.CloseSheet();

			if(IMEI_Check_UP_ZTE!=TRUE)
			{
				m_Result.GetWindowText(LOG_Show_Old);
				m_Result.SetWindowText("上传  ORACLE数据库表ZTE_MBC_BARCODE_IMEI或者ZTE_LOCK_IMEI 再次确认失败！！！！！！！！！！！！！！！！！！！！！！！！ \r\nSQL语句:select IMEI from ZTE_LOCK_IMEI WHERE IMEI ='"+m_IMEI+"'"+"\r\n\r\n"+LOG_Show_Old);
				m_Result.UpdateWindow();
			}
		}
	}
	if(UP_FAIL_ZTE==true&&IMEI_Check_UP_ZTE==TRUE)
	{
		
		m_Result.GetWindowText(LOG_Show_Old);
		m_Result.SetWindowText("上传  ORACLE数据库成功...... \r\n\r\n"+LOG_Show_Old);
		//UpdateData(true);  //把中间值赋给变量  多线程中不允许
		//UpdateData(false);//把变量显示在界面
		m_Result.UpdateWindow();
		//gColor = RGB(0,255,0);
		//gFinalResult.Format("     OK");
		//RedrawWindow(); 
	}
	else
	{
		m_Result.GetWindowText(LOG_Show_Old);
		m_Result.SetWindowText("上传  ORACLE数据库失败...... \r\n\r\n"+LOG_Show_Old);
		//UpdateData(true);  //把中间值赋给变量  多线程中不允许
		//UpdateData(false);//把变量显示在界面
		m_Result.UpdateWindow();
		
		//删除SQL_Server数据库错误数据
		CString Conn="";
		CString DB_FAIL;
		Conn.Format("driver={SQL Server};Server=%s;DATABASE=%s;UID=%s;PWD=%s", m_server_M,m_db_M,m_user_M, m_pwd_M);
		if(Setdlg.myado.m_pCon==NULL)
		{
			DB_FAIL=Setdlg.myado.ConnecDataLibrary(Conn,"","",adModeUnknown);   
		}
		if(Setdlg.myado.m_pCon->State==0)  //1表示已经打开,0表示关闭，数据库意外断开，重连
		{
			m_Result.GetWindowText(LOG_Show_Old);  
			m_Result.SetWindowText("=================================数据库意外断开，重连........\r\n\r\n"+LOG_Show_Old);
			m_Result.UpdateWindow();
			DB_FAIL=Setdlg.myado.ConnecDataLibrary(Conn,"","",adModeUnknown);//关闭ORACLE数据库，再连接SQL Server---数据库意外断开，重连
		}
		else
		{
			m_Result.GetWindowText(LOG_Show_Old);  
			m_Result.SetWindowText("=================================数据库连接正常中...\r\n\r\n"+LOG_Show_Old);
			m_Result.UpdateWindow();
		}
		if(DB_FAIL=="FAIL")
		{
			m_Result.GetWindowText(LOG_Show_Old);
			m_Result.SetWindowText("删除SQL_Server 数据库中的  错误数据失败，请手动删除！！！！！！！！！！！！！！！！！ \r\n\r\n"+LOG_Show_Old);
			//gColor = RGB(255,0,0);
			//gFinalResult.Format("UP_Fail");
			//RedrawWindow();  
			m_Result.UpdateWindow(); 
			//return;
		}
		else
		{
			bool UP_FAIL=false;
			Setdlg.myado.OpenSheet("select * from dse_barcode_imei");
			//strSQL = "DELETE FROM dse_barcode_imei WHERE (barcode='"+BoardNumber+"')";//imei ='"+m_IMEI+"' dse_barcode_imei
			CString strSQL = "DELETE FROM dse_barcode_imei WHERE (imei ='"+m_IMEI+"')";
			UP_FAIL=Setdlg.myado.Execute(strSQL);	
			if(UP_FAIL==true)
			{
				m_Result.GetWindowText(LOG_Show_Old);
				m_Result.SetWindowText("成功删除SQL_Server  错误数据！！！！！！！！！！！！！！！！！\r\n\r\n"+LOG_Show_Old);
				//gColor = RGB(255,0,0);
				//gFinalResult.Format("UP_Fail");
				//RedrawWindow();  
				m_Result.UpdateWindow(); 
			}
			else
			{
				m_Result.GetWindowText(LOG_Show_Old);
				m_Result.SetWindowText("删除SQL_Server  错误数据失败，请手动删除！！！！！！！！！！！！！！！！！ \r\n\r\n"+LOG_Show_Old);
				//gColor = RGB(255,0,0);
				//gFinalResult.Format("UP_Fail");
				//RedrawWindow();  
				m_Result.UpdateWindow(); 
			}
			//Setdlg.myado.CloseDataLibrary();//此时不关闭数据库==================================================删除错误数据后，再关闭SQL Server数据库
		}
		
		//删除SQL_Server数据库错误数据
		return false;
	}
	return true;
}
void CWriteIMEIDlg::OnEnChangeBtAddr3()//选择了话机板号OnEnChangeImei9
{
	//待处理
}

void CWriteIMEIDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	MetaContextDeInit();//清除MTK的初始化
	OnOK();
}

void CWriteIMEIDlg::OnBnClickedButton7()//AP Database路径
{
	// TODO: 在此添加控件通知处理程序代码 
	 CFileDialog fDlg(TRUE,
                     _T("ini"),
                     NULL,
                     OFN_HIDEREADONLY|OFN_FILEMUSTEXIST,
                     _T("MTK Modem database (*.*)|*.*|所有文件 (*.*)|*.*||"),
                     this);

    // 打开的路径为本程序所在的路径
    char szName[MAX_PATH] = _T("");
    CString  szPath       = _T("");

    GetModuleFileName(NULL, szName, MAX_PATH);

    szPath = szName;
    szPath = szPath.Mid(0, szPath.ReverseFind('\\')+1);
    fDlg.m_ofn.lpstrInitialDir = szPath;
    //AfxMessageBox(szPath);
    // 改变标题：Open - means 导入脚本文件
    fDlg.m_ofn.lpstrTitle = _T("导入MTK Ap database文件:");
	//AfxMessageBox("wait");

    if ( IDOK == fDlg.DoModal() )
    {
        // 依据得到的路径和文件名
        m_strExchangeFileName_Ap = fDlg.GetPathName(); 
        m_sPath_AP=m_strExchangeFileName_Ap;
        // 此处不再验证文件的有效性
        ASSERT(!m_strExchangeFileName_Ap.IsEmpty());
    }
	
    UpdateData(FALSE);	
}


void CWriteIMEIDlg::func_four_in_one_hdlr_smart_phone ( void )
{

}
void CWriteIMEIDlg::OnBnClickedButton8()//数据库操作
{
	// TODO: 在此添加控件通知处理程序代码
	CDataBase dig1;
	ShowWindow(FALSE);
	dig1.DoModal();//.DoModal();
	ShowWindow(TRUE);
}


void CWriteIMEIDlg::OnBnClickedButtonstart10()
{
	// TODO: 在此添加控件通知处理程序代码
	
	//Sensor_Test_Thread.GetClientRect(&rs);
	//调整子对话框在父窗口中的位置
	//ClientToScreen(&rs);//移动窗口，控件跟着移动
	//设置子对话框尺寸并移动到指定位置

	GetDlgItem(IDC_BUTTON10)->EnableWindow(FALSE);
	//GetDlgItem(IDC_BUTTON1)->EnableWindow(FALSE);
	//CWinThread *pThread1=AfxBeginThread(RUNTIME_CLASS(CUIThread));//界面线程
	//Sleep(800);
	//CWinThread *pThread2=AfxBeginThread(RUNTIME_CLASS(CUIThread));//界面线程
	//Sleep(800);
	//CWinThread *pThread3=AfxBeginThread(RUNTIME_CLASS(CUIThread));//界面线程
	//Sleep(800);
	//CWinThread *pThread4=AfxBeginThread(RUNTIME_CLASS(CUIThread));//界面线程
}

void CWriteIMEIDlg::OnEnChangeNetcode()
{
	if(NETCODE_Choose_M=="true")
	{
		UpdateData(TRUE);
		UpdateData(false);//把变量显示在界面

		int NETCODE_length=NETCODE_Input.GetLength();
		
		//if(NETCODE_Input.Find(NETCODE_PRE)!=-1&&NETCODE_length==NETCODE_Count)//选择了网标
		if(NETCODE_Input.Find(NETCODE_PRE)!=-1&&   ((NETCODE_Input.Find("\r")!=-1)|| (NETCODE_Input.Find("\n")!=-1))  )//选择了IMEI1  &&IMEI_length==15
		{
			//check whether digits
			NETCODE_Input.Replace("\r","");
			NETCODE_Input.Replace("\n","");
			NETCODE_length=NETCODE_Input.GetLength();
			////////////////////入网号判断///////////////////
			//check whether digits
			if(NETCODE_Only_Num_Choose_M=="true")
			{
				for(int i=0;i<NETCODE_length;i++)
				{
					if(NETCODE_length!=NETCODE_Count||NETCODE_Input.GetAt(i) <'0' || NETCODE_Input.GetAt(i) >'9')
					{
						PrintError("Please check 网标. Only digits allowed!");
						m_Write.EnableWindow(true);
						NETCODE_Control.SetFocus();
						NETCODE_Control.SetSel(0,-1);
						return;
					}
				}
				/*long NetCodeStart_Number=0;
				long NetCodeEnd_Number=0;
				long NetCodeCurr_Number=0;
				NetCodeStart_Number=atol(NetCodeStart_M.Right(atoi(NetCode_SwiftNumber_M)));
				NetCodeEnd_Number=atol(NetCodeEnd_M.Right(atoi(NetCode_SwiftNumber_M)));
				NetCodeCurr_Number=atol(NETCODE_Input.Right(atoi(NetCode_SwiftNumber_M)));
				if((NetCodeCurr_Number>NetCodeEnd_Number)||(NetCodeCurr_Number<NetCodeStart_Number))
				{
					PrintError("Please check 网标. 超出了设定范围！！");
					m_Write.EnableWindow(true);
					NETCODE_Control.SetFocus();
					NETCODE_Control.SetSel(0,-1);
					return;
				}
				else
				{
					m_Result.GetWindowText(LOG_Show_Old);  
					m_Result.SetWindowText(" 网标在设定范围内，继续..\r\n\r\n"+LOG_Show_Old);
					m_Result.UpdateWindow();
				}*/
			}
			////////////////////入网号判断///////////////////
			NETCODE_Control.SetWindowTextA(NETCODE_Input);
			if((Scan_Choose_M=="true")&&(INITIAL_POWER==true))
				MAIN_Function();//
		}
		else if(NETCODE_length==5&&NETCODE_Input.Find(NETCODE_PRE.Left(5))==-1)
		{
			NETCODE_Control.SetFocus();
			NETCODE_Control.SetSel(0,-1);
			m_Result.GetWindowText(LOG_Show_Old);
			m_Result.SetWindowText("网标不正确，请重新扫描! \r\n\r\n"+LOG_Show_Old);
		}
		else if(NETCODE_length==2&&NETCODE_Input.Find(NETCODE_PRE.Left(2))==-1)
		{
			NETCODE_Control.SetFocus();
			NETCODE_Control.SetSel(0,-1);
			m_Result.GetWindowText(LOG_Show_Old);
			m_Result.SetWindowText("网标不正确，请重新扫描! \r\n\r\n"+LOG_Show_Old);
		}
		else if(NETCODE_length==1&&NETCODE_Input.Find(NETCODE_PRE.Left(1))==-1)
		{
			NETCODE_Control.SetFocus();
			NETCODE_Control.SetSel(0,-1);
			m_Result.GetWindowText(LOG_Show_Old);
			m_Result.SetWindowText("网标不正确，请重新扫描! \r\n\r\n"+LOG_Show_Old);
		}
	}
}

void CWriteIMEIDlg::Only_Update_Netcode()
{
	extern COLORREF gColor;
	extern CString gFinalResult;
	if(Initial_Connect_DB_M=="true")//上传数据库//初始化连接了数据库才能上传，权限限制
	{
		CString Conn="";
		CString DB_FAIL="";
		Conn.Format("driver={SQL Server};Server=%s;DATABASE=%s;UID=%s;PWD=%s", m_server_M,m_db_M,m_user_M, m_pwd_M);
		if(Setdlg.myado.m_pCon==NULL)
		{
			DB_FAIL=Setdlg.myado.ConnecDataLibrary(Conn,"","",adModeUnknown);   
		}
		if(Setdlg.myado.m_pCon->State==0)  //1表示已经打开,0表示关闭，数据库意外断开，重连
		{
			m_Result.GetWindowText(LOG_Show_Old);  
			m_Result.SetWindowText("=================================数据库意外断开，重连........\r\n\r\n"+LOG_Show_Old);
			m_Result.UpdateWindow();
			DB_FAIL=Setdlg.myado.ConnecDataLibrary(Conn,"","",adModeUnknown);//数据库意外断开，重连
		}
		else
		{
			m_Result.GetWindowText(LOG_Show_Old);  
			m_Result.SetWindowText("=================================数据库连接正常中...\r\n\r\n"+LOG_Show_Old);
			m_Result.UpdateWindow();
		}

		if(DB_FAIL=="FAIL")
		{
			IMEI_Control.SetFocus();
			IMEI_Control.SetSel(0,-1);
			m_Result.GetWindowText(LOG_Show_Old);
			m_Result.SetWindowText("连接数据库失败，无法上传数据，请检查网络.......\r\n\r\n"+LOG_Show_Old);
			gColor = RGB(255,0,0);
			gFinalResult.Format("UP_FAIL");
			RedrawWindow();  m_Result.UpdateWindow(); 
			return ;

			/*  后续会执行
				IMEI_Control.SetFocus();
				IMEI_Control.SetSel(0,-1);
				IMEI2_Result=true;//置0处理
				IMEI3_Result=true;
				IMEI4_Result=true;
				BT_Result=true;
				WIFI_Result=true;
			*/
		}

		BOOL UP_NetCode;
		BOOL IMEI_Check_UP_NETCODE;
		if(IMEI1_Exist==true)
		{
			if(IMEI_Exist_Update_NC_Choose_M!="true")//设置了写号表  不存在输入的IMEI,说明数据库写号数据（IMEI）重复
			{
				m_Result.GetWindowText(LOG_Show_Old);
				m_Result.SetWindowText("没有勾选《写号时已上传IMEI,更新对应网标》,数据库中本不应该存在该写号数据（IMEI="+m_IMEI+"），说明IMEI重复了！！！\r\n\r\n"+LOG_Show_Old);
				m_Result.UpdateWindow();

				gColor = RGB(255,0,0);
				gFinalResult.Format("UP_FAIL");
				RedrawWindow();  m_Result.UpdateWindow(); 
				return;
			}
			//更新网标CheckIMEI
			CString strSQL_Write_NetCode="UPDATE dse_barcode_imei SET NetCode = '"+NETCODE_Input+"' WHERE imei ='"+m_IMEI+"'";
			Setdlg.myado.OpenSheet("select * from dse_barcode_imei WHERE imei ='"+m_IMEI+"'");
			UP_NetCode=Setdlg.myado.Execute(strSQL_Write_NetCode);
			Setdlg.myado.CloseSheet();
			//更新网标
			m_Result.GetWindowText(LOG_Show_Old);
			m_Result.SetWindowText("更新IMEI对应的网标...\r\n\r\n"+LOG_Show_Old);
			m_Result.UpdateWindow();
		}
		else
		{
			if(IMEI_Exist_Update_NC_Choose_M=="true")//设置了写号表已经存在输入的IMEI,说明数据库漏掉了写号数据（IMEI）
			{
				m_Result.GetWindowText(LOG_Show_Old);
				m_Result.SetWindowText("勾选了《写号时已上传IMEI,更新对应网标》,说明数据库漏掉了写号数据（IMEI="+m_IMEI+"）\r\n\r\n"+LOG_Show_Old);
				m_Result.UpdateWindow();

				gColor = RGB(255,0,0);
				gFinalResult.Format("UP_FAIL");
				RedrawWindow();  m_Result.UpdateWindow(); 
				return;
			}
			//直接上传
			///////////////////解锁码///////////////////////////////////////
			if(Unlock_Code_Choose_M=="true")
			{
				TranslateMuch(m_IMEI);
				m_Result.GetWindowText(LOG_Show_Old);  
				m_Result.SetWindowText("获取IMEI："+m_IMEI+"对应的解锁码："+Unlock_code_Number+"\r\n"+LOG_Show_Old);
				m_Result.UpdateWindow();
			}
			///////////////////解锁码///////////////////////////////////////
			CString strSQL = "Insert into dse_barcode_imei(imei,NetCode,Unlock_Code,TYPE)\
							 values('"+m_IMEI+"','"+NETCODE_Input+"','"+Unlock_code_Number+"','"+Machine_Type_M+"')";	
						
			Setdlg.myado.OpenSheet("select * from dse_barcode_imei");
			UP_NetCode=Setdlg.myado.Execute(strSQL);
			Setdlg.myado.CloseSheet();
			m_Result.GetWindowText(LOG_Show_Old);
			m_Result.SetWindowText("直接上传IMEI对应的网标...\r\n\r\n"+LOG_Show_Old);
			m_Result.UpdateWindow();

		}
		if(UP_NetCode==TRUE)
		{
			//再次检查网标
			Sleep(500);
			Setdlg.myado.OpenSheet("select NetCode from dse_barcode_imei WHERE NetCode ='"+NETCODE_Input+"'");
			IMEI_Check_UP_NETCODE=Setdlg.myado.Find("NetCode='"+NETCODE_Input+"'");
			Setdlg.myado.CloseSheet();
			//再次检查网标
		}	
		
		
		if((UP_NetCode==TRUE&&IMEI_Check_UP_NETCODE==TRUE))
		{
			m_Result.GetWindowText(LOG_Show_Old);
			m_Result.SetWindowText("写IMEI对应的网标成功！\r\n\r\n"+LOG_Show_Old);
			m_Result.UpdateWindow();
			gColor = RGB(0,255,0);
			gFinalResult.Format("     OK");
			RedrawWindow();  m_Result.UpdateWindow(); 
		}
		else
		{
			m_Result.GetWindowText(LOG_Show_Old);
			m_Result.SetWindowText("上传数据库失败！！！！！！！！！！！！！\r\n\r\n"+LOG_Show_Old);
			m_Result.UpdateWindow();

			gColor = RGB(255,0,0);
			gFinalResult.Format("UP_FAIL");
			RedrawWindow();  m_Result.UpdateWindow(); 
		}
	}
	else
	{
		m_Result.GetWindowText(LOG_Show_Old);  
		m_Result.SetWindowText("因没有选择   初始化时连接数据库   无法上传数据库\r\n\r\n"+LOG_Show_Old);
		gColor = RGB(255,0,0);
		gFinalResult.Format("     UP_Fail");
		RedrawWindow();  m_Result.UpdateWindow(); 
	}
}

void CWriteIMEIDlg::Story_Show()
{
	CLibincStory Story;
	Story.DoModal();
}

LRESULT CWriteIMEIDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if(message == WM_SEND_MSG)
	{
		m_Result.GetWindowText(LOG_Show_Old);
		LOG_Show_Old=LOG_Show_Old.Left(2048);
		CString Message_Temp;
		Message_Temp.Format("%s",(TCHAR *)lParam);
		m_Result.SetWindowText(Message_Temp+"\r\n"+LOG_Show_Old);
		m_Result.UpdateWindow();
	}
	return CDialog::WindowProc(message, wParam, lParam); 
}

HBRUSH CWriteIMEIDlg::OnCtlColor(CDC* pDC,CWnd* pWnd,UINT nCtlColor)//改变颜色
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC,pWnd,nCtlColor);
	switch(pWnd->GetDlgCtrlID())
	{

	case(IDC_RESULT):
		pDC->SetTextColor(RGB(3,146,207));
		break;
	}
	return hbr;

}
void CWriteIMEIDlg::OnBnClickedButtonstart11()//点击打印贴纸
{
	CString SN;
	SN=m_IMEI.Left(14);
	SN=SN.Right(12);
	////////////////////////////
	//上传成功后调用动态库打印
	CZebraPrinter PRINTER;
	//
	CString SSID_Name,Password;
	SSID_Name="TDL1261-"+m_strWifi.Right(6);
	Password=m_IMEI.Right(8);

	int X_Position;		//整体X轴偏移
	int Y_Position;		//整体Y轴偏移
	int Height;			//高度
	int Width;			//宽度
	CString Font;			//字体
	int X_Pos[10]={0};		//[0]--网络名 值偏移量  [1]--"WIFI密码：" 字符偏移量  [2]--WIFI密码 值偏移量  [3]--MSN 值偏移量   [4]--IMEI  值偏移量  [5]--IMEI条码偏移量
	int Y_Pos[10]={0};		//[0]--MSN Y轴偏移量   [1]--IMEI Y轴偏移量  [2]--IMEI条码 Y轴偏移量 

	CString CH_Font;        //汉字字体
	//高度30，宽度0，X轴10，Y轴200，字体D，XPssidvalue100，XPpwname200，XPpwvalue300，XPmsnvalue50，XPimeivalue50，XPbarcode400，YPmsn260，YPimei300，YPbarcode300，

	Height=atoi(Get_Printer_Para("高度"));
	Width=atoi(Get_Printer_Para("宽度"));
	X_Position=atoi(Get_Printer_Para("X轴"));
	Y_Position=atoi(Get_Printer_Para("Y轴"));

	Font=(Get_Printer_Para("字体"));
	CH_Font=(Get_Printer_Para("汉字字体"));

	X_Pos[0]=atoi(Get_Printer_Para("XPssidvalue"));
	X_Pos[1]=atoi(Get_Printer_Para("XPpwname"));
	X_Pos[2]=atoi(Get_Printer_Para("XPpwvalue"));
	X_Pos[3]=atoi(Get_Printer_Para("XPmsnvalue"));
	X_Pos[4]=atoi(Get_Printer_Para("XPimeivalue"));
	X_Pos[5]=atoi(Get_Printer_Para("XPbarcode"));

	Y_Pos[0]=atoi(Get_Printer_Para("YPmsn"));
	Y_Pos[1]=atoi(Get_Printer_Para("YPimei"));
	Y_Pos[2]=atoi(Get_Printer_Para("YPbarcode"));


	bool Print_Result=PRINTER.OnPrintFunction(Height,Width,X_Position,Y_Position,(LPSTR)(LPCSTR)SSID_Name,(LPSTR)(LPCSTR)Password,(LPSTR)(LPCSTR)SN,(LPSTR)(LPCSTR)m_IMEI,(LPSTR)(LPCSTR)Font,X_Pos,Y_Pos,(LPSTR)(LPCSTR)CH_Font);

	if(Print_Result==true)
	{
		//AfxMessageBox("打印成功！");
	}
	else
	{
		AfxMessageBox("打印失败！");
	}
	/*
	if(DB_Result==false)
		return;
	HINSTANCE m_Handle;
	m_Handle=LoadLibrary("print_dll.dll");
	if(!m_Handle)
	{
		AfxMessageBox("Load print_dll.dll fail!");
		return ;
	}
	void (__stdcall * PrintFunc)(int,int,int,int,char*,char*,char*,char*,char*);//声明函数
	//PrintDll(x,y,hight,width:integer;datauser,password,ipaddress,database,imei:String);
	PrintFunc=(void (__stdcall *) (int,int,int,int,char*,char*,char*,char*,char*))GetProcAddress(m_Handle,"FrPrint");
	PrintFunc(2,2,50,50,(LPSTR)(LPCSTR)m_user_M,(LPSTR)(LPCSTR)m_pwd_M,(LPSTR)(LPCSTR)m_server_M,(LPSTR)(LPCSTR)m_db_M,(LPSTR)(LPCSTR)m_IMEI);
	*/
	//上传成功后调用动态库打印
	////////////////////////////
}

///////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
/////更新蓝牙芯片//////////////////////////////////////////////////////////////////////////////////
/*iRet = REQ_WriteAP_NVRAM_Start(WRITE_BT, m_sScanData.strBTAddr, 1);
if (iRet == META_SUCCESS)
{
    spNextStage = SP_WIFI_ADDRESS_STAGE;
}
else
{
    sprintf(m_strMsg, "ERROR!! BT : MetaResult = %s", ResultToString_SP(iRet));
    spNextStage = SP_END_STAGE;
}
*/

META_RESULT CWriteIMEIDlg::REQ_WriteAP_NVRAM_Start(WriteData_Type_e dataType, char *pInData, unsigned short iRID)
{
	unsigned int m_usRID = 1;

	 if(!pInData)
	{
		return META_INVALID_ARGUMENTS;//META_INVALID_ARGUMENTS;
	}   
	
	int	 iBufLen = 0;
	char *pLID = NULL;
	 
	/*
	if (!SP_META_NVRAM_GetRecLen ( "AP_CFG_RDCL_FILE_BT_ADDR_LID", &m_iBTBufSize ))     //===META_SUCCESS    to Test LID exist
	{
		pLID = "AP_CFG_RDCL_FILE_BT_ADDR_LID";  //after DUMA 1004
	} 
	else if (!SP_META_NVRAM_GetRecLen ( "AP_CFG_RDEB_FILE_BT_ADDR_LID", &m_iBTBufSize ))//===META_SUCCESS
	{
		pLID = "AP_CFG_RDEB_FILE_BT_ADDR_LID";  //before DUMA 1004
	} 
	else
	{
		return META_LID_INVALID;
	}
	*/

	switch(dataType)
	{       
	case WRITE_BT:
		iBufLen = 6;
		pLID = "AP_CFG_RDEB_FILE_BT_ADDR_LID";
		break;
	    
	case WRITE_WIFI:
		iBufLen = 6;
		pLID = "AP_CFG_RDEB_FILE_WIFI_LID";
		break;

	case WRITE_ETHERNET_MAC:
		iBufLen = 6;
		pLID = "AP_CFG_RDEB_FILE_ETHERNET_LID";
		break;

	case WRITE_TEMPERATURE:
		iBufLen = strlen(pInData);
		pLID = "AP_CFG_RDCL_FILE_AUDIO_SPEAKER_MONITOR_LID";
		break;

	default:
		goto FREEM;
		return META_INVALID_ARGUMENTS;
	}   

	const int	MAX_DATA_LENGTH = 6;
	int			iMetaTimeout = 5000;


	char pWriteData[MAX_DATA_LENGTH] = {0};
	char pReadData[MAX_DATA_LENGTH] = {0};
	int  iWriteBufSize = 0;

	unsigned long wifiChipVersion = 0;
	char *pFuncName = NULL;
	int  rs = 0;

	g_MetaAP.eMeta_Result = SP_META_NVRAM_GetRecLen ( pLID, &iWriteBufSize );
    if ( g_MetaAP.eMeta_Result != META_SUCCESS )
    {
		goto FREEM;
        return g_MetaAP.eMeta_Result;
    }

    

	g_MetaAP.tNVRAM_ReadReq.LID = pLID;
    g_MetaAP.tNVRAM_ReadReq.RID = m_usRID;
    g_MetaAP.tNVRAM_ReadCnf.len = iWriteBufSize;
    g_MetaAP.tNVRAM_ReadCnf.buf = (unsigned char*) malloc (iWriteBufSize*sizeof(unsigned char));

	m_Result.GetWindowText(LOG_Show_Old);  
	m_Result.SetWindowText("开始读取智能机的NVRAM...\r\n\r\n"+LOG_Show_Old);
	m_Result.UpdateWindow();


	g_MetaAP.eMeta_Result =  REQ_ReadFromNVRAM_AP ();  //读NVRAM----智能机BT 
	if ( g_MetaAP.eMeta_Result!=META_SUCCESS )
    {
		if ( NULL != g_MetaAP.tNVRAM_ReadCnf.buf )
		{
			free ( g_MetaAP.tNVRAM_ReadCnf.buf );	//读NVRAM失败，释放分配的存储空间--read---智能机
			g_MetaAP.tNVRAM_ReadCnf.buf = NULL;
		}
		m_Result.GetWindowText(LOG_Show_Old);  
		m_Result.SetWindowText("读取智能机的NVRAM失败！！！\r\n\r\n"+LOG_Show_Old);
		m_Result.UpdateWindow();
		goto FREEM;
    	return g_MetaAP.eMeta_Result;
    }
	else
	{
		m_Result.GetWindowText(LOG_Show_Old);  
		m_Result.SetWindowText("读取智能机的NVRAM成功，继续...\r\n\r\n"+LOG_Show_Old);
		m_Result.UpdateWindow();
	}

	////////////////////////////////////////////////////////////////////////
	
	if (strcmp(pLID, "AP_CFG_RDEB_FILE_WIFI_LID") == 0)
	{        
		unsigned int  offset = 0;
		unsigned long u4Value = 0;

		//pFuncName = "SP_META_WiFi_readMCR32_r()";
		g_MetaAP.eMeta_Result = SP_META_WiFi_readMCR32_r(g_Meta.iMetaAP_handle, 20000, offset, &u4Value);
		if(g_MetaAP.eMeta_Result == META_SUCCESS && u4Value != 0)
		{		    
			//pFuncName = "SNBase::ConductWifiAddrData()";
			wifiChipVersion = u4Value & 0xFFFF;//Start Conduct wifi nvram data...
			if(wifiChipVersion == MT5921)
			{
				//Wifi address offset is 0x26
				rs = ConductWifiAddrData((char*)g_MetaAP.tNVRAM_ReadCnf.buf+0x26, iRID, pInData, iBufLen,true);
				for(int i = 0; i < iBufLen; i++)
				{
					pWriteData[i] = g_MetaAP.tNVRAM_ReadCnf.buf[0x26+i];
				}
			}
			else if(wifiChipVersion == MT5931 || 
				  wifiChipVersion == MT6620 || 
				  wifiChipVersion == MT6628 ||
				  wifiChipVersion == MT6571 ||
				  wifiChipVersion == MT6572 || 
				  wifiChipVersion == MT6582 || 			      
				  wifiChipVersion == MT6592 ||
				  wifiChipVersion == MT6595 ||
				  wifiChipVersion == MT8127)
			{
				//Wifi address offset is 0x4
				rs = ConductWifiAddrData((char*)g_MetaAP.tNVRAM_ReadCnf.buf+0x4, iRID, pInData, iBufLen,true);
				for(int i = 0; i < iBufLen; i++)
				{
					pWriteData[i] = g_MetaAP.tNVRAM_ReadCnf.buf[0x4+i];
				}
			}
			else
			{
				goto FREEM;
				return META_FAILED;
			}			
		}
		else
		{
			goto FREEM;
			return g_MetaAP.eMeta_Result;
		}	
	}
	else if(strcmp(pLID, "AP_CFG_RDEB_FILE_BT_ADDR_LID") == 0)
	{
		rs = ConductBTAddrData((char*)g_MetaAP.tNVRAM_ReadCnf.buf, iRID, pInData, iBufLen,true);//Start Conduct BT nvram data
		for(int i = 0; i < iBufLen; i++)
		{
			pWriteData[i] = g_MetaAP.tNVRAM_ReadCnf.buf[i];
		}
	}
	else if(strcmp(pLID, "AP_CFG_RDEB_FILE_ETHERNET_LID") == 0)
	{
		rs = ConductWifiAddrData((char*)g_MetaAP.tNVRAM_ReadCnf.buf, iRID, pInData, iBufLen,true);//Start Conduct Ethernet Mac nvram data
		for(int i = 0; i < iBufLen; i++)
		{
			pWriteData[i] = g_MetaAP.tNVRAM_ReadCnf.buf[i];
		}
	}
	else if (strcmp(pLID, "AP_CFG_RDCL_FILE_AUDIO_SPEAKER_MONITOR_LID") == 0)
	{
		/*
		FT_NVRAM_READ_CNF	g_MetaAP.tNVRAM_ReadCnf;
		rs = 0;
		for(int i = 0; i < iBufLen; i++)
		{
			pWriteData[i] = pInData[i];
		}
		//Start Conduct Audio temperature data
		AUDIO_SPREAKER_MONITOR_PARAM_STRUCT sWriteTempStruct;
		memset(&sWriteTempStruct, 0, sizeof(AUDIO_SPREAKER_MONITOR_PARAM_STRUCT));

		memcpy(&sWriteTempStruct, g_MetaAP.tNVRAM_ReadCnf.buf, iWriteBufSize);
		sWriteTempStruct.temp_initial = atof(pInData);
		memcpy(g_MetaAP.tNVRAM_ReadCnf.buf,  &sWriteTempStruct,  iWriteBufSize);
	    */
	}
	 
	g_MetaAP.eMeta_Result = (META_RESULT)rs;
	if (g_MetaAP.eMeta_Result !=  META_SUCCESS)
	{
		goto FREEM;
		return g_MetaAP.eMeta_Result;
	}
	
	g_MetaAP.tNVRAM_WriteReq.LID = pLID;
	g_MetaAP.tNVRAM_WriteReq.RID = iRID;
	g_MetaAP.tNVRAM_WriteReq.len = iWriteBufSize;
	g_MetaAP.tNVRAM_WriteReq.buf = g_MetaAP.tNVRAM_ReadCnf.buf;

	m_Result.GetWindowText(LOG_Show_Old);  
	m_Result.SetWindowText("开始写入智能机的NVRAM...\r\n\r\n"+LOG_Show_Old);
	m_Result.UpdateWindow();
	
	g_MetaAP.eMeta_Result =  REQ_Write2NVRAM();			//Start to write nvram data...
	
	if ( g_MetaAP.eMeta_Result != META_SUCCESS )
    {
		m_Result.GetWindowText(LOG_Show_Old);  
		m_Result.SetWindowText("写入智能机的NVRAM失败！！！\r\n\r\n"+LOG_Show_Old);
		m_Result.UpdateWindow();
		goto FREEM;
		return g_MetaAP.eMeta_Result;
    }
	else
	{
		m_Result.GetWindowText(LOG_Show_Old);  
		m_Result.SetWindowText("写入智能机的NVRAM成功，继续...\r\n\r\n"+LOG_Show_Old);
		m_Result.UpdateWindow();
	}
	//Write nvram data successfully!");

	//写成功后，再次读取出来验证
	//Read nvram data for check start...");
	memset(g_MetaAP.tNVRAM_ReadCnf.buf, 0 , g_MetaAP.tNVRAM_ReadCnf.len);
	g_MetaAP.eMeta_Result = REQ_ReadFromNVRAM_AP();
	if (g_MetaAP.eMeta_Result != META_SUCCESS )
	{
		goto FREEM;
		return g_MetaAP.eMeta_Result;
	}
	else
	{
		char *pDataType = NULL;
		memset(pReadData, 0, sizeof(char)*MAX_DATA_LENGTH);
		if (strcmp(pLID, "AP_CFG_RDEB_FILE_WIFI_LID") == 0)
		{
			pDataType = "Wifi";
			if(wifiChipVersion == MT5921)
			{
				//Wifi address offset is 0x26
				for(int i = 0; i < iBufLen; i++)
				{
					pReadData[i] = g_MetaAP.tNVRAM_ReadCnf.buf[0x26+i];
				}
			}
			else if(wifiChipVersion == MT5931 || 
				  wifiChipVersion == MT6620 || 
				  wifiChipVersion == MT6628 ||
				  wifiChipVersion == MT6571 ||
				  wifiChipVersion == MT6572 || 
				  wifiChipVersion == MT6582 || 			      
				  wifiChipVersion == MT6592 ||
				  wifiChipVersion == MT6595 ||
				  wifiChipVersion == MT8127)
			{
				//Wifi address offset is 0x4
				for(int i = 0; i < iBufLen; i++)
				{
					pReadData[i] = g_MetaAP.tNVRAM_ReadCnf.buf[0x4+i];
				}
			}
			else
			{
				//Unsupport wifi chip version, %d!", wifiChipVersion);
				goto FREEM;
				return META_FAILED;
			}
		}
		else if(strcmp(pLID, "AP_CFG_RDEB_FILE_BT_ADDR_LID") == 0)
		{
			pDataType = "BT";
			for(int i = 0; i < iBufLen; i++)
			{
				pReadData[i] = g_MetaAP.tNVRAM_ReadCnf.buf[i];
			}
		}
		else if(strcmp(pLID, "AP_CFG_RDEB_FILE_ETHERNET_LID") == 0)
		{
			pDataType = "Ethernet_Mac";
			for(int i = 0; i < iBufLen; i++)
			{
				pReadData[i] = g_MetaAP.tNVRAM_ReadCnf.buf[i];
			}
		}
		else if(strcmp(pLID, "AP_CFG_RDCL_FILE_AUDIO_SPEAKER_MONITOR_LID") == 0)
		{
			/*
			pDataType = "Audio_Temperature";
	        
			AUDIO_SPREAKER_MONITOR_PARAM_STRUCT sWriteTempStruct;
			memset(&sWriteTempStruct, 0, sizeof(AUDIO_SPREAKER_MONITOR_PARAM_STRUCT));
	        
			memcpy(&sWriteTempStruct, g_MetaAP.tNVRAM_ReadCnf.buf, iWriteBufSize);
			float fTemp = sWriteTempStruct.temp_initial;            
			sprintf(pReadData, "%.1f", fTemp);
			*/
		}

		char tmpReadData[13] = {0};		
		if(dataType != WRITE_TEMPERATURE)//不是温度
		{
			//m_bWriteNvram = false;
			ConductWifiAddrData(tmpReadData, iRID, pReadData, iBufLen,false);//从NVRAM中读取蓝牙、wifi地址
		}
		else
		{
			strncpy(tmpReadData, pReadData, strlen(pReadData));
		}
		
		char* strLog;
		CString strLog_CS;
		strLog=new char[128];
		strLog_CS="";
		//sprintf ( strLog, "   OK: Write BT Address successfully.\n"  );//sprintf ( strLog, "   Step: Start to check BT Address.\n"  );
		sprintf ( strLog, "   从智能机读出的. BT Address = \"%x %x %x %x %x %x\".\n",  pReadData[0],
																										 pReadData[1],
																										 pReadData[2],
																										 pReadData[3],
																										 pReadData[4],
																										 pReadData[5]);
		strLog_CS.Format("%s",strLog);

		if(strncmp(pWriteData, pReadData, iBufLen) != 0)
		{			
			m_Result.GetWindowText(LOG_Show_Old);  
			m_Result.SetWindowText(strLog_CS+"\r\n写入的与读出的蓝牙地址不一致，请重新写入！！！\r\n\r\n"+LOG_Show_Old);
			m_Result.UpdateWindow();
			goto FREEM;
		}
		else
		{
			m_Result.GetWindowText(LOG_Show_Old);  
			m_Result.SetWindowText(strLog_CS+"\r\n写入的与读出的蓝牙地址一致，写BT地址确认OK！！！\r\n\r\n"+LOG_Show_Old);
			m_Result.UpdateWindow();
		}
		/////////////////////////////
	}
FREEM:
	if (g_MetaAP.tNVRAM_ReadCnf.buf != NULL)
	{
		free (g_MetaAP.tNVRAM_ReadCnf.buf);
		g_MetaAP.tNVRAM_ReadCnf.buf = NULL;
	}

	return g_MetaAP.eMeta_Result;
}

META_RESULT CWriteIMEIDlg::ConductBarcodeData(char *pOutData, unsigned short RID_para, char *pInDatabuf, int bufSize)
{    
    int dataLength = strlen(pInDatabuf);
    char calFlag[2] = {0};   
    calFlag[0] = pOutData[60];  //Backup calibration & FinalTest flag temporary
    calFlag[1] = pOutData[61];
    char ftFlag = pOutData[62];
    
    memset(pOutData, 0x20, 64);
    for (int i = 0; i < dataLength; i++)
    {
        pOutData[i] = pInDatabuf[i];
    }
    
    pOutData[60] = calFlag[0]; //Restore calibration & FinalTest flag
    pOutData[61] = calFlag[1];
    pOutData[62] = ftFlag;
    pOutData[63] = '\0';

    //Sync to AP Prod_Info
    //strncpy(m_sScanData.strBarcode, pOutData, 64);
    
    return META_SUCCESS;
}
META_RESULT CWriteIMEIDlg::ConductBTAddrData(char *pOutData, unsigned short RID_para, char *pInDatabuf, int bufSize,bool m_bWriteNvram)
{
    if (!pInDatabuf || !pOutData)
    {
        return META_INVALID_ARGUMENTS;
    }
        
    /* 
    *** Feature phone ***
    *  UI input = "1234567890AC" storage in Modem nvram will be:
    * btAddr[0] = 0xAC
    * btAddr[1] = 0x90
    * btAddr[2] = 0x78
    * btAddr[3] = 0x56
    * btAddr[4] = 0x34
    * btAddr[5] = 0x12
    ---------------------------------------------------------
    *** Smart Phone ***
    * UI input = "1234567890AC" storage in AP nvram will be:
    * btAddr[0] = 0x12
    * btAddr[1] = 0x34
    * btAddr[2] = 0x56
    * btAddr[3] = 0x78
    * btAddr[4] = 0x90
    * btAddr[5] = 0xAC
    */

       
    int tmpCh;
    if (0)//(g_sMetaComm.eTargetType == FEATURE_PHONE)功能机
    {
        if (m_bWriteNvram == false)
        {
            for (int i = 0; i < bufSize; i++)
            {
                tmpCh = (pInDatabuf[i] & 15);  //15 -->0000 1111, get Low 4Bits
                if (0 <= tmpCh && tmpCh <= 9)  //0 - 9
                {
                    pOutData[(5-i)*2 + 1] = tmpCh + '0';
                }
                else if (10 <= tmpCh && tmpCh <= 15) //a - f
                {
                    pOutData[(5-i)*2 + 1] = (tmpCh - 10) + 'A';
                }
                
                tmpCh = ((pInDatabuf[i] & 240) >> 4); //240 -->1111 0000, get High 4Bits
                if (0 <= tmpCh && tmpCh <= 9)  //0 - 9
                {
                    pOutData[(5-i)*2] = tmpCh + '0';
                }
                else if (10 <= tmpCh && tmpCh <= 15) //a - f
                {
                    pOutData[(5-i)*2] = (tmpCh - 10) + 'A';
                }           
            }
        } 
        else if(m_bWriteNvram == true)
        {
            strupr(pInDatabuf);   //转换字符串中的小写字母为大写
            for (int i = 0; i < bufSize; i++)
            {
                if ('0'<= pInDatabuf[((5-i)*2)] && pInDatabuf[((5-i)*2)] <= '9')
                {
                    tmpCh = pInDatabuf[((5-i)*2)] - '0';
                    tmpCh = ((tmpCh << 4) & 240);  //240 -->1111 0000, get High 4bits
                }
                else if('A'<= pInDatabuf[((5-i)*2)] && pInDatabuf[((5-i)*2)] <= 'F')
                {
                    tmpCh = pInDatabuf[((5-i)*2)] - 'A' + 10;
                    tmpCh = ((tmpCh << 4) & 240);  //240 -->1111 0000, get High 4bits
                }
                
                if ('0'<= pInDatabuf[((5-i)*2 + 1)] && pInDatabuf[((5-i)*2 + 1)] <= '9')
                {
                    tmpCh += pInDatabuf[((5-i)*2 + 1)] - '0'; //get Low 4bits
                } 
                else if('A'<= pInDatabuf[((5-i)*2 + 1)] && pInDatabuf[((5-i)*2 + 1)] <= 'F')
                {
                    tmpCh += pInDatabuf[((5-i)*2 + 1)] - 'A' + 10; //get Low 4bits
                }
                
                pOutData[i] = tmpCh;
            }
        }     
    }//end if (g_sMetaComm.eTargetType == FEATURE_PHONE)
    else
    {
        if (m_bWriteNvram == false)
        {
            for (int i = 0; i < bufSize; i++)
            {
                tmpCh = (pInDatabuf[i] & 15);  //15 -->0000 1111, get Low 4Bits
                if (0 <= tmpCh && tmpCh <= 9)  //0 - 9
                {
                    pOutData[(i*2 + 1)] = tmpCh + '0';
                }
                else if (10 <= tmpCh && tmpCh <= 15) //A - F
                {
                    pOutData[(i*2 + 1)] = (tmpCh - 10) + 'A';
                }
                
                //tmpCh = ((pInDatabuf[i] >> 4)  & 15);
                tmpCh = ((pInDatabuf[i] & 240) >> 4); //240 -->1111 0000, get High 4Bits
                if (0 <= tmpCh && tmpCh <= 9)  //0 - 9
                {
                    pOutData[i*2] = tmpCh + '0';
                }
                else if (10 <= tmpCh && tmpCh <= 15) //A - F
                {
                    pOutData[i*2] = (tmpCh - 10) + 'A';
                }           
            }
        } 
        else if(m_bWriteNvram == true)
        {
            strupr(pInDatabuf);   //转换字符串中的小写字母为大写
            for (int i = 0; i < bufSize; i++)
            {
                if ('0'<= pInDatabuf[i*2] && pInDatabuf[i*2] <= '9')
                {
                    tmpCh = pInDatabuf[i*2] - '0';
                    tmpCh = ((tmpCh << 4) & 240);  //240 -->1111 0000, get High 4bits
                }
                else if('A'<= pInDatabuf[i*2] && pInDatabuf[i*2] <= 'F')
                {
                    tmpCh = pInDatabuf[i*2] - 'A' + 10;
                    tmpCh = ((tmpCh << 4) & 240);  //240 -->1111 0000, get High 4bits
                }
                
                if ('0'<= pInDatabuf[(i*2 + 1)] && pInDatabuf[(i*2 + 1)] <= '9')
                {
                    tmpCh += pInDatabuf[(i*2 + 1)] - '0'; //get Low 4bits
                } 
                else if('A'<= pInDatabuf[(i*2 + 1)] && pInDatabuf[(i*2 + 1)] <= 'F')
                {
                    tmpCh += pInDatabuf[(i*2 + 1)] - 'A' + 10; //get Low 4bits
                }
                
                pOutData[i] = tmpCh;
            }
        }
    }
    
    return META_SUCCESS;
}

META_RESULT CWriteIMEIDlg::ConductWifiAddrData(char *pOutData, unsigned short RID_para, char *pInDatabuf, int bufSize,bool m_bWriteNvram)
{
    if (!pInDatabuf || !pOutData)
    {
        return META_INVALID_ARGUMENTS;
    }
    
    //strupr(pInDatabuf);       //转换字符串中的小写字母为大写
    //strlwr(pInDatabuf);       //转换字符串的大写字母为小写
    /*
    * UI input = "1234567890AC" storage in AP nvram will be:
    * wifiAddr[0] = 0x12
    * wifiAddr[1] = 0x34
    * wifiAddr[2] = 0x56
    * wifiAddr[3] = 0x78
    * wifiAddr[4] = 0x90
    * wifiAddr[5] = 0xAC
    */
    
    int tmpCh;
    if (m_bWriteNvram == false)
    {
        for (int i = 0; i < bufSize; i++)
        {
            tmpCh = (pInDatabuf[i] & 15);  //15 -->0000 1111, get Low 4Bits
            if (0 <= tmpCh && tmpCh <= 9)  //0 - 9
            {
                pOutData[(i*2 + 1)] = tmpCh + '0';
            }
            else if (10 <= tmpCh && tmpCh <= 15) //a - f
            {
                pOutData[(i*2 + 1)] = (tmpCh - 10) + 'A';
            }
            
            //tmpCh = ((pInDatabuf[i] >> 4)  & 15);
            tmpCh = ((pInDatabuf[i] & 240) >> 4); //240 -->1111 0000, get High 4Bits
            if (0 <= tmpCh && tmpCh <= 9)  //0 - 9
            {
                pOutData[i*2] = tmpCh + '0';
            }
            else if (10 <= tmpCh && tmpCh <= 15) //A - F
            {
                pOutData[i*2] = (tmpCh - 10) + 'A';
            }           
        }
    } 
    else if(m_bWriteNvram == true)
    {
        strupr(pInDatabuf);       //转换字符串中的小写字母为大写
        for (int i = 0; i < bufSize; i++)
        {
            if ('0'<= pInDatabuf[i*2] && pInDatabuf[i*2] <= '9')
            {
                tmpCh = pInDatabuf[i*2] - '0';
                tmpCh = ((tmpCh << 4) & 240);  //240 -->1111 0000, get High 4bits
            }
            else if('A'<= pInDatabuf[i*2] && pInDatabuf[i*2] <= 'F')
            {
                tmpCh = pInDatabuf[i*2] - 'A' + 10;
                tmpCh = ((tmpCh << 4) & 240);  //240 -->1111 0000, get High 4bits
            }
            
            if ('0'<= pInDatabuf[(i*2 + 1)] && pInDatabuf[(i*2 + 1)] <= '9')
            {
                tmpCh += pInDatabuf[(i*2 + 1)] - '0'; //get Low 4bits
            } 
            else if('A'<= pInDatabuf[(i*2 + 1)] && pInDatabuf[(i*2 + 1)] <= 'F')
            {
                tmpCh += pInDatabuf[(i*2 + 1)] - 'A' + 10; //get Low 4bits
            }
            
            pOutData[i] = tmpCh;
        }
    }
    
    return META_SUCCESS;
} 
void CWriteIMEIDlg::OnBnClickedButtonstart13()
{
	IMEIWrite_MulAT dig1;

	ShowWindow(FALSE);
	dig1.DoModal();//.DoModal();
	ShowWindow(TRUE);

	//三合一实现自动登陆,测试窗口被关闭后，这个窗口也要自动关闭
	if (g_ADCTFlag == 1)
	{
		this->DestroyWindow();
	}

}

void CWriteIMEIDlg::OnBnClickedButton24()
{
	// TODO: 在此添加控件通知处理程序代码
	TagsBatchPrint dig1;
	ShowWindow(FALSE);
	dig1.DoModal();//.DoModal();
	ShowWindow(TRUE);
}

void CWriteIMEIDlg::OnBnClickedButton33()
{
	// TODO: 在此添加控件通知处理程序代码
	CPackingPrint dig1;
	ShowWindow(FALSE);
	dig1.DoModal();//.DoModal();
	ShowWindow(TRUE);
}

BOOL CWriteIMEIDlg::Permission()
{
	//CSSLClient SSLConfirm;
	//CString HostName(SSLConfirm.GetHostName());
	CString HostName = "";
	//CString IPAddr = "192.168.0.243";
	CString IPAddr = g_BackDoorSocket;
	//CString IPAddr = "192.168.0.147";
	//CString IPAddr = "192.168.38.138";
	//CString IPAddr = "192.168.1.101";
	int Port = 41000;

	CEventWrapper ShutDownEvent;

	CActiveSock * pActiveSock = new CActiveSock(ShutDownEvent);
	
	pActiveSock->SetRecvTimeoutSeconds(30);
	pActiveSock->SetSendTimeoutSeconds(30);
	bool b = pActiveSock->Connect(HostName, Port, IPAddr);
	if (b)
	{
		char Msg[100];
		//if (pActiveSock->SendPartial("Licence#", 8) == 8)
		CString SendMsg = "Licence#";
		if (pActiveSock->SendPartial(SendMsg, SendMsg.GetLength()) == SendMsg.GetLength())
		{
			int len = 0;
			CString MsgCS;
			while (0 < (len = pActiveSock->RecvPartial(Msg, sizeof(Msg))))
			{
				Msg[len] = '\0';
				MsgCS.Format("%s", Msg);
				if (MsgCS.Find("License ISOK") != -1)
				{
					::SetEvent(ShutDownEvent);
					if (0)//测试
					{
						CString TestData = "Hello From LBC";
						pActiveSock->SendPartial(TestData, TestData.GetLength());

						Sleep(1000);
						Msg[pActiveSock->RecvPartial(Msg, sizeof(Msg))]='\0';
						MsgCS.Format("%s", Msg);
						AfxMessageBox(MsgCS);
					}
					pActiveSock->Close();
					return TRUE;
				}
				else
				{
					AfxMessageBox("0");
				}
			}
		}
		else
		{
			AfxMessageBox("1");
		}
		::SetEvent(ShutDownEvent);
		pActiveSock->Close();
	}
	else
	{
		if (g_ADCTFlag == 0)
		{
			AfxMessageBox("2");
		}

	}
	return FALSE;
}