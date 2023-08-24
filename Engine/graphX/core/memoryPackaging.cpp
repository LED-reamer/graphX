#include "graphX/core/memoryPackaging.h"

#include <fstream>
#include <utility>
#include <future>

#include "graphX/core/debug.h"

#include "graphX/internal/internalTypes.h"

#include "graphX/vendor/miniz/miniz.h"
#include "graphX/vendor/stb_image/stb_image.h"
#include "graphX/vendor/audio/loadWav.h"


void gx::mem::addPackageTexture(gx::MemoryStorage* storage, std::string virtualPath, std::string assetPath, textureAttributes::target target, textureAttributes::wrapping wrapping, textureAttributes::filtering filtering, textureAttributes::format formatInternal, textureAttributes::dataType dataType, gx::textureAttributes::format imageFormat)
{
    memoryTexture texture{ };
    texture.target = target;
    texture.formatInternal = formatInternal;
    texture.format = imageFormat;
    texture.dataType = dataType;
    texture.filtering = filtering;
    texture.wrapping = wrapping;
    texture.depth = 0;
    texture.channels = 0;
    texture.levels = 0;

    int _nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(assetPath.c_str(), &texture.width, &texture.height, &texture.channels, 0);
    

    if (data)
    {
        texture.data = std::vector<uint8_t>();
        texture.data.reserve(texture.width * texture.height * texture.channels);
        std::copy(data, data + texture.width * texture.height * texture.channels, std::back_inserter(texture.data));

        unsigned int _imageFormat = gx::textureAttributes::format::rgba;
        if (imageFormat == gx::textureAttributes::automatically)
        {
            if (texture.channels == 1) _imageFormat = gx::textureAttributes::format::red;
            if (texture.channels == 3) _imageFormat = gx::textureAttributes::format::rgb;
            if (texture.channels == 4) _imageFormat = gx::textureAttributes::format::rgba;
        }
        else _imageFormat = imageFormat;
        texture.format = _imageFormat;
    }
    else
    {
        gx::debug::logError(std::string("there was an error when loading the texture at: " + assetPath));
    }
    stbi_image_free(data);
    gx::mem::addPackage<memoryTexture>(storage, &texture, virtualPath);
}

void gx::mem::addPackageSound(gx::MemoryStorage* storage, std::string virtualPath, std::string assetPath)
{
    memorySound sound;
    

    float* sampleData;
    size_t dataCount;

    std::string fileExtension = assetPath.substr(assetPath.find_last_of(".") + 1);
    if (fileExtension == "wav" || true)// ALWAYS TRUE EXTENSION JUST FOR FUTURE
    {
        drwav wav;
        if (!drwav_init_file(&wav, assetPath.c_str(), NULL))
        {
            gx::debug::logError(std::string("Could not open ") + assetPath);
        }

        sound.channels = wav.channels;
        sound.bitsPerSample = wav.bitsPerSample;
        sound.sampleRate = wav.sampleRate;
        
        
        sampleData = (float*)malloc(wav.totalPCMFrameCount * wav.channels * sizeof(float) * (sound.bitsPerSample == 16 ? 2 : 1));

        drwav_read_raw(&wav, wav.totalPCMFrameCount * wav.channels * sizeof(float), sampleData);

        dataCount = wav.totalPCMFrameCount * wav.channels * (sound.bitsPerSample == 16 ? 2 : 1);

        std::vector<float> copy(sampleData, sampleData + dataCount);
        sound.data = copy;
        copy.clear();
        //sound.data = std::vector<float>();
        //sound.data.reserve(dataCount+1);
        //std::copy(sampleData, sampleData + dataCount, std::back_inserter(sound.data));


        drwav_uninit(&wav);


        if (sound.channels == 1 && sound.bitsPerSample == 8)
            sound.format = 0x1100;
        else if (sound.channels == 1 && sound.bitsPerSample == 16)
            sound.format = 0x1101;
        else if (sound.channels == 2 && sound.bitsPerSample == 8)
            sound.format = 0x1102;
        else if (sound.channels == 2 && sound.bitsPerSample == 16)
            sound.format = 0x1103;
        else
        {
            gx::debug::logError(std::string("unrecognised wave format: ") + std::to_string(sound.channels) + std::string("channels, ") + std::to_string(sound.bitsPerSample) + std::string("bps"));
        }

        
    }
    free(sampleData);

    gx::mem::addPackage<memorySound>(storage, &sound, virtualPath);
}

