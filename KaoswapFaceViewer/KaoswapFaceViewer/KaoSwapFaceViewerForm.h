#pragma once;

#include "KaoDataStruct.h"
#include "BushouRetsudenImporter.h"

using namespace std;
using namespace System;
using namespace System::Drawing;
using namespace System::Windows::Forms;
using namespace System::Collections::Generic;
using namespace System::Diagnostics;
using namespace System::IO;

extern bool ImportKaoDataFileToProgramStruct(String^ strInputFileName);
extern Image^ GetBushouFaceImage(int iBushouID);



ref class KaoswapFaceViewer : public Form {
public:
	static int BUSHOU_NUM = 0;

private:
	static List<PictureBox^>^ pbList;
	static List<Label^>^ lbList;
	static int iBushouStartPosition = 0;

	FileSystemWatcher^ watcher;

	static String^ strTargetFile;
	static String^ outDirName = "kao_3bit";
	static String^ importer = "KaoswapFaceImporter.exe";
	static BushouNameList_Importer^ bsNameListImpoter;
	static List<String^>^ strBushouFullNameList;

	static String^ strFontName = "MS Gothic";
public:
	KaoswapFaceViewer(String^ strTargetFileName) {
		this->KeyPreview = true; // キーイベントについては、フォームで全て受け取る

		iBushouStartPosition = 0;

		// もしターゲットが指定されているならば…、それにする
		strTargetFile = strTargetFileName;

		this->Activated += gcnew EventHandler(this, &KaoswapFaceViewer::form_Activated);
		this->Shown += gcnew EventHandler(this, &KaoswapFaceViewer::form_Shown);

		SetFormAttribute();
	}
private:
	void form_Activated(Object^ o, EventArgs^ e) {
		ImportKaoDataFileToProgramStruct(strTargetFile);
		InitBushouNameList();
	}

	void form_Shown(Object^ o, EventArgs^ e) {
		InitPictureBoxList();
		UpdatePictureBoxList(0);
		InitFileWatcher();
	}

	void SetFormAttribute() {
		this->StartPosition = ::FormStartPosition::Manual;
		this->Width = 830;
		this->Height = 1022;
		this->MaximizeBox = false;

		this->KeyDown += gcnew KeyEventHandler(this, &KaoswapFaceViewer::form_KeyDown);
	}

	void InitBushouNameList() {
		// 武将名前の一覧が読めそうなら読む
		bsNameListImpoter = gcnew BushouNameList_Importer();
		strBushouFullNameList = bsNameListImpoter->GetBushouFullNameList();
	}

	void InitPictureBoxList() {

		this->SuspendLayout();

		pbList = gcnew List<PictureBox^>();
		lbList = gcnew List<Label^>();

		int iBushouID = 0;
		for (int y = 0; y < 10; y++) {
			for (int x = 0; x < 10; x++) {
				PictureBox^ pb = gcnew PictureBox();
				pb->Width = FaceWidth;
				pb->Height = FaceHeight;
				pb->Left = x * 80 + 10;
				pb->Top = y * 98 + 2;
				pb->Image = nullptr;
				pb->Tag = iBushouID;
				pbList->Add(pb);
				this->Controls->Add(pb);

				const int offset = 5;
				Label^ lb = gcnew Label();
				lb->Parent = pb;
				lb->Width = FaceWidth + offset*2; // 画像より10ピクセル拾い
				lb->Height = 20;
				lb->Left = pb->Left - offset; // 画像より５ピクセル左へ
				lb->Top = pb->Bottom-1;
				lb->Text = "";
				lb->TextAlign = ContentAlignment::TopCenter;
				lb->Font = gcnew Drawing::Font("MS Gothic", 7);
				lbList->Add(lb);
				this->Controls->Add(lb);

				iBushouID++;
			}
		}

		this->ResumeLayout();
	}


private:
	String^ GettLabelText(int iBushouID) {
		// 武将の名前一覧の範囲内のiBushouIDであるならば
		if (0 <= iBushouID && iBushouID < strBushouFullNameList->Count) {
			String^ name = strBushouFullNameList[iBushouID];
			if (name->Length > 6) {
				name = name->Substring(0, 5) + "…";
			}

			return name + "\n" + String::Format("└ {0:D4} ┘", iBushouID + 1);
		}
		else {
			return String::Format("{0:D4}", iBushouID + 1);
		}
	}

	void UpdatePictureBoxList(int iStartBushouID) {

		this->SuspendLayout();

		int iBushouID = iStartBushouID;
		for (int i = 0; i < pbList->Count; i++) {

			PictureBox^ pb = pbList[i];
			pb->Image = GetBushouFaceImage(iBushouID);

			Label^ lb = lbList[i];
			lb->Text = GettLabelText(iBushouID);

			iBushouID++;
		}

		this->ResumeLayout();
	}

	void InitFileWatcher() {
		watcher = gcnew FileSystemWatcher();
		//監視するディレクトリを指定
		watcher->Path = Application::StartupPath;
		// 特定のファイルのみの監視
		watcher->Filter = IO::Path::GetFileName(strTargetFile);

		// 最終書き込み日時の変更を監視
		watcher->NotifyFilter = NotifyFilters::LastWrite;

		//サブディレクトリは監視しない
		watcher->IncludeSubdirectories = false;

		// UIスレッドにマーシャリング
		watcher->SynchronizingObject = this;

		//イベントハンドラの追加
		watcher->Changed += gcnew FileSystemEventHandler(this, &KaoswapFaceViewer::watcher_Changed);

		watcher->EnableRaisingEvents = true;

	}

	void form_KeyDown(Object^ sender, KeyEventArgs^ e) {

		int iPrevBushouStartPositon = iBushouStartPosition;

		int iFloor = (BUSHOU_NUM / 100) * 100;

		auto code = e->KeyCode;
		// 前のデータというような意味を指すキーボードを押した
		if (code == Keys::Left || code == Keys::Up || code == Keys::PageUp) {
			iBushouStartPosition -= 100;
		}
		// 次のデータというような意味を指すキーボードを押した
		else if (code == Keys::Right || code == Keys::Down || code == Keys::PageDown) {
			iBushouStartPosition += 100;
		}	

		else if (code == Keys::Home) {
			iBushouStartPosition = 0;
		}
		else if (code == Keys::End) {
			iBushouStartPosition = iFloor; // 100 で割って余った分は切り捨て
		}

		if (iBushouStartPosition < 0) {
			iBushouStartPosition = 0;
		}
		if (iBushouStartPosition > iFloor) {
			iBushouStartPosition = iFloor;
		}

		if (code == Keys::O) {
			auto ret = MessageBox::Show("全ての顔を " + outDirName + " ディレクトリへと出力しますか？", "確認", ::MessageBoxButtons::YesNo);
			if (ret == ::DialogResult::Yes) {
				if (!Directory::Exists(outDirName)) {
					Directory::CreateDirectory(outDirName);
				}

				OutputAllFaceToBmp();
			}
		}

		else if (code == Keys::I) {
			if ( File::Exists(importer) ) {
				auto ret = MessageBox::Show("全ての顔を " + outDirName + " ディレクトリから " + strTargetFile + " へと取込しますか？", "確認", ::MessageBoxButtons::YesNo);
				if (ret == ::DialogResult::Yes) {
					Process::Start(importer);
				}
			}
		}

		if (iPrevBushouStartPositon != iBushouStartPosition) {
			UpdatePictureBoxList(iBushouStartPosition);
		}
	}

	void OutputAllFaceToBmp() {
		int error_cnt = 0;
		for (int b = 0; b < BUSHOU_NUM; b++) {
			Image^ img = GetBushouFaceImage(b);
			String^ filefullpath = String::Format(outDirName + "\\{0:D4}.bmp", b + 1); // ファイルにする時は、１つ多い数値
			try {
				img->Save(filefullpath);
			}
			catch (Exception^ e) {
				error_cnt++;
				MessageBox::Show(e->Message + "\n" + filefullpath);
			}
		}

		MessageBox::Show("完了");
	}

	void watcher_Changed(Object^ sender, FileSystemEventArgs ^ e) {
		System::Diagnostics::Trace::WriteLine(e->Name);
		// 顔データ全体を読み直す
		ImportKaoDataFileToProgramStruct(strTargetFile);
		// 表示更新
		UpdatePictureBoxList(iBushouStartPosition);
	}

};
