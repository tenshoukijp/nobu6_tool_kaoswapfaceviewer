
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



// ���傤�� vBufBushouDecodedData[0]=�P�Ԗڂ̕����t�@�C���̃f�[�^��AvBufBushouDecodedData[1]=�Q�Ԗڂ̕����t�@�C���̃f�[�^��A�݂����Ȋ���
vector<vector<byte>> vBufBushouDecodedData;


ref class BushouNameList_Importer {
	char *szBushouTargetFileName = "bfile.n6p";
private:
	List<String^>^ lstStrFullName; // ����

public:
	BushouNameList_Importer() {
		lstStrFullName = gcnew List<String^>();
		BushouNameList_Import();
	}

	List<String^>^ GetBushouFullNameList() {
		return lstStrFullName;
	}

	// bfile.n6p���z��ւƊi�[�B
	void BushouNameList_Import() {

		if (!System::IO::File::Exists(gcnew String(szBushouTargetFileName))) {
			System::Windows::Forms::MessageBox::Show(gcnew String(szBushouTargetFileName) + "��ǂݍ��߂܂���", "�G���[", MessageBoxButtons::OK, ::MessageBoxIcon::Error);
			return;
		}

		// �O�̂��߃N���A
		vBufBushouDecodedData.clear();

		// ���k���ꂽLS11�t�H�[�}�b�g���f�R�[�h����֐�
		ls11_DecodePack(szBushouTargetFileName, NULL, &vBufBushouDecodedData);

		// �f�R�[�h�����f�[�^��(�����I)�t�@�C���ł܂킵�Ă����B0�Ԃ͑Ώۂł͂Ȃ��B�����f�[�^��1�Ԃ���B
		int ivBufDecodedDataSize = vBufBushouDecodedData.size();

		// 1�Ԗڂ���X�^�[�g���Ă���!! ����!!
		for (int ifile = 1; ifile < (int)ivBufDecodedDataSize; ifile++) {

			// ���傤�� vSplittedData[0]=�P�Ԗڂ̗v�f�̃f�[�^��AvSplittedData[1]=�Q�Ԗڂ̗v�f�̃f�[�^��A�݂����Ȋ���
			vector<vector<byte>> vSplittedData;

			ls11_SplitData(vBufBushouDecodedData[ifile], &vSplittedData);

			int ivSplittedDataSize = vSplittedData.size();

			// �e�v�f���Ƃɥ�� �܂����O�A�ǂ݉��� ���v�N�̗�
			for (int ielem = 0; ielem < (int)ivSplittedDataSize; ielem++) {

				// �������ǂ݉����Ƃ�
				if (ielem % 2 == 0) {

					int datasize = vSplittedData[ielem].size();
					if (datasize > 0) {

						char *bytedata = new char[datasize + 1];
						memcpy(bytedata, &vSplittedData[ielem][0], datasize);
						bytedata[datasize] = NULL; // �ԕ�

						String^ mngdata = gcnew String(bytedata);
						mngdata = mngdata->Trim();
						int index = mngdata->IndexOf(" ");
						if (index > 0) {
							mngdata = mngdata->Substring(0, index);
						}
						lstStrFullName->Add(mngdata); // �}�l�[�W�h�f�[�^�ւƑ��

						delete bytedata;
					}


					// ��͗�`�B
				}
				else {


				}
			}
		}
	}

};