#include <gtest/gtest.h>
#include <benchmark/benchmark.h>
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
using namespace tencent_log_sdk_cpp_v2;
using namespace std;


class UserResult : public CallBack
{
public:
    UserResult() = default;
    ~UserResult() = default;
    void Success(PostLogStoreLogsResponse result) override { std::cout << result.Printf() << std::endl; }
    void Fail(PostLogStoreLogsResponse result) override { std::cout << result.Printf() << std::endl; }
};
 /*
 ���ԣ����ո��ؾ���ģʽ�ϴ���־
 Ԥ�ڽ����ret.statusCode����200ok
 */
  TEST(tencent_log_sdk_test, load_balancing)
  {
      std::cout << "ProducerClient��ʼ����" << endl;
      cls_config::LogProducerConfig config;
      config.set_endpoint("");
      config.set_acceskeyid("");
      config.set_accesskeysecret("");
      // config.set_lingerms(0);
      auto client = std::make_shared<ProducerClient>(config);
      client->Start();
      cls::Log log;
      log.set_time(time(NULL));
      auto content = log.add_contents();
      content->set_key("content");
      content->set_value("this my test log");
      std::string topic = "";
      auto callback = std::make_shared<UserResult>();
      for (int i = 0; i < 2; ++i)
      {
          PostLogStoreLogsResponse ret = client->PostLogStoreLogs(topic, log, callback);
          std::cout << ret.Printf() << std::endl;
      }
      client->LogProducerEnvDestroy();
  }


 /*
 ���ԣ����úϷ���У�飬���ڴ�С�ڵ���0
 Ԥ�ڽ������ʹ��Ĭ��ֵtotalsizelnbytes=104857600
 */
 TEST(tencent_log_sdk_test, config_totalsizelnbytes)
 {
     cls_config::LogProducerConfig config;
     config.set_endpoint("");
     config.set_acceskeyid("");
     config.set_accesskeysecret("");
     config.set_totalsizelnbytes(0);
     auto client = std::make_shared<ProducerClient>(config);
     EXPECT_EQ(config.totalsizelnbytes(), 104857600);
 }

 /*
 ���ԣ����úϷ���У�飬������С��0
 Ԥ�ڽ������ʹ��Ĭ�ϵĲ�������maxsendworkercount=50
 */
 TEST(tencent_log_sdk_test, config_maxsendworkercount)
 {
     cls_config::LogProducerConfig config;
     config.set_endpoint("");
     config.set_acceskeyid("");
     config.set_accesskeysecret("");
     config.set_maxsendworkercount(0);
     auto client = std::make_shared<ProducerClient>(config);
     EXPECT_EQ(config.maxsendworkercount(), 50);
 }

 /*
 ���ԣ����úϷ���У�飬�ۼƵ�batchsizeС�ڵ���0
 Ԥ�ڽ������ʹ��Ĭ��ֵmaxbatchsize=5M
 */
 TEST(tencent_log_sdk_test, config_maxbatchsize_0)
 {
     cls_config::LogProducerConfig config;
     config.set_endpoint("");
     config.set_acceskeyid("");
     config.set_accesskeysecret("");
     config.set_maxbatchsize(0);
     auto client = std::make_shared<ProducerClient>(config);
     EXPECT_EQ(config.maxbatchsize(), 5242880);
 }

 /*
 ���ԣ����úϷ���У�飬�ۼƵ�batchsize��С����5M
 Ԥ�ڽ������ʹ��Ĭ��ֵtotalsizelnbytes=104857600
 */
 TEST(tencent_log_sdk_test, config_maxbatchsize_max)
 {
     cls_config::LogProducerConfig config;
     config.set_endpoint("");
     config.set_acceskeyid("");
     config.set_accesskeysecret("");
     config.set_maxbatchsize(5242881);
     auto client = std::make_shared<ProducerClient>(config);
     EXPECT_EQ(config.maxbatchsize(), 5242880);
 }

 /*
 ���ԣ����úϷ���У�飬����ʱ��С��100ms
 Ԥ�ڽ������ʹ��Ĭ��ֵlingerms=2000
 */
 TEST(tencent_log_sdk_test, config_lingerms)
 {
     cls_config::LogProducerConfig config;
     config.set_endpoint("");
     config.set_acceskeyid("");
     config.set_accesskeysecret("");
     config.set_lingerms(80);
     auto client = std::make_shared<ProducerClient>(config);
     EXPECT_EQ(config.lingerms(), 2000);
 }

 /*
 ���ԣ����úϷ���У�飬�״����Ե��˱�ʱ��С�ڵ���0
 Ԥ�ڽ������ʹ��Ĭ��ֵbaseretrybackoffms=100
 */
 TEST(tencent_log_sdk_test, config_baseretrybackoffms)
 {
     cls_config::LogProducerConfig config;
     config.set_endpoint("");
     config.set_acceskeyid("");
     config.set_accesskeysecret("");
     config.set_baseretrybackoffms(0);
     auto client = std::make_shared<ProducerClient>(config);
     EXPECT_EQ(config.baseretrybackoffms(), 100);
 }

 /*
 ���ԣ����úϷ���У�飬���Ե�����˱�ʱ��С�ڵ���0
 Ԥ�ڽ������ʹ��Ĭ��ֵmaxretrybackoffms=50ms
 */
 TEST(tencent_log_sdk_test, config_maxretrybackoffms)
 {
     cls_config::LogProducerConfig config;
     config.set_endpoint("");
     config.set_acceskeyid("");
     config.set_accesskeysecret("");
     config.set_maxretrybackoffms(0);
     auto client = std::make_shared<ProducerClient>(config);
     EXPECT_EQ(config.maxretrybackoffms(), 50000);
 }

 /*
 ���ԣ�δ���õ�����
 Ԥ�ڽ��������Ĭ��ֵ
 */
 TEST(tencent_log_sdk_test, config_default)
 {
     cls_config::LogProducerConfig config;
     config.set_endpoint("");
     config.set_acceskeyid("");
     config.set_accesskeysecret("");
     auto client = std::make_shared<ProducerClient>(config);
     EXPECT_EQ(config.totalsizelnbytes(), 104857600);
     EXPECT_EQ(config.maxsendworkercount(), 50);
     EXPECT_EQ(config.maxblocksec(), 60);
     EXPECT_EQ(config.maxbatchsize(), 5242880);
     EXPECT_EQ(config.lingerms(), 2000);
     EXPECT_EQ(config.retries(), 10);
     EXPECT_EQ(config.baseretrybackoffms(), 100);
     EXPECT_EQ(config.maxretrybackoffms(), 50000);
     EXPECT_EQ(config.compressflag(), 1);
     EXPECT_EQ(config.baseretrybackoffms(), 100);
     EXPECT_EQ(config.baseretrybackoffms(), 100);
 }

 /*
 ���ԣ�TotalSizeLnBytes��Сʹ��Ĭ��ֵ100M���ߣ���ʹ��Ĭ��ֵ
 MaxBlockSec=0s.�û�ʹ�õ�
 memtotalsize_����TotalSizeLnBytes
 Ԥ�ڣ����ش����룬�������İ���ϢTotalSizeLnBytes limit �ڴ��С����
 */
 TEST(tencent_log_sdk_test, limit_totalsizelnbyte_0)
 {
     cls_config::LogProducerConfig config;
     config.set_endpoint("");
     config.set_acceskeyid("");
     config.set_accesskeysecret("");
     config.set_totalsizelnbytes(10); //�����ڴ�Ĵ�Сh��10K
     config.set_maxblocksec(0); //��ֵ��Ϊ0��������ռ��޷��õ�����ʱ����������
     auto client = std::make_shared<ProducerClient>(config);
     client->Start();
     cls::Log log;
     log.set_time(time(NULL));
     for (int i = 0; i < 10; ++i)
     {
         auto content = log.add_contents();
         content->set_key("content");
         content->set_value("this my test log");
     }
     std::string topic = "";
     auto callback = std::make_shared<UserResult>();
     client->PostLogStoreLogs(topic, log, callback);
     PostLogStoreLogsResponse ret = client->PostLogStoreLogs(topic, log, callback);
     EXPECT_EQ(ret.statusCode, 2);
     EXPECT_STREQ(ret.content.c_str(), "TotalSizeLnBytes limit");
     Sleep(3); //ģ��3s����������ʱ�����
     client->LogProducerEnvDestroy();
 }

 /*
 ���ԣ�TotalSizeLnBytes��Сʹ��Ĭ��ֵ100M���ߣ���ʹ��Ĭ��ֵ
 MaxBlockSec=60s.�û�ʹ�õ�
 memtotalsize_����TotalSizeLnBytes
 Ԥ�ڣ��ȴ�60s֮�󷵻ش����룬�������İ���ϢTotalSizeLnBytes limit �ڴ��С����
 */
 TEST(tencent_log_sdk_test, limit_totalsizelnbyte_default)
 {
     cls_config::LogProducerConfig config;
     config.set_endpoint("");
     config.set_acceskeyid("");
     config.set_accesskeysecret("");
     config.set_totalsizelnbytes(10); //�����ڴ�Ĵ�Сh��10K
     config.set_maxblocksec(3); //��ֵ��Ϊ0��������ռ��޷��õ�����ʱ����������
     auto client = std::make_shared<ProducerClient>(config);
     client->Start();
     cls::Log log;
     log.set_time(time(NULL));
     for (int i = 0; i < 10; ++i)
     {
         auto content = log.add_contents();
         content->set_key("content");
         content->set_value("this my test log");
     }
     std::string topic = "";
     auto callback = std::make_shared<UserResult>();
     client->PostLogStoreLogs(topic, log, callback);
     PostLogStoreLogsResponse ret = client->PostLogStoreLogs(topic, log, callback);
     EXPECT_EQ(ret.statusCode, 2);
     EXPECT_STREQ(ret.content.c_str(), "TotalSizeLnBytes limit");
     Sleep(5); //ģ��10s����������ʱ�����
     client->LogProducerEnvDestroy();
 }

 /*
 ���ԣ�TotalSizeLnBytes��Сʹ��Ĭ��ֵ100M���ߣ��û����õ�MaxBlockSec = -1
 memtotalsize_����TotalSizeLnBytes
 Ԥ�ڣ�һֱ�ȴ���ֱ���ڴ��ͷţ�����������ʱ���,��ǰģ��ȴ�3s��Ȼ������������ݣ�
 */
 TEST(tencent_log_sdk_test, limit_totalsizelnbyte_block)
 {
     cls_config::LogProducerConfig config;
     config.set_endpoint("");
     config.set_acceskeyid("");
     config.set_accesskeysecret("");
     config.set_totalsizelnbytes(10); //�����ڴ�Ĵ�Сh��10K
     config.set_maxblocksec(-1); //��ֵ��Ϊ0��������ռ��޷��õ�����ʱ����������
     config.set_lingerms(3000);
     auto client = std::make_shared<ProducerClient>(config);
     client->Start();
     cls::Log log;
     log.set_time(time(NULL));
     for (int i = 0; i < 10; ++i)
     {
         auto content = log.add_contents();
         content->set_key("content");
         content->set_value("this my test log");
     }
     std::string topic = "";
     auto callback = std::make_shared<UserResult>();
     client->PostLogStoreLogs(topic, log, callback);
     PostLogStoreLogsResponse ret = client->PostLogStoreLogs(topic, log, callback);
     EXPECT_EQ(ret.statusCode, 0);
     Sleep(8); //ģ��10s����������ʱ�����
     client->LogProducerEnvDestroy();
 }


