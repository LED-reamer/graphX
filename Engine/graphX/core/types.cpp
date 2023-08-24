#include "types.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <thread>

#include "graphX/vendor/GL/glew.h"
#include "graphX/vendor/glm/gtc/type_ptr.hpp"
#include "graphX/vendor/stb_image/stb_image.h"
#include "graphX/vendor/AL/al.h"
#include "graphX/vendor/AL/alc.h"
#include "graphX/vendor/audio/loadWav.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "graphX/internal/context.h"
#include "graphX/internal/internalTypes.h"

#include "graphX/core/window.h"
#include "graphX/core/memoryPackaging.h"
#include "graphX/core/debug.h"
#include "graphX/core/2d/renderer2d.h"


// -- MATRIX

gx::Matrix gx::matrixCreate(float x)
{
    return gx::Matrix(x);
}
gx::Matrix gx::matrixTranslate(Matrix* matrix, Vec3 translation)
{
    return glm::translate(*matrix, translation);
}
gx::Matrix gx::matrixRotate(Matrix* matrix, Vec3 rotation)
{
    *matrix = glm::rotate(*matrix, glm::radians(rotation.x), gx::Vec3(1, 0, 0));
    *matrix = glm::rotate(*matrix, glm::radians(rotation.y), gx::Vec3(0, 1, 0));
    *matrix = glm::rotate(*matrix, glm::radians(rotation.z), gx::Vec3(0, 0, 1));
    return *matrix;
}
gx::Matrix gx::matrixScale(Matrix* matrix, Vec3 scale)
{
    return glm::scale(*matrix, scale);
}
gx::Matrix gx::matrixLookAt(Vec3 position, Vec3 target, Vec3 up)
{
    return glm::lookAt(position, target, up);
}
gx::Matrix gx::matrixProjectOrthographic(float leftX, float rightX, float bottomY, float topY, float nearZ, float farZ)
{
    return glm::ortho(leftX, rightX, bottomY, topY, nearZ, farZ);
}
gx::Matrix gx::matrixProjectPerspective(float fov, float nearZ, float farZ, unsigned int width, unsigned int height)
{
    if (height == 0) height = 1;

    return glm::perspective(fov, (float)width/height, nearZ, farZ);
}

// -- MESH
gx::Mesh gx::meshCreate(std::vector<VertexAttribute> vertexAttributes)
{
    Mesh mesh;
    glGenVertexArrays(1, &mesh.VA);
    glBindVertexArray(mesh.VA);

    glGenBuffers(1, &mesh.VB);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.VB);

    //count vertex layout size
    for (int i = 0; i < vertexAttributes.size(); i++)
    {
        switch (vertexAttributes[i])
        {
        case float1:
            mesh.vertexLayoutSize += sizeof(float);
            break;
        case float2:
            mesh.vertexLayoutSize += sizeof(float) * 2;
            break;
        case float3:
            mesh.vertexLayoutSize += sizeof(float) * 3;
            break;
        case float4:
            mesh.vertexLayoutSize += sizeof(float) * 4;
            break;
        case int1:
            mesh.vertexLayoutSize += sizeof(int);
            break;
        case uint1:
            mesh.vertexLayoutSize += sizeof(unsigned int);
            break;
        case float1Norm:
            mesh.vertexLayoutSize += sizeof(GLbyte); // GLbyte for GL_BYTE normalized
            break;
        case float2Norm:
            mesh.vertexLayoutSize += sizeof(GLbyte) * 2;
            break;
        case float3Norm:
            mesh.vertexLayoutSize += sizeof(GLbyte) * 3;
            break;
        case float4Norm:
            mesh.vertexLayoutSize += sizeof(GLbyte) * 4;
            break;
        case int1Norm:
            mesh.vertexLayoutSize += sizeof(GLint); // GLint for GL_INT normalized
            break;
        case uint1Norm:
            mesh.vertexLayoutSize += sizeof(GLuint); // GLuint for GL_UNSIGNED_INT normalized
            break;
        default:
            break;
        }
    }


    // apply vertex layout
    size_t offset = 0;
    for (int i = 0; i < vertexAttributes.size(); i++)
    {
        switch (vertexAttributes[i])
        {
        case float1:
            glEnableVertexAttribArray(i);
            glVertexAttribPointer(i, 1, GL_FLOAT, GL_FALSE, mesh.vertexLayoutSize, (const void*)offset);
            offset += sizeof(float);
            break;
        case float2:
            glEnableVertexAttribArray(i);
            glVertexAttribPointer(i, 2, GL_FLOAT, GL_FALSE, mesh.vertexLayoutSize, (const void*)offset);
            offset += sizeof(float) * 2;
            break;
        case float3:
            glEnableVertexAttribArray(i);
            glVertexAttribPointer(i, 3, GL_FLOAT, GL_FALSE, mesh.vertexLayoutSize, (const void*)offset);
            offset += sizeof(float) * 3;
            break;
        case float4:
            glEnableVertexAttribArray(i);
            glVertexAttribPointer(i, 4, GL_FLOAT, GL_FALSE, mesh.vertexLayoutSize, (const void*)offset);
            offset += sizeof(float) * 4;
            break;
        case int1:
            glEnableVertexAttribArray(i);
            glVertexAttribPointer(i, 1, GL_INT, GL_FALSE, mesh.vertexLayoutSize, (const void*)offset);
            offset += sizeof(int);
            break;
        case uint1:
            glEnableVertexAttribArray(i);
            glVertexAttribPointer(i, 1, GL_UNSIGNED_INT, GL_FALSE, mesh.vertexLayoutSize, (const void*)offset);
            offset += sizeof(unsigned int);
            break;
        case float1Norm:
            glEnableVertexAttribArray(i);
            glVertexAttribPointer(i, 1, GL_BYTE, GL_TRUE, mesh.vertexLayoutSize, (const void*)offset);
            offset += sizeof(GLbyte);
            break;
        case float2Norm:
            glEnableVertexAttribArray(i);
            glVertexAttribPointer(i, 2, GL_BYTE, GL_TRUE, mesh.vertexLayoutSize, (const void*)offset);
            offset += sizeof(GLbyte) * 2;
            break;
        case float3Norm:
            glEnableVertexAttribArray(i);
            glVertexAttribPointer(i, 3, GL_BYTE, GL_TRUE, mesh.vertexLayoutSize, (const void*)offset);
            offset += sizeof(GLbyte) * 3;
            break;
        case float4Norm:
            glEnableVertexAttribArray(i);
            glVertexAttribPointer(i, 4, GL_BYTE, GL_TRUE, mesh.vertexLayoutSize, (const void*)offset);
            offset += sizeof(GLbyte) * 4;
            break;
        case int1Norm:
            glEnableVertexAttribArray(i);
            glVertexAttribPointer(i, 1, GL_INT, GL_TRUE, mesh.vertexLayoutSize, (const void*)offset);
            offset += sizeof(GLint);
            break;
        case uint1Norm:
            glEnableVertexAttribArray(i);
            glVertexAttribPointer(i, 1, GL_UNSIGNED_INT, GL_TRUE, mesh.vertexLayoutSize, (const void*)offset);
            offset += sizeof(GLuint);
            break;
        default:
            break;
        }
    }

    glGenBuffers(1, &mesh.EB);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    return mesh;
}

