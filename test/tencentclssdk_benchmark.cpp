#include <string>
#include <iostream>

#include "cls/producerclient.h"
#include "cls/common.h"
#include "cls_logs.pb.h"
#include "logproducerconfig.pb.h"
#include <string>
#include <iostream>
#include "cls/result.h"
#include "cls/error.h"
#include <array>
#include <chrono>
#include <queue>
#include <mutex>
#include <condition_variable>
// using namespace chrono;
using namespace std::chrono;
using namespace tencent_log_sdk_cpp_v2;
using namespace std;


/*
���棺100MB
�ۺ�ʱ�䣺2��
�ۺ����ݰ���С��5MB
�����̣߳�1
�����̣߳�50
*/

class UserResult : public CallBack
{
public:
    UserResult() = default;
    ~UserResult() = default;
    void Success(PostLogStoreLogsResponse result) override { 
        //std::cout << result.Printf() << std::endl; 
    }
    void Fail(PostLogStoreLogsResponse result) override { 
        //std::cout << result.Printf() << std::endl; 
    }
};

double total;

// ��־���м�ͬ��ԭ��
std::queue<cls::Log> log_queue;
std::mutex queue_mutex;
std::condition_variable queue_cv;
bool finished = false;

// �������̣߳�ֻ����д����־������
void producer_thread(int send_count) {
    for (int i = 0; i < send_count; ++i) {
        cls::Log log;
        log.set_time(time(NULL));
        auto content = log.add_contents();
        content->set_key("content");
        content->set_value("this my end log");
        {
            std::lock_guard<std::mutex> lock(queue_mutex);
            log_queue.push(log);
        }
        queue_cv.notify_one();
    }
}

// �������̣߳�ֻ����Ӷ���ȡ��־������
void consumer_thread(std::shared_ptr<ProducerClient> client) {
    while (true) {
        cls::Log log;
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            queue_cv.wait(lock, [] { return !log_queue.empty() || finished; });
            if (log_queue.empty() && finished) {
                break;
            }
            if (!log_queue.empty()) {
                log = log_queue.front();
                log_queue.pop();
            } else {
                continue;
            }
        }
        std::string topic = "";
        auto callback = std::make_shared<UserResult>();
        PostLogStoreLogsResponse ret = client->PostLogStoreLogs(topic, log, callback);
        if (ret.statusCode != 0) {
            std::cout << ret.content << std::endl;
        }



    }
}

int main(int argc, char** argv)
{ 
    cls_config::LogProducerConfig config;
    config.set_endpoint("ap-guangzhou.cls.tencentyun.com");
    config.set_acceskeyid("");
    config.set_accesskeysecret("");
    
    config.set_maxsendworkercount(50);

    auto client = std::make_shared<ProducerClient>(config);
    client->Start();
    int send_count = 20000;
    int producer_count = 1;
    int consumer_count = 50;
    if (argc == 4) {
        producer_count = atoi(argv[1]);
        consumer_count = atoi(argv[2]);
        send_count = atoi(argv[3]);
    }
    std::vector<std::thread> consumers;
    std::thread producer;
    producer = std::thread(producer_thread, send_count*100);
    for (int i = 0; i < consumer_count; ++i) {
        consumers.emplace_back(consumer_thread, client);
    }
    producer.join();
    // ֪ͨ������������־��д��
    {
        std::lock_guard<std::mutex> lock(queue_mutex);
        finished = true;
    }
    queue_cv.notify_all();
    for (auto& t : consumers) {
        t.join();
    } 
    client->LogProducerEnvDestroy();
    return 0;
}