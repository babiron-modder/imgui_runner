#include <iostream>
#include <string>
#include "util.hpp"

int main(int argc, char* argv[]) {
    // GLFWの初期化
    if (!glfwInit()) {
        return -1;
    }
    GLFWwindow* window = glfwCreateWindow(640, 480, "Tool", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    // gl3wの初期化
    gl3wInit();

    // ImGuiの初期化
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();

    // フォントの設定
    ImGuiIO& io = ImGui::GetIO();
    ImFontGlyphRangesBuilder builder;
    ImVector<ImWchar> ranges;
    builder.AddText(
        "―‐∥…‥‘’“”≠≦≧∞∴♂♀′″℃☆★○●◎◇◆□■△▲▽▼※→←↑↓∈∋"
        "⊆⊇⊂⊃∪∩∧∨⇒⇔∀∃∠⊥⌒∂∇≡≒≪≫√∽∝∵∫∬Å‰♯♭♪†‡◯ΑΒΓΔΕΖΗΘΙΚΛΜΝ"
        "ΞΟΠΡΣΤΥΦΧΨΩαβγδεζηθικλμνξοπρστυφχψωАБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯаб"
        "вгдеёжзийклмнопрстуфхцчшщъыьэюя─│┌┐┘└├┬┤┴┼━┃┏┓┛┗┣┳┫"
        "┻╋┠┯┨┷┿┝┰┥┸╂\uf85b\uf016\uf114\uf15b");
    builder.AddRanges(io.Fonts->GetGlyphRangesChineseFull());
    builder.BuildRanges(&ranges);
    io.Fonts->AddFontFromFileTTF("font/PlemolJP35ConsoleNF-Light.ttf", 15.f, nullptr,
                                 ranges.Data);

    // Pythonのアプリ一覧を取得する
    std::wstring program_folder_path = L"program";
    std::vector<std::string> files = getFoldersAndFiles(program_folder_path);

    // ファイルの初期化をしておく

    // シグナル検知は入れない


    // メモリマップとツールの描画準備
    mmap::mmap *python_mmap = nullptr;
    imgui_ex::window tool_window;

    // メインループ
    while (!glfwWindowShouldClose(window)) {
        // バッファのクリア
        glClearColor(0.9375f, 0.9375f, 0.9375f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Imguiの初期化
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // カラーテーマ
        ImGui::StyleColorsLight();

        // 各種変数
        static bool file_select_window_is_shown = true;
        static char selected_filename[300] = "";

        // 右のアイコン
        ImGui::BeginMainMenuBar();
        ImGui::Dummy(ImVec2(ImGui::GetWindowWidth() -35, 1));
        if (ImGui::BeginMenu("\uf85b ")) {
            if (ImGui::MenuItem("実行ファイルを選択する           ")) {
                file_select_window_is_shown = true;
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();


        // ================ ファイル選択画面 ================
        if (file_select_window_is_shown) {
            ImGui::SetNextWindowPos(ImVec2(320, 240), ImGuiCond_Once,
                                    ImVec2(0.5, 0.5));
            ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_Once);
            // window表示
            ImGui::Begin("ファイル選択", &file_select_window_is_shown, ImGuiWindowFlags_NoTitleBar);
            ImGui::Text("■ 実行するファイルを選択してください");
            ImGui::Separator();
            // ファイル一覧(テーブル)
            if (ImGui::BeginTable("##table_folder", 4, ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_ScrollY, ImVec2(0.0f, -33.0f))) {
                ImGui::TableSetupColumn(" ", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("ファイル", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("作成日時", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("サイズ", ImGuiTableColumnFlags_WidthFixed);
                static int id = -1;
                char temp_label[300]; // 使いまわし用ラベル
                for (int row = 0; row < files.size(); row+=3)
                {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    if(id != row){
                        ImGui::Text(" \uf016 ");
                    }else{
                        ImGui::Text(" \uf15b ");
                    }
                    ImGui::TableSetColumnIndex(1);
                    sprintf(temp_label, files[row].c_str());
                    if(ImGui::Selectable(temp_label, id == row,ImGuiSelectableFlags_SpanAllColumns)){
                        id = row;
                        sprintf(selected_filename, temp_label);
                    }
                    ImGui::TableSetColumnIndex(2);
                    sprintf(temp_label, files[row+1].c_str());
                    ImGui::Text(temp_label);

                    ImGui::TableSetColumnIndex(3);
                    sprintf(temp_label, files[row+2].c_str());
                    ImGui::Text(temp_label);
                }
                ImGui::EndTable();
            }
            ImGui::Separator();
            
            // ファイル名表示欄
            ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
            ImGui::PushStyleColor(ImGuiCol_Button,
                                  ImVec4{0.879f, 0.879f, 0.879f, 1.0f});
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                                  ImVec4{0.664f, 0.797f, 0.953f, 1.0f});
            ImGui::PushStyleColor(ImGuiCol_ButtonActive,
                                  ImVec4{0.564f, 0.697f, 0.953f, 1.0f});
            ImGui::PushStyleColor(ImGuiCol_FrameBg,
                                  ImVec4{0.900f, 0.900f, 0.900f, 1.0f});
            ImGui::Dummy(ImVec2(0,0.1f));
            ImGui::SetNextItemWidth(-43.0f);
            ImGui::InputText("##selected_filename",selected_filename, sizeof(selected_filename), ImGuiInputTextFlags_ReadOnly);
            ImGui::SameLine();

            if(ImGui::Button("実行") && selected_filename[0] != '\0'){
                if(python_mmap != nullptr){
                    python_mmap -> close();
                }
                // selected_filename を実行する
                python_mmap = new mmap::mmap(uuid::generate_uuid_v4());
                // Init処理
                if(python_mmap != nullptr){
                    // initを実行
                    python_mmap->send(selected_filename);
                    python_mmap->receive();
                    std::cout << python_mmap->getReveiveStr() << std::endl;

                    // drawを実行
                    python_mmap->send("draw");
                    python_mmap->receive();
                    tool_window.setDrawFormat(python_mmap->getReveiveStr());

                    file_select_window_is_shown = false;
                }
            }

            ImGui::PopStyleColor(4);
            ImGui::PopStyleVar(1);

            ImGui::End();
        }
        // ================================================
        

        // ツールの描画
        tool_window.drawWindow(python_mmap);

        // Imguiの画面更新
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // ダブルバッファのスワップ
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // ImGuiの終了処理
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // GLFWの終了処理
    glfwTerminate();

    if(python_mmap != nullptr){
        python_mmap -> close();
    }

    return 0;
}