void gx::meshSetData(gx::Mesh* mesh, void* vertices, size_t length0, unsigned int* indices, size_t length1)
{
    glBindBuffer(GL_ARRAY_BUFFER, mesh->VB);
    glBufferData(GL_ARRAY_BUFFER, length0 * mesh->vertexLayoutSize, vertices, GL_STATIC_DRAW);//GL_STREAM_DRAW, STATIC ???

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->EB);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, length1 * sizeof(unsigned int), indices, GL_STATIC_DRAW);//GL_STREAM_DRAW, STATIC ???

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    mesh->indicesCount = length1;
}

void gx::meshDraw(Mesh* mesh)
{
    if (mesh->indicesCount == 0 || mesh->VA == 0) return;

    glBindVertexArray(mesh->VA);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->EB);
    glDrawElements(GL_TRIANGLES, mesh->indicesCount, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void gx::meshDrawInstanced(Mesh* mesh, unsigned int count)
{
    if (mesh->indicesCount == 0 || mesh->VA == 0) return;

    glBindVertexArray(mesh->VA);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->EB);
    glDrawElementsInstanced(GL_TRIANGLES, mesh->indicesCount, GL_UNSIGNED_INT, 0, count);

    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    /* !!!
    * 
    * colors, scales, textures, animation, object-id, materials, user defined ATTRIBUTES PER INSTANCE
    * 
    * 
    // Generate instance data (transformation matrices)
    glm::mat4 instanceTransforms[10];
    // ... Initialize instanceTransforms array with transformation matrices for each instance

    GLuint instanceVBO;
    glGenBuffers(1, &instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * 10, &instanceTransforms[0], GL_STATIC_DRAW);

    // Set instance data attribute pointer (mat4 = 4 vec4)
    for (GLuint i = 0; i < 4; ++i)
    {
        glEnableVertexAttribArray(2 + i);
        glVertexAttribPointer(2 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4) * i));
        glVertexAttribDivisor(2 + i, 1); // Set the divisor to 1 to indicate per-instance data
    }

    // ...

    // Bind VAO, draw instances
    glBindVertexArray(VAO);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 3, 10); // Draw 10 instances of the triangle TEN IS LEN OF instanceTransforms !!!
    */
}

void gx::meshDelete(gx::Mesh* mesh)
{
    glDeleteVertexArrays(1, &mesh->VA);
    glDeleteVertexArrays(1, &mesh->VB);
    glDeleteVertexArrays(1, &mesh->EB);
}



// -- MATERIAL

gx::Material gx::materialLoad(gx::MemoryStorage* storage, std::string virtualPath)
{
    gx::mem::memoryMaterial memoryMaterial = gx::mem::getPackage<gx::mem::memoryMaterial>(storage, virtualPath);
    gx::Material material;
    
    material.name = memoryMaterial.name;
    
    material.color.r = memoryMaterial.colorR;
    material.color.g = memoryMaterial.colorG;
    material.color.b = memoryMaterial.colorB;
    material.metallic = memoryMaterial.metallic;
    material.roughness = memoryMaterial.roughness;
    
    
    ctx3d.defaultMaterialTextures.push_back(gx::textureLoad(storage, virtualPath + ".colorTexture"));
    ctx3d.defaultMaterialTextures.push_back(gx::textureLoad(storage, virtualPath + ".normalTexture"));
    ctx3d.defaultMaterialTextures.push_back(gx::textureLoad(storage, virtualPath + ".metallicTexture"));
    ctx3d.defaultMaterialTextures.push_back(gx::textureLoad(storage, virtualPath + ".roughnessTexture"));
    ctx3d.defaultMaterialTextures.push_back(gx::textureLoad(storage, virtualPath + ".ambientOcclusionTexture"));

    //set pointers
    material.colorTexture = &ctx3d.defaultMaterialTextures[0];
    material.normalTexture = &ctx3d.defaultMaterialTextures[1];
    material.metallicTexture = &ctx3d.defaultMaterialTextures[2];
    material.roughnessTexture = &ctx3d.defaultMaterialTextures[3];
    material.ambientOcclusionTexture = &ctx3d.defaultMaterialTextures[4];

    return material;
}



// -- MODEL

