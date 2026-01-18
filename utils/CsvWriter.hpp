#pragma once

#include <fstream>
#include <string>
#include <vector>
#include <iostream>

namespace utils {

    class CsvWriter {
    public:
        explicit CsvWriter(const std::string& filename) : m_file(filename) {
            
            m_buffer.resize(65536);
            m_file.rdbuf()->pubsetbuf(m_buffer.data(), m_buffer.size());

            if (!m_file.is_open()) {
                std::cerr << "[Error] Could not open file: " << filename << std::endl;
            }

            m_file << "Time,PathId,Price\n";
        }

        ~CsvWriter() {
            if (m_file.is_open()) {
                m_file.close();
            }
        }

        void log_row(double time_in_days, int path_id, double price) {
            m_file << time_in_days << "," << path_id << "," << price << "\n";
        }

    private:
        std::ofstream m_file;
        std::vector<char> m_buffer;
    };
}