#pragma once
#include <vector>
#include <string>
#include <sstream>

#include "graphX/core/types.h"
#include "graphX/core/debug.h"

#include "graphX/vendor/alpaca/alpaca.h"

namespace gx
{
	namespace mem
	{
		template <typename T>
		void addPackage(gx::MemoryStorage* storage, T* package, std::string virtualPath)
		{
			std::vector<uint8_t> bytes;
			size_t bytes_written = alpaca::serialize(*package, bytes);
			storage->packages.push_back(std::make_pair(virtualPath, bytes));
		}

		void addPackageTexture(gx::MemoryStorage* storage, std::string virtualPath, std::string assetPath, gx::textureAttributes::target target = textureAttributes::target::texture2d, gx::textureAttributes::wrapping wrapping = textureAttributes::wrapping::repeat, textureAttributes::filtering filtering = textureAttributes::filtering::nearest, textureAttributes::format formatInternal = textureAttributes::format::rgba8, textureAttributes::dataType dataType = textureAttributes::dataType::uByte, gx::textureAttributes::format imageFormat = gx::textureAttributes::automatically);
		void addPackageSound(gx::MemoryStorage* storage, std::string virtualPath, std::string assetPath);
		void addPackageShader(gx::MemoryStorage* storage, std::string virtualPath, std::string assetPath);
		void addPackageMaterial(gx::MemoryStorage* storage, std::string virtualPath, gx::Material* material, std::string colorTexturePath = "", std::string normalTexturePath = "", std::string metallicTexturePath = "", std::string roughnessTexturePath = "", std::string ambientOcclusionTexturePath = "");

		
		void saveToFile(gx::MemoryStorage* storage, std::string path);
		
		gx::MemoryStorage loadFromFile(std::string path);

		//TODO return false if could not load -> loading material textures safer
		template <typename T>
		T getPackage(gx::MemoryStorage* storage, std::string virtualPath)
		{
			for (size_t i = 0; i < storage->packages.size(); i++)
			{
				if (storage->packages[i].first == virtualPath)
				{
					std::vector<uint8_t> data = storage->packages[i].second;
					std::error_code ec;

					
					return alpaca::deserialize<T>(data, ec);
					
					if (ec)
						return {};
				}
			}

			gx::debug::logErrorStatic(std::string("Could not find virtual path: ") + virtualPath);
			return{};
		}

		void clearPackages(gx::MemoryStorage* storage);

		//TODO COMPRESSION
		void compressFile(const char* path);
		void decompressFile(const char* path);
	}
}