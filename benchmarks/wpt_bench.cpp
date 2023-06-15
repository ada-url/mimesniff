#include <cstdlib>
#include <sstream>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <benchmark/benchmark.h>

#include "performancecounters/event_counter.h"
#include "mimesniff.h"
#include "simdjson.h"

using namespace simdjson;

event_collector collector;
size_t N = 1000;

bool file_exists(const char *filename) {
  namespace fs = std::filesystem;
  std::filesystem::path f{filename};
  if (std::filesystem::exists(filename)) {
    return true;
  } else {
    return false;
  }
}

double mime_examples_bytes{};

std::vector<std::pair<std::string, std::string>> mime_examples;

size_t init_data(const char *source) {
  ondemand::parser parser;
  std::vector<std::pair<std::string, std::string>> answer;

  if (!file_exists(source)) {
    return 0;
  }
  padded_string json = padded_string::load(source);
  ondemand::document doc = parser.iterate(json);
  for (auto element : doc.get_array()) {
    if (element.type() == ondemand::json_type::object) {
      std::string_view input;
      if (element["input"].get_string(true).get(input) != simdjson::SUCCESS) {
        printf("missing input.\n");
      }
      std::string_view base;
      if (element["base"].get_string(true).get(base) != simdjson::SUCCESS) {
      }
      mime_examples.push_back({std::string(input), std::string(base)});
      mime_examples_bytes += input.size() + base.size();
    }
  }
  return mime_examples.size();
}

static void BasicBench(benchmark::State &state) {
  // volatile to prevent optimizations.
  volatile size_t mime_size = 0;
  for (auto _ : state) {
    for (const std::pair<std::string, std::string> &mime_strings :
         mime_examples) {
      auto mime = ada::mimesniff::parse_mime_type(mime_strings.first);
      if (mime) {
        mime_size += mime->serialized().size();
        ;
      }
    }
  }
  if (collector.has_events()) {
    event_aggregate aggregate{};
    for (size_t i = 0; i < N; i++) {
      std::atomic_thread_fence(std::memory_order_acquire);
      collector.start();
      for (const std::pair<std::string, std::string> &mime_strings :
           mime_examples) {
        auto mime = ada::mimesniff::parse_mime_type(mime_strings.first);
        if (mime) {
          mime_size += mime->serialized().size();
          ;
        }
      }
      std::atomic_thread_fence(std::memory_order_release);
      event_count allocate_count = collector.end();
      aggregate << allocate_count;
    }
    state.counters["cycles/mime"] =
        aggregate.best.cycles() / std::size(mime_examples);
    state.counters["instructions/mime"] =
        aggregate.best.instructions() / std::size(mime_examples);
    state.counters["instructions/cycle"] =
        aggregate.best.instructions() / aggregate.best.cycles();
    state.counters["instructions/byte"] =
        aggregate.best.instructions() / mime_examples_bytes;
    state.counters["instructions/ns"] =
        aggregate.best.instructions() / aggregate.best.elapsed_ns();
    state.counters["GHz"] =
        aggregate.best.cycles() / aggregate.best.elapsed_ns();
    state.counters["ns/mime"] =
        aggregate.best.elapsed_ns() / std::size(mime_examples);
    state.counters["cycle/byte"] =
        aggregate.best.cycles() / mime_examples_bytes;
  }
  state.counters["time/byte"] = benchmark::Counter(
      mime_examples_bytes, benchmark::Counter::kIsIterationInvariantRate |
                               benchmark::Counter::kInvert);
  state.counters["time/mime"] =
      benchmark::Counter(double(std::size(mime_examples)),
                         benchmark::Counter::kIsIterationInvariantRate |
                             benchmark::Counter::kInvert);
  state.counters["speed"] = benchmark::Counter(
      mime_examples_bytes, benchmark::Counter::kIsIterationInvariantRate);
  state.counters["mime/s"] =
      benchmark::Counter(double(std::size(mime_examples)),
                         benchmark::Counter::kIsIterationInvariantRate);
}
BENCHMARK(BasicBench);

int main(int argc, char **argv) {
  if (argc == 1 || !init_data(argv[1])) {
    std::cout << "pass the path to the file wpt/generated-mime-types.json as a "
                 "parameter."
              << std::endl;
    std::cout
        << "E.g., './build/benchmarks/wpt_bench wpt/generated-mime-types.json'"
        << std::endl;
    return EXIT_SUCCESS;
  }
#if (__APPLE__ && __aarch64__) || defined(__linux__)
  if (!collector.has_events()) {
    benchmark::AddCustomContext("performance counters",
                                "No privileged access (sudo may help).");
  }
#else
  if (!collector.has_events()) {
    benchmark::AddCustomContext("performance counters", "Unsupported system.");
  }
#endif

  if (collector.has_events()) {
    benchmark::AddCustomContext("performance counters", "Enabled");
  }
  benchmark::Initialize(&argc, argv);
  benchmark::RunSpecifiedBenchmarks();
  benchmark::Shutdown();
}