/*
���ԣ�����shutdown
Ԥ�ڣ��޷��������,���ش�����ERR_CLS_SDK_TASK_SHUTDOWN
*/
 TEST(tencent_log_sdk_test, shutdown)
 {
     cls_config::LogProducerConfig config;
     config.set_endpoint("");
     config.set_acceskeyid("");
     config.set_accesskeysecret("");
     config.set_maxbatchsize(300);
     auto client = std::make_shared<ProducerClient>(config);
     client->Start();
     cls::Log log;
     log.set_time(time(NULL));
     for (int i = 0; i < 10; ++i)
     {
         auto content = log.add_contents();
         content->set_key("content");
         content->set_value("this my test log");
     }
     std::string topic = "";
     auto callback = std::make_shared<UserResult>();
     client->PostLogStoreLogs(topic, log, callback);
     PostLogStoreLogsResponse ret = client->PostLogStoreLogs(topic, log, callback);
     client->LogProducerEnvDestroy();
     EXPECT_EQ(ret.statusCode, 3);
 }

/*
���Գ�����AddLogToProducerBatch�������topic�����ҵ������������maxbatchsize����С��5M
Ԥ�ڽ�����������������ݡ�����֧�־ۺ�
*/

 TEST(tencent_log_sdk_test, maxbatchsize_0)
 {
     cls_config::LogProducerConfig config;
     config.set_endpoint("");
     config.set_acceskeyid("");
     config.set_accesskeysecret("");
     config.set_maxbatchsize(300);
     auto client = std::make_shared<ProducerClient>(config);
     client->Start();
     cls::Log log;
     log.set_time(time(NULL));
     for (int i = 0; i < 10; ++i)
     {
         auto content = log.add_contents();
         content->set_key("content");
         content->set_value("this my test log");
     }
     std::string topic = "";
     auto callback = std::make_shared<UserResult>();
     client->PostLogStoreLogs(topic, log, callback);
     PostLogStoreLogsResponse ret = client->PostLogStoreLogs(topic, log, callback);
     client->LogProducerEnvDestroy();
     EXPECT_EQ(ret.statusCode, 0);
 }

