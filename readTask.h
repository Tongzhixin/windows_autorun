#ifndef READTASK_H
#define READTASK_H
#include <windows.h>
#include <iostream>
#include <stdio.h>
#include "convert.h"
#include <comdef.h>
#include <combaseapi.h>
#include <taskschd.h>
#include <map>
#include <ShlObj_core.h>
#include <ShObjIdl.h>
#include <io.h>
#include <vector>
#include <string>
using namespace std;
BOOL readAllTask(ITaskFolder* pRootFolder, HRESULT hr, BSTR allfolderName, map<string,string>* taskMap);
int get_files(string fileFolderPath, string fileExtension, map<string,string>& file);
void createConnectToStart(map<string, map<string,string>>& logonDir);
BOOL InitialiseCOM();
BOOL createConnectToTask(ITaskService*& pTaskService,ITaskFolder*& pRootFolder);
BOOL entryTask(map<string,string>* taskMap);
BOOL readAllTask(ITaskFolder* pRootFolder, HRESULT hr, BSTR allfolderName, map<string,string>* taskMap);
int get_files(string fileFolderPath, string fileExtension, map<string,string>& file)
{
    std::string fileFolder = fileFolderPath + "\\*" ;
    std::string fileName;
    struct _finddata_t fileInfo;
    long long findResult = _findfirst(fileFolder.c_str(), &fileInfo);
    if (findResult == -1)
    {
        _findclose(findResult);
        return 0;
    }
    bool flag = 0;

    do
    {   string m_name = fileInfo.name;
        fileName = fileFolderPath + "\\" + fileInfo.name;
        if (fileInfo.attrib == _A_ARCH)
        {
            file[m_name] = fileName;
        } else if(fileInfo.attrib == _A_SUBDIR) {
            if(strcmp(fileInfo.name,".") != 0  &&  strcmp(fileInfo.name,"..") != 0)
                get_files(fileName,"",file);
        } else  { //if(fileInfo.attrib == _A_NORMAL||fileInfo.attrib == _A_RDONLY||fileInfo.attrib == _A_HIDDEN||fileInfo.attrib == _A_SYSTEM)
            if(strcmp(fileInfo.name,".") != 0  &&  strcmp(fileInfo.name,"..") != 0){
            file[m_name] = fileName;
            }
        }
    } while (_findnext(findResult, &fileInfo) == 0);

    _findclose(findResult);
    return 1;
}

