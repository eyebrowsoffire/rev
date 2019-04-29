#pragma once

#include "rev/gl/ProgramResource.h"

#include <memory>
#include <unordered_map>

namespace rev {

// Programs conform to the following contract
//
// class Program {
//   public:
//     // Programs must have a constructor that takes a ProgramResource
//     (compiled shader program)
//     // The ProgramFactory will use this to construct the object.
//     Program(ProgramResource programResource);
//
//     // Programs must declare a meta-structure called "Source" which provides
//     the shader source
//     // code. The ProgramFactory compiles the source code on behalf of the
//     program. struct Source {
//         // Returns the vertex shader source code.
//         static std::string_view getVertexSource();
//
//         // Returns the fragment shader source code.
//         static std::string_view getFragmentSource();
//     };
// }

// ProgramFactory compiles and instantiates program objects from their source
// code, or returns an existing program object if it is already in use. This
// allows decoupled systems that use the same shader program to not have to
// compile the shader source multiple times.
class ProgramFactory {
public:
  // Compiles and instantiates the specified program type, or returns an
  // existing instance of the program if it has already been compiled.
  template <typename ProgramType> std::shared_ptr<ProgramType> getProgram() {
    ProgramId programId = getProgramId<ProgramType>();
    auto &programWrapper = _programMap[programId];
    auto typedProgramWrapper =
        static_cast<ProgramWrapper<ProgramType> *>(programWrapper.get());
    if (typedProgramWrapper != nullptr) {
      auto program = typedProgramWrapper->getProgram();
      if (program != nullptr) {
        return program;
      }

      program = createProgram<ProgramType>();
      typedProgramWrapper->setProgram(program);

      return program;
    } else {
      auto newProgramWrapper = std::make_unique<ProgramWrapper<ProgramType>>();
      auto program = createProgram<ProgramType>();

      newProgramWrapper->setProgram(program);

      programWrapper = std::move(newProgramWrapper);

      return program;
    }
  }

private:
  template <typename ProgramType> std::shared_ptr<ProgramType> createProgram() {
    using ProgramSource = typename ProgramType::Source;

    ProgramResource programResource;
    programResource.buildWithSource(ProgramSource::getVertexSource(),
                                    ProgramSource::getFragmentSource());

    auto program = std::make_shared<ProgramType>(std::move(programResource));
    return program;
  }

  using ProgramId = void *;

  template <typename ProgramType> ProgramId getProgramId() {
    static int x = 0;
    return reinterpret_cast<ProgramId>(&x);
  }

  class IProgramWrapper {
  public:
    virtual ~IProgramWrapper() = default;
  };

  template <typename ProgramType>
  class ProgramWrapper : public IProgramWrapper {
  public:
    ProgramWrapper() {}

    std::shared_ptr<ProgramType> getProgram() const { return _program.lock(); }

    void setProgram(const std::shared_ptr<ProgramType> &program) {
      _program = program;
    }

  private:
    std::weak_ptr<ProgramType> _program;
  };

  std::unordered_map<ProgramId, std::unique_ptr<IProgramWrapper>> _programMap;
};
} // namespace rev