/*
���Գ�����AddLogToProducerBatch�������topic�����ҵ������δ����maxbatchsize
Ԥ�ڣ�ֻ��������ݵ�ma�У��ȴ����ݳ���maxbatchsize�������ݣ����ߴﵽ����ʱ����������
*/
 TEST(tencent_log_sdk_test, maxbatchsize_1)
 {
     cls_config::LogProducerConfig config;
     config.set_endpoint("");
     config.set_acceskeyid("");
     config.set_accesskeysecret("");
     config.set_maxbatchsize(1024 * 1024); //�������ó�1M
     auto client = std::make_shared<ProducerClient>(config);
     client->Start();
     cls::Log log;
     log.set_time(time(NULL));
     for (int i = 0; i < 10; ++i)
     {
         auto content = log.add_contents();
         content->set_key("content");
         content->set_value("this my test log");
     }
     std::string topic = "";
     auto callback = std::make_shared<UserResult>();
     PostLogStoreLogsResponse ret = client->PostLogStoreLogs(topic, log, callback);
     Sleep(3);
     client->LogProducerEnvDestroy();
     EXPECT_EQ(ret.statusCode, 0);
 }

/*
���Գ�����AddLogToProducerBatch�������topic�����ҵ����������5M(�ӳ�������ʱ�䣬��Ӱ�쵱ǰ����)
Ԥ�ڣ��Ȱ�֮ǰ�����ݷ�����ɣ������´����µ�batchloggroup
*/
 TEST(tencent_log_sdk_test, maxbatchsize_2)
 {
     cls_config::LogProducerConfig config;
     config.set_endpoint("");
     config.set_acceskeyid("");
     config.set_accesskeysecret("");
     // config.set_maxbatchsize(1024 * 1024); //�������ó�1M
     config.set_lingerms(10000);
     auto client = std::make_shared<ProducerClient>(config);
     client->Start();
     cls::Log log;
     log.set_time(time(NULL));
     // for (int i = 0; i < 10; ++i)
     // {
     //     auto content = log.add_contents();
     //     content->set_key("content");
     //     content->set_value("this my test log");
     // }
     std::string topic = "";
     auto callback = std::make_shared<UserResult>();
     // PostLogStoreLogsResponse ret = client->PostLogStoreLogs(topic, log, callback);
     auto content = log.add_contents();
     content->set_key("content");
     std::string tmp = "this my test log";
     for (int i = 0; i < 130000; ++i)
     {
         tmp += "add data";
     }
     content->set_value(tmp);
     PostLogStoreLogsResponse ret = client->PostLogStoreLogs(topic, log, callback);
     ret = client->PostLogStoreLogs(topic, log, callback);
     ret = client->PostLogStoreLogs(topic, log, callback);
     ret = client->PostLogStoreLogs(topic, log, callback);
     ret = client->PostLogStoreLogs(topic, log, callback);
     ret = client->PostLogStoreLogs(topic, log, callback);
     ret = client->PostLogStoreLogs(topic, log, callback);
     Sleep(5);
     client->LogProducerEnvDestroy();
     // EXPECT_EQ(ret.statusCode, 0);
 }


