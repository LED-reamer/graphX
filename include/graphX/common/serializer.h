#pragma once

#include <fstream>

namespace gx
{
	namespace serializer
	{
        template<typename T>
        void save(T obj, std::string filename)
        {
            std::ofstream file(filename, std::ios::binary);
            if (!file)
                return;

            file.write(reinterpret_cast<const char*>(&obj), sizeof(obj));

            if (!file)
                return;
        }

        template<typename T>
        T load(std::string filename) {
            T obj;
            std::ifstream file(filename, std::ios::binary);
            if (file) {
                file.read(reinterpret_cast<char*>(&obj), sizeof(T));
            }
            file.close();
            return obj;
        }
	}
}