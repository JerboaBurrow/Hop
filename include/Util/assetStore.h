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
    /**
     * @brief A data store for a general asset T.
     *
     * @tparam T type of the asset.
     * @remark Assets are loading from relative paths to a supplied root, seet AssetStore::AssetStore.
     */
    template <class T>
    class AssetStore
    {
    public:

        /**
         * @brief Construct a new AssetStore.
         *
         * @param root the relative path for all assets.
         */
        AssetStore(std::filesystem::path root)
        : root(root)
        {}

        /**
         * @brief Caches all matching asset paths relative to root.
         * @remark Specialised AssetStores overloads a method to check if a path is an asset of type T.
         * This is done by file extensions (.png, .mp3, ...).
         */
        void scan()
        {
            if (std::filesystem::is_directory(root))
            {
                scanDirectory(root);
            }
        };

        /**
         * @brief Loads the asset at this relative path.
         *
         * @param assetPath relative path to an asset.
         */
        virtual void load(std::filesystem::path assetPath) = 0;

        /**
         * @brief Loads all assets in the asset store.
         * @remark Use with scan.
         */
        void loadAll()
        {
            for (const auto & p : assets)
            {
                load(p);
            }
        }

        /**
         * @brief Return the asset.
         *
         * @param relative_path path to asset.
         * @return std::shared_ptr<T> the asset.
         * @remark Will attempt to load an unloaded asset.
         */
        std::shared_ptr<T> get(std::filesystem::path relative_path)
        {
            if (assets[relative_path] == nullptr)
            {
                load(relative_path);
            }
            return assets[relative_path];
        }

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
                    if (matchesAssetType(entry) && assets.find(entry) == assets.end()) { assets[entry] = nullptr; }
                }
                else
                {
                    scanDirectory(entry);
                }
            }
        }
    };

    /**
     * @brief AssetStore specialisation to store texture assets.
     *
     */
    class TextureAssetStore : public AssetStore<jGL::Texture>
    {
    public:

        /**
         * @brief Construct a new TextureAssetStore
         *
         * @param root relative path for textures.
         * @param instance jGLInstance for creating textures.
         * @remark Currently supports .png, .jpg files.
         */
        TextureAssetStore(std::filesystem::path root, std::shared_ptr<jGL::jGLInstance> & instance)
        : AssetStore<jGL::Texture>(root), instance(instance)
        {}

        /**
         * @brief Load a texture.
         *
         * @param assetPath path relative to root.
         */
        void load(std::filesystem::path assetPath)
        {
            if (matchesAssetType(assetPath))
            {
                std::filesystem::path relative = std::filesystem::relative(assetPath, root);
                assets[relative] = instance->createTexture
                (
                    assetPath.generic_string(),
                    jGL::Texture::Type::RGBA
                );
            }
        };

        /**
         * @brief Supported texture extension.
         * @remark These will be loaded as RGBA textures.
         */
        const std::vector<std::string> extensions = {".png, .jpg"};

    protected:
        std::shared_ptr<jGL::jGLInstance> & instance;

        bool matchesAssetType(std::filesystem::path file)
        {
            return std::find(extensions.begin(), extensions.end(), file.extension().generic_string()) != extensions.end();
        }
    };
}
#endif /* ASSETSTORE_H */
