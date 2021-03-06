/*##############################################################################

    HPCC SYSTEMS software Copyright (C) 2015 HPCC Systems.

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
############################################################################## */


#include <cstdio>
#include "jstream.ipp"
#include "jfile.hpp"
#include "jbuff.hpp"
#include "HPCCNagiosToolSet.hpp"
#include "HPCCNagiosToolSetCommon.hpp"
#include <cstring>
#include "XMLTags.h"
#include <iostream>
#include <ctime>

#define BUFFER_SIZE_3 1024

const char *P_DALI("dali");
const char *P_SASHA("sasha");
const char *P_ROXIE("roxie");
const char *P_DAFILESRV("dafilesrv");
const char *P_THOR("thor");

const char *P_ECLWATCH_FLAG(" -e ");
const char *P_USER_MACRO_FLAG(" -u ");
const char *P_PASSWORD_MACRO_FLAG(" -p ");
const char *P_USE_HTTPS_FLAG(" -l ");
const char *P_SET_DETAIL_URL_FLAG( " -r " );
const char *P_SET_APPEND_PORT_FROM_DETAIL(" -g ");

static bool bDoLookUp = true;

bool CHPCCNagiosToolSet::m_bVerbose                             = false;
bool CHPCCNagiosToolSet::m_retryHostNameLookUp                  = false;
bool CHPCCNagiosToolSet::m_bUseNPRE                             = false;
bool CHPCCNagiosToolSet::m_bUseAuthentication                   = false;
bool CHPCCNagiosToolSet::m_bCheckAllDisks                       = true;
bool CHPCCNagiosToolSet::m_bCheckLoad                           = true;
bool CHPCCNagiosToolSet::m_bCheckUsers                          = true;
bool CHPCCNagiosToolSet::m_bCheckProcs                          = true;
bool CHPCCNagiosToolSet::m_bEnableServiceEscalations            = false;
bool CHPCCNagiosToolSet::m_bEnableHostEscalations               = false;
bool CHPCCNagiosToolSet::m_bUseHTTPS                            = false;
char CHPCCNagiosToolSet::m_pSeparator[BUFFER_SIZE_1]            = {"ip:"};
char CHPCCNagiosToolSet::m_pNRPE[BUFFER_SIZE_1]                 = {""};
char CHPCCNagiosToolSet::m_pUserMacro[BUFFER_SIZE_1]            = {};
char CHPCCNagiosToolSet::m_pPasswordMacro[BUFFER_SIZE_1]        = {};
StringBuffer CHPCCNagiosToolSet::m_pUserMacroArray[32];
StringBuffer CHPCCNagiosToolSet::m_pPasswordMacroArray[32];
StringBuffer CHPCCNagiosToolSet::m_EspUserNamePWOverrides[32];
StringBuffer CHPCCNagiosToolSet::m_strCommandLine;
char CHPCCNagiosToolSet::m_pCheckPeriod[BUFFER_SIZE_1]          = {"24x7"};
int  CHPCCNagiosToolSet::m_pCheckInterval                       = 1;
char CHPCCNagiosToolSet::m_pContacts[BUFFER_SIZE_2]             = {"eclwatch"};
char CHPCCNagiosToolSet::m_pContactGroups[BUFFER_SIZE_2]        = {"eclwatch_group"};
int  CHPCCNagiosToolSet::m_nNotificationInterval                = 1;
char CHPCCNagiosToolSet::m_pNotificationPeriod[BUFFER_SIZE_2]   = {"24x7"};
int  CHPCCNagiosToolSet::m_nEnabled                             = 1;
int  CHPCCNagiosToolSet::m_nDisabled                            = 0;
int  CHPCCNagiosToolSet::m_nRetryInteval                        = 1;
int  CHPCCNagiosToolSet::m_nActiveChecksEnabled                 = CHPCCNagiosToolSet::m_nEnabled;
int  CHPCCNagiosToolSet::m_nPassiveChecksEnabled                = CHPCCNagiosToolSet::m_nEnabled;
int  CHPCCNagiosToolSet::m_nParallelizeCheck                    = CHPCCNagiosToolSet::m_nEnabled;
int  CHPCCNagiosToolSet::m_nObsessOverService                   = CHPCCNagiosToolSet::m_nEnabled;
int  CHPCCNagiosToolSet::m_nCheckFreshness                      = CHPCCNagiosToolSet::m_nDisabled;
int  CHPCCNagiosToolSet::m_nEventHandlerEnabled                 = CHPCCNagiosToolSet::m_nEnabled;
int  CHPCCNagiosToolSet::m_nFlapDetectionEnabled                = CHPCCNagiosToolSet::m_nEnabled;
int  CHPCCNagiosToolSet::m_nFailurePredictionEnabled            = CHPCCNagiosToolSet::m_nEnabled;
int  CHPCCNagiosToolSet::m_nProcessPerfData                     = CHPCCNagiosToolSet::m_nEnabled;
int  CHPCCNagiosToolSet::m_nRetainStatusInformation             = CHPCCNagiosToolSet::m_nDisabled;
int  CHPCCNagiosToolSet::m_nRetainNonStatusInformation          = CHPCCNagiosToolSet::m_nDisabled;
int  CHPCCNagiosToolSet::m_nIsVolatile                          = CHPCCNagiosToolSet::m_nDisabled;
int  CHPCCNagiosToolSet::m_nNormalCheckInterval                 = 1;
int  CHPCCNagiosToolSet::m_nRetryCheckInterval                  = 1;

char CHPCCNagiosToolSet::m_pCheckProcs[BUFFER_SIZE_2]           = {"check_procs"};
char CHPCCNagiosToolSet::m_pCheckDiskSpace[BUFFER_SIZE_2]       = {"check_all_disks"};
char CHPCCNagiosToolSet::m_pCheckUsers[BUFFER_SIZE_2]           = {"check_users"};
char CHPCCNagiosToolSet::m_pCheckLoad[BUFFER_SIZE_2]            = {"check_load"};
char CHPCCNagiosToolSet::m_pDevNULL[BUFFER_SIZE_1]              = {""};//{ "2> /dev/null/"};
char CHPCCNagiosToolSet::m_pSendHostStatus[BUFFER_SIZE_2]       = {EXEC_DIR"/send_status -o $HOSTADDRESS$ -s $HOSTSTATE$ -d '$HOSTNOTES$' -t $TIMET$ -n $HOSTDISPLAYNAME$"};
char CHPCCNagiosToolSet::m_pSendServiceStatus[BUFFER_SIZE_2]    = {EXEC_DIR"/send_status -o $HOSTADDRESS$ -s $SERVICESTATE$ -d '$SERVICENOTES$' -t $TIMET$ -n $SERVICEDISPLAYNAME$"};

char CHPCCNagiosToolSet::m_pServiceNotificatonCommand[BUFFER_SIZE_2] = {"send_service_status"};
char CHPCCNagiosToolSet::m_pHostNotificatonCommand[BUFFER_SIZE_2]    = {"send_host_status"};
char CHPCCNagiosToolSet::m_pNotificationURL[URL_BUFFER_SIZE]    = "";

int CHPCCNagiosToolSet::m_uMaxCheckAttempts             = 10;
int CHPCCNagiosToolSet::m_nDiskSpacePercentageWarning   = 15;
int CHPCCNagiosToolSet::m_nDiskSpacePercentageCritical  = 8;
int CHPCCNagiosToolSet::m_nUserNumberWarning            = 5;
int CHPCCNagiosToolSet::m_nUserNumberCritical           = 10;
int CHPCCNagiosToolSet::m_nTotalProcsWarning            = 350;
int CHPCCNagiosToolSet::m_nTotalProcsCritical           = 500;
float CHPCCNagiosToolSet::m_fSystemLoad1Warn            = 5.0;
float CHPCCNagiosToolSet::m_fSystemLoad5Warn            = 4.0;
float CHPCCNagiosToolSet::m_fSystemLoad15Warn           = 3.0;
float CHPCCNagiosToolSet::m_fSystemLoad1Critical        = 10.0;
float CHPCCNagiosToolSet::m_fSystemLoad5Critical        = 6.0;
float CHPCCNagiosToolSet::m_fSystemLoad15Critical       = 4.0;

