#include <iostream>
#include <filesystem>
#include <vector>
#include <string>
#include "BS_thread_pool.hpp"

namespace fs = std::filesystem;
using namespace std;

// 获取目录下的所有压缩包
vector<string> GetZipFiles(const string &dir) {
    vector<string> zipFiles;
    for (auto &p: fs::directory_iterator(dir)) {
        if (p.path().extension() == ".zip" || p.path().extension() == ".rar" || p.path().extension() == ".7z") {
            zipFiles.push_back(p.path().string());
        }
    }
    return zipFiles;
}

// 调用Bandizip.exe 批量解压压缩包
void UnzipFile(const string &zipFile, const string &dir) {
    // 创建临时目录，压缩包名_temp
    string tempDir = dir + "\\" + fs::path(zipFile).stem().string() + "_temp";
    if (fs::exists(tempDir))
        fs::remove_all(tempDir);
    fs::create_directory(tempDir);

    // 解压到临时目录
    //Bandizip x -o:C:\Users\zjz\Desktop\c++_temp C:\Users\zjz\Desktop\c++.zip
    string cmd = "bz.exe x -o:" + tempDir + " " + zipFile;
    system(cmd.c_str());
    cout << cmd << endl;
    // 统计临时目录下的文件数量
    int count = distance(fs::directory_iterator(tempDir), fs::directory_iterator{});
    if (count == 1) {
        cmd = "bz.exe x -y -aos -o:" + dir + " " + zipFile;
        system(cmd.c_str());
    } else {
        string unzipDir = dir + "\\" + fs::path(zipFile).stem().string();
        fs::create_directory(unzipDir);
        //输出tempDir下的文件数量
        cout << "tempdir: " << tempDir << " count: " << count << endl;
        for (auto &p: fs::directory_iterator(tempDir)) {
            fs::rename(p.path(), unzipDir + "\\" + p.path().filename().string());
        }
        //输出unzipDir下的文件数量
        cout << "unzipDir: " << unzipDir << " count: "
             << distance(fs::directory_iterator(unzipDir), fs::directory_iterator{}) << endl;
    }

    fs::remove_all(tempDir);
    fs::remove(zipFile);
}

void Unzip(const string &dir, int minThreadSize) {
    vector<string> zipFiles = GetZipFiles(dir);
    cout << "Found " << zipFiles.size() << " zip files. Unzipping..." << endl;
   // 创建线程池
    BS::thread_pool pool(minThreadSize);
    for (const auto &zipFile: zipFiles) {
        cout << "Processing zip file: " << zipFile << endl;
        pool.detach_task([zipFile, dir]() { UnzipFile(zipFile, dir); });
    }
    pool.wait();  // 使用 wait 等待所有任务完成
    cout << "Unzip completed." << endl;
}

bool CheckDirectory(const string &dir) {
    if (!fs::exists(dir)) {
        cout << "Directory does not exist: " << dir << endl;
        return false;
    }

    bool hasZip = false;
    for (auto &p: fs::directory_iterator(dir)) {
        if (p.path().extension() == ".zip" || p.path().extension() == ".rar" || p.path().extension() == ".7z") {
            hasZip = true;
            break;
        }
    }

    if (!hasZip) {
        cout << "No zip/rar/7z file found in directory: " << dir << std::endl;
        return false;
    }
    return true;
}

int main(int argc, char *argv[]) {
    system("chcp 65001");
    if (argc < 2) {
        cout << "Usage: " << argv[0] << " <dir> [minThreadSize defualt 4]" << endl;
        return 1;
    }
    if (!CheckDirectory(argv[1]))
        return 1;
    int minThreadSize = 4;
    if (argc == 3) {
        minThreadSize = stoi(argv[2]);
    }
    CheckDirectory(argv[1]);
    Unzip(argv[1], minThreadSize);
    return 0;
}