void gx::mem::addPackageShader(gx::MemoryStorage* storage, std::string virtualPath, std::string assetPath)
{
    enum class ShaderType
    {
        Vertex = GL_VERTEX_SHADER,
        Fragment = GL_FRAGMENT_SHADER,
        Geometry = GL_GEOMETRY_SHADER,
        TessellationControl = GL_TESS_CONTROL_SHADER,
        TessellationEvaluation = GL_TESS_EVALUATION_SHADER,
        Compute = GL_COMPUTE_SHADER
    };

    memoryShader shader{ };
    
    std::ifstream shaderFile(assetPath);
    std::string line;

    std::stringstream shaderSourceVertex;
    std::stringstream shaderSourceFragment;
    std::stringstream shaderSourceGeometry;
    std::stringstream shaderSourceTessellationControl;
    std::stringstream shaderSourceTessellationEvaluation;
    std::stringstream shaderSourceCompute;

    ShaderType currentShaderType = ShaderType::Vertex;

    if (shaderFile.fail())
    {
        debug::logError(std::string("Could not load shader: ") + assetPath);
        return;
    }

    while (std::getline(shaderFile, line))
    {
        if (line.find("#shader") != std::string::npos)
        {
            if (line.find("vertex") != std::string::npos)
                currentShaderType = ShaderType::Vertex;
            else if (line.find("fragment") != std::string::npos)
                currentShaderType = ShaderType::Fragment;
            else if (line.find("geometry") != std::string::npos)
                currentShaderType = ShaderType::Geometry;
            else if (line.find("tessellation_control") != std::string::npos)
                currentShaderType = ShaderType::TessellationControl;
            else if (line.find("tessellation_evaluation") != std::string::npos)
                currentShaderType = ShaderType::TessellationEvaluation;
            else if (line.find("compute") != std::string::npos)
                currentShaderType = ShaderType::Compute;
        }
        else
        {
            switch (currentShaderType)
            {
            case ShaderType::Vertex:
                shaderSourceVertex << line << "\n";
                break;
            case ShaderType::Fragment:
                shaderSourceFragment << line << "\n";
                break;
            case ShaderType::Geometry:
                shaderSourceGeometry << line << "\n";
                break;
            case ShaderType::TessellationControl:
                shaderSourceTessellationControl << line << "\n";
                break;
            case ShaderType::TessellationEvaluation:
                shaderSourceTessellationEvaluation << line << "\n";
                break;
            case ShaderType::Compute:
                shaderSourceCompute << line << "\n";
                break;
            default:
                break;
            }
        }
    }

    shader.sources.push_back(shaderSourceVertex.str());
    shader.sources.push_back(shaderSourceFragment.str());
    shader.sources.push_back(shaderSourceGeometry.str());
    shader.sources.push_back(shaderSourceTessellationControl.str());
    shader.sources.push_back(shaderSourceTessellationEvaluation.str());
    shader.sources.push_back(shaderSourceCompute.str());

    gx::mem::addPackage<memoryShader>(storage, &shader, virtualPath);
}

