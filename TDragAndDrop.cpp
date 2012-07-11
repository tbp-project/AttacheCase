#include "TDragAndDrop.h"
#include <System.hpp>

//===========================================================================
//�R���X�g���N�^
//===========================================================================
__fastcall TDragAndDrop::TDragAndDrop(TForm *_Form): _RefCount(1), Form(_Form)
{

//�����t�@�C���̎󂯓���������邩
fMultipleFilesOk = true;

//�h���b�v�C���[�W�^�C�v�i�f�t�H���g���R�s�[�j
DropImageType = DROPIMAGE_COPY;


}
//===========================================================================
//�f�X�g���N�^
//===========================================================================
__fastcall TDragAndDrop::~TDragAndDrop(void)
{
//
}
//===========================================================================
HRESULT __stdcall TDragAndDrop::QueryInterface(const IID& iid, void** ppv)
{

HRESULT hr;

if(iid == IID_IDropTarget || iid == IID_IUnknown){
	hr = S_OK;
	*ppv = (void*)this;
	AddRef();
}
else{
	hr = E_NOINTERFACE;
	*ppv = 0;
}
return hr;

}
//---------------------------------------------------------------------------
ULONG __stdcall TDragAndDrop::AddRef()
{

InterlockedIncrement(&_RefCount);
return (ULONG)_RefCount;

}
//---------------------------------------------------------------------------
ULONG __stdcall TDragAndDrop::Release()
{

ULONG ret = (ULONG)InterlockedDecrement(&_RefCount);
if(ret == 0){
	delete this;
}
return (ULONG)_RefCount;

}
//---------------------------------------------------------------------------
HRESULT __stdcall TDragAndDrop::
	DragEnter(IDataObject* pDataObject, DWORD grfKeyState, POINTL ptl, DWORD* pdwEffect)
{

_DataObject = pDataObject;


// IDataObject �� HDROP �̒l���擾���Ă���
STGMEDIUM td = { TYMED_HGLOBAL };
FORMATETC fr = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };

HRESULT hr = _DataObject->GetData(&fr, &td);

if ( FAILED(hr) == true ){
	return E_FAIL;
}

HDROP hDrop = static_cast<HDROP>(GlobalLock(td.hGlobal));

if (!hDrop){
	return E_INVALIDARG;
}

//�����̃h���b�v���ꂽ�t�@�C�������擾
FileCount = DragQueryFileW(hDrop, 0xFFFFFFFF, NULL, 0);

// Windows���őO�ʂ�
SetForegroundWindow(Form->Handle);
SetCapture(Form->Handle);
Form->Cursor = crDefault;


if ( fMultipleFilesOk == true ) {
	*pdwEffect = DROPIMAGE_COPY;
}
else{
	if (FileCount == 1 ) {
		*pdwEffect = DROPIMAGE_NONE;
	}
	else{
		*pdwEffect = DROPIMAGE_COPY;
	}
}

//�f�o�b�O
*pdwEffect = DropImageType;


return DragOver(grfKeyState, ptl, pdwEffect);

}
//---------------------------------------------------------------------------
HRESULT __stdcall TDragAndDrop::DragOver(DWORD grfKeyState, POINTL ptl, DWORD* pdwEffect)
{

if ( fMultipleFilesOk == true ) {
	*pdwEffect = DROPIMAGE_COPY;
}
else{
	if (FileCount == 1 ) {
		*pdwEffect = DROPIMAGE_NONE;
	}
	else{
		*pdwEffect = DROPIMAGE_COPY;
	}
}

//�f�o�b�O
*pdwEffect = DropImageType;

FilesDragOver(ptl);	         //�R�[���o�b�N�֐�

return S_OK;

}
//---------------------------------------------------------------------------
HRESULT __stdcall TDragAndDrop::DragLeave()
{

Form->Cursor = crDefault;

FilesDragLeave();	//�R�[���o�b�N�֐�
ReleaseCapture();

_DataObject = NULL;

return S_OK;

}
//---------------------------------------------------------------------------
HRESULT __stdcall TDragAndDrop::Drop
	(IDataObject* pDataObject, DWORD grfKeyState, POINTL ptl, DWORD* pdwEffect)
{

int i;
wchar_t filename[MAX_PATH];

TStringList *FileList = new TStringList;

// IDataObject �� HDROP �̒l���擾���Ă���
STGMEDIUM td = { TYMED_HGLOBAL };
FORMATETC fr = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };

HRESULT hr = pDataObject->GetData(&fr, &td);

if ( FAILED(hr) == true ){
	return E_FAIL;
}

HDROP hDrop = static_cast<HDROP>(GlobalLock(td.hGlobal));

if (!hDrop){
	return E_INVALIDARG;
}

//�����̃h���b�v���ꂽ�t�@�C�������擾
int FileCount = DragQueryFileW(hDrop, 0xFFFFFFFF, NULL, 0);

for (i = 0; i < FileCount; i++) {
	//�t�@�C�������擾����
	DragQueryFileW(hDrop, i, filename, MAX_PATH);
	FileList->Add(filename);
}

//�R�[���o�b�N�֐�
FilesDragEnd(ptl, FileList->Text);

//��n��
GlobalUnlock(td.hGlobal);
ReleaseStgMedium(&td);
Form->Cursor = crDefault;
*pdwEffect = DROPEFFECT_NONE;
ReleaseCapture();
delete FileList;

return S_OK;

}
//---------------------------------------------------------------------------