class CHPCCNagiosHostEventForSSH : public CHPCCNagiosHostEvent
{
public:
    CHPCCNagiosHostEventForSSH(StringBuffer *pStrBuffer) : CHPCCNagiosHostEvent(pStrBuffer)
    {
    }
    virtual void onHostEvent(const char *pHostName, int idx, const char *pToken, const char* pNote)
    {
        StringBuffer strNote;

        if (pNote != NULL)
        {
            strNote.set(pNote);
        }

        if (pHostName == NULL || *pHostName == 0)
        {
            return;
        }
        else
        {
            m_pStrBuffer->append(P_NAGIOS_SERVICE_CONFIG_2).append(pHostName).append(P_NAGIOS_SERVICE_DESCRIPTION).append("check for ssh connectivity")\
                    .append(P_NAGIOS_SERVICE_CHECK_COMMAND).append(CHPCCNagiosToolSet::m_pNRPE).append(P_CHECK_SSH_SERVICE);

            if (CHPCCNagiosToolSet::m_bUseNPRE == false)
            {
                m_pStrBuffer->append(P_NAGIOS_SEPERATOR).append(CHPCCNagiosHostEventForSSH::m_nTimeOut);
            }

            CHPCCNagiosToolSet::generateNagiosServiceEscalationConfig(*m_pStrBuffer, P_SSH, strNote.str());

            m_pStrBuffer->append(P_NAGIOS_SERVICE_END_BRACKET);

            m_pStrBuffer->append("\n");
        }
    }
    static const int m_nTimeOut;

protected:

    CHPCCNagiosHostEventForSSH()
    {
    }
};

const int CHPCCNagiosHostEventForSSH::m_nTimeOut = 10;

class CHPCCNagiosHostEventHostConfig : public CHPCCNagiosHostEvent
{
public:
    CHPCCNagiosHostEventHostConfig(StringBuffer *pStrBuffer) : CHPCCNagiosHostEvent(pStrBuffer)
    {
    }
    virtual void onHostEvent(const char *pHostName, int idx, const char *pToken, const char* pNote)
    {
        StringBuffer strNote;

        if (pNote != NULL)
        {
            strNote.set(pNote);
        }

        if (pHostName == NULL || *pHostName == 0)
        {
            return;
        }
        else
        {
            m_pStrBuffer->append(P_NAGIOS_HOST_CONFIG_1).append(pHostName).append(P_NAGIOS_HOST_CONFIG_ALIAS).append(pHostName).append(" ")\
                    .append(idx).append(P_NAGIOS_HOST_CONFIG_ADDRESS).append(pToken)\
                    .append(P_NAGIOS_HOST_DISPLAY_NAME).append(pHostName)\
                    .append(P_NAGIOS_SERVICE_RETAIN_STATUS_INFORMATION).append(CHPCCNagiosToolSet::m_nRetainStatusInformation)\
                    .append(P_NAGIOS_SERVICE_RETAIN_NONSTATUS_INFORMATION).append(CHPCCNagiosToolSet::m_nRetainNonStatusInformation)\
                    .append(P_NAGIOS_SERVICE_EVENT_HANDLER_ENABLED).append(CHPCCNagiosToolSet::m_nEventHandlerEnabled)\
                    .append(P_NAGIOS_SERVICE_FLAP_DETECTION_ENABLED).append(CHPCCNagiosToolSet::m_nFlapDetectionEnabled)\
                    .append(P_NAGIOS_SERVICE_FAILURE_PREDICTION_ENABLED).append(CHPCCNagiosToolSet::m_nFailurePredictionEnabled)\
                    .append(P_NAGIOS_HOST_CONFIG_MAX_CHECK_ATTEMPTS).append(CHPCCNagiosToolSet::m_uMaxCheckAttempts)\
                    .append(P_NAGIOS_HOST_CONFIG_CHECK_PERIOD).append(CHPCCNagiosToolSet::m_pCheckPeriod)\
/*                    .append(P_NAGIOS_HOST_CONFIG_CONTACTS).append(CHPCCNagiosToolSet::m_pContacts)\*/
                    .append(P_NAGIOS_HOST_CONFIG_CONTACT_GROUPS).append(CHPCCNagiosToolSet::m_pContactGroups)\
                    .append(P_NAGIOS_HOST_CONFIG_NOTIFICATION_INTERVAL).append(CHPCCNagiosToolSet::m_nNotificationInterval)\
                    .append(P_NAGIOS_HOST_CONFIG_NOTIFICATION_PERIOD).append(CHPCCNagiosToolSet::m_pNotificationPeriod)\
                    .append(P_NAGIOS_HOST_CONFIG_NOTIFICATIONS_ENABLED);

            if (CHPCCNagiosToolSet::m_bEnableHostEscalations == true)
            {
                m_pStrBuffer->append(CHPCCNagiosToolSet::m_nEnabled);
            }
            else
            {
                m_pStrBuffer->append(CHPCCNagiosToolSet::m_nDisabled);
            }


            m_pStrBuffer->append(P_NAGIOS_HOST_CONFIG_END);
            m_pStrBuffer->append("\n");
        }
    }
protected:
    CHPCCNagiosHostEventHostConfig()
    {
    }
};

class CHPCCNagiosNRPEClientEventConfig : public CHPCCNagiosHostEvent
{
public:
    CHPCCNagiosNRPEClientEventConfig(StringBuffer *pStrBuffer) : CHPCCNagiosHostEvent(pStrBuffer)
    {
    }
    virtual void onHostEvent(const char *pHostName, int idx, const char *pToken, const char* pNote)
    {
        StringBuffer strNote;

        if (pNote != NULL)
        {
            strNote.set(pNote);
        }

        if (pHostName == NULL || *pHostName == 0)
        {
            return;
        }
        else
        {
            m_pStrBuffer->append(P_NAGIOS_HOST_CONFIG_1).append(pHostName).append(P_NAGIOS_HOST_CONFIG_ALIAS).append(pHostName).append(" ")\
                    .append(idx).append(P_NAGIOS_HOST_CONFIG_ADDRESS).append(pToken)\
                    .append(P_NAGIOS_HOST_DISPLAY_NAME).append(pHostName)\
                    .append(P_NAGIOS_HOST_CONFIG_MAX_CHECK_ATTEMPTS).append(CHPCCNagiosToolSet::m_uMaxCheckAttempts)\
                    .append(P_NAGIOS_HOST_CONFIG_CHECK_PERIOD).append(CHPCCNagiosToolSet::m_pCheckPeriod)\
                    .append(P_NAGIOS_HOST_CONFIG_CONTACTS).append(CHPCCNagiosToolSet::m_pContacts)\
                    .append(P_NAGIOS_HOST_CONFIG_CONTACT_GROUPS).append(CHPCCNagiosToolSet::m_pContactGroups)\
                    .append(P_NAGIOS_HOST_CONFIG_NOTIFICATION_INTERVAL).append(CHPCCNagiosToolSet::m_nNotificationInterval)\
                    .append(P_NAGIOS_HOST_CONFIG_NOTIFICATION_PERIOD).append(CHPCCNagiosToolSet::m_pNotificationPeriod)\
                    .append(P_NAGIOS_HOST_CONFIG_END);
            m_pStrBuffer->append("\n");
        }
    }
protected:
    CHPCCNagiosNRPEClientEventConfig()
    {
    }
};


bool CHPCCNagiosToolSet::generateNagiosHostConfig(CHPCCNagiosHostEvent &evHost, MapIPtoNode &mapIPtoHostName, const char* pEnvXML, const char* pConfigGenPath)
{
    char *pOutput = CHPCCNagiosToolSet::invokeConfigGen(pEnvXML, pConfigGenPath, P_CONFIGGEN_PARAM_MACHINES);

    if (pOutput == NULL)
    {
        return false;
    }

    int nCount = 0;

    char *pch = NULL;
    pch = strtok(pOutput, ",\n");

    int i = 0;
    while (pch != NULL)
    {
        if (nCount % 2 ==  0) // Process name
        {
            char pHostName[BUFFER_SIZE_3] = "";
            struct hostent* hp = NULL;

            if (bDoLookUp == true)
            {
                unsigned int addr = inet_addr(pch);
                hp = gethostbyaddr((const char*)&addr, sizeof(addr), AF_INET);
            }

            if (hp == NULL)
            {
                bDoLookUp =  m_retryHostNameLookUp;
                strcpy(pHostName, pch);
            }
            else
            {
                strcpy(pHostName,hp->h_name);
            }

            evHost.onHostEvent(pHostName, i, pch);

            struct NodeName nm;

            nm.strHostName.set(pHostName);
            nm.strHostAlias.setf("%s %d", pHostName, i);
            mapIPtoHostName.setValue(pch, nm);

            i++;
        }

        pch = strtok(NULL, ",\n");

        nCount++;
    }

    free(pOutput);

    return true;
}