/*
���Գ�����AddLogToProducerBatchΪ����maxbatchsize,���Ƕ���ʱ�䴥��
Ԥ�ڣ���������ӵ��̳߳���
*/
 TEST(tencent_log_sdk_test, linems_1)
 {
     cls_config::LogProducerConfig config;
     config.set_endpoint("");
     config.set_acceskeyid("");
     config.set_accesskeysecret("");
     // config.set_maxbatchsize(1024 * 1024); //�������ó�1M
     config.set_lingerms(500);
     auto client = std::make_shared<ProducerClient>(config);
     client->Start();
     cls::Log log;
     log.set_time(time(NULL));
     std::string topic = "";
     auto callback = std::make_shared<UserResult>();
     // PostLogStoreLogsResponse ret = client->PostLogStoreLogs(topic, log, callback);
     auto content = log.add_contents();
     content->set_key("content");
     std::string tmp = "this my test log";
     for (int i = 0; i < 130000; ++i)
     {
         tmp += "add data";
     }
     content->set_value(tmp);
     PostLogStoreLogsResponse ret = client->PostLogStoreLogs(topic, log, callback);
     ret = client->PostLogStoreLogs(topic, log, callback);
     ret = client->PostLogStoreLogs(topic, log, callback);
     Sleep(5);
     client->LogProducerEnvDestroy();
     EXPECT_EQ(ret.statusCode, 0);
 }

