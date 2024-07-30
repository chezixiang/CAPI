#include"CAPIheader.h"

int main() {
    // 假设您有一个有效的.wav文件URL  
    std::string url = "https://s19.aconvert.com/convert/p3r68-cdx67/i4dvf-dfb99.wav"; // 修改为有效的.wav URL  
    std::string filepath = "C:\\music\\music.wav";

    // 确保目录存在  
    namespace fs = std::filesystem;
    if (!fs::exists("C:\\music")) {
        fs::create_directories("C:\\music");
    }

    download_file(url, filepath);
    play_wav(filepath);

    return 0;
}
