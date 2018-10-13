#pragma once

#include "rev/gl/ProgramResource.h"

#include <memory>
#include <unordered_map>

namespace rev
{
class ProgramFactory
{
  public:
    template <typename ProgramType>
    std::shared_ptr<ProgramType> getProgram()
    {
        ProgramId programId = getProgramId<ProgramType>();
        auto &programWrapper = _programMap[programId]);
        auto typedProgramWrapper = static_cast<ProgramWrapper<ProgramType>*>(programWrapper.get());
        if(typedProgramWrapper != nullptr)
        {
            auto program = typedProgramWrapper->getProgram();
            if(program != nullptr)
            {
                return program;
            }

            program = createProgram<ProgramType>();
            typedProgramWrapper->setProgram(program);

            return program;
        }
        else
        {
            auto newProgramWrapper = std::make_unique<ProgramWrapper<ProgramType>>();
            auto program = createProgram<ProgramType>();

            newProgramWrapper->setProgram(program);

            programWrapper = std::move(newProgramWrapper);

            return program;
        }
    }

  private:
    template <typename ProgramType>
    std::shared_ptr<ProgramType> createProgram()
    {
        using ProgramSource = typename ProgramType::Source;

        ProgramResource programResource;
        programResource.buildWithSource(ProgramSource::getVertexSource(), ProgramSource::getFragmentSource());

        auto program = std::make_shared<ProgramType>(std::move(programResource));
        return program;
    }

    using ProgramId = void*;

    template <typename ProgramType>
    ProgramId getProgramId()
    {
        static int x = 0;
        return reinterpret_cast<ProgramId>(&x);
    }

    class IProgramWrapper
    {
    public:
        virtual ~IProgramWrapper() = default;
    };

    template <typename ProgramType>
    class ProgramWrapper : public IProgramWrapper
    {
    public:
        ProgramWrapper()
        {
        }

        std::shared_ptr<ProgramType> getProgram() const
        {
            return _program.lock();
        }

        void setProgram(const std::shared_ptr<ProgramType> &program)
        {
            _program = program;
        }

    private:
        std::weak_ptr<ProgramType> _program;
    };

    std::unordered_map<ProgramId, std::unique_ptr<IProgramWrapper>> _programMap;
};
} // namespace rev
