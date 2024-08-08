#pragma pack(8)

bool debugmode = false;
#include "CAPIheader.h"   
#include "ServerStdOut.hpp"
#include "urlbase.h"

int main(int argc, char** argv) {

    /*
    argc = 10;
    const char* charr[10] = {
        "CAPI.exe","-text", "���Ӳ���" ,"-character", "��ԭ������" ,"-url", "127.0.0.1", "-port", "80" ,"-post"
    };
    argv = const_cast<char**>(const_cast<const char* const*>(charr));
    */
    using std::cout;
    using std::endl;
    startup();
    // ��ʼ������
   
    std::string ip = "127.0.0.1";
    std::string port="80";
    std::string text="Null";
    std::string character;
    std::string emotion = "default"; // ������Ĭ������  
    std::string text_language = "auto";
    
    std::string pack;
    CURLcode rev = CURLE_OK;

    int topk = 5; // GPTģ�Ͳ�����������Ĭ��ֵ  
    float topp = 0.8; // GPTģ�Ͳ�����������Ĭ��ֵ  
    float temperature = 0.8; // GPTģ�Ͳ�����������Ĭ��ֵ  
    int batch_size = 1;
    float speed = 1.0;
    bool save_temp = false;
    bool stream = false;
    std::string format = "WAV";
    bool usepost = false;
    

    try {
        if (argc <= 1) {
            //cout << "zh_cn:" << endl;
            INFO("help", "����һ���򵥵�API��������Text-to-Speech API��", 1);
            INFO("help", "-text��Ҫת�����ı�������ʹ��URL���롣", 1);
            INFO("help", "-character���ַ��ļ������ƣ�ע�����ִ�Сд��ȫ��/��Ǻ����ԡ�", 1);
            INFO("help", "-emotion����ɫ�����������ǽ�ɫʵ��֧�ֵ�����������ʹ��Ĭ��������", 1);
            INFO("help", "-text_language���ı����ԣ�auto/zh/en/ja����Ĭ��Ϊ�����Ի�ϡ�", 1);
            INFO("help", "-topk��topp��temperature��GPTģ�Ͳ���������Ϥ�����޸ġ�", 1);
            INFO("help", "��������ø����й����ǵ���Ϣ����ʹ�� - help ѡ�", 1);
            INFO("help", "-speed�������ٶȣ�Ĭ��Ϊ1.0��", 1);
            INFO("help", "-batch_size������Ŀ���д���N�̣߳��Լӿ촦���ٶȣ�������Ĭ��Ϊ1��", 1);
            INFO("help", "-save_temp���Ƿ񱣴���ʱ�ļ������Ϊtrue����˽��������ɵ���Ƶ��", 1);
            INFO("help", "-stream���Ƿ�����Ϊtrueʱ����Ƶ����䷵�أ�Ĭ��Ϊfalse��", 1);
            INFO("help", "-format����ʽ��Ĭ��ΪWAV������MP3/WAV/OGG��", 1);
            INFO("help", "-url��ָ����������URL��ַ�������˿ڡ�", 1);
            INFO("help", "-port��ָ���������ķ���˿�,ʹ�ô��������ָ������֮ǰָ��-urlѡ�", 1);
            INFO("help", "-post��ָ��ʹ��POST��������������������޸�(Ĭ��ʹ��get����)��", 1);
        }
        else
        {

            bool have_url = false;
            // ��������  
            for (int i = 1; i < argc; ++i) {
                std::string arg = argv[i];

                if (arg == "-help") {
                    // �������--help����Ψһ�Ĳ���  
                    if (argc > 2) {
                        fatal("EMS", "��Ч�÷�����-help�����뵥��ʹ�á�", 4001);
                        return 4002;
                    }
                    INFO("help", "����һ���򵥵�API��������Text-to-Speech API��", 1);
                    INFO("help", "-text��Ҫת�����ı�������ʹ��URL���롣", 1);
                    INFO("help", "-character���ַ��ļ������ƣ�ע�����ִ�Сд��ȫ��/��Ǻ����ԡ�", 1);
                    INFO("help", "-emotion����ɫ�����������ǽ�ɫʵ��֧�ֵ�����������ʹ��Ĭ��������", 1);
                    INFO("help", "-text_language���ı����ԣ�auto/zh/en/ja����Ĭ��Ϊ�����Ի�ϡ�", 1);
                    INFO("help", "-topk���ڴʱ���ѡ�������ߵ�ǰk��token��Ϊ��ѡ��Ȼ�����Щ��ѡ�����ѡ��һ����Ϊ�����", 1);
                    INFO("help", "-topp��top-p��һ����top-k���ƵĲ����������������ǻ��ڹ̶���k����ѡ�����ǻ��ڸ��ʵ��ۼƺ͡�", 1);
                    INFO("help", "-temperature��ͨ����softmax����֮ǰ��logits��δ����һ���ĸ��ʣ������������ı���ʷֲ��ġ��¶ȡ���", 1);
                    INFO("help", "-speed�������ٶȣ�Ĭ��Ϊ1.0��", 1);
                    INFO("help", "-batch_size���������һ̨����ǿ��ļ������һ�ο������Ӷ��������Լӿ촦���ٶȣ�������Ĭ��Ϊ1��", 1);
                    INFO("help", "-save_temp���Ƿ񱣴���ʱ�ļ������Ϊtrue����˽��������ɵ���Ƶ��������ͬ������ֱ�ӷ��ظ����ݡ�", 1);
                    INFO("help", "-stream���Ƿ�����Ϊtrueʱ����Ƶ����䷵�أ�Ĭ��Ϊfalse��", 1);
                    INFO("help", "-format����ʽ��Ĭ��ΪWAV������MP3/WAV/OGG��", 1);
                    INFO("help", "-url��ָ����������URL��ַ�������˿ڡ�", 1);
                    INFO("help", "-port��ָ���������ķ���˿�,ʹ�ô��������ָ������֮ǰָ��-urlѡ�", 1);
                    INFO("help", "-post��ָ��ʹ��POST��������������������޸�(Ĭ��ʹ��get����)��", 1);
                    std::cout << "�밴�س����˳�..." << endl;
                    std::cin.get();
                    return 0; // �����˳�  
                }
                else if (arg[0] != '-') {
                    fatal("EMS", "���ҵĲ����б���ʽ����", 4000);
                    return 4000;
                }
                else if (arg == "-url") {
                    // ����Ƿ��ṩ��·��  
                    if (i + 1 >= argc) {
                        fatal("EMS", "��Ч�÷�����-url�������ṩ������", 4001);
                        return 4001;
                    }
                    have_url = true;
                    ip = argv[i + 1];
                    i++;
                }
                else if (arg == "-port") {
                    if (!have_url)
                    {
                        fatal("EMS", "��Ч�÷����ڡ�-port��֮ǰ�����ṩ��-url���", 4004);
                    }
                    if (i + 1 >= argc) {
                        fatal("EMS", "��Ч�÷�����-port�������ṩ������", 4001);
                        return 4001;
                    }
                    port = argv[i + 1];
                    i++;
                }
                else if (arg == "-text") {
                    if (i + 1 >= argc) {
                        fatal("EMS", "��Ч�÷�����-text�������ṩ������", 4001);
                        return 4001;
                    }
                    text = argv[i + 1];
                    i++;
                }
                else if (arg == "-character") {
                    if (i + 1 >= argc) {
                        fatal("EMS", "��Ч�÷�����-character�������ṩ������", 4001);
                        return 4001;
                    }
                    character = argv[i + 1];
                    i++;
                }
                else if (arg == "-format") {
                    if (i + 1 >= argc) {
                        fatal("EMS", "��Ч�÷�����-format�������ṩ������", 4001);
                        return 4001;
                    }
                    if(argv[i + 1]=="MP3"|| argv[i + 1] == "WAV" || argv[i + 1] == "OGG"|| argv[i + 1] == "mp3" || argv[i + 1] == "wav" || argv[i + 1] == "ogg")
                    character = argv[i + 1];
                    else fatal("EMS", "��Ч��������-format���Ĳ���ֻ����MP3/WAV/OGG��", 4000);
                    i++;
                }
                else if (arg == "-emotion") {
                    if (i + 1 >= argc) {
                        fatal("EMS", "��Ч�÷�����-emotion�������ṩ������", 4001);
                        return 4001;
                    }
                    emotion = argv[i + 1];
                    i++;
                }
                else if (arg == "-text_language") {
                    if (i + 1 >= argc) {
                        fatal("EMS", "��Ч�÷�����-text_language�������ṩ������", 4001);
                        return 4001;
                    }
                    text_language = argv[i + 1];
                    i++;
                }
                else if (arg == "-topk") {
                    if (i + 1 >= argc) {
                        fatal("EMS", "��Ч�÷�����-topk�������ṩ������", 4001);
                        return 4001;
                    }
                    topk = std::stoi(argv[i+1]);
                    i++;
                }
                else if (arg == "-topp") {
                    if (i + 1 >= argc) {
                        fatal("EMS", "��Ч�÷�����-topp�������ṩ������", 4001);
                        return 4001;
                    }
                    topp = std::stof(argv[i + 1]);
                    i++;
                }
                else if (arg == "-temperature") {
                    if (i + 1 >= argc) {
                        fatal("EMS", "��Ч�÷�����-temperature�������ṩ������", 4001);
                        return 4001;
                    }
                    temperature = std::stof(argv[i + 1]);
                    i++;
                }
                else if (arg == "-speed") {
                    if (i + 1 >= argc) {
                        fatal("EMS", "��Ч�÷�����-speed�������ṩ������", 4001);
                        return 4001;
                    }
                    speed = std::stof(argv[i + 1]);
                    i++;
                }
                else if (arg == "-topp") {
                    if (i + 1 >= argc) {
                        fatal("EMS", "��Ч�÷�����-topp�������ṩ������", 4001);
                        return 4001;
                    }
                    topp = std::stof(argv[i + 1]);
                    i++;
                }
                else if (arg == "-batch_size") {
                    if (i + 1 >= argc) {
                        fatal("EMS", "��Ч�÷�����-batch_size�������ṩ������", 4001);
                        return 4001;
                    }
                    batch_size = std::stoi(argv[i + 1]);
                    i++;
                }
                else if (arg == "-save_temp") {
                    if (i + 1 >= argc) {
                        fatal("EMS", "��Ч�÷�����-save_temp�������ṩ������", 4001);
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
                        fatal("EMS", "��Ч�÷�����-stream�������ṩ������", 4001);
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
                    debug("�ѿ���������־");
                    debug(logFilePath);
                }
                else {
                    // ��Ч����  
                    fatal("EMS", "�޷�ʶ��Ĳ���", 4000);
                    return 4000;
                }
            }
            if (text == "Null")
            {
                fatal("EMS", "ȱ����Ҫ�ı����ݣ�text", 4001);
            }
            //�ı�����
            //std::unordered_map<std::string, std::string> params = {  
            //{ "character", "zh_voice" },
            //{ "text", "��ã����磡" },
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
            //  /tts?character=����characterName����&text������text����
            info("TTS", "��Ƶ�����Ѿ���ʼ�����Ժ�...");
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
            info("TTS", "��Ƶ�����Ѿ���ɣ�����Ŀ¼�µ��ļ���");
        }
    }
    catch (const std::invalid_argument& e) {
        fatal("EMS", "�������ʹ���", 0,1);
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