/*
���Գ�����AddLogToProducerBatchΪ����maxbatchsize�����Ҷ���ʱ�䴥��
Ԥ�ڣ���������ӵ��̳߳��������ݲ�����
*/
 TEST(tencent_log_sdk_test, linems_2)
 {
     cls_config::LogProducerConfig config;
     config.set_endpoint("");
     config.set_acceskeyid("");
     config.set_accesskeysecret("");
     // config.set_maxbatchsize(1024 * 1024); //�������ó�1M
     config.set_lingerms(1);
     auto client = std::make_shared<ProducerClient>(config);
     client->Start();
     cls::Log log;
     log.set_time(time(NULL));
     std::string topic = "";
     auto callback = std::make_shared<UserResult>();
     // PostLogStoreLogsResponse ret = client->PostLogStoreLogs(topic, log, callback);
     auto content = log.add_contents();
     content->set_key("content");
     std::string tmp = "this my test log";
     for (int i = 0; i < 130000; ++i)
     {
         tmp += "add data";
     }
     content->set_value(tmp);
     PostLogStoreLogsResponse ret = client->PostLogStoreLogs(topic, log, callback);
     for (int i = 0; i < 10; ++i)
     {
         Sleep(1);
         ret = client->PostLogStoreLogs(topic, log, callback);
     }
     Sleep(5);
     client->LogProducerEnvDestroy();
     EXPECT_EQ(ret.statusCode, 0);
 }

/*
���Գ��������ݷ���ʧ�ܣ����뵽���Զ�����
Ԥ�ڣ����ݰ��մ�С�����������
*/
 TEST(tencent_log_sdk_test, retry_1)
 {
     cls_config::LogProducerConfig config;
     config.set_endpoint("");
     config.set_acceskeyid("");
     config.set_accesskeysecret("");
     // config.set_maxbatchsize(1024 * 1024); //�������ó�1M
     config.set_lingerms(1);
     auto client = std::make_shared<ProducerClient>(config);
     client->Start();
     cls::Log log;
     log.set_time(time(NULL));
     std::string topic = "0b31137-c547-4676-a19e-eaf1e5888921";
     auto callback = std::make_shared<UserResult>();
     // PostLogStoreLogsResponse ret = client->PostLogStoreLogs(topic, log, callback);
     auto content = log.add_contents();
     content->set_key("content");
     std::string tmp = "this my test log";
     for (int i = 0; i < 130000; ++i)
     {
         tmp += "add data";
     }
     content->set_value(tmp);
     PostLogStoreLogsResponse ret = client->PostLogStoreLogs(topic, log, callback);
     for (int i = 0; i < 10; ++i)
     {
         ret = client->PostLogStoreLogs(topic, log, callback);
         Sleep(2);
     }
     Sleep(20);
     client->LogProducerEnvDestroy();
     EXPECT_EQ(ret.statusCode, 0);
 }


/*
���Գ��������ݷ���ʧ�ܣ����뵽���Զ�����,����retryes������ʧ��
Ԥ�ڣ��ص��û�ʧ��
*/
TEST(tencent_log_sdk_test, retry_2)
{
    cls_config::LogProducerConfig config;
    config.set_endpoint("");
    config.set_acceskeyid("");
    config.set_accesskeysecret("");
    // config.set_maxbatchsize(1024 * 1024); //�������ó�1M
    config.set_lingerms(1);
    config.set_retries(5);
    auto client = std::make_shared<ProducerClient>(config);
    client->Start();
    cls::Log log;
    log.set_time(time(NULL));
    std::string topic = "0b31137-c547-4676-a19e-eaf1e5888921";
    auto callback = std::make_shared<UserResult>();
    // PostLogStoreLogsResponse ret = client->PostLogStoreLogs(topic, log, callback);
    auto content = log.add_contents();
    content->set_key("content");
    std::string tmp = "this my test log";
    for (int i = 0; i < 130000; ++i)
    {
        tmp += "add data";
    }
    content->set_value(tmp);
    PostLogStoreLogsResponse ret = client->PostLogStoreLogs(topic, log, callback);
    Sleep(20);
    client->LogProducerEnvDestroy();
    EXPECT_EQ(ret.statusCode, 0);
}


int main(int argc, char** argv) {

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();

    return 0;
}