#ifndef ASSETSTORE_H
#define ASSETSTORE_H

#include <jGL/jGL.h>
#include <jGL/texture.h>

#include <map>
#include <filesystem>
#include <memory>
#include <iterator>

namespace Hop::Util::Assets
{
    template <class T>
    class AssetStore 
    {
    public:

        AssetStore(std::filesystem::path root)
        : root(root) 
        {}

        void scan() 
        {
            if (std::filesystem::is_directory(root))
            {
                scanDirectory(root);
            }
        };

        virtual void load(std::filesystem::path assetPath) = 0;

        std::shared_ptr<T> get(std::filesystem::path relative_path) { return assets[relative_path]; }

        typename std::map<std::filesystem::path, std::shared_ptr<T>>::const_iterator begin() { return assets.cbegin(); }
        typename std::map<std::filesystem::path, std::shared_ptr<T>>::const_iterator end() { return assets.cend(); }

    protected:
        std::filesystem::path root;
        std::map<std::filesystem::path, std::shared_ptr<T>> assets;

        virtual bool matchesAssetType(std::filesystem::path file) = 0;

        void scanDirectory(std::filesystem::path dir)
        {
            for (auto const & entry : std::filesystem::directory_iterator{dir})
            {
                if (entry.is_regular_file())
                {
                    if (matchesAssetType(entry)) { load(entry); }
                }
                else
                {
                    scanDirectory(entry);
                }
            }
        }
    };

    class TextureAssetStore : public AssetStore<jGL::Texture>
    {
    public:

        TextureAssetStore(std::filesystem::path root, std::unique_ptr<jGL::jGLInstance> & instance)
        : AssetStore<jGL::Texture>(root), instance(instance)
        {}

        void load(std::filesystem::path assetPath) 
        {
            std::filesystem::path relative = std::filesystem::relative(assetPath, root);
            assets[relative] = instance->createTexture
            (
                assetPath,
                jGL::Texture::Type::RGBA
            );
        };

    protected:
        std::unique_ptr<jGL::jGLInstance> & instance;

        bool matchesAssetType(std::filesystem::path file) 
        { 
            static std::vector<std::string> extensions = {".png"};
            return std::find(extensions.begin(), extensions.end(), file.extension()) != extensions.end();
        }
    };
}
#endif /* ASSETSTORE_H */
