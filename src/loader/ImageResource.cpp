#include "StarFishConfig.h"
#include "ImageResource.h"

#include "platform/message_loop/MessageLoop.h"
#include "platform/file_io/FileIO.h"
#include "dom/Document.h"
#include "loader/ResourceLoader.h"

namespace StarFish {

void ImageResource::doLoad(void* data)
{
    Resource* res = (Resource*)data;
    // special path for image resource
    if (res->url().isFileURL()) {
        FileIO* fio = FileIO::create();
        // NOTE
        // we should special logic to load file url for image
        // we can pass src of image to platform layer
        // TODO handle decoding failed
        String* path = res->url().urlStringWithoutSearchPart();
        String* filePath = path->substring(7, path->length() - 7);
        bool canLoad = fio->open(filePath);
        delete fio;
        if (!canLoad) {
            res->didLoadFailed();
            return;
        }
        std::string utf8 = filePath->utf8Data();
        auto iter = res->loader()->m_offlineImageCache.find(utf8);
        ImageData* id;
        if (iter == res->loader()->m_offlineImageCache.end()) {
            id = ImageData::create(filePath);
            res->loader()->m_offlineImageCache.insert(std::make_pair(utf8, id));
        } else {
            id = iter->second;
        }
        res->asImageResource()->m_imageData = id;
        res->didLoadFinished();
    } else {
        res->didLoadFailed();
    }
}

void ImageResource::request(bool needsSyncRequest)
{
    loader()->fetchResourcePreprocess(this);

    if (needsSyncRequest) {
        doLoad(this);
    } else {
        pushIdlerHandle(m_loader->m_document->window()->starFish()->messageLoop()->addIdler([](size_t handle, void* data) {
            Resource* res = (Resource*)data;
            res->removeIdlerHandle(handle);
            doLoad(data);
        }, this));
    }
}

}
