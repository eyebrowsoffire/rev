#pragma once

#include "rev/Types.h"
#include "rev/Utilities.h"
#include "rev/gl/FrameBuffer.h"
#include "rev/gl/Texture.h"

#include <vector>

namespace rev
{

template <typename RenderPropertyArg, GLenum attachmentPoint,
          GLenum internalFormat, GLenum format, GLenum type>
struct RenderStageAttachment
{
    using RenderProperty = RenderPropertyArg;
    static constexpr GLenum kAttachmentPoint = attachmentPoint;
    static constexpr GLenum kInternalFormat = internalFormat;
    static constexpr GLenum kFormat = format;
    static constexpr GLenum kType = type;
};

template <typename... Attachments>
class RenderStage
{
  public:
    RenderStage()
    {
        ReadWriteFrameBufferContext context(_frameBuffer);
        std::vector<GLenum> drawBuffers;
        visitTuple(_attachments, [&context, &drawBuffers](auto &attachment) {
            using AttachmentType = std::decay_t<decltype(attachment)>;
            context.setTextureAttachment(AttachmentType::kAttachmentPoint,
                                         attachment.getTexture());

            // TODO: Make this a whitelist instead of a blacklist
            if constexpr (AttachmentType::kAttachmentPoint != GL_DEPTH_ATTACHMENT)
            {
                drawBuffers.push_back(AttachmentType::kAttachmentPoint);
            }
        });
        glDrawBuffers(drawBuffers.size(), drawBuffers.data());
        glGetError();
    }

    void setOutputSize(const RectSize<GLsizei> &size)
    {
        visitTuple(_attachments,
                   [&size](auto &attachment) { attachment.setSize(size); });
    }

    template <typename RenderProperty>
    const Texture &getOutputTexture() const
    {
        return findTextureForRenderProperty<RenderProperty, 0, Attachments...>();
    }

    ReadWriteFrameBufferContext getRenderContext()
    {
        return ReadWriteFrameBufferContext(_frameBuffer);
    }

  private:
    template <typename RenderProperty, size_t index, typename FirstAttachment,
              typename... RemainingAttachments>
    const Texture &findTextureForRenderProperty() const
    {
        if constexpr (std::is_same_v<RenderProperty,
                                     typename FirstAttachment::RenderProperty>)
        {
            return std::get<index>(_attachments).getTexture();
        }
        else
        {
            return findTextureForRenderProperty<RenderProperty, index + 1,
                                                RemainingAttachments...>();
        }
    }

    template <typename RenderProperty, size_t index = 0>
    const Texture &findTextureForRenderProperty() const
    {
        static_assert(sizeof(RenderProperty) < 0,
                      "Render property not found in render stage.");
    }

    template <typename RenderStageAttachment>
    class AttachmentTexture
    {
      public:
        static constexpr GLenum kAttachmentPoint =
            RenderStageAttachment::kAttachmentPoint;

        AttachmentTexture()
        {
            Texture2DContext context(_texture);
            context.setParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            context.setParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            context.setParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            context.setParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        }

        const Texture &getTexture() const { return _texture; }

        void setSize(const RectSize<GLsizei> &size)
        {
            Texture2DContext context(_texture);
            context.setImage(0, RenderStageAttachment::kInternalFormat, size.width,
                             size.height, 0, RenderStageAttachment::kFormat,
                             RenderStageAttachment::kType, nullptr);
        }

      private:
        Texture _texture;
    };

    FrameBuffer _frameBuffer;
    std::tuple<AttachmentTexture<Attachments>...> _attachments;
};

} // namespace rev