gx::Model gx::modelLoadFromFile(std::string path)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        gx::debug::logError(importer.GetErrorString());
        return gx::Model();
    }

    gx::Model model;

    for (size_t i = 0; i < scene->mNumMeshes; ++i) {
        const aiMesh* mesh = scene->mMeshes[i];
        
        std::vector<gx::VertexAttribute> vertexAttributes;
        std::vector<float> vertices;
        std::vector<unsigned int> indices;

        unsigned int floatsPerVertex = 0;
        model.desc = "";
        //vertex attributes
        {
            if (mesh->HasPositions())
            {
                vertexAttributes.push_back(gx::float3);
                floatsPerVertex += 3;
                model.desc += "position;";
            }
            if (mesh->HasNormals())
            {
                vertexAttributes.push_back(gx::float3);
                floatsPerVertex += 3;
                model.desc += "normals;";
            }
            if (mesh->HasTangentsAndBitangents())
            {
                vertexAttributes.push_back(gx::float4);
                floatsPerVertex += 4;
                model.desc += "tangents;";
            }
            if (mesh->HasTextureCoords(0))//just checking for one attribute for now
            {
                vertexAttributes.push_back(gx::float2);
                floatsPerVertex += 2;
                model.desc += "textureCoord;";
            }
            if (mesh->HasVertexColors(0))//just checking for one attribute for now
            {
                vertexAttributes.push_back(gx::float4);
                floatsPerVertex += 4;
                model.desc += "color;";
            }
            if (mesh->HasBones())//TODO: handle animations and bones
            {
                //vertexAttributes.push_back(gx::float4);//joint
                //vertexAttributes.push_back(gx::float4);//weight
                //floatsPerVertex += 4;
                //floatsPerVertex += 4;
            }
        }

        //vertices
        vertices.resize(floatsPerVertex * mesh->mNumVertices);
        int smartIterator = 0;
        for (size_t i = 0; i < mesh->mNumVertices; i++)
        {
            if (mesh->HasPositions())
            {
                vertices[smartIterator++] = mesh->mVertices[i].x;
                vertices[smartIterator++] = mesh->mVertices[i].y;
                vertices[smartIterator++] = mesh->mVertices[i].z;
            }
            if (mesh->HasNormals())
            {
                vertices[smartIterator++] = mesh->mNormals[i].x;
                vertices[smartIterator++] = mesh->mNormals[i].y;
                vertices[smartIterator++] = mesh->mNormals[i].z;
            }
            if (mesh->HasTangentsAndBitangents())
            {
                vertices[smartIterator++] = mesh->mTangents[i].x;
                vertices[smartIterator++] = mesh->mTangents[i].y;
                vertices[smartIterator++] = mesh->mTangents[i].z;
            }
            if (mesh->HasTextureCoords(0))//just checking for one attribute for now
            {
                vertices[smartIterator++] = mesh->mTextureCoords[0][i].x;
                vertices[smartIterator++] = mesh->mTextureCoords[0][i].y;
            }
            if (mesh->HasVertexColors(0))//just checking for one attribute for now
            {
                vertices[smartIterator++] = mesh->mColors[0][i].r;
                vertices[smartIterator++] = mesh->mColors[0][i].g;
                vertices[smartIterator++] = mesh->mColors[0][i].b;
                vertices[smartIterator++] = mesh->mColors[0][i].a;
            }
            if (mesh->HasBones())//TODO: handle animations and bones
            {
                //todo bones
            }
        }

        //indices
        indices.resize(mesh->mNumFaces * 3);
        smartIterator = 0;
        for (size_t i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace* face = &mesh->mFaces[i];

            for (unsigned int j = 0; j < face->mNumIndices; j++)
                indices[smartIterator++] = face->mIndices[j];
        }

        gx::Mesh submesh = gx::meshCreate(vertexAttributes);
        gx::meshSetData(&submesh, &vertices[0], mesh->mNumVertices, &indices[0], indices.size());

        model.materialIds.push_back(mesh->mMaterialIndex);
        model.submeshes.push_back(submesh);
    }

    for (size_t i = 0; i < scene->mNumMaterials; i++)
    {
        gx::Material material;
        aiMaterial* aiMat = scene->mMaterials[i];
        aiString name;
        if (aiMat->Get(AI_MATKEY_NAME, name) == AI_SUCCESS)
        {
            material.name = name.C_Str();
        }

        aiColor3D color(1.0f, 1.0f, 1.0f);
        if (aiMat->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS)
        {
            material.color = gx::Vec3(color.r, color.g, color.b);
        }

        float metallicValue = 1.0f;
        if (aiMat->Get(AI_MATKEY_METALLIC_FACTOR, metallicValue) == AI_SUCCESS)
        {
            material.metallic = metallicValue;
        }


        float roughnessValue = 1.0f;
        if (aiMat->Get(AI_MATKEY_SHININESS, roughnessValue) == AI_SUCCESS)
        {
            // WARNING: [0, 100] to [0, 1] if needed!
            material.roughness = roughnessValue / 100.0f;
        }

        //TODO WARNING: NO TEXTURE CACHING YET

        #define LoadAndReferenceTexture(type0, member0)\
        {\
        aiString aiPath("null");\
        if (aiMat->GetTexture(type0, 0, &aiPath) == AI_SUCCESS && aiPath != aiString("null"))\
        {\
            std::string slashedPath(aiPath.C_Str());\
            for (size_t i = 0; i < slashedPath.size(); i++)\
                if (slashedPath[i] == '\\')\
                    slashedPath[i] = '/';\
            std::string texturePath = path.substr(0, path.find_last_of("/")) + "/" + slashedPath;\
            gx::Texture t = gx::textureCreate();\
            gx::textureLoadImage(&t, texturePath);\
            gx::textureSetAttribute(&t, gx::textureAttributes::linear);\
            model.textures[oldNumTextures + currentTextureIndex] = t;\
            material.member0 = &model.textures[oldNumTextures + currentTextureIndex];\
            currentTextureIndex++;\
        }\
        }\
        
        unsigned int numTextures = 0;
        aiString dummy("null");

        if (aiMat->GetTexture(aiTextureType_DIFFUSE, 0, &dummy) == AI_SUCCESS) numTextures++;
        if (aiMat->GetTexture(aiTextureType_NORMALS, 0, &dummy) == AI_SUCCESS) numTextures++;
        if (aiMat->GetTexture(aiTextureType_METALNESS, 0, &dummy) == AI_SUCCESS) numTextures++;
        if (aiMat->GetTexture(aiTextureType_DIFFUSE_ROUGHNESS, 0, &dummy) == AI_SUCCESS) numTextures++;
        if (aiMat->GetTexture(aiTextureType_AMBIENT_OCCLUSION, 0, &dummy) == AI_SUCCESS) numTextures++;
        unsigned int oldNumTextures = model.textures.size();
        model.textures.resize(oldNumTextures + numTextures);
        
        unsigned int currentTextureIndex = 0;

        LoadAndReferenceTexture(aiTextureType_DIFFUSE, colorTexture);
        LoadAndReferenceTexture(aiTextureType_NORMALS, normalTexture);
        LoadAndReferenceTexture(aiTextureType_METALNESS, metallicTexture);
        LoadAndReferenceTexture(aiTextureType_DIFFUSE_ROUGHNESS, roughnessTexture);
        LoadAndReferenceTexture(aiTextureType_AMBIENT_OCCLUSION, ambientOcclusionTexture);
        
        
        model.materials.push_back(material);
    }

    return model;
}

void gx::modelDraw(Model* model)
{
    if (model->submeshes.size() == 0) return;
    for (int i = 0; i < model->submeshes.size(); i++)
    {
        gx::meshDraw(&model->submeshes[i]);
    }
}

void gx::modelDrawInstanced(Model* model, unsigned int count)
{
    if (model->submeshes.size() == 0) return;
    for (int i = 0; i < model->submeshes.size(); i++)
    {
        gx::meshDrawInstanced(&model->submeshes[i], count);
    }
}

void gx::modelDelete(Model* model)
{
    for (int i = 0; i < model->submeshes.size(); i++)
    {
        gx::meshDelete(&model->submeshes[i]);
    }
}



// -- SHADER

enum class ShaderType
{
    Vertex = GL_VERTEX_SHADER,
    Fragment = GL_FRAGMENT_SHADER,
    Geometry = GL_GEOMETRY_SHADER,
    TessellationControl = GL_TESS_CONTROL_SHADER,
    TessellationEvaluation = GL_TESS_EVALUATION_SHADER,
    Compute = GL_COMPUTE_SHADER
};

