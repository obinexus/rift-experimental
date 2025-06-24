#include "rift1/core/rift.h""
// RIFT Configuration CLI Main
#include "rift1/core/rift.h""

// Configuration CLI main function
int main(int argc, char* argv[]) {
    printf("🔧 RIFT Advanced Configuration Utility\n");
    printf("======================================\n");
    
    if (argc < 2) {
        printf("Usage: %s <command> [config_file]\n", argv[0]);
        printf("Commands:\n");
        printf("  show      - Show current configuration\n");
        printf("  validate  - Validate configuration\n");
        printf("  demo      - Run configuration demo\n");
        return 1;
    }
    
    const char* command = argv[1];
    const char* config_file = (argc > 2) ? argv[2] : ".riftrc";
    
    // Create configuration
    RiftAdvancedConfig* config = rift_advanced_config_create();
    if (!config) {
        printf("❌ Failed to create configuration\n");
        return 1;
    }
    
    // Load configuration file
    rift_advanced_config_load(config, config_file);
    
    // Execute command
    if (strcmp(command, "show") == 0) {
        printf("\n📋 RIFT Configuration Status:\n");
        rift_demo_pipeline_config(config);
        
    } else if (strcmp(command, "validate") == 0) {
        printf("\n🔍 Validating configuration...\n");
        if (rift_advanced_config_validate(config) == 0) {
            printf("✅ Configuration is valid\n");
        } else {
            printf("❌ Configuration validation failed\n");
        }
        
    } else if (strcmp(command, "demo") == 0) {
        printf("\n🎮 Running configuration demo...\n");
        rift_demo_pipeline_config(config);
        
    } else {
        printf("❌ Unknown command: %s\n", command);
        rift_advanced_config_destroy(config);
        return 1;
    }
    
    rift_advanced_config_destroy(config);
    return 0;
}
