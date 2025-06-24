// Driver binding logic for functor-style data flow
#ifdef __EMSCRIPTEN__
#include <emscripten/bind.h>

template<typename Functor>
class WASMFunctorWrapper {
    Functor functor;
    
public:
    explicit WASMFunctorWrapper(Functor f) : functor(f) {}
    
    emscripten::val operator()(emscripten::val input) {
        auto result = functor(input.as<typename Functor::input_type>());
        return emscripten::val(result);
    }
};

// Data flow pipeline binding
class DataFlowPipeline {
    std::vector<WASMFunctorWrapper<std::function<void*>>> stages;
    
public:
    template<typename InputType, typename OutputType>
    void addStage(std::function<OutputType(InputType)> processor) {
        stages.push_back(WASMFunctorWrapper(processor));
    }
    
    emscripten::val process(emscripten::val input) {
        emscripten::val current = input;
        for (auto& stage : stages) {
            current = stage(current);
        }
        return current;
    }
};

EMSCRIPTEN_BINDINGS(rift_bridge_bindings) {
    emscripten::class_<DataFlowPipeline>("DataFlowPipeline")
        .constructor<>()
        .function("addStage", &DataFlowPipeline::addStage)
        .function("process", &DataFlowPipeline::process);
}
#endif
