#pragma once

#include <MetaNN/data/facilities/traits.h>
#include <MetaNN/evaluate/facilities/eval_plan.h>
#include <MetaNN/evaluate/facilities/eval_unit.h>
#include <MetaNN/operators/activations/tags.h>
#include <MetaNN/operators/facilities/tail_calculator.h>
#include <cassert>
#include <type_traits>

namespace MetaNN
{
namespace OperReLU::NSCaseGen
{
template <typename TInputHandle, typename TOutputHandle>
class EvalUnit : public BaseEvalUnit<DeviceTypeFromHandle<TOutputHandle>>
{
public:
    template <typename TAuxParams>
    EvalUnit(TInputHandle oriHandle, TOutputHandle outputHandle, const TAuxParams&)
        : m_inputHandle(std::move(oriHandle))
        , m_outputHandle(std::move(outputHandle))
    {}
    
    void Eval() override final
    {
        const auto& in = m_inputHandle.Data();
        m_outputHandle.Allocate(in.Shape());
        auto& out = m_outputHandle.MutableData();
        
        using ElementType = ElementTypePicker<decltype(out)>;
        
        const size_t count = in.Shape().Count();
        assert(count == out.Shape().Count());
        
        auto low_in = LowerAccess(in);
        ElementType* mem_in = low_in.MutableRawMemory();

        auto low_out = LowerAccess(out);
        ElementType* mem_out = low_out.MutableRawMemory();
                
        static_assert(std::is_same_v<DeviceTypeFromHandle<TOutputHandle>, DeviceTags::CPU>, "Currently only CPU is supported");
        
        const ElementType zero{};
        for (size_t i = 0; i < count; ++i)
        {
            mem_out[i] = (mem_in[i] > zero) ? mem_in[i] : zero;
        }
        m_outputHandle.SetEval();
    }
    
private:
    const TInputHandle m_inputHandle;
    TOutputHandle m_outputHandle;
};
}

template <>
struct OperSeq_<OpTags::ReLU>
{
    using type = OperSeqContainer<TailCalculator<OperReLU::NSCaseGen::EvalUnit>>;
};

template <typename TP,
          typename = std::enable_if_t<IsValidOper<OpTags::ReLU, TP>>>
auto ReLU(TP&& p_m)
{
    using rawM = RemConstRef<TP>;
    using ResType = Operator<OpTags::ReLU, rawM>;
    return ResType(std::forward<TP>(p_m));
}
}