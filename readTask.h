#ifndef READTASK_H
#define READTASK_H

#include <windows.h>
#include <iostream>
#include <stdio.h>
#include <comdef.h>
#include <combaseapi.h>
#include <taskschd.h>
#include <map>
using namespace std;

BOOL InitialiseCOM() {
    HRESULT hResult;

    hResult = CoInitialize(NULL);
    if (!SUCCEEDED(hResult)) {
        return FALSE;
    }

    hResult = CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_PKT_PRIVACY, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, 0, NULL);
    if (!SUCCEEDED(hResult)) {
        CoUninitialize();
        return FALSE;
    }

    return TRUE;
}

BOOL createConnectToTask(ITaskService*& pTaskService,ITaskFolder*& pRootFolder) {
    HRESULT hResult = ::CoCreateInstance(CLSID_TaskScheduler, NULL, CLSCTX_INPROC_SERVER, IID_ITaskService, (void**)&pTaskService);
    if (!SUCCEEDED(hResult)) {
       CoUninitialize();
       return FALSE;

    }
    VARIANT username;
    VARIANT password;
    VARIANT server;
    VARIANT domain;
    ::VariantInit(&username);
    ::VariantInit(&password);
    ::VariantInit(&server);
    ::VariantInit(&domain);

    hResult = pTaskService->Connect(server, username, domain, password);
    if (!SUCCEEDED(hResult)) {
        pTaskService->Release();
        CoUninitialize();
        return FALSE;
    }
    ::VariantClear(&username);
    ::VariantClear(&password);
    ::VariantClear(&server);
    ::VariantClear(&domain);
    hResult = pTaskService->GetFolder(_bstr_t(L"\\"), &pRootFolder);
    if (FAILED(hResult)) {
        return FALSE;
    }
    return TRUE;
}


BOOL readAllTask(ITaskFolder* pRootFolder, HRESULT hr, BSTR allfolderName, map<string,string>* taskMap) {
    BSTR currentFolderName = NULL;
    pRootFolder->get_Name(&currentFolderName);
    IRegisteredTaskCollection* pTaskCollection = NULL;
    hr = pRootFolder->GetTasks(NULL, &pTaskCollection);
    if (SUCCEEDED(hr)) {
        LONG numTask = 0;
        pTaskCollection->get_Count(&numTask);
        for (LONG i = 0;i<numTask; i++) {
            IRegisteredTask* pTask = NULL;
            hr = pTaskCollection->get_Item(_variant_t(i + 1), &pTask);
            if(SUCCEEDED(hr)) {
                BSTR taskName = NULL;
                hr = pTask->get_Name(&taskName);
                if (SUCCEEDED(hr)) {
                    ITaskDefinition* taskDefination = NULL;
                    IActionCollection* actionCollection = NULL;
                    IAction* action = NULL;
                    IExecAction* execAction = NULL;
                    BSTR bstrTaskimg = NULL;
                    BSTR mImagePath = NULL;
                    TASK_STATE taskState;
                    hr = pTask->get_State(&taskState);
                    if (FAILED(hr)) {
                        printf("\n\tCannot get the registered task state: %x", hr);
                        ;
                    }
                    hr = pTask->get_Definition(&taskDefination);
                    if (FAILED(hr))
                    {
                        //return FALSE;
                        pTask->Release();
                        continue;
                    }
                    hr = taskDefination->get_Actions(&actionCollection);
                    if (FAILED(hr))
                    {
                        pTask->Release();
                        continue;
                    }
                    taskDefination->Release();


                    hr = actionCollection->get_Item(1, &action);
                    if (FAILED(hr)) {
                        pTask->Release();
                        continue;
                    }
                    actionCollection->Release();

                    hr = action->QueryInterface(IID_IExecAction, (void**)&execAction);
                    if (FAILED(hr)) {
                        pTask->Release();

                    }
                    action->Release();
                    hr = execAction->get_Path(&mImagePath);
                    if (SUCCEEDED(hr))
                    {
                        string m_floder_name = _com_util::ConvertBSTRToString(allfolderName);
                        string m_task_name = _com_util::ConvertBSTRToString(taskName);
                        string imagePath = _com_util::ConvertBSTRToString(mImagePath);
                        string m_left_key = m_floder_name+m_task_name;
                        taskMap->insert(make_pair(m_left_key,imagePath));
                        SysFreeString(taskName);
                        execAction->Release();
                    }
                }
                else {
                    continue;
                }
                pTask->Release();
            }
        }
        pTaskCollection->Release();
    }

    ITaskFolderCollection* pSubFolders = NULL;
    hr = pRootFolder->GetFolders(0, &pSubFolders);

    if (FAILED(hr)) {
        return FALSE;
    }
    LONG numberFolder = 0;
    hr = pSubFolders->get_Count(&numberFolder);
    for (LONG i = 0; i < numberFolder; i++) {
        ITaskFolder* pNewTaskFolder = NULL;
        hr = pSubFolders->get_Item(_variant_t(i + 1), &pNewTaskFolder);
        if (FAILED(hr)) {
            continue;
        }
        BSTR nameFolder = NULL;
        hr = pNewTaskFolder->get_Name(&nameFolder);
        if (FAILED(hr)) {
            continue;
        }
        string m_name_folder = _com_util::ConvertBSTRToString(nameFolder);
        string m_root_name = _com_util::ConvertBSTRToString(allfolderName);
        string m_new_folder = m_name_folder + "\\" + m_root_name;
        BSTR new_folder = _com_util::ConvertStringToBSTR(m_new_folder.c_str());
        readAllTask(pNewTaskFolder, hr, new_folder, taskMap);
        SysFreeString(nameFolder);
        pNewTaskFolder->Release();
    }
    pSubFolders->Release();

    return TRUE;

}

















#endif // READTASK_H