bool CHPCCNagiosToolSet::generateHostGroupsConfigurationFile(const char* pOutputFilePath, const char* pEnvXML, const char* pConfigGenPath)
{
    if (pOutputFilePath == NULL || *pOutputFilePath == 0 || checkFileExists(pConfigGenPath) == false)
    {
        return false;
    }
    char *pOutput = CHPCCNagiosToolSet::invokeConfigGen(pEnvXML, pConfigGenPath);

    if (m_bVerbose == true)
    {
        std::cout << "\nOutput:\n" << pOutput << "\n";
    }

    if (pOutput == NULL)
    {
        return false;
    }

    OwnedIFile outputFile = createIFile(pOutputFilePath);
    OwnedIFileIO io = outputFile->open(IFOcreaterw);

    if (io == NULL)
    {
        return false;
    }

    int nCount = 0;
    char pProcess[BUFFER_SIZE_3] = "";
    bool bAdd = false;
    char *pch = NULL;
    char pHostName[BUFFER_SIZE_3] = "";
    StringBuffer strHostConfig;
    int i = 0;

    pch = strtok(pOutput, ",\n");

    while (pch != NULL)
    {
        if (nCount % 6 ==  0) // Process name
        {
            if (pProcess != NULL && *pProcess != 0 && strcmp(pProcess, pch) != 0)
            {
                strHostConfig.append(P_NAGIOS_HOSTS_GROUP_END_BRACKET);
                strHostConfig.append("\n");
                strncpy(pProcess, pch, sizeof(pProcess));
                bAdd = true;
                i = 0;
            }
            else if (pProcess == NULL || *pProcess == 0 || strcmp(pProcess, pch) != 0)
            {
                strncpy(pProcess, pch, sizeof(pProcess));
                bAdd = true;
                i++;
            }
            else if (strcmp(pProcess,pch) == 0)
            {
                i++;
                bAdd = false;
            }
        }
        else if (nCount % 6 == 2) // IP address
        {
            hostent *hp;

            memset(pHostName,0,sizeof(pHostName));

            if (bDoLookUp == true)
            {
                unsigned int addr = inet_addr(pch);
                hp = gethostbyaddr((const char*)&addr, sizeof(addr), AF_INET);
            }

            if (hp == NULL)
            {
                bDoLookUp =  m_retryHostNameLookUp;
                if (strcmp(pch, pHostName) == 0)
                {
                    pch = strtok(NULL, ",\n");
                    nCount++;

                    continue;
                }
                strcpy(pHostName, pch);
            }
            else
            {
                if (strcmp(pch, pHostName) == 0)
                {
                    pch = strtok(NULL, ",\n");
                    nCount++;

                    continue;
                }
                strcpy(pHostName,hp->h_name);
            }

            static char pLastHostName[BUFFER_SIZE_3] = "";

            if (bAdd == true)
            {
                strHostConfig.append(P_NAGIOS_HOSTS_GROUP_CONFIG_1).append(pProcess).append("-servers").append(P_NAGIOS_HOSTS_GROUP_ALIAS).append(pProcess).append(" servers")\
                                .append(P_NAGIOS_HOSTS_GROUP_MEMBERS).append(pHostName);

                strcpy(pLastHostName,pHostName);
                bAdd = false;
            }
            else
            {
                if (strcmp(pLastHostName,pHostName) != 0)
                {
                    strHostConfig.append(", ").append(pHostName);
                    strcpy(pLastHostName,pHostName);
                }
            }
        }

        pch = strtok(NULL, ",\n");

        nCount++;
    }

    if (strHostConfig.length() > 0)
    {
        appendHeader(strHostConfig);        
        strHostConfig.append(P_NAGIOS_HOSTS_GROUP_END_BRACKET);

        io->write(0, strHostConfig.length(), strHostConfig.str());
    }

    io->close();
    free(pOutput);

    return true;
}

bool CHPCCNagiosToolSet::generateNagiosNRPEClientConfig(CHPCCNagiosHostEvent &evHost, MapIPtoNode &mapIPtoHostName, const char* pEnvXML, const char* pConfigGenPath)
{
    char *pOutput = CHPCCNagiosToolSet::invokeConfigGen(pEnvXML, pConfigGenPath);

    free(pOutput);
}

bool CHPCCNagiosToolSet::generateEscalationCommandConfigurationFile(const char* pOutputFilePath, const StringArray &strEclWatchHostPortArray, const char* pUserMacro, const char* pPasswordMacro,\
                                                                     bool bUseHTTPS, bool bAppendHostPortFromDetail, const char *pURL, const char* pEnvXML, const char* pConfigGenPath)
{
    StringBuffer strCommandConfig;

    CHPCCNagiosToolSet::generateNagiosEscalationCommandConfig(strCommandConfig, strEclWatchHostPortArray, pUserMacro, pPasswordMacro, bUseHTTPS, bAppendHostPortFromDetail,\
                                                              pURL, pEnvXML, pConfigGenPath);

    OwnedIFile outputFile = createIFile(pOutputFilePath);
    OwnedIFileIO io = outputFile->open(IFOcreaterw);

    if (io == NULL)
    {
        return false;
    }

    if (strCommandConfig.length() != 0)
    {
        appendHeader(strCommandConfig);
        io->write(0, strCommandConfig.length(), strCommandConfig.str());
    }
    io->close();

    return true;
}

bool CHPCCNagiosToolSet::generateServerAndHostConfigurationFile(const char* pOutputFilePath, const char* pEnvXML, const char* pConfigGenPath)
{
    if (pOutputFilePath == NULL || *pOutputFilePath == 0)
    {
        return false;
    }

    char *pOutput = CHPCCNagiosToolSet::invokeConfigGen(pEnvXML, pConfigGenPath);

    if (pOutput == NULL)
    {
        return false;
    }

    const int nNumValues = 6;
    int nCount = 0;
    char pProcess[BUFFER_SIZE_3] = "";

    OwnedIFile outputFile = createIFile(pOutputFilePath);
    OwnedIFileIO io = outputFile->open(IFOcreaterw);

    if (io == NULL)
    {
        return false;
    }

    StringBuffer strServiceConfig;
    MapIPtoNode mapIPtoHostName;
    CHPCCNagiosHostEventHostConfig evHost(&strServiceConfig);
    CHPCCNagiosToolSet::generateNagiosHostConfig(evHost, mapIPtoHostName, pEnvXML, pConfigGenPath);

    int i = -1;
    char *pch = NULL;
    pch = strtok(pOutput, ",\n");

    while (pch != NULL)
    {
        if (nCount % nNumValues ==  0) // Process name
        {
            if (*pch != 0 && (strcmp(pch,XML_TAG_ESPPROCESS) == 0) || strcmp(pch, XML_TAG_DALISERVERPROCESS) == 0\
                || strcmp(pch, XML_TAG_SASHA_SERVER_PROCESS) == 0 || strcmp(pch, XML_TAG_DAFILESERVERPROCESS) == 0)
            {
                i++;
            }
            else if (pProcess != NULL && *pProcess != 0 && strcmp(pProcess, pch) != 0)
            {
                strncpy(pProcess, pch, sizeof(pProcess));
                i = 0;
            }
            else if (pProcess == NULL || *pProcess == 0 || strcmp(pProcess, pch) != 0)
            {
                strncpy(pProcess, pch, sizeof(pProcess));
                i++;
            }
            else if (strcmp(pProcess,pch) == 0)
            {
                i++;
            }
        }

        pch = strtok(NULL, ",\n");

        nCount++;
    }

    if (CHPCCNagiosToolSet::m_bUseNPRE == true)
    {
        strncpy(CHPCCNagiosToolSet::m_pNRPE, P_CHECK_NRPE_1_ARG, sizeof(CHPCCNagiosToolSet::m_pNRPE));
    }

    CHPCCNagiosToolSet::generateNagiosEspServiceConfig(strServiceConfig, pEnvXML, pConfigGenPath);
    CHPCCNagiosToolSet::generateNagiosDaliCheckConfig(strServiceConfig, pEnvXML, pConfigGenPath);
    CHPCCNagiosToolSet::generateNagiosThorCheckConfig(strServiceConfig, pEnvXML, pConfigGenPath);
    CHPCCNagiosToolSet::generateNagiosSashaCheckConfig(strServiceConfig, pEnvXML, pConfigGenPath);
    CHPCCNagiosToolSet::generateNagiosRoxieCheckConfig(strServiceConfig, pEnvXML, pConfigGenPath);
    CHPCCNagiosToolSet::generateNagiosDafileSrvCheckConfig(strServiceConfig, pEnvXML, pConfigGenPath);
    CHPCCNagiosToolSet::generateNagiosSystemCheckConfig(strServiceConfig, pEnvXML, pConfigGenPath);

    CHPCCNagiosHostEventForSSH event(&strServiceConfig);
    MapIPtoNode map;
    CHPCCNagiosToolSet::generateNagiosHostConfig(event,map, pEnvXML, pConfigGenPath);

    if (strServiceConfig.length() > 0)
    {
        appendHeader(strServiceConfig);
        io->write(0, strServiceConfig.length(), strServiceConfig.str());
    }
    io->close();

    free(pOutput);
    return true;
}