gx::Shader gx::shaderLoadFromFile(std::string path)
{
    std::ifstream shaderFile(path);
    std::string line;

    std::stringstream shaderSourceVertex;
    std::stringstream shaderSourceFragment;
    std::stringstream shaderSourceGeometry;
    std::stringstream shaderSourceTessellationControl;
    std::stringstream shaderSourceTessellationEvaluation;
    std::stringstream shaderSourceCompute;

    ShaderType currentShaderType = ShaderType::Vertex;

    if(shaderFile.fail())
    {
        debug::logError(std::string("Could not load shader: ") + path);
        return gx::Shader{ 0, "" };
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

    std::string vertexShaderSource = shaderSourceVertex.str();
    std::string fragmentShaderSource = shaderSourceFragment.str();
    std::string geometryShaderSource = shaderSourceGeometry.str();
    std::string tessControlShaderSource = shaderSourceTessellationControl.str();
    std::string tessEvalShaderSource = shaderSourceTessellationEvaluation.str();
    std::string computeShaderSource = shaderSourceCompute.str();
    
    gx::Shader shader = shaderLoadFromString(vertexShaderSource, fragmentShaderSource, geometryShaderSource, tessControlShaderSource, tessEvalShaderSource, computeShaderSource);
    shader.path = path;

    if (shader.id == 0)
        debug::logError("Could not create shader!");

    return shader;
}

gx::Shader gx::shaderLoad(gx::MemoryStorage* storage, std::string virtualPath)
{
    gx::mem::memoryShader memoryShader = gx::mem::getPackage<gx::mem::memoryShader>(storage, virtualPath);
    if (memoryShader.sources.size() != 6)
        return gx::Shader{};

    gx::Shader shader = gx::shaderLoadFromString(memoryShader.sources[0], memoryShader.sources[1], memoryShader.sources[2], memoryShader.sources[3], memoryShader.sources[4], memoryShader.sources[5]);
    return shader;
}

gx::Shader gx::shaderLoadFromString(std::string vertexSource, std::string fragmentSource, std::string geometrySource, std::string tessControlSource, std::string tessEvalSource, std::string computeSource)
{
    std::unordered_map<ShaderType, GLuint> shaderObjects;
    
    // Vertex Shader
    if (!vertexSource.empty())
    {
        GLuint vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
        const char* vertexShaderSourcePtr = vertexSource.c_str();
        glShaderSource(vertexShaderObject, 1, &vertexShaderSourcePtr, nullptr);
        glCompileShader(vertexShaderObject);

        GLint compileStatus;
        glGetShaderiv(vertexShaderObject, GL_COMPILE_STATUS, &compileStatus);

        if (compileStatus == GL_FALSE)
        {
            GLint infoLogLength;
            glGetShaderiv(vertexShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);

            std::vector<GLchar> infoLog(infoLogLength);
            glGetShaderInfoLog(vertexShaderObject, infoLogLength, nullptr, infoLog.data());

            gx::debug::logError("Vertex shader compilation failed: " + std::string(infoLog.data()));

            glDeleteShader(vertexShaderObject);

            return gx::Shader{ 0, ""};
        }
        shaderObjects[ShaderType::Vertex] = vertexShaderObject;
    }
    // Fragment Shader
    if (!fragmentSource.empty())
    {
        GLuint fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
        const char* fragmentShaderSourcePtr = fragmentSource.c_str();
        glShaderSource(fragmentShaderObject, 1, &fragmentShaderSourcePtr, nullptr);
        glCompileShader(fragmentShaderObject);

        GLint compileStatus;
        glGetShaderiv(fragmentShaderObject, GL_COMPILE_STATUS, &compileStatus);

        if (compileStatus == GL_FALSE)
        {
            GLint infoLogLength;
            glGetShaderiv(fragmentShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);

            std::vector<GLchar> infoLog(infoLogLength);
            glGetShaderInfoLog(fragmentShaderObject, infoLogLength, nullptr, infoLog.data());

            gx::debug::logError("Fragment shader compilation failed: " + std::string(infoLog.data()));

            glDeleteShader(fragmentShaderObject);

            return gx::Shader{ 0, "" };
        }

        shaderObjects[ShaderType::Fragment] = fragmentShaderObject;
    }

    // Geometry Shader
    if (!geometrySource.empty())
    {
        GLuint geometryShaderObject = glCreateShader(GL_GEOMETRY_SHADER);
        const char* geometryShaderSourcePtr = geometrySource.c_str();
        glShaderSource(geometryShaderObject, 1, &geometryShaderSourcePtr, nullptr);
        glCompileShader(geometryShaderObject);

        GLint compileStatus;
        glGetShaderiv(geometryShaderObject, GL_COMPILE_STATUS, &compileStatus);

        if (compileStatus == GL_FALSE)
        {
            GLint infoLogLength;
            glGetShaderiv(geometryShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);

            std::vector<GLchar> infoLog(infoLogLength);
            glGetShaderInfoLog(geometryShaderObject, infoLogLength, nullptr, infoLog.data());

            gx::debug::logError("Geometry shader compilation failed: " + std::string(infoLog.data()));

            glDeleteShader(geometryShaderObject);

            return gx::Shader{ 0, "" };
        }

        shaderObjects[ShaderType::Geometry] = geometryShaderObject;
    }

    // Tessellation Control Shader
    if (!tessControlSource.empty())
    {
        GLuint tessControlShaderObject = glCreateShader(GL_TESS_CONTROL_SHADER);
        const char* tessControlShaderSourcePtr = tessControlSource.c_str();
        glShaderSource(tessControlShaderObject, 1, &tessControlShaderSourcePtr, nullptr);
        glCompileShader(tessControlShaderObject);

        GLint compileStatus;
        glGetShaderiv(tessControlShaderObject, GL_COMPILE_STATUS, &compileStatus);

        if (compileStatus == GL_FALSE)
        {
            GLint infoLogLength;
            glGetShaderiv(tessControlShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);

            std::vector<GLchar> infoLog(infoLogLength);
            glGetShaderInfoLog(tessControlShaderObject, infoLogLength, nullptr, infoLog.data());

            gx::debug::logError("Tessellation Control shader compilation failed: " + std::string(infoLog.data()));

            glDeleteShader(tessControlShaderObject);

            return gx::Shader{ 0, "" };
        }

        shaderObjects[ShaderType::TessellationControl] = tessControlShaderObject;
    }

    // Tessellation Evaluation Shader
    if (!tessEvalSource.empty())
    {
        GLuint tessEvalShaderObject = glCreateShader(GL_TESS_EVALUATION_SHADER);
        const char* tessEvalShaderSourcePtr = tessEvalSource.c_str();
        glShaderSource(tessEvalShaderObject, 1, &tessEvalShaderSourcePtr, nullptr);
        glCompileShader(tessEvalShaderObject);

        GLint compileStatus;
        glGetShaderiv(tessEvalShaderObject, GL_COMPILE_STATUS, &compileStatus);

        if (compileStatus == GL_FALSE)
        {
            GLint infoLogLength;
            glGetShaderiv(tessEvalShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);

            std::vector<GLchar> infoLog(infoLogLength);
            glGetShaderInfoLog(tessEvalShaderObject, infoLogLength, nullptr, infoLog.data());

            gx::debug::logError("Tessellation Evaluation shader compilation failed: " + std::string(infoLog.data()));

            glDeleteShader(tessEvalShaderObject);

            return gx::Shader{ 0, "" };
        }

        shaderObjects[ShaderType::TessellationEvaluation] = tessEvalShaderObject;
    }

    // Compute Shader
    if (!computeSource.empty())
    {
        GLuint computeShaderObject = glCreateShader(GL_COMPUTE_SHADER);
        const char* computeShaderSourcePtr = computeSource.c_str();
        glShaderSource(computeShaderObject, 1, &computeShaderSourcePtr, nullptr);
        glCompileShader(computeShaderObject);

        GLint compileStatus;
        glGetShaderiv(computeShaderObject, GL_COMPILE_STATUS, &compileStatus);

        if (compileStatus == GL_FALSE)
        {
            GLint infoLogLength;
            glGetShaderiv(computeShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);

            std::vector<GLchar> infoLog(infoLogLength);
            glGetShaderInfoLog(computeShaderObject, infoLogLength, nullptr, infoLog.data());


            gx::debug::logError("Compute shader compilation failed: " + std::string(infoLog.data()));

            glDeleteShader(computeShaderObject);

            return gx::Shader{ 0, "" };
        }

        shaderObjects[ShaderType::Compute] = computeShaderObject;
    }
    
    GLuint programObject = glCreateProgram();

    for (const auto& shader : shaderObjects)
    {
        glAttachShader(programObject, shader.second);
    }

    glLinkProgram(programObject);
    for (const auto& shader : shaderObjects)
    {
        glDetachShader(programObject, shader.second);
        glDeleteShader(shader.second);
    }

    GLint linkStatus;
    glGetProgramiv(programObject, GL_LINK_STATUS, &linkStatus);

    if (linkStatus == GL_FALSE)
    {
        GLint infoLogLength;
        glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &infoLogLength);

        std::vector<GLchar> infoLog(infoLogLength);
        glGetProgramInfoLog(programObject, infoLogLength, nullptr, infoLog.data());


        gx::debug::logError("Program linking failed: " + std::string(infoLog.data()));

        glDeleteProgram(programObject);

        for (const auto& shader : shaderObjects)
        {
            glDeleteShader(shader.second);
        }

        return gx::Shader{ 0, "" };
    }

    for (const auto& shader : shaderObjects)
    {
        glDeleteShader(shader.second);
    }

    return gx::Shader{ programObject, "" };
}

