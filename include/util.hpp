

#ifdef _UNICODE
#ifndef UNICODE
#define UNICODE
#endif
#endif

#include <random>
#include <sstream>
#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <iomanip>
#include <sstream>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

std::string wstringToString(std::wstring wstr_in);
std::vector<std::string> getFoldersAndFiles(std::wstring path);
namespace uuid{
    std::wstring generate_uuid_v4();
}
namespace mmap {
    static DWORD MEMORY_SIZE = 1048576;
    static DWORD SEND_MAX_MESSAGE_LENGTH = 1024000;
    static std::wstring MEMORY_FOLDER_NAME = L"mem";
    static bool SEND_OK = true;
    static bool SEND_NG = false;
    static bool RECEIVE_OK = true;
    static bool RECEIVE_NG = false;

    class mmap {
        private:
            std::wstring id;
            std::wstring send_mmap_file_name;
            std::wstring receive_mmap_file_name;
            std::wstring tmp_mmap_file_name;
            HANDLE send_hfile;
            HANDLE send_mmap_hfile;
            HANDLE receive_hfile;
            HANDLE receive_mmap_hfile;
            PWCHAR send_map_view;
            PWCHAR receive_map_view;
            unsigned int message_id;
            bool is_message_can_send;
            std::string received_string;
            PROCESS_INFORMATION python_process;

        public:
            mmap(std::wstring id);
            void init();
            void close();
            bool send(const char*);
            bool can_send();
            bool receive();
            std::string getReveiveStr();
    };
}

namespace imgui_ex{
    class window {
        private:
            std::string draw_format;
            std::map<std::string,std::string> variable;

        public:
            window();
            void setDrawFormat(std::string);
            void drawWindow(mmap::mmap*);
    };
}