bool CHPCCNagiosToolSet::getConfigGenOutput(const char* pEnvXML, const char* pConfigGenPath, const char* pCommandLine, StringBuffer &strBuff)
{
    if (pConfigGenPath == NULL || *pConfigGenPath == 0)
    {
        pConfigGenPath = PCONFIGGEN_PATH;
    }

    if (pEnvXML == NULL || *pEnvXML == 0)
    {
        pEnvXML = PENV_XML;
    }

    int nRetCode = 0;

    Owned<IPipeProcess> pipe = createPipeProcess();

    if (pipe->run("configgen", pCommandLine, ".", false, true, true, 0))
    {
        nRetCode = pipe->wait();

        if (nRetCode != 0)
        {
            std::cerr << "\nERROR " << nRetCode << ": unable to execute " << pCommandLine << "\n";
            exit(1);
        }

        Owned<ISimpleReadStream> pipeReader = pipe->getOutputStream();
        const size32_t chunkSize = 8192;
        int nTotalBytes = 0;

        loop
        {
            size32_t sizeRead = pipeReader->read(chunkSize, strBuff.reserve(chunkSize));

            nTotalBytes += sizeRead;

            if (sizeRead < chunkSize)
            {
                strBuff.setLength(strBuff.length() - (chunkSize - sizeRead));
                break;
            }
        }
        pipe->closeOutput();
    }

    if (CHPCCNagiosToolSet::m_bVerbose == true)
    {
        std::cout << "Buffer->\n" << strBuff.str() << "\n<--\n";
    }

    return true;
}

bool CHPCCNagiosToolSet::generateNagiosEspServiceConfig(StringBuffer &strServiceConfig, const char* pEnvXML, const char* pConfigGenPath)
{
    char *pOutput = CHPCCNagiosToolSet::invokeConfigGen(pEnvXML, pConfigGenPath, P_CONFIGGEN_PARAM_LIST_ESP_SERVICES);

    int i = -1;
    char pProcess[BUFFER_SIZE_3] = "";
    int nCount = 0;
    char *pch = NULL;
    pch = strtok(pOutput, ",\n");
    StringBuffer strPort;
    StringBuffer strIPAddress;
    char pHostName[BUFFER_SIZE_3] = "";
    char pServiceName[BUFFER_SIZE_3] = "";
    char pProcessName[BUFFER_SIZE_3] = "";

    while (pch != NULL)
    {
        if (nCount % 7 ==  0) // Process type
        {
            if (*pch != 0 && strcmp(pch, XML_TAG_ESPPROCESS) != 0)
            {
                free(pOutput);
                return false;  // expecting only EspProcess
            }
            else if (pProcess != NULL && *pProcess != 0 && strcmp(pProcess, pch) != 0)
            {
                strncpy(pProcess, pch, sizeof(pProcess));
                i = 0;
            }
            else if (pProcess == NULL || *pProcess == 0 || strcmp(pProcess, pch) != 0)
            {
                strncpy(pProcess, pch, sizeof(pProcess));
                i++;
            }
            else if (strcmp(pProcess,pch) == 0)
            {
                i++;
            }
        }
        if (nCount % 7 ==  1) // Process name
        {
            strncpy(pProcessName, pch, sizeof(pProcessName));
        }
        else if (nCount % 7 == 3) // service name
        {
            strncpy(pServiceName,pch, sizeof(pServiceName));
        }
        else if (nCount % 7 == 4) // IP Address
        {
            strIPAddress.clear().append(pch);

            struct hostent* hp = NULL;

            if (bDoLookUp == true)
            {
                unsigned int addr = inet_addr(pch);
                hp = gethostbyaddr((const char*)&addr, sizeof(addr), AF_INET);
            }

            if (hp == NULL)
            {
                bDoLookUp =  m_retryHostNameLookUp;
                strcpy(pHostName, pch);
            }
            else
            {
                strcpy(pHostName,hp->h_name);
            }
        }
        else if (nCount % 7 == 5) // IP Port
        {
            strPort.clear().append(pch);
        }
        else if (nCount % 7 == 6) // protocol
        {
            VStringBuffer strNote("check for %s of type %s %s%s:%s", pProcessName, pProcess, CHPCCNagiosToolSet::m_pSeparator, strIPAddress.str(), strPort.str());

            strServiceConfig.append(P_NAGIOS_SERVICE_CONFIG_2).append(pHostName).append(P_NAGIOS_SERVICE_DESCRIPTION)\
                    .append(strNote.str())\
                    .append(P_NAGIOS_SERVICE_CHECK_COMMAND).append(m_pNRPE).append(P_CHECK_ESP_SERVICE).append( strcmp(pch,P_HTTP) == 0 ? P_HTTP : P_HTTPS)\
                    .append(CHPCCNagiosToolSet::m_bUseAuthentication ? "_auth" : "");

            if (m_bUseNPRE == false)
            {
                strServiceConfig.appendf("%s%s", P_NAGIOS_SEPERATOR, strPort.str());
            }

            if (m_bUseAuthentication == true)
            {
                strServiceConfig.appendf("%s%s%s%s", P_NAGIOS_SEPERATOR, getEspUserName(pProcessName, m_pUserMacro), P_NAGIOS_SEPERATOR, getEspPassword(pProcessName, m_pPasswordMacro));
            }

            CHPCCNagiosToolSet::generateNagiosServiceEscalationConfig(strServiceConfig, XML_TAG_ESPPROCESS, strNote.str());

            strServiceConfig.appendf("%s\n", P_NAGIOS_SERVICE_END_BRACKET);
        }

        pch = strtok(NULL, ",\n");

        nCount++;
    }

    free(pOutput);

    return true;
}

bool CHPCCNagiosToolSet::generateNagiosDaliCheckConfig(StringBuffer &strServiceConfig, const char* pEnvXML, const char* pConfigGenPath)
{
    const int nNumValues = 5;

    char *pOutput = CHPCCNagiosToolSet::invokeConfigGen(pEnvXML, pConfigGenPath, P_CONFIGGEN_PARAM_LIST_ALL, P_DALI);

    if (pOutput == NULL)
    {
        return false;
    }

    int i = -1;
    char pProcess[BUFFER_SIZE_3] = "";
    int nCount = 0;
    char *pch = NULL;
    pch = strtok(pOutput, ",\n");
    StringBuffer strPort;
    StringBuffer strIPAddress;
    char pHostName[BUFFER_SIZE_3] = "";
    char pProcessName[BUFFER_SIZE_3] = "";

    while (pch != NULL)
    {
        if (nCount % nNumValues ==  0) // Process name
        {
            if (*pch != 0 && strcmp(pch, XML_TAG_DALISERVERPROCESS) != 0)
            {
                free(pOutput);
                return false;  // expecting only Dali
            }
            else if (pProcess != NULL && *pProcess != 0 && strcmp(pProcess, pch) != 0)
            {
                strncpy(pProcess, pch, sizeof(pProcess));
                i = 0;
            }
            else if (pProcess == NULL || *pProcess == 0 || strcmp(pProcess, pch) != 0)
            {
                strncpy(pProcess, pch, sizeof(pProcess));
                i++;
            }
            else if (strcmp(pProcess,pch) == 0)
            {
                i++;
            }
        }
        else if (nCount % nNumValues == 1) // process name
        {
            strcpy(pProcessName,pch);
        }
        else if (nCount % nNumValues == 2) // IP Address
        {
            strIPAddress.set(pch);

            struct hostent* hp = NULL;

            if (bDoLookUp == true)
            {
                unsigned int addr = inet_addr(pch);
                hp = gethostbyaddr((const char*)&addr, sizeof(addr), AF_INET);
            }

            if (hp == NULL)
            {
                bDoLookUp =  m_retryHostNameLookUp;
                strcpy(pHostName, pch);
            }
            else
            {
                strcpy(pHostName,hp->h_name);
            }
        }
        else if (nCount % nNumValues == 3) // IP Port
        {
            strPort.clear().append(pch);
        }
        else if (nCount % nNumValues == 4)
        {
            StringBuffer strNote;
            strNote.appendf("check for %s of type %s %s%s:%s", pProcessName, pProcess, CHPCCNagiosToolSet::m_pSeparator, strIPAddress.str(), strPort.str());

            strServiceConfig.append(P_NAGIOS_SERVICE_CONFIG_2).append(pHostName).append(P_NAGIOS_SERVICE_DESCRIPTION).append(strNote.str())\
                    .append(P_NAGIOS_SERVICE_CHECK_COMMAND).append(CHPCCNagiosToolSet::m_pNRPE).append(P_CHECK_DALI);

            if (CHPCCNagiosToolSet::m_bUseNPRE == false)
            {
                strServiceConfig.append(P_NAGIOS_SEPERATOR).append(strPort.str()).append(P_NAGIOS_SEPERATOR).append(DALI_CHECK_TIMEOUT);
            }

            CHPCCNagiosToolSet::generateNagiosServiceEscalationConfig(strServiceConfig, XML_TAG_DALISERVERPROCESS, strNote.str());

            strServiceConfig.append(P_NAGIOS_SERVICE_END_BRACKET).append("\n");
        }

        pch = strtok(NULL, ",\n");

        nCount++;
    }

    free(pOutput);

    return true;
}

