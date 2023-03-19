#include "util.hpp"

std::string wstringToString(std::wstring wstr_in) {
    int arrsize = (wstr_in.size() + 1) * 3 + 1;
    char* chr_out = new char[arrsize];
    WideCharToMultiByte(CP_ACP, 0, wstr_in.c_str(), wstr_in.size() + 1, chr_out, arrsize, NULL, 0);
    std::string str_out(chr_out);
    delete chr_out;
    return str_out;
}

std::wstring stringToWstring(std::string str_in) {
    int arrsize = str_in.size() * 3 + 1;
    wchar_t* wchar_out = new wchar_t[arrsize];
    MultiByteToWideChar(CP_UTF8, 0, str_in.c_str(), str_in.size() + 1, wchar_out, arrsize);
    std::wstring wstr_out(wchar_out);
    delete wchar_out;
    return wstr_out;
}

// 指定したフォルダの中身を取得
std::vector<std::string> getFoldersAndFiles(std::wstring path) {
    HANDLE hFind;
    WIN32_FIND_DATA win32fd;
    std::vector<std::string> return_files;
    std::string tmp;
    char tmp_char[256];
    //拡張子付きファイルを全取得する
    hFind = FindFirstFile((path + L"\\\\*.py").c_str(), &win32fd);
    if (hFind != INVALID_HANDLE_VALUE){
        do {
            if (win32fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                // ディレクトリは無視
            } else {
                tmp = wstringToString(win32fd.cFileName);
                return_files.push_back(tmp);

                FILETIME tmp_ftime = win32fd.ftLastWriteTime;
                SYSTEMTIME tmp_stime;
                FileTimeToSystemTime(&tmp_ftime, &tmp_stime);
                sprintf(tmp_char, "%d/%02d/%02d", tmp_stime.wYear, tmp_stime.wMonth, tmp_stime.wDay);
                tmp = tmp_char;
                return_files.push_back(tmp);
                long long datasize = win32fd.nFileSizeHigh;
                datasize = (datasize << 32) + win32fd.nFileSizeLow;
                if(datasize > (1<<20)){
                    tmp = std::to_string(datasize>>20) + "MB";
                }else if(datasize > (1<<10)){
                    tmp = std::to_string(datasize>>10) + "KB";
                }else{
                    tmp = "0KB";
                }
                std::ostringstream oss;
                oss << std::setw(6) << tmp;
                return_files.push_back(oss.str());
            }
        } while (FindNextFile(hFind, &win32fd));
    }
    FindClose(hFind);
    return return_files;
}

void log(const char* input_str) {
    std::cout << "[LOG]: " << input_str << std::endl;
}
void log(const wchar_t* input_str) {
    std::wcout << "[LOG]: " << input_str << std::endl;
}
void log(std::string input_str) {
    std::cout << "[LOG]: " << input_str << std::endl;
}
void log(std::wstring input_str) {
    std::wcout << "[LOG]: " << input_str << std::endl;
}

namespace uuid {
    static std::random_device              rd;
    static std::mt19937                    gen(rd());
    static std::uniform_int_distribution<> dis(0, 15);
    static std::uniform_int_distribution<> dis2(8, 11);

    std::wstring generate_uuid_v4() {
        std::wstringstream ss;
        int i;
        ss << std::hex;
        for (i = 0; i < 8; i++) {
            ss << dis(gen);
        }
        ss << "-";
        for (i = 0; i < 4; i++) {
            ss << dis(gen);
        }
        ss << "-4";
        for (i = 0; i < 3; i++) {
            ss << dis(gen);
        }
        ss << "-";
        ss << dis2(gen);
        for (i = 0; i < 3; i++) {
            ss << dis(gen);
        }
        ss << "-";
        for (i = 0; i < 12; i++) {
            ss << dis(gen);
        };
        return ss.str();
    }
}

