#pragma once

#include <MetaNN/layers/compose/compose_kernel.h>
#include <MetaNN/layers/elementary/weight_layer.h>
#include <MetaNN/layers/elementary/add_layer.h>
#include <MetaNN/layers/elementary/sigmoid_layer.h>
#include <MetaNN/layers/elementary/tanh_layer.h>
#include <MetaNN/layers/elementary/element_mul_layer.h>
#include <MetaNN/layers/elementary/interpolate_layer.h>
#include <MetaNN/layers/facilities/common_io.h>

namespace MetaNN
{
using GruInput = VarTypeDict<RnnLayerHiddenBefore,
                              LayerIO>;

template <typename TPolicies> class GruStep;

template <>
struct Sublayerof<GruStep>
{
    struct Wz; struct Uz; struct Add_z; struct Act_z; 
    struct Wr; struct Ur; struct Add_r; struct Act_r;
    struct W; struct U; struct Elem; struct Add; struct Act_Hat;
    struct Interpolate;
};

namespace NSGruStep
{
using Wz = typename Sublayerof<GruStep>::Wz;
using Uz = typename Sublayerof<GruStep>::Uz;
using Add_z = typename Sublayerof<GruStep>::Add_z;
using Act_z = typename Sublayerof<GruStep>::Act_z;

using Wr = typename Sublayerof<GruStep>::Wr;
using Ur = typename Sublayerof<GruStep>::Ur;
using Add_r = typename Sublayerof<GruStep>::Add_r;
using Act_r = typename Sublayerof<GruStep>::Act_r;

using W = typename Sublayerof<GruStep>::W;
using U = typename Sublayerof<GruStep>::U;
using Elem = typename Sublayerof<GruStep>::Elem;
using Add = typename Sublayerof<GruStep>::Add;
using Act_Hat = typename Sublayerof<GruStep>::Act_Hat;

using Interpolate = typename Sublayerof<GruStep>::Interpolate;


using Topology = ComposeTopology<
        Sublayer<Wz, WeightLayer>,
        Sublayer<Uz, WeightLayer>,
        Sublayer<Add_z, AddLayer>,
        Sublayer<Act_z, SigmoidLayer>,
        InConnect<LayerIO, Wz, LayerIO>,
        InConnect<RnnLayerHiddenBefore, Uz, LayerIO>,
        InternalConnect<Wz, LayerIO, Add_z, AddLayerIn1>,
        InternalConnect<Uz, LayerIO, Add_z, AddLayerIn2>,
        InternalConnect<Add_z, LayerIO, Act_z, LayerIO>,
        
        Sublayer<Wr, WeightLayer>,
        Sublayer<Ur, WeightLayer>,
        Sublayer<Add_r, AddLayer>,
        Sublayer<Act_r, SigmoidLayer>,
        InConnect<LayerIO, Wr, LayerIO>,
        InConnect<RnnLayerHiddenBefore, Ur, LayerIO>,
        InternalConnect<Wr, LayerIO, Add_r, AddLayerIn1>,
        InternalConnect<Ur, LayerIO, Add_r, AddLayerIn2>,
        InternalConnect<Add_r, LayerIO, Act_r, LayerIO>,
        
        Sublayer<Elem, ElementMulLayer>,
        InConnect<RnnLayerHiddenBefore, Elem, ElementMulLayerIn1>,
        InternalConnect<Act_r, LayerIO, Elem, ElementMulLayerIn2>,
        Sublayer<U, WeightLayer>,
        InternalConnect<Elem, LayerIO, U, LayerIO>,
        Sublayer<W, WeightLayer>,
        InConnect<LayerIO, W, LayerIO>,
        Sublayer<Add, AddLayer>,
        InternalConnect<U, LayerIO, Add, AddLayerIn1>,
        InternalConnect<W, LayerIO, Add, AddLayerIn2>,
        Sublayer<Act_Hat, TanhLayer>,
        InternalConnect<Add, LayerIO, Act_Hat, LayerIO>,
        
        Sublayer<Interpolate, InterpolateLayer>,
        InternalConnect<Act_Hat, LayerIO, Interpolate, InterpolateLayerWeight1>,
        InConnect<RnnLayerHiddenBefore, Interpolate, InterpolateLayerWeight2>,
        InternalConnect<Act_z, LayerIO, Interpolate, InterpolateLayerLambda>,

        OutConnect<Interpolate, LayerIO, LayerIO>>;

template <typename TPolicies>
using Base = ComposeKernel<GruInput, LayerIO, TPolicies, Topology>;
}

template <typename TPolicies>
class GruStep : public NSGruStep::Base<TPolicies>
{
    static_assert(IsPolicyContainer<TPolicies>, "TPolicies is not a policy container.");

    using TBase = NSGruStep::Base<TPolicies>;

public:
    GruStep(const std::string& p_name, size_t p_inLen, size_t p_outLen)
        : TBase(TBase::CreateSubLayers()
                        .template Set<NSGruStep::Wz>(p_name + "-Wz", p_inLen, p_outLen)
                        .template Set<NSGruStep::Uz>(p_name + "-Uz", p_outLen, p_outLen)
                        .template Set<NSGruStep::Wr>(p_name + "-Wr", p_inLen, p_outLen)
                        .template Set<NSGruStep::Ur>(p_name + "-Ur", p_outLen, p_outLen)
                        .template Set<NSGruStep::W>(p_name + "-W", p_inLen, p_outLen)
                        .template Set<NSGruStep::U>(p_name + "-U", p_outLen, p_outLen))
        {}
};
}