bool CHPCCNagiosToolSet::generateNagiosSashaCheckConfig(StringBuffer &strServiceConfig, const char* pEnvXML, const char* pConfigGenPath)
{
    const int nNumValues = 5;

    char *pOutput = CHPCCNagiosToolSet::invokeConfigGen(pEnvXML, pConfigGenPath, P_CONFIGGEN_PARAM_LIST_ALL, P_SASHA);

    if (pOutput == NULL)
    {
        return false;
    }

    int i = -1;
    char pProcess[BUFFER_SIZE_3] = "";
    int nCount = 0;
    char *pch = NULL;
    pch = strtok(pOutput, ",\n");
    StringBuffer strPort;
    StringBuffer strIPAddress;
    char pHostName[BUFFER_SIZE_3] = "";
    char pProcessName[BUFFER_SIZE_3] = "";

    while (pch != NULL)
    {
        if (nCount % nNumValues ==  0) // Process name
        {
            if (*pch != 0 && strcmp(pch, XML_TAG_SASHA_SERVER_PROCESS) != 0)
            {
                free(pOutput);
                return false;  // expecting only sasha
            }
            else if (pProcess != NULL && *pProcess != 0 && strcmp(pProcess, pch) != 0)
            {
                strncpy(pProcess, pch, sizeof(pProcess));
                i = 0;
            }
            else if (pProcess == NULL || *pProcess == 0 || strcmp(pProcess, pch) != 0)
            {
                strncpy(pProcess, pch, sizeof(pProcess));
                i++;
            }
            else if (strcmp(pProcess,pch) == 0)
            {
                i++;
            }
        }
        else if (nCount % nNumValues == 1) // process name
        {
            strcpy(pProcessName,pch);
        }
        else if (nCount % nNumValues == 2) // IP Address
        {
            strIPAddress.clear().append(pch);

            struct hostent* hp = NULL;

            if (bDoLookUp == true)
            {
                unsigned int addr = inet_addr(pch);
                hp = gethostbyaddr((const char*)&addr, sizeof(addr), AF_INET);
            }

            if (hp == NULL)
            {
                bDoLookUp =  m_retryHostNameLookUp;
                strcpy(pHostName, pch);
            }
            else
            {
                strcpy(pHostName,hp->h_name);
            }
        }
        else if (nCount % nNumValues == 3) // IP Port
        {
            strPort.clear().append(pch);
        }
        else if (nCount % nNumValues == 4)
        {
            StringBuffer strNote;
            strNote.appendf("check for %s of type %s %s%s:%s", pProcessName, pProcess, CHPCCNagiosToolSet::m_pSeparator, strIPAddress.str(), strPort.str());

            strServiceConfig.append(P_NAGIOS_SERVICE_CONFIG_2).append(pHostName).append(P_NAGIOS_SERVICE_DESCRIPTION).append(strNote.str())\
                    .append(P_NAGIOS_SERVICE_CHECK_COMMAND).append(CHPCCNagiosToolSet::m_pNRPE).append(P_CHECK_SASHA);

            if (CHPCCNagiosToolSet::m_bUseNPRE == false)
            {
                strServiceConfig.append(P_NAGIOS_SEPERATOR).append(strPort.str()).append(P_NAGIOS_SEPERATOR).append(SASHA_CHECK_TIMEOUT);
            }

            CHPCCNagiosToolSet::generateNagiosServiceEscalationConfig(strServiceConfig, XML_TAG_SASHA_SERVER_PROCESS, strNote.str());
            strServiceConfig.append(P_NAGIOS_SERVICE_END_BRACKET).append("\n");
        }

        pch = strtok(NULL, ",\n");

        nCount++;
    }

    free(pOutput);

    return true;
}

bool CHPCCNagiosToolSet::generateNagiosRoxieCheckConfig(StringBuffer &strServiceConfig, const char* pEnvXML , const char* pConfigGenPath)
{
    const int nNumValues = 5;
    char *pOutput = CHPCCNagiosToolSet::invokeConfigGen(pEnvXML, pConfigGenPath, P_CONFIGGEN_PARAM_LIST_ALL, P_ROXIE);

    if (pOutput == NULL)
    {
        return false;
    }

    int i = -1;
    char pProcess[BUFFER_SIZE_3] = "";
    int nCount = 0;
    char *pch = NULL;
    pch = strtok(pOutput, ",\n");
    StringBuffer strIPAddress;
    char pHostName[BUFFER_SIZE_3] = "";
    char pProcessName[BUFFER_SIZE_3] = "";

    while (pch != NULL)
    {
        if (nCount % nNumValues ==  0) // Process name
        {
            if (*pch != 0 && strcmp(pch, XML_TAG_ROXIE_SERVER) != 0)
            {
                free(pOutput);
                return false;  // expecting only roxie
            }
            else if (pProcess != NULL && *pProcess != 0 && strcmp(pProcess, pch) != 0)
            {
                strncpy(pProcess, pch, sizeof(pProcess));
                i = 0;
            }
            else if (pProcess == NULL || *pProcess == 0 || strcmp(pProcess, pch) != 0)
            {
                strncpy(pProcess, pch, sizeof(pProcess));
                i++;
            }
            else if (strcmp(pProcess,pch) == 0)
            {
                i++;
            }
        }
        else if (nCount % nNumValues == 1) // process name
        {
            strcpy(pProcessName,pch);
        }
        else if (nCount % nNumValues == 2) // IP Address
        {
            strIPAddress.clear().append(pch);

            struct hostent* hp = NULL;

            if (bDoLookUp == true)
            {
                unsigned int addr = inet_addr(pch);
                hp = gethostbyaddr((const char*)&addr, sizeof(addr), AF_INET);
            }

            if (hp == NULL)
            {
                bDoLookUp =  m_retryHostNameLookUp;
                strcpy(pHostName, pch);
            }
            else
            {
                strcpy(pHostName,hp->h_name);
            }
        }
        else if (nCount % nNumValues == 4)
        {
            StringBuffer strNote;
            strNote.appendf("check for %s of type %s", pProcessName, pProcess);

            strServiceConfig.append(P_NAGIOS_SERVICE_CONFIG_2).append(pHostName).append(P_NAGIOS_SERVICE_DESCRIPTION).append(strNote.str())\
                    .append(P_NAGIOS_SERVICE_CHECK_COMMAND).append(CHPCCNagiosToolSet::m_pNRPE).append(P_CHECK_ROXIE);

            CHPCCNagiosToolSet::generateNagiosServiceEscalationConfig(strServiceConfig, XML_TAG_ROXIE_SERVER, strNote.str());

            strServiceConfig.append(P_NAGIOS_SERVICE_END_BRACKET).append("\n");
        }

        pch = strtok(NULL, ",\n");

        nCount++;
    }

    free(pOutput);

    return true;
}