void gx::mem::addPackageMaterial(gx::MemoryStorage* storage, std::string virtualPath, gx::Material* material, std::string colorTexturePath, std::string normalTexturePath, std::string metallicTexturePath, std::string roughnessTexturePath, std::string ambientOcclusionTexturePath)
{
    memoryMaterial memMaterial;
    memMaterial.name = material->name;
    
    memMaterial.colorR = material->color.r;
    memMaterial.colorG = material->color.g;
    memMaterial.colorB = material->color.b;
    memMaterial.metallic = material->metallic;
    memMaterial.roughness = material->roughness;
    
    if(!colorTexturePath.empty())
        gx::mem::addPackageTexture(storage, virtualPath + ".colorTexture", colorTexturePath);
    if(!normalTexturePath.empty())
        gx::mem::addPackageTexture(storage, virtualPath + ".normalTexture", normalTexturePath);
    if (!metallicTexturePath.empty())
        gx::mem::addPackageTexture(storage, virtualPath + ".metallicTexture", metallicTexturePath);
    if (!roughnessTexturePath.empty())
        gx::mem::addPackageTexture(storage, virtualPath + ".roughnessTexture", roughnessTexturePath);
    if (!ambientOcclusionTexturePath.empty())
        gx::mem::addPackageTexture(storage, virtualPath + ".ambientOcclusionTexture", ambientOcclusionTexturePath);
    
    gx::mem::addPackage<memoryMaterial>(storage, &memMaterial, virtualPath);
}

void gx::mem::saveToFile(gx::MemoryStorage* storage, std::string path)
{
    std::ofstream outFile(path, std::ios::binary);
    if (!outFile)
    {
        gx::debug::logError("Could not open file for writing: " + path);
        return;
    }

    size_t storageSize = storage->packages.size();
    outFile.write(reinterpret_cast<const char*>(&storageSize), sizeof(size_t));

    for (const auto& package : storage->packages)
    {
        size_t nameSize = package.first.size();
        outFile.write(reinterpret_cast<const char*>(&nameSize), sizeof(size_t));
        outFile.write(package.first.c_str(), nameSize);

        size_t dataSize = package.second.size();
        outFile.write(reinterpret_cast<const char*>(&dataSize), sizeof(size_t));
        outFile.write(reinterpret_cast<const char*>(package.second.data()), dataSize);
    }

    outFile.close();

    //compress
    //compressFile(path.c_str());
}

gx::MemoryStorage gx::mem::loadFromFile(std::string path)
{
    // decompress
    //decompressFile(path.c_str());



    gx::MemoryStorage storage;

    std::ifstream inFile(path, std::ios::binary);
    if (!inFile)
    {
        gx::debug::logError("Could not open file: " + path);
        return storage;
    }

    size_t storageSize;
    inFile.read(reinterpret_cast<char*>(&storageSize), sizeof(size_t));

    for (size_t i = 0; i < storageSize; ++i)
    {
        size_t nameSize;
        inFile.read(reinterpret_cast<char*>(&nameSize), sizeof(size_t));

        std::string packageName(nameSize, '\0');
        inFile.read(&packageName[0], nameSize);

        size_t dataSize;
        inFile.read(reinterpret_cast<char*>(&dataSize), sizeof(size_t));

        std::vector<uint8_t> packageData(dataSize);
        inFile.read(reinterpret_cast<char*>(packageData.data()), dataSize);

        storage.packages.emplace_back(packageName, packageData);
    }

    inFile.close();

    //compress it again
    //compressFile(path.c_str());

    return storage;
}

void gx::mem::clearPackages(gx::MemoryStorage* storage)
{
	storage->packages.clear();
}

void gx::mem::compressFile(const char* path)
{
    mz_zip_archive zip;
    memset(&zip, 0, sizeof(zip));
    mz_zip_writer_init_file(&zip, path, 0);
    mz_zip_writer_add_file(&zip, "data", path, NULL, 0, NULL);
    mz_zip_writer_finalize_archive(&zip);
    mz_zip_writer_end(&zip);
}

void gx::mem::decompressFile(const char* path)
{
    mz_zip_archive zip2;
    memset(&zip2, 0, sizeof(zip2));
    mz_zip_reader_init_file(&zip2, path, 0);
    mz_zip_reader_extract_to_file(&zip2, 0, path, 0);
    mz_zip_reader_end(&zip2);
}