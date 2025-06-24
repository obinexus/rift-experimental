#include "rift1/core/rift.h"
         "
// Fixed CLI main.c with proper argument parsing
#include <getopt.h>

static void print_usage(const char *program_name) {
  printf("🔤 RIFT Stage 0 - Tokenizer\n");
  printf("============================\n");
  printf("Usage: %s [options] <input_file>\n\n", program_name);
  printf("Options:\n");
  printf("  -h, --help              Show this help message\n");
  printf("  -v, --version           Show version information\n");
  printf("  -V, --verbose           Enable verbose output\n");
  printf(
      "  -o, --output <file>     Specify output file (default: <input>.0)\n");
  printf("  -c, --config <file>     Use custom configuration file\n");
  printf("  -d, --output-dir <dir>  Set output directory\n");
  printf("  --debug                 Enable debug mode\n");
  printf("  --stage <name>          Set stage name (default: rift.0)\n");
  printf("\n");
  printf("Examples:\n");
  printf("  %s input.rift                    # Basic tokenization\n",
         program_name);
  printf("  %s --verbose input.rift          # Verbose output\n", program_name);
  printf("  %s -o custom.ir input.rift       # Custom output file\n",
         program_name);
  printf("  %s --config .riftrc input.rift   # Custom config\n", program_name);
  printf("\n");
  printf("🏗️  RIFT Architecture: Structure IS the syntax!\n");
  printf("📚 Part of OBINexus governance-first compilation toolchain\n");
}

static void print_version(void) {
  printf("🔤 RIFT Stage 0 Tokenizer v1.0.0\n");
  printf("AEGIS Automaton Engine for Generative Interpretation & Syntax\n");
  printf("OBINexus Computing - Governance-First Compilation Architecture\n");
  printf("Built: %s %s\n", __DATE__, __TIME__);
  printf("\n");
  printf("🏗️  Architecture: Memory-first token triplets "
         "(memory→type→value)\n");
  printf("🔗 Toolchain: riftlang.exe → .so.a → rift.exe → gosilang\n");
  printf("📋 Stage: 0 (Tokenization with behavior-driven automaton)\n");
}

int main(int argc, char *argv[]) {
  // Configuration variables
  const char *input_file = NULL;
  const char *output_file = NULL;
  const char *config_file = ".riftrc";
  const char *output_dir = NULL;
  const char *stage_name = NULL;
  bool verbose = false;
  bool debug = false;
  bool show_help = false;
  bool show_version = false;

  // Long option definitions
  static struct option long_options[] = {
      {"help", no_argument, 0, 'h'},
      {"version", no_argument, 0, 'v'},
      {"verbose", no_argument, 0, 'V'},
      {"output", required_argument, 0, 'o'},
      {"config", required_argument, 0, 'c'},
      {"output-dir", required_argument, 0, 'd'},
      {"debug", no_argument, 0, 1000},
      {"stage", required_argument, 0, 1001},
      {0, 0, 0, 0}};

  // Parse command line arguments
  int option_index = 0;
  int c;

  while ((c = getopt_long(argc, argv, "hvVo:c:d:", long_options,
                          &option_index)) != -1) {
    switch (c) {
    case 'h':
      show_help = true;
      break;
    case 'v':
      show_version = true;
      break;
    case 'V':
      verbose = true;
      break;
    case 'o':
      output_file = optarg;
      break;
    case 'c':
      config_file = optarg;
      break;
    case 'd':
      output_dir = optarg;
      break;
    case 1000: // --debug
      debug = true;
      break;
    case 1001: // --stage
      stage_name = optarg;
      break;
    case '?':
      // getopt_long already printed an error message
      fprintf(stderr, "Try '%s --help' for more information.\n", argv[0]);
      return 1;
    default:
      fprintf(stderr, "❌ Unknown option: %c\n", c);
      return 1;
    }
  }

  // Handle help and version flags
  if (show_help) {
    print_usage(argv[0]);
    return 0;
  }

  if (show_version) {
    print_version();
    return 0;
  }

  // Check for input file
  if (optind >= argc) {
    fprintf(stderr, "❌ Error: No input file specified\n");
    fprintf(stderr, "Try '%s --help' for more information.\n", argv[0]);
    return 1;
  }

  input_file = argv[optind];

  // Create and configure RIFT config
  RiftConfig *config = rift_config_create();
  if (!config) {
    fprintf(stderr, "❌ Failed to create configuration\n");
    return 1;
  }

  // Load configuration file
  if (access(config_file, F_OK) == 0) {
    RiftResult result = rift_config_load(config, config_file);
    if (result != RIFT_SUCCESS && verbose) {
      printf("⚠️  Warning: Could not load config file %s: %s\n",
             config_file, rift_result_string(result));
    }
  }

  // Override config with command line options
  if (verbose)
    config->verbose = true;
  if (debug)
    config->debug_mode = true;
  if (output_dir) {
    free(config->output_dir);
    config->output_dir = strdup(output_dir);
  }
  if (stage_name) {
    free(config->stage_name);
    config->stage_name = strdup(stage_name);
  }

  // Generate output filename if not specified
  char generated_output[1024];
  if (!output_file) {
    snprintf(generated_output, sizeof(generated_output), "%s/%s.0",
             config->output_dir, basename(strdup(input_file)));
    output_file = generated_output;
  }

  // Ensure output directory exists
  char *output_dirname = strdup(output_file);
  char *dir = dirname(output_dirname);
  struct stat st = {0};
  if (stat(dir, &st) == -1) {
    if (mkdir(dir, 0755) != 0 && errno != EEXIST) {
      fprintf(stderr, "❌ Failed to create output directory: %s\n", dir);
      free(output_dirname);
      rift_config_destroy(config);
      return 1;
    }
  }
  free(output_dirname);

  // Print startup information
  if (verbose || debug) {
    printf("🔤 RIFT Stage 0 - Tokenization\n");
    printf("==============================\n");
    printf("📁 Input:  %s\n", input_file);
    printf("📁 Output: %s\n", output_file);
    printf("⚙️  Config: %s\n", config_file);
    printf("🏗️  Stage:  %s\n", config->stage_name);
    if (debug) {
      printf("🐛 Debug mode: enabled\n");
    }
    printf("\n");
  }

  // Process the file
  printf("🚀 Processing: %s\n", input_file);
  RiftResult result = rift_process_file(input_file, output_file, config);

  if (result == RIFT_SUCCESS) {
    printf("✅ Stage 0 processing complete\n");
    if (verbose) {
      printf("📄 Output saved: %s\n", output_file);
    }
  } else {
    printf("❌ Processing failed: %s\n", rift_result_string(result));
  }

  rift_config_destroy(config);
  return (result == RIFT_SUCCESS) ? 0 : 1;
}