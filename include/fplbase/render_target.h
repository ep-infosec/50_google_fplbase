// Copyright 2014 Google Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef FPLBASE_RENDER_TARGET_H
#define FPLBASE_RENDER_TARGET_H

#include "fplbase/config.h"  // Must come first.

#include "mathfu/glsl_mappings.h"
#include "fplbase/material.h"
#include "fplbase/mesh.h"
#include "fplbase/renderer.h"
#include "fplbase/shader.h"

#ifdef __ANDROID__
#include "fplbase/renderer_android.h"
#endif

namespace fplbase {

/// Texture formats for the texture generated by the render target.
enum RenderTargetTextureFormat {
  kRenderTargetTextureFormatA8,     /// 8-bits alpha.
  kRenderTargetTextureFormatR8,     /// 8-bits red.
  kRenderTargetTextureFormatRGB8,   /// 8-bits red, green, blue.
  kRenderTargetTextureFormatRGBA8,  /// 8-bits red, green, blue, alpha.

  // Depth Formats:
  kRenderTargetTextureFormatDepth16,   /// 16-bits depth texture format.
  kRenderTargetTextureFormatDepth32F,  /// 32-bits depth texture format.

  kRenderTargetTextureFormatNone,  /// No texture buffer.
  kRenderTargetTextureFormatCount = kRenderTargetTextureFormatNone,
};

/// Depth stencil formats used by the render target's depth buffer.
enum DepthStencilFormat {
  kDepthStencilFormatDepth16,           /// 16-bits depth.
  kDepthStencilFormatDepth24,           /// 24-bits depth.
  kDepthStencilFormatDepth32F,          /// 32-bits floating point depth.
  kDepthStencilFormatDepth24Stencil8,   /// 24-bits, 8-bits stencil.
  kDepthStencilFormatDepth32FStencil8,  /// 32-bits floating point depth, 8-bits
                                        /// stencil.
  kDepthStencilFormatStencil8,          /// 8-bits stencil.
  kDepthStencilFormatNone,              /// No depth buffer.
  kDepthStencilFormatCount = kDepthStencilFormatNone,
};

/// @file
/// @addtogroup fplbase_render_target
/// @{

/// @class RenderTarget
/// @brief Abstracts a surface that can be rendered to.
///
/// Calling SetAsRenderTarget() will cause all subsequent draw calls to be
/// drawn onto the RenderTarget instead of to the screen buffer.
class RenderTarget {
 public:
  RenderTarget() : initialized_(false) {}
  /// @brief Initialize a render target of the provided dimensions.
  ///
  /// Defaults the format to GL_UNSIGNED_BYTE, using a depth buffer.
  ///
  /// @param dimensions The dimensions of the render target.
  void Initialize(const mathfu::vec2i& dimensions);
  /// @brief Initialize a render target of the provided dimensions.
  ///
  /// @param dimensions The dimensions of the render target.
  /// @param texture_format The format of the generated texture.
  /// @param depth_stencil_format The depth stencl format for the render target.
  void Initialize(const mathfu::vec2i& dimensions,
                  RenderTargetTextureFormat texture_format,
                  DepthStencilFormat depth_stencil_format);

  /// @brief Deletes the associated opengl resources associated with the
  ///        RenderTarget.
  void Delete();

  /// @brief Sets the RenderTarget as the active render target.
  ///
  /// All subsequent openGL draw calls will render to this RenderTarget
  /// instead of wherever they were going before.
  // TODO(shanee): deprecate, remove and implement Renderer::SetRenderTarget.
  void SetAsRenderTarget() const;

  /// @brief Binds the texture associated with this rendertarget as the active
  ///        texture.
  ///
  /// Primarily useful when rendering the RenderTarget's texture as part of a
  /// mesh.  Throws an assert if the RenderTarget does not have a texture.
  ///
  /// @param texture_number The index of the texture to make active.
  void BindAsTexture(int texture_number) const;

  /// @brief Checks if this rendertarget refer to an off-screen texture.
  ///
  /// This is important because rendertargets that aren't texture-based will
  /// assert if you try to bind them as texture or access their textureId.
  ///
  /// @return Returns true if this rendertarget refers to an off-screen texture,
  ///         and false if it refers to the screen itself.
  inline bool IsTexture() const { return ValidBufferHandle(framebuffer_id_); }

  /// @brief Gets the TextureId associated with the RenderTarget, assuming that
  ///        it is texture-based.
  ///
  /// Throws an assert if you try to call GetTextureId on a RenderTarget that
  /// doesn't have a texture backing it, such as the screen's display buffer.
  ///
  /// @return Returns the TextureId associated with the RenderTarget.
  inline TextureHandle GetTextureId() const {
    assert(IsTexture());
    return rendered_texture_id_;
  }

  /// @brief Checks if the RenderTarget has been initialized.
  ///
  /// Returns true if this RenderTarget has been initialized and is ready to
  /// use. Returns false if has not yet been initialized, failed initalization,
  /// or has been deleted.  Trying to use an uninitialized RenderTarget will
  /// generally cause errors or asserts.
  ///
  /// @brief Returns true if it has been initialized, false otherwise.
  bool initialized() const { return initialized_; }

  /// @brief Gets the RenderTarget that corresponds to the screen.
  ///
  /// @param renderer The renderer object to use.
  /// @return Returns the RenderTarget that corresponds to the screen.
  static RenderTarget ScreenRenderTarget(Renderer& renderer);

 private:
  mathfu::vec2i dimensions_;
  BufferHandle framebuffer_id_;
  TextureHandle rendered_texture_id_;
  BufferHandle depth_buffer_id_;
  bool initialized_;
};

/// @}
}  // namespace fplbase

#endif  // FPLBASE_RENDER_TARGET_H