void gx::shaderBind(Shader* shader)
{
	glUseProgram(shader->id);
}

void gx::shaderDelete(Shader* shader)
{
    glDeleteProgram(shader->id);
}

//internal function
int getShaderUniformLocationCached(gx::Shader* shader, const char* uniformName)
{
    std::size_t hash = 0;
    const char* str = uniformName;
    while (*str) {
        hash = (hash * 131) + *str;
        ++str;
    }

    std::string key = std::to_string(shader->id) + "_" + std::to_string(hash);
    auto it = ctx.cachedShaderUniformLocations.find(key);
    if (it != ctx.cachedShaderUniformLocations.end()) {
        return it->second;
    }

    GLint location = glGetUniformLocation(shader->id, uniformName);
    ctx.cachedShaderUniformLocations[key] = location;
    return location;
}

void gx::shaderSetUniform(Shader* shader, const char* uniformName, float float0)
{
    int location = getShaderUniformLocationCached(shader, uniformName);
    glUniform1f(location, float0);
}
void gx::shaderSetUniform(Shader* shader, const char* uniformName, int int0)
{
    int location = getShaderUniformLocationCached(shader, uniformName);
    glUniform1i(location, int0);
}
void gx::shaderSetUniform(Shader* shader, const char* uniformName, unsigned int uInt0)
{
    int location = getShaderUniformLocationCached(shader, uniformName);
    glUniform1ui(location, uInt0);
}
void gx::shaderSetUniform(Shader* shader, const char* uniformName, std::vector<float> vector0)
{
    int location = getShaderUniformLocationCached(shader, uniformName);
    glUniform1fv(location, vector0.size(), vector0.data());
}
void gx::shaderSetUniform(Shader* shader, const char* uniformName, std::vector<int> vector0)
{
    int location = getShaderUniformLocationCached(shader, uniformName);
    glUniform1iv(location, vector0.size(), vector0.data());
}
void gx::shaderSetUniform(Shader* shader, const char* uniformName, gx::Matrix mat4)
{
    int location = getShaderUniformLocationCached(shader, uniformName);
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat4));
}
void gx::shaderSetUniform(Shader* shader, const char* uniformName, gx::Vec2 vec2)
{
    int location = getShaderUniformLocationCached(shader, uniformName);
    glUniform2f(location, vec2.x, vec2.y);
}
void gx::shaderSetUniform(Shader* shader, const char* uniformName, gx::Vec3 vec3)
{
    int location = getShaderUniformLocationCached(shader, uniformName);
    glUniform3f(location, vec3.x, vec3.y, vec3.z);
}
void gx::shaderSetUniform(Shader* shader, const char* uniformName, gx::Vec4 vec4)
{
    int location = getShaderUniformLocationCached(shader, uniformName);
    glUniform4f(location, vec4.x, vec4.y, vec4.z, vec4.w);
}



// -- TEXTURE

gx::Texture gx::textureCreate(TextureDescription* description)
{
    return gx::textureCreate(description->target, description->wrapping, description->filtering, description->format, description->dataType, description->width, description->height, description->depth);
}

gx::Texture gx::textureCreate(textureAttributes::target target, textureAttributes::wrapping wrapping, textureAttributes::filtering filtering, textureAttributes::format formatInternal, textureAttributes::dataType dataType, int width, int height, int depth)
{
    Texture t;
    t.id = 0;
    t.target = target;
    t.format = formatInternal;
    t.dataType = dataType;
    t.width = width;
    t.height = height;
    t.depth = depth;
    t.levels = 1;

    //generate and bind
    glGenTextures(1, &t.id);
    glBindTexture(target, t.id);

    //wrapping
    glTexParameteri(target, GL_TEXTURE_WRAP_S, wrapping);
    glTexParameteri(target, GL_TEXTURE_WRAP_T, wrapping);

    if (wrapping == textureAttributes::clampToBorder)
    {
        float borderColor[] = { 1.0f, 1.0f, 0.0f, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    }

    //filtering
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, filtering);
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, filtering);


    //allocate memory and set size
    glTexImage2D(t.target, 0, t.format, t.width, t.height, 0, gx::textureAttributes::format::rgba, t.dataType, nullptr);
    return t;
}

gx::Texture gx::textureLoadFromFile(std::string path, gx::textureAttributes::format imageFormat)
{
    gx::Texture texture = gx::textureCreate();
    gx::textureLoadImage(&texture, path, imageFormat);
    return texture;
}

gx::Texture gx::textureLoad(gx::MemoryStorage* storage, std::string virtualPath)
{
    gx::mem::memoryTexture memoryTexture = gx::mem::getPackage<gx::mem::memoryTexture>(storage, virtualPath);
    gx::Texture texture = gx::textureCreate();

    gx::textureLoadImage(&texture, memoryTexture.data.data(), memoryTexture.width, memoryTexture.height, memoryTexture.channels, (gx::textureAttributes::format)memoryTexture.format);
    memoryTexture.data.clear();
    return texture;
}

void gx::textureLoadImage(Texture* texture, std::string path, gx::textureAttributes::format imageFormat)
{
    gx::textureBind(texture);

    int _width, _height, _nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path.c_str(), &_width, &_height, &_nrChannels, 0);
    if (data)
    {
        gx::textureResize(texture, _width, _height);
        texture->depth = 0;

        unsigned int _imageFormat = gx::textureAttributes::format::rgba;
        if (imageFormat == gx::textureAttributes::automatically)
        {
            if (_nrChannels == 1) _imageFormat = gx::textureAttributes::format::red;
            if (_nrChannels == 3) _imageFormat = gx::textureAttributes::format::rgb;
            if (_nrChannels == 4) _imageFormat = gx::textureAttributes::format::rgba;
        }
        else _imageFormat = imageFormat;


        glTexImage2D(texture->target, 0, texture->format, texture->width, texture->height, 0, _imageFormat, texture->dataType, data);
    }
    else
    {
        gx::debug::logError(std::string("Could not load image at " + path + "!"));
    }
    stbi_image_free(data);
}