bool CHPCCNagiosToolSet::generateNagiosSystemCheckConfig(StringBuffer &strServiceConfig, const char* pEnvXML, const char* pConfigGenPath)
{
    const int nNumValues = 2;
    char *pOutput = CHPCCNagiosToolSet::invokeConfigGen(pEnvXML, pConfigGenPath, P_CONFIGGEN_PARAM_MACHINES);

    if (pOutput == NULL)
    {
        return false;
    }

    int i = -1;
    int nCount = 0;
    char *pch = NULL;
    pch = strtok(pOutput, ",\n");
    StringBuffer strIPAddress;
    char pHostName[BUFFER_SIZE_3] = "";

    while (pch != NULL)
    {
        if (nCount % nNumValues == 0) // IP Address
        {
            strIPAddress.clear().append(pch);

            struct hostent* hp = NULL;

            if (bDoLookUp == true)
            {
                unsigned int addr = inet_addr(pch);
                hp = gethostbyaddr((const char*)&addr, sizeof(addr), AF_INET);
            }

            if (hp == NULL)
            {
                bDoLookUp =  m_retryHostNameLookUp;
                strcpy(pHostName, pch);
            }
            else
            {
                strcpy(pHostName,hp->h_name);
            }
            if (CHPCCNagiosToolSet::m_bCheckProcs == true)
            {
                // processes
                strServiceConfig.append(P_NAGIOS_SERVICE_CONFIG_2).append(pHostName).append(P_NAGIOS_SERVICE_DESCRIPTION).appendf("check for %s", m_pCheckProcs)\
                        .append(P_NAGIOS_SERVICE_CHECK_COMMAND).append(CHPCCNagiosToolSet::m_pNRPE).append(m_pCheckProcs);

                if (CHPCCNagiosToolSet::m_bUseNPRE == false)
                {
                    strServiceConfig.append(P_NAGIOS_SEPERATOR).append(m_nTotalProcsWarning)\
                        .append(P_NAGIOS_SEPERATOR).append(m_nTotalProcsCritical);
                }

                StringBuffer strNote;
                strNote.appendf("check for %s", m_pCheckProcs);
                CHPCCNagiosToolSet::generateNagiosServiceEscalationConfig(strServiceConfig, m_pCheckProcs, strNote.str());

                strServiceConfig.append(P_NAGIOS_SERVICE_END_BRACKET).append("\n");
            }

            if (CHPCCNagiosToolSet::m_bCheckAllDisks  == true)
            {
                // disk space
                strServiceConfig.append(P_NAGIOS_SERVICE_CONFIG_2).append(pHostName).append(P_NAGIOS_SERVICE_DESCRIPTION).appendf("check for %s", m_pCheckDiskSpace)\
                        .append(P_NAGIOS_SERVICE_CHECK_COMMAND).append(CHPCCNagiosToolSet::m_pNRPE).append(m_pCheckDiskSpace);

                if (CHPCCNagiosToolSet::m_bUseNPRE == false)
                {
                    strServiceConfig.append(P_NAGIOS_SEPERATOR).append(m_nDiskSpacePercentageWarning)\
                        .append(P_NAGIOS_SEPERATOR).append(m_nDiskSpacePercentageCritical);
                }

                StringBuffer strNote;
                strNote.appendf("check for %s", m_pCheckDiskSpace);
                CHPCCNagiosToolSet::generateNagiosServiceEscalationConfig(strServiceConfig, m_pCheckDiskSpace, strNote.str());

                strServiceConfig.append(P_NAGIOS_SERVICE_END_BRACKET).append("\n");
            }

            if (CHPCCNagiosToolSet::m_bCheckUsers == true)
            {
                // number of users
                strServiceConfig.append(P_NAGIOS_SERVICE_CONFIG_2).append(pHostName).append(P_NAGIOS_SERVICE_DESCRIPTION).appendf("check for %s", m_pCheckUsers)\
                        .append(P_NAGIOS_SERVICE_CHECK_COMMAND).append(CHPCCNagiosToolSet::m_pNRPE).append(m_pCheckUsers);

                if (CHPCCNagiosToolSet::m_bUseNPRE == false)
                {
                    strServiceConfig.append(P_NAGIOS_SEPERATOR).append(m_nUserNumberWarning).append(P_NAGIOS_SEPERATOR).append(m_nUserNumberCritical);
                }

                StringBuffer strNote;
                strNote.appendf("check for %s", m_pCheckUsers);
                CHPCCNagiosToolSet::generateNagiosServiceEscalationConfig(strServiceConfig, m_pCheckUsers, strNote.str());

                strServiceConfig.append(P_NAGIOS_SERVICE_END_BRACKET).append("\n");
            }

            if (CHPCCNagiosToolSet::m_bCheckLoad  == true)
            {
                // system load
                strServiceConfig.append(P_NAGIOS_SERVICE_CONFIG_2).append(pHostName).append(P_NAGIOS_SERVICE_DESCRIPTION).appendf("check for %s", m_pCheckLoad)\
                        .append(P_NAGIOS_SERVICE_CHECK_COMMAND).append(CHPCCNagiosToolSet::m_pNRPE).append(m_pCheckLoad);

                if (CHPCCNagiosToolSet::m_bUseNPRE == false)
                {
                    strServiceConfig.append(P_NAGIOS_SEPERATOR).append(m_fSystemLoad1Warn)\
                        .append(P_NAGIOS_SEPERATOR).append(m_fSystemLoad5Warn)\
                        .append(P_NAGIOS_SEPERATOR).append(m_fSystemLoad15Warn)\
                        .append(P_NAGIOS_SEPERATOR).append(m_fSystemLoad1Critical)\
                        .append(P_NAGIOS_SEPERATOR).append(m_fSystemLoad5Critical)\
                        .append(P_NAGIOS_SEPERATOR).append(m_fSystemLoad15Critical);
                }

                StringBuffer strNote;
                strNote.appendf("check for %s", m_pCheckLoad);
                CHPCCNagiosToolSet::generateNagiosServiceEscalationConfig(strServiceConfig, m_pCheckLoad, strNote.str());

                strServiceConfig.append(P_NAGIOS_SERVICE_END_BRACKET).append("\n");
            }
        }

        pch = strtok(NULL, ",\n");

        nCount++;
    }

    free(pOutput);

    return true;
}

bool CHPCCNagiosToolSet::generateNagiosThorCheckConfig(StringBuffer &strServiceConfig, const char* pEnvXML, const char* pConfigGenPath)
{
    const int nNumValues = 5;

    char *pOutput = CHPCCNagiosToolSet::invokeConfigGen(pEnvXML, pConfigGenPath, P_CONFIGGEN_PARAM_LIST_ALL, P_THOR);

    if (pOutput == NULL)
        return false;

    int i = -1;
    char pProcess[BUFFER_SIZE_3] = "";
    int nCount = 0;
    char *pch = NULL;
    pch = strtok(pOutput, ",\n");
    StringBuffer strPort;
    StringBuffer strIPAddress;
    char pHostName[BUFFER_SIZE_3] = "";
    char pProcessName[BUFFER_SIZE_3] = "";

    while (pch != NULL)
    {
        if (nCount % nNumValues ==  0) // Process name
        {
            if (*pch != 0 && strcmp(pch, XML_TAG_THORMASTERPROCESS) != 0 && strcmp(pch, XML_TAG_THORSLAVEPROCESS) != 0 && strcmp(pch, XML_TAG_THORSPAREPROCESS) != 0)
            {
                free(pOutput);
                return false;  // expecting only thor processes
            }
            else if (strcmp(pch, XML_TAG_THORSLAVEPROCESS) == 0 || strcmp(pch, XML_TAG_THORSPAREPROCESS) == 0)
            {
                for (int i = 0; i < nNumValues; i++)
                {
                    pch = strtok(NULL, ",\n");
                    nCount++;
                }
                continue;
            }
            else if (pProcess != NULL && *pProcess != 0 && strcmp(pProcess, pch) != 0)
            {
                strncpy(pProcess, pch, sizeof(pProcess));
                i = 0;
            }
            else if (pProcess == NULL || *pProcess == 0 || strcmp(pProcess, pch) != 0)
            {
                strncpy(pProcess, pch, sizeof(pProcess));
                i++;
            }
            else if (strcmp(pProcess,pch) == 0)
            {
                i++;
            }
        }
        else if (nCount % nNumValues == 1) // process name
        {
            strcpy(pProcessName,pch);
        }
        else if (nCount % nNumValues == 2) // IP Address
        {
            strIPAddress.clear().append(pch);

            struct hostent* hp = NULL;

            if (bDoLookUp == true)
            {
                unsigned int addr = inet_addr(pch);
                hp = gethostbyaddr((const char*)&addr, sizeof(addr), AF_INET);
            }

            if (hp == NULL)
            {
                bDoLookUp =  m_retryHostNameLookUp;
                strcpy(pHostName, pch);
            }
            else
            {
                strcpy(pHostName,hp->h_name);
            }
        }
        else if (nCount % nNumValues == 3)
        {
            strPort.clear().append(pch);
        }
        else if (nCount % nNumValues == 4)
        {
            VStringBuffer strNote("check for %s of type %s %s%s:%s", pProcessName, pProcess, CHPCCNagiosToolSet::m_pSeparator, strIPAddress.str(), strPort.str());

            strServiceConfig.append(P_NAGIOS_SERVICE_CONFIG_2).append(pHostName).append(P_NAGIOS_SERVICE_DESCRIPTION).append(strNote.str())\
                    .append(P_NAGIOS_SERVICE_CHECK_COMMAND).append(CHPCCNagiosToolSet::m_pNRPE).append(P_CHECK_THORMASTER).append(P_NAGIOS_SEPERATOR).append(strPort.str());

            CHPCCNagiosToolSet::generateNagiosServiceEscalationConfig(strServiceConfig, XML_TAG_THORMASTERPROCESS, strNote.str());

            strServiceConfig.append(P_NAGIOS_SERVICE_END_BRACKET).append("\n");
        }

        pch = strtok(NULL, ",\n");

        nCount++;
    }

    free(pOutput);

    return true;
}