void createConnectToStart(map<string, map<string,string>>& logonDir){
    PWSTR common_path ;
    HRESULT hr,hr2;
    hr = SHGetKnownFolderPath(FOLDERID_CommonStartup,NULL,NULL,&common_path);
    PWSTR user_path ;
    hr2 = SHGetKnownFolderPath(FOLDERID_Startup,NULL,NULL,&user_path);
    if(SUCCEEDED(hr)){
        string tmp = Wchar_tToString(common_path);
        map<string,string> submap;
        get_files(tmp,"",submap);
        logonDir[tmp] = submap;
        /*
        map<string,string>::iterator It = submap.begin();
        while(It!=submap.end()) {

            cout <<It->second<<'\n';
            It++;
        }
        */

    }
    if(SUCCEEDED(hr2)){
        string tmp = Wchar_tToString(user_path);
        map<string,string> submap;
        get_files(tmp,"",submap);
        logonDir[tmp] = submap;
        /*
        map<string,string>::iterator It = submap.begin();
        while(It!=submap.end()) {
            cout <<It->second<<'\n';
            It++;
        }
        cout<<tmp<<'\n';
        */
    }
}

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
        CoUninitialize();
        return FALSE;
    }
    ::VariantClear(&username);
    ::VariantClear(&password);
    ::VariantClear(&server);
    ::VariantClear(&domain);
    hResult = pTaskService->GetFolder(_bstr_t(L"\\"), &pRootFolder);
    if (FAILED(hResult)) {
        pTaskService->Release();
        CoUninitialize();
        return FALSE;
    }
    return TRUE;
}
BOOL entryTask(map<string,string>* taskMap) {
    ITaskService* pTaskService = NULL;
    ITaskFolder* pRootFolder = NULL;

    HRESULT hr;
    BOOL flag = FALSE;
    flag = InitialiseCOM();
    if (!flag) {
        return flag;
    }
    flag = createConnectToTask(pTaskService,pRootFolder);
    if (!flag) {
        return flag;
    }
    BSTR currentFolderName = NULL;
    hr = pRootFolder->get_Name(&currentFolderName);
    if (SUCCEEDED(hr)) {
        flag = readAllTask(pRootFolder,hr,currentFolderName,taskMap);
        SysFreeString(currentFolderName);
        if (!flag) {
            pRootFolder->Release();
            pTaskService->Release();
            CoUninitialize();
            return flag;
        }

    }
    CoUninitialize();
    return 1;
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
                    BSTR pPath = NULL;
                    BSTR mImagePath = NULL;
                    hr = pTask->get_Definition(&taskDefination);
                    if (SUCCEEDED(hr)) {
                        hr = taskDefination->get_Actions(&actionCollection);
                        if(SUCCEEDED(hr)) {
                            hr = actionCollection->get_Item(1, &action);
                            if (SUCCEEDED(hr)) {
                                 hr = action->QueryInterface(IID_IExecAction, (void**)&execAction);

                                 TASK_STATE taskState;
                                 pTask->get_State(&taskState);
                                 bool disabled = (taskState == TASK_STATE_DISABLED);
                                 bool queued = (taskState == TASK_STATE_QUEUED);
                                 bool ready = (taskState == TASK_STATE_READY);
                                 bool running = (taskState == TASK_STATE_RUNNING);
                                 bool unknown = (taskState == TASK_STATE_UNKNOWN);
                                 if (ready || running || queued) {
                                     if (SUCCEEDED(hr)) {
                                         hr = execAction->get_Path(&mImagePath);
                                         if (SUCCEEDED(hr)) {
                                             string m_floder_name = _com_util::ConvertBSTRToString(allfolderName);
                                             string m_task_name = _com_util::ConvertBSTRToString(taskName);
                                             string imagePath = _com_util::ConvertBSTRToString(mImagePath);
                                             string m_left_key = m_floder_name+m_task_name;
                                             hr = pTask->get_Path(&pPath);
                                             if (SUCCEEDED(hr)) {
                                                 string m1_left_key = _com_util::ConvertBSTRToString(pPath);

                                                 taskMap->insert(make_pair(m1_left_key,imagePath));
                                             }
                                             //taskMap->insert(make_pair(m_left_key,imagePath));
                                             execAction->Release();
                                         }
                                         action->Release();
                                     }
                                 }

                                 actionCollection->Release();
                            }
                            taskDefination->Release();
                        }

                    }
                    SysFreeString(taskName);
                    pTask->Release();

                }

            }
        }
        pTaskCollection->Release();
    }
    ITaskFolderCollection* pSubFolders = NULL;
    ITaskFolder* pNewTaskFolder = NULL;
    hr = pRootFolder->GetFolders(0, &pSubFolders);

    if (SUCCEEDED(hr)) {
        LONG numberFolder = 0;
        hr = pSubFolders->get_Count(&numberFolder);
        if (SUCCEEDED(hr)) {
            for (LONG i = 0; i < numberFolder; i++) {

                hr = pSubFolders->get_Item(_variant_t(i + 1), &pNewTaskFolder);
                if (SUCCEEDED(hr)) {
                    BSTR nameFolder = NULL;
                    hr = pNewTaskFolder->get_Name(&nameFolder);
                    if (SUCCEEDED(hr)) {
                        string m_name_folder = _com_util::ConvertBSTRToString(nameFolder);
                        string m_root_name = _com_util::ConvertBSTRToString(allfolderName);
                        string m_new_folder = m_name_folder + "\\" + m_root_name;
                        BSTR new_folder = _com_util::ConvertStringToBSTR(m_new_folder.c_str());
                        readAllTask(pNewTaskFolder, hr, new_folder, taskMap);
                        SysFreeString(nameFolder);
                    }
                }

            }
            pSubFolders->Release();
        }
        pRootFolder->Release();
        return TRUE;
    }
    pRootFolder->Release();

    return FALSE;

}

#endif // READTASK_H