void gx::textureLoadImage(Texture* texture, unsigned char* data, int width, int height, int channels, gx::textureAttributes::format imageFormat)
{
    if (data == nullptr)
        gx::debug::logError("Texture data was null");

    gx::textureBind(texture);

    stbi_set_flip_vertically_on_load(true);
    if (data)
    {
        gx::textureResize(texture, width, height);
        texture->depth = 0;

        unsigned int _imageFormat = gx::textureAttributes::format::rgba;
        if (imageFormat == gx::textureAttributes::automatically)
        {
            if (channels == 1) _imageFormat = gx::textureAttributes::format::red;
            if (channels == 3) _imageFormat = gx::textureAttributes::format::rgb;
            if (channels == 4) _imageFormat = gx::textureAttributes::format::rgba;
        }
        else _imageFormat = imageFormat;


        glTexImage2D(texture->target, 0, texture->format, texture->width, texture->height, 0, _imageFormat, texture->dataType, data);
    }
    else
    {
        gx::debug::logError(std::string("Could not load image from memory"));
    }
}

void gx::textureSetAttribute(Texture* texture, textureAttributes::wrapping newWrapping)
{
    gx::textureBind(texture);
    glTexParameteri(texture->target, GL_TEXTURE_WRAP_S, newWrapping);
    glTexParameteri(texture->target, GL_TEXTURE_WRAP_T, newWrapping);
    gx::textureUnbind();
}

void gx::textureSetAttribute(Texture* texture, textureAttributes::filtering newFiltering)
{
    gx::textureBind(texture);
    glTexParameteri(texture->target, GL_TEXTURE_MIN_FILTER, newFiltering);
    glTexParameteri(texture->target, GL_TEXTURE_MAG_FILTER, newFiltering);
    gx::textureUnbind();
}

void gx::textureSetBorderColor(Texture* texture, gx::Vec4 color)
{
    gx::textureBind(texture);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, &color[0]);
    gx::textureUnbind();
}

void gx::textureResize(Texture* texture, int width, int height)
{
    gx::textureBind(texture);
    glTexImage2D(texture->target, 0, texture->format, width, height, 0, gx::textureAttributes::format::rgba, texture->dataType, nullptr);
    texture->width = width;
    texture->height = height;
}

void gx::textureBind(Texture* texture, unsigned int slot)
{
    if (texture == nullptr)
        return gx::debug::logError("could not bind texture, texture was null");
    if (texture->id == 0)
        return;
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(texture->target, texture->id);
    //glActiveTexture(GL_TEXTURE0); // IF NOTHING WORKS TRY THIS, NOT SURE IF NEEDED
}

void gx::textureUnbind()
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void gx::textureDelete(Texture* texture)
{
    glDeleteTextures(1, &texture->id);
}



// -- CUBEMAP

gx::CubeMap gx::cubeMapCreate(gx::Texture textureXP, gx::Texture textureXN, gx::Texture textureYP, gx::Texture textureYN, gx::Texture textureZP, gx::Texture textureZN)
{
    return gx::CubeMap{ {textureXP, textureXN, textureYP, textureYN, textureZP, textureZN}, &ctx3d.cube };
}



// -- RENDERBUFFER

gx::Renderbuffer gx::renderbufferCreate(unsigned int width, unsigned int height, gx::textureAttributes::format format)
{
    Renderbuffer renderbuffer;
    glGenRenderbuffers(1, &renderbuffer.id);

    renderbuffer.format = format;

    if (width != 0 && height != 0)
        gx::renderbufferSetStorage(&renderbuffer, width, height, format);


    return renderbuffer;
}

void gx::renderbufferSetStorage(Renderbuffer* renderbuffer, unsigned int width, unsigned int height, gx::textureAttributes::format format)
{
    renderbuffer->format = format;

    glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer->id);
    glRenderbufferStorage(GL_RENDERBUFFER, format, width, height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void gx::renderbufferDelete(Renderbuffer* renderbuffer)
{
    glDeleteRenderbuffers(1, &renderbuffer->id);
}



// -- FRAMEBUFFER

gx::Framebuffer gx::framebufferCreate()
{
    Framebuffer framebuffer;
    glGenFramebuffers(1, &framebuffer.id);

    return framebuffer;
}

void gx::framebufferBind(Framebuffer* framebuffer)
{
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->id);
}

void gx::framebufferResize(Framebuffer* framebuffer, int width, int height)
{
    gx::framebufferBind(framebuffer);
    for (size_t i = 0; i < framebuffer->textureAttachements.size(); i++)
    {
        gx::textureResize(&framebuffer->textureAttachements[i], width, height);
    }
    gx::textureUnbind();

    for (size_t i = 0; i < framebuffer->renderbufferAttachements.size(); i++)
    {
        gx::renderbufferSetStorage(&framebuffer->renderbufferAttachements[i], width, height, framebuffer->renderbufferAttachements[i].format);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, framebuffer->renderbufferAttachementTypes[i], GL_RENDERBUFFER, framebuffer->renderbufferAttachements[i].id);
    }
    gx::framebufferUnbindSmart();
}

void gx::framebufferUnbindSmart()
{
    if (ctx.currentRenderTargets.size() > 1)//if there are other in priority
    {
        gx::framebufferBind(&ctx.currentRenderTargets.back()->framebuffer);
        glViewport(0, 0, ctx.currentRenderTargets.back()->texturePtr->width, ctx.currentRenderTargets.back()->texturePtr->height);
    }
    else//bind to default
    {
        gx::framebufferUnbindAll();
        glViewport(0, 0, gx::window::getSize().x, gx::window::getSize().y);
    }
}

void gx::framebufferUnbindAll()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void gx::framebufferAttachTexture(Framebuffer* framebuffer, FramebufferAttachment::type attachment, TextureDescription description)
{
    framebuffer->textureAttachements.push_back(gx::textureCreate(&description));
    glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, framebuffer->textureAttachements.back().id, 0);//hardcoded level = 0 !?
}

void gx::framebufferAttachRenderbuffer(Framebuffer* framebuffer, FramebufferAttachment::type attachment, gx::textureAttributes::format format, unsigned int width, unsigned int height)
{
    framebuffer->renderbufferAttachements.push_back(gx::renderbufferCreate(width, height, format));
    framebuffer->renderbufferAttachementTypes.push_back(attachment);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, framebuffer->renderbufferAttachements.back().id);
}

void gx::framebufferSetOutputBuffers(Framebuffer* framebuffer, std::vector<FramebufferAttachment::type> attachments)
{
    glDrawBuffers(attachments.size(), reinterpret_cast<const GLenum*>(attachments.data()));
}

bool gx::framebufferComplete(Framebuffer* framebuffer)
{
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
        return true;
    return false;
}

void gx::framebufferDelete(Framebuffer* framebuffer)
{
    for (size_t i = 0; i < framebuffer->textureAttachements.size(); i++)
    {
        gx::textureDelete(&framebuffer->textureAttachements[i]);
    }

    for (size_t i = 0; i < framebuffer->renderbufferAttachements.size(); i++)
    {
        gx::renderbufferDelete(&framebuffer->renderbufferAttachements[i]);
    }

    glDeleteFramebuffers(1, &framebuffer->id);
}



// -- RENDERTARGET