namespace mmap {
    // MemoryMapの生成処理
    mmap::mmap(std::wstring id) {
        this->id = id;
        this->send_mmap_file_name = MEMORY_FOLDER_NAME + L"\\" + id + L"_s.map";
        this->receive_mmap_file_name = MEMORY_FOLDER_NAME + L"\\" + id + L"_r.map";
        this->tmp_mmap_file_name = MEMORY_FOLDER_NAME + L"\\" + id + L"_t.map";

        this->send_map_view = nullptr;
        this->message_id = 0;
        this->is_message_can_send = true;

        // フォルダがない場合は作成しておく
        CreateDirectory(MEMORY_FOLDER_NAME.c_str(), NULL);

        // 送信用ファイルを作成してハンドルを取得しておく
        this->send_hfile = CreateFile(this->send_mmap_file_name.c_str(), GENERIC_READ | GENERIC_WRITE,
                                        FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        this->send_mmap_hfile = CreateFileMapping(this->send_hfile, NULL, PAGE_READWRITE,
                                        0, MEMORY_SIZE, NULL);
        this->send_map_view = (PWCHAR) MapViewOfFile(this->send_mmap_hfile , FILE_MAP_WRITE , 0 , 0 , 0);

        // 受信用ファイルを作成だけしておく
        this->receive_hfile = CreateFile(this->receive_mmap_file_name.c_str(), GENERIC_READ | GENERIC_WRITE, 
                                        FILE_SHARE_READ,NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        this->receive_mmap_hfile = CreateFileMapping(this->receive_hfile, NULL, PAGE_READWRITE,
                                        0, MEMORY_SIZE, NULL);
        CloseHandle(this->receive_mmap_hfile);
        CloseHandle(this->receive_hfile);

        std::string hogehoge;
        // std::cin >> hogehoge;

        // Pythonを起動する
        STARTUPINFO si{};
        si.cb = sizeof(si);
        wchar_t py[100];
        _snwprintf(py, 100, L"python readmemorymap.py %s\0", this->id.c_str());
        CreateProcess(nullptr, py, nullptr, nullptr, false, 0, nullptr, nullptr, &si, &(this->python_process));

        // std::cin >> hogehoge;

        // ファイルができるまで待つ
        while(GetFileAttributes(this->tmp_mmap_file_name.c_str())==INVALID_FILE_ATTRIBUTES) {
            Sleep(1);
        }

        // 受信用ファイルを開いてハンドルを取得する
        this->receive_hfile = CreateFile(this->receive_mmap_file_name.c_str(), GENERIC_READ,
                                        FILE_SHARE_READ | FILE_SHARE_WRITE,
                                        NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        this->receive_mmap_hfile = CreateFileMapping(this->receive_hfile, NULL, PAGE_READONLY,
                                        0, MEMORY_SIZE, NULL);
        this->receive_map_view = (PWCHAR) MapViewOfFile(this->receive_mmap_hfile , FILE_MAP_READ , 0 , 0 , 0);

        std::cout << "this->send_hfile: " << this->send_hfile << std::endl;
        std::cout << "this->send_mmap_hfile: " << this->send_mmap_hfile << std::endl;
        std::cout << "this->send_map_view: " << this->send_map_view << std::endl;
        std::cout << "this->receive_hfile: " << this->receive_hfile << std::endl;
        std::cout << "this->receive_mmap_hfile: " << this->receive_mmap_hfile << std::endl;
        std::cout << "this->receive_map_view: " << this->receive_map_view << std::endl;

    }
    
    // MemoryMapで送信が行えるかどうか
    bool mmap::can_send() {
        return this->is_message_can_send;
    }
    
    // MemoryMapの終了処理
    void mmap::close() {
        this->send("exit");
        WaitForSingleObject((this->python_process).hProcess, INFINITE);
        UnmapViewOfFile(this->send_map_view);
        CloseHandle(this->send_mmap_hfile);
        CloseHandle(this->send_hfile);
        UnmapViewOfFile(this->receive_map_view);
        CloseHandle(this->receive_mmap_hfile);
        CloseHandle(this->receive_hfile);

        DeleteFile(this->send_mmap_file_name.c_str());
        DeleteFile(this->receive_mmap_file_name.c_str());
        DeleteFile(this->tmp_mmap_file_name.c_str());
        std::cout << "closed!!" <<std::endl;
    }

    // MemoryMapで情報を送る
    // return: 0=NG 1=OK
    bool mmap::send(const char* send_str) {
        log("send start");
        if(NULL == this->send_mmap_hfile) {
            // ERROR: initがされていない
            log("please initiarize mmap");
            return SEND_NG;
        } else if(false == this->is_message_can_send){
            // ERROR: 送ったメッセージがまだ読まれていない
            log("please read received message");
            return SEND_NG;
        } else {
            // 送信可能状態
            if(sizeof(send_str) > SEND_MAX_MESSAGE_LENGTH) {
                // ERROR: メッセージが長すぎる
                log("send messege is too long");
                return SEND_NG;
            }
            this->message_id++;
            this->is_message_can_send = false;
            std::string tmp_string(send_str);
            std::wstring tmp_wstring(stringToWstring(tmp_string));
            _snwprintf(this->send_map_view, MEMORY_SIZE, L"%032x\n%s\n", this->message_id, tmp_wstring.c_str());
            log("send OK");
            return SEND_OK;
        }
    }

    bool mmap::receive() {
        log("read start");
        if(NULL == this->receive_mmap_hfile) {
            // ERROR: initがされていない
            log("please initiarize mmap");
            return RECEIVE_NG;
        } else if(true == this->is_message_can_send){
            // ERROR: メッセージがない
            log("cann't received message");
            return RECEIVE_NG;
        } else {
            // 受信可能状態
            // 送ったIDと同じIDが帰ってきたらOK
            char tmp[35];
            _snprintf(tmp, sizeof(tmp), "%032x", this->message_id);
            bool onaji;
            while(true) {
                std::wstring tmp_wstring(this->receive_map_view);
                this->received_string = wstringToString(tmp_wstring);
                onaji = true;
                for(int i=0; i<32; ++i){
                    if(tmp[i]!=this->received_string[i]){
                        onaji = false;
                    }
                }
                if(true == onaji){
                    break;
                }
            }
            std::wstring tmp_wstring(this->receive_map_view);
            this->received_string = wstringToString(tmp_wstring);
            this->is_message_can_send = true;
            return RECEIVE_OK;
        }
    }

    std::string mmap::getReveiveStr() {
        return this->received_string.substr(34);
    }
}

namespace imgui_ex{
    window::window(){
        this->draw_format = "";
        this->variable.clear();
    }
    void window::setDrawFormat(std::string format) {
        this->draw_format = format;
    }
    void window::drawWindow(mmap::mmap* python_mmap) {
        if(this->draw_format == ""){
            // 登録されていない場合は何もしない
            return;
        }

        int i=0;
        std::string tag = "";
        std::string property_name = "";
        std::string property_data = "";
        static std::string variable = ""; //jsonファイル
        std::string tmp_variable = "{"; //jsonファイル
        std::map<std::string, std::string> property;
        static std::map<std::string, char*> inputs;
        bool is_in_tag = false; // タグ<>の中
        bool is_tag_kimari = false; // タグ判定中
        bool is_property_name_kimari = false; // プロパティ名判定中
        bool is_property_equal_kimari = false; // プロパティのイコール判定中
        bool is_property_quotation_kimari = false; // プロパティのクォーテーション判定中
        bool is_escape_data = false; // エスケープ判定中
        bool kousin = false;
        bool hantei = false; //wordとtagを判定する

        // formatを解析して頑張る
        for (i=0;i<this->draw_format.length();++i) {
            if(!is_in_tag){
                //タグ外では「<」以外無視する
                if(draw_format[i]=='<'){
                    is_in_tag = true;
                }
            }else{
                //タグ内ではタグ判定をまずする
                if(!is_tag_kimari){
                    //決まってない場合は英数字-_と「 」以外は不正な値
                    if(draw_format[i]==' '){
                        if(tag==""){
                            //何もしない
                        }else{
                            is_tag_kimari = true;
                        }
                    }else if(draw_format[i]=='>'){
                        // 終了
                        is_tag_kimari = true;
                        hantei = true;
                    }else if(draw_format[i]>=65 && draw_format[i]<=90){
                        //大文字英字
                        tag += draw_format[i];
                    }else if(draw_format[i]>=97 && draw_format[i]<=122){
                        //小文字英字
                        tag += draw_format[i];
                    }else if(draw_format[i]>=48 && draw_format[i]<=57){
                        //数字
                        tag += draw_format[i];
                    }else if(draw_format[i]=='-' && draw_format[i]=='_'){
                        //ハイフンとアンダーバー
                        tag += draw_format[i];
                    }else{
                        // 不正な値
                        return;
                    }
                }else{
                    // タグが決まっている場合はプロパティを決める
                    if(!is_property_name_kimari){
                        //決まってない場合は英数字_と「 」と「=」「>」以外は不正な値
                        if(draw_format[i]==' '){
                            if(property_name ==""){
                                //何もしない
                            }else{
                                is_property_name_kimari = true;
                            }
                        }else if(draw_format[i]=='='){
                            if(property_name ==""){
                                //プロパティがない場合は不正
                                return;
                            }
                            is_property_name_kimari = true;
                            is_property_equal_kimari = true;
                        }else if(draw_format[i]=='>'){
                            // 終了
                            if(property_name!=""){
                                // プロパティが書きかけの場合は不正
                                return;
                            }
                            hantei = true;
                        }else if(draw_format[i]>=65 && draw_format[i]<=90){
                            //大文字英字
                            property_name += draw_format[i];
                        }else if(draw_format[i]>=97 && draw_format[i]<=122){
                            //小文字英字
                            property_name += draw_format[i];
                        }else if(draw_format[i]>=48 && draw_format[i]<=57){
                            //数字
                            property_name += draw_format[i];
                        }else if(draw_format[i]=='_'){
                            //アンダーバー
                            property_name += draw_format[i];
                        }else{
                            //不正な値
                            return;
                        }
                    }else{
                        // プロパティ名が決まったらイコールを待つ
                        if(!is_property_equal_kimari){
                            // 決まっていない場合は「=」「 」以外は不正な値
                            if(draw_format[i]==' '){
                                //何もしない
                            }else if(draw_format[i]=='='){
                                is_property_equal_kimari = true;
                            }else{
                                //不正な値
                                return;
                            }
                        }else{
                            // プロパティ名が決まったらクォーテーションを待つ
                            if(!is_property_quotation_kimari){
                                // 「"」と「 」以外は不正な値
                                if(draw_format[i]==' '){
                                    //何もしない
                                }else if(draw_format[i]=='\"'){
                                    is_property_quotation_kimari = true;
                                }else{
                                    //不正な値
                                    return;
                                }
                            }else{
                                // クォーテーションが来たらデータ終了まで待つ
                                if(is_escape_data){
                                    property_data += draw_format[i];
                                    is_escape_data = false;
                                }else if(draw_format[i]=='\\'){
                                    is_escape_data = true;
                                }else if(draw_format[i]=='\"'){
                                    // プロパティ判定はおしまい
                                    property[property_name] = property_data;

                                    // 値を初期化しておく
                                    is_property_name_kimari = false;
                                    is_property_equal_kimari = false;
                                    is_property_quotation_kimari = false;
                                    property_name = "";
                                    property_data = "";
                                }else{
                                    property_data += draw_format[i];
                                }
                            }
                        }
                    }
                }
            }
            if(hantei){
                if(tag=="Begin"){
                    ImGui::Begin(property["label"].c_str());
                }else if(tag=="End"){
                    ImGui::End();
                }else if(tag=="Text"){
                    ImGui::Text(property["label"].c_str());
                }else if(tag=="Button"){
                    if(ImGui::Button(property["label"].c_str())){
                        std::string tmp = "event:"+property["click"]+"\n"+variable+"\n";
                        python_mmap->send(tmp.c_str());
                        python_mmap->receive();
                        kousin = true;
                    }
                }else if(tag=="InputText"){
                    if(inputs.count(property["var"])==0){
                        inputs[property["var"]]=new char[1024];
                        _snprintf(inputs[property["var"]], sizeof(inputs[property["var"]]), "%s", property["value"].c_str());
                    }
                    ImGui::InputText(property["id"].c_str(),inputs[property["var"]], 1024);
                    tmp_variable+="\""+property["var"]+"\":\""+inputs[property["var"]]+"\",";
                }
                hantei=false;

                //タグとプロパティが決まったら判定をして次へ
                tag = "";
                property_name = "";
                property_data = "";
                property.clear();
                is_in_tag = false;
                is_tag_kimari = false;
                is_property_name_kimari = false;
                is_property_equal_kimari = false;
                is_property_quotation_kimari = false;
                is_escape_data = false;
            }
            
        }
        tmp_variable += "\"__\":\"__\"}";
        variable = tmp_variable;
        if(kousin){
            python_mmap->send("draw");
            python_mmap->receive();
            this->setDrawFormat(python_mmap->getReveiveStr());
        }

    }
}

