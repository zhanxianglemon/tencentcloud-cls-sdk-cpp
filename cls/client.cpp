#include "client.h"
#include "adapter.h"
#include "common.h"
#include "signature.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <iostream>
#include <curl/curl.h>

//#include <unistd.h>
//#include <sys/ioctl.h>
//#include <arpa/inet.h>
//#include <netdb.h>
//#include <sys/socket.h>
//#include <netinet/in.h>
//#include <sys/ioctl.h>
//#include <net/if.h>
//#include <net/if_arp.h>


#include <winioctl.h>
#include <winsock2.h>
#include <iphlpapi.h>
#include <ws2tcpip.h>
#include <windows.h>

#include <string>
#include <cstdlib>
#include <cstdio>

#define ETH_NAME "eth0"

#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib,"ws2_32.lib")

using namespace std;
using namespace rapidjson;

extern const char* const tencent_log_sdk_cpp_v2::LOG_SDK_IDENTIFICATION = "tencent-log-sdk-cpp v0.0.2";

static string GetHostIpByHostName()
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        return string();
    }

    char hostname[255];
    gethostname(hostname, 255);
    struct hostent *entry = gethostbyname(hostname);

    if (entry == NULL)
    {
        WSACleanup();
        return string();
    }
    struct in_addr* addr = (struct in_addr*)entry->h_addr_list[0];
    if (addr == NULL)
    {
        WSACleanup();
        return string();
    }
    char* ipaddr = inet_ntoa(*addr);
    WSACleanup();
    if (ipaddr == NULL)
    {
        return string();
    }
    return string(ipaddr);
}


//static string GetHostIpByETHName()
//{
//    int sock;
//    struct sockaddr_in sin;
//    struct ifreq ifr;
//
//    sock = socket(AF_INET, SOCK_DGRAM, 0);
//    if (sock == -1)
//    {
//        return string();
//    }
//
//    // use eth0 as the default ETH name
//    strncpy(ifr.ifr_name, "eth0", IFNAMSIZ);
//    ifr.ifr_name[IFNAMSIZ - 1] = 0;
//
//    if (ioctl(sock, SIOCGIFADDR, &ifr) < 0)
//    {
//        close(sock); //added by gaolei 13-10-09
//        return string();
//    }
//
//    memcpy(&sin, &ifr.ifr_addr, sizeof(sin));
//
//    char* ipaddr = inet_ntoa(sin.sin_addr);
//    close(sock); //added by gaolei 13-10-09
//    if (ipaddr == NULL)
//    {
//        return string();
//    }
//    return string(ipaddr);
//}
static string GetHostIpByETHName()
{
    // Windows下获取本地网卡IP地址
    ULONG bufLen = 0;
    if (GetAdaptersAddresses(AF_UNSPEC,
        GAA_FLAG_INCLUDE_GATEWAYS,
        NULL,
        NULL,
        &bufLen) != ERROR_BUFFER_OVERFLOW) {
        return string();
    }

    PIP_ADAPTER_ADDRESSES pAddresses = (PIP_ADAPTER_ADDRESSES)malloc(bufLen);
    if (pAddresses == NULL) {
        return string();
    }

    if (GetAdaptersAddresses(AF_UNSPEC,
        GAA_FLAG_INCLUDE_GATEWAYS,
        NULL,
        pAddresses,
        &bufLen) != NO_ERROR) {
        free(pAddresses);
        return string();
    }

    string ip;
    for (PIP_ADAPTER_ADDRESSES pCurr = pAddresses; pCurr != NULL; pCurr = pCurr->Next) {
        // 跳过回环适配器和未启用的适配器
        if (pCurr->IfType == IF_TYPE_SOFTWARE_LOOPBACK ||
            pCurr->OperStatus != IfOperStatusUp) {
            continue;
        }

        // 遍历所有单播地址
        for (PIP_ADAPTER_UNICAST_ADDRESS ua = pCurr->FirstUnicastAddress;
            ua != NULL;
            ua = ua->Next) {

            if (ua->Address.lpSockaddr->sa_family == AF_INET) {
                sockaddr_in* sa_in = (sockaddr_in*)ua->Address.lpSockaddr;
                char buf[INET_ADDRSTRLEN] = { 0 };
                if (inet_ntop(AF_INET, &(sa_in->sin_addr), buf, INET_ADDRSTRLEN) != NULL) {
                    // 排除回环地址
                    if (strcmp(buf, "127.0.0.1") != 0) {
                        ip = buf;
                        free(pAddresses);
                        return ip;
                    }
                }
            }
        }
    }

    free(pAddresses);
    return string(); // 没有找到合适的IP
}

static string GetHostIp()
{
    string ip = GetHostIpByHostName();
    if (ip.empty() || ip.find("127.0.0.1") != string::npos)
    {
        return GetHostIpByETHName();
    }
    return ip;
}