gx::RenderTarget gx::renderTargetCreate(int width, int height)
{
    RenderTarget target;

    target.framebuffer = gx::framebufferCreate();

    gx::framebufferBind(&target.framebuffer);

    gx::TextureDescription desc = { textureAttributes::repeat, textureAttributes::nearest, textureAttributes::texture2d, textureAttributes::rgba8, textureAttributes::uByte, width, height, 0 };
    gx::framebufferAttachTexture(&target.framebuffer, gx::FramebufferAttachment::color0, desc);
    target.texturePtr = &target.framebuffer.textureAttachements.back();
    gx::framebufferAttachRenderbuffer(&target.framebuffer, gx::FramebufferAttachment::depthStencil, gx::textureAttributes::depth24_stencil8, width, height);//even needed?

    gx::framebufferUnbindSmart();

    return target;
}

void gx::renderTargetResize(RenderTarget* target, int width, int height)
{
    
    gx::framebufferResize(&target->framebuffer, width, height);
}

void gx::renderTargetBind(RenderTarget* target)
{
    ctx3d.flushDeferredRenderer();
    ctx2d.flushAllBatchRenderers();
    gx::framebufferBind(&target->framebuffer);
    glViewport(0, 0, target->texturePtr->width, target->texturePtr->height);
    ctx.currentRenderTargets.push_back(target);

    gx::renderer2d::clear(true);
}

void gx::renderTargetUnbind()
{
    ctx3d.flushDeferredRenderer();
    ctx2d.flushAllBatchRenderers();

    ctx.currentRenderTargets.pop_back();

    gx::framebufferUnbindSmart();
}

void gx::renderTargetUnbindAll()
{
    ctx3d.flushDeferredRenderer();
    ctx2d.flushAllBatchRenderers();
    gx::framebufferUnbindAll();
    glViewport(0, 0, gx::window::getSize().x, gx::window::getSize().y);


    ctx.currentRenderTargets = {};
    ctx.currentRenderTargets.push_back(nullptr);
}

void gx::renderTargetDelete(RenderTarget* target)
{
    gx::framebufferDelete(&target->framebuffer);
}



// -- RENDERPROFILE

gx::RenderProfile gx::renderProfileCreate(std::vector<renderParameter::param> renderParameters)
{
    gx::RenderProfile profile;

    profile.parameters[gx::renderParameter::wireframe] = false;
    profile.parameters[gx::renderParameter::depthTest] = false;
    profile.parameters[gx::renderParameter::blend] = false;
    profile.parameters[gx::renderParameter::anisotropicFiltering] = false;
    profile.parameters[gx::renderParameter::antialiasing] = false;
    profile.parameters[gx::renderParameter::faceCulling] = false;
    profile.parameters[gx::renderParameter::hideFrontFaces] = false;
    profile.parameters[gx::renderParameter::frontFaceCW] = false;


    for (int i = 0; i < renderParameters.size(); i++)
    {
        switch (renderParameters[i])
        {
        case gx::renderParameter::wireframe:
            profile.parameters[gx::renderParameter::wireframe] = true;
            break;
        case gx::renderParameter::depthTest:
            profile.parameters[gx::renderParameter::depthTest] = true;
            break;
        case gx::renderParameter::blend:
            profile.parameters[gx::renderParameter::blend] = true;
            break;
        case gx::renderParameter::anisotropicFiltering:
            profile.parameters[gx::renderParameter::anisotropicFiltering] = true;
            break;
        case gx::renderParameter::antialiasing:
            profile.parameters[gx::renderParameter::antialiasing] = true;
            break;
        case gx::renderParameter::faceCulling:
            profile.parameters[gx::renderParameter::faceCulling] = true;
            break;
        case gx::renderParameter::hideFrontFaces:
            profile.parameters[gx::renderParameter::hideFrontFaces] = true;
            break;
        case gx::renderParameter::frontFaceCW:
            profile.parameters[gx::renderParameter::frontFaceCW] = true;
            break;

        default:
            break;
        }
    }

    return profile;
}

void gx::renderProfileUse(RenderProfile* newRenderProfile)
{
    std::unordered_map<gx::renderParameter::param, bool> currentParams = ctx.currentRenderProfile.get()->parameters;

    #define PARAMS_DIFFER(param) ctx.currentRenderProfile.get()->parameters[param] != newRenderProfile->parameters[param]
    #define PARAM_IS_ACTIVE(param) newRenderProfile->parameters[param] == true

    if (PARAMS_DIFFER(gx::renderParameter::wireframe))
    {
        if(PARAM_IS_ACTIVE(gx::renderParameter::wireframe))
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        else
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
    }
    if (PARAMS_DIFFER(gx::renderParameter::depthTest))
    {
        if (PARAM_IS_ACTIVE(gx::renderParameter::depthTest))
        {
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LESS);
        }
        else
        {
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_ALWAYS);
        }
    }
    if (PARAMS_DIFFER(gx::renderParameter::blend))
    {
        if (PARAM_IS_ACTIVE(gx::renderParameter::blend))
        {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }
        else
        {
            glDisable(GL_BLEND);
        }
    }
    if (PARAMS_DIFFER(gx::renderParameter::anisotropicFiltering))
    {
        if (PARAM_IS_ACTIVE(gx::renderParameter::anisotropicFiltering))
        {
            glEnable(GL_TEXTURE_GEN_S);
        }
        else
        {
            glDisable(GL_TEXTURE_GEN_S);
        }
    }
    if (PARAMS_DIFFER(gx::renderParameter::antialiasing))
    {
        if (PARAM_IS_ACTIVE(gx::renderParameter::antialiasing))
        {
            glEnable(GL_MULTISAMPLE);
            glfwWindowHint(GLFW_SAMPLES, 4);
        }
        else
        {
            glDisable(GL_MULTISAMPLE);
            glfwWindowHint(GLFW_SAMPLES, 1);
        }
    }
    if (PARAMS_DIFFER(gx::renderParameter::faceCulling))
    {
        if (PARAM_IS_ACTIVE(gx::renderParameter::faceCulling))
        {
            glEnable(GL_CULL_FACE);
        }
        else
        {
            glDisable(GL_CULL_FACE);
        }
    }
    if (PARAMS_DIFFER(gx::renderParameter::hideFrontFaces))
    {
        if (PARAM_IS_ACTIVE(gx::renderParameter::hideFrontFaces))
        {
            glCullFace(GL_FRONT);
        }
        else
        {
            glCullFace(GL_BACK);
        }
    }
    if (PARAMS_DIFFER(gx::renderParameter::frontFaceCW))
    {
        if (PARAM_IS_ACTIVE(gx::renderParameter::frontFaceCW))
        {
            glFrontFace(GL_CW);
        }
        else
        {
            glFrontFace(GL_CCW);
        }
    }

    ctx.currentRenderProfile = std::make_unique<gx::RenderProfile>(*newRenderProfile);
}



// -- SOUND

gx::Sound gx::soundLoad(gx::MemoryStorage* storage, std::string virtualPath)
{
    gx::mem::memorySound memorySound = gx::mem::getPackage<gx::mem::memorySound>(storage, virtualPath);

    return gx::soundLoad(memorySound.data.data(), memorySound.data.size(), memorySound.sampleRate, memorySound.format);
}