bool CHPCCNagiosToolSet::generateNagiosDafileSrvCheckConfig(StringBuffer &strServiceConfig, const char* pEnvXML, const char* pConfigGenPath)
{
    const int nNumValues = 5;

    char *pOutput = CHPCCNagiosToolSet::invokeConfigGen(pEnvXML, pConfigGenPath, P_CONFIGGEN_PARAM_LIST_ALL, P_DAFILESRV);

    if (pOutput == NULL)
    {
        return false;
    }

    int i = -1;
    char pProcess[BUFFER_SIZE_3] = "";
    int nCount = 0;
    char *pch = NULL;
    pch = strtok(pOutput, ",\n");
    StringBuffer strIPAddress;
    char pHostName[BUFFER_SIZE_3] = "";
    char pProcessName[BUFFER_SIZE_3] = "";

    while (pch != NULL)
    {
        if (nCount % nNumValues ==  0) // Process name
        {
            if (*pch != 0 && strcmp(pch, XML_TAG_DAFILESERVERPROCESS) != 0)
            {
                free(pOutput);
                return false;  // expecting only dafilesrvprocess
            }
            else if (pProcess != NULL && *pProcess != 0 && strcmp(pProcess, pch) != 0)
            {
                strncpy(pProcess, pch, sizeof(pProcess));
                i = 0;
            }
            else if (pProcess == NULL || *pProcess == 0 || strcmp(pProcess, pch) != 0)
            {
                strncpy(pProcess, pch, sizeof(pProcess));
                i++;
            }
            else if (strcmp(pProcess,pch) == 0)
            {
                i++;
            }
        }
        else if (nCount % nNumValues == 1) // process name
        {
            strcpy(pProcessName,pch);
        }
        else if (nCount % nNumValues == 2) // IP Address
        {
            strIPAddress.clear().append(pch);

            struct hostent* hp = NULL;

            if (bDoLookUp == true)
            {
                unsigned int addr = inet_addr(pch);
                hp = gethostbyaddr((const char*)&addr, sizeof(addr), AF_INET);
            }

            if (hp == NULL)
            {
                bDoLookUp =  m_retryHostNameLookUp;
                strcpy(pHostName, pch);
            }
            else
            {
                strcpy(pHostName,hp->h_name);
            }
        }
        else if (nCount % nNumValues == 4)
        {
            StringBuffer strNote;
            strNote.appendf("check for %s of type %s", pProcessName, pProcess);

            strServiceConfig.append(P_NAGIOS_SERVICE_CONFIG_2).append(pHostName).append(P_NAGIOS_SERVICE_DESCRIPTION).append(strNote.str())\
                    .append(P_NAGIOS_SERVICE_CHECK_COMMAND).append(CHPCCNagiosToolSet::m_pNRPE).append(P_CHECK_DAFILESRV);

            CHPCCNagiosToolSet::generateNagiosServiceEscalationConfig(strServiceConfig, XML_TAG_DAFILESERVERPROCESS, strNote.str());

            strServiceConfig.append(P_NAGIOS_SERVICE_END_BRACKET).append("\n");
        }

        pch = strtok(NULL, ",\n");

        nCount++;
    }

    free(pOutput);

    return true;
}

char* CHPCCNagiosToolSet::invokeConfigGen(const char* pEnvXML, const char* pConfigGenPath, const char *pCmd, const char *pType, const char *pCmdSuffix)
{
    if (pConfigGenPath == NULL || *pConfigGenPath == 0)
    {
        pConfigGenPath = PCONFIGGEN_PATH;
    }

    if (pEnvXML == NULL || *pEnvXML == 0)
    {
        pEnvXML = PENV_XML;
    }

    if (pCmd == NULL || *pCmd == 0 || checkFileExists(pConfigGenPath) == false)
    {
        return NULL;
    }

    StringBuffer strBuff;
    StringBuffer strConfigGenCmdLine(pConfigGenPath);

    strConfigGenCmdLine.append(pCmd).append(P_CONFIGGEN_PARAM_ENVIRONMENT).append(pEnvXML);

    if (pType != NULL && *pType != 0)
    {
        strConfigGenCmdLine.append(P_BY_TYPE).append(pType);
    }
    if (pCmdSuffix != NULL && *pCmdSuffix != 0)
    {
        strConfigGenCmdLine.append(pCmdSuffix);
    }

    CHPCCNagiosToolSet::getConfigGenOutput(pEnvXML, pConfigGenPath, strConfigGenCmdLine.str(), strBuff);

    if (m_bVerbose == true)
    {
        std::cout << "configen cmd line: " << strConfigGenCmdLine.str() << std::endl;
        std::cout << "original :\n " << strBuff.str() << std::endl;
    }

    strBuff.replaceString(",,",",X,"); // sttrok pecularity with adjacent delimiters

    if (pType == NULL || *pType == 0)
    {
        strBuff.replaceString(",\n",",X\n"); // sttrok pecularity with adjacent delimiters
    }

    char *pOutput = strdup(strBuff.str());

    if (m_bVerbose == true)
    {
        std::cout << "returning: \n" << pOutput << std::endl;
    }

    return pOutput;
}

bool CHPCCNagiosToolSet::generateNagiosServiceEscalationConfig(StringBuffer &strServiceConfig, const char* pDisplayName, const char* pNote)
{
    // service escalation to eclwatch
    strServiceConfig.append(P_NAGIOS_SERVICE_DISPLAY_NAME).append(pDisplayName);
    strServiceConfig.append(P_NAGIOS_SERVICE_MAX_CHECK_ATTEMPTS).append(m_uMaxCheckAttempts);
//    strServiceConfig.append(P_NAGIOS_SERVICE_CHECK_INTERVAL).append(m_pCheckInterval);
    strServiceConfig.append(P_NAGIOS_SERVICE_CHECK_PERIOD).append(m_pCheckPeriod);
    strServiceConfig.append(P_NAGIOS_SERVICE_RETRY_INTERVAL).append(m_nRetryInteval);
    strServiceConfig.append(P_NAGIOS_SERVICE_NOTIFICATION_PERIOD).append(m_pNotificationPeriod);
    //strServiceConfig.append(P_NAGIOS_SERVICE_CONTACTS).append(m_pContacts);
    strServiceConfig.append(P_NAGIOS_SERVICE_CONTACT_GROUPS).append(m_pContactGroups);
    strServiceConfig.append(P_NAGIOS_SERVICE_NOTES).append(pNote);
    strServiceConfig.append(P_NAGIOS_SERVICE_NOTIFICATIONS_ENABLED);

    if (m_bEnableServiceEscalations == true)
    {
        strServiceConfig.append(CHPCCNagiosToolSet::m_nEnabled);
    }
    else
    {
        strServiceConfig.append(CHPCCNagiosToolSet::m_nDisabled);
    }

    strServiceConfig.append(P_NAGIOS_SERVICE_ACTIVE_CHECKS_ENABLED).append(m_nActiveChecksEnabled);
    strServiceConfig.append(P_NAGIOS_SERVICE_PASSIVE_CHECKS_ENABLED).append(m_nPassiveChecksEnabled);
    strServiceConfig.append(P_NAGIOS_SERVICE_PARALLELIZE_CHECK).append(m_nParallelizeCheck);
    strServiceConfig.append(P_NAGIOS_SERVICE_OBSESS_OVER_SERVICE).append(m_nObsessOverService);
    strServiceConfig.append(P_NAGIOS_SERVICE_CHECK_FRESHNESS).append(m_nCheckFreshness);
    strServiceConfig.append(P_NAGIOS_SERVICE_EVENT_HANDLER_ENABLED).append(m_nEventHandlerEnabled);
    strServiceConfig.append(P_NAGIOS_SERVICE_FLAP_DETECTION_ENABLED).append(m_nFlapDetectionEnabled);
    strServiceConfig.append(P_NAGIOS_SERVICE_FAILURE_PREDICTION_ENABLED).append(m_nFailurePredictionEnabled);
    strServiceConfig.append(P_NAGIOS_SERVICE_PROCESS_PERF_DATA).append(m_nProcessPerfData);
    strServiceConfig.append(P_NAGIOS_SERVICE_RETAIN_STATUS_INFORMATION).append(m_nRetainStatusInformation);
    strServiceConfig.append(P_NAGIOS_SERVICE_RETAIN_NONSTATUS_INFORMATION).append(m_nRetainNonStatusInformation);
    strServiceConfig.append(P_NAGIOS_SERVICE_NOTIFICATION_INTERVAL).append(m_nNotificationInterval);
    strServiceConfig.append(P_NAGIOS_SERVICE_IS_VOLATILE).append(m_nIsVolatile);
    strServiceConfig.append(P_NAGIOS_SERVICE_NORMAL_CHECK_INTERVAL).append(m_nNormalCheckInterval);
    strServiceConfig.append(P_NAGIOS_SERVICE_RETRY_CHECK_INTERVAL).append(m_nRetryCheckInterval);


    return true;
}

