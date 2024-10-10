#include "Synthesizer.h"
#include <loris.h>

#include <LinearEnvelope.h>
#include <PartialList.h>
#include <SpectralPeaks.h>

#include <pybind11/numpy.h> // Include the numpy header
#include <pybind11/pybind11.h>
#include <pybind11/stl.h> // For iterator bindings

namespace py = pybind11;

// Wrap for analyze function
PartialList *wrap_analyze(const py::array_t<double> &samples, double sr) {
    py::buffer_info buf_info = samples.request();
    const double *buffer = static_cast<const double *>(buf_info.ptr);
    unsigned int bufferSize = static_cast<unsigned int>(buf_info.size);
    PartialList *partials = createPartialList();
    analyze(buffer, bufferSize, sr, partials);
    return partials;
}

// ─────────────────────────────────────
PartialList *wrap_importSdif(const char *filename) {
    PartialList *sdif = createPartialList();
    importSdif(filename, sdif);
    return sdif;
}

// ╭─────────────────────────────────────╮
// │          Create _lorisbind          │
// ╰─────────────────────────────────────╯
PYBIND11_MODULE(_lorisbind, m) {
    // Peaks
    // py::class_<Loris::SpectralPeak>(m, "SpectralPeak").def(py::init<>());

    // Partials
    py::class_<PartialList>(m, "PartialList")
        .def(py::init<>())
        .def("__str__",
             [](const PartialList &p) {
                 std::string str = "PartialList(";
                 str += std::to_string(p.size());
                 str += ")";
                 return str;
             })
        .def(
            "__iter__", [](PartialList &p) { return py::make_iterator(p.begin(), p.end()); },
            py::keep_alive<0, 1>())
        .def("push_back", &PartialList::push_back)
        .def("synthesize", [](PartialList &p, double srate) {
            std::vector<double> vec;
            Loris::Synthesizer synth(srate, vec);
            synth.synthesize(p.begin(), p.end());
            return pybind11::array(vec.size(), vec.data());
        });

    py::class_<Partial>(m, "Partial")
        .def(py::init<>())
        .def("__str__",
             [](const Partial &p) {
                 std::string str = "Partial(";
                 str += std::to_string(p.size());
                 str += ")";
                 return str;
             })
        .def("__repr__",
             [](const Partial &p) {
                 std::string str = "<Partial(";
                 str += std::to_string(p.size());
                 str += " points)>";
                 return str;
             })
        .def(
            "__iter__", [](Partial &p) { return py::make_iterator(p.begin(), p.end()); },
            py::keep_alive<0, 1>())
        .def_property("duration", &Partial::duration, nullptr)
        .def("startTime", &Partial::startTime)
        // par
        .def_readonly_static("ShortestSafeFadeTime", &Partial::ShortestSafeFadeTime)

        // read
        .def("frequencyAt", &Partial::frequencyAt)
        .def("amplitudeAt", &Partial::amplitudeAt)
        .def("phaseAt", &Partial::phaseAt)
        .def("bandwidthAt", &Partial::bandwidthAt)
        .def("parametersAt", &Partial::parametersAt)

        // change
        .def("insert", &Partial::insert);

    py::class_<Loris::Partial_Iterator>(m, "Partial_Iterator")
        .def("__next__", [](Loris::Partial_Iterator &it, Partial &partial) {
            if (it == partial.end())
                throw py::stop_iteration();
            return *it++;
        });

    py::class_<Breakpoint>(m, "Breakpoint")
        .def(py::init<>())
        .def(py::init<double, double, double, double>(), py::arg("frequency"), py::arg("amplitude"),
             py::arg("bandwidth"), py::arg("phase") = 0.0)
        .def("__str__",
             [](const Breakpoint &p) {
                 std::string str = "Breakpoint(";
                 str += "freq: " + std::to_string(p.frequency()) + ", ";
                 str += "amp: " + std::to_string(p.amplitude()) + ", ";
                 str += "phase: " + std::to_string(p.phase()) + ", ";
                 str += "band: " + std::to_string(p.bandwidth());
                 str += ")";
                 return str;
             })
        .def_property("frequency", &Breakpoint::frequency, &Breakpoint::setFrequency)
        .def_property("amplitude", &Breakpoint::amplitude, &Breakpoint::setAmplitude)
        .def_property("phase", &Breakpoint::phase, &Breakpoint::setPhase)
        .def_property("bandwidth", &Breakpoint::bandwidth, &Breakpoint::setBandwidth);

    py::class_<LinearEnvelope>(m, "LinearEnvelope")
        .def(py::init([]() { return createLinearEnvelope(); }),
             py::return_value_policy::take_ownership)
        .def("__str__",
             [](const LinearEnvelope &p) {
                 std::string str = "LinearEnvelope(";
                 str += std::to_string(p.size());
                 str += ")";
                 return str;
             })
        .def("valueAt", &LinearEnvelope::valueAt)
        .def("insertBreakpoint", &LinearEnvelope::insertBreakpoint);

    py::class_<Loris::Synthesizer::Parameters>(m, "Parameters")
        .def(py::init<>())
        .def_readwrite("fadeTime", &Loris::Synthesizer::Parameters::fadeTime)
        .def_readwrite("sampleRate", &Loris::Synthesizer::Parameters::sampleRate)
        .def_readwrite("filter", &Loris::Synthesizer::Parameters::filter);

    py::class_<Loris::Synthesizer>(m, "Synthesizer")
        .def(py::init<std::vector<double> &>())
        .def(py::init<Loris::Synthesizer::Parameters, std::vector<double> &>())
        .def(py::init<double, std::vector<double> &>())
        .def(py::init<double, std::vector<double> &, double>())
        .def("samples", (const std::vector<double> &(Loris::Synthesizer::*)() const) &
                            Loris::Synthesizer::samples)
        .def("samples",
             (std::vector<double> & (Loris::Synthesizer::*)()) & Loris::Synthesizer::samples)
        .def("fadeTime", &Loris::Synthesizer::fadeTime)
        .def("sampleRate", &Loris::Synthesizer::sampleRate)
        .def("setFadeTime", &Loris::Synthesizer::setFadeTime)
        .def("setSampleRate", &Loris::Synthesizer::setSampleRate)
        .def("filter", &Loris::Synthesizer::filter)
        .def_static("DefaultParameters", &Loris::Synthesizer::DefaultParameters,
                    py::return_value_policy::reference)
        .def_static("SetDefaultParameters", &Loris::Synthesizer::SetDefaultParameters)
        .def_static("IsValidParameters", &Loris::Synthesizer::IsValidParameters);

    m.def("analyzer_configure", &analyzer_configure);
    m.def("analyzer_setFreqDrift", &analyzer_setFreqDrift);
    m.def("analyze", &wrap_analyze);
    m.def("importSdif", &importSdif, py::return_value_policy::take_ownership);

    // Morph
    m.def("createF0Estimate", &createF0Estimate, py::return_value_policy::automatic);
    m.def("channelize", &channelize);
    m.def("dilate", &dilate);
    m.def("distill", &distill);
    m.def("sift", &sift);

    m.def("createLinearEnvelope", &createLinearEnvelope, py::return_value_policy::take_ownership);
    m.def("morph", &morph);
    m.def("synthesize", &synthesize);

    // m.def("createF0Estimate", &createF0Estimate, py::return_value_policy::take_ownership);
    // m.def("exportSdif", &exportSdif);
    // m.def("createF0Estimate", &createF0Estimate);
    // m.def("channelize", &channelize);
    // m.def("destroyLinearEnvelope", &destroyLinearEnvelope);
    // m.def("linearEnvelope_insertBreakpoint", &linearEnvelope_insertBreakpoint);
    // m.def("shiftPitch", &shiftPitch);
    // m.def("exportAiff", &exportAiff);
};
