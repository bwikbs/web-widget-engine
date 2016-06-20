#include "StarFishConfig.h"
#include "NetworkRequest.h"

#include "platform/file_io/FileIO.h"
#include "platform/window/Window.h"
#include "platform/threading/ThreadPool.h"
#include "dom/Document.h"
#include "platform/message_loop/MessageLoop.h"

#ifdef STARFISH_TIZEN_WEARABLE
#include <net_connection.h>
#endif

namespace StarFish {

class ActiveNetworkRequestTracker : public NetworkRequestClient {
public:
    virtual void onProgressEvent(NetworkRequest* request, bool isExplicitAction)
    {
        if (request->progressState() == NetworkRequest::LOADSTART) {
            request->document()->m_activeNetworkRequests.push_back(request);
        } else if (request->progressState() == NetworkRequest::LOADEND) {
            std::vector<NetworkRequest*, gc_allocator<NetworkRequest*>>& v = request->document()->m_activeNetworkRequests;
            auto iter = std::find(v.begin(), v.end(), request);
            if (iter != v.end())
                v.erase(iter);
        }
    }
};

void* NetworkWorkerHelper::networkWorker(void* data)
{
    NetworkWorkerData* requestData = (NetworkWorkerData*)data;
    CURL* curl = requestData->curl;
    curl_slist* list = requestData->headerList;
    auto res = curl_easy_perform(curl);
    long code;
    curl_easy_getinfo(requestData->curl, CURLINFO_RESPONSE_CODE, &code);
    requestData->responseCode = code;
    requestData->res = res;

    responseHandlerWrapper(res, requestData);

    curl_slist_free_all(list);
    curl_easy_cleanup(curl);
    return NULL;
}

void NetworkWorkerHelper::responseHandler(size_t handle, void* data)
{
    NetworkWorkerData* requestData = (NetworkWorkerData*) data;
    if (requestData->res == 0) {
        if (!requestData->isSync) {
            Locker<Mutex> locker(*requestData->request->m_mutex);
            requestData->request->m_pendingNetworkWorkerEndIdlerHandle = SIZE_MAX;
        }
        STARFISH_ASSERT(requestData->request->m_pendingNetworkWorkerEndIdlerHandle == SIZE_MAX);
        requestData->request->m_status = requestData->responseCode;
        requestData->request->handleResponseEOF();

        requestData->request->m_starFish->removePointerFromRootSet(requestData->request);

        GC_FREE(requestData);
    } else if (requestData->res == CURLE_ABORTED_BY_CALLBACK) {
        requestData->request->m_starFish->removePointerFromRootSet(requestData->request);
        GC_FREE(requestData);
    } else if (requestData->res == CURLE_OPERATION_TIMEDOUT) {
        if (!requestData->isSync) {
            Locker<Mutex> locker(*requestData->request->m_mutex);
            requestData->request->m_pendingNetworkWorkerEndIdlerHandle = SIZE_MAX;
        }
        STARFISH_ASSERT(requestData->request->m_pendingNetworkWorkerEndIdlerHandle);
        STARFISH_LOG_INFO("got timeout %s[%d]\n", requestData->request->m_url.urlString()->utf8Data(), (int)requestData->responseCode);
        requestData->request->m_status = requestData->responseCode;
        requestData->request->handleError(NetworkRequest::TIMEOUT);

    } else {
        if (!requestData->isSync) {
            Locker<Mutex> locker(*requestData->request->m_mutex);
            requestData->request->m_pendingNetworkWorkerEndIdlerHandle = SIZE_MAX;
        }
        STARFISH_ASSERT(requestData->request->m_pendingNetworkWorkerEndIdlerHandle);
        STARFISH_LOG_INFO("failed to open %s\n", requestData->request->m_url.urlString()->utf8Data());
        requestData->request->m_status = requestData->responseCode;
        requestData->request->handleError(NetworkRequest::ERROR);

        requestData->request->m_starFish->removePointerFromRootSet(requestData->request);

        GC_FREE(requestData);
    }
}

void SyncNetworkWorkHelper::responseHandlerWrapper(int res, NetworkWorkerData *requestData)
{
    responseHandler(res, requestData);
}

void AsyncNetworkWorkHelper::responseHandlerWrapper(int res, NetworkWorkerData *requestData)
{
    Locker<Mutex> locker(*requestData->request->m_mutex);
    requestData->request->m_pendingNetworkWorkerEndIdlerHandle = requestData->request->m_starFish->messageLoop()->addIdlerWithNoGCRootingInOtherThread(this->responseHandler, requestData);
}

NetworkRequest::NetworkRequest(Document* document)
    : m_starFish(document->window()->starFish())
    , m_document(document)
    , m_url(document->documentURI()) // FIXME implement empty url
    , m_readyState(UNSENT)
    , m_progressState(NONE)
    , m_method(UNKNOWN_METHOD)
    , m_responseType(DEFAULT_RESPONSE)
    , m_status(0)
    , m_timeout(0)
    , m_mutex(new Mutex())
    , m_pendingOnHeaderReceivedEventIdlerHandle(SIZE_MAX)
    , m_pendingOnProgressEventIdlerHandle(SIZE_MAX)
    , m_loaded(0)
    , m_total(0)
    , m_pendingNetworkWorkerEndIdlerHandle(SIZE_MAX)
{

    GC_REGISTER_FINALIZER_NO_ORDER(this, [] (void* obj, void* cd) {
        // STARFISH_LOG_INFO("NetworkRequest::~NetworkRequest\n");
        NetworkRequest* nr = (NetworkRequest*)obj;
        nr->m_response.clear();
        nr->m_response.shrink_to_fit();
        nr->m_responseHeaderData.clear();
        nr->m_responseHeaderData.shrink_to_fit();
    }, NULL, NULL, NULL);

    initVariables();
    addNetworkRequestClient(new ActiveNetworkRequestTracker());
}


void NetworkRequest::initVariables()
{
    m_userName = String::emptyString;
    m_password = String::emptyString;
    m_responseMimeType = String::emptyString;
    m_response.clear();
    m_responseHeaderData.clear();
    m_isAborted = false;
    m_isSync = false;
    m_isReceivedHeader = false;
    m_containsBase64Content = false;
    m_didSend = false;
    m_total = 0;
    m_loaded = 0;
    m_status = 0;
}

void NetworkRequest::clearIdlers()
{
    auto iter2 = m_requstedIdlers.begin();
    while (iter2 != m_requstedIdlers.end()) {
        m_starFish->messageLoop()->removeIdler(*iter2);
        iter2++;
    }
    m_requstedIdlers.clear();

    if (m_pendingOnHeaderReceivedEventIdlerHandle != SIZE_MAX) {
        m_starFish->messageLoop()->removeIdlerWithNoGCRooting(m_pendingOnHeaderReceivedEventIdlerHandle);
        m_pendingOnHeaderReceivedEventIdlerHandle = SIZE_MAX;
    }

    if (m_pendingOnProgressEventIdlerHandle != SIZE_MAX) {
        m_starFish->messageLoop()->removeIdlerWithNoGCRooting(m_pendingOnProgressEventIdlerHandle);
        m_pendingOnProgressEventIdlerHandle = SIZE_MAX;
    }

    if (m_pendingNetworkWorkerEndIdlerHandle != SIZE_MAX) {
        m_starFish->messageLoop()->removeIdlerWithNoGCRooting(m_pendingNetworkWorkerEndIdlerHandle);
        m_pendingNetworkWorkerEndIdlerHandle = SIZE_MAX;
    }
}

void NetworkRequest::handleResponseEOF()
{
    changeProgress(PROGRESS, true);
    changeReadyState(DONE, true);
    changeProgress(LOAD, true);
    changeProgress(LOADEND, true);
}

void NetworkRequest::handleError(ProgressState error)
{
    changeReadyState(DONE, true);
    changeProgress(PROGRESS, true);
    changeProgress(error, true);
    changeProgress(LOADEND, true);
}

// trim from start (in place)
static inline void ltrim(std::string &s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
}

// trim from end (in place)
static inline void rtrim(std::string &s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
}

// trim from both ends (in place)
static inline void trim(std::string &s)
{
    ltrim(s);
    rtrim(s);
}

// trim from start (copying)
static inline std::string ltrimmed(std::string s)
{
    ltrim(s);
    return s;
}

// trim from end (copying)
static inline std::string rtrimmed(std::string s)
{
    rtrim(s);
    return s;
}

// trim from both ends (copying)
static inline std::string trimmed(std::string s)
{
    trim(s);
    return s;
}

void NetworkRequest::changeReadyState(ReadyState readyState, bool isExplicitAction)
{
    if (readyState == HEADERS_RECEIVED) {
        std::istringstream resp(m_responseHeaderData);
        std::string header;
        std::string::size_type index;
        while (std::getline(resp, header) && header != "\r") {
            index = header.find(':', 0);
            if (index != std::string::npos) {
                std::string h = header.substr(0, index);
                std::string d = header.substr(index + 1);

                if (h == "Content-Type") {
                    std::istringstream is(d);
                    std::string part;
                    String* mimeType = String::emptyString;
                    bool hasBase64 = false;
                    bool first = true;
                    // TODO read encoding
                    while (std::getline(is, part, ';')) {
                        trim(part);
                        if (part == "base64") {
                            hasBase64 = true;
                        } else if (first) {
                            first = false;
                            mimeType = String::createASCIIString(part.data());
                        }
                    }
                    m_responseMimeType = mimeType;
                    m_containsBase64Content = hasBase64;
                }
            }
        }
    } else if (readyState == DONE) {
        if (m_containsBase64Content) {
            m_response = parseBase64String(m_response, 0, m_response.size());
        }
    }

    if (readyState != m_readyState) {
        m_readyState = readyState;
        for (size_t i = 0; i < m_clients.size(); i ++) {
            m_clients[i]->onReadyStateChange(this, isExplicitAction);
        }
    }
}

void NetworkRequest::changeProgress(ProgressState progress, bool isExplicitAction)
{
    if (m_progressState != progress || (progress == PROGRESS)) {
        m_progressState = progress;
        for (size_t i = 0; i < m_clients.size(); i ++) {
            m_clients[i]->onProgressEvent(this, isExplicitAction);
        }
    }

    if (m_progressState == LOADEND) {
        m_response.clear();
        m_response.shrink_to_fit();
        m_responseHeaderData.clear();
        m_responseHeaderData.shrink_to_fit();
    }
}

void NetworkRequest::open(MethodType method, String* url, bool async, String* userName, String* password)
{
    bool shouldAbort = false;
    {
        STARFISH_ASSERT(!(!async && m_timeout != 0));
        shouldAbort = m_progressState >= LOADSTART;
    }
    if (shouldAbort) {
        abort(true);
    }
    {
        initVariables();
        m_method = method;
        m_url = URL(m_starFish->window()->document()->documentURI().urlString(), url);
        m_userName = userName;
        m_password = password;
        m_isSync = !async;
    }
    changeReadyState(OPENED, true);
}

void NetworkRequest::abort(bool isExplicitAction)
{
    clearIdlers();

    m_isAborted = true;
    if (m_readyState >= UNSENT) {
        auto theStatusWas = m_progressState;
        if (m_readyState == OPENED && m_didSend) {
            changeProgress(ABORT, false);
        } else {
            changeProgress(ABORT, isExplicitAction);
        }

        if (theStatusWas == LOADEND && isExplicitAction) {
            changeReadyState(DONE, false);
        } else {
            changeReadyState(DONE, m_didSend);
        }
        changeProgress(LOADEND, true);
        changeReadyState(UNSENT, false);
    }
}

int NetworkRequest::curlProgressCallback(void* clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow)
{
    NetworkRequest* request = (NetworkRequest*)clientp;
    Locker<Mutex> locker(*request->m_mutex);
    // check abort
    if (request->m_isAborted)
        return 1;

    if (request->m_pendingOnHeaderReceivedEventIdlerHandle == SIZE_MAX && !request->m_isReceivedHeader) {
        request->m_isReceivedHeader = true;
        request->m_pendingOnHeaderReceivedEventIdlerHandle = request->m_starFish->messageLoop()->addIdlerWithNoGCRootingInOtherThread([](size_t handle, void* data) {
            NetworkRequest* request = (NetworkRequest*)data;
            {
                Locker<Mutex> locker(*request->m_mutex);
                STARFISH_ASSERT(handle == request->m_pendingOnHeaderReceivedEventIdlerHandle);
                request->m_pendingOnHeaderReceivedEventIdlerHandle = SIZE_MAX;
            }
            request->changeReadyState(HEADERS_RECEIVED, true);
        }, request);
    }

    request->m_loaded = static_cast<uint32_t>(dlnow);
    request->m_total = static_cast<uint32_t>(dltotal);
    if (request->m_pendingOnProgressEventIdlerHandle == SIZE_MAX) {
        request->m_pendingOnProgressEventIdlerHandle = request->m_starFish->messageLoop()->addIdlerWithNoGCRootingInOtherThread([](size_t handle, void* data) {
            NetworkRequest* request = (NetworkRequest*)data;
            {
                Locker<Mutex> locker(*request->m_mutex);
                STARFISH_ASSERT(handle == request->m_pendingOnProgressEventIdlerHandle);
                request->m_pendingOnProgressEventIdlerHandle = SIZE_MAX;
            }
            request->changeReadyState(LOADING, true);
            request->changeProgress(PROGRESS, true);
        }, request);
    }
    return 0;
}

size_t NetworkRequest::curlWriteCallback(void* ptr, size_t size, size_t nmemb, void* data)
{
    NetworkRequest* request = (NetworkRequest*)data;
    Locker<Mutex> locker(*request->m_mutex);

    size_t realSize = size * nmemb;
    const char* memPtr = (const char*)ptr;

    request->m_response.insert(request->m_response.end(), memPtr, memPtr + realSize);

    return realSize;
}

size_t NetworkRequest::curlWriteHeaderCallback(void* ptr, size_t size, size_t nmemb, void* data)
{
    size_t realsize = size * nmemb;
    NetworkRequest* request = (NetworkRequest*)data;
    Locker<Mutex> locker(*request->m_mutex);

    size_t realSize = size * nmemb;
    const char* memPtr = (const char*)ptr;

    request->m_responseHeaderData.insert(request->m_responseHeaderData.end(), memPtr, memPtr + realSize);
    return realsize;
}

void NetworkRequest::send(String* body)
{
    m_didSend = true;
    changeProgress(LOADSTART, true);
    if (m_url.isFileURL()) {
        // this area doesn't require lock.
        // reading file does not require thread
        String* path = m_url.urlStringWithoutSearchPart();
        String* filePath = path->substring(7, path->length() - 7);

        if (m_isSync) {
            fileWorker(this, filePath);
        } else {
            size_t handle = m_starFish->messageLoop()->addIdler([](size_t handle, void* data, void* data1) {
                NetworkRequest* request = (NetworkRequest*)data;
                request->removeIdlerHandle(handle);
                fileWorker((NetworkRequest*)data, (String*)data1);
            }, this, filePath);
            pushIdlerHandle(handle);
        }
    } else if (m_url.isDataURL())  {
        // this area doesn't require lock.
        // reading file does not require thread
        if (m_isSync) {
            dataURLWorker(this, m_url.urlString());
        } else {
            size_t handle = m_starFish->messageLoop()->addIdler([](size_t handle, void* data, void* data1) {
                NetworkRequest* request = (NetworkRequest*)data;
                request->removeIdlerHandle(handle);
                dataURLWorker((NetworkRequest*)data, (String*)data1);
            }, this, m_url.urlString());
            pushIdlerHandle(handle);
        }

    } else {
        NetworkWorkerData* data = new(NoGC) NetworkWorkerData;
        {
            CURL* curl = curl_easy_init();
            STARFISH_ASSERT(curl);

            data->request = this;
            data->curl = curl;
            data->isSync = m_isSync;

#ifdef STARFISH_TIZEN_WEARABLE
            connection_h connection;
            int conn_err;
            conn_err = connection_create(&connection);
            char* proxy_address = NULL;
            if (conn_err == CONNECTION_ERROR_NONE) {
                connection_get_proxy(connection, CONNECTION_ADDRESS_FAMILY_IPV4, &proxy_address);
                if (proxy_address) {
                    curl_easy_setopt(curl, CURLOPT_PROXY, proxy_address);
                    free(proxy_address);
                }
                connection_destroy(connection);
            } else {
                STARFISH_LOG_INFO("got error while opening tizen network connection\n");
            }
#endif

            const char* url = m_url.urlString()->utf8Data();
            curl_easy_setopt(curl, CURLOPT_URL, url);
            STARFISH_LOG_INFO("sending network request to %s\n", url);
            curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, static_cast<unsigned long>(m_timeout));

            std::string headerText;
            struct curl_slist* list = NULL;
            // list = curl_slist_append(list, "Accept:text/plain");
            list = curl_slist_append(list, "Accept-Charset:utf-8");
            headerText = "Accept-Language:";
            headerText += m_starFish->locale().getName();
            headerText.replace(headerText.begin(), headerText.end(), '_', '-');
            list = curl_slist_append(list, headerText.data());
            list = curl_slist_append(list, "Connection:keep-alive");
            list = curl_slist_append(list, "User-Agent: " USER_AGENT(APP_CODE_NAME, VERSION));

            if (m_document->documentURI().isFileURL() || m_document->documentURI().isDataURL()) {
                list = curl_slist_append(list, "Origin:null");
            } else {
                std::string hostString = m_url.urlString()->utf8Data();
                size_t pos = hostString.find("://");
                hostString = hostString.substr(pos + 3);
                auto pos2 = hostString.find('/');
                if (pos2 != std::string::npos) {
                    hostString = hostString.substr(0, pos2);
                }
                headerText = "Host:";
                headerText += hostString;
                list = curl_slist_append(list, headerText.data());
                headerText = "Referer:";
                headerText += m_document->documentURI().urlString()->utf8Data();
                list = curl_slist_append(list, headerText.data());
            }

            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);
            data->headerList = list;

            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
            // TODO
            // we should prevent infinite redirect
            curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 128);

            curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, curlProgressCallback);
            curl_easy_setopt(curl, CURLOPT_XFERINFODATA, this);
            curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);

            curl_easy_setopt(curl, CURLOPT_HEADERDATA, this);
            curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, curlWriteHeaderCallback);

            curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlWriteCallback);

            if (m_userName->length()) {
                curl_easy_setopt(curl, CURLOPT_USERNAME, m_userName->utf8Data());
            }
            if (m_password->length()) {
                curl_easy_setopt(curl, CURLOPT_USERPWD, m_password->utf8Data());
            }

            if (m_method == POST_METHOD) {
                curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body->utf8Data());
            } else {
                STARFISH_ASSERT(m_method == GET_METHOD);
            }

            m_starFish->addPointerInRootSet(this);
        }

        if (m_isSync) {
            data->networkWorker = new SyncNetworkWorkHelper();
            networkWorker(data);
        } else {
            data->networkWorker = new AsyncNetworkWorkHelper();
            m_starFish->threadPool()->addWork(networkWorker, data);
            // Thread* t = new Thread();
            // t->run(networkWorker, data);
        }

    }
}

void NetworkRequest::fileWorker(NetworkRequest* res, String* filePath)
{
    FileIO* fio = FileIO::create();
    if (fio->open(filePath)) {
        res->m_status = 200;
        res->changeReadyState(HEADERS_RECEIVED, true);
        res->changeReadyState(LOADING, true);
        size_t responseLength = fio->length();
        res->m_response.resize(responseLength);
        fio->read(res->m_response.data(), sizeof(const char), responseLength);
        fio->close();
        res->handleResponseEOF();
    } else {
        STARFISH_LOG_INFO("failed to open %s\n", res->m_url.urlString()->utf8Data());
        res->m_status = 0;
        res->handleError(ERROR);
    }
    delete fio;
}