namespace tencent_log_sdk_cpp_v2
{

/************************ common method ***********************/
/************************ json method *************************/
void ExtractJsonResult(const string& response, rapidjson::Document& document)
{
    document.Parse(response.c_str());
    if (document.HasParseError())
    {
        throw JsonException("ParseException", "Fail to parse from json string");
    }
}

void JsonMemberCheck(const rapidjson::Value& value, const char* name)
{
    if (!value.IsObject())
    {
        throw JsonException("InvalidObjectException", "response is not valid JSON object");
    }
    if (!value.HasMember(name))
    {
        throw JsonException("NoMemberException", string("Member ") + name + " does not exist");
    }
}

void ExtractJsonResult(const rapidjson::Value& value, const char* name, int32_t& number)
{
    JsonMemberCheck(value, name);
    if (value[name].IsInt())
    {
        number = value[name].GetInt();
    }
    else
    {
        throw JsonException("ValueTypeException", string("Member ") + name + " is not int type");
    }
}

void ExtractJsonResult(const rapidjson::Value& value, const char* name, uint32_t& number)
{
    JsonMemberCheck(value, name);
    if (value[name].IsUint())
    {
        number = value[name].GetUint();
    }
    else
    {
        throw JsonException("ValueTypeException", string("Member ") + name + " is not uint type");
    }
}

void ExtractJsonResult(const rapidjson::Value& value, const char* name, int64_t& number)
{
    JsonMemberCheck(value, name);
    if (value[name].IsInt64())
    {
        number = value[name].GetInt64();
    }
    else
    {
        throw JsonException("ValueTypeException", string("Member ") + name + " is not int type");
    }
}

void ExtractJsonResult(const rapidjson::Value& value, const char* name, uint64_t& number)
{
    JsonMemberCheck(value, name);
    if (value[name].IsUint64())
    {
        number = value[name].GetUint64();
    }
    else
    {
        throw JsonException("ValueTypeException", string("Member ") + name + " is not uint type");
    }
}

void ExtractJsonResult(const rapidjson::Value& value, const char* name, bool& boolean)
{
    JsonMemberCheck(value, name);
    if (value[name].IsBool())
    {
        boolean = value[name].GetBool();
    }
    else
    {
        throw JsonException("ValueTypeException", string("Member ") + name + " is not boolean type");
    }
}

void ExtractJsonResult(const rapidjson::Value& value, const char* name, string& dst)
{
    JsonMemberCheck(value, name);
    if (value[name].IsString())
    {
        dst = value[name].GetString();
    }
    else
    {
        throw JsonException("ValueTypeException", string("Member ") + name + " is not string type");
    }
}

const rapidjson::Value& GetJsonValue(const rapidjson::Value& value, const char* name)
{
    JsonMemberCheck(value, name);
    if (value[name].IsObject() || value[name].IsArray())
    {
        return value[name];
    }
    else
    {
        throw JsonException("ValueTypeException", string("Member ") + name + " is not json value type");
    }
}


static void ErrorCheck(const string& response, const string& requestId, const int32_t httpCode)
{
    rapidjson::Document document;
    try
    {
        ExtractJsonResult(response, document);

        string errorCode;
        ExtractJsonResult(document, LOG_ERROR_CODE, errorCode);

        string errorMessage;
        ExtractJsonResult(document, LOG_ERROR_MESSAGE, errorMessage);
        throw LOGException(errorCode, errorMessage, requestId, httpCode);
    }
    catch (JsonException& e)
    {
        if (httpCode == 500)
        {
            throw LOGException(LOGE_INTERNAL_SERVER_ERROR, response, requestId, httpCode);
        }
        else
        {
            throw LOGException(LOGE_BAD_RESPONSE, string("Unextractable error:") + response, requestId, httpCode);
        }
    }
}


LOGClient::LOGClient(const std::string& clsHost, const std::string& accessKeyId, const std::string& accessKey, int64_t timeout,
              int64_t connecttimeout,const std::string& token,const std::string& source, bool compressFlag)
    : mClsHost(clsHost), mAccessKeyId(accessKeyId), mAccessKey(accessKey), mSource(source), mCompressFlag(compressFlag),
      mTimeout(timeout), mConnectTimeout(connecttimeout), mSecurityToken(token),mUserAgent(LOG_SDK_IDENTIFICATION),
      mGetDateString(CodecTool::GetDateString), mLOGSend(LOGAdapter::Send)
{
    //pthread_mutex_init(&mMutexLock, NULL);
    InitializeSRWLock(&mMutexLock);
    SetClsHost(clsHost);
    if (mSource == "")
    {
        mSource = GetHostIp();
    }
    if (mTimeout <= 0)
    {
        mTimeout = LOG_REQUEST_TIMEOUT;
    }

    //发送频率
    mMaxSendSpeedInBytePerSec = 1024 * 1024 * 1024;
}

LOGClient::~LOGClient()
{
    //pthread_mutex_destroy(&mMutexLock);
}


string LOGClient::GetClsHost()
{
    //共享锁
    //pthread_mutex_lock(&mMutexLock);
    AcquireSRWLockShared(&mMutexLock);
    string clsHost = mClsHost;
    //pthread_mutex_unlock(&mMutexLock);
    ReleaseSRWLockShared(&mMutexLock);
    return clsHost;
}

void LOGClient::SetClsHost(const string& clsHost)
{
    //排他锁
    //pthread_mutex_lock(&mMutexLock);
    AcquireSRWLockExclusive(&mMutexLock);
    size_t bpos = clsHost.find("://");
    if (bpos == string::npos)
        bpos = 0;
    else
        bpos += 3;
    string tmpstr = clsHost.substr(bpos);
    size_t epos = tmpstr.find_first_of("/");
    if (epos == string::npos)
        epos = tmpstr.length();
    string host = tmpstr.substr(0, epos);

    mClsHost = host;
    //pthread_mutex_unlock(&mMutexLock);
    ReleaseSRWLockExclusive(&mMutexLock);
}

void LOGClient::SetCommonHeader(map<string, string>& httpHeader, int32_t contentLength)
{
    httpHeader[HOST] = GetClsHost();
    httpHeader[USER_AGENT] = mUserAgent;
    httpHeader[CONTENT_TYPE] = TYPE_LOG_PROTOBUF;
}

void LOGClient::SetCommonParameter(map<string, string>& parameterList) {}

void LOGClient::SendRequest(const string& httpMethod, const string& url, const string& body,
                            const map<string, string>& parameterList, map<string, string>& header,
                            HttpMessage& httpMessage)
{
    string host = GetClsHost();
    SetCommonHeader(header, body.length());
    std::string sign = signature(mAccessKeyId, mAccessKey, httpMethod, url, parameterList, header, 300);
    header[AUTHORIZATION] = sign;
    if (!mSecurityToken.empty()){
        header[CLS_TOKEN] = mSecurityToken;
    }

    string queryString;
    LOGAdapter::GetQueryString(parameterList, queryString);

    mLOGSend(httpMethod, host, 80, url, queryString, header, body, mTimeout, mConnectTimeout, httpMessage,
             mMaxSendSpeedInBytePerSec);
}

PostLogStoreLogsResponse LOGClient::PostLogStoreLogs(const std::string& topic, const cls::LogGroup& loggroup_)
{
    if (LogSizeCalculate(loggroup_) != 0)
    {
        throw LOGException(ERR_LIMIT_LOG_SIZE, "error log size limit");
    }
    string body = "";
    string serializeData = "";
    cls::LogGroup pbLogGroup;
    pbLogGroup = loggroup_;
    if ((loggroup_.source()).empty())
    {
        pbLogGroup.set_source(mSource);
    }

    cls::LogGroupList loggrouplst;
    auto plogGroup = loggrouplst.add_loggrouplist();
    plogGroup->CopyFrom(pbLogGroup);
    loggrouplst.SerializeToString(&serializeData);

    string operation = STRUCTUREDLOG; //请求uri
    map<string, string> httpHeader;
    if (mCompressFlag)
    {
        if (!tencent_log_sdk_cpp_v2::CompressAlgorithm::CompressLz4(serializeData, body))
        {
            throw LOGException(LOGE_UNKNOWN_ERROR, "Data compress failed.");
        }
        httpHeader[CLS_LOG_COMPRESSTYPE] = LOG_LZ4;
    }
    else
    {
        body = serializeData;
    }

    map<string, string> parameterList;

    parameterList["topic_id"] = topic;

    SetCommonParameter(parameterList);

    HttpMessage httpResponse;
    SendRequest(HTTP_POST, operation, body, parameterList, httpHeader, httpResponse);
    PostLogStoreLogsResponse ret;
    ret.bodyBytes = body.size();
    ret.statusCode = httpResponse.statusCode;
    ret.requestId = httpResponse.header[CLS_LOG_REQUEST_ID];
    ret.content = httpResponse.content;
    ret.header = httpResponse.header;
    return ret;
}

int LOGClient::LogSizeCalculate(const cls::LogGroup& loggroup_)
{
    int ret = 0;
    int64_t sizeInBytes = 0;
    for (int logindex = 0; logindex < loggroup_.logs_size(); ++logindex)
    {
        for (int contentindex = 0; contentindex < loggroup_.logs(logindex).contents_size(); ++contentindex)
        {
            if (loggroup_.logs(logindex).contents(contentindex).value().size() > 1 * 1024 * 1024)
            {
                return ERRCODE_LIMIT_LOG_SIZE;
            }
            sizeInBytes += loggroup_.logs(logindex).contents(contentindex).value().size();
            sizeInBytes += loggroup_.logs(logindex).contents(contentindex).key().size();
        }
    }

    if (sizeInBytes > 1024 * 1024 * 5)
    {
        return ERRCODE_LIMIT_LOG_SIZE;
    }
    return ret;
}

} // namespace tencent_log_sdk_cpp_v2