bool CHPCCNagiosToolSet::generateNagiosHostEscalationConfig(StringBuffer &strServiceConfig)
{
    if (m_bEnableHostEscalations == true)
    {
        strServiceConfig.append(P_NAGIOS_HOST_CONFIG_NOTIFICATIONS_ENABLED);
    }

    return true;
}

bool CHPCCNagiosToolSet::generateNagiosEscalationCommandConfig(StringBuffer &strCommandConfig, const StringArray &strEclWatchHostPortArray, const char *pUserMacro, const char* pPasswordMacro,
                                                               bool bUseHTTPS, bool bAppendHostPortFromDetail, const char *pURL, const char* pEnvXML, const char* pConfigGenPath)
{
    char *pOutput = CHPCCNagiosToolSet::invokeConfigGen(pEnvXML, pConfigGenPath, P_CONFIGGEN_PARAM_LIST_ESP_SERVICES);

    int i = -1;
    char pProcess[BUFFER_SIZE_3] = "";
    int nCount = 0;
    char *pch = NULL;
    pch = strtok(pOutput, ",\n");
    StringBuffer strPort;
    StringBuffer strIPAddress;
    StringArray strArrayIPPort;
    char pHostName[BUFFER_SIZE_3] = "";

    while (pch != NULL)
    {
        if (nCount % 7 ==  0) // Process type
        {
            if (*pch != 0 && strcmp(pch, XML_TAG_ESPPROCESS) != 0)
            {
                free(pOutput);
                return false;  // expecting only EspProcess
            }
            else if (pProcess != NULL && *pProcess != 0 && strcmp(pProcess, pch) != 0)
            {
                strncpy(pProcess, pch, sizeof(pProcess));
                i = 0;
            }
            else if (pProcess == NULL || *pProcess == 0 || strcmp(pProcess, pch) != 0)
            {
                strncpy(pProcess, pch, sizeof(pProcess));
                i++;
            }
            else if (strcmp(pProcess,pch) == 0)
            {
                i++;
            }
        }
        else if (nCount % 7 == 2) // service name
        {
            if (strcmp(pch, "EclWatch") != 0)
            {
                for (int c = 0; c < 4; c++)
                {
                    pch = strtok(NULL, ",\n");
                    nCount++;
                }
            }
        }
        else if (nCount % 7 == 4) // IP Address
        {
            strIPAddress.clear().append(pch);

            struct hostent* hp = NULL;

            if (bDoLookUp == true)
            {
                unsigned int addr = inet_addr(pch);
                hp = gethostbyaddr((const char*)&addr, sizeof(addr), AF_INET);
            }

            if (hp == NULL)
            {
                bDoLookUp =  m_retryHostNameLookUp;
                strcpy(pHostName, pch);
            }
            else
            {
                strcpy(pHostName,hp->h_name);
            }
        }
        else if (nCount % 7 == 5) // IP Port
        {
            strPort.clear().append(pch);
        }
        else if (nCount % 7 == 6) // protocol
        {
            StringBuffer strTemp;
            strTemp.setf(" %s:%s ", strIPAddress.str(), strPort.str());
            strArrayIPPort.append(strTemp.str());
        }

        pch = strtok(NULL, ",\n");

        nCount++;
    }

    free(pOutput);

    strCommandConfig.append(P_NAGIOS_COMMAND_CONFIG_1);
    strCommandConfig.append(P_NAGIOS_COMMAND_NAME).append(CHPCCNagiosToolSet::m_pServiceNotificatonCommand);
    strCommandConfig.append(P_NAGIOS_COMMAND_LINE).append(CHPCCNagiosToolSet::m_pSendServiceStatus);

    if (strEclWatchHostPortArray.length() > 0)
    {
        for (int i = 0; i < strEclWatchHostPortArray.length(); i++)
        {
            strCommandConfig.append(P_ECLWATCH_FLAG).append(strEclWatchHostPortArray.item(i));
        }
    }
    else
    {
        for (int i = 0; i < strArrayIPPort.length(); i++)
        {
            strCommandConfig.append(P_ECLWATCH_FLAG).append(strArrayIPPort.item(i));
        }
    }

    CHPCCNagiosToolSet::addCommonParamsToSendStatus(strCommandConfig, pUserMacro, pPasswordMacro, bUseHTTPS, bAppendHostPortFromDetail, pURL);

    strCommandConfig.appendf("%s\n", P_NAGIOS_COMMAND_END_BRACKET);

    strCommandConfig.append(P_NAGIOS_COMMAND_CONFIG_1);
    strCommandConfig.append(P_NAGIOS_COMMAND_NAME).append(CHPCCNagiosToolSet::m_pHostNotificatonCommand);
    strCommandConfig.append(P_NAGIOS_COMMAND_LINE).append(CHPCCNagiosToolSet::m_pSendHostStatus);

    if (strEclWatchHostPortArray.length() > 0)
    {
        for (int i = 0; i < strEclWatchHostPortArray.length(); i++)
        {
            strCommandConfig.append(P_ECLWATCH_FLAG).append(strEclWatchHostPortArray.item(i));
        }
    }
    else
    {
        for (int i = 0; i < strArrayIPPort.length(); i++)
        {
            strCommandConfig.append(P_ECLWATCH_FLAG).append(strArrayIPPort.item(i));
        }
    }

    CHPCCNagiosToolSet::addCommonParamsToSendStatus(strCommandConfig, pUserMacro, pPasswordMacro, bUseHTTPS, bAppendHostPortFromDetail, pURL);

    strCommandConfig.appendf("%s\n", P_NAGIOS_COMMAND_END_BRACKET);

    return true;
}

bool CHPCCNagiosToolSet::addCommonParamsToSendStatus(StringBuffer &strCommandConfig, const char* pUserMacro, const char* pPasswordMacro, const bool bUseHTTPS, const bool bAppendHostPortFromDetail,
                                                     const char  *pURL)
{
    if (pUserMacro != NULL && *pUserMacro != 0)
    {
        strCommandConfig.append(P_USER_MACRO_FLAG).append(pUserMacro);
    }
    if (pPasswordMacro != NULL && *pPasswordMacro != 0)
    {
        strCommandConfig.append(P_PASSWORD_MACRO_FLAG).append(pPasswordMacro);
    }
    if (bUseHTTPS == true)
    {
        strCommandConfig.append(P_USE_HTTPS_FLAG);
    }
    if (bAppendHostPortFromDetail == true)
    {
        strCommandConfig.append(P_SET_APPEND_PORT_FROM_DETAIL); // if we provide a port in the detail we can
                                                                // append the port to the host to generate a more unique key for notificaiton and push to HPCC
    }
    if (pURL != NULL && pURL[0] != 0)
    {
        strCommandConfig.append(P_SET_DETAIL_URL_FLAG).append(pURL).append("\n");
    }
    return true;
}

const char* CHPCCNagiosToolSet::getEspUserName(const char *pEspName, const char* pDefaultUserName)
{
    for (int i = 0; i < MAX_CUSTOM_VARS; i++)
    {
        if (strcmp(m_EspUserNamePWOverrides[i], pEspName) == 0)
            return m_pUserMacroArray[i].str();
    }
    return pDefaultUserName;
}

const char* CHPCCNagiosToolSet::getEspPassword(const char *pEspName, const char* pDefaultPassword)
{
    for (int i = 0; i < MAX_CUSTOM_VARS; i++)
    {
        if (strcmp(m_EspUserNamePWOverrides[i], pEspName) == 0)
            return m_pPasswordMacroArray[i].str();
    }
    return pDefaultPassword;
}

void CHPCCNagiosToolSet::appendHeader(StringBuffer &strBuffer)
{
    char currTime[1024];\
    time_t t = time(NULL);\
    struct tm* now = localtime(&t);\
    strftime(currTime, sizeof(currTime), "%Y-%m-%d %H:%M:%S", now);\
    StringBuffer strTemp;\
    strTemp.setf("#\n# Auto-Generated by hpcc-nagios-tools (%d.%d.%d) on %s\n# Parameters Used: %s\n#\n%s", BUILD_VERSION_MAJOR, BUILD_VERSION_MINOR, BUILD_VERSION_POINT,\
                 currTime, m_strCommandLine.str(), strBuffer.str());
    strBuffer.set(strTemp.str());
}
