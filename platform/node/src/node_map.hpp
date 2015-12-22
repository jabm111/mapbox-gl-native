#pragma once

#include <mbgl/map/map.hpp>
#include <mbgl/storage/file_source.hpp>
#include <mbgl/platform/default/headless_view.hpp>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wshadow"
#include <nan.h>
#pragma GCC diagnostic pop

#include <queue>

namespace node_mbgl {

class NodeMap : public Nan::ObjectWrap,
                public mbgl::FileSource {
public:
    struct RenderOptions;
    class RenderWorker;

    static NAN_MODULE_INIT(Init);

    static NAN_METHOD(New);
    static NAN_METHOD(Load);
    static NAN_METHOD(Render);
    static NAN_METHOD(Release);
    static NAN_METHOD(DumpDebugLogs);

    void startRender(RenderOptions options);
    void renderFinished();

    void release();

    inline bool isLoaded() { return loaded; }
    inline bool isValid() { return valid; }

    static RenderOptions ParseOptions(v8::Local<v8::Object>);
    static Nan::Persistent<v8::Function> constructor;

    NodeMap(v8::Local<v8::Object>);
    ~NodeMap();

    std::unique_ptr<mbgl::FileRequest> requestStyle(const std::string& url, Callback) override;
    std::unique_ptr<mbgl::FileRequest> requestSource(const std::string& url, Callback) override;
    std::unique_ptr<mbgl::FileRequest> requestTile(const mbgl::SourceInfo&, const mbgl::TileID&, float pixelRatio, Callback) override;
    std::unique_ptr<mbgl::FileRequest> requestGlyphs(const std::string& urlTemplate, const std::string& fontStack, const mbgl::GlyphRange&, Callback) override;
    std::unique_ptr<mbgl::FileRequest> requestSpriteJSON(const std::string& urlBase, float pixelRatio, Callback) override;
    std::unique_ptr<mbgl::FileRequest> requestSpriteImage(const std::string& urlBase, float pixelRatio, Callback) override;

    template <typename... Handles>
    void request(const char * method, Callback callback, Handles... handles);

    mbgl::HeadlessView view;
    std::unique_ptr<mbgl::Map> map;

    std::exception_ptr error;
    mbgl::PremultipliedImage image;
    std::unique_ptr<Nan::Callback> renderCallback;

    // Async for delivering the notifications of render completion.
    uv_async_t *async;

    bool loaded = false;
    bool valid = true;
};

}
