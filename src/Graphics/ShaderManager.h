// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_SHADERMANAGER_H_
#define GRAPHICS_SHADERMANAGER_H_

#include <vector>

#include "Common/Global.h"
#include "Common/Passkey.h"
#include "Graphics/OpenGL.h"
#include "Graphics/ShaderDetails.h"
#include "Math/Geometry.h"
#include "Math/Vec2.h"
#include "Memory/Array.h"

namespace rainbow
{
    struct ISolemnlySwearThatIAmOnlyTesting;
}

namespace rainbow::graphics
{
    struct Context;

    class ShaderManager : public Global<ShaderManager>
    {
    public:
        enum
        {
            kInvalidProgram,
            kDefaultProgram
        };

        /// <summary>
        ///   Saves the current shader context and restores it when exiting
        ///   scope.
        /// </summary>
        class Context
        {
        public:
            Context(ShaderManager& sm, unsigned int program)
                : shader_manager_(sm), program_(sm.current_)
            {
                sm.use(program);
            }

            ~Context() { shader_manager_.use(program_); }

        private:
            ShaderManager& shader_manager_;
            unsigned int program_;
        };

        ShaderManager(graphics::Context& ctx, const Passkey<graphics::Context>&)
            : context_(&ctx)
        {
        }

        ~ShaderManager();

        auto graphics_context() const -> graphics::Context&
        {
            return *context_;
        }

        bool init();

        /// <summary>Compiles program.</summary>
        /// <param name="shaders">Shader parameters.</param>
        /// <param name="attributes">Shader attributes.</param>
        /// <returns>
        ///   Unique program identifier; <c>kInvalidProgram</c> if unsuccessful.
        /// </returns>
        auto compile(ArraySpan<Shader::Params> shaders,
                     const Shader::AttributeParams* attributes) -> unsigned int;

        /// <summary>Returns current program details.</summary>
        auto get_program() const -> const Shader::Details&
        {
            R_ASSERT(current_ > 0, "ShaderManager is uninitialised");
            return programs_[current_ - 1];
        }

        /// <summary>Returns program details.</summary>
        auto get_program(unsigned int pid) -> Shader::Details&
        {
            R_ASSERT(pid > 0, "Invalid shader program id");
            return programs_[pid - 1];
        }

        /// <summary>Updates orthographic projection.</summary>
        void update_projection();

        /// <summary>Updates viewport.</summary>
        void update_viewport();

        /// <summary>Activates program.</summary>
        void use(unsigned int program);

        /// <summary>Activates program in current scope only.</summary>
        auto use_scoped(unsigned int program) -> Context
        {
            return Context(*this, program);
        }

        ShaderManager(const ISolemnlySwearThatIAmOnlyTesting&)
        {
            make_global();
        }

    private:
        unsigned int current_ = kInvalidProgram;  ///< Currently used program.
        graphics::Context* context_ = nullptr;
        std::vector<Shader::Details> programs_;   ///< Linked shader programs.
        std::vector<unsigned int> shaders_;       ///< Compiled shaders.
    };
}  // namespace rainbow::graphics

#endif
