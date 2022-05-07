
#pragma once

#include <vector>
#include <string>

#include "ls11_mod.h"


using namespace System;
using namespace System::Text;
using namespace System::Drawing;
using namespace System::Collections::Generic;
using namespace System::Windows::Forms;
using namespace System::Text::RegularExpressions;



// ちょうど vBufBushouDecodedData[0]=１番目の分割ファイルのデータ列、vBufBushouDecodedData[1]=２番目の分割ファイルのデータ列、みたいな感じ
vector<vector<byte>> vBufBushouDecodedData;


ref class BushouNameList_Importer {
	char *szBushouTargetFileName = "bfile.n6p";
private:
	List<String^>^ lstStrFullName; // 姓名

public:
	BushouNameList_Importer() {
		lstStrFullName = gcnew List<String^>();
		BushouNameList_Import();
	}

	List<String^>^ GetBushouFullNameList() {
		return lstStrFullName;
	}

	// bfile.n6p→配列へと格納。
	void BushouNameList_Import() {

		if (!System::IO::File::Exists(gcnew String(szBushouTargetFileName))) {
			System::Windows::Forms::MessageBox::Show(gcnew String(szBushouTargetFileName) + "を読み込めません", "エラー", MessageBoxButtons::OK, ::MessageBoxIcon::Error);
			return;
		}

		// 念のためクリア
		vBufBushouDecodedData.clear();

		// 圧縮されたLS11フォーマットをデコードする関数
		ls11_DecodePack(szBushouTargetFileName, NULL, &vBufBushouDecodedData);

		// デコードしたデータの(内部的)ファイルでまわしていく。0番は対象ではない。武将データは1番から。
		int ivBufDecodedDataSize = vBufBushouDecodedData.size();

		// 1番目からスタートしている!! 注意!!
		for (int ifile = 1; ifile < (int)ivBufDecodedDataSize; ifile++) {

			// ちょうど vSplittedData[0]=１番目の要素のデータ列、vSplittedData[1]=２番目の要素のデータ列、みたいな感じ
			vector<vector<byte>> vSplittedData;

			ls11_SplitData(vBufBushouDecodedData[ifile], &vSplittedData);

			int ivSplittedDataSize = vSplittedData.size();

			// 各要素ごとに･･･ まず名前、読み仮名 生没年の類
			for (int ielem = 0; ielem < (int)ivSplittedDataSize; ielem++) {

				// 偶数が読み仮名とか
				if (ielem % 2 == 0) {

					int datasize = vSplittedData[ielem].size();
					if (datasize > 0) {

						char *bytedata = new char[datasize + 1];
						memcpy(bytedata, &vSplittedData[ielem][0], datasize);
						bytedata[datasize] = NULL; // 番兵

						String^ mngdata = gcnew String(bytedata);
						mngdata = mngdata->Trim();
						int index = mngdata->IndexOf(" ");
						if (index > 0) {
							mngdata = mngdata->Substring(0, index);
						}
						lstStrFullName->Add(mngdata); // マネージドデータへと代入

						delete bytedata;
					}


					// 奇数は列伝。
				}
				else {


				}
			}
		}
	}

};