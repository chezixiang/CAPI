#pragma pack(8)

bool debugmode = false;
#include "CAPIheader.h"   
#include "ServerStdOut.hpp"
#include "urlbase.h"

int main(int argc, char** argv) {

    /*
    argc = 10;
    const char* charr[10] = {
        "CAPI.exe","-text", "连接测试" ,"-character", "【原神】五郎" ,"-url", "127.0.0.1", "-port", "80" ,"-post"
    };
    argv = const_cast<char**>(const_cast<const char* const*>(charr));
    */
    using std::cout;
    using std::endl;
    startup();
    // 初始化变量
   
    std::string ip = "127.0.0.1";
    std::string port="80";
    std::string text="Null";
    std::string character;
    std::string emotion = "default"; // 假设有默认情绪  
    std::string text_language = "auto";
    
    std::string pack;
    CURLcode rev = CURLE_OK;

    int topk = 5; // GPT模型参数，假设有默认值  
    float topp = 0.8; // GPT模型参数，假设有默认值  
    float temperature = 0.8; // GPT模型参数，假设有默认值  
    int batch_size = 1;
    float speed = 1.0;
    bool save_temp = false;
    bool stream = false;
    std::string format = "WAV";
    bool usepost = false;
    

    try {
        if (argc <= 1) {
            //cout << "zh_cn:" << endl;
            INFO("help", "这是一个简单的API程序，用于Text-to-Speech API。", 1);
            INFO("help", "-text：要转换的文本，不能使用URL编码。", 1);
            INFO("help", "-character：字符文件夹名称，注意区分大小写、全角/半角和语言。", 1);
            INFO("help", "-emotion：角色情绪，必须是角色实际支持的情绪，否则将使用默认情绪。", 1);
            INFO("help", "-text_language：文本语言（auto/zh/en/ja），默认为多语言混合。", 1);
            INFO("help", "-topk、topp、temperature：GPT模型参数，不熟悉无需修改。", 1);
            INFO("help", "如果你想获得更多有关它们的信息，请使用 - help 选项。", 1);
            INFO("help", "-speed：语音速度，默认为1.0。", 1);
            INFO("help", "-batch_size：对项目并行处理N线程，以加快处理速度，整数，默认为1。", 1);
            INFO("help", "-save_temp：是否保存临时文件，如果为true，后端将保存生成的音频。", 1);
            INFO("help", "-stream：是否流，为true时，音频将逐句返回，默认为false。", 1);
            INFO("help", "-format：格式，默认为WAV，允许MP3/WAV/OGG。", 1);
            INFO("help", "-url：指定服务器的URL地址，不带端口。", 1);
            INFO("help", "-port：指定服务器的服务端口,使用此项必须在指定此项之前指定-url选项。", 1);
            INFO("help", "-post：指定使用POST方法，正常情况下无需修改(默认使用get方法)。", 1);
        }
        else
        {

            bool have_url = false;
            // 遍历参数  
            for (int i = 1; i < argc; ++i) {
                std::string arg = argv[i];

                if (arg == "-help") {
                    // 如果发现--help且是唯一的参数  
                    if (argc > 2) {
                        fatal("EMS", "无效用法：“-help”必须单独使用。", 4001);
                        return 4002;
                    }
                    INFO("help", "这是一个简单的API程序，用于Text-to-Speech API。", 1);
                    INFO("help", "-text：要转换的文本，不能使用URL编码。", 1);
                    INFO("help", "-character：字符文件夹名称，注意区分大小写、全角/半角和语言。", 1);
                    INFO("help", "-emotion：角色情绪，必须是角色实际支持的情绪，否则将使用默认情绪。", 1);
                    INFO("help", "-text_language：文本语言（auto/zh/en/ja），默认为多语言混合。", 1);
                    INFO("help", "-topk：在词表中选择概率最高的前k个token作为候选，然后从这些候选中随机选择一个作为输出。", 1);
                    INFO("help", "-topp：top-p是一种与top-k类似的采样方法，但它不是基于固定的k个候选，而是基于概率的累计和。", 1);
                    INFO("help", "-temperature：通过在softmax函数之前对logits（未经归一化的概率）进行缩放来改变概率分布的“温度”。", 1);
                    INFO("help", "-speed：语音速度，默认为1.0。", 1);
                    INFO("help", "-batch_size：如果你有一台功能强大的计算机，一次可以增加多少批，以加快处理速度，整数，默认为1。", 1);
                    INFO("help", "-save_temp：是否保存临时文件，如果为true，后端将保存生成的音频，后续相同的请求将直接返回该数据。", 1);
                    INFO("help", "-stream：是否流，为true时，音频将逐句返回，默认为false。", 1);
                    INFO("help", "-format：格式，默认为WAV，允许MP3/WAV/OGG。", 1);
                    INFO("help", "-url：指定服务器的URL地址，不带端口。", 1);
                    INFO("help", "-port：指定服务器的服务端口,使用此项必须在指定此项之前指定-url选项。", 1);
                    INFO("help", "-post：指定使用POST方法，正常情况下无需修改(默认使用get方法)。", 1);
                    std::cout << "请按回车键退出..." << endl;
                    std::cin.get();
                    return 0; // 正常退出  
                }
                else if (arg[0] != '-') {
                    fatal("EMS", "混乱的参数列表或格式错误", 4000);
                    return 4000;
                }
                else if (arg == "-url") {
                    // 检查是否提供了路径  
                    if (i + 1 >= argc) {
                        fatal("EMS", "无效用法：“-url”必须提供参数。", 4001);
                        return 4001;
                    }
                    have_url = true;
                    ip = argv[i + 1];
                    i++;
                }
                else if (arg == "-port") {
                    if (!have_url)
                    {
                        fatal("EMS", "无效用法：在“-port”之前必须提供“-url”项。", 4004);
                    }
                    if (i + 1 >= argc) {
                        fatal("EMS", "无效用法：“-port”必须提供参数。", 4001);
                        return 4001;
                    }
                    port = argv[i + 1];
                    i++;
                }
                else if (arg == "-text") {
                    if (i + 1 >= argc) {
                        fatal("EMS", "无效用法：“-text”必须提供参数。", 4001);
                        return 4001;
                    }
                    text = argv[i + 1];
                    i++;
                }
                else if (arg == "-character") {
                    if (i + 1 >= argc) {
                        fatal("EMS", "无效用法：“-character”必须提供参数。", 4001);
                        return 4001;
                    }
                    character = argv[i + 1];
                    i++;
                }
                else if (arg == "-format") {
                    if (i + 1 >= argc) {
                        fatal("EMS", "无效用法：“-format”必须提供参数。", 4001);
                        return 4001;
                    }
                    if(argv[i + 1]=="MP3"|| argv[i + 1] == "WAV" || argv[i + 1] == "OGG"|| argv[i + 1] == "mp3" || argv[i + 1] == "wav" || argv[i + 1] == "ogg")
                    character = argv[i + 1];
                    else fatal("EMS", "无效参数：“-format”的参数只允许MP3/WAV/OGG。", 4000);
                    i++;
                }
                else if (arg == "-emotion") {
                    if (i + 1 >= argc) {
                        fatal("EMS", "无效用法：“-emotion”必须提供参数。", 4001);
                        return 4001;
                    }
                    emotion = argv[i + 1];
                    i++;
                }
                else if (arg == "-text_language") {
                    if (i + 1 >= argc) {
                        fatal("EMS", "无效用法：“-text_language”必须提供参数。", 4001);
                        return 4001;
                    }
                    text_language = argv[i + 1];
                    i++;
                }
                else if (arg == "-topk") {
                    if (i + 1 >= argc) {
                        fatal("EMS", "无效用法：“-topk”必须提供参数。", 4001);
                        return 4001;
                    }
                    topk = std::stoi(argv[i+1]);
                    i++;
                }
                else if (arg == "-topp") {
                    if (i + 1 >= argc) {
                        fatal("EMS", "无效用法：“-topp”必须提供参数。", 4001);
                        return 4001;
                    }
                    topp = std::stof(argv[i + 1]);
                    i++;
                }
                else if (arg == "-temperature") {
                    if (i + 1 >= argc) {
                        fatal("EMS", "无效用法：“-temperature”必须提供参数。", 4001);
                        return 4001;
                    }
                    temperature = std::stof(argv[i + 1]);
                    i++;
                }
                else if (arg == "-speed") {
                    if (i + 1 >= argc) {
                        fatal("EMS", "无效用法：“-speed”必须提供参数。", 4001);
                        return 4001;
                    }
                    speed = std::stof(argv[i + 1]);
                    i++;
                }
                else if (arg == "-topp") {
                    if (i + 1 >= argc) {
                        fatal("EMS", "无效用法：“-topp”必须提供参数。", 4001);
                        return 4001;
                    }
                    topp = std::stof(argv[i + 1]);
                    i++;
                }
                else if (arg == "-batch_size") {
                    if (i + 1 >= argc) {
                        fatal("EMS", "无效用法：“-batch_size”必须提供参数。", 4001);
                        return 4001;
                    }
                    batch_size = std::stoi(argv[i + 1]);
                    i++;
                }
                else if (arg == "-save_temp") {
                    if (i + 1 >= argc) {
                        fatal("EMS", "无效用法：“-save_temp”必须提供参数。", 4001);
                        return 4001;
                    }
                    if (argv[i + 1] == "true")
                        save_temp = true;
                    else
                        save_temp = false;
                    i++;
                }
                else if (arg == "-stream") {
                    if (i + 1 >= argc) {
                        fatal("EMS", "无效用法：“-stream”必须提供参数。", 4001);
                        return 4001;
                    }
                    if (argv[i + 1] == "true")
                        stream = true;
                    else
                        stream = false;
                    i++;
                }
                else if (arg == "-post") {
                    usepost = true;
                }
                else if (arg == "-debug") {
                    debugmode = true;
                    debug("已开启调试日志");
                    debug(logFilePath);
                }
                else {
                    // 无效参数  
                    fatal("EMS", "无法识别的参数", 4000);
                    return 4000;
                }
            }
            if (text == "Null")
            {
                fatal("EMS", "缺少重要文本数据：text", 4001);
            }
            //文本处理
            //std::unordered_map<std::string, std::string> params = {  
            //{ "character", "zh_voice" },
            //{ "text", "你好，世界！" },
            //{ "speed", "1.0" },
            //{ "pitch", "0.5" }
            //};
            std::ostringstream oss1;
            std::ostringstream oss2;
            oss1 << speed;
            oss2 << topp;
            std::unordered_map<std::string, std::string> params = {
            {"task_type","text"},
            {"text",text},
            {"character", character},
            {"emotion", emotion},
            {"format", format},
            {"sample_rate","32000"},
            {"speed",oss1.str()},
            {"stream",std::to_string(stream)},
            {"save_temp", std::to_string(save_temp)},
            {"prompt_text",""},
            {"prompt_language","auto"},
            {"text_language","auto"},
            {"batch_size",std::to_string(batch_size)},
            {"top_k",std::to_string(topk)},
            {"top_p",std::to_string(topp)},
            {"temperature",std::to_string(temperature)},
            {"cut_method", "auto_cut"},
            {"max_cut_length","50"},
            {"seed","-1"},
            {"parallel_infer", std::to_string(true)},
            {"repetition_penalty","1.35"},
            };
            std::string getmem = ip + ":" + port + "/tts?character=" +UrlEncode(GbkToUtf8(character.c_str())) + "&text=" + UrlEncode(GbkToUtf8(text.c_str()));
            //  /tts?character=｛｛characterName｝｝&text＝｛｛text｝｝
            info("TTS", "音频处理已经开始，请稍后...");
            if (usepost)
            {
                if (debugmode)
                    debug(post(ip + ":" + port + "/tts", params, true, "C:\\music\\music.wav"));
                else
                    post(ip + ":" + port + "/tts", params, true, "C:\\music\\music.wav");
            }
            else
            {
                rev=CURLcode(stoi(get(getmem, true)));

                if (debugmode)  debug(getmem);
                if (debugmode)  debug(UrlEncode(getmem));
            }
            info("TTS", "音频处理已经完成，请检查目录下的文件。");
        }
    }
    catch (const std::invalid_argument& e) {
        fatal("EMS", "参数类型错误", 0,1);
        fatal("EMS", e.what(), 4002);
        return 4002;
    }
    catch (const std::out_of_range& e) {
        fatal("EMS", "out of range", 0, 0);
        fatal("EMS", e.what(), 4000);
        return 4000;
    }
    return 0;
}