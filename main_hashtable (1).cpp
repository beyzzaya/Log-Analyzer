/* 25.12.23 DATA STRUCTURES & ALGORITHMS TERM PROJECT
 * Berika DİKİCİ 2200424
 * Burak AKCAY 2102816
 * Beyza YAYLACIOGLU 2001308
 * Zeren KORKMAZ 2101901
 */

#include <string>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <chrono>
#include <filesystem>
#include <vector>


using namespace std;
using namespace chrono;

//implementation of the hash function
struct custom_hash_function {
    size_t operator()(const string& key) const {
        size_t hash = 99999;
        for (char c : key) {
            hash = (hash << 5) + hash + c;
        }
        return hash;
    }
};

class hash_table {
public:
    explicit hash_table(size_t size) : table(size), size(size), count(0) {}
    void insert(const string& key) {
        size_t index = hashFunc(key) % size;

        //using linear probing technique in order to prevent collisions (detailed explanation in report)
        while (!table[index].first.empty() && table[index].first != key) {
            index = (index + 1) % size;
        }

        //adding the key value
        table[index].first = key;
        table[index].second++;

        // increasing the size of the table when its 80 percent full
        if (++count > size * 0.8) {
            rehash();
        }
    }

    vector<pair<string, int>> getTopItems(size_t count) const {
        vector<pair<string, int>> result;

        for (const auto& entry : table) {
            if (!entry.first.empty()) {
                result.push_back(entry);
            }
        }

        // Sort the result vector based on visit counts
        sort(result.begin(), result.end(), [](const auto& a, const auto& b) {
            return a.second > b.second;
        });

        // Return the top items
        return vector<pair<string, int>>(result.begin(), result.begin() + min(count, result.size()));
    }

private:
    vector<pair<string, int>> table;
    custom_hash_function hashFunc;
    size_t size;
    size_t count;

    void rehash() {
        size_t new_size = size * 2;
        vector<pair<string, int>> new_table(new_size);

        for (const auto& entry : table) {
            if (!entry.first.empty()) {
                size_t index = hashFunc(entry.first) % new_size;

                while (!new_table[index].first.empty()) {
                    index = (index + 1) % new_size;
                }

                new_table[index] = entry;
            }
        }

        table = move(new_table);
        size = new_size;
        count = 0;
    }
};

int main() {
    //input log file path
    string input_file_path = R"(C:\Users\SBR\Desktop\access_log)";
    //process start time
    auto proc_start_time = high_resolution_clock::now();
    //initiliaze table with size 200, hash_table will increase size if needed
    hash_table web_request_counts(200);
    //check if input log file exists
    ifstream input_file(input_file_path);
    if (filesystem::exists(input_file_path)) {
        uintmax_t fileSize = filesystem::file_size(input_file_path);
        cout << "Size of access_log file: " << fileSize << " bytes" << std::endl;
    }
    else {
        cerr << "Error: File not found." << std::endl;
    }
    //read log file and insert to hash table
    string line;
    while (getline(input_file, line)) {
        int reqest_start_pos = line.find("\"");
        if (reqest_start_pos != string::npos) {
            int request_end_pos = line.find("\"", reqest_start_pos + 1);
            if (request_end_pos != string::npos) {
                string request = line.substr(reqest_start_pos + 1, request_end_pos - reqest_start_pos - 1);
                int url_start_pos = request.find(" ");
                if (url_start_pos != string::npos) {
                    string url = request.substr(url_start_pos + 1);
                    int url_end_pos = url.find(" ");
                    if (url_end_pos != string::npos) {
                        url = url.substr(0, url_end_pos);
                        web_request_counts.insert(url);
                    }
                }
            }
        }
    }
    //find out top 10 most visited web sites
    vector<pair<string, int>> top_pages = web_request_counts.getTopItems(10);
    //print top 10 pages
    cout << "------------------------------" << std::endl;
    cout << "Top 10 Most Visited Pages:" << endl;
    for (const auto& entry : top_pages) {
        cout << entry.first << " => number of request times " << entry.second << endl;
    }
    //process end time
    auto proc_end_time = high_resolution_clock::now();
    auto proc_elapsed_time = duration_cast<milliseconds>(proc_end_time - proc_start_time);
    cout << "------------------------------" << std::endl;
    cout << "Total Elapsed Time: " << proc_elapsed_time.count() << " milliseconds" << endl;




    return 0;
}