void NetworkRequest::dataURLWorker(NetworkRequest* res, String* url)
{
    res->m_status = 200;

    size_t idxColon = url->indexOf(':');
    size_t idx = url->indexOf(',');

    if (idx != SIZE_MAX && idxColon != SIZE_MAX && idxColon < idx) {
        String* sub = url->substring(idxColon + 1, idx - idxColon - 1);
        res->m_responseHeaderData = "Content-Type:";
        for (size_t i = 0; i < sub->length(); i ++) {
            res->m_responseHeaderData.push_back((char)sub->charAt(i));
        }
    }
    res->changeReadyState(HEADERS_RECEIVED, true);

    res->changeReadyState(LOADING, true);
    for (size_t i = idx + 1; i < url->length(); i++) {
        char32_t c = url->charAt(i);
        // TODO filter url string correctly according RFC 3986
        if (c < 128) {
            res->m_response.push_back((char)c);
        }
    }

    res->handleResponseEOF();
}

void* NetworkRequest::networkWorker(void* data)
{
    NetworkWorkerData* requestData = (NetworkWorkerData*) data;
    return requestData->networkWorker->networkWorker(data);
}

static size_t base64Table[128] =
{
    std::string::npos, std::string::npos, std::string::npos, std::string::npos, std::string::npos, std::string::npos, std::string::npos, std::string::npos, std::string::npos, std::string::npos, // 0~9
    std::string::npos, std::string::npos, std::string::npos, std::string::npos, std::string::npos, std::string::npos, std::string::npos, std::string::npos, std::string::npos, std::string::npos, // 10~19
    std::string::npos, std::string::npos, std::string::npos, std::string::npos, std::string::npos, std::string::npos, std::string::npos, std::string::npos, std::string::npos, std::string::npos, // 20~29
    std::string::npos, std::string::npos, std::string::npos, std::string::npos, std::string::npos, std::string::npos, std::string::npos, std::string::npos, std::string::npos, std::string::npos, // 30~39
    std::string::npos, std::string::npos, std::string::npos, 62, std::string::npos, std::string::npos, std::string::npos, 63, 52, 53, // 40~49
    54, 55, 56, 57, 58, 59, 60, 61, std::string::npos, std::string::npos, // 50~59
    std::string::npos, std::string::npos, std::string::npos, std::string::npos, std::string::npos, 0, 1, 2, 3, 4, // 60~69
    5, 6, 7, 8, 9, 10, 11, 12, 13, 14, // 70~79
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, // 80~89
    25, std::string::npos, std::string::npos, std::string::npos, std::string::npos, std::string::npos, std::string::npos, 26, 27, 28, // 90~99
    29, 30, 31, 32, 33, 34, 35, 36, 37, 38, // 100~109
    39, 40, 41, 42, 43, 44, 45, 46, 47, 48, // 110~119
    49, 50, 51, std::string::npos, std::string::npos, std::string::npos, std::string::npos, std::string::npos,
};

