// This file is part of OpenCV project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution and at http://opencv.org/license.html.
//
// Copyright (C) 2020 Intel Corporation

#ifndef OPENCV_GAPI_INFER_ONNX_HPP
#define OPENCV_GAPI_INFER_ONNX_HPP

#include <unordered_map>
#include <string>
#include <array>
#include <tuple> // tuple, tuple_size

#include <opencv2/gapi/opencv_includes.hpp>
#include <opencv2/gapi/util/any.hpp>

#include <opencv2/core/cvdef.h>     // GAPI_EXPORTS
#include <opencv2/gapi/gkernel.hpp> // GKernelPackage

namespace cv {
namespace gapi {
namespace onnx {

GAPI_EXPORTS cv::gapi::GBackend backend();

enum class TraitAs: int {
    TENSOR, //!< G-API traits an associated cv::Mat as a raw tensor
            // and passes dimensions as-is
    IMAGE   //!< G-API traits an associated cv::Mat as an image so
            // creates an "image" blob (NCHW/NHWC, etc)
};

using PostProc = std::function<void(const std::unordered_map<std::string, cv::Mat> &,
                                          std::unordered_map<std::string, cv::Mat> &)>;


namespace detail {
struct ParamDesc {
    std::string model_path;

    // NB: nun_* may differ from topology's real input/output port numbers
    // (e.g. topology's partial execution)
    std::size_t num_in;  // How many inputs are defined in the operation
    std::size_t num_out; // How many outputs are defined in the operation

    // NB: Here order follows the `Net` API
    std::vector<std::string> input_names;
    std::vector<std::string> output_names;

    using ConstInput = std::pair<cv::Mat, TraitAs>;
    std::unordered_map<std::string, ConstInput> const_inputs;

    std::vector<cv::Scalar> mean;
    std::vector<cv::Scalar> stdev;

    std::vector<cv::GMatDesc> out_metas;
    PostProc custom_post_proc;

    std::vector<bool> normalize;

    std::vector<std::string> names_to_remap;
};
} // namespace detail

template<typename Net>
struct PortCfg {
    using In = std::array
        < std::string
        , std::tuple_size<typename Net::InArgs>::value >;
    using Out = std::array
        < std::string
        , std::tuple_size<typename Net::OutArgs>::value >;
    using NormCoefs = std::array
        < cv::Scalar
        , std::tuple_size<typename Net::InArgs>::value >;
    using Normalize = std::array
        < bool
        , std::tuple_size<typename Net::InArgs>::value >;
};

template<typename Net> class Params {
public:
    Params(const std::string &model) {
        desc.model_path = model;
        desc.num_in  = std::tuple_size<typename Net::InArgs>::value;
        desc.num_out = std::tuple_size<typename Net::OutArgs>::value;
    };

    // BEGIN(G-API's network parametrization API)
    GBackend      backend() const { return cv::gapi::onnx::backend(); }
    std::string   tag()     const { return Net::tag(); }
    cv::util::any params()  const { return { desc }; }
    // END(G-API's network parametrization API)

    Params<Net>& cfgInputLayers(const typename PortCfg<Net>::In &ll) {
        desc.input_names.assign(ll.begin(), ll.end());
        return *this;
    }

    Params<Net>& cfgOutputLayers(const typename PortCfg<Net>::Out &ll) {
        desc.output_names.assign(ll.begin(), ll.end());
        return *this;
    }

    Params<Net>& constInput(const std::string &layer_name,
                            const cv::Mat &data,
                            TraitAs hint = TraitAs::TENSOR) {
        desc.const_inputs[layer_name] = {data, hint};
        return *this;
    }

    Params<Net>& cfgMeanStd(const typename PortCfg<Net>::NormCoefs &m,
                            const typename PortCfg<Net>::NormCoefs &s) {
        desc.mean.assign(m.begin(), m.end());
        desc.stdev.assign(s.begin(), s.end());
        return *this;
    }

    /** @brief Configures graph output and sets the post processing function from user.

    The function is used for the case of infer of networks with dynamic outputs.
    Since these networks haven't known output parameters needs provide them for
    construction of output of graph.
    The function provides meta information of outputs and post processing function.
    Post processing function is used for copy information from ONNX infer's result
    to output of graph which is allocated by out meta information.

    @param out_metas out meta information.
    @param pp post processing function, which has two parameters. First is onnx
    result, second is graph output. Both parameters is std::map that contain pair of
    layer's name and cv::Mat.
    @return reference to object of class Params.
    */
    Params<Net>& cfgPostProc(const std::vector<cv::GMatDesc> &out_metas,
                             const PostProc &pp) {
        desc.out_metas        = out_metas;
        desc.custom_post_proc = pp;
        return *this;
    }

    /** @overload
    The function has rvalue parameters.
    */
    Params<Net>& cfgPostProc(std::vector<cv::GMatDesc> &&out_metas,
                             PostProc &&pp) {
        desc.out_metas        = std::move(out_metas);
        desc.custom_post_proc = std::move(pp);
        return *this;
    }

    /** @overload
    The function has additional parameter names_to_remap. This parameter provides
    information about output layers which will be used for infer and in post
    processing function.

    @param out_metas out meta information.
    @param pp post processing function.
    @param names_to_remap contains names of output layers. CNN's infer will be done on these layers.
    Infer's result will be processed in post processing function using these names.
    @return reference to object of class Params.
    */
    Params<Net>& cfgPostProc(const std::vector<cv::GMatDesc> &out_metas,
                             const PostProc &pp,
                             const std::vector<std::string> &names_to_remap) {
        desc.out_metas        = out_metas;
        desc.custom_post_proc = pp;
        desc.names_to_remap   = names_to_remap;
        return *this;
    }

    /** @overload
    The function has rvalue parameters.
    */
    Params<Net>& cfgPostProc(std::vector<cv::GMatDesc> &&out_metas,
                             PostProc &&pp,
                             std::vector<std::string> &&names_to_remap) {
        desc.out_metas        = std::move(out_metas);
        desc.custom_post_proc = std::move(pp);
        desc.names_to_remap   = std::move(names_to_remap);
        return *this;
    }

    Params<Net>& cfgNormalize(const typename PortCfg<Net>::Normalize &n) {
        desc.normalize.assign(n.begin(), n.end());
        return *this;
    }

protected:
    detail::ParamDesc desc;
};

} // namespace onnx
} // namespace gapi
} // namespace cv

#endif // OPENCV_GAPI_INFER_HPP
