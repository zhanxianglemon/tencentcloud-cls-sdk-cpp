# 描述

tencent-log-sdk-cpp 是C++用户日志结构化上传的SDK，采用轻量的设计模式，简单易用可快速上手、支持同步模式以及异步模式。具体参见如下的demo

## 如何使用

### 依赖（具体安装见末尾）

- 使用vs2022或其他版本
- 使用vcpkg进行统一管理进行安装
- `protobuf` 
  - PB 描述文件是通信双方约定的数据交换格式，上传日志时须将规定的协议格式编译成对应语言版本的调用接口，然后添加到工程代码里。
- curl 可使用系统自带的工具或者自己安装即可
- boost 依赖boot库
- `openssl` 签名的计算依赖openssl,这里建议使用[官网](https://www.openssl.org/source/) 提供的1.1.1版本，签名的方式参见[文档](https://cloud.tencent.com/document/product/614/12445)
测试依赖：
- `gtest`
- `benchmark`

### 异步模式


#### 使用步骤

- step1:使用vs打开代码目录
- step2:配置release编译器
- step3:生产解决方案
- step3:生成并运行启动器

#### 配置说明logproducerconfig.proto

| 参数               | 类型  | 描述                                                         |
| ------------------ | ----- | ------------------------------------------------------------ |
| TotalSizeLnBytes   | Int64 | 实例能缓存的日志大小上限，默认为 100MB。                     |
| MaxSendWorkerCount | Int64 | client能并发的最多线程的数量，默认为50                |
| MaxBlockSec        | Int   | 如果client可用空间不足，调用者在 send 方法上的最大阻塞时间，默认为 60 秒。 如果超过这个时间后所需空间仍无法得到满足，send 方法会抛出TimeoutException。如果将该值设为0，当所需空间无法得到满足时，send 方法会立即抛出 TimeoutException。如果您希望 send 方法一直阻塞直到所需空间得到满足，可将该值设为负数。 |
| MaxBatchSize       | Int64 | 当一个Batch中缓存的日志大小大于等于 batchSizeThresholdInBytes 时，该 batch 将被发送，默认为 512 KB，最大可设置成 5MB。 |
| LingerMs           | Int64 | Batch从创建到可发送的逗留时间，默认为 2 秒，最小可设置成 100 毫秒。 |
| Retries            | Int   | 如果某个Batch首次发送失败，能够对其重试的次数，默认为 10 次。 如果 retries 小于等于 0，该 ProducerBatch 首次发送失败后将直接进入失败队列。 |
| BaseRetryBackoffMs | Int64 | 首次重试的退避时间，默认为 100 毫秒。 client采样指数退避算法，第 N 次重试的计划等待时间为 baseRetryBackoffMs * 2^(N-1)。 |
| MaxRetryBackoffMs  | Int64 | 重试的最大退避时间，默认为 50 秒。                           |


#### 异步demo:  sample.cpp文件

- SecretId和SecretKey为云API密钥，密钥信息获取请前往[密钥获取](https://console.cloud.tencent.com/cam/capi)。并请确保密钥关联的账号具有相应的[SDK上传日志权限](https://cloud.tencent.com/document/product/614/68374#.E4.BD.BF.E7.94.A8-api-.E4.B8.8A.E4.BC.A0.E6.95.B0.E6.8D.AE)


```c++
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
    void Fail(PostLogStoreLogsResponse result) override 
    { 
        std::cout << result.Printf() << std::endl; 
        // std::cout<<result.loggroup_.ShortDebugString().c_str()<<std::endl;
    }
};


int main()
{
    cls_config::LogProducerConfig config;
    config.set_endpoint("ap-guangzhou.cls.tencentyun.com");
    config.set_acceskeyid("");
    config.set_accesskeysecret("");
    std::string topic = "";

    auto client = std::make_shared<ProducerClient>(config);
    auto callback = std::make_shared<UserResult>();
    client->Start();
    cls::Log log;
    log.set_time(time(NULL));
    
    auto content = log.add_contents();
    content->set_key("content");
    content->set_value("this my test");
    PostLogStoreLogsResponse ret = client->PostLogStoreLogs(topic, log, callback);
    if(ret.statusCode != 0){
        cout<<ret.content<<endl; 
    }
    client->LogProducerEnvDestroy();

    return 0;
}


```

- 运行生成的exe程序包，可登陆[腾讯云平台](https://console.cloud.tencent.com/cls/search)查看日志是否上传成功

### 同步模式

#### 使用说明

- step1:使用vs打开代码目录
- step2:配置release编译器
- step3:生产解决方案
- step3:生成并运行启动器

#### 同步demo: syncsample.cpp 

```c++
#include "client.h"
#include "common.h"
#include "cls_logs.pb.h"
#include <string>
#include <iostream>
#include <unistd.h>
#include <memory>
using namespace tencent_log_sdk_cpp_v2;
using namespace std;
int main(int argc,char ** argv)
{
    //eg:内网域名：ap-guangzhou.cls.tencentyun.com
    //eg:外网域名：ap-guangzhou.cls.tencentcs.com
    string endpoint = "ap-guangzhou.cls.tencentcs.com";
    string accessKeyId = "";
    string accessKey = "";
    string topic = "";
    string token = "";
    std::shared_ptr<LOGClient> ptr= std::make_shared<LOGClient>(endpoint,accessKeyId,accessKey,LOG_REQUEST_TIMEOUT,LOG_CONNECT_TIMEOUT,token,"127.0.0.1",true);
    cls::LogGroup loggroup;
    auto log = loggroup.add_logs();
    log->set_time(time(NULL));
    auto content = log->add_contents();
    content->set_key("content");
    content->set_value("this my test log");
    loggroup.set_source("127.0.0.1");
    PostLogStoreLogsResponse ret;
    try{
        for(int i = 0; i < 10; ++i){
            ret = ptr -> PostLogStoreLogs(topic,loggroup);
            printf("%s\n",ret.Printf().c_str());
        }
    }
    catch(LOGException & e)
    {
        cout<<e.GetErrorCode()<<":"<<e.GetMessage()<<endl;
    }
    return 0;
}

```


## FAQ

- 如果不使用vcpg进行配置，在分别安装依赖时，需要判断依赖之间是否会出现冲突，造成可以编译但不能运行的问题（很难排查）

- 编译时将cpkg的installed的x64-windows移动到代码同目录下，或者更改makelist中的位置

- 使用vs进行编译的时候出现error，显示不安全使用：预处理器增加_SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS

- 编译test进行单测时，需要选择c++17及以上的编译器进行编译

- 安装时候出现error2503（Windows server大部分情况下没有管理员权限）
	- 解决方法：
	```
管理员命令行安装
复制安装包到c盘
1.运行CMD（以管理员权限运行）
2.进入c盘根目录
3.输入以下命令： msiexec /package "your_package_name.msi"
	```	
### 安装：
- 统一使用vcpkg包管理器进行依赖库管理，可以配合vs联合使用，省略交叉编译和配置环境变量等过程。
	- 依赖安装：
```shell
#使用cpkg进行安装
git clone https://github.com/microsoft/vcpkg.git 
cd vcpkg
.\bootstrap-vcpkg.bat
#查看依赖库版本列表
.\vcpkg search package_name
#安装boost  （时间比较长） 
.\vcpkg install boost
#安装protobuf
.\vcpkg install protobuf
#安装测试环境
.\vcpkg install gtest
.\vcpkg install benchmark
#集成到 Visual Studio   强烈推荐，可以省略配置vs
.\vcpkg integrate install
#如果没有进行集成，可以在vs中是手动配置参考http://www.cppcns.com/ruanjian/c/712786.html
```
	- protobuf验证：
		 - contacts.proto 文件
```c++
// 首行：语法指定行（proto2）
syntax = "proto2";
package contacts;

// 定义联系人message
message PeopleInfo {
  optional string name = 1;  // 姓名（proto2 必须声明 required/optional/repeated）
  optional int32 age = 2;    // 年龄（proto2 必须声明 required/optional/repeated）
}
```
		- test.cc 文件
```c++
#include <iostream> 
#include "contacts.pb.h"

int main() { 
    std::string people_str; 

    {
        // 对⼀个联系⼈的信息使⽤ PB 进⾏序列化，并将结果打印出来。
        contacts::PeopleInfo people; 
        people.set_name("张珊"); 
        people.set_age(20); 
        if (!people.SerializeToString(&people_str)) { 
            std::cerr << "序列化联系⼈失败！" << std::endl; 
            return -1;
        }
        std::cout << "序列化成功，结果：" << people_str << std::endl; 
    }
    
    {
        // 对序列化后的内容使⽤ PB 进⾏反序列，解析出联系⼈信息并打印出来。
        contacts::PeopleInfo people; 
        if (!people.ParseFromString(people_str)) { 
            std::cerr << "反序列化联系⼈失败！" << std::endl; 
            return -1;
        } 
        std::cout << "反序列化成功！" << std::endl
                  << "姓名： " << people.name() << std::endl
                  << "年龄： " << people.age() << std::endl;
    }

    return 0;
}
```

	- boost验证：
```c++
#include <IOStream>
#include <fstream>
#include <boost/filesystem.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
 
namespace fs = boost::filesystem;
namespace pt = boost::posix_time;
 
void print_directory(const fs::path& dir) {
     try {
         if (fs::exists(dir)) {
             std::cout << "目录内容: " << dir << "\n";
             for (const auto& entry : fs::directory_iterator(dir)) {
                 std::cout << " " << entry.path().filename() << std::endl;
             }
         }
     }
     catch (const fs::filesystem_error& e) {
         std::cerr << "文件系统错误: " << e.what() << std::endl;
     }
}
 
int main() {
     // 1. 文件系统操作
     fs::path current_dir = fs::current_path();
     std::cout << "当前工作目录: " << current_dir << "\n\n";
 
     // 创建测试目录
     fs::create_directories("test_dir/data");
     std::ofstream("test_dir/sample.txt") << "Boost测试文件";
 
     // 列出目录内容
     print_directory("test_dir");
 
     // 2. 日期时间操作
     pt::ptime now = pt::second_clock::local_time();
     pt::time_duration td = now.time_of_day();
 
     std::cout << "\n当前时间: "
         << now.date().year() << "-"
         << std::setw(2) << std::setfill('0') << now.date().month().as_number() << "-"
         << std::setw(2) << now.date().day() << " "
         << td.hours() << ":" << td.minutes() << ":" << td.seconds()
         << std::endl;
 
     // 3. 路径操作演示
     fs::path p("test_dir/data/file.dat");
     std::cout << "\n路径分解演示:\n"
         << "根目录: " << p.root_name() << "\n"
         << "相对路径: " << p.relative_path() << "\n"
         << "父目录: " << p.parent_path() << "\n"
         << "文件名: " << p.filename() << std::endl;
 
     // 清理测试目录
     fs::remove_all("test_dir");
 
     return 0;
}
```

	
	

- ### 代码说明

  ```
  .
  ├── adapter.cpp                #适配器 主要是发送网络请求
  ├── adapter.h
  ├── batchloggroup.cpp          #对聚合的数据抽象
  ├── batchloggroup.h
  ├── client.cpp
  ├── client.h                   #内存发起网络请求的client
  ├── cls_logs.pb.cc
  ├── cls_logs.pb.h
  ├── cls_logs.proto
  ├── CMakeLists.txt
  ├── common.cpp
  ├── common.h
  ├── error.h
  ├── logaccumulator.cpp         #负责数据的聚合
  ├── logaccumulator.h
  ├── logmemmgr.cpp              #内存管理类
  ├── logmemmgr.h
  ├── logproducerconfig.pb.cc
  ├── logproducerconfig.pb.h
  ├── logproducerconfig.proto    #配置相关的协议
  ├── logretryqueue.cpp
  ├── logretryqueue.h            #重试队列
  ├── logthreadpool.cpp
  ├── logthreadpool.h            #线程池
  ├── logtimer.cpp
  ├── logtimer.h                 #定时器
  ├── lz4
  │   ├── lz4.c
  │   └── lz4.h
  ├── producerclient.cpp
  ├── producerclient.h           #对外提供能力的类
  ├── rapidjson
  │   ├── allocators.h
  │   ├── document.h
  │   ├── encodedstream.h
  │   ├── encodings.h
  │   ├── error
  │   │   ├── en.h
  │   │   └── error.h
  │   ├── filereadstream.h
  │   ├── filewritestream.h
  │   ├── fwd.h
  │   ├── internal
  │   │   ├── biginteger.h
  │   │   ├── diyfp.h
  │   │   ├── dtoa.h
  │   │   ├── ieee754.h
  │   │   ├── itoa.h
  │   │   ├── meta.h
  │   │   ├── pow10.h
  │   │   ├── regex.h
  │   │   ├── stack.h
  │   │   ├── strfunc.h
  │   │   ├── strtod.h
  │   │   └── swap.h
  │   ├── istreamwrapper.h
  │   ├── memorybuffer.h
  │   ├── memorystream.h
  │   ├── msinttypes
  │   │   ├── inttypes.h
  │   │   └── stdint.h
  │   ├── ostreamwrapper.h
  │   ├── pointer.h
  │   ├── prettywriter.h
  │   ├── rapidjson.h
  │   ├── reader.h
  │   ├── schema.h
  │   ├── stream.h
  │   ├── stringbuffer.h
  │   └── writer.h
  ├── README.md
  ├── RestfulApiCommon.h
  ├── result.h
  ├── sample.cpp
  ├── signature.cpp         #签名算法，使用的是腾讯sdk能力
  ├── signature.h
  ├── test
  │   ├── CMakeLists.txt
  │   ├── tencentclssdk_benchmark.cpp
  │   └── tencentclssdk_test.cpp
  ├── utils.cpp
  └── utils.h
  ```