void gx::soundDelete(gx::Sound* sound)
{
    alDeleteSources(1, &sound->source);
    alDeleteBuffers(1, &sound->buffer);
}

gx::Sound gx::soundLoad(float* sampleData, size_t dataCount, unsigned int sampleRate, int format)
{
    Sound sound;

    if (sampleData == nullptr)
    {
        gx::debug::logError("Sound data was null");
        return gx::Sound();
    }

    alGenBuffers(1, &sound.buffer);

    alBufferData(sound.buffer, format, sampleData, dataCount, sampleRate);

    alGenSources(1, &sound.source);
    alSourcef(sound.source, AL_PITCH, 1.0f);
    alSourcef(sound.source, AL_GAIN, 1.0f);
    alSource3f(sound.source, AL_POSITION, 0, 0, 0);
    alSource3f(sound.source, AL_VELOCITY, 0, 0, 0);
    alSourcei(sound.source, AL_LOOPING, AL_FALSE);
    alSourcei(sound.source, AL_BUFFER, sound.buffer);

    return sound;
}

gx::Sound gx::soundLoadWAV(std::string path)
{
    unsigned int channels = 0;
    unsigned int sampleRate = 0;
    unsigned int bitsPerSample = 0;
    float* sampleData;
    size_t dataCount;


    std::string fileExtension = path.substr(path.find_last_of(".") + 1);
    if (fileExtension == "wav" || true)// ALWAYS TRUE EXTENSION JUST FOR FUTURE
    {
        drwav wav;
        if (!drwav_init_file(&wav, path.c_str(), NULL))
        {
            gx::debug::logError(std::string("Could not open ") + path);
            return gx::Sound();
        }

        channels = wav.channels;
        bitsPerSample = wav.bitsPerSample;
        sampleRate = wav.sampleRate;
        sampleData = (float*)malloc(wav.totalPCMFrameCount * wav.channels * sizeof(float));

        drwav_read_raw(&wav, wav.totalPCMFrameCount * wav.channels * sizeof(float), sampleData);

        dataCount = wav.totalPCMFrameCount * wav.channels * (bitsPerSample == 16 ? 2 : 1);

        drwav_uninit(&wav);
    }

    if (sampleData == nullptr)
    {
        gx::debug::logError(std::string("Could not load ") + path);
        return gx::Sound();
    }


    ALenum format;
    if (channels == 1 && bitsPerSample == 8)
        format = AL_FORMAT_MONO8;
    else if (channels == 1 && bitsPerSample == 16)
        format = AL_FORMAT_MONO16;
    else if (channels == 2 && bitsPerSample == 8)
        format = AL_FORMAT_STEREO8;
    else if (channels == 2 && bitsPerSample == 16)
        format = AL_FORMAT_STEREO16;
    else
    {
        gx::debug::logError(std::string("unrecognised wave format: ") + std::to_string(channels) + std::string("channels, ") + std::to_string(bitsPerSample) + std::string("bps"));
        return gx::Sound();
    }

    gx::Sound sound = gx::soundLoad(sampleData, dataCount, sampleRate, format);
    free(sampleData);

    return sound;
}

void gx::soundPlay(gx::Sound* sound)
{
    if (gx::soundIsPlaying(sound))
    {
        unsigned int newSource = 0;
        alGenSources(1, &newSource);
        alSourcef(newSource, AL_PITCH, 1.0f);
        alSourcef(newSource, AL_GAIN, 1.0f);
        alSource3f(newSource, AL_POSITION, 0, 0, 0);
        alSource3f(newSource, AL_VELOCITY, 0, 0, 0);
        alSourcei(newSource, AL_LOOPING, AL_FALSE);
        alSourcei(newSource, AL_BUFFER, sound->buffer);

        //todo: set attributes !!!

        //play
        std::thread playingThread([](unsigned int source)
            {
                alSourcePlay(source);

                ALint state = AL_PLAYING;

                while (state == AL_PLAYING)
                {
                    alGetSourcei(source, AL_SOURCE_STATE, &state);
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                }
                alDeleteSources(1, &source);
            }, newSource);

        playingThread.detach();

        return;
    }

    std::thread playingThread([](unsigned int source)
        {
            alSourcePlay(source);

            ALint state = AL_PLAYING;

            while (state == AL_PLAYING)
            {
                alGetSourcei(source, AL_SOURCE_STATE, &state);
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        }, sound->source);

    playingThread.detach();
}

void gx::soundStop(gx::Sound* sound)
{
    alSourceStop(sound->source);
}

void gx::soundSetAttributes(gx::Sound* sound, std::vector<std::pair<gx::SourceAttribute, gx::Vec3>> attributes)
{
    for (size_t i = 0; i < attributes.size(); i++)
    {
        if (attributes[i].first == gx::pitch)
            alSourcef(sound->source, AL_PITCH, attributes[i].second.x);
        else if (attributes[i].first == gx::gain)
            alSourcef(sound->source, AL_GAIN, attributes[i].second.x);
        else if (attributes[i].first == gx::position)
            alSource3f(sound->source, AL_POSITION, attributes[i].second.x, attributes[i].second.y, attributes[i].second.z);
        else if (attributes[i].first == gx::velocity)
            alSource3f(sound->source, AL_VELOCITY, attributes[i].second.x, attributes[i].second.y, attributes[i].second.z);
        else if (attributes[i].first == gx::direction)
        {
            std::vector<float> direction = { attributes[i].second.x, attributes[i].second.y, attributes[i].second.z };
            alSourcefv(sound->source, AL_DIRECTION, direction.data());
        }
        else if (attributes[i].first == gx::maxDistance)
            alSourcef(sound->source, AL_MAX_DISTANCE, attributes[i].second.x);
        else if (attributes[i].first == gx::distanceLoss)
            alSourcef(sound->source, AL_ROLLOFF_FACTOR, attributes[i].second.x);
        else if (attributes[i].first == gx::coneOuterGain)
            alSourcef(sound->source, AL_CONE_OUTER_GAIN, attributes[i].second.x);
        else if (attributes[i].first == gx::coneInnerAngle)
            alSourcef(sound->source, AL_CONE_INNER_ANGLE, attributes[i].second.x);
        else if (attributes[i].first == gx::coneOuterAngle)
            alSourcef(sound->source, AL_CONE_OUTER_ANGLE, attributes[i].second.x);
        else if (attributes[i].first == gx::doppler)
            alSourcef(sound->source, AL_DOPPLER_FACTOR, attributes[i].second.x);
        else if (attributes[i].first == gx::dopplerVelocity)
            alSourcef(sound->source, AL_DOPPLER_VELOCITY, attributes[i].second.x);
        else if (attributes[i].first == gx::secondsOffset)
            alSourcef(sound->source, AL_SEC_OFFSET, attributes[i].second.x);
        else if (attributes[i].first == gx::looping)
            alSourcei(sound->source, AL_LOOPING, (attributes[i].second.x == 1 ? AL_TRUE : AL_FALSE));
    }
}

bool gx::soundIsPlaying(gx::Sound* sound)
{
    ALint state = AL_PLAYING;
    alGetSourcei(sound->source, AL_SOURCE_STATE, &state);

    return (state == AL_PLAYING);
}