#ifndef NDEBUG
static const std::string base64CharsDebug =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";
#endif

static inline bool isBase64(unsigned char c)
{
    return (isalnum(c) || (c == '+') || (c == '/'));
}


template <typename StrType>
NetworkRequestResponse NetworkRequest::parseBase64String(const StrType& str, size_t startAt, size_t endAt)
{
    size_t inLen = endAt - startAt;
    size_t i = 0;
    size_t j = 0;
    size_t in_ = startAt;
    unsigned char charArray4[4] = { }, charArray3[3] = { };
    NetworkRequestResponse result;

    while (inLen--) {
        if (((unsigned char) str[in_] != '=') && isBase64((unsigned char) str[in_])) {
            charArray4[i++] = str[in_];
            in_++;
            if (i == 4) {
                for (i = 0; i < 4; i++) {
#ifndef NDEBUG
                    STARFISH_ASSERT((char)base64CharsDebug.find(charArray4[i]) == (char)base64Table[charArray4[i]]);
#endif
                    charArray4[i] = base64Table[charArray4[i]];
                }

                charArray3[0] = (charArray4[0] << 2) + ((charArray4[1] & 0x30) >> 4);
                charArray3[1] = ((charArray4[1] & 0xf) << 4) + ((charArray4[2] & 0x3c) >> 2);
                charArray3[2] = ((charArray4[2] & 0x3) << 6) + charArray4[3];

                for (i = 0; (i < 3); i++) {
                    result.push_back((char)charArray3[i]);
                }
                i = 0;
            }
        }
    }

    if (i) {
        for (j = i; j < 4; j++)
            charArray4[j] = 0;
        for (j = 0; j < 4; j++) {
#ifndef NDEBUG
            auto ret = base64CharsDebug.find(charArray4[j]);
            STARFISH_ASSERT((char)base64CharsDebug.find(charArray4[j]) == (char)base64Table[charArray4[j]]);
            ret = !ret;
#endif
            charArray4[j] = base64Table[charArray4[j]];
        }

        charArray3[0] = (charArray4[0] << 2) + ((charArray4[1] & 0x30) >> 4);
        charArray3[1] = ((charArray4[1] & 0xf) << 4) + ((charArray4[2] & 0x3c) >> 2);
        charArray3[2] = ((charArray4[2] & 0x3) << 6) + charArray4[3];

        for (j = 0; (j < i - 1); j++) {
            result.push_back((char)charArray3[j]);
        }
    }

    return result